
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
#include "assemblycodegen.h"
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
    bool  firsterror;
    bool  fun_type;         //to make a distinction between fundecl and fundef
    int   type_op;          //type of the operation 0 unknown, 1 load, 2 store
    int   cnt_esr;          //the number of params and local vars
    type  current_type;     //preserve the type of the  node which is current visiting
    node *vardec_list;
    node *param_list;
    node *global_decl_list;
    int   offset_cnt;        //the number used in offset
    int   cnt_arg;           //the number of arguments in funap
    int   cnt_jump;          //the number of jump offset
    bool  as_code_gen;       //some case, we only traverse the tree to get some information but don't generate the assembly code
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)
#define INFO_ESR_COUNT(n) ((n)->cnt_esr)
#define INFO_FUNNAME(n) ((n)->funname)
#define INFO_FUN_TYPE(n) ((n)->fun_type)
#define INFO_TYPE_OP(n) ((n)->type_op)
#define INFO_CURRENT_TYPE(n) ((n)->current_type)
#define INFO_VARDEC_LIST(n) ((n)->vardec_list)
#define INFO_PARAM_LIST(n) ((n)->param_list)
#define INFO_GLOBAL_DECL_LIST(n) ((n)->global_decl_list)
#define INFO_OFFSET_CNT(n) ((n)->offset_cnt)
#define INFO_CN_ARG(n) ((n)->cnt_arg)
#define INFO_CN_JUMP(n) ((n)->cnt_jump)
#define INFO_AS_CODE_GEN(n) ((n)->as_code_gen)


static info *MakeInfo()
{
    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_FIRSTERROR(result) = FALSE;
    INFO_ESR_COUNT(result)  = 0;
    INFO_FUNNAME(result)    = NULL;
    INFO_FUN_TYPE(result)   = FUN_UNKNOWN;
    INFO_TYPE_OP(result)    = OP_UNKNOWN;
    INFO_CURRENT_TYPE(result) = T_unknown;
    INFO_OFFSET_CNT(result)  = 0;
    INFO_CN_ARG(result)  = 0;
    INFO_CN_JUMP(result)  = 0;
    INFO_AS_CODE_GEN(result) = TRUE;

    return result;
}

//the generated assembly file
FILE * pFile;


