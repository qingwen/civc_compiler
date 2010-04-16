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



#include <stdio.h>

#include "scanparse.h"
#include "dbug.h"
#include "globals.h"
#include "ctinfo.h"
#include "str.h"
#include "memory.h"
#include "myglobals.h"


extern myglobals_t myglobal;
/*
 * file handle for parsing
 */
FILE *yyin;

/*
 * external parser function from fun.y
 */
extern node *YYparseTree();

node *SPdoScanParse( node *syntax_tree)
{
  node *result = NULL;
  char *filename;
  
  DBUG_ENTER("SPdoScanParse");

  DBUG_ASSERT( syntax_tree == NULL, 
               "SPdoScanParse() called with existing syntax tree.");
  
  if (global.cpp) {
    filename = STRcatn( 3,
                        ".",
                        global.infile,
                        ".cpp");
  }
  else {
    filename = STRcpy( global.infile);
  }
  
  yyin = fopen( filename, "r");
  
  if (yyin == NULL) {
    CTIabort( "Cannot open file '%s'.", filename);
  }

  MEMfree( filename);
  
  result = YYparseTree();

  DBUG_RETURN( result);
}


node *SPdoRunPreProcessor( node *syntax_tree)
{
  int  err;
  char *cppcallstr;
  
  DBUG_ENTER("SPdoRunPreProcessor");

  cppcallstr = STRcatn( 5, 
                        "gcc -E ",
                        global.infile,
                        " > .",
                        global.infile,
                        ".cpp");
  
  err = system( cppcallstr);

  cppcallstr = MEMfree( cppcallstr);

  if ( err) {
    CTIabort( "Unable to run C preprocessor");
  }
  
  global.cpp = TRUE;
  
  DBUG_RETURN( syntax_tree);
}

