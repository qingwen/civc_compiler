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



#ifndef _SAC_TYPES_H_
#define _SAC_TYPES_H_

#include <stdio.h>

#include "types_nodetype.h"
#include "types_trav.h"


/*
 * bool values
 */

typedef int bool;

#define FALSE 0
#define TRUE  1

/*
 * The NEW node structure of the SAC syntax tree
 * The type is abstract, as there is _no_ way to access a node other
 * than using tree_basic.h. Thus the structure is defined in
 * tree_basic.h. This as well solves dependency problems.
 */
typedef struct NODE node;

/*****************************************************************************
 * The info structure is used during traversal to store some stateful
 * information. It replaces the old N_info node. The structure is defined
 * as an abstract type here, so it can be definied by the different
 * traversals to suit the local needs. To do so, define a structure INFO
 * within the .c file of your traversal or create a specific .h/.c file
 * included by all .c files of your traversal. You as well have to create
 * a static MakeInfo/FreeInfo function.
 *****************************************************************************/

typedef struct INFO info;


/*
 * type of traversal functions
 */
typedef node *(*travfun_p) (node *, info *);

/*
 * types for compiler phases
 */

#define PHASE(name, text, cond)    \
  PH_##name,

#define SUBPHASE(name, text, fun, cond, phase)   \
  PH_##phase##_##name,

#define CYCLE(name, text, cond, phase, setup)         \
  PH_##phase##_##name,

#define CYCLEPHASE(name, text, fun, cond, phase, cycle)   \
  PH_##phase##_##cycle##_##name,

typedef enum {
  PH_initial=0,
#include "phase.mac"
  PH_final,
  PH_undefined
} compiler_phase_t;

//add by Michael for supporting Clemen's XML file
typedef enum {
  T_unknown=0, T_void=1, T_bool=2, T_int=3, T_float=4
} type;

#undef PHASE
#undef SUBPHASE
#undef CYCLE
#undef CYCLEPHASE


/*
 * project specific type definitions
 */

#include "mytypes.h"

#endif  /* _SAC_TYPES_H_ */

