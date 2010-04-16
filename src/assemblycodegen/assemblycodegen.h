
/**
 * @file print.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _ASSEMBLY_CODE_GEN_H_
#define _ASSEMBLY_CODE_GEN_H_

#define OP_UNKNOWN   0
#define OP_LOAD      1
#define OP_STORE     2

#define FUN_UNKNOWN          0
#define FUN_EXTERN_DECL      1
#define FUN_EXPORT_DECL      2
#define FUN_DEF              3


#include "types.h"

extern void  print_type_name(type t);
extern node *ACGinstrs (node * arg_node, info * arg_info);
extern node *ACGassign (node * arg_node, info * arg_info);
extern node *ACGvar (node * arg_node, info * arg_info);
extern node *ACGvarlet (node * arg_node, info * arg_info);
extern node *ACGbinop (node * arg_node, info * arg_info);
extern node *ACGfloat (node * arg_node, info * arg_info);
extern node *ACGnum (node * arg_node, info * arg_info);
extern node *ACGbool (node * arg_node, info * arg_info);
extern node *ACGerror (node * arg_node, info * arg_info);

extern node *ACGfundef(node * arg_node, info * arg_info);
extern node *ACGvardec(node * arg_node, info * arg_info);

extern node *ACGexprs(node * arg_node, info * arg_info);

extern node *ACGwhile(node * arg_node, info * arg_info);
extern node *ACGdowhile(node * arg_node, info * arg_info);
extern node *ACGfor(node * arg_node, info * arg_info);
extern node *ACGreturn(node * arg_node, info * arg_info);

extern node *ACGvar(node * arg_node, info * arg_info);
extern node *ACGmonop(node * arg_node, info * arg_info);
extern node *ACGcast(node * arg_node, info * arg_info);

extern node *ACGglobaldec(node * arg_node, info * arg_info);
extern node *ACGglobaldef(node * arg_node, info * arg_info);

extern node *ACGmoduledecls(node * arg_node, info * arg_info);
extern node *ACGmodule(node * arg_node, info * arg_info);
extern node *ACGvarinfo(node * arg_node, info * arg_info);
extern node *ACGparam(node * arg_node, info * arg_info);
extern node *ACGif(node * arg_node, info * arg_info);
extern node *ACGfunap(node * arg_node, info * arg_info);
extern node *ACGarg(node * arg_node, info * arg_info);
#endif /* _ASSEMBLY_CODE_GEN_H_*/
