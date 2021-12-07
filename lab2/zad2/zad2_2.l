%{
    /*Definition section */
    #include "header.h"
    #include <string.h>
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

%x COMMENT
%x STR
%x CDATA

%%
\<!--           BEGIN(COMMENT);
<COMMENT>{
    .*(-->.*<!--)+.*-->   {BEGIN(INITIAL);handleMultiple(yytext);}
    .*(-->)+.*-->   {BEGIN(INITIAL);handleMultiple(yytext);}
    .*--.*-->           {fprintf(yyout, "%s", "<!--");
                            ECHO;
                            BEGIN(INITIAL);
                            printf("invalid comment\n");
                            printf("%s\n", yytext);}
    .*--->              {fprintf(yyout, "%s", "<!--");
                            ECHO;
                            BEGIN(INITIAL);
                            printf("132 invalid comment\n");}
    .*-->           {BEGIN(INITIAL); printf("valid comment\n");}
}

\"              {ECHO;BEGIN(STR);}
<STR>{
    [^\""]      ECHO;
    \"          {ECHO;BEGIN(INITIAL);}
}

\<!\[CDATA      {ECHO;BEGIN(CDATA);}
<CDATA>{
    \n|.        ECHO;
    ]>          {ECHO; BEGIN(INITIAL);}
}
%%

int handleMultiple(char* text) {
    for (int i = 2; i < strlen(text); i++) {
        if (text[i-2]=='-' && text[i-1]=='-' && text[i]=='>') {
            char temp[100] = "";
            i++;
            int j = i;
            for (i; j < strlen(text); j++) {
                // printf("i: %d; j: %d; dodaje %c do temp %s\n",i, j, text[i], temp);
                if (text[j-3]=='<' && text[j-2]=='!' && text[j-1]=='-' && text[j]=='-') {
                    // printf("STOP\n");
                    temp[j-i-3] = '\0';
                    break;
                }
                else {
                    temp[j-i] = text[j];
                }
            }
            i = j;
            fprintf(yyout, "%s", temp);
        }
    }
}

int yywrap(){}
 
// driver code 
int main(int argc, char *argv[])
{
  

    yyin = fopen("input.txt", "r");
  
    yyout = fopen("Output.txt", "w");
  
    yylex();
    return 0;
}

