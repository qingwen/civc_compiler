
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
    bool arg_check;
    bool return_exist;
    node *fundef_param;
    type  current_type;    //preserve the type of the  node which is current visiting
    type  fun_return_type; //preserve the return type of the function for the return check
};


#define INFO_FIRSTERROR(n) ((n)->firsterror)
#define INFO_ARG_CHECK(n) ((n)->arg_check)
#define INFO_RETURN_EXIST(n) ((n)->return_exist)
#define INFO_FUNNAME(n) ((n)->funname)
#define INFO_CURRENT_TYPE(n) ((n)->current_type)
#define INFO_FUN_RETURN_TYPE(n) ((n)->fun_return_type)
#define INFO_FUNDEF_PARAM(n) ((n)->fundef_param)

static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_FIRSTERROR(result) = FALSE;
    INFO_ARG_CHECK(result) = FALSE;
    INFO_RETURN_EXIST(result) = FALSE;
    INFO_FUNNAME(result) = NULL;  
    INFO_CURRENT_TYPE(result) = T_unknown;
    INFO_FUN_RETURN_TYPE(result) = T_unknown;
    INFO_FUNDEF_PARAM(result) = NULL;
    
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
 * author: michael
 * time: April 1st 2010
 * get the type of var and store in arg_info
 */
node *TCvar(node * arg_node, info * arg_info)
{
    if ( VAR_INFO(arg_node) != NULL)
    {
        arg_info->current_type = VARINFO_TYPE(VAR_INFO(arg_node));
    }
    else
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: %s var has no relevant declaration\n",
                NODE_LINE(arg_node), VAR_NAME(arg_node));
    }

    return arg_node;
}



/** <!--******************************************************************-->
 *
 * @fn TCinstr
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
TCinstrs (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCinstrs");

    if (INSTRS_INSTR( arg_node) != NULL) {
        INSTRS_INSTR( arg_node) = TRAVdo( INSTRS_INSTR( arg_node), arg_info);
    }

    if (INSTRS_NEXT( arg_node) != NULL) {
        INSTRS_NEXT( arg_node) = TRAVopt( INSTRS_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/*
 * author: michael
 * time: April 1st 2010
 * first get the left hand side types
 * then get the exprs in the right hand side, which will recursively visit each expr and calculate the type of the exprs
 * then compare left type and right type
 */
node *
TCassign (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCassign");

    type varlet_type = T_unknown;
    type expr_type = T_unknown;

    if (ASSIGN_LET( arg_node) != NULL) 
    {
        ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
        //get the type of the varlet
        varlet_type = arg_info->current_type;
    }
    else   //void for funag
    {
        varlet_type = T_void;
    }

    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

    //get the type of right hand expr
    expr_type = arg_info->current_type;

    if (expr_type != varlet_type )
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: assign instruction has not match type between left hand right\n",
                NODE_LINE(arg_node));
    }

    DBUG_RETURN (arg_node);
}


/*
 * author: michael
 * time: April 1st 2010
 * first get the type of left hand exprs, which will recursively visit each expr and calculate the type of the exprs
 * then get the type of right hand exprs, which will recursively visit each expr and calculate the type of the exprs
 * finally compare them
 */

