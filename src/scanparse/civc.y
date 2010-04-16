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
 float               cflt;
 binop               cbinop;
 monop               cmonop;
 node               *node;
 type               *type;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET


%token NOT NEG EXTERN EXPORT
%token IF RETURN FOR ELSE WHILE DO
%token TYPE_FLOAT TYPE_BOOL TYPE_INT TYPE_VOID
%token BRACE_L BRACE_R SQBR_L SQBR_R GLOBAL


        
%type <type> basictype


%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

%type <node> intval floatval boolval constant expr params
%type <node> block instr instrs assign varlet var return
%type <node> fundef fundecl globaldecl globaldef
%type <node> module decl decls
%type <node> expr_fun_call opt_arguments vardecs 
%type <node> forloop whileloop cond doloop optelse fundef1 fundef2 globaldef1 globaldef2
%type <cmonop> monop

%left OR
%left AND
%left EQ NE
%left LE LT GE GT
%left PLUS MINUS
%left PERCENT STAR SLASH

%left PREC_6th
%left PREC_5th
%left PREC_4th
%left PREC_3rd
%left PREC_2nd
%left PREC_1st
%right MM_OP CAST
        
%right SQBR_L BRACKET_L
%right ELSE 
%right GLOBAL

%start module

%%

module: decls
       {
         parseresult = $1;
       };


decls: decl decls
       {
        $$ = TBmakeModuledecls($1, $2);
        NODE_LINE($$) = global.line;
       }
       | decl
       {
        $$ = TBmakeModuledecls($1, NULL);
        NODE_LINE($$) = global.line;
       }
       ;

decl: globaldecl  
      {
       $$ = $1;
      }
     | fundecl 
      {
       $$ = $1;
      }
     | globaldef  
      {
       $$ = $1;
      }
     | fundef   
      {
       $$ = $1;
      }
     ;



globaldecl: EXTERN basictype ID SEMICOLON
       {
         node* varinfo = TBmakeVarinfo ((type)$2, $3, NULL);
         $$ = TBmakeGlobaldef(varinfo, NULL, NULL);
         GLOBALDEF_ISEXTERN ( $$) = TRUE;
         VARINFO_DECL(varinfo) = $$;
         NODE_LINE($$) = global.line;
       }
      ;


globaldef: EXPORT globaldef1
            { 
              $$ = $2;
              GLOBALDEF_ISEXPORT ( $$) = TRUE;
            }
       | globaldef1
       {
         $$ = $1;
       }
       ;

globaldef1: basictype globaldef2
           {
             VARINFO_TYPE(GLOBALDEF_VARINFO($2)) = (type)$1; 
             $$ = $2;
           }
          ;

globaldef2:  ID LET expr SEMICOLON
       {
         node* varinfo = TBmakeVarinfo ((type)T_unknown, $1, NULL);
         $$ = TBmakeGlobaldef(varinfo, $3, NULL);
         VARINFO_DECL(varinfo) = $$;
         NODE_LINE( $$ ) = global.line;
       }
      | ID SEMICOLON
       {
         node* varinfo = TBmakeVarinfo ((type)T_unknown, $1, NULL);
         $$ = TBmakeGlobaldef(varinfo, NULL, NULL);
         VARINFO_DECL(varinfo) = $$;
         NODE_LINE( $$ ) = global.line;
       }
       ;


fundef: EXPORT fundef1 
        {
         $$ = $2;
         FUNDEF_ISEXPORT ($$) = TRUE;
        }
       | fundef1
       {
         $$ = $1;
       }
       ;


fundef1: basictype fundef2     
        {
          FUNDEF_RETTYPE($2) = (type)$1;
          $$ = $2;
        }
        | TYPE_VOID fundef2
        {
          FUNDEF_RETTYPE($2) = T_void;
          $$ = $2;
        }
        ;

fundef2:  ID BRACKET_L params BRACKET_R BRACE_L vardecs instrs BRACE_R
      {
        $$ = TBmakeFundef((type)T_unknown, $1, $3, NULL, $6, $7, NULL);
        NODE_LINE( $$ ) = global.line;
      }
     ;


fundecl:  EXTERN basictype ID BRACKET_L params BRACKET_R SEMICOLON
       {
         $$ = TBmakeFundef((type)$2, $3, $5, NULL, NULL, NULL, NULL);
         FUNDEF_ISEXTERN ( $$) = TRUE;
         NODE_LINE( $$ ) = global.line;
       } 
        | EXTERN TYPE_VOID ID BRACKET_L params BRACKET_R SEMICOLON
       {
         $$ = TBmakeFundef((type)T_void, $3, $5, NULL, NULL, NULL, NULL);
         FUNDEF_ISEXTERN ( $$) = TRUE;
         NODE_LINE( $$ ) = global.line;
       }   
      ;


