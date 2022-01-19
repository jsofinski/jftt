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
    int check = 32768;
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

void push_jump_to(int type, int jump_to) {
    switch (type) {
        case cEQ:
            push_line(JPOS, jump_to);
            push_line(JNEG, jump_to);
            break;
        case cNEQ:
            push_line(JZERO, jump_to);
            break;
        case cGE:
            push_line(JNEG, jump_to);
            push_line(JZERO, jump_to);
            break;
        case cLE:
            push_line(JPOS, jump_to);
            push_line(JZERO, jump_to);
            break;
        case cGEQ:
            push_line(JNEG, jump_to);
            break;
        case cLEQ:
            push_line(JPOS, jump_to);
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
    // Rg <= value
    expression->codegen();
    push_line(SWAP, 'g');

    set_register_a_to_value(id->id); // Ra = &identifier
    push_line(SWAP, 'g'); // value <=> &identifier

    push_line(STORE, 'g'); // P(Rg) <= Ra
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
            push_line(RESET, 'e');
            push_line(RESET, 'f');
            push_line(INC, 'e');
            push_line(DEC, 'f');

            /*
                Re = 1 do shiftowania
                Rf = -1 do shiftowania

                Rd = wynik
                Rc = prawa liczba
                Rb = lewa liczba (shiftowana w prawo)
                Ra = rejestr roboczy
            */

            cvalue->codegen();
            push_line(SWAP, 'c');
            bvalue->codegen();
            push_line(SWAP, 'b');
            push_line(RESET, 'd');

            int jump_to_start = instruction_pointer;

            // testuj najmniej znaczacy bit Rb
            push_line(RESET, 'a');
            push_line(ADD, 'b');

            push_line(SHIFT, 'f');
            push_line(SHIFT, 'e');
            push_line(SUB, 'b');
            // Ra to teraz -1 albo 0

            prepare_jump(JZERO); // if (Rb % 2 != 0) {
            // Rd += Rc
            push_line(SWAP, 'd');
            push_line(ADD, 'c');
            push_line(SWAP, 'd');

            backfill_jump(); // }

            // przesun Rb o 1 w prawo
            push_line(SWAP, 'b');
            push_line(SHIFT, 'f');
            push_line(SWAP, 'b');
            // przesun Rc o 1 w lewo
            push_line(SWAP, 'c');
            push_line(SHIFT, 'e');
            push_line(SWAP, 'c');

            push_line(RESET, 'a');
            push_line(ADD, 'b');

            // jezeli Rb == 0, koniec iteracji, w Rd jest wynik
            push_line(JPOS, - instruction_pointer + jump_to_start);
            push_line(JNEG, - instruction_pointer + jump_to_start);
            push_line(SWAP, 'd');
        break;
        case eDIV:
            push_line(RESET, 'e');
            push_line(RESET, 'f');
            push_line(INC, 'e');
            push_line(DEC, 'f');

            /* Re = 1 do shiftowania
               Rf = -1 do shiftowania
            */
            cvalue->codegen();
            push_line(SWAP, 'c');
            bvalue->codegen();
            push_line(SWAP, 'b');
            push_line(RESET, 'd');

            // obliczanie log2(Rb) -> wynik leci do Rd
            push_line(RESET, 'a');
            push_line(RESET, 'd');

            push_line(ADD, 'b');

            int jump_here = instruction_pointer;

            push_line(INC, 'd');
            push_line(SHIFT, 'f');

            push_line(JPOS, - instruction_pointer + jump_here);


        break;
        // case eMOD:
        //     push_line(RESET, 'a');  
        // break;
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

While::While(Condition* condition, Node* commands)
: condition{condition},
    commands{commands}
{};

void While::codegen() {
    int jump_to_start = instruction_pointer;
    condition->codegen();
    int type = condition->symbol;

    push_jumps(type);

    commands->codegen();

    push_line(JUMP, - instruction_pointer + jump_to_start);

    if (type == cEQ || type == cGE || type == cLE) {
        backfill_jump();
        backfill_jump();
    } else {
        backfill_jump();
    }
}

Repeat::Repeat(Node* commands, Condition* condition)
: condition{condition},
    commands{commands}
{};

void Repeat::codegen() {
    int jump_to_start = instruction_pointer;
    commands->codegen();
    condition->codegen();
    push_jump_to(condition->symbol, - instruction_pointer + jump_to_start);
}

For::For(Identifier* identifier, Node* fromValue, Node* toValue, Node* commands, int step)
: identifier{identifier},
    fromValue{fromValue},
    toValue{toValue},
    commands{commands},
    step{step}
{};

void For::codegen() {
    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(SWAP, 'd'); // Rd <= &identifier
    fromValue->codegen();
    push_line(STORE, 'd'); // P(Rd) <= Ra
    
    int jump_to_start = instruction_pointer;

    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(LOAD, 'a'); // Ra <= P(Ra)
    push_line(SWAP, 'f'); // P(Rh) <= Ra, current i
    
    toValue->codegen(); // Ra = &identifier
    push_line(SUB, 'f'); // toValue - i
    
    if (step == 1) {
        push_jumps(cGEQ); // if toValue - i != 0 Jump Outside loop
    } else {
        push_jumps(cLEQ); // if toValue - i != 0 Jump Outside loop
    }

    commands->codegen();


    set_register_a_to_value(step); // a <= step
    push_line(SWAP, 'b');  // Rb <= step
    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(LOAD, 'a'); // Ra <= P(Ra)
    push_line(ADD, 'b');   // Ra += step
    push_line(SWAP, 'f');  // Rh <= current i 
    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(SWAP, 'f');  // Ra <= current i && Rh <= &identifier
    push_line(STORE, 'f'); // P(Ra) <= Ra


    push_line(JUMP, - instruction_pointer + jump_to_start);
    
    backfill_jump();
}



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