/* ---------------------------------------------------------------------------
 * 
 * SAC Compiler Construction Framework
 * 
 * ---------------------------------------------------------------------------
 * 
 * SAC COPYRIGHT NOTICE, LICENSE, AND DISCLAIMER
 * 
 * (c) Copyright 1994 - 2010 by
 * 
 *   SAC Development Team
 *   SAC Research Foundation
 * 
 *   http://www.sac-home.org
 *   email:info@sac-home.org
 * 
 *   All rights reserved
 * 
 * ---------------------------------------------------------------------------
 * 
 * The SAC compiler construction framework, all accompanying 
 * software and documentation (in the following named this software)
 * is developed by the SAC Development Team (in the following named
 * the developer) which reserves all rights on this software.
 * 
 * Permission to use this software is hereby granted free of charge
 * exclusively for the duration and purpose of the course 
 *   "Compilers and Operating Systems" 
 * of the MSc programme Grid Computing at the University of Amsterdam.
 * Redistribution of the software or any parts thereof as well as any
 * alteration  of the software or any parts thereof other than those 
 * required to use the compiler construction framework for the purpose
 * of the above mentioned course are not permitted.
 * 
 * The developer disclaims all warranties with regard to this software,
 * including all implied warranties of merchantability and fitness.  In no
 * event shall the developer be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data, or profits, whether in an action of contract, negligence, or
 * other tortuous action, arising out of or in connection with the use or
 * performance of this software. The entire risk as to the quality and
 * performance of this software is with you. Should this software prove
 * defective, you assume the cost of all servicing, repair, or correction.
 * 
 * ---------------------------------------------------------------------------
 */ 



/*
 *
 * $Id: traverse.c 14483 2005-12-14 17:13:58Z ktr $
 *
 */

#include "traverse.h"
#include "traverse_tables.h"
#include "traverse_helper.h"
#include "free.h"
#include "memory.h"
#include "dbug.h"
#include "globals.h"
#include "tree_basic.h"

struct TRAVSTACK_T {
  struct TRAVSTACK_T *next;
  travfun_p           *funs;
  trav_t              traversal;
};

typedef struct TRAVSTACK_T travstack_t;

static travstack_t *travstack = NULL;

node *TRAVdo(node *arg_node, info *arg_info)
{
  DBUG_PRINT ("MAKE", ("TRAVdo enters"));

  DBUG_ASSERT( (arg_node != NULL),
                 "Trav: tried to traverse into subtree NULL !");
  
  DBUG_ASSERT( (NODE_TYPE( arg_node) <= MAX_NODES),
               "Trav: illegal node type !");

  DBUG_ASSERT( (travstack != NULL),
      "no traversal on stack!");

  global.line = NODE_LINE( arg_node);

  if (pretable[ travstack->traversal] != NULL) {
    arg_node = pretable[ travstack->traversal]( arg_node, arg_info);
  }

  arg_node = (travstack->funs[ NODE_TYPE( arg_node)])( arg_node, arg_info);

  if (posttable[ travstack->traversal] != NULL) {
    arg_node = posttable[ travstack->traversal]( arg_node, arg_info);
  }

  return( arg_node);
}

node *TRAVopt( node *arg_node, info *arg_info)
{
  DBUG_PRINT ("MAKE", ("TRAVopt enters"));

  if (arg_node != NULL) {
    arg_node = TRAVdo( arg_node, arg_info);
  }
  
  return( arg_node);
}

node *TRAVcont( node *arg_node, info *arg_info)
{
  arg_node = TRAVsons( arg_node, arg_info);

  return( arg_node);
}

void TRAVpush( trav_t traversal)
{
  travstack_t *new;

  DBUG_ENTER("TRAVpush");

  new = MEMmalloc( sizeof( travstack_t));

  new->next = travstack;
  new->traversal = traversal;
  new->funs = travtables[ traversal];

  travstack = new;

  DBUG_VOID_RETURN;
}

trav_t TRAVpop() {
  travstack_t *tmp;
  trav_t result;

  DBUG_ENTER("TRAVpop");

  DBUG_ASSERT( (travstack != NULL),
      "no traversal on stack!");

  tmp = travstack;
  travstack = tmp->next;
  result = tmp->traversal;

  tmp = MEMfree( tmp);

  DBUG_RETURN( result);
}

const char *TRAVgetName()
{
  const char *result;

  DBUG_ENTER("TRAVgetName");

  if (travstack == NULL) {
    result = "no_active_traversal";
  } else {
    result = travnames[ travstack->traversal];
  }

  DBUG_RETURN( result);
}

void TRAVsetPreFun( trav_t traversal, travfun_p prefun)
{
  DBUG_ENTER("TRAVsetPreFun");

  pretable[traversal] = prefun;

  DBUG_VOID_RETURN;
}

void TRAVsetPostFun( trav_t traversal, travfun_p postfun)
{
  DBUG_ENTER("TRAVsetPreFun");

  posttable[traversal] = postfun;

  DBUG_VOID_RETURN;
}
