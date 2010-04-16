%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civc.tab.h"
#include "globals.h"
#include "ctinfo.h"


#define FILTER(token) \
  charpos += yyleng;  \
  if (commlevel == 0) { return( token); }

#define LINEBUF_SIZE 256
#define YY_NO_UNPUT 1

int start_token;
char linebuf[ LINEBUF_SIZE];
char *tmp_buf = NULL;

char *linebuf_ptr = NULL;
int commlevel = 0;
int charpos = 0;

%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%option noyywrap


%%

"{"                        { FILTER( BRACE_L); }
"}"                        { FILTER( BRACE_R); }
"["                        { FILTER( SQBR_L); }
"]"                        { FILTER( SQBR_R); }


"("                        { FILTER( BRACKET_L); }
")"                        { FILTER( BRACKET_R); }
","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( STAR);  } 
"/"                        { FILTER( SLASH);  } 
"%"                        { FILTER( PERCENT);  } 

"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    } 
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }

"!"                        { FILTER( NOT);    }

"="                        { FILTER( LET);    }

"true"                     { FILTER( TRUEVAL); }
"false"                    { FILTER( FALSEVAL); }

"void"                     { FILTER( TYPE_VOID); }

"extern"                   { FILTER( EXTERN); }
"export"                   { FILTER( EXPORT); }

"return"                   { FILTER( RETURN); }
"if"                       { FILTER( IF); }
"else"                     { FILTER( ELSE); }
"do"                       { FILTER( DO); }
"while"                    { FILTER( WHILE); }
"for"                      { FILTER( FOR); }


"float"                    { FILTER( TYPE_FLOAT); }
"bool"                     { FILTER( TYPE_BOOL); }
"int"                      { FILTER( TYPE_INT); }


[_A-Za-z][_A-Za-z0-9_]*      { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }


[0-9]+([eE][+-]?[0-9]+)?   { yylval.cint=atoi(yytext);
                             FILTER( NUM);
                           }

[0-9]*"."[0-9]+([eE][+-]?[0-9]+)? {
    
                             yylval.cflt=atof(yytext);
                             FILTER( FLOAT);
                           }
[0-9]+[.][0-9]*([eE][+-]?[0-9]+)? {
                             yylval.cflt=atof(yytext);
                             FILTER( FLOAT);
                           }




"/*"(.|\n)*"*/"            {
                             for (int i = 0; i < yyleng; i++) {
                                 if(yytext[i] == '\n'){
                                     global.line ++;
                                     global.col = 1;
                                 }
                                 else 
                                     global.col++;
                             }
                           }

"//".*                     {
                             //global.col += yyleng;
                           }

\n.*                       { global.line += 1;
                             global.col = 1;
                             yyless(1);
                           }


[ \t]                      { global.col += yyleng;
                           } 

