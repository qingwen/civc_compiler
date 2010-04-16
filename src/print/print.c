
/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */


#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include <string.h>


/*
 * INFO structure
 */
struct INFO {
  bool firsterror;
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)

static int indent = 0;

static inline void printNewLineWithIndent(int indent)
{
    printf("\n");
    for (int i = 0; i < indent; i++)
        printf("    ");
}

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_FIRSTERROR(result) = FALSE;
  
  return result;
}


static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}



/** <!--******************************************************************-->
 *
 * @fn PRTinstr
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTinstrs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTinstrs");

  if (INSTRS_INSTR( arg_node) != NULL) {
    INSTRS_INSTR( arg_node) = TRAVdo( INSTRS_INSTR( arg_node), arg_info);
  }
  
  if (INSTRS_NEXT( arg_node) != NULL) {
    INSTRS_NEXT( arg_node) = TRAVopt( INSTRS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTinstr
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTassign (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTassign");

  if (ASSIGN_LET( arg_node) != NULL) {
    ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
    printf( " = ");
  }

  if (ASSIGN_EXPR( arg_node) != NULL)
      ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

  printf( ";");
  printNewLineWithIndent(indent);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbinop (node * arg_node, info * arg_info)
{
  char *tmp;

  DBUG_ENTER ("PRTbinop");
  
  printf("( ");
  
  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

  switch (BINOP_OP( arg_node)) {
    case BO_add:
      tmp = "+";
      break;
    case BO_sub:
      tmp = "-";
      break;
    case BO_mul:
      tmp = "*";
      break;
    case BO_div:
      tmp = "/";
      break;
    case BO_mod:
      tmp = "%";
      break;
    case BO_lt:
      tmp = "<";
      break;
    case BO_le:
      tmp = "<=";
      break;
    case BO_gt:
      tmp = ">";
      break;
    case BO_ge:
      tmp = ">=";
      break;
    case BO_eq:
      tmp = "==";
      break;
    case BO_ne:
      tmp = "!=";
      break;
    case BO_or:
      tmp = "||";
      break;
    case BO_and:
      tmp = "&&";
      break;
    case BO_let:
      tmp = "=";
      break;
    case BO_unknown:
      DBUG_ASSERT( 0, "unknown binop detected!");
      break;
  }

  printf( " %s ", tmp);

  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
  
  printf(" )");

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node monop
 ***************************************************************************/

