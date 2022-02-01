#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codegen.h"
#include "header.h"
#include <typeinfo> 

#pragma GCC diagnostic ignored "-Wwrite-strings"

typedef long long i64;
int arrayVariablesNumber = 0;
int arrayVariables[100] = {};
int variablesNumber = 0;
int variables[100] = {};
int initializedVariablesNumber = 0;
int initializedVariables[100] = {};
int iteratorsNumber = 0;
int interators[100] = {};
int banIteratorsNumber = 0;
int banIteratos[100] = {};

struct VMLine {
    VMInstruction op;
    i64 arg; // 0-7 albo 0-2^63
    char* comment;
};

struct VMLine instructions[10000];
int instruction_pointer = 0;

void error(char* reason){
  printf("ERROR: %s\n", reason);
  exit(1);
}

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
            printf("%s %lld", names[instructions[i].op], instructions[i].arg);
        } else {
            printf("%s %c", names[instructions[i].op], (int) instructions[i].arg);
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
    i64 check = 1073741824;
    while ( check < value ) {
        check = check * 2;
    }
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
    push_line(op, -2136);
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

void addVariable(int id) {
    for (int i = 0; i < variablesNumber; i++) {
        if (variables[i] == id) {
            printf("%d\n",id);
            error("Zmienna juz zadeklarowana!");
        }
    }
    // printf("cyk:%d\n", id);
    variablesNumber++;
    variables[variablesNumber] = id;
}

void initVariable(int id) {
    for (int i = 0; i < initializedVariablesNumber; i++) {
        if (initializedVariables[i] == id) {
            return;
        }
    }
    initializedVariablesNumber++;
    initializedVariables[initializedVariablesNumber] = id;
}

void addArrayVariable(int id) {
    for (int i = 0; i < arrayVariablesNumber; i++) {
        if (arrayVariables[i] == id) {
            error("Tablica juz zadeklarowana!");
        }
    }
    // printf("cyk:%d\n", id);
    arrayVariablesNumber++;
    arrayVariables[arrayVariablesNumber] = id;
}

void assertVariableExists(int id) {
    int exists = 0;
    for (int i = 0; i <= variablesNumber; i++) {
        if (variables[i] == id) {
            exists = 1;
        }
    }
    if (!exists) {
        printf("%d\n", id);
        error("Zmienna nie zostala zadeklarowana!");
    }
}

void assertArrayVariableExists(int id) {
    int exists = 0;
    for (int i = 0; i <= arrayVariablesNumber; i++) {
        if (arrayVariables[i] == id) {
            exists = 1;
        }
    }
    if (!exists) {
        error("Tablica nie zostala zadeklarowana!");
    }
}

void assertVariableInitialized(int id) {
    int exists = 0;
    for (int i = 0; i < initializedVariablesNumber; i++) {
        if (initializedVariables[i] == id) {
            exists = 1;
        }
    }
    if (!exists) {
        printf("Zmienna: %d\n", id);
        error("Zmienna nie zostala zainicjalizowana!");
    }
}


void initArray(i64 id, i64 start, i64 end) {
    // printf("initarray \n");
    // printf("%d \n", id);
    // printf("%d \n", start);
    // printf("%d \n", end);
    addArrayVariable(id);
    set_register_a_to_value(id); // Ra = &id
    push_line(SWAP, 'g'); // value <=> &id  // P(Rg) => index start
    set_register_a_to_value(start);
    push_line(STORE, 'g'); // P(Rg) <= Ra


    push_line(INC, 'g'); // P(Rg) => index end
    set_register_a_to_value(end);
    push_line(STORE, 'g'); // P(Rg) <= Ra
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
    initVariable(id->id);
    // iterator check
    for (int i = 0; i < iteratorsNumber; i++) {
        if (id->id == interators[i]) {
            error("Error: zmiana wartosci iteratora!");
        }
    }
    // ban iterator check
    for (int i = 0; i < banIteratorsNumber; i++) {
        if (id->id == banIteratos[i]) {
            error("Error: zmiana wartosci iteratora poza petla!");
        }
    }

    // Rg <= value
    expression->codegen();
    
    // if (typeid(*expression) == typeid(Identifier)) {
    //     push_line(LOAD, 'a'); // P(Rg) <= Ra
    // }
    push_line(SWAP, 'f'); // value <=> &identifier

    id->codegenGetIndex();

    
    push_line(SWAP, 'f'); // value <=> &identifier
    push_line(STORE, 'f'); // P(Rg) <= Ra
}

Expression::Expression(Node* bvalue, int symbol, Node* cvalue)
: bvalue{bvalue},
    symbol{symbol},
    cvalue{cvalue}
{};

void Expression::codegen() {
    if (typeid(bvalue) == typeid(Identifier)) {
        // assertVariableExists(dynamic_cast<Identifier*>(bvalue)->id);
        // assertVariableInitialized(dynamic_cast<Identifier*>(bvalue)->id);

    }
    if (typeid(cvalue) == typeid(Identifier)) {
        // assertVariableExists(dynamic_cast<Identifier*>(cvalue)->id);
        // assertVariableInitialized(dynamic_cast<Identifier*>(cvalue)->id);

    }
    switch(symbol) {
        case ePLUS: {
            cvalue->codegen();
            push_line(SWAP, 'b');
            bvalue->codegen();
            push_line(ADD, 'b');
        break; }
        case eMINUS: {
            cvalue->codegen();
            push_line(SWAP, 'b');
            bvalue->codegen();
            push_line(SUB, 'b');
        break; }
        case eTIMES: {
            //      Rejestr f -> flaga ujemnego wyniku; Jeśli 0 to dodatni
            push_line(RESET, 'e');
            push_line(RESET, 'f');
            push_line(INC, 'e');

            /*
                Re = 1/-1 do shiftowania
                
                Rd = wynik
                Rc = prawa liczba
                Rb = lewa liczba (shiftowana w prawo)
                Ra = rejestr roboczy
            */

            cvalue->codegen();  
            // zmiana flagi h na 1 jeśli ujemne
            prepare_jump(JPOS);
                push_line(SWAP, 'c');
                push_line(RESET, 'a');
                push_line(SUB, 'c');
                push_line(INC, 'f');
            backfill_jump();          
            push_line(SWAP, 'c');

            bvalue->codegen();
            // zmiana flagi h z 1 na 0 lub z 0 na -1 jesli ujemne
            prepare_jump(JPOS);
                push_line(SWAP, 'b');
                push_line(RESET, 'a');
                push_line(SUB, 'b');
                push_line(DEC, 'f');
            backfill_jump();      
            push_line(SWAP, 'b');


            // push_line(RESET, 'h');      // TODO TU GDZIES JEST BLAD W MNOZENIU!! ZŁE WYNIKI


            push_line(RESET, 'd');


            int jump_to_start = instruction_pointer;

            // testuj najmniej znaczacy bit Rb
            push_line(RESET, 'a');
            push_line(ADD, 'b');
            
            push_line(RESET, 'e');
            push_line(DEC, 'e');
            push_line(SHIFT, 'e');
            push_line(INC, 'e');
            push_line(INC, 'e');
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
            push_line(RESET, 'e');
            push_line(DEC, 'e');
            push_line(SHIFT, 'e');
            push_line(SWAP, 'b');
            // przesun Rc o 1 w lewo
            push_line(SWAP, 'c');
            push_line(RESET, 'e');
            push_line(INC, 'e');
            push_line(SHIFT, 'e');
            push_line(SWAP, 'c');

            push_line(RESET, 'a');
            push_line(ADD, 'b');

            // jezeli Rb == 0, koniec iteracji, w Rd jest wynik
            push_line(JPOS, - instruction_pointer + jump_to_start);
            push_line(JNEG, - instruction_pointer + jump_to_start);
            
            push_line(SWAP, 'f');
            prepare_jump(JZERO);
                push_line(RESET, 'a');
                push_line(SUB, 'd');
                push_line(SWAP, 'd');
            backfill_jump();

            push_line(SWAP, 'd');


        break; }
        case eDIV: { 
            push_line(RESET, 'e');
            push_line(RESET, 'f');
            push_line(INC, 'e');

            push_line(RESET, 'g'); // WYNIK
            
            /* Re = 1/-1 do shiftowania
                Rf = flag, tak jak w mnozeniu, if -1 to poprawic
            */
            cvalue->codegen();
            // zmiana flagi f na 1 jeśli ujemne
            prepare_jump(JPOS);
                push_line(SWAP, 'c');
                push_line(RESET, 'a');
                push_line(SUB, 'c'); 
                push_line(INC, 'f');
            backfill_jump();        

            prepare_jump(JZERO); // if c != 0;

                push_line(SWAP, 'c');
                bvalue->codegen();
                prepare_jump(JPOS);
                    push_line(SWAP, 'b');
                    push_line(RESET, 'a');
                    push_line(SUB, 'b');
                    // push_line(DEC, 'f');
                    push_line(INC, 'f');
                backfill_jump();  
                push_line(SWAP, 'b');
                push_line(RESET, 'd');


                // obliczanie log2(Rb) +1 - liczba cyfr +1 -> leci do Rd
                push_line(RESET, 'a');
                push_line(RESET, 'd');

                push_line(ADD, 'b');

                int jump_here = instruction_pointer;

                push_line(INC, 'd');
                push_line(RESET, 'e');
                push_line(DEC, 'e');
                push_line(SHIFT, 'e');

                push_line(JPOS, - instruction_pointer + jump_here);
                push_line(INC, 'd'); // NOWA LINIA

                // Pętla odejmowania
                jump_here = instruction_pointer;         

                    push_line(RESET, 'a');
                    push_line(RESET, 'h');      
                    push_line(ADD, 'b');        // Ra = B
                    push_line(SWAP, 'h');       // wynik iteracji
                    
                    // Rh = Rh >> Rd
                    push_line(RESET, 'a'); // Ra = Rd (liczba cyfr)
                    push_line(SUB, 'd'); // Ra = Rd (liczba cyfr)
                    push_line(SWAP, 'h');
                    push_line(SHIFT, 'h');
                    push_line(SWAP, 'h');
                    push_line(RESET, 'a');

                    // if Rh > Rc put 1 and shift result; else put 0 and shift
                    push_line(SWAP, 'h');         
                    push_line(SUB, 'c');
                    
                    prepare_jump(JNEG);      //RA reminder
                        
                        push_line(SWAP, 'h'); //RH reminder

                        //  Rh = Rh << Rd
                        push_line(RESET, 'a');
                        push_line(ADD, 'd');
                        push_line(SWAP, 'h');
                        push_line(SHIFT, 'h');
                        push_line(SWAP, 'h');
                        push_line(RESET, 'a');

                        // Rh = Rh + Rb
                        push_line(RESET, 'a');
                        push_line(ADD, 'h');
                        push_line(ADD, 'b');
                        push_line(SWAP, 'h');

                        // Rb = Rb >> Rd
                        push_line(RESET, 'a');
                        push_line(SUB, 'd');
                        push_line(SWAP, 'b');
                        push_line(SHIFT, 'b');
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');

                        // Rb = Rb << Rd
                        push_line(RESET, 'a');
                        push_line(ADD, 'd');
                        push_line(SWAP, 'b');
                        push_line(SHIFT, 'b');
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');

                        push_line(SWAP, 'h');
                        push_line(SUB, 'b');
                        push_line(SWAP, 'b');
                        
                        push_line(INC, 'g');
                    backfill_jump();
                    
                    push_line(SWAP, 'g');      
                        push_line(RESET, 'e');
                        push_line(INC, 'e');
                        push_line(SHIFT, 'e');
                    push_line(SWAP, 'g');      
                    push_line(SWAP, 'h');                   

                    push_line(DEC, 'd');    // Rd - liczba potrzebnych przesuniec
                    push_line(RESET, 'a');
                    push_line(ADD, 'd');
                    push_line(DEC, 'a');
                push_line(JPOS, - instruction_pointer + jump_here);
                push_line(JZERO, - instruction_pointer + jump_here);  
                

                // naprawa 1
                push_line(SWAP, 'b');
                push_line(SUB, 'c');
                prepare_jump(JNEG);
                    push_line(INC, 'g');
                backfill_jump();
                push_line(ADD, 'c');
                push_line(SWAP, 'b');
                

                // prepare_jump(JZERO);
                // prepare_jump(JPOS);
                //     push_line(INC, 'g');
                // backfill_jump();
                // backfill_jump();

                // sprawdzenie flagi i ewentualna naprawa znaku
                push_line(SWAP, 'f');
                prepare_jump(JZERO);
                    push_line(SWAP, 'f');

                    push_line(SWAP, 'b');
                    prepare_jump(JZERO);
                        push_line(INC, 'g');
                    backfill_jump();
                    push_line(SWAP, 'b');

                    push_line(DEC, 'f');
                    push_line(RESET, 'a');
                    push_line(SUB, 'g');
                    push_line(SWAP, 'g');
                    push_line(SWAP, 'f');
                    prepare_jump(JZERO);
                        push_line(SWAP, 'b');
                        prepare_jump(JZERO);
                            push_line(INC, 'g');
                        backfill_jump();
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');
                        push_line(SUB, 'g');
                        push_line(SWAP, 'g');                        
                    backfill_jump();
                backfill_jump();
                
                
                push_line(SWAP, 'g');   
            backfill_jump();
        break; }
        case eMOD: {
            push_line(RESET, 'e');
            push_line(RESET, 'f');
            push_line(INC, 'e');

            /* Re = 1 do shiftowania
                f: -3, oba ujemne zmiana znaku
                f: -2, ujemne tylko c, zmiana znaku i poprawa
                f: -1, ujemne tylko b, poprawa
            */
            cvalue->codegen();
            prepare_jump(JPOS);
                push_line(SWAP, 'c');
                push_line(RESET, 'a');
                push_line(SUB, 'c'); 
                push_line(DEC, 'f');
                push_line(DEC, 'f');
            backfill_jump();        

            prepare_jump(JZERO); // if c == 0;

                push_line(SWAP, 'c');
                bvalue->codegen();
                prepare_jump(JPOS);
                    push_line(SWAP, 'b');
                    push_line(RESET, 'a');
                    push_line(SUB, 'b');
                    push_line(DEC, 'f');
                backfill_jump();  
                push_line(SWAP, 'b');
                push_line(RESET, 'd');


                push_line(RESET, 'g'); // WYNIK

                // obliczanie log2(Rb) - liczba cyfr -> wynik leci do Rd
                push_line(RESET, 'a');
                push_line(RESET, 'd');

                push_line(ADD, 'b');

                int jump_here = instruction_pointer;

                push_line(INC, 'd');
                push_line(RESET, 'e');
                push_line(DEC, 'e');
                push_line(SHIFT, 'e');

                push_line(JPOS, - instruction_pointer + jump_here);

                // Pętla odejmowania
                jump_here = instruction_pointer;         

                    push_line(RESET, 'a');
                    push_line(RESET, 'h');      
                    push_line(ADD, 'b');        // Ra = B
                    push_line(SWAP, 'h');       // wynik iteracji

                    // Rh = Rh >> Rd - 1
                    push_line(RESET, 'a'); // Ra = Rd (liczba cyfr)
                    push_line(SUB, 'd'); // Ra = Rd (liczba cyfr)
                    push_line(DEC, 'a'); // Ra = Rd (liczba cyfr)
                    push_line(SWAP, 'h');
                    push_line(SHIFT, 'h');
                    push_line(SWAP, 'h');
                    push_line(RESET, 'a');

                    // if Rh > Rc put 1 and shift result; else put 0 and shift
                    push_line(SWAP, 'h');         
                    push_line(SUB, 'c');
                    
                    prepare_jump(JNEG);      //RA reminder
                        
                        push_line(SWAP, 'h'); //RH reminder

                        // Rh = Rh << Rd + 1
                        push_line(RESET, 'a');
                        push_line(ADD, 'd');
                        push_line(INC, 'a');
                        push_line(SWAP, 'h');
                        push_line(SHIFT, 'h');
                        push_line(SWAP, 'h');
                        push_line(RESET, 'a');

                        push_line(RESET, 'a');
                        push_line(ADD, 'h');
                        push_line(ADD, 'b');
                        push_line(SWAP, 'h');

                        // Rb = Rb >> (Rd + 1)
                        push_line(RESET, 'a');
                        push_line(SUB, 'd');
                        push_line(DEC, 'a');
                        push_line(SWAP, 'b');
                        push_line(SHIFT, 'b');
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');

                        // Rb = Rb << (Rd + 1)
                        push_line(RESET, 'a');
                        push_line(ADD, 'd');
                        push_line(INC, 'a');
                        push_line(SWAP, 'b');
                        push_line(SHIFT, 'b');
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');

                        push_line(SWAP, 'h');
                        push_line(SUB, 'b');
                        push_line(SWAP, 'b');
                        
                        push_line(INC, 'g');
                    backfill_jump();
                    
                    push_line(SWAP, 'g');        
                        push_line(RESET, 'e');
                        push_line(INC, 'e');
                        push_line(SHIFT, 'e');
                    push_line(SWAP, 'g');      
                    push_line(SWAP, 'h');                   

                    push_line(DEC, 'd');    // Rd - liczba potrzebnych przesuniec
                    push_line(RESET, 'a');
                    push_line(ADD, 'd');
                push_line(JPOS, - instruction_pointer + jump_here);
                push_line(JZERO, - instruction_pointer + jump_here);  
                

                push_line(RESET, 'a');
                push_line(RESET, 'g');  
                push_line(ADD, 'b');  
                push_line(SWAP, 'g');

                push_line(ADD, 'b'); 
                push_line(SUB, 'c');

                prepare_jump(JZERO);
                prepare_jump(JPOS);
                    push_line(SWAP, 'g');
                backfill_jump();
                backfill_jump();

                /* 
                    f: -3, oba ujemne zmiana znaku
                    f: -2, ujemne tylko c, zmiana znaku i poprawa
                    f: -1, ujemne tylko b, poprawa
                */  
                // b i c dodatnie
                prepare_jump(JZERO);

                push_line(SWAP, 'f');
                prepare_jump(JZERO);
                    // b ujemne
                    push_line(SWAP, 'f');
                    // poprawa
                    push_line(SWAP, 'c');
                    push_line(SUB, 'c');
                    
                    push_line(INC, 'f');
                    push_line(SWAP, 'f');
                    prepare_jump(JZERO);
                        push_line(SWAP, 'f');
                        // c ujemne
                        // zmiana znaku
                        push_line(SWAP, 'b');
                        push_line(RESET, 'a');
                        push_line(SUB, 'b');

                        // poprawa
                        //oba ujemne
                        push_line(INC, 'f');
                        push_line(SWAP, 'f');
                        prepare_jump(JZERO);
                            push_line(SWAP, 'f');
                            push_line(RESET, 'a');
                            push_line(SUB, 'c');
                                // zmiana znaku
                            push_line(SWAP, 'f');
                            backfill_jump();
                    backfill_jump();
                backfill_jump();
                push_line(SWAP, 'f');

                backfill_jump();
            backfill_jump();
        break; }
    }      
}

Condition::Condition(Node* bvalue, int symbol, Node* cvalue)
: bvalue{bvalue},
    symbol{symbol},
    cvalue{cvalue}
{};

void Condition::codegen() {
    cvalue->codegen();
    // if (typeid(*cvalue) == typeid(Identifier)) {
    //     push_line(LOAD, 'a');
    // }
    push_line(SWAP, 'b');
    bvalue->codegen();
    // if (typeid(*bvalue) == typeid(Identifier)) {
    //     push_line(LOAD, 'a');
    // }
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
    for (int i = 0; i < banIteratorsNumber; i++) {
        if (banIteratos[i] == identifier->id) {
            for (int j = i; j < banIteratorsNumber -1; j++) {
                banIteratos[j] = banIteratos[j+1];
            }
            banIteratorsNumber--;
        }
    }
    interators[iteratorsNumber] = identifier->id;
    iteratorsNumber++;

    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(SWAP, 'd'); // Rd <= &identifier
    fromValue->codegen();
    push_line(STORE, 'd'); // P(Rd) <= Ra
    
    toValue->codegen(); // Ra = &identifier
    push_line(INC, 'd'); // P(Rd) <= Ra
    push_line(STORE, 'd'); // P(Rd) <= Ra

    int jump_to_start = instruction_pointer;

    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(LOAD, 'a'); // Ra <= P(Ra)
    push_line(SWAP, 'f'); // P(Rh) <= Ra, current i
    
    toValue->codegen(); // Ra = &identifier

    set_register_a_to_value(identifier->id); // Ra = &identifier
    push_line(INC, 'a'); // P(Ra) = to value
    push_line(LOAD, 'a'); // Ra = to value
    
    // if (typeid(*toValue) == typeid(Identifier)) {
    //     push_line(LOAD, 'a');
    // }
    
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

    banIteratos[banIteratorsNumber] = identifier->id;
    banIteratorsNumber++;
}



Read::Read(Identifier* identifier) : identifier{identifier} {};
    
void Read::codegen() {
    // assertVariableExists(identifier->id);
    initVariable(identifier->id); 

    identifier->codegenGetIndex();
    push_line(SWAP, 'b'); // P(Rb) = Ra
    push_line(GET, 0); // Ra = z klawiatury
    push_line(STORE, 'b'); // P(Rb) = Ra
}

Write::Write(i64 intval) : value{NULL}, intval(intval) {};
Write::Write(Identifier* value) : value{value} {};

void Write::codegen() {    
    if (typeid(value) == typeid(Identifier)) {
        // assertVariableExists(dynamic_cast<Identifier*>(value)->id);
        // assertVariableInitialized(dynamic_cast<Identifier*>(value)->id);

        for (int i = 0; i < banIteratorsNumber; i++) {
            if (dynamic_cast<Identifier*>(value)->id == banIteratos[i]) {
                error("Error, zmiana wartosci iteratora poza petla!");
            }
        }      
    }
    if (value != NULL) {  
        value->codegen();    
    } else {
        set_register_a_to_value(intval); 
    }
    push_line(PUT, 0);      
}

    
Num::Num(i64 value) : value{value} {};

void Num::codegen() {
    set_register_a_to_value(value);
}

Identifier::Identifier(i64 id) : id{id} {};
Identifier::Identifier(i64 id, Node* value) : id(id), value(value) {};

void Identifier::codegen() {  
    codegenGetIndex();
    push_line(LOAD, 'a');
    // assertVariableExists(id);
    // assertVariableInitialized(id);
}

void Identifier::codegenGetIndex() {  
    if (value == NULL) {
        // for (int i = 0; i <= arrayVariablesNumber; i++) {
        //     if (arrayVariables[i] == id) {
        //         printf("%d\n", id);
        //         error("Niewłaściwe użycie zmiennej tablicowej!");
        //     }
        // }
        set_register_a_to_value(id);
    } else {        
        // assertArrayVariableExists(id);
        value->codegen();                                   // Ra = -4                    
            
        push_line(SWAP, 'g'); // Rh = start index           Rg = -4 Ra = 2137
        set_register_a_to_value(id); //                     Ra = 32
        push_line(SWAP, 'h'); // True memory index          Ra = 0 Rh = 32 Rg = -4  
        push_line(LOAD, 'h'); // Ra = start index           Ra = -5 Rh = 32 Rg = -4
        push_line(SWAP, 'h'); // True memory index          Ra = 32 Rh = -5 Rg = -4
        push_line(INC, 'a'); // skip start value            Ra = 33
        push_line(INC, 'a'); // skip end value              Ra = 34
        push_line(ADD, 'g'); //                             Ra = 29
        push_line(SUB, 'h'); //                             Ra = 33   
        // tab[4]
        // Ra = indexMap( tab ) + 2 + 4(offset) - -5

        // push_line(INC, 'a'); // skip start value            Ra = -4
        // push_line(INC, 'a'); // skip end value              Ra = -3
        // push_line(SWAP, 'h'); // True memory index          Ra = 6 Rh = -3
        // set_register_a_to_value(id);                    //  Ra = 16
    }      
}