%option noyywrap
%option yylineno
%{
    /*Definition section */
    #include "gram.tab.h"
    extern int yylval;
    int yylex();
%}

/* 
Comments cannot appear before XML declaration.
Comments may appear anywhere in a document.
Comments must not appear within attribute values.
Comments cannot be nested inside the other comments. 
*/
NUMBER         [0-9]+(\.[0-9]+)?

%%
^"#".*"\n"            ;
[ \t]           ;
{NUMBER}        { yylval = atol(yytext); return NUM; }
"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"				{ return MOD; }
"^"             { return POW; }
"("				{ return LBR; }
")"				{ return RBR; }
"\\\n"          {  }
"\n"            { return END; }
.               { return ERROR; }
%%
  
// int yywrap(){}


// "%"				{ return MOD; }
// "^"             { return POW; }
// "("				{ return L_BRACKET; }
// ")"				{ return R_BRACKET; }