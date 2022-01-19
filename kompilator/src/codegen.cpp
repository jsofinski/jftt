#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codegen.h"
#include "header.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"

typedef int i64;

struct VMLine {
    VMInstruction op;
    i64 arg; // 0-7 albo 0-2^63
    char* comment;
};

struct VMLine instructions[10000];
int instruction_pointer = 0;

void push_line(VMInstruction op, int arg, char* comment) {
    instructions[instruction_pointer].op = op;
    instructions[instruction_pointer].arg = arg;
    instructions[instruction_pointer].comment = comment;
    instruction_pointer++;
}
void push_line(VMInstruction op, int arg) {
    push_line(op, arg, NULL);
}

char *names[] = {"GET", "PUT", "LOAD", "STORE", "ADD", "SUB", "SHIFT", "SWAP", "RESET", "INC", "DEC", "JUMP", "JPOS", "JZERO", "JNEG", "HALT"};

void print_lines() {
    for (int i = 0; i < instruction_pointer; i++) {
        if (instructions[i].op == GET || 
                instructions[i].op == PUT ||
                instructions[i].op == HALT) {
            printf("%s", names[instructions[i].op]);
        } else if (instructions[i].op == JZERO || 
                instructions[i].op == JPOS ||
                instructions[i].op == JNEG ||
                instructions[i].op == JUMP) {
            printf("%s %d", names[instructions[i].op], instructions[i].arg);
        } else {
            printf("%s %c", names[instructions[i].op], instructions[i].arg);
        }

        if(instructions[i].comment != NULL) {
            printf("\t (%s)", instructions[i].comment);
        }
        printf("\n");
    }
}

// wynik w Ra, psuje Rh
void set_register_a_to_value(i64 value) {
    char reg = 'a';
    if (value < 6 && value >= 0) {
        push_line(RESET, reg);
        for(int i = 0; i < value; i++) {
            push_line(INC, reg);
        }
        return;
    }
    char helper = 'h';
    int gtz = value > 0;
    if (!gtz) {
        value = -value;
    }
    push_line(RESET, helper);
    push_line(INC, helper); // do shiftowania a o 1
    push_line(RESET, reg);
    int check = 16;
    int found = 0;
    while (check != 0) {
        if (found) {
            push_line(SHIFT, helper); // Ra = Ra << 1
        }
        if (value & check) {
            push_line(INC, reg);
            found = 1;
        }
        check /= 2;
    }
    if (!gtz) {
        push_line(SWAP, helper);
        push_line(RESET, reg);
        push_line(SUB, helper);
    }
}

int jump_stack[100] = {0};    
int jump_stack_pointer = 0;

void prepare_jump(VMInstruction op) {
    int q = instruction_pointer;
    push_line(op, -2137);
    jump_stack[jump_stack_pointer++] = q;
}

void backfill_jump() {
    int line = jump_stack[--jump_stack_pointer];
    int last_line = instruction_pointer;
    int diff = last_line - line;
    instructions[line].arg = diff;
}

void backfill_else_jump() {
    // swap top two elements of jump stack
    int tmp = jump_stack[jump_stack_pointer - 1];
    jump_stack[jump_stack_pointer - 1] = jump_stack[jump_stack_pointer - 2];
    jump_stack[jump_stack_pointer - 2] = tmp;

    backfill_jump();
}

void comment(char* comment) {
    push_line(JUMP, 1, comment);
}

void push_jumps(int jmp) {
    switch (jmp) {
        case cEQ:
            prepare_jump(JPOS);
            prepare_jump(JNEG);
            break;
        case cNEQ:
            prepare_jump(JZERO);
            break;
        case cGE:
            prepare_jump(JNEG);
            prepare_jump(JZERO);
            break;
        case cLE:
            prepare_jump(JPOS);
            prepare_jump(JZERO);
            break;
        case cGEQ:
            prepare_jump(JNEG);
            break;
        case cLEQ:
            prepare_jump(JPOS);
            break;
    }
}