node *
TCbinop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCbinop");

    type left_type = T_unknown;
    type right_type = T_unknown;

    BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
    left_type = arg_info->current_type;

    BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
    right_type = arg_info->current_type;

    switch (BINOP_OP( arg_node)) {
        case BO_add:
        case BO_sub:
        case BO_mul:
        case BO_div:
            if ( (right_type != left_type) ||
                    ((right_type != T_int) &&  (right_type != T_float) )    )
            {
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: mismatched type on arithmetic binop node, %d binop %d\n",
                        NODE_LINE(arg_node), left_type, right_type);
            }
            INFO_CURRENT_TYPE(arg_info) = right_type;
            break;
        case BO_lt:
        case BO_le:
        case BO_gt:
        case BO_ge:
            if ( (right_type != left_type) ||
                    ((right_type != T_int) &&  (right_type != T_float) )    )
            {
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: mismatched type on arithmetic binop node\n",
                        NODE_LINE(arg_node));
            }
            INFO_CURRENT_TYPE(arg_info) = T_bool;
            break;
        case BO_eq:
        case BO_ne:
            if ( (right_type != left_type) ||
                    ((right_type != T_int) &&  (right_type != T_float) && (right_type != T_bool) )    )
            {
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: mismatched type on arithmetic binop node\n",
                    NODE_LINE(arg_node));
            }
            INFO_CURRENT_TYPE(arg_info) = T_bool;
            break;
        case BO_mod:
            if ( (right_type != left_type) || (right_type != T_int)    )
            {
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: mismatched type on arithmetic binop node\n",
                    NODE_LINE(arg_node));
            }
            INFO_CURRENT_TYPE(arg_info) = right_type;
            break;
        case BO_or:
        case BO_and:
            if ( (right_type != left_type) || (right_type != T_bool) )
            {
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: type error on bool binop node\n",
                    NODE_LINE(arg_node));
            }
            INFO_CURRENT_TYPE(arg_info) = T_bool;
            break;
        case BO_unknown:
            DBUG_ASSERT( 0, "unknown binop detected!");
    }
    
    if (left_type > right_type)  //decide the type of binop node
    {
        arg_info->current_type = left_type;
    }
    else if (left_type < right_type)
    {
        arg_info->current_type = right_type;
    }
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn TCfloat
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
TCfloat (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCfloat");

    arg_info->current_type = T_float;

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn TCnum
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
TCnum (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCnum");

    arg_info->current_type = T_int;

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn TCboolean
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
TCbool (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCbool");

    arg_info->current_type = T_bool;

    DBUG_RETURN (arg_node);
}



/*
 * author: michael
 * time: April 1st 2010
 * get the varlet type info and assign it to arg_info
 */

node *
TCvarlet (node * arg_node, info * arg_info)
{
    if ( VARLET_INFO(arg_node) != NULL)
    {
        arg_info->current_type = VARINFO_TYPE(VARLET_INFO(arg_node));
    }
    else
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: %s var has no relevant type\n",
                NODE_LINE(arg_node), VARLET_NAME(arg_node));
    }

    return (arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn TCerror
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
TCerror (node * arg_node, info * arg_info)
{
    bool first_error;

    DBUG_ENTER ("TCerror");

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
TCfundef(node * arg_node, info * arg_info)
{
    //reset arg_info
    arg_info->return_exist = FALSE;
    
    arg_info->fun_return_type = FUNDEF_RETTYPE( arg_node) ;

    if (FUNDEF_PARAMS( arg_node) != NULL) {
        FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
    }

    if (FUNDEF_VARDECS( arg_node) != NULL) {
        FUNDEF_VARDECS( arg_node) = TRAVdo( FUNDEF_VARDECS( arg_node), arg_info);
    }

    if (FUNDEF_BODY( arg_node) != NULL) 
    {
        FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
	
        if ( arg_info->fun_return_type != T_void && arg_info->return_exist != TRUE)
        {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: semantic error: fun %s miss return statement\n",
                    NODE_LINE(arg_node), FUNDEF_NAME( arg_node) );
        }
    }

    if (FUNDEF_NEXT( arg_node) != NULL) {
        FUNDEF_NEXT( arg_node) = TRAVdo( FUNDEF_NEXT( arg_node), arg_info);
    }

    return (arg_node);
}




/** <!--******************************************************************-->
 *
 * @add by michael for node globaldec
 ***************************************************************************/

node *
TCglobaldec(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCglobaldec");

    if (GLOBALDEF_VARINFO( arg_node) != NULL) {
        //traverse varinfo node
        GLOBALDEF_VARINFO( arg_node) = TRAVdo( GLOBALDEF_VARINFO( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node globaldef
 ***************************************************************************/

node *
TCglobaldef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCglobaldef");

    type varinfo_type = T_unknown;

    if (GLOBALDEF_VARINFO( arg_node) != NULL) {
        //traverse varinfo node
        GLOBALDEF_VARINFO( arg_node) = TRAVdo( GLOBALDEF_VARINFO( arg_node), arg_info);
        varinfo_type = INFO_CURRENT_TYPE(arg_info);
    }

    if (GLOBALDEF_INIT ( arg_node) != NULL) {
        //traverse varinfo node
        GLOBALDEF_INIT ( arg_node) = TRAVdo( GLOBALDEF_INIT ( arg_node), arg_info);
        if(varinfo_type != INFO_CURRENT_TYPE(arg_info)) {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: mismatched types in global variable declaration\n",
                    NODE_LINE(arg_node));
        }
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
TCvardec(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCvardec");

    type vardec_type = T_unknown;

    if (VARDEC_VARINFO( arg_node) != NULL) {
        VARDEC_VARINFO( arg_node) = TRAVdo( VARDEC_VARINFO( arg_node), arg_info);
        vardec_type = INFO_CURRENT_TYPE(arg_info);
    }

    if (VARDEC_INIT( arg_node) != NULL) {
        VARDEC_INIT( arg_node) = TRAVopt( VARDEC_INIT( arg_node), arg_info);
        if(vardec_type != INFO_CURRENT_TYPE(arg_info)) {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: mismatched types in local variable declaration\n",
                    NODE_LINE(arg_node));
        }
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
TCvardecs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCvardecs");

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @add by michael for node exprs
 ***************************************************************************/

node *
TCexprs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCexprs");

    printf("TCexprs is visited\n");
    if (EXPRS_EXPR( arg_node) != NULL)
    {
        //traverse varinfo node
        EXPRS_EXPR( arg_node) = TRAVdo( EXPRS_EXPR( arg_node), arg_info);
/*
        if ( arg_info->arg_check == TRUE)  //need to check the type match between fun arg and fun param
        {
            type arg_type = T_unknown;    //the type preserved for arg node of fun call
            type param_type = T_unknown;  //the type preserved for param node of fundef

            arg_type = arg_info->current_type;

            if (PARAM_VARINFO( arg_info->fundef_param) != NULL)
            {
                PARAM_VARINFO( arg_info->fundef_param) = TRAVdo( PARAM_VARINFO( arg_info->fundef_param), arg_info);
		param_type = arg_info->current_type;
		
                PARAM_VARINFO( arg_info->fundef_param) = PARAM_NEXT( arg_info->fundef_param);                
            }

            printf("fun arg type %d\n", arg_type);
            printf("fun param type %d\n", param_type);

            if ( arg_type != param_type )
            {
                printf("fun arg does not match corresponding fundef param type\n");
            }
        }
	*/
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
TCwhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCwhile");

    if (WHILE_PRED( arg_node) != NULL) {
        WHILE_PRED( arg_node) = TRAVdo( WHILE_PRED( arg_node), arg_info);
    }

    if ( arg_info->current_type != T_bool)
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: condition expression of while is not bool\n",
                NODE_LINE(arg_node));
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
TCdowhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCdowhile");

    if (DOWHILE_BODY( arg_node) != NULL)
    {
        DOWHILE_BODY( arg_node) = TRAVdo( DOWHILE_BODY( arg_node), arg_info);
    }

    if (DOWHILE_PRED( arg_node) != NULL)
    {
        DOWHILE_PRED( arg_node) = TRAVdo( DOWHILE_PRED( arg_node), arg_info);

        if ( arg_info->current_type != T_bool)
        {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: semantic error: condition expression of dowhile is not bool\n",
                    NODE_LINE(arg_node));
        }
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node for
 ***************************************************************************/

node *
TCfor(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCfor");

    if (FOR_VAR( arg_node) != NULL) {
        FOR_VAR( arg_node) = TRAVdo( FOR_VAR( arg_node), arg_info);
    }

    if (FOR_START( arg_node) != NULL) {
        FOR_START( arg_node) = TRAVdo( FOR_START( arg_node), arg_info);
        if (INFO_CURRENT_TYPE(arg_info) != T_int) {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: wrong type of the initial value of for-loop\n",
                    NODE_LINE(arg_node));
        }
    }

    if (FOR_STOP( arg_node) != NULL) {
        FOR_STOP( arg_node) = TRAVdo( FOR_STOP( arg_node), arg_info);
    }

    if (FOR_STEP( arg_node) != NULL) {
        FOR_STEP( arg_node) = TRAVdo( FOR_STEP( arg_node), arg_info);
	    if ( arg_info->current_type != T_int)
        {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: step value of for-loop is not int\n",
                    NODE_LINE(arg_node));
        }
    }

    if (FOR_BODY( arg_node) != NULL) {
        FOR_BODY( arg_node) = TRAVdo( FOR_BODY( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node return
 ***************************************************************************/

node *
TCreturn(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCreturn");
    type expr_type = T_unknown;

    RETURN_EXPR( arg_node) = TRAVopt( RETURN_EXPR( arg_node), arg_info);
    expr_type = INFO_CURRENT_TYPE(arg_info);

    if ( expr_type != arg_info->fun_return_type )
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: return expr does not match the return type of the function\n",
                NODE_LINE(arg_node));
    }

    arg_info->return_exist = TRUE;
    
    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @add by michael for node vardef
 ***************************************************************************/

node *
TCvardef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCvardef");

    DBUG_RETURN (arg_node);
}


/*
 * author: michael
 * time: April 1st 2010
 * set the type of arg_info to the cast type
 */
node *
TCcast(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCcast");
    
    switch (CAST_TYPE(arg_node))
    {
    case T_int:
        arg_info->current_type = T_int;
        break;
    case T_float:
        arg_info->current_type = T_float;
        break;
    case T_bool:
        arg_info->current_type = T_bool;
        break;
    case T_void:
        DBUG_ASSERT( 0, "illigal type cast (void) detected!");
        break;
    case T_unknown:
        DBUG_ASSERT( 0, "unknown type cast detected!");
        break;
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node monop
 ***************************************************************************/

node *
TCmonop(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCmonop");

    MONOP_ARG( arg_node) = TRAVdo( MONOP_ARG( arg_node), arg_info);

    switch(MONOP_OP(arg_node)) {
        case MO_not:
            if(INFO_CURRENT_TYPE(arg_info) != T_bool){
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: wrong type for monop '!'\n",
                         NODE_LINE(arg_node));
            }
            INFO_CURRENT_TYPE(arg_info) = T_bool;
            break;
        case MO_neg:
            if(INFO_CURRENT_TYPE(arg_info) != T_int
                    && INFO_CURRENT_TYPE(arg_info) != T_float){
                INFO_FIRSTERROR(arg_info) = TRUE;
                printf("#LINE %d: semantic error: wrong type for monop '-'\n",
                         NODE_LINE(arg_node));
            }
            break;
    }


    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node module
 ***************************************************************************/

node *
TCmodule(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCmodule");

    if (MODULE_MODULEDECLS( arg_node) != NULL) {
        MODULE_MODULEDECLS( arg_node) = TRAVdo( MODULE_MODULEDECLS( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node TCvarinfo
 ***************************************************************************/

node *
TCvarinfo(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCvarinfo");

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
    arg_info->current_type = VARINFO_TYPE( arg_node);
   
    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node TCparam
 ***************************************************************************/

node *
TCparam(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCparam");

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
 * @add by michael for node TCif
 ***************************************************************************/

node *
TCif(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCif");

    if (IF_PRED( arg_node) != NULL) {
        IF_PRED( arg_node) = TRAVdo( IF_PRED( arg_node), arg_info);
    }

    if ( arg_info->current_type != T_bool)
    {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: semantic error: condition expression of if is not bool\n",
                NODE_LINE(arg_node));
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
 * @add by michael for node TCfunap
 ***************************************************************************/

node *
TCfunap(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCfunap");


    if (FUNAP_FUN( arg_node ) != NULL) {
        INFO_FUNDEF_PARAM(arg_info) = FUNDEF_PARAMS(FUNAP_FUN(arg_node));
    }

    if (FUNAP_ARGS(arg_node) != NULL)
    {
        /*
	to be continued by michael for fun arg check
        node *param_list = MakeEmptyNode();
        NODE_TYPE (param_list) = N_param;
        param_list = FUNDEF_PARAMS(FUNAP_FUN(arg_node));
	
        arg_info->fundef_param = param_list;
	arg_info->arg_check = TRUE;
	*/
        FUNAP_ARGS( arg_node) = TRAVdo( FUNAP_ARGS( arg_node), arg_info);
    }

    
    //after traversing the arg node, set the fun return type to arg_info
    arg_info->current_type =  FUNDEF_RETTYPE((FUNAP_FUN(arg_node)));

    //after finish type match between fun arg and fun param, reset to FALSE
    /*
    to be continued by michael for fun arg check
    arg_info->fundef_param = NULL;
    arg_info->arg_check = FALSE;
    */
    //printf("this return type of funcation call is %d\n", arg_info->current_type);
    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node TCarg
 ***************************************************************************/

node *
TCarg(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCarg");
    
    if (ARG_EXPR(arg_node) != NULL) {
        ARG_EXPR( arg_node) = TRAVdo( ARG_EXPR( arg_node), arg_info);
        if(INFO_CURRENT_TYPE(arg_info) != VARINFO_TYPE(PARAM_VARINFO(INFO_FUNDEF_PARAM(arg_info)))) {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: mismatched types of arguments of a function\n",
                    NODE_LINE(arg_node));
        }
    }


   
    if (ARG_NEXT(arg_node) != NULL) {
        if(PARAM_NEXT(INFO_FUNDEF_PARAM(arg_info)) == NULL) {
            INFO_FIRSTERROR(arg_info) = TRUE;
            printf("#LINE %d: symantic error: mismatched number of arguments of a function\n",
                    NODE_LINE(arg_node));
        } else {
            INFO_FUNDEF_PARAM(arg_info) = PARAM_NEXT(INFO_FUNDEF_PARAM(arg_info));
        }
        ARG_NEXT( arg_node) = TRAVdo( ARG_NEXT( arg_node), arg_info);
    } else if (PARAM_NEXT(INFO_FUNDEF_PARAM(arg_info)) != NULL) {
        INFO_FIRSTERROR(arg_info) = TRUE;
        printf("#LINE %d: symantic error: mismatched number of arguments of a function\n",
                    NODE_LINE(arg_node));
    }
    
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node TCmoduledecls
 ***************************************************************************/

node *
TCmoduledecls(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("TCmoduledecls");

    if (MODULEDECLS_MODULEDECL( arg_node) != NULL) {
        MODULEDECLS_MODULEDECL( arg_node) = TRAVopt( MODULEDECLS_MODULEDECL( arg_node), arg_info);
    }

    if (MODULEDECLS_NEXT(arg_node) != NULL) {
        MODULEDECLS_NEXT( arg_node) = TRAVdo( MODULEDECLS_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}
/** <!-- ****************************************************************** -->
 * @brief TCdoTypeCheck do the context analysis
 *
 * @param syntaxtree a node structure
 *
 ******************************************************************************/

node
*TCdoTypeCheck( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("TCdoTypeCheck");

    DBUG_ASSERT( (syntaxtree!= NULL), "TCdoTypeCheck called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush( TR_tc);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    if(INFO_FIRSTERROR(info) == FALSE) {
        printf("\n                   Succeed                     \n");
    }

    info = FreeInfo(info);

    printf( "\n\n------------------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
