%{
    /*Definition section */
%}
  
/* 
Comments cannot appear before XML declaration.
Comments may appear anywhere in a document.
Comments must not appear within attribute values.
Comments cannot be nested inside the other comments. 
*/

%%
^[\t|' ']*"<!--"[a-zA-Z|' '|\t^'-']*"-->"[\n]    {fprintf(yyout, "");} //empty lines
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
    return 0;
}

