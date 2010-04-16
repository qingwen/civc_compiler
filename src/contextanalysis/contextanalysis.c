
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

#include "node_basic.h"
#include "contextanalysis.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "types.h"
#include "ctinfo.h"
#include "str.h"

/*
 * INFO structure
 */
struct INFO {
  char* funname;
  bool firsterror;
  int  cnt_fun;   //the number of functions
  node *vardec_list;
  node *param_list;
  node *global_decl_list;
  node *induction_list;
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)
#define INFO_CNT_FUN(n) ((n)->cnt_fun)
#define INFO_FUNNAME(n) ((n)->funname)
#define INFO_VARDEC_LIST(n) ((n)->vardec_list)
#define INFO_PARAM_LIST(n) ((n)->param_list)
#define INFO_GLOBAL_DECL_LIST(n) ((n)->global_decl_list)
#define INFO_INDUCTION_LIST(n) ((n)->induction_list)


static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_FIRSTERROR(result) = FALSE;
  INFO_CNT_FUN(result) = 0;
  INFO_FUNNAME(result) = NULL;
  INFO_VARDEC_LIST(result) = NULL;
  INFO_PARAM_LIST(result) = NULL;
  INFO_GLOBAL_DECL_LIST(result) = NULL;
  INFO_INDUCTION_LIST(result) = NULL;
  
  return result;
}

static node *
MakeEmptyNode ()
{
    node *result;

    DBUG_ENTER ("MakeEmptyNode");

    result = (node *) MEMmalloc (sizeof (node));

    NODE_LINE (result) = global.line;

    DBUG_RETURN (result);
}


static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
}

/*
 * @author: Qingwen Chen
 *
 * @description: get the declaration of a variable according to its name
 *
 * @param: var_name, name of the variable
 * @param: arg_info, with list of local variable declaration, global variable
 *          declaration and parameters
 *
 * @return: a pointer to the corresponding varinfo node if found, otherwise NULL
 */
node*
getVarDecl(char* var_name, info* arg_info)
{
    node *vardec_list = MakeEmptyNode();
    NODE_TYPE (vardec_list) = N_vardec;
    vardec_list = arg_info->vardec_list;

    node *param_list = MakeEmptyNode();
    NODE_TYPE (param_list) = N_param;
    param_list = arg_info->param_list;

    node *global_list = MakeEmptyNode();
    NODE_TYPE (global_list) = N_moduledecls;
    global_list = arg_info->global_decl_list;

    node *induction_list = MakeEmptyNode();
    NODE_TYPE (induction_list) = N_for;
    induction_list = arg_info->induction_list;

    if (induction_list != NULL)
    {
        if ( STReq(var_name, VARINFO_NAME(induction_list) ) == TRUE )
        {
            return induction_list;
        }
    }
    
    while (vardec_list != NULL)
    {
        if ( STReq(var_name, VARINFO_NAME(VARDEC_VARINFO(vardec_list)) ) == TRUE )
        {
            return VARDEC_VARINFO(vardec_list);
        }
        vardec_list = VARDEC_NEXT(vardec_list);
    }

    //then check the param list
    //if error_flag is FALSE var is already link to its def no need to check again
    while ( param_list != NULL )
    {
        if ( STReq(var_name, VARINFO_NAME(PARAM_VARINFO(param_list)) ) == TRUE )
        {
            return PARAM_VARINFO(param_list);
        }
        param_list = PARAM_NEXT(param_list);
    }

    //finally check the global list
    while ( global_list != NULL )
    {
        if ( NODE_TYPE (MODULEDECLS_MODULEDECL (global_list)) == N_globaldef )
        {
            if ( STReq(var_name, VARINFO_NAME(GLOBALDEF_VARINFO(MODULEDECLS_MODULEDECL (global_list))) ) == TRUE )
            {
                return GLOBALDEF_VARINFO(MODULEDECLS_MODULEDECL (global_list));
            }
        }

        global_list = MODULEDECLS_NEXT(global_list);
    }

    return NULL;
}