params:  { 
           $$ = NULL;
         }
       | basictype ID COMMA params
        {
          node* varinfo = TBmakeVarinfo ((type)$1, $2, NULL);
          $$ = TBmakeParam(varinfo, $4);
          VARINFO_DECL(varinfo) = $$;
          NODE_LINE( $$ ) = global.line;
        }
      | basictype ID
        {
          node* varinfo = TBmakeVarinfo ((type)$1, $2, NULL);
          $$ = TBmakeParam(varinfo, NULL);
          VARINFO_DECL(varinfo) = $$;
          NODE_LINE( $$ ) = global.line;
        }
      ;


vardecs: { 
           $$ = NULL;
         }
       | basictype ID LET expr SEMICOLON vardecs
        {
          node* varinfo = TBmakeVarinfo ((type)$1, $2, NULL);
          $$ = TBmakeVardec(varinfo, $4, $6);
          VARINFO_DECL(varinfo) = $$;
          NODE_LINE( $$ ) = global.line;
        }
      | basictype ID SEMICOLON vardecs
        {
          node* varinfo = TBmakeVarinfo ((type)$1, $2, NULL);
          $$ = TBmakeVardec(varinfo, NULL, $4);
          VARINFO_DECL(varinfo) = $$;
          NODE_LINE( $$ ) = global.line;
        }
      ;

instrs: instr
         { 
           $$ = TBmakeInstrs( $1, NULL);
           NODE_LINE( $$ ) = global.line;
         }
       | instr instrs
         { 
           $$ = TBmakeInstrs( $1, $2);
           NODE_LINE( $$ ) = global.line;
         }
       ;


instr: assign               { $$ = $1; }
      | cond                { $$ = $1; }
      | doloop SEMICOLON    { $$ = $1; }
      | whileloop           { $$ = $1; }
      | forloop             { $$ = $1; }
      | return              { $$ = $1; }

      ;


return: RETURN expr SEMICOLON
        {
          $$ = TBmakeReturn($2);
          NODE_LINE( $$ ) = global.line;
        }
       ;


assign:    varlet LET { $<cint>$ = global.line; } expr SEMICOLON
           { $$ = TBmakeAssign( $1, $4);
             NODE_LINE( $$) = $<cint>3;
           }
          | expr_fun_call SEMICOLON
            { 
              $$ = TBmakeAssign(NULL, $1);
              NODE_LINE( $$) = global.line;
            }
          ;


cond: IF { $<cint>$ = global.line; } BRACKET_L expr BRACKET_R block optelse
      {
        $$ = TBmakeIf( $4, $6, $7);
        NODE_LINE( $$) = $<cint>2;
      }
      ;


optelse: ELSE block
         {
           $$ = $2;
         }
        | /* empty */   %prec ELSE  { $$ = NULL;}
         ;


doloop: DO { $<cint>$ = global.line; } block WHILE BRACKET_L expr BRACKET_R 
        {
          $$ = TBmakeDowhile( $6, $3);
          NODE_LINE( $$) = $<cint>2;
        }
        ;


whileloop: WHILE { $<cint>$ = global.line; } BRACKET_L expr BRACKET_R block
           {
             $$ = TBmakeWhile( $4, $6);
             NODE_LINE( $$) = $<cint>2;
           }
           ;


forloop:   FOR BRACKET_L TYPE_INT ID LET expr COMMA expr COMMA expr BRACKET_R block
           { 
             node* varinfo = TBmakeVarinfo(TYPE_INT, $4, NULL);
             $$ = TBmakeFor(varinfo, $6, $8, $10, $12);
             VARINFO_DECL(varinfo) = $$;
             NODE_LINE( $$ ) = global.line;
           }  
         | FOR BRACKET_L TYPE_INT ID LET expr COMMA expr BRACKET_R block
           {
             node* varinfo = TBmakeVarinfo(TYPE_INT, $4, NULL);
             $$ = TBmakeFor(varinfo, $6, $8, NULL, $10);
             VARINFO_DECL(varinfo) = $$;
             NODE_LINE( $$ ) = global.line;
           }
           ;


