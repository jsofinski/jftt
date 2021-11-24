%{
    /*Definition section */
    #include "header.h"
    int xmlDeclaration = 0;
    int attributeCount = 0;
    int stringCount = 0;
%}
  
/* 
Comments cannot appear before XML declaration.
Comments may appear anywhere in a document.
Comments must not appear within attribute values.
Comments cannot be nested inside the other comments. 
*/

%%
"<?xml".*"?>"   {fprintf(yyout, "%s", yytext); xmlDeclaration = 1;}
"<"             {fprintf(yyout, "%s", yytext); attributeCount++;}
">"             {fprintf(yyout, "%s", yytext); attributeCount--;}
"\""            {fprintf(yyout, "%s", yytext); stringCount++;}
("<!--").*("--")+.*("-->")  {fprintf(yyout, "%s", yytext);}
("<!--").*("--->")          {fprintf(yyout, "%s", yytext);}
("<!--").*("-->")           {handleComment(yytext);}
.               {fprintf(yyout, "%s", yytext); }
%%
  
int yywrap(){}
  
int handleComment(char* text) {
    if (stringCount%2 != 0 || attributeCount != 0) {
        fprintf(yyout, "%s", text); 
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
    return 0;
}

