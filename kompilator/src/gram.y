%{
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "header.h"
    #include "codegen.h"
    int yylex();
    int yyerror(char*); 
    int errors = 0;
    int yydebug = 0;

    Node *ASTroot = NULL;
%}

%union { 
    int intval;
    Node* node;
    Identifier* idf;
    Condition* cond;
}

%token <intval> NUM
%token <intval> PIDENTIFIER /* Simple identifier */
%token VAR BEGINX END
%token ASSIGN IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token FOR FROM TO DOWNTO ENDFOR
%token READ WRITE

%left PLUS MINUS TIMES DIV MOD
%left EQ NEQ LE GE LEQ GEQ

%start program
%type <node> program
%type <node> commands
%type <node> command
%type <node> expression
%type <cond> condition
%type <node> bvalue
%type <node> cvalue
%type <idf> identifier
%type <intval> conditionSymbol

%%
program: VAR 
            declarations
        BEGINX
            commands
        END                 { ASTroot = new Program($4); }
    |   BEGINX commands END { ASTroot = new Program($2); }
;
declarations: declarations ',' PIDENTIFIER          {}
    |   declarations ',' PIDENTIFIER'['NUM':'NUM']' {}
    |   PIDENTIFIER                                 {}
    |   PIDENTIFIER'['NUM':'NUM']'                  {}
;
commands: commands command  {$$ = new Commands($1, $2);}
    | command               {$$ = $1;}
;
command: identifier ASSIGN expression ';'    { $$ = new Assign($1, $3); }
    |   IF condition
        THEN commands
        ELSE commands
        ENDIF { $$ = new IfThen($2, $4, $6); }
    |   IF condition
        THEN commands
        ENDIF { $$ = new IfThen($2, $4, NULL); }
    |   WHILE condition
        DO commands
        ENDWHILE { $$ = new While($2, $4); }
    |   REPEAT commands
        UNTIL condition ';' { $$ = new Repeat($2, $4); }
    |   FOR identifier
        FROM bvalue
        TO cvalue
        DO commands
        ENDFOR { $$ = new For($2, $4, $6, $8, 1); }
    |   FOR identifier
        FROM bvalue
        DOWNTO cvalue
        DO commands
        ENDFOR { $$ = new For($2, $4, $6, $8, -1); }
    |   READ identifier ';' { $$ = new Read($2); }
    |   WRITE NUM ';' { $$ = new Write($2); }
    |   WRITE identifier ';' { $$ = new Write($2); }
;
expression: bvalue            { $$ = $1; }
    |   bvalue PLUS cvalue    { $$ = new Expression($1, ePLUS, $3); }
    |   bvalue MINUS cvalue   { $$ = new Expression($1, eMINUS, $3); }
    |   bvalue TIMES cvalue   { $$ = new Expression($1, eTIMES, $3); }
    |   bvalue DIV cvalue     { $$ = new Expression($1, eDIV, $3); }
    |   bvalue MOD cvalue     { $$ = new Expression($1, eMOD, $3); }
;
condition: bvalue conditionSymbol cvalue { $$ = new Condition($1, $2, $3); }
;
conditionSymbol: EQ { $$ = cEQ; }
    |   NEQ { $$ = cNEQ; }
    |   LE { $$ = cLE; }
    |   GE { $$ = cGE; }
    |   LEQ { $$ = cLEQ; }
    |   GEQ { $$ = cGEQ; }
;
bvalue: NUM         { $$ = new Num($1); }
    | identifier    { $$ = $1; }
;
cvalue: NUM         { $$ = new Num($1); }
    | PIDENTIFIER    { $$ = new Identifier($1); /* TODO uzywaj `identifier` zmiast PIDENTIFIER */ }
;
identifier: PIDENTIFIER             { $$ = new Identifier($1); }
    | PIDENTIFIER'['PIDENTIFIER']'  {}
    | PIDENTIFIER'['NUM']'          {}

%%

int yyerror(char *s) {
    printf("%s\n", s);
    return 0;
}


int main(int argc, char *argv[]) {

    extern FILE *yyin;

    yyin =  fopen(argv[1], "r");
    if (!yyin) {
        printf("Nie można otworzyć pliku %s\n", argv[1]);
        return -1;
    }
    yydebug = 1;
    errors = 0;
    yyparse();

    ASTroot->codegen();

    print_lines();

    return 0;
}
