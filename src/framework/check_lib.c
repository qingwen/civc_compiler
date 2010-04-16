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


#include "check_lib.h"

#include "print.h"
#include "free.h"
#include "str.h"
#include "traverse.h"
#include "tree_basic.h"
#include "types.h"
#include "globals.h"
#include "ctinfo.h"
#include "phase.h"
#include "dbug.h"
#include "memory.h"



/** <!--**********************************************************************-->
 *
 * @fn node *CHKinsertError( node *arg_node, char *string)
 *******************************************************************************/
node *CHKinsertError( node *arg_node, char *string)
{
  DBUG_ENTER( "CHKinsertError");

  if ( arg_node == NULL) {
    
    CTIwarn( "%s", string);

    arg_node = TBmakeError( STRcpy( string), global.compiler_anyphase, arg_node);
  } 
  else {
    if ( !(STReq( string, ERROR_MESSAGE( arg_node)))) {
      ERROR_NEXT( arg_node) = CHKinsertError( ERROR_NEXT( arg_node), string);
    }
    else {
      ERROR_ANYPHASE( arg_node) = global.compiler_anyphase;
    }
  } 
  
  DBUG_RETURN( arg_node);
}


/** <!--**********************************************************************-->
 *
 * @fn node *CHKexistSon( node *child, node *arg_node, char *string)
 *
 *******************************************************************************/
node *CHKexistSon( node *son, node *arg_node, char *string)
{
  DBUG_ENTER( "CHKexistSon");
  
  if ( son == NULL) {

    NODE_ERROR( arg_node) = CHKinsertError( NODE_ERROR( arg_node),
                                            string);
  }

  DBUG_RETURN( son);
}


/** <!--**********************************************************************-->
 *
 * @fn node *CHKexistAttribute( node *attribute, node *arg_node, char *string)
 *
 *******************************************************************************/
node *CHKexistAttribute( void *attribute, node *arg_node, char *string)
{
  DBUG_ENTER( "CHKexistAttribute");

  if ( attribute == NULL) {
      
    NODE_ERROR( arg_node) = CHKinsertError( NODE_ERROR( arg_node),
                                            string);
  }
  
  DBUG_RETURN( attribute);
}


/** <!--**********************************************************************-->
 *
 * @fn node *CHKnotExist( node *son_attribute, node *arg_node, char *string)
 *
 *******************************************************************************/

node *CHKnotExist( void *son_attribute, node *arg_node, char *string)
{
  DBUG_ENTER( "CHKnotExist");

  if ( son_attribute != NULL) {

    NODE_ERROR( arg_node) = CHKinsertError( NODE_ERROR( arg_node),
                                            string);
  }

  DBUG_RETURN( son_attribute);
}


/** <!--********************************************************************-->
 *
 * @fn node *CHKnotExistAttribute( node *attribute, node *arg_node, char *string)
 *
 *****************************************************************************/
node *CHKnotExistAttribute( void *attribute, node *arg_node, char *string)
{
  DBUG_ENTER( "CHKexistAttribute");

  if ( attribute != NULL) {

    NODE_ERROR( arg_node) = CHKinsertError( NODE_ERROR( arg_node),
                                            string);
  }

  DBUG_RETURN( attribute);
}


/** <!--**********************************************************************-->
 *
 * @fn node *CHKcorrectTypeInsertError( node *arg_node, char *string)
 *
 *******************************************************************************/
node *CHKcorrectTypeInsertError( node *arg_node, char *string)
{
  DBUG_ENTER( "CHKcorrectType");

  NODE_ERROR( arg_node) = CHKinsertError( NODE_ERROR( arg_node),
                                          string);
  
  DBUG_RETURN( arg_node); 
}

