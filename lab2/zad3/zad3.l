%{
    /*Definition section */
    #include "header.h"
    int leaveDoc = 0;
    int inString = 0;
    int singleLineComment = 0;      // // 
    int multipleLineComment = 0;    // /* */
    int docSingleLineComment = 0;   // ///    or //!
    int docmultipleLineComment = 0; // /** */ or /*! */
%}


%%
"\""|"<"|">"            {stringHandler(yytext);}
"///"|"//!"|"/\\\n//"|"//\\\n/"|"/\\\n/!"|"//\\\n!"    {docSingleLineCommentHandler(1, yytext);}
"/**"|"/*!"|"/\\\n**"|"/*\\\n*"|"/\\\n*!"|"/*\\\n!"    {docmultipleLineCommentHandler(1, yytext);}
"/*"|"/\\\n*"           {multipleLineCommentHandler(1, yytext);}
"*/"|"*\\\n*"           {docmultipleLineCommentHandler(0, yytext); multipleLineCommentHandler(0, "");}
"//"|"/\\\n/"           {singleLineCommentHandler(1, yytext);}
\\\n                    {}
\n                      {singleLineCommentHandler(0, yytext); docSingleLineCommentHandler(0, "");everythingElseHandler(yytext);}
.                       {everythingElseHandler(yytext);}
%%
  
int yywrap(){}


int everythingElseHandler(char* text) {
    if (leaveDoc) {
        if (!singleLineComment && !multipleLineComment) {
            fprintf(yyout, "%s", text);
        }
    }
    else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
        fprintf(yyout, "%s", text);
    }    
}
int stringHandler(char* text) {
    if (leaveDoc) {
        if (!singleLineComment && !multipleLineComment && !inString) {
            if (!docSingleLineComment && !docmultipleLineComment) {
                inString = 1;
            }
            fprintf(yyout, "%s", text);
        }
        else if (!singleLineComment && !multipleLineComment && inString) {
            if (!docSingleLineComment && !docmultipleLineComment) {
                inString = 0;
            }
            fprintf(yyout, "%s", text);
        }
    }
    else {
        if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment && !inString) {
            inString = 1;
            fprintf(yyout, "%s", text);
        }
        else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment && inString) {
            inString = 0;
            fprintf(yyout, "%s", text);
        }
    }
}
int singleLineCommentHandler(int begin, char* text) {
    if (inString) {
        fprintf(yyout, "%s", text);
    }
    else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
        if (begin == 1) {
            singleLineComment = 1;
        }
    }
    else if (singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
        if (begin == 0) {
            singleLineComment = 0;
        }
    }            
}
int multipleLineCommentHandler(int begin, char* text) {
    if (inString) {
        fprintf(yyout, "%s", text);
    }
    else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
        if (begin == 1) {
            multipleLineComment = 1;
        }
    }
    else if (!singleLineComment && multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
        if (begin == 0) {
            multipleLineComment = 0;
        }
    }
}
int docSingleLineCommentHandler(int begin, char* text) {
    if (!leaveDoc) {
        if (inString) {
            fprintf(yyout, "%s", text);
        }
        else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
            if (begin == 1) {
                docSingleLineComment = 1;
            }
        }
        else if (!singleLineComment && !multipleLineComment && docSingleLineComment && !docmultipleLineComment) {
            if (begin == 0) {
                docSingleLineComment = 0;
            }
        }
    }
    else {
        if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
            fprintf(yyout, "%s", text);
            if (begin == 1) {
                docSingleLineComment = 1;
            }
        }
        else if (!singleLineComment && !multipleLineComment && docSingleLineComment && !docmultipleLineComment) {
            fprintf(yyout, "%s", text);
            if (begin == 0) {
                docSingleLineComment = 0;
            }
        }
    }
}
int docmultipleLineCommentHandler(int begin, char* text) {
    if (!leaveDoc) {
        if (inString) {
            fprintf(yyout, "%s", text);
        }
        else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
            if (begin == 1) {
                docmultipleLineComment = 1;
            }
        }
        else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && docmultipleLineComment) {
            if (begin == 0) {
                docmultipleLineComment = 0;
            }
        }
    }
    else {
        if (!singleLineComment && !multipleLineComment && !docSingleLineComment && !docmultipleLineComment) {
            fprintf(yyout, "%s", text);
            if (begin == 1) {
                docmultipleLineComment = 1;
            }
        }
        else if (!singleLineComment && !multipleLineComment && !docSingleLineComment && docmultipleLineComment) {
            fprintf(yyout, "%s", text);
            if (begin == 0) {
                docmultipleLineComment = 0;
            }
        }    
    }
}

// driver code 
int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (!strcmp(argv[1], "-doc")) {
            leaveDoc = 1;
        }
    }
    
    yyin = fopen("input.txt", "r");
  
    yyout = fopen("Output.txt", "w");
  
    yylex();
    return 0;
}