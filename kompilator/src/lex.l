%{
    #include <string.h>
    #include "codegen.h"
    #include "gram.tab.h"

    i64 size = 2147483647;
    int maxSize = 1000;
    i64 currentIndex = 0;

    i64 indexMap[1000] = {0};
    char *strs[1000] = {0};

    i64 str_to_id_int(char *yytext) {
        for(i64 i = 0; i < maxSize; i++) {
            if (strs[i] == NULL) {
                strs[i] = strdup(yytext);
                // printf("%d: %s\n", i, strs[i]);
                indexMap[i] = currentIndex;
                currentIndex++;
                return indexMap[i];
            } else if (strcmp(strs[i], yytext) == 0) {
                // printf("%d: %s\n", i, strs[i]);
                return indexMap[i];
            }
        }
        return -1;
    }

    int pushStrs() {
        for(i64 i = 0; i < size; i++) {
            if (strs[i] == NULL) {
                strs[i+1] = "ABCiteratorXYZ"; 
                currentIndex++;
                return 0;
            }
        }
        return 0;
    }
    
    i64 arrayDeclare(char *yytext) {
        char* name = strtok(yytext, "[");
        // printf("name: %s\n", name);
        int start = atoi(strtok(NULL, ":"));
        // printf("start: %d\n", start);
        int end = atoi(strtok(NULL, "]"));
        // printf("end: %d\n", end);

        if (end < start) {
            printf("ERROR: niepoprawny zakres tablicy\n");
            exit(1);
        }

        /* printf("xxx%sxxx\n", yytext); */
        
        for(i64 i = 0; i < size; i++) {
            if (strs[i] == NULL) {
                // nie znaleziono
                strs[i] = strdup(yytext);
                indexMap[i] = currentIndex;
                currentIndex++;

                // strs[fi]

                strs[i+1] = strdup(yytext);
                indexMap[i+1] = currentIndex;
                currentIndex++;

                for (i64 j = i+2; j < (i + 2 + end - start + 1); j++) {
                    currentIndex++;
                }
                // printf("%d: %s\n", i, strs[i]);
                initArray(indexMap[i], start, end);
                return i;
            } else if (strcmp(strs[i], yytext) == 0) {
                return i;
            }
        }
        return -1;
    }
    void printall() {
        for(i64 i = 0; i < size; i++) {
            if (strs[i] == NULL) {
                break;
            } else {
                printf("%s %lld\n", strs[i], indexMap[i]);
            }

        }
    }
%}

DIGIT [0-9]
ID [a-z]*

%%
"("[^()]*")"        {}
-{DIGIT}+        { yylval.intval = atoll(yytext); return(NUM); }
{DIGIT}+        { yylval.intval = atoll(yytext); return(NUM); printf("%s", yytext); }
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
FOR             { pushStrs(); return(FOR); }
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
{ID}" "?"["" "?"-"?{DIGIT}+" "?":"" "?"-"?{DIGIT}+" "?"]"           { yylval.intval = arrayDeclare(yytext); return(PIDENTIFIER);  }
{ID}            { yylval.intval = str_to_id_int(yytext); return(PIDENTIFIER); }
[ \t\n]+
.               { return yytext[0]; }
%%

int yywrap(void){}
// {ID}"["{DIGIT}+"]"                      { yylval.intval = arrayDeclare(yytext); return(PIDENTIFIER); printf("TABLICA [num]\n"); }
// {ID}"["{ID}"]"                          { yylval.intval = arrayDeclare(yytext); return(PIDENTIFIER); printf("TABLICA [id]\n"); }