static info *FreeInfo( info *info)
{
    info = MEMfree( info);

    return info;
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


/*
 * author: michael
 * time: April 1st 2010
 * check whether var has relevant def or decl information
 * if so let info node of var points to vardec
 * if not generate semantics error
 */
node *ACGvar(node * arg_node, info * arg_info)
{
    if (VAR_INFO (arg_node) != NULL)
    {
        VAR_INFO (arg_node) = TRAVdo (VAR_INFO (arg_node), arg_info);
    }

    return (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn ACGinstr
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
ACGinstrs (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGinstrs");

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
 * @fn ACGinstr
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
ACGassign (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGassign");

    arg_info->type_op = OP_LOAD;
    ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

    if (ASSIGN_LET( arg_node) != NULL) {
        arg_info->type_op = OP_STORE;
        ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
    }

    arg_info->type_op = OP_UNKNOWN;
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn ACGbinop
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
ACGbinop (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGbinop");

    int temp_cnt_jump = 0;
    bool temp_code_gen = FALSE;

    if (BINOP_OP(arg_node) != BO_or && BINOP_OP(arg_node)) {

        arg_info->type_op = OP_LOAD;
        BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);

        arg_info->type_op = OP_LOAD;
        BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

        switch (arg_info->current_type) {
            case T_int:
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "\ti");
                }
                break;
            case T_float:
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "\tf");
                }
                break;
            case T_bool:
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "\tb");
                }
                break;
            case T_void:
                break;
            case T_unknown:
                DBUG_ASSERT(0, "unknown type detected!\n");
        }

        switch (BINOP_OP(arg_node)) {
            case BO_add:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "add\n");
                }
                break;
            case BO_sub:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "sub\n");
                }
                break;
            case BO_mul:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "mul\n");
                }
                break;
            case BO_div:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "div\n");
                }
                break;
            case BO_mod:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "mod\n");
                }
                break;
            case BO_lt:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "lt\n");
                }
                break;
            case BO_le:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "le\n");
                }
                break;
            case BO_gt:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "gt\n");
                }
                break;
            case BO_ge:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "ge\n");
                }
                break;
            case BO_eq:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "eq\n");
                }
                break;
            case BO_ne:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "ne\n");
                }
                break;
            case BO_or:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "or\n");
                }
                break;
            case BO_and:
                arg_info->cnt_jump = arg_info->cnt_jump + 1;
                if (arg_info->as_code_gen == TRUE) {
                    fprintf(pFile, "and\n");
                }
                break;
            case BO_unknown:
                DBUG_ASSERT(0, "unknown binop detected!\n");
        }
    }
    else if (BINOP_OP(arg_node) == BO_or){
        arg_info->type_op = OP_LOAD;
        BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);

        arg_info->cnt_jump = arg_info->cnt_jump
                + 1 + 2  // branch_f 4
                + 1      // bloadc_t
                + 1 + 2; // jump #

        /*store the information in arg_info*/
        temp_cnt_jump = arg_info->cnt_jump;
        temp_code_gen = arg_info->as_code_gen;

        arg_info->cnt_jump = 1; //start with 1
        arg_info->as_code_gen = FALSE;
        arg_info->type_op = OP_LOAD;
        BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);


        /*recover the information in arg_info*/
        arg_info->as_code_gen = temp_code_gen;
        arg_info->cnt_jump += temp_cnt_jump;
        if (arg_info->as_code_gen == TRUE) {
            fprintf(pFile, "\tbranch_f 4\n"); //to the instruction after jump
            fprintf(pFile, "\tbloadc_t\n");
            fprintf(pFile, "\tjump %d\n", arg_info->cnt_jump-temp_cnt_jump);
            
            arg_info->type_op = OP_LOAD;
            BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
        }
    }
    else if (BINOP_OP(arg_node) == BO_and){
        arg_info->type_op = OP_LOAD;
        BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);

        arg_info->cnt_jump = arg_info->cnt_jump
                + 1 + 2  // branch_t 4
                + 1      // bloadc_f
                + 1 + 2; // jump #

        /*store the information in arg_info*/
        temp_cnt_jump = arg_info->cnt_jump;
        temp_code_gen = arg_info->as_code_gen;

        arg_info->cnt_jump = 1; //start with 1
        arg_info->as_code_gen = FALSE;
        arg_info->type_op = OP_LOAD;
        BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);


        arg_info->as_code_gen = temp_code_gen;
        arg_info->cnt_jump += temp_cnt_jump;
        if (arg_info->as_code_gen == TRUE) {
            fprintf(pFile, "\tbranch_t %d\n", 4); //to the instruction after jump
            fprintf(pFile, "\tbloadc_f\n");
            fprintf(pFile, "\tjump %d\n", arg_info->cnt_jump-temp_cnt_jump);

            arg_info->type_op = OP_LOAD;
            BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
        }
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn ACGfloat
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
ACGfloat (node * arg_node, info * arg_info)
{
    float num;
    arg_info->current_type = T_float;

    DBUG_ENTER ("ACGfloat");

    num = NUM_VALUE( arg_node);

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn ACGnum
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
ACGnum (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGnum");

    arg_info->current_type = T_int;

    if ( NUM_VALUE( arg_node) == 0 && arg_info->type_op == OP_LOAD )
    {
        fprintf (pFile, "\tiloadc_0\n" );
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn ACGboolean
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
ACGbool (node * arg_node, info * arg_info)
{
    arg_info->current_type = T_bool;

    DBUG_ENTER ("ACGbool");

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
ACGvarlet (node * arg_node, info * arg_info)
{
    if (VARLET_INFO (arg_node) != NULL)
    {
        VARLET_INFO (arg_node) = TRAVdo (VARLET_INFO (arg_node), arg_info);
    }

    return (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn ACGerror
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
ACGerror (node * arg_node, info * arg_info)
{
    bool first_error;

    DBUG_ENTER ("ACGerror");

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
ACGfundef(node * arg_node, info * arg_info)
{
    arg_info->fun_type = FUN_UNKNOWN;
    arg_info->cnt_esr = 0;

    //for import fun decl
    //should printf expr like '.import "scanFloat" float'
    if (FUNDEF_ISEXTERN(arg_node) == TRUE)
    {
        fprintf (pFile, ".import " );
        fprintf (pFile, "\"%s\" ", FUNDEF_NAME (arg_node) );

        print_type_name(FUNDEF_RETTYPE (arg_node));

        arg_info->fun_type = FUN_EXTERN_DECL;

        if (FUNDEF_PARAMS( arg_node) != NULL) {
            FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
        }
        fprintf (pFile, "\n" );
        return (arg_node);
    }

    //for export fun decl
    if (FUNDEF_ISEXPORT(arg_node) == TRUE)
    {
        fprintf (pFile, ".export " );
        fprintf (pFile, "\"%s\" ", FUNDEF_NAME (arg_node) );

        print_type_name(FUNDEF_RETTYPE (arg_node));

        arg_info->fun_type = FUN_EXPORT_DECL;

        if (FUNDEF_PARAMS( arg_node) != NULL) {
            FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
        }

        if ( STReq(FUNDEF_NAME (arg_node), "main") == TRUE )
        {
            fprintf (pFile, "main" );
        }
        fprintf (pFile, "\n" );
    }

    if (FUNDEF_LOCALFUNS ( arg_node) != NULL) {
        FUNDEF_LOCALFUNS ( arg_node) = TRAVdo( FUNDEF_LOCALFUNS ( arg_node), arg_info);
    }

    //for all fundef
    fprintf (pFile, "%s:\n", FUNDEF_NAME (arg_node) );

    arg_info->fun_type = FUN_UNKNOWN;

    //first traverse params node and vardecs node to get the param for 'esr'
    if (FUNDEF_PARAMS( arg_node) != NULL) {
        arg_info->param_list  = FUNDEF_PARAMS(arg_node);
        FUNDEF_PARAMS( arg_node) = TRAVdo( FUNDEF_PARAMS( arg_node), arg_info);
    }

    if (FUNDEF_VARDECS( arg_node) != NULL) {
        arg_info->vardec_list = FUNDEF_VARDECS(arg_node);
        FUNDEF_VARDECS( arg_node) = TRAVdo( FUNDEF_VARDECS( arg_node), arg_info);
    }

    fprintf (pFile, "\tesr %d\n", arg_info->cnt_esr );

    arg_info->fun_type = FUN_DEF;

    //traverse vardecs node again in case any vardec has init operation
    if (FUNDEF_VARDECS( arg_node) != NULL) {
        FUNDEF_VARDECS( arg_node) = TRAVdo( FUNDEF_VARDECS( arg_node), arg_info);
    }

    if (FUNDEF_BODY( arg_node) != NULL) {
        FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
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
ACGglobaldec(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGglobaldec");

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
ACGglobaldef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGglobaldef");

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
ACGvardec(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGvardec");

    //traverse vardec node to increment the number of local var and then return
    if ( arg_info->fun_type == FUN_UNKNOWN )
    {
        arg_info->cnt_esr = arg_info->cnt_esr + 1;

        if (VARDEC_NEXT( arg_node) != NULL)
        {
            VARDEC_NEXT( arg_node) = TRAVopt( VARDEC_NEXT( arg_node), arg_info);
        }
        DBUG_RETURN (arg_node);
    }

    //if the init expr is empty, don't do anything
    //if not, first load expr, then store var
    if (VARDEC_VARINFO( arg_node) != NULL && VARDEC_INIT( arg_node) != NULL )
    {
        arg_info->type_op = OP_LOAD;
        VARDEC_INIT( arg_node) = TRAVopt( VARDEC_INIT( arg_node), arg_info);

        arg_info->type_op = OP_STORE;
        VARDEC_VARINFO( arg_node) = TRAVdo( VARDEC_VARINFO( arg_node), arg_info);
    }

    if (VARDEC_NEXT( arg_node) != NULL)
    {
        VARDEC_NEXT( arg_node) = TRAVopt( VARDEC_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);

}




/** <!--******************************************************************-->
 *
 * @add by michael for node vardefs
 ***************************************************************************/

node *
ACGvardefs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGvardefs");

    DBUG_RETURN (arg_node);
}




/** <!--******************************************************************-->
 *
 * @add by michael for node exprs
 ***************************************************************************/

node *
ACGexprs(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGexprs");

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
ACGwhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGwhile");

    int cnt_cond_offset = 0;              //the number of cond assembly code offsets
    int cnt_instrs_offset = 0;            //the number of instrs code offsets

    //do the first traverse only to get the value of the offset
    arg_info->as_code_gen = FALSE;
    arg_info->cnt_jump = 0;
    if (WHILE_PRED( arg_node) != NULL) {
        WHILE_PRED( arg_node) = TRAVdo( WHILE_PRED( arg_node), arg_info);
    }
    cnt_cond_offset = arg_info->cnt_jump;

    //do the first traverse only to get the value of the offset
    arg_info->as_code_gen = FALSE;
    arg_info->cnt_jump = 0;
    if (WHILE_BODY( arg_node) != NULL)
    {
        WHILE_BODY( arg_node) = TRAVdo( WHILE_BODY( arg_node), arg_info);
    }
    cnt_instrs_offset = arg_info->cnt_jump;

    //do the second traverse to generate assembly code
    arg_info->as_code_gen = TRUE;
    arg_info->cnt_jump = 0;
    if (WHILE_PRED( arg_node) != NULL) {
        WHILE_PRED( arg_node) = TRAVdo( WHILE_PRED( arg_node), arg_info);
    }

    //+3 because branch_f is one byte, the number followed is two byte
    //again +3 because jump is one byte, the number followed is two byte
    fprintf (pFile, "\tbranch_f %d\n", cnt_instrs_offset + 3 + 3);

    //do the first traverse only to get the value of the offset
    arg_info->as_code_gen = TRUE;
    arg_info->cnt_jump = 0;
    if (WHILE_BODY( arg_node) != NULL)
    {
        WHILE_BODY( arg_node) = TRAVdo( WHILE_BODY( arg_node), arg_info);
    }

    fprintf (pFile, "\tjump %d\n", -(cnt_instrs_offset + 3  + cnt_cond_offset) );

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node dowhile
 ***************************************************************************/

node *
ACGdowhile(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGdowhile");

    //do the traverse go get the offset and generate code at the same time
    arg_info->as_code_gen = TRUE;
    arg_info->cnt_jump = 0;
    if (DOWHILE_BODY( arg_node) != NULL) {
        DOWHILE_BODY( arg_node) = TRAVdo( DOWHILE_BODY( arg_node), arg_info);
    }

    if (DOWHILE_PRED( arg_node) != NULL) {
        DOWHILE_PRED( arg_node) = TRAVdo( DOWHILE_PRED( arg_node), arg_info);
    }

    fprintf (pFile, "\tbranch_t %d\n", -arg_info->cnt_jump );
    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node for
 ***************************************************************************/

node *
ACGfor(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGfor");

    if (FOR_VAR( arg_node) != NULL) {
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

    DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @add by michael for node return
 ***************************************************************************/

node *
ACGreturn(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGreturn");

    arg_info->type_op = OP_LOAD;
    RETURN_EXPR( arg_node) = TRAVopt( RETURN_EXPR( arg_node), arg_info);

    if ( arg_info->current_type == T_int )
    {
        fprintf (pFile, "\tireturn\n" );
    }
    else if ( arg_info->current_type == T_float )
    {
        fprintf (pFile, "\tfreturn\n" );
    }
    else if ( arg_info->current_type == T_bool )
    {
        fprintf (pFile, "\tbreturn\n" );
    }
    else
    {
        DBUG_ASSERT( 0, "unknown return type detected!\n");
    }

    DBUG_RETURN (arg_node);
}




/** <!--******************************************************************-->
 *
 * @add by michael for node vardef
 ***************************************************************************/

node *
ACGvardef(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGvardef");

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node cast
 ***************************************************************************/

node *
ACGcast(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGcast");

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node monop
 ***************************************************************************/

node *
ACGmonop(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGmonop");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node module
 ***************************************************************************/

node *
ACGmodule(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGmodule");

    if (MODULE_MODULEDECLS( arg_node) != NULL) {
        MODULE_MODULEDECLS( arg_node) = TRAVdo( MODULE_MODULEDECLS( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @add by michael for node ACGvarinfo
 ***************************************************************************/

node *
ACGvarinfo(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGvarinfo");

    node *vardec_list = MakeEmptyNode();
    NODE_TYPE (vardec_list) = N_vardec;
    vardec_list = arg_info->vardec_list;

    node *param_list = MakeEmptyNode();
    NODE_TYPE (param_list) = N_param;
    param_list = arg_info->param_list;

    node *global_list = MakeEmptyNode();
    NODE_TYPE (global_list) = N_moduledecls;
    global_list = arg_info->global_decl_list;

    //if is fundecl, just print the type of param, then return
    if ( arg_info->fun_type == FUN_EXPORT_DECL || arg_info->fun_type == FUN_EXTERN_DECL )
    {
        print_type_name(VARINFO_TYPE( arg_node));  //print the type in fundecl, eg, .import "scanFloat" float
        fprintf (pFile, " " );
        DBUG_RETURN (arg_node);
    }

    //first decide the type of var
    if ( VARINFO_TYPE( arg_node) == T_int )
    {
        arg_info->current_type = T_int;

        if (arg_info->as_code_gen == TRUE)
        {
            fprintf (pFile, "\ti");
        }
    }
    else if ( VARINFO_TYPE( arg_node) == T_float )
    {
        arg_info->current_type = T_float;

        if (arg_info->as_code_gen == TRUE)
        {
            fprintf (pFile, "\tf");
        }
    }
    else if ( VARINFO_TYPE( arg_node) == T_bool )
    {
        arg_info->current_type = T_bool;

        if (arg_info->as_code_gen == TRUE)
        {
            fprintf (pFile, "\tb");
        }
    }
    else
    {
        DBUG_ASSERT( 0, "wrong var type!\n");
    }

    //then decide the operation of the var
    if ( arg_info->type_op == OP_LOAD )
    {
        arg_info->cnt_jump = arg_info->cnt_jump + 1;

        if (arg_info->as_code_gen == TRUE)
        {
            fprintf (pFile, "load ");
        }
    }
    else if ( arg_info->type_op == OP_STORE )
    {
        arg_info->cnt_jump = arg_info->cnt_jump + 1;

        if (arg_info->as_code_gen == TRUE)
        {
            fprintf (pFile, "store ");
        }
    }
    else
    {
     //   DBUG_ASSERT( 0, "wrong op type!\n");
    }

    int index = 0;

    //then find the index of the var in the current frame
    if ( VARINFO_NAME( arg_node) != NULL )
    {
        //first check the param list
        while ( param_list != NULL )
        {
            if ( STReq(VARINFO_NAME(arg_node), VARINFO_NAME(PARAM_VARINFO(param_list)) ) == TRUE )
            {
                arg_info->cnt_jump = arg_info->cnt_jump + 1;

                if (arg_info->as_code_gen == TRUE)
                {
                    fprintf (pFile, "%d\n", index);
                }

                break;
            }
            index++;
            param_list = PARAM_NEXT(param_list);
        }

        //first check the vardec list
        while ( vardec_list != NULL )
        {
            if ( STReq(VARINFO_NAME(arg_node), VARINFO_NAME(VARDEC_VARINFO(vardec_list)) ) == TRUE )
            {
                arg_info->cnt_jump = arg_info->cnt_jump + 1;

                if (arg_info->as_code_gen == TRUE)
                {
                    fprintf (pFile, "%d\n", index);
                }
                break;
            }
            index++;
            vardec_list = VARDEC_NEXT(vardec_list);
        }

        index = 0;

        //finally check the global list
        while ( global_list != NULL )
        {
            if ( NODE_TYPE (MODULEDECLS_MODULEDECL (global_list)) == N_globaldef )
            {
                if ( STReq(VARINFO_NAME(arg_node), VARINFO_NAME(GLOBALDEF_VARINFO(MODULEDECLS_MODULEDECL (global_list))) ) == TRUE )
                {
                    arg_info->cnt_jump = arg_info->cnt_jump + 1;

                    if (arg_info->as_code_gen == TRUE)
                    {
                        fprintf (pFile, "%d\n", index);
                    }
                    break;
                }
            }
            index++;
            global_list = MODULEDECLS_NEXT(global_list);
        }
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node ACGparam
 ***************************************************************************/

node *
ACGparam(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGparam");

    //for esr, we need to get the total number of params and local vars
    if ( arg_info->fun_type == FUN_UNKNOWN )
    {
        arg_info->cnt_esr = arg_info->cnt_esr + 1;
        DBUG_RETURN (arg_node);
    }

    if (PARAM_VARINFO( arg_node) != NULL)
    {
        PARAM_VARINFO( arg_node) = TRAVdo( PARAM_VARINFO( arg_node), arg_info);
    }

    if (PARAM_NEXT( arg_node) != NULL)
    {
        PARAM_NEXT( arg_node) = TRAVdo( PARAM_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node ACGif
 ***************************************************************************/

node *
ACGif(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGif");

    if (IF_PRED( arg_node) != NULL) {
        IF_PRED( arg_node) = TRAVdo( IF_PRED( arg_node), arg_info);
    }

    //do the first traverse only to get the value of the offset
    arg_info->as_code_gen = FALSE;
    arg_info->cnt_jump = 0;
    if (IF_THEN( arg_node) != NULL)
    {
        IF_THEN( arg_node) = TRAVdo( IF_THEN( arg_node), arg_info);
    }

    //+3 because branch_f is one byte and the number is two byte
    //again +3 because jump is one byte and the number is two byte
    fprintf (pFile, "\tbranch_f %d\n", arg_info->cnt_jump + 3 + 3);


    //do the second traverse to generate code
    arg_info->as_code_gen = TRUE;
    arg_info->cnt_jump = 0;
    if (IF_THEN( arg_node) != NULL)
    {
        IF_THEN( arg_node) = TRAVdo( IF_THEN( arg_node), arg_info);
    }

    //do the first traverse only to get the value of the offset
    arg_info->as_code_gen = FALSE;
    arg_info->cnt_jump = 0;
    if (IF_ELSE( arg_node) != NULL)
    {
        IF_ELSE( arg_node) = TRAVdo( IF_ELSE( arg_node), arg_info);
    }

    fprintf (pFile, "\tjump %d\n", arg_info->cnt_jump + 3);

    //do the second traverse to generate code
    arg_info->as_code_gen = TRUE;
    arg_info->cnt_jump = 0;
    if (IF_ELSE( arg_node) != NULL)
    {
        IF_ELSE( arg_node) = TRAVdo( IF_ELSE( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/*
 * author: michael
 * time: April 5th 2010
 * assembly code generate for funap
 * first check whether is extern fun or not
 * if is extern isrg jsre id
 * if not isr jsr number_param offset
 */
node *
ACGfunap(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGfunap");

    node *global_list = MakeEmptyNode();
    NODE_TYPE (global_list) = N_moduledecls;
    global_list = arg_info->global_decl_list;

    int  extern_index = 0;

    //first find whether funap is gloabl extern or not
    while ( global_list != NULL )
    {
        if ( NODE_TYPE (MODULEDECLS_MODULEDECL (global_list)) == N_fundef )
        {
            printf("function in the global list is %s\n", FUNDEF_NAME(MODULEDECLS_MODULEDECL (global_list)) );
            printf("function call is %s\n", FUNAP_NAME(arg_node));


            //if funap is extern function
            if ( FUNDEF_ISEXTERN(MODULEDECLS_MODULEDECL (global_list)) == TRUE )
            {
                if ( STReq(FUNAP_NAME(arg_node), FUNDEF_NAME(MODULEDECLS_MODULEDECL (global_list)) ) == TRUE )
                {
                    fprintf (pFile, "\tisrg\n");

                    if (FUNAP_ARGS(arg_node) != NULL)
                    {
                        //it is rather naive solution here, will make a distinction between fundef and funap in the future
                        arg_info->fun_type = FUN_DEF;
                        arg_info->type_op = OP_LOAD;
                        FUNAP_ARGS( arg_node) = TRAVdo( FUNAP_ARGS( arg_node), arg_info);
                    }

                    fprintf (pFile, "\tjsre %d\n", extern_index );

                    break;
                }
                extern_index++;
            }
            else   //funap is local function
            {
                if ( STReq(FUNAP_NAME(arg_node), FUNDEF_NAME(MODULEDECLS_MODULEDECL (global_list)) ) == TRUE )
                {
                    fprintf (pFile, "\tisr\n" );

                    if (FUNAP_ARGS(arg_node) != NULL)
                    {
                        //it is rather naive solution here, will make a distinction between fundef and funap in the future
                        arg_info->fun_type = FUN_DEF;
                        arg_info->type_op = OP_LOAD;

                        arg_info->cnt_arg = 0; //reset the number of arg before traverse
                        FUNAP_ARGS( arg_node) = TRAVdo( FUNAP_ARGS( arg_node), arg_info);
                    }

                    //jsr + number_param + offset label
                    fprintf (pFile, "\tjsr %d %s\n", arg_info->cnt_arg, FUNAP_NAME( arg_node) );
                    break;
                }
            }
        }

        global_list = MODULEDECLS_NEXT(global_list);
    }

    DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @add by michael for node ACGarg
 ***************************************************************************/

node *
ACGarg(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGarg");

    arg_info->cnt_arg = arg_info->cnt_arg + 1;

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
 * @add by michael for node ACGmoduledecls
 ***************************************************************************/

node *
ACGmoduledecls(node * arg_node, info * arg_info)
{
    DBUG_ENTER ("ACGmoduledecls");

    if (MODULEDECLS_MODULEDECL( arg_node) != NULL) {

        MODULEDECLS_MODULEDECL( arg_node) = TRAVopt( MODULEDECLS_MODULEDECL( arg_node), arg_info);
    }

    if (MODULEDECLS_NEXT(arg_node) != NULL) {
        MODULEDECLS_NEXT( arg_node) = TRAVdo( MODULEDECLS_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN (arg_node);
}


/*
 * author: michael
 * time: April 3rd 2010
 * print the name of type according to the int value
 */
void print_type_name(type t)
{
    switch (t)
    {
    case T_int:
        fprintf (pFile, "int ");
        break;
    case T_float:
        fprintf (pFile, "float ");
        break;
    case T_bool:
        fprintf (pFile, "bool ");
        break;
    case T_void:
        fprintf (pFile, "void ");
        break;
    case T_unknown:
        DBUG_ASSERT( 0, "unknown variable type detected!");
        break;
    }
}


/** <!-- ****************************************************************** -->
 * @brief ACGdoContextAnalysis do the context analysis
 *
 * @param syntaxtree a node structure
 *
 ******************************************************************************/

node
*ACGdoAssemblyCodeGen( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("ACGdoContextAnalysis");

    DBUG_ASSERT( (syntaxtree!= NULL), "ACGdoContextAnalysis called with empty syntaxtree");

    printf( "\n\n------------------------------\n\n");

    info = MakeInfo();
    info->global_decl_list = syntaxtree;

    pFile = fopen ("a.as","w");

    fprintf (pFile, ".export \"__init\" void __init\n" );
    fprintf (pFile, "__init:\n" );
    fprintf (pFile, "\tesr 0\n" );
    fprintf (pFile, "\treturn\n" );
    TRAVpush( TR_acg);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);
    fclose (pFile);
    printf( "\n\n------------------------------\n\n");

    DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
