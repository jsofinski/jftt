%{
    /*Definition section */
    #include "header.h"
    int wordsCount = 0;
    int linesCount = 1;
    char lastWs = 'x';
    int lastWasWs = 0;
%}


%%
^[\t|' ']*[\n]  {} //empty lines
^[\t|' ']+      {} //remove whitespaces from begging
[\t|' ']+$      {} //remove whitespaces from end
[\t|' ']+       {lastWs = yytext[0]; lastWasWs = 1;} // multiple whitepaces
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
int main()
{
  
    /* yyin and yyout as pointer
    of File type */
    extern FILE *yyin, *yyout;
  
    /* yyin points to the file input.txt
    and opens it in read mode*/
    yyin = fopen("input.txt", "r");
  
    /* yyout points to the file output.txt
    and opens it in write mode*/
    yyout = fopen("Output.txt", "w");
  
    yylex();

    printf("lines: %8d\nwords: %8d\n", linesCount, wordsCount);

    return 0;
}