void Node::codegen() {}

Program::Program(Node* commands) : commands(commands) {};
void Program::codegen() {
    commands->codegen();
    push_line(HALT, 0);
}

Commands::Commands(Node* commands, Node* command)
: commands{commands},
    command{command}
{};
void Commands::codegen() {
    commands->codegen();
    command->codegen();
}

Assign::Assign(Identifier* id, Node* expression)
: id{id},
    expression{expression}
{};

void Assign::codegen() {
    set_register_a_to_value(id->id); // Ra = &identifier
    push_line(SWAP, 'd'); // Rd <= &identifier
    expression->codegen(); // Ra <= value
    push_line(STORE, 'd'); // P(Rb) <= Ra
}

Expression::Expression(Node* bvalue, int symbol, Node* cvalue)
: bvalue{bvalue},
    symbol{symbol},
    cvalue{cvalue}
{};

void Expression::codegen() {
    switch(symbol) {
        case ePLUS:
            cvalue->codegen();
            push_line(SWAP, 'b');
            bvalue->codegen();
            push_line(ADD, 'b');
        break;
        case eMINUS:
            cvalue->codegen();
            push_line(SWAP, 'b');
            bvalue->codegen();
            push_line(SUB, 'b');
        break;
        case eTIMES:
            push_line(RESET, 'a');  
        break;
        case eDIV:
            push_line(RESET, 'a');  
        break;
        case eMOD:
            push_line(RESET, 'a');  
        break;
    }      
}

Condition::Condition(Node* bvalue, int symbol, Node* cvalue)
: bvalue{bvalue},
    symbol{symbol},
    cvalue{cvalue}
{};

void Condition::codegen() {
    cvalue->codegen();
    push_line(SWAP, 'b');
    bvalue->codegen();
    push_line(SUB, 'b');
}

IfThen::IfThen(Condition* condition, Node* commands, Node* elseBody)
: condition{condition},
    commands{commands},
    elseBody{elseBody}
{};

void IfThen::codegen() {
    condition->codegen();
    int type = condition->symbol;

    push_jumps(type);

    commands->codegen();

    if (elseBody != NULL) {
        prepare_jump(JUMP);

        if (type == cEQ || type == cGE || type == cLE) {
            backfill_else_jump();
            backfill_else_jump();
        } else {
            backfill_else_jump();
        }

        elseBody->codegen();
    }

    if (elseBody == NULL && (type == cEQ || type == cGE || type == cLE)) {
        backfill_jump();
        backfill_jump();
    } else {
        backfill_jump();
    }
}

While::While(Condition* condition, Commands* commands)
: condition{condition},
    commands{commands}
{};

Repeat::Repeat(Condition* condition, Commands* commands)
: condition{condition},
    commands{commands}
{};
For::For(Node* identifier, Node* fromValue, Node* toValue, Node* commands, int step)
: identifier{identifier},
    fromValue{fromValue},
    toValue{toValue},
    commands{commands},
    step{step}
{};

Read::Read(Identifier* identifier) : identifier{identifier} {};
    
void Read::codegen() {
    push_line(RESET, 'b'); // Rb = 0
    for(int i = 0; i < identifier->id; i++) {
        push_line(INC, 'b');
    } // Rb = &identifier
    push_line(GET, 0); // Ra = z klawiatury
    push_line(STORE, 'b'); // P(Rb) = Ra
}

Write::Write(int intval) : value{NULL}, intval(intval) {};
Write::Write(Node* value) : value{value} {};

void Write::codegen() {
    if (value != NULL) {
        value->codegen();
    } else {
        set_register_a_to_value(intval);
    }
    push_line(PUT, 0);
}

    Num::Num(int value) : value{value} {};

void Num::codegen() {
    set_register_a_to_value(value);
}

Identifier::Identifier(int id) : id{id} {};

void Identifier::codegen() {
    set_register_a_to_value(id);
    push_line(LOAD, 'a');
}