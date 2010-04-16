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



/*****************************************************************************
 *
 * This file contains the all static information derived from the phase
 * macro file(s).
 *****************************************************************************/


#include "phase_info.h"

#include "dbug.h"
#include "phase.h"
#include "globals.h"
#include "phase_drivers.h"


/*
 * Extern declarations for non-generated functions used to implement subphases
 * and cycle phases.
 */

#define SUBPHASEfun(fun)      extern node *fun( node *syntax_tree); 
#define CYCLEPHASEfun(fun)    extern node *fun( node *syntax_tree); 
#define CYCLEPHASEFUNfun(fun) extern node *fun( node *syntax_tree); 

#include "phase.mac"

#undef SUBPHASEfun
#undef CYCLEPHASEfun
#undef CYCLEPHASEFUNfun


/****************************************************************************/

/*
 * Dummy phase functions
 */

static
node *DummyPhaseFun( node *syntax_tree)
{
  DBUG_ENTER("DummyPhaseFun");
  
  DBUG_ASSERT( FALSE, "This function should never be called.");
  
  DBUG_RETURN( syntax_tree);
}


/****************************************************************************/

#define PHASEname(name)       PHDdrivePhase_##name,
#define SUBPHASEfun(fun)      fun,
#define CYCLEname(name)       PHDdriveCycle_##name,
#define CYCLEPHASEfun(fun)    fun,
#define FUNBEGINname(name)    PHDdriveCycleFun_##name,
#define CYCLEPHASEFUNfun(fun) fun,

phase_fun_t PHIphaseFun( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseFun");
  
  static const phase_fun_t phase_fun[] = { DummyPhaseFun, 
#include "phase.mac"
                                           DummyPhaseFun
  };

  DBUG_RETURN( phase_fun[phase]);
}

#undef PHASEname
#undef SUBPHASEfun
#undef CYCLEname
#undef CYCLEPHASEfun
#undef FUNBEGINname
#undef CYCLEPHASEFUNfun

/****************************************************************************/

#define PHASEtext(text)         text,
#define SUBPHASEtext(text)      text,
#define CYCLEtext(text)         text,
#define CYCLEPHASEtext(text)    text,
#define FUNBEGINname(name)      "",
#define CYCLEPHASEFUNtext(text) text,

const char *PHIphaseText( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseText");
  
  static const char *phase_text[] = {
    "initial",
#include "phase.mac"
    "final"
  };

  DBUG_RETURN( phase_text[phase]);
}

#undef PHASEtext
#undef SUBPHASEtext
#undef CYCLEtext
#undef CYCLEPHASEtext
#undef FUNBEGINname
#undef CYCLEPHASEFUNtext

/****************************************************************************/

#define PHASEname(name)         PHT_phase,
#define SUBPHASEname(name)      PHT_subphase,
#define CYCLEname(name)         PHT_cycle,
#define CYCLEPHASEname(name)    PHT_cyclephase,
#define FUNBEGINname(name)      PHT_cycle_fun,
#define CYCLEPHASEFUNname(name) PHT_cyclephase_fun,

phase_type_t PHIphaseType( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseType");
  
  static phase_type_t phase_type[] = {
    PHT_dummy,
#include "phase.mac"
    PHT_dummy
  };

  DBUG_RETURN( phase_type[phase]);
}

#undef PHASEname
#undef SUBPHASEname
#undef CYCLEname
#undef CYCLEPHASEname
#undef FUNBEGINname
#undef CYCLEPHASEFUNname


/****************************************************************************/

#define PHASEname(name)         #name,
#define SUBPHASEname(name)      #name,
#define CYCLEname(name)         #name,
#define CYCLEPHASEname(name)    #name,
#define FUNBEGINname(name)      #name,
#define CYCLEPHASEFUNname(name) #name,

const char *PHIphaseName( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseName");
  
  static const char *phase_name[] = {
    "initial",
#include "phase.mac"
    "final"
  };
  
  DBUG_RETURN( phase_name[phase]);
}

#undef PHASEname
#undef SUBPHASEname
#undef CYCLEname
#undef CYCLEPHASEname
#undef FUNBEGINname
#undef CYCLEPHASEFUNname

/****************************************************************************/

#define PHASE(name, text, cond)    \
  PH_initial,

#define SUBPHASE(name, text, fun, cond, phase)   \
  PH_##phase,

#define CYCLE(name, text, cond, phase, setup)         \
  PH_##phase,

#define CYCLEPHASE(name, text, fun, cond, phase, cycle)   \
  PH_##phase##_##cycle,

#define FUNBEGIN( name, phase, cycle)   \
  PH_##phase##_##cycle,

#define CYCLEPHASEFUN(name, text, fun, cond, phase, cycle)   \
  PH_##phase##_##cycle,

compiler_phase_t PHIphaseParent( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseParent");
  
  static compiler_phase_t phase_parent[] = {
    PH_initial,
#include "phase.mac"
    PH_final
  };
  
  DBUG_RETURN( phase_parent[phase]);
}

#undef PHASE
#undef SUBPHASE
#undef CYCLE
#undef CYCLEPHASE
#undef FUNBEGIN
#undef CYCLEPHASEFUN

/****************************************************************************/

#define PHASE(name, text, cond)    \
  #name,

#define SUBPHASE(name, text, fun, cond, phase)   \
  #phase ":" #name,

#define CYCLE(name, text, cond, phase, setup)         \
  #phase ":" #name,

#define CYCLEPHASE(name, text, fun, cond, phase, cycle)   \
  #phase ":" #cycle ":" #name,

#define FUNBEGIN( name, phase, cycle)   \
  #phase ":" #cycle ":" #name,

#define CYCLEPHASEFUN(name, text, fun, cond, phase, cycle)   \
  #phase ":" #cycle ":" #name,

const char *PHIphaseIdent( compiler_phase_t phase)
{
  DBUG_ENTER("PHIphaseIdent");
  
  static const char *phase_ident[] = {
    "",
#include "phase.mac"
    ""
  };
  
  DBUG_RETURN( phase_ident[phase]);
}

#undef PHASE
#undef SUBPHASE
#undef CYCLE
#undef CYCLEPHASE
#undef FUNBEGIN
#undef CYCLEPHASEFUN

/****************************************************************************/

#define PHASEname(name)         FALSE,
#define SUBPHASEname(name)      FALSE,
#define CYCLEname(name)         FALSE,
#define CYCLEPHASEname(name)    FALSE,
#define FUNBEGINname(name)      FALSE,
#define CYCLEPHASEFUNname(name) TRUE,

bool PHIisFunBased( compiler_phase_t phase)
{
  DBUG_ENTER("PHIisFunBased");
  
  static bool phase_isfunbased[] = {
    FALSE,
#include "phase.mac"
    FALSE
  };
  
  DBUG_RETURN( phase_isfunbased[phase]);
}

#undef PHASEname
#undef SUBPHASEname
#undef CYCLEname
#undef CYCLEPHASEname
#undef FUNBEGINname
#undef CYCLEPHASEFUNname