node *
PRTmonop(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTmonop");
  
  printf("( ");

  char *tmp = NULL;

  switch (MONOP_OP(arg_node)) {
      case MO_neg:
          tmp = "-";
          break;
      case MO_not:
          tmp = "!";
          break;
      case MO_unknown:
          DBUG_ASSERT(0, "unknown monop detected!");
          break;
  }

  printf("%s", tmp);

  if(MONOP_ARG(arg_node) != NULL)
      MONOP_ARG(arg_node) = TRAVdo( MONOP_ARG(arg_node), arg_info);
      
  printf(" )");
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfloat (node * arg_node, info * arg_info)
{
  float num;

  DBUG_ENTER ("PRTfloat");

  num = FLOAT_VALUE( arg_node);

  printf( "%f", num);
  DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn PRTnum
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Num node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTnum (node * arg_node, info * arg_info)
{
  int num;

  DBUG_ENTER ("PRTnum");

  num = NUM_VALUE( arg_node);

  printf( "%d", num);
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbool (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbool");

  if (BOOL_VALUE( arg_node)) {
    printf( "true");
  }
  else {
    printf( "false");
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvar (node * arg_node, info * arg_info)
{
  char *tmp;
  DBUG_ENTER ("PRTvar");

  tmp = VAR_NAME( arg_node);
  printf( "%s", tmp);

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvarlet
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvarlet (node * arg_node, info * arg_info)
{
  char *tmp;
  DBUG_ENTER ("PRTvarlet");

  tmp = VARLET_NAME( arg_node);
  printf( "%s", tmp);

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTerror (node * arg_node, info * arg_info)
{
  bool first_error;

  DBUG_ENTER ("PRTerror");

  if (NODE_ERROR (arg_node) != NULL) {
    NODE_ERROR (arg_node) = TRAVdo (NODE_ERROR (arg_node), arg_info);
  }

  first_error = INFO_FIRSTERROR( arg_info);

  if( (global.outfile != NULL)
      && (ERROR_ANYPHASE( arg_node) == global.compiler_anyphase)) {

    if ( first_error) {
      printf ( "\n/******* BEGIN TREE CORRUPTION ********\n");
      INFO_FIRSTERROR( arg_info) = FALSE;
    }

    printf ( "%s\n", ERROR_MESSAGE( arg_node));

    if (ERROR_NEXT (arg_node) != NULL) {
      TRAVopt (ERROR_NEXT (arg_node), arg_info);
    }

    if ( first_error) {
      printf ( "********  END TREE CORRUPTION  *******/\n");
      INFO_FIRSTERROR( arg_info) = TRUE;
    }
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node fundef
 ***************************************************************************/

node *
PRTfundef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTfundef");

    printNewLineWithIndent(indent);

    if (FUNDEF_ISEXTERN(arg_node) == TRUE)
    {
        printf( "extern ");
    }

    if (FUNDEF_ISEXPORT(arg_node) == TRUE)
    {
        printf( "export ");
    }

    switch (FUNDEF_RETTYPE( arg_node)) {
    case T_int:
        printf("int ");
        break;
    case T_float:
        printf("float ");
        break;
    case T_bool:
        printf("bool ");;
        break;
    case T_void:
        printf("void ");
        break;
    case T_unknown:
        //  DBUG_ASSERT( 0, "unknown variable type detected!");
        break;
    }

    printf(" %s", FUNDEF_NAME( arg_node));   //function name

    printf("(");
    if (FUNDEF_PARAMS( arg_node) != NULL) {
        FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
    }
    printf(") ");

    if ( (FUNDEF_LOCALFUNS ( arg_node) == NULL)  && (FUNDEF_VARDECS ( arg_node) == NULL) && (FUNDEF_BODY ( arg_node) == NULL)) //function declare
    {
        printf(";\n ");
    }
    else  //function define
    {
        printf("{");
        indent++;
        printNewLineWithIndent(indent);
        if (FUNDEF_LOCALFUNS ( arg_node) != NULL) {
            FUNDEF_LOCALFUNS ( arg_node) = TRAVdo( FUNDEF_LOCALFUNS ( arg_node), arg_info);
        }

        if (FUNDEF_VARDECS( arg_node) != NULL) {
            FUNDEF_VARDECS( arg_node) = TRAVdo( FUNDEF_VARDECS( arg_node), arg_info);
        }

        if (FUNDEF_BODY( arg_node) != NULL) {
            FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
        }
        indent--;
        printNewLineWithIndent(indent);
        printf("}");
        printNewLineWithIndent(indent);
    }

    if (FUNDEF_NEXT( arg_node) != NULL) {
        FUNDEF_NEXT( arg_node) = TRAVdo( FUNDEF_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node globaldef
 ***************************************************************************/

node *
PRTglobaldef(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTglobaldef");
  
  if(GLOBALDEF_ISEXTERN(arg_node) == TRUE)
  {
    printf( "extern ");
  }
  
  if(GLOBALDEF_ISEXPORT(arg_node) == TRUE)
  {
    printf( "export ");
  }

  if (GLOBALDEF_VARINFO( arg_node) != NULL) {
    //traverse varinfo node
    GLOBALDEF_VARINFO( arg_node) = TRAVdo( GLOBALDEF_VARINFO( arg_node), arg_info);
  }

  if (GLOBALDEF_INIT ( arg_node) != NULL) {
    //traverse varinfo node
    GLOBALDEF_INIT ( arg_node) = TRAVdo( GLOBALDEF_INIT ( arg_node), arg_info);
  }
  
  if (GLOBALDEF_NEXT( arg_node) != NULL) {
    //traverse varinfo node
    GLOBALDEF_NEXT( arg_node) = TRAVdo( GLOBALDEF_NEXT( arg_node), arg_info);
  }

  printf(";");
  printNewLineWithIndent(indent);

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node vardec
 ***************************************************************************/

node *
PRTvardec(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvardec");

  if (VARDEC_VARINFO( arg_node) != NULL) {
    //traverse varinfo node
    VARDEC_VARINFO( arg_node) = TRAVdo( VARDEC_VARINFO( arg_node), arg_info);
  }
  
  if (VARDEC_INIT( arg_node) != NULL) {
    printf( " = ");
    VARDEC_INIT( arg_node) = TRAVopt( VARDEC_INIT( arg_node), arg_info);
  }
    
  printf( ";");
  printNewLineWithIndent(indent);

  if (VARDEC_NEXT( arg_node) != NULL) {
    VARDEC_NEXT( arg_node) = TRAVopt( VARDEC_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);

}


/** <!--******************************************************************-->
 *
 * @add by michael for node exprs
 ***************************************************************************/

node *
PRTexprs(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTexprs");

  printf("hello, this is exprs\n");

  if (EXPRS_EXPR( arg_node) != NULL) {
    //traverse varinfo node
    EXPRS_EXPR( arg_node) = TRAVdo( EXPRS_EXPR( arg_node), arg_info);
  }

  if (EXPRS_NEXT( arg_node) != NULL) {
    printf( ", ");
    EXPRS_NEXT( arg_node) = TRAVopt( EXPRS_NEXT( arg_node), arg_info);
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node while
 ***************************************************************************/

node *
PRTwhile(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTwhile");

  printNewLineWithIndent(indent);
  printf( "while (");

  if (WHILE_PRED( arg_node) != NULL) {
    WHILE_PRED( arg_node) = TRAVdo( WHILE_PRED( arg_node), arg_info);
  }

  printf( ") {");
  indent++;
  printNewLineWithIndent(indent);

  if (WHILE_BODY( arg_node) != NULL) {
    WHILE_BODY( arg_node) = TRAVdo( WHILE_BODY( arg_node), arg_info);
  }

  indent--;
  printNewLineWithIndent(indent);
  printf( "}\n");
  printNewLineWithIndent(indent);
  
  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node dowhile
 ***************************************************************************/

node *
PRTdowhile(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTdowhile");

  printNewLineWithIndent(indent);
  printf( "do {");
  indent++;
  printNewLineWithIndent(indent);

  if (DOWHILE_BODY( arg_node) != NULL) {
    DOWHILE_BODY( arg_node) = TRAVdo( DOWHILE_BODY( arg_node), arg_info);
  }

  indent--;
  printNewLineWithIndent(indent);
  printf( "} while (");

  if (DOWHILE_PRED( arg_node) != NULL) {
    DOWHILE_PRED( arg_node) = TRAVdo( DOWHILE_PRED( arg_node), arg_info);
  }

  printf( ");\n");
  printNewLineWithIndent(indent);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node for
 ***************************************************************************/

node *
PRTfor(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfor");

  printNewLineWithIndent(indent);
  printf( "for (int ");

  if (FOR_VAR( arg_node) != NULL) {
    FOR_VAR( arg_node) = TRAVdo( FOR_VAR( arg_node), arg_info);
  }

  printf(" = ");

  if (FOR_START( arg_node) != NULL) {
    FOR_START( arg_node) = TRAVdo( FOR_START( arg_node), arg_info);
  }

  printf( ", ");

  if (FOR_STOP( arg_node) != NULL) {
    FOR_STOP( arg_node) = TRAVdo( FOR_STOP( arg_node), arg_info);
  }

  if (FOR_STEP( arg_node) != NULL) {
    printf( ", ");
    FOR_STEP( arg_node) = TRAVdo( FOR_STEP( arg_node), arg_info);
  }
  
  printf( ") {");
  indent++;
  printNewLineWithIndent(indent);
  if (FOR_BODY( arg_node) != NULL) {
    FOR_BODY( arg_node) = TRAVdo( FOR_BODY( arg_node), arg_info);
  }
  indent--;
  printNewLineWithIndent(indent);
  printf( "}\n");
  printNewLineWithIndent(indent);

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node return
 ***************************************************************************/

node *
PRTreturn(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTreturn");

  printf( "return ");

  RETURN_EXPR( arg_node) = TRAVopt( RETURN_EXPR( arg_node), arg_info);

  printf( ";" );
  printNewLineWithIndent(indent);
  DBUG_RETURN (arg_node);
}




/** <!--******************************************************************-->
 *
 * @add by michael for node cast
 ***************************************************************************/


node *
PRTcast(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTcast");

  switch(CAST_TYPE(arg_node)) {
      case T_int:
          printf("(int) ");
          break;
      case T_float:
          printf("(float) ");
          break;
      case T_bool:
          printf("(bool) ");
          break;
      case T_void:
          DBUG_ASSERT( 0, "illigal type cast (void) detected!");
          break;
      case T_unknown:
          DBUG_ASSERT( 0, "unknown type cast detected!");
          break;
  }



  if (CAST_ARG(arg_node) != NULL)
      CAST_ARG(arg_node) = TRAVdo(CAST_ARG(arg_node), arg_info);

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node module
 ***************************************************************************/

node *
PRTmodule(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTmodule");
  printf( "\n\n-in module\n\n");
  if (MODULE_MODULEDECLS( arg_node) != NULL) {
    MODULE_MODULEDECLS( arg_node) = TRAVdo( MODULE_MODULEDECLS( arg_node), arg_info);
  }

  DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @add by michael for node PRTvarinfo
 ***************************************************************************/

node *
PRTvarinfo(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarinfo");

  switch (VARINFO_TYPE( arg_node)) {
    case T_int:
      printf("int ");
      break;
    case T_float:
      printf("float ");
      break;
    case T_bool:
      printf("bool ");;
      break;
    case T_void:
      printf("void ");
      break;
    case T_unknown:
    //  DBUG_ASSERT( 0, "unknown variable type detected!");
      break;
  }

  if (VARINFO_NAME( arg_node) != NULL) {
    printf(" %s", VARINFO_NAME(arg_node));   //the name of the variable
  }

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node PRTparam
 ***************************************************************************/

node *
PRTparam(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTparam");

  if (PARAM_VARINFO( arg_node) != NULL) {
    PARAM_VARINFO( arg_node) = TRAVdo( PARAM_VARINFO( arg_node), arg_info);
  }

  if (PARAM_NEXT( arg_node) != NULL) {
    printf(",  ");
    PARAM_NEXT( arg_node) = TRAVdo( PARAM_NEXT( arg_node), arg_info);
  }
  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node PRTif
 ***************************************************************************/

node *
PRTif(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTif");

  printf("if (");

  if(IF_PRED(arg_node) != NULL)
      IF_PRED(arg_node) = TRAVdo(IF_PRED(arg_node), arg_info);

  printf(") {");
  indent++;
  printNewLineWithIndent(indent);

  if(IF_THEN(arg_node) != NULL) {
      IF_THEN(arg_node) = TRAVdo(IF_THEN(arg_node), arg_info);
  }

  indent--;
  printNewLineWithIndent(indent);
  printf("}");

  if(IF_ELSE(arg_node) != NULL) {
      printf(" else {");
      indent++;
      printNewLineWithIndent(indent);

      IF_ELSE(arg_node) = TRAVdo(IF_ELSE(arg_node), arg_info);

      indent--;
      printNewLineWithIndent(indent);
      printf("}");
  }

  printNewLineWithIndent(indent);

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node PRTfunap
 ***************************************************************************/

node *
PRTfunap(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfunap");

  if(arg_node != NULL) {
      printf("%s(", FUNAP_NAME(arg_node));

      if(FUNAP_ARGS(arg_node) != NULL) {
          FUNAP_ARGS( arg_node) = TRAVdo( FUNAP_ARGS( arg_node), arg_info);
      }

      printf(")");
  }

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node PRTarg
 ***************************************************************************/

node *
PRTarg(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTarg");

  if(ARG_EXPR(arg_node) != NULL)
      ARG_EXPR( arg_node) = TRAVdo( ARG_EXPR( arg_node), arg_info);
  
  if(ARG_NEXT(arg_node) != NULL) {
      printf(", ");
      ARG_NEXT( arg_node) = TRAVdo( ARG_NEXT( arg_node), arg_info);
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node PRTmoduledecls
 ***************************************************************************/

node *
PRTmoduledecls(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTmoduledecls");

  if (MODULEDECLS_MODULEDECL( arg_node) != NULL) {
    MODULEDECLS_MODULEDECL( arg_node) = TRAVopt( MODULEDECLS_MODULEDECL( arg_node), arg_info);
  }

  if (MODULEDECLS_NEXT(arg_node) != NULL) {
    MODULEDECLS_NEXT( arg_node) = TRAVdo( MODULEDECLS_NEXT( arg_node), arg_info);
  }

  DBUG_RETURN (arg_node);
}


/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 * 
 * @param syntaxtree a node structure
 * 
 * @return the unchanged nodestructure
 ******************************************************************************/

node 
*PRTdoPrint( node *syntaxtree)
{
  info *info;
  
  DBUG_ENTER("PRTdoPrint");

  DBUG_ASSERT( (syntaxtree!= NULL), "PRTdoPrint called with empty syntaxtree");

  info = MakeInfo();
  
  TRAVpush( TR_prt);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  printf( "\n\n-------------------------------------------\n\n");

  DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
