%{
    #include <string.h>
    #include "codegen.h"
    #include "gram.tab.h"

    char *strs[1000] = {0};
    int str_to_id_int(char *yytext) {
        for(int i = 0; i < 1000; i++) {
            if (strs[i] == NULL) {
                strs[i] = strdup(yytext);
                // printf("%d: %s\n", i, strs[i]);
                return i;
            } else if (strcmp(strs[i], yytext) == 0) {
                return i;
            }
        }
        return -1;
    }
%}

DIGIT [0-9]
ID [a-z][a-z0-9]*

%%
-{DIGIT}+        { yylval.intval = atoi(yytext); return(NUM); }
{DIGIT}+        { yylval.intval = atoi(yytext); return(NUM); printf("%s", yytext); }
VAR             { return(VAR);  }
BEGIN           { return(BEGINX); }
END             { return(END); }
ASSIGN          { return(ASSIGN); }
IF              { return(IF); }
THEN            { return(THEN); }
ELSE            { return(ELSE); }
ENDIF           { return(ENDIF); }
WHILE           { return(WHILE); }
DO              { return(DO); }
ENDWHILE        { return(ENDWHILE); }
REPEAT          { return(REPEAT); }
UNTIL           { return(UNTIL); }
FOR             { return(FOR); }
FROM            { return(FROM); }
TO              { return(TO); }
DOWNTO          { return(DOWNTO); }
ENDFOR          { return(ENDFOR); }
READ            { return(READ); }
WRITE           { return(WRITE); }
PLUS            { return(PLUS); }
MINUS           { return(MINUS); }
TIMES           { return(TIMES); }
DIV             { return(DIV); }
MOD             { return(MOD); }
EQ              { yylval.intval = EQ; return(EQ); }
NEQ             { yylval.intval = NEQ; return(NEQ); }
LE              { yylval.intval = LE; return(LE); }
GE              { yylval.intval = GE; return(GE); }
LEQ             { yylval.intval = LEQ; return(LEQ); }
GEQ             { yylval.intval = GEQ; return(GEQ); }
{ID}            { yylval.intval = str_to_id_int(yytext); return(PIDENTIFIER); }
[ \t\n]+
.               { return yytext[0]; }
%%

int yywrap(void){}