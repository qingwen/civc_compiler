%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

%}

%union {
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cfloat;
 binop               cbinop;
 monop               cmonop;
 node               *node;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET


%token NEG EXTERN EXPORT
%token IF RETURN FOR ELSE WHILE DO
%token TYPE_FLOAT TYPE_BOOL TYPE_INT TYPE_VOID
%token BRACE_L BRACE_R SQBR_L SQBR_R 

%token <cint> NUM
%token <cfloat> FLOAT
%token <id> ID

%type <node> intval floatval boolval constant expr exprs
%type <node> instrs instr program declares declare 
%type <node> fundec fundef fundefs globaldec globaldef funbody
%type <node> funheader block cond assign
%type <node> rettype basictype type id ids param paramscalar paramarray params vardecs vardec return
%type <node> doloop whileloop forloop optelse
%type <cbinop> binop

%type <cmonop> monop

%start program

%%

program: declares
     { 
       $$ = $1;
     }
   ;

declares: declare declares
        {
          $$ = TBmakeDeclares( $1, $2);
        }
      | declare
        {
          $$ = TBmakeDeclares( $1, NULL);
        }
        ;

declare: fundec
        {
          $$ = TBmakeDeclares( $1, NULL);
        }
      | fundef
        {
          $$ = TBmakeDeclares( $1, NULL);
        }
      | globaldef
        {
          $$ = TBmakeDeclares( $1, NULL);
        }
      | EXPORT globaldef
        {
          $$ = TBmakeDeclares( $2, NULL);
        }
        ;

fundec: EXTERN funheader SEMICOLON
        {
          $$ = TBmakeFunDec( $2);
        }
        ;

fundef: funheader BRACE_L funbody BRACE_R
        {
          $$ = TBmakeFunDef($1, $3);
        }
      | EXPORT funheader BRACE_L funbody BRACE_R
        {
          $$ = TBmakeFunDef($2, $4);
        }
        ;

globaldec: type id SEMICOLON
        {
          $$ = TBmakeGlobalDec( $1, $2);
        }
        ;

globaldef: type id LET expr SEMICOLON
        {
          $$ = TBmakeGlobalDef( $1, $2, $4);
        }
        ;

funheader: rettype id BRACKET_L params BRACKET_R 
        {
          $$ = TBmakeFunHeader( $1, $2, $4);
        }
        ;

rettype:   basictype 
        {
          $$ = $1;
        }
        | TYPE_VOID   { $$ = NULL; }
        ;

type:   basictype 
        {
          $$ = $1;
        }
        ;

basictype: FLOAT
           {
             $$ = TBmakeFloat( $1);
           }
          | NUM
           {
             $$ = TBmakeNum( $1);
           }
          | TRUEVAL
           {
             $$ = TBmakeBool( TRUE);
           }
          | FALSEVAL
           {
             $$ = TBmakeBool( FALSE);
           }
       ;


funbody: vardecs fundefs instrs return 
        {
          $$ = TBmakeFunBody( $1, $2,$3, $4);
        }
        ;

fundefs: fundef fundefs 
        {
          $$ = TBmakeFunDefs( $1, $2);
        }
        | fundef
        {
          $$ = TBmakeFunDefs( $1, NULL);
        }
        ;
        ;

vardecs: vardec vardecs
        {
          $$ = TBmakeVarDecs( $1, $2);
        }
      | vardec
        {
          $$ = TBmakeVarDecs( $1, NULL);
        }
        ;

vardec: type id LET expr SEMICOLON
        {
          $$ = TBmakeVarDec( $1, $2,$4);
        }
        ;

instrs: instr instrs
        {
          $$ = TBmakeInstrs( $1, $2);
        }
      | instr
        {
          $$ = TBmakeInstrs( $1, NULL);
        }
        ;

params: param COMMA params
        {
          $$ = TBmakeParams( $1, $3);
        }
      | param
        {
          $$ = TBmakeParams( $1, NULL);
        }
       ;

param: paramarray
       {
          $$ = TBmakeParams( $1, NULL);
       }
      | paramscalar
       {
          $$ = TBmakeParams( $1, NULL);
       }
       ;

paramarray: type SQBR_L ids SQBR_R id
        {
          $$ = TBmakeParamArray($1, $3, $5);
        }
        ;

paramscalar: type id
        {
          $$ = TBmakeParamScalar($1, $2);
        }
        ;

return: RETURN expr
        {
          $$ = TBmakeReturn( $2);
        }
        ;

exprs: expr COMMA exprs
        {
          $$ = TBmakExprs( $1, $3);
        }
      | expr
        {
          $$ = TBmakeExprs( $1, NULL);
        }
       ;

instr: assign SEMICOLON     { $$ = TBmakeInstrs( $1, NULL); }
      | cond                { $$ = TBmakeInstrs( $1, NULL); }
      | doloop SEMICOLON    { $$ = TBmakeInstrs( $1, NULL); }
      | whileloop           { $$ = TBmakeInstrs( $1, NULL); }
      | forloop             { $$ = $1; /* forloop already produces assign node. */}
      ;

assign:    id LET { $<cint>$ = global.line; } expr
           { $$ = TBmakeLet( $1, $4);
             NODE_LINE( $$) = $<cint>3;
           }
         ;

cond: IF { $<cint>$ = global.line; } BRACKET_L expr BRACKET_R block optelse
      {
        $$ = TBmakeCond( $4, $6, $7);
        NODE_LINE( $$) = $<cint>2;
      }
      ;

optelse: ELSE block           
         {      $$ = $2;                 }
        ;


doloop: DO { $<cint>$ = global.line; } block
        WHILE BRACKET_L expr BRACKET_R 
        {
          $$ = TBmakeDowhile( $6, $3);
          NODE_LINE( $$) = $<cint>2;
        }
      ;

whileloop: WHILE { $<cint>$ = global.line; } BRACKET_L expr BRACKET_R
           block
           {
             $$ = TBmakeWhile( $4, $6);
             NODE_LINE( $$) = $<cint>2;
           }
         ;

forloop:   FOR { $<cint>$ = global.line; }
           BRACKET_L assign COMMA expr COMMA expr BRACKET_R block
           { 
           }
         ;

block:     BRACE_L { $<cint>$ = global.linenum; }  instrs BRACE_R
             { if ($3 == NULL) {
                 $$ = MAKE_EMPTY_BLOCK();
               }
               else {
                 $$ = TBmakeBlock( $3, NULL);
                 NODE_LINE( $$) = $<cint>2;
               }
             }
           | instr
             { $$ = TBmakeBlock( $1, NULL);
             }
           ;

id:  ID     
     { 
       $$ = TBmakeId( STRcpy( $1));
     } 
     ; 

ids: id COMMA ids
     { 
       $$ = TBmakeids( $1, $3);
     }
   | id
     { $$ = TBmakeids( $1, NULL);
     }
   ;

expr: constant
      {
        $$ = $1;
      }
    | ID
      {
        $$ = TBmakeId( STRcpy( $1));
      }
    | BRACKET_L expr binop expr BRACKET_R
      {
        $$ = TBmakeBinop( $3, $2, $4);
      }
    ;

constant: floatval
          {
            $$ = $1;
          }
        | intval
          {
            $$ = $1;
          }
        | boolval
          {
            $$ = $1;
          }
        ;

floatval: FLOAT
           {
             $$ = TBmakeFloat( $1);
           }
         ;

intval: NUM
        {
          $$ = TBmakeNum( $1);
        }
      ;

boolval: TRUEVAL
         {
           $$ = TBmakeBool( TRUE);
         }
       | FALSEVAL
         {
           $$ = TBmakeBool( FALSE);
         }
       ;

binop: PLUS      { $$ = BO_add; }
     | MINUS     { $$ = BO_sub; }
     | STAR      { $$ = BO_mul; }
     | SLASH     { $$ = BO_div; }
     | PERCENT   { $$ = BO_mod; }
     | LE        { $$ = BO_le; }
     | LT        { $$ = BO_lt; }
     | GE        { $$ = BO_ge; }
     | GT        { $$ = BO_gt; }
     | EQ        { $$ = BO_eq; }
     | OR        { $$ = BO_or; }
     | AND       { $$ = BO_and; }
     ;

monop: NEG      { $$ = MO_neg; }
     ;

%%

static int yyerror( char *error)
{
  CTIabort( "line %d, col %d\nError parsing source code: %s\n", 
            global.line, global.col, error);

  return( 0);
}

node *YYparseTree()
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