block:     BRACE_L { $<cint>$ = global.line; } instrs BRACE_R
             { 
               $$ = $3;
               NODE_LINE( $$) = $<cint>2;
             }
           | instr
             {
               $$ = TBmakeInstrs($1, NULL);
               NODE_LINE( $$) = global.line;
             }
           ;


varlet: ID 
        {
          $$ = TBmakeVarlet( $1, NULL, 0);
          NODE_LINE( $$) = global.line;
        }
        ;


var: ID
        {
          $$ = TBmakeVar( $1, NULL, 0);
          NODE_LINE( $$) = global.line;
        }
        ;


expr: constant
      {
        $$ = $1;
      }
    | var
      {
        $$ = $1;
      }
    | expr OR expr   { $$ = TBmakeIf( BO_or, $1, $3); NODE_LINE($$) = global.line; }
    | expr AND expr   { $$ = TBmakeBinop( BO_and, $1, $3); NODE_LINE($$) = global.line; }
    | expr EQ expr   { $$ = TBmakeBinop( BO_eq, $1, $3); NODE_LINE($$) = global.line; }
    | expr NE expr   { $$ = TBmakeBinop( BO_ne, $1, $3); NODE_LINE($$) = global.line; }
    | expr LE expr   { $$ = TBmakeBinop( BO_le, $1, $3); NODE_LINE($$) = global.line; }
    | expr LT expr   { $$ = TBmakeBinop( BO_lt, $1, $3); NODE_LINE($$) = global.line; }
    | expr GE expr   { $$ = TBmakeBinop( BO_ge, $1, $3); NODE_LINE($$) = global.line; }
    | expr GT expr   { $$ = TBmakeBinop( BO_gt, $1, $3); NODE_LINE($$) = global.line; }
    | expr PLUS expr   { $$ = TBmakeBinop( BO_add, $1, $3); NODE_LINE($$) = global.line; }
    | expr MINUS expr   { $$ = TBmakeBinop( BO_sub, $1, $3); NODE_LINE($$) = global.line; }
    | expr PERCENT expr   { $$ = TBmakeBinop( BO_mod, $1, $3); NODE_LINE($$) = global.line; }
    | expr STAR expr   { $$ = TBmakeBinop( BO_mul, $1, $3); NODE_LINE($$) = global.line; }
    | expr SLASH expr   { $$ = TBmakeBinop( BO_div, $1, $3); NODE_LINE($$) = global.line; }

    | monop expr %prec MM_OP
      { 
        $$ = TBmakeMonop( $1, $2);
        NODE_LINE($$) = global.line; 
      }
    | BRACKET_L expr BRACKET_R      /* for (expr) */
      {  
        $$ = $2;
      }
    | expr_fun_call                /* for function calls */
      { 
        $$ = $1;
      }
    | BRACKET_L basictype BRACKET_R expr %prec CAST /* for cast operation */
      { 
        $$ = TBmakeCast((type)$2, $4);
        NODE_LINE($$) = global.line; 
      }
    ;

expr_fun_call: 
         ID BRACKET_L { $<cint>$ = global.line; } opt_arguments BRACKET_R
         {
           $$ = TBmakeFunap( $1,NULL,0,$4);
           NODE_LINE( $$) = $<cint>3;
         }
        | ID BRACKET_L { $<cint>$ = global.line; }  BRACKET_R
         {
           $$ = TBmakeFunap( $1,NULL,0,NULL);
           NODE_LINE( $$) = $<cint>3;
         }
         ;

opt_arguments: expr COMMA opt_arguments
              {
                 $$ = TBmakeArg($1, $3);
                 NODE_LINE( $$) = global.line;
              }
             | expr
              {
                 $$ = TBmakeArg($1, NULL);
                 NODE_LINE( $$) = global.line;
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
             NODE_LINE( $$) = global.line;
           }
         ;


basictype: TYPE_FLOAT         { $$ = T_float;       }
           | TYPE_BOOL        { $$ = T_bool;        }
           | TYPE_INT         { $$ = T_int;         }
          ;


intval: NUM
        {
          $$ = TBmakeNum( $1);
          NODE_LINE( $$) = global.line;
        }
       ;


boolval: TRUEVAL
         {
           $$ = TBmakeBool( TRUE);
           NODE_LINE( $$) = global.line;
         }
       | FALSEVAL
         {
           $$ = TBmakeBool( FALSE);
           NODE_LINE( $$) = global.line;
         }
       ;


monop: NOT       { $$ = MO_not; }
     | MINUS     { $$ = MO_neg; }
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
