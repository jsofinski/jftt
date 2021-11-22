%{
    /*Definition section */
    int wordsCount = 0;
    int linesCount = 1;
%}
  
%%
^[\t|' ']*[\n]      {fprintf(yyout, "");} //empty lines
^[\t|' ']+          {fprintf(yyout, "");} //remove whitespaces from begging
[\t|' ']+$       {fprintf(yyout, "");} //remove whitespaces from end
[\t|' ']+  {fprintf(yyout, " ");} // multiple whitepaces
[\n]        {linesCount++; fprintf(yyout, "%s", yytext);}
[a-zA-Z]+    {wordsCount++; fprintf(yyout, "%s", yytext);}
.       {fprintf(yyout, "%s", yytext); }
%%
  
int yywrap(){}
  
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

    printf("lines: %8d\n words: %8d\n", linesCount, wordsCount);

    return 0;
}

