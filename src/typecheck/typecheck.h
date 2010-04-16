
/**
 * @file typecheck.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _TYPE_CHECK_H_
#define _TYPE_CHECK_H_

#include "types.h"

extern node *TCinstrs (node * arg_node, info * arg_info);
extern node *TCassign (node * arg_node, info * arg_info);
extern node *TCvar (node * arg_node, info * arg_info);
extern node *TCvarlet (node * arg_node, info * arg_info);
extern node *TCbinop (node * arg_node, info * arg_info);
extern node *TCfloat (node * arg_node, info * arg_info);
extern node *TCnum (node * arg_node, info * arg_info);
extern node *TCbool (node * arg_node, info * arg_info);
extern node *TCerror (node * arg_node, info * arg_info);

extern node *TCfundef(node * arg_node, info * arg_info);
extern node *TCvardec(node * arg_node, info * arg_info);

extern node *TCvardecs(node * arg_node, info * arg_info);
extern node *TCfundefs(node * arg_node, info * arg_info);
extern node *TCexprs(node * arg_node, info * arg_info);

extern node *TCwhile(node * arg_node, info * arg_info);
extern node *TCdowhile(node * arg_node, info * arg_info);
extern node *TCfor(node * arg_node, info * arg_info);
extern node *TCreturn(node * arg_node, info * arg_info);

extern node *TCvar(node * arg_node, info * arg_info);
extern node *TCmonop(node * arg_node, info * arg_info);
extern node *TCcast(node * arg_node, info * arg_info);

extern node *TCglobaldec(node * arg_node, info * arg_info);
extern node *TCglobaldef(node * arg_node, info * arg_info);

extern node *TCmoduledecls(node * arg_node, info * arg_info);
extern node *TCmodule(node * arg_node, info * arg_info);
extern node *TCvarinfo(node * arg_node, info * arg_info);
extern node *TCparam(node * arg_node, info * arg_info);
extern node *TCif(node * arg_node, info * arg_info);
extern node *TCfunap(node * arg_node, info * arg_info);
extern node *TCarg(node * arg_node, info * arg_info);
extern node *TCdoTypeCheck( node *syntaxtree);

#endif /* _TYPE_CHECK_H_ */
