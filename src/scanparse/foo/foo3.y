%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "internal_lib.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

%}

%union {
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cflt;
 binop               cbinop;
 node               *node;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON BRACE_L BRACE_R
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET VOID INT

%token  NUM
%token  FLOAT
%token  ID


%start program

%%

program: decls fundefs
         ;

decls: decl decls
| ;

decl: type ID SEMICOLON;

fundefs: fundefs fundef | ;

fundef: funheader BRACE_L BRACE_R;

funheader: type ID  BRACKET_L BRACKET_R
| VOID ID  BRACKET_L BRACKET_R;

type: INT | FLOAT;



      
%%

static int yyerror( char *error)
{
  CTIabort( "Error parsing source code. The error was: %s\n", error);

  return( 0);
}

node *YYparseTree()
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}
