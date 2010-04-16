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
 * $Id: free.c 14440 2005-11-25 16:08:28Z ktr $
 */



#include "free.h"
#include "dbug.h"
#include "traverse.h"
#include "memory.h"
#include "free_info.h"
#include "tree_basic.h"

/*
 * INFO functions
 */

static info *
MakeInfo ()
{
  info *result;

  DBUG_ENTER ("MakeInfo");

  result = MEMmalloc (sizeof (info));

  INFO_FREE_FLAG (result) = NULL;
  INFO_FREE_ASSIGN (result) = NULL;

  DBUG_RETURN (result);
}

static info *
FreeInfo (info * info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree (info);

  DBUG_RETURN (info);
}

/******************************************************************************
 *
 * Function:
 *   node *FREEdoFreeNode( node *free_node)
 *
 * Description:
 *   - if 'free_node' is not a N_fundef node:
 *        Removes the given node and returns a pointer to the NEXT node if it
 *        exists, NULL otherwise.
 *   - if 'free_node' is a N_fundef node:
 *        Transforms the given fundef into a zombie and returns it.
 *
 ******************************************************************************/

node *
FREEdoFreeNode (node * free_node)
{
  info *arg_info;

  DBUG_ENTER ("FREEfreeNode");

  arg_info = MakeInfo ();

  INFO_FREE_FLAG (arg_info) = free_node;

  TRAVpush (TR_free);

  free_node = TRAVdo (free_node, arg_info);

  TRAVpop ();

  arg_info = FreeInfo (arg_info);

  DBUG_RETURN (free_node);
}


/******************************************************************************
 *
 * Function:
 *   node *FREEdoFreeTree( node *free_node)
 *
 * Description:
 *   - if 'free_node' is not a N_fundef node:
 *        Removes the whole sub tree behind the given pointer.
 *   - if 'free_node' is a N_fundef node:
 *        Transforms the whole fundef chain into zombies and returns it.
 *
 ******************************************************************************/

node *
FREEdoFreeTree (node * free_node)
{
  info *arg_info;

  DBUG_ENTER ("FREEfreeTree");
 // printf("it is here in Free now\n");
  arg_info = MakeInfo ();
  INFO_FREE_FLAG (arg_info) = NULL;

  TRAVpush (TR_free);

  free_node = TRAVdo (free_node, arg_info);

  TRAVpop (TR_free);

  arg_info = FreeInfo (arg_info);

  DBUG_RETURN (free_node);
}
