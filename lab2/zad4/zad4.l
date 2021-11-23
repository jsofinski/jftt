%{
    /*Definition section */
    char stackChar[1000];
    char *scp;
    int stackNumber[1000];
    int *snp;
    int result = 0;
    int error = 0; // 0 = ok; 1 = za mało operatorow; 2 = zły znak
    char wrongSymbol = ' ';
    int countNumbers = 0;
    #define push(sp, n) (*((sp)++) = (n))
    #define pop(sp) (*--(sp))
%}
  

%%
['+'|'\-'|'*'|'/'|'^'|'%']  {inputSymbol(yytext);}
['\-']?[0-9]*               {inputNumber(yytext);}
[\n]                        {handleInput();}
[' '|\t]                    {printf("%s", yytext); fprintf(yyout, "%s", yytext);}
.                           {printf("%s", yytext); fprintf(yyout, "%s", yytext); wrongSymbol=yytext[0]; error = 2;}
%%
  
#include <stdio.h>
#include <math.h>
#include <string.h>

int yywrap(){}

int inputNumber(char* input) {
    countNumbers++;
    fprintf(yyout, "%s", yytext); 
    int value = atoi(input);
    printf("%d", value);
    push(snp, value);
}
int inputSymbol(char* input) {
    fprintf(yyout, "%s", yytext); 
    printf("%s", input);
    char symbol = input[0];
    // push(scp, input[0]);
    // tutej będą obliczenia 

    int a = pop(snp);
    int b = pop(snp);
    countNumbers--;

    if (countNumbers < 1) {
        error = 1;
    }
    
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
    if (error == 1) {
        printf("\nBłąd: za mała liczba argumentów\n");
            fprintf(yyout, "\nBłąd: za mała liczba argumentów\n");
        countNumbers = 0;
    }
    else if (error == 2) {
        printf("\nBłąd: zły symbol \"%c\"\n", wrongSymbol);
        fprintf(yyout, "\nBłąd: zły symbol \"%c\"\n", wrongSymbol);
    }
    else {
        int value = pop(snp);
        int test = pop(snp);
        // printf("countNumbers: %d\n", countNumbers);
        if (countNumbers > 1) {
            error = 1;
            printf("\nBłąd: za mała liczba operatorów\n");
            fprintf(yyout, "\nBłąd: za mała liczba operatorów\n");
        }
        else {
            countNumbers = 0;
            printf("\n = %d\n", value);
            fprintf(yyout, "\n = %d\n", value);
        }   
    }
    error = 0;
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
    
    snp = stackNumber;
    
    yylex();
    return 0;
}

