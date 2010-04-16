
/**
 * @file print.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _SAC_PRT_NODE_H_
#define _SAC_PRT_NODE_H_

#include "types.h"

extern node *PRTinstrs (node * arg_node, info * arg_info);
extern node *PRTassign (node * arg_node, info * arg_info);
extern node *PRTvar (node * arg_node, info * arg_info);
extern node *PRTvarlet (node * arg_node, info * arg_info);
extern node *PRTbinop (node * arg_node, info * arg_info);
extern node *PRTfloat (node * arg_node, info * arg_info);
extern node *PRTnum (node * arg_node, info * arg_info);
extern node *PRTbool (node * arg_node, info * arg_info);
extern node *PRTerror (node * arg_node, info * arg_info);

extern node *PRTfundec(node * arg_node, info * arg_info);
extern node *PRTfundef(node * arg_node, info * arg_info);
extern node *PRTvardec(node * arg_node, info * arg_info);

extern node *PRTvardecs(node * arg_node, info * arg_info);
extern node *PRTfundefs(node * arg_node, info * arg_info);
extern node *PRTexprs(node * arg_node, info * arg_info);

extern node *PRTwhile(node * arg_node, info * arg_info);
extern node *PRTdowhile(node * arg_node, info * arg_info);
extern node *PRTfor(node * arg_node, info * arg_info);
extern node *PRTreturn(node * arg_node, info * arg_info);;

extern node *PRTvar(node * arg_node, info * arg_info);
extern node *PRTmonop(node * arg_node, info * arg_info);
extern node *PRTcast(node * arg_node, info * arg_info);

extern node *PRTglobaldec(node * arg_node, info * arg_info);
extern node *PRTglobaldef(node * arg_node, info * arg_info);

extern node *PRTmoduledecls(node * arg_node, info * arg_info);
extern node *PRTmodule(node * arg_node, info * arg_info);
extern node *PRTvarinfo(node * arg_node, info * arg_info);
extern node *PRTparam(node * arg_node, info * arg_info);
extern node *PRTif(node * arg_node, info * arg_info);
extern node *PRTfunap(node * arg_node, info * arg_info);
extern node *PRTarg(node * arg_node, info * arg_info);


extern node *PRTdoPrint( node *syntaxtree);

#endif /* _SAC_PRT_NODE_H_ */
