%{
    /*Definition section */
    #include "header.h"
    int wordsCount = 0;
    int linesCount = 1;
    char lastWs = 'x';
    int lastWasWs = 0;
%}


%%
^[\t' ']*[\n]  {}
^[\t' ']+      {}
[\t' ']+$      {}
(\t|' ')+       {lastWs = yytext[0]; lastWasWs = 1;}
[\n]            {linesCount++; fprintf(yyout, "%s", yytext);}
[a-zA-Z]+       {checkWs(); wordsCount++; fprintf(yyout, "%s", yytext);}
.               {checkWs(); fprintf(yyout, "%s", yytext);}
%%
  
int yywrap(){}
  

int checkWs() {
    if (lastWasWs) {
        lastWasWs = 0;
        fprintf(yyout, " "); 
    }
}

// driver code 
int main(int argc, char *argv[])
{
  
    if (argc > 2) {
        yyin = fopen(argv[1], "r");
        yyout = fopen(argv[2], "w");
    }
    else {
        yyin = fopen("input.txt", "r");
        yyout = fopen("Output.txt", "w");
    }
  

    yylex();

    printf("lines: %8d\nwords: %8d\n", linesCount, wordsCount);

    return 0;
}

