%{
    #define YYSTYPE long
    #include<stdio.h>
    #include<math.h>
    #include "header.h"
    extern int yylineno; // z lexa
    int yylex();
    int yyerror(char*); 
    long size = 1234577;
%}

%token NUM
%left SUB
%left ADD
%left MUL
%left DIV
%left MOD
%left NEG
%right POW
%token END
%left LBR
%left RBR
%token ERROR


%%
input:
    | input line
;

line: expr END {printf("\n= %ld\n", $$);}
    | error END {printf("Blad w skladni w linii %d\n", yylineno-1);}
;

expr:   NUM           {printf("%ld ", num(yylval, 0)); $$ = num($1, 0);}
    |   SUB expn      {printf("%ld ", num(-yylval, 0)); $$ = num(-$2, 0);}
    |   expr ADD expr {printf("+ "); $$ = add($1, $3, 0);}
    |   expr MUL expr {printf("* "); $$ = mul($1, $3, 0);}
    |   expr SUB expr {printf("[-]"); $$ = sub($1, $3, 0);}
    |   expr DIV expr {printf("/" ); $$ = div($1, $3, 0);}
    |   expr MOD expr {printf("mod "); $$ = mod($1, $3, 0);}
    |   expr POW expp {printf("^ "); $$ = myPow($1, $3, 0);}
    |   LBR expr RBR  {printf("() "); $$ = $2;}
    ;
expp:   NUM           {printf("%ld ", yylval); $$ = $1;}
    |   SUB expn      {printf("%ld ", num(-yylval, 1)); $$ = -$2;}
    |   expp ADD expp {printf("+ "); $$ = add($1, $3, 1);}
    |   expp MUL expp {printf("* "); $$ = mul($1, $3, 1);}
    |   expp SUB expp {printf("[-,%ld]", sub($1, $3)); $$ = sub($1, $3, 1);}
    |   expp DIV expp {printf("/" ); $$ = div($1, $3, 1);}
    |   expp MOD expp {printf("mod "); $$ = mod($1, $3, 1);}
    |   LBR expp RBR  {printf("() "); $$ = $2;}
    ;
expn:   NUM           {$$ = $1;}
    |   expr ADD expr {printf("+ "); $$ = add($1, $3);}
    |   expr MUL expr {printf("* "); $$ = mul($1, $3);}
    |   expr SUB expr {printf("[-]"); $$ = sub($1, $3);}
    |   expr DIV expr {printf("/" ); $$ = div($1, $3);}
    |   expr MOD expr {printf("mod "); $$ = mod($1, $3);}
    |   expr POW expp {printf("^ "); $$ = myPow($1, $3);}
    |   LBR expr RBR  {printf("() "); $$ = num($2);}
%%
int yyerror(char *s) {
    printf("%s\n", s);
    return 0;
}


// expn:   NUM           {$$ = $1;}
//     |   LBR expn RBR  {printf("() "); $$ = num($2);}
//     |   expn ADD expn {printf("+ "); $$ = add($1, $3);}
//     |   expn MUL expn {printf("* "); $$ = mul($1, $3);}
//     |   expn SUB expn {printf("[-]"); $$ = sub($1, $3);}
//     |   expn DIV expn {printf("/" ); $$ = div($1, $3);}
//     |   expn MOD expn {printf("mod "); $$ = mod($1, $3);}
//     |   expn POW expp {printf("^ "); $$ = myPow($1, $3);}
//     ;



long normalize(long a, long x) {
    long newSize = size-x;
    
    if (a >= newSize) {
        return (a%newSize);
    }
    while (a < 0) {
        a += newSize;
    }
    return a;
}

long num(long a, long x) {
    return normalize(a, x);
}

long add(long a, long b, long x) {
    return normalize(a+b, x);
}

long mul(long a, long b, long x) {
    return normalize(a*b, x);
}

long neg(long a, long x) {
    return normalize(-a, x);
}

long sub(long a, long b, long x) {
    return normalize(a-b, x);
}

long div(long a, long b, long x) {
    for (long i = 1; i < size-2; i++) {
        if (normalize(i * b, x) == a) {
            return i;
        }
    }
    printf("Brak elementu odwrotnego\n");
    return 0;
}

long mod(long a, long b, long x) {
    return normalize(a%b, x);
}

long myPow(long a, long b, long x) {
    long i = 1;
    long result = a;
    long power = b;
    long newSize = size-1;
    if (power >= newSize) {
        power = power%newSize;
    }
    while (power < 0) {
        power += newSize;
    }
    while (i < power) {
        result = normalize(result*a, x);
        i++;
    }
    return result;
}

int main(int argc, char *argv[])
{
    yyparse();

    // yyin = fopen("input.txt", "r");
    // yyout = fopen("Output.txt", "w");

    printf("Przetworzono linii: %d\n", yylineno-1);

    return 0;
}