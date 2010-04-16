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



#include "phase_drivers.h"

#include "dbug.h"
#include "phase.h"
#include "globals.h"



/*
 * Generated function-based cycle driver functions 
 */

#define FUNBEGINname( name) \
node *PHDdriveCycleFun_##name( node *fundef)    \
{                                                    \
  DBUG_ENTER("PHDdriveCycleFun_" #name);                         

#define CYCLEPHASEFUN( name, text, fun, cond, phase, cycle)  \
  fundef = PHrunCyclePhaseFun( PH_##phase##_##cycle##_##name, fundef, cond);

#define FUNEND( name) \
  DBUG_RETURN( fundef);   \
}

#include "phase.mac"

#undef FUNBEGINname
#undef CYCLEPHASEFUN
#undef FUNEND


/*
 * Generated cycle driver functions 
 */

#define CYCLEname( name)   \
node *PHDdriveCycle_##name( node *syntax_tree)    \
{                                                     \
  DBUG_ENTER("PHDdriveCycle_" #name);                         

#define CYCLEPHASE( name, text, fun, cond, phase, cycle)  \
  syntax_tree = PHrunCyclePhase( PH_##phase##_##cycle##_##name, syntax_tree, cond);

#define FUNBEGIN( name, phase, cycle) \
  syntax_tree = PHrunCycleFun( PH_##phase##_##cycle##_##name, syntax_tree); 
  
#define ENDCYCLE( name) \
  DBUG_RETURN( syntax_tree);   \
}

#include "phase.mac"

#undef CYCLEname
#undef CYCLEPHASE
#undef FUNBEGIN
#undef ENDCYCLE 



/*
 * Generated phase driver functions 
 */

#define PHASEname( name)                       \
  node *PHDdrivePhase_##name ( node *syntax_tree)       \
  {                                                     \
    DBUG_ENTER("PHDdrivePhase_" #name);                         

#define SUBPHASE( name, text, fun, cond, phase)           \
  syntax_tree = PHrunSubPhase( PH_##phase##_##name, syntax_tree, cond);
  

#define CYCLE( name, text, cond, phase, reset)     \
  syntax_tree = PHrunCycle( PH_##phase##_##name, syntax_tree, cond, reset);

#define ENDPHASE( name)  \
    DBUG_RETURN( syntax_tree);   \
  }

#include "phase.mac"

#undef PHASEname
#undef SUBPHASE
#undef CYCLE
#undef ENDPHASE



/*
 * Generated tool driver functions 
 */


node *PHDdrive( node *syntax_tree)
{
  DBUG_ENTER("PHDdriveSac2c");

#define PHASEname(name)                                        \
  syntax_tree = PHrunPhase( PH_##name, syntax_tree, 

#define PHASEcond(cond) \
  cond);

#include "phase.mac"

#undef PHASEname
#undef PHASEcond

  DBUG_RETURN( syntax_tree);
}