/*
 * author: michael
 * time: April 1st 2010
 * check whether var has relevant def or decl information
 * if so let info node of var points to vardec
 * if not generate semantics error
 */
node *CAvar(node * arg_node, info * arg_info)
{
    DBUG_ENTER("CAvar");
    node* result = getVarDecl(VAR_NAME(arg_node), arg_info); //return a Varinfo node if found

    if ( result == NULL )
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: var %s has no relevant definition\n",
                NODE_LINE(arg_node), VAR_NAME(arg_node));
    }

    VAR_INFO(arg_node) = result;

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn CAinstr
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
CAinstrs (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAinstrs");

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
 * @fn CAinstr
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
CAassign (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("CAassign");

  if (ASSIGN_LET( arg_node) != NULL ) {
    ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
  }
  
  if(ASSIGN_LET(arg_node) != NULL
          && VARLET_INFO(ASSIGN_LET(arg_node)) != NULL
          && NODE_TYPE(VARINFO_DECL(VARLET_INFO(ASSIGN_LET(arg_node)))) == N_for) {
      INFO_FIRSTERROR(arg_info) = TRUE;
      printf("#LINE %d: semantic error: assignment for induction variable %s is invalid!\n",
              NODE_LINE(arg_node), VARLET_NAME(ASSIGN_LET(arg_node)));
  }

  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn CAbinop
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
CAbinop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("CAbinop");

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
    case BO_unknown:
        DBUG_ASSERT( 0, "unknown binop detected!");
    }

    BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn CAfloat
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
CAfloat (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAfloat");

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn CAnum
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
CAnum (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAnum");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn CAboolean
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
CAbool (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAbool");

    DBUG_RETURN (arg_node);
}



/*
 * author: michael
 * time: April 1st 2010
 * check whether varlet has relevant def or decl information
 * if so let info node of var points to vardec
 * if not generate semantics error
 */

node *
CAvarlet (node * arg_node, info * arg_info)
{
    DBUG_ENTER("CAvarlet");

    node* result = getVarDecl(VARLET_NAME(arg_node), arg_info);

    if ( result == NULL )
    {
        printf("#LINE %d: semantic error: var %s has no relevant definition\n",
                NODE_LINE(arg_node), VARLET_NAME(arg_node));
    }
    
    VARLET_INFO(arg_node) = result;

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn CAerror
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
CAerror (node * arg_node, info * arg_info)
{
    bool first_error;

    DBUG_ENTER ("CAerror");

    if (NODE_ERROR (arg_node) != NULL) {
        NODE_ERROR (arg_node) = TRAVdo (NODE_ERROR (arg_node), arg_info);
    }

    first_error = INFO_FIRSTERROR( arg_info);

    if ( (global.outfile != NULL)
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
CAfundef(node * arg_node, info * arg_info)
{
    DBUG_ENTER("CAfundef");
    
    node *vardec_list = MakeEmptyNode();
    NODE_TYPE (vardec_list) = N_vardec;
    vardec_list = FUNDEF_VARDECS(arg_node);

    arg_info->param_list  = FUNDEF_PARAMS(arg_node);
    if (FUNDEF_PARAMS( arg_node) != NULL) {   
        FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
    }


    if (FUNDEF_LOCALFUNS ( arg_node) != NULL) {
        FUNDEF_LOCALFUNS ( arg_node) = TRAVdo( FUNDEF_LOCALFUNS ( arg_node), arg_info);
    }

    arg_info->vardec_list = FUNDEF_VARDECS(arg_node);
    if (FUNDEF_VARDECS( arg_node) != NULL) {
        
        FUNDEF_VARDECS( arg_node) = TRAVdo( FUNDEF_VARDECS( arg_node), arg_info);
    }

    if (FUNDEF_BODY( arg_node) != NULL) {
        FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
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
CAglobaldef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAglobaldef");

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
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node vardec
 ***************************************************************************/

node *
CAvardec(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAvardec");

    if (VARDEC_VARINFO( arg_node) != NULL) {
        //traverse varinfo node
        VARDEC_VARINFO( arg_node) = TRAVdo( VARDEC_VARINFO( arg_node), arg_info);
    }

    if (VARDEC_INIT( arg_node) != NULL) {
        VARDEC_INIT( arg_node) = TRAVopt( VARDEC_INIT( arg_node), arg_info);
    }

    if (VARDEC_NEXT( arg_node) != NULL) {
        VARDEC_NEXT( arg_node) = TRAVopt( VARDEC_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);

}



/** <!--******************************************************************-->
 *
 * @add by michael for node vardecs
 ***************************************************************************/

node *
CAvardecs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAvardecs");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node fundefs
 ***************************************************************************/

node *
CAfundefs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAfundefs");

    //printf( "%s", VARLET_NAME( arg_node));

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node exprs
 ***************************************************************************/

node *
CAexprs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAexprs");

    if (EXPRS_EXPR( arg_node) != NULL) {
        //traverse varinfo node
        EXPRS_EXPR( arg_node) = TRAVdo( EXPRS_EXPR( arg_node), arg_info);
    }

    if (EXPRS_NEXT( arg_node) != NULL) {
        EXPRS_NEXT( arg_node) = TRAVopt( EXPRS_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @add by michael for node while
 ***************************************************************************/

node *
CAwhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAwhile");

    if (WHILE_PRED( arg_node) != NULL) {
        WHILE_PRED( arg_node) = TRAVdo( WHILE_PRED( arg_node), arg_info);
    }

    if (WHILE_BODY( arg_node) != NULL) {
        WHILE_BODY( arg_node) = TRAVdo( WHILE_BODY( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node dowhile
 ***************************************************************************/

node *
CAdowhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAdowhile");

    if (DOWHILE_BODY( arg_node) != NULL) {
        DOWHILE_BODY( arg_node) = TRAVdo( DOWHILE_BODY( arg_node), arg_info);
    }

    if (DOWHILE_PRED( arg_node) != NULL) {
        DOWHILE_PRED( arg_node) = TRAVdo( DOWHILE_PRED( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node for
 ***************************************************************************/

node *
CAfor(node * arg_node, info * arg_info)
{
  DBUG_ENTER ("CAfor");

  if (FOR_VAR( arg_node) != NULL) {
    arg_info->induction_list = FOR_VAR(arg_node);
    FOR_VAR( arg_node) = TRAVdo( FOR_VAR( arg_node), arg_info);

  }

  if (FOR_START( arg_node) != NULL) {
    FOR_START( arg_node) = TRAVdo( FOR_START( arg_node), arg_info);
  }

  if (FOR_STOP( arg_node) != NULL) {
    FOR_STOP( arg_node) = TRAVdo( FOR_STOP( arg_node), arg_info);
  }

  if (FOR_STEP( arg_node) != NULL) {
    FOR_STEP( arg_node) = TRAVdo( FOR_STEP( arg_node), arg_info);
  }
  
  if (FOR_BODY( arg_node) != NULL) {
    FOR_BODY( arg_node) = TRAVdo( FOR_BODY( arg_node), arg_info);
  }

  arg_info->induction_list = NULL;

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node return
 ***************************************************************************/

node *
CAreturn(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAreturn");

    RETURN_EXPR( arg_node) = TRAVopt( RETURN_EXPR( arg_node), arg_info);

    DBUG_RETURN (arg_node);
}





/** <!--******************************************************************-->
 *
 * @add by michael for node cast
 ***************************************************************************/

node *
CAcast(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAcast");

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node monop
 ***************************************************************************/

node *
CAmonop(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAmonop");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node module
 ***************************************************************************/

node *
CAmodule(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAmodule");

    if (MODULE_MODULEDECLS( arg_node) != NULL) {
        MODULE_MODULEDECLS( arg_node) = TRAVdo( MODULE_MODULEDECLS( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node CAvarinfo
 ***************************************************************************/

node *
CAvarinfo(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAvarinfo");

    switch (VARINFO_TYPE( arg_node)) {
    case T_int:
        break;
    case T_float:
        break;
    case T_bool:
        break;
    case T_void:
        break;
    case T_unknown:
        //  DBUG_ASSERT( 0, "unknown variable type detected!");
        break;
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node CAparam
 ***************************************************************************/

node *
CAparam(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAparam");

    if (PARAM_VARINFO( arg_node) != NULL) {
        PARAM_VARINFO( arg_node) = TRAVdo( PARAM_VARINFO( arg_node), arg_info);
    }

    if (PARAM_NEXT( arg_node) != NULL) {
        PARAM_NEXT( arg_node) = TRAVdo( PARAM_NEXT( arg_node), arg_info);
    }
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node CAif
 ***************************************************************************/

node *
CAif(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAif");

    if (IF_PRED( arg_node) != NULL) {
        IF_PRED( arg_node) = TRAVdo( IF_PRED( arg_node), arg_info);
    }

    if (IF_THEN( arg_node) != NULL) {
        IF_THEN( arg_node) = TRAVdo( IF_THEN( arg_node), arg_info);
    }
    
    if (IF_ELSE( arg_node) != NULL) {
        IF_ELSE( arg_node) = TRAVdo( IF_ELSE( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node CAfunap
 ***************************************************************************/

node *
CAfunap(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAfunap");

    node *global_list = MakeEmptyNode();
    NODE_TYPE (global_list) = N_moduledecls;
    global_list = arg_info->global_decl_list;
    bool error_flag = TRUE;

    while ( global_list != NULL )
    {
        if ( NODE_TYPE (MODULEDECLS_MODULEDECL (global_list)) == N_fundef )
        {
            if ( STReq(FUNAP_NAME(arg_node), FUNDEF_NAME(MODULEDECLS_MODULEDECL (global_list)) ) == TRUE )
            {
                FUNAP_FUN(arg_node) = MODULEDECLS_MODULEDECL (global_list);
                error_flag = FALSE;
                break;
            }
        }

        global_list = MODULEDECLS_NEXT(global_list);
    }

    if ( error_flag == TRUE )
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: function %s has no relevant definition\n",
                NODE_LINE(arg_node), FUNAP_NAME(arg_node));
    }
    
    if (FUNAP_ARGS(arg_node) != NULL) {
        FUNAP_ARGS( arg_node) = TRAVdo( FUNAP_ARGS( arg_node), arg_info);
    }
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node CAarg
 ***************************************************************************/

node *
CAarg(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAarg");

    if (ARG_EXPR(arg_node) != NULL) {
        ARG_EXPR( arg_node) = TRAVdo( ARG_EXPR( arg_node), arg_info);
    }
    
    if (ARG_NEXT(arg_node) != NULL) {
        ARG_NEXT( arg_node) = TRAVdo( ARG_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node CAmoduledecls
 ***************************************************************************/

node *
CAmoduledecls(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("CAmoduledecls");

    if (MODULEDECLS_MODULEDECL( arg_node) != NULL) {

        MODULEDECLS_MODULEDECL( arg_node) = TRAVopt( MODULEDECLS_MODULEDECL( arg_node), arg_info);
    }

    if (MODULEDECLS_NEXT(arg_node) != NULL) {
        MODULEDECLS_NEXT( arg_node) = TRAVdo( MODULEDECLS_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!-- ****************************************************************** -->
 * @brief CAdoContextAnalysis do the context analysis
 * 
 * @param syntaxtree a node structure
 * 
 ******************************************************************************/

node 
*CAdoContextAnalysis( node *syntaxtree)
{
  info *info;
  
  DBUG_ENTER("CAdoContextAnalysis");

  DBUG_ASSERT( (syntaxtree!= NULL), "CAdoContextAnalysis called with empty syntaxtree");

  info = MakeInfo();
  info->global_decl_list = syntaxtree;
  TRAVpush( TR_ca);

  syntaxtree = TRAVdo( syntaxtree, info);
  
  if(INFO_FIRSTERROR(info) == FALSE) {
      printf("\n              Succeed                             \n");
  }

  TRAVpop();

  info = FreeInfo(info);

  printf( "\n\n----------------------------------------------\n\n");

  DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
