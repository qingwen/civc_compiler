
/**
 * @file print.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _CONTEXT_ANALYSIS_H_
#define _CONTEXT_ANALYSIS_H_

#include "types.h"

extern node *CAinstrs (node * arg_node, info * arg_info);
extern node *CAassign (node * arg_node, info * arg_info);
extern node *CAvar (node * arg_node, info * arg_info);
extern node *CAvarlet (node * arg_node, info * arg_info);
extern node *CAbinop (node * arg_node, info * arg_info);
extern node *CAfloat (node * arg_node, info * arg_info);
extern node *CAnum (node * arg_node, info * arg_info);
extern node *CAbool (node * arg_node, info * arg_info);
extern node *CAerror (node * arg_node, info * arg_info);

extern node *CAfundef(node * arg_node, info * arg_info);;
extern node *CAvardec(node * arg_node, info * arg_info);

extern node *CAvardecs(node * arg_node, info * arg_info);
extern node *CAfundefs(node * arg_node, info * arg_info);
extern node *CAexprs(node * arg_node, info * arg_info);

extern node *CAwhile(node * arg_node, info * arg_info);
extern node *CAdowhile(node * arg_node, info * arg_info);
extern node *CAfor(node * arg_node, info * arg_info);
extern node *CAreturn(node * arg_node, info * arg_info);

extern node *CAvar(node * arg_node, info * arg_info);
extern node *CAmonop(node * arg_node, info * arg_info);
extern node *CAcast(node * arg_node, info * arg_info);

extern node *CAglobaldef(node * arg_node, info * arg_info);

extern node *CAmoduledecls(node * arg_node, info * arg_info);
extern node *CAmodule(node * arg_node, info * arg_info);
extern node *CAvarinfo(node * arg_node, info * arg_info);
extern node *CAparam(node * arg_node, info * arg_info);
extern node *CAif(node * arg_node, info * arg_info);
extern node *CAfunap(node * arg_node, info * arg_info);
extern node *CAarg(node * arg_node, info * arg_info);
#endif /* _CONTEXT_ANALYSIS_H_*/
