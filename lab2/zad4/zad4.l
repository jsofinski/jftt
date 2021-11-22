%{
    /*Definition section */
    char stackChar[1000];
    char *scp;
    int stackNumber[1000];
    int *snp;
    int result = 0;
    int error = 0;
    #define push(sp, n) (*((sp)++) = (n))
    #define pop(sp) (*--(sp))
%}
  

%%
['+'|'\-'|'*'|'/'|'^'|'%']  {inputSymbol(yytext);}
['\-']?[0-9]*               {inputNumber(yytext);}
[\n]                        {handleInput();}
[' '|\t]                    {fprintf(yyout, "%s", yytext);}
.                           {fprintf(yyout, "%s", yytext); printf("\nBłąd: zły symbol \"%s\"\n", yytext); error = 1;}
%%
  
#include <stdio.h>
#include <math.h>
#include <string.h>

int yywrap(){}

int inputNumber(char* input) {
    fprintf(yyout, "%s", yytext); 
    int value = atoi(input);
    printf("%d ", value);
    push(snp, value);
}
int inputSymbol(char* input) {
    fprintf(yyout, "%s", yytext); 
    printf("%s ", input);
    char symbol = input[0];
    push(scp, input[0]);
    // tutej będą obliczenia 

    int a = pop(snp);
    int b = pop(snp);

    if(symbol == '+') {
        // printf("dodawanie\n");
        push(snp, b+a);
    }
    else if(symbol == '-'){
        // printf("odejmowanie\n");
        push(snp, b-a);
    }
    else if(symbol == '*'){
        // printf("mnożenie\n");
        push(snp, b*a);
    }
    else if(symbol == '/'){
        push(snp, b/a);
    }
    else if(symbol == '^'){
        push(snp, (int)pow((double)b, (double)a));
    }
    else if(symbol == '%'){
        push(snp, b%a);
    }
    else {
        printf("dupa\n");
    }
}
int handleInput() {
    int value = pop(snp);
    printf("\n = %d\n", value);
    fprintf(yyout, "\n = %d", value);
}

// driver code 
int main() {
    /* yyin and yyout as pointer
    of File type */
    extern FILE *yyin, *yyout;
  
    /* yyin points to the file input.txt
    and opens it in read mode*/
    yyin = fopen("input.txt", "r");
  
    /* yyout points to the file output.txt
    and opens it in write mode*/
    yyout = fopen("Output.txt", "w");
  

    scp = stackChar;
    // push(scp, 'a');
    // char y = pop(scp);
    // char z = pop(scp);
    // printf("%c\n", y);

    snp = stackNumber;
    // push(snp, 10);
    // int x = pop(snp);
    // int q = pop(snp);
    // printf("%d\n", x);
    // printf("%d\n", q);



    yylex();

    // int x = pop(snp);
    // while (x != NULL) {
    //     printf("%d ", x);
    //     x = pop(snp);
    // }
    // printf("\n");

    // char z = pop(scp);
    // while (z != NULL) {
    //     printf("%c ", z);
    //     z = pop(scp);
    // }
    // printf("\n");


    return 0;
}

