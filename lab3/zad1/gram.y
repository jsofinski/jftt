%{
    #define YYSTYPE int
    #include<stdio.h>
    #include<math.h>
    extern int yylineno; // z lexa
    int yylex();
    int yyerror(char*); 
    char stackChar[1000];
    char *scp;
    int stackNumber[1000];
    int *snp;
    #define push(sp, n) (*((sp)++) = (n))
    #define pop(sp) (*--(sp))
%}

%token NUM
%token SUB
%token ADD
%token MUL
%token DIV
%token MOD
%token POW
%token END
%token LBR
%token RBR
%token ERROR


%%
input:
    | input line
;

line: expr END {printf("Linia %d = %d\n", yylineno-1, $$);}
    | error END {printf("Blad w skladni w linii %d\n", yylineno-1);}
;

expr:   NUM           {printf("%d", yylval); $$ = $1;}
    |   expr ADD expr {printf("+"); $$ = $1 + $3;}
    |   expr MUL expr {printf("*"); $$ = $1 * $3;}
    |   expr SUB expr {printf("-"); $$ = $1 - $3;}
    |   expr DIV expr {printf("/"); $$ = $1 / $3;}
    |   expr MOD expr {printf("mod"); $$ = $1 % $3;}
    |   expr POW expr {printf("^"); $$ = (int)pow($1, $3);}
    |   LBR expr RBR  {printf("()"); $$ = $2;}
    ;
%%
int yyerror(char *s) {
    printf("%s\n", s);
    return 0;
}
// driver code 


int main(int argc, char *argv[])
{
    yyparse();

    // yyin = fopen("input.txt", "r");
    // yyout = fopen("Output.txt", "w");

    printf("Przetworzono linii: %d\n", yylineno-1);

    return 0;
}