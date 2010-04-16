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



#include "phase.h"

#include "dbug.h"
#include "ctinfo.h"
#include "globals.h"
#include "str.h"
#include "memory.h"
#include "phase_drivers.h"
#include "phase_info.h"
#include "check.h"


static bool go_one_cycle_more;


static
void CheckEnableDbug( compiler_phase_t phase)
{
  DBUG_ENTER("CheckEnableDbug");
    
  if (global.my_dbug 
      && (phase >= global.my_dbug_from)
      && !global.my_dbug_active) {
    DBUG_PUSH( global.my_dbug_str);
    global.my_dbug_active = TRUE; 
  } 

  DBUG_VOID_RETURN;
}

static
void CheckDisableDbug( compiler_phase_t phase)
{
  DBUG_ENTER("CheckDisableDbug");
    
  if (global.my_dbug
      && global.my_dbug_active
      && (phase >= global.my_dbug_to)) {
    DBUG_POP();
    global.my_dbug_active = FALSE;
  }

  DBUG_VOID_RETURN;
}



node *PHrunPhase( compiler_phase_t phase, node *syntax_tree, bool cond)
{
  static int phase_num=0;
  
  DBUG_ENTER("PHrunPhase");
  
  DBUG_ASSERTF( PHIphaseType( phase) == PHT_phase,
                ("PHrunPhase called with incompatible phase: %s",
                 PHIphaseIdent( phase)));
  
  global.compiler_phase = phase;
  global.compiler_anyphase = phase;
  phase_num += 1;
  CheckEnableDbug( phase);

  CTInote(" ");

  if (cond) {
    CTIstate("** %2d: %s ...", phase_num, PHIphaseText( phase));
    syntax_tree = PHIphaseFun( phase)( syntax_tree);

    CTIabortOnError();

    if (global.treecheck && (syntax_tree != NULL)) {
      syntax_tree = CHKdoTreeCheck( syntax_tree);
    }
  }
  else {
    CTIstate("** %2d: %s skipped.", phase_num, PHIphaseText( phase));
  }
    
  CheckDisableDbug( phase);

  CTIabortOnError();

  if (global.break_after_phase == phase) {
    CTIterminateCompilation( syntax_tree);
  }
  
  DBUG_RETURN( syntax_tree);
}



node *PHrunSubPhase( compiler_phase_t subphase, node *syntax_tree, bool cond)
{
  DBUG_ENTER("PHrunSubPhase");

  DBUG_ASSERTF( PHIphaseType( subphase) == PHT_subphase,
                ("PHrunSubPhase called with incompatible phase: %s",
                 PHIphaseIdent( subphase)));
  
  global.compiler_subphase = subphase;
  global.compiler_anyphase = subphase;

  CheckEnableDbug( subphase);

  if (cond) {
    if (PHIphaseType( subphase) != PHT_cycle) {
      CTInote("**** %s ...", PHIphaseText( subphase));
    }
    syntax_tree = PHIphaseFun( subphase)( syntax_tree);
    CTIabortOnError();

    if (global.treecheck && (syntax_tree != NULL)) {
      syntax_tree = CHKdoTreeCheck( syntax_tree);
    }
  }
  
  CTIabortOnError();

  CheckDisableDbug( subphase);

  if ( global.break_after_subphase == subphase ) {
    CTIterminateCompilation( syntax_tree);
  }
  
  DBUG_RETURN( syntax_tree);
}


node *PHrunCycle( compiler_phase_t cycle, node *syntax_tree, bool cond, bool reset)
{
  DBUG_ENTER("PHrunCycle");
  
  DBUG_ASSERTF( PHIphaseType( cycle) == PHT_cycle,
                ("PHrunCycle called with incompatible phase: %s",
                 PHIphaseIdent( cycle)));
  
  global.compiler_subphase = cycle;
  global.compiler_anyphase = cycle;

  if (cond) {
    CheckEnableDbug( cycle);
    global.cycle_counter = 1;

    do  { 
      CTInote(" ");
      CTInote("**** %s pass: %i", PHIphaseText( cycle), global.cycle_counter);

      go_one_cycle_more = FALSE;

      syntax_tree = PHIphaseFun( cycle)( syntax_tree);

      CTIabortOnError();
      
      if (global.treecheck && (syntax_tree != NULL)) {
        syntax_tree = CHKdoTreeCheck( syntax_tree);
      }

      if (!go_one_cycle_more) {
        CTInote(" "); 
      } 

      global.cycle_counter += 1;

    } while ( go_one_cycle_more
              && (global.cycle_counter <= global.max_optcycles) 
              && ((global.cycle_counter <= global.break_cycle_specifier) 
                  || (global.break_after_cyclephase > global.compiler_cyclephase ))); 

    if (go_one_cycle_more && (global.cycle_counter == global.max_optcycles)) { 
      CTIwarn( "Maximum number of optimization cycles reached");  
    } 

    CheckDisableDbug( cycle);
  }

  CTIabortOnError();

  if ( global.break_after_subphase == cycle ) {
    CTIterminateCompilation( syntax_tree);
  }
  
  DBUG_RETURN( syntax_tree);
}


node *PHrunCyclePhase( compiler_phase_t cyclephase,
                       node *syntax_tree, bool cond)
{
  DBUG_ENTER("PHrunCyclePhase");

  DBUG_ASSERTF( PHIphaseType( cyclephase) == PHT_cyclephase,
                ("PHrunPhase called with incompatible phase: %s",
                 PHIphaseIdent( cyclephase)));
  
  global.compiler_cyclephase = cyclephase;
  global.compiler_anyphase = cyclephase;

  CheckEnableDbug( cyclephase);

  if (cond && ((cyclephase <= global.break_after_cyclephase)
               || (global.cycle_counter < global.break_cycle_specifier))) {
    CTInote( "****** %s ...", PHIphaseText( cyclephase));
    
    syntax_tree = PHIphaseFun( cyclephase)( syntax_tree);

    CTIabortOnError();

    if (global.treecheck && (syntax_tree != NULL)) {
      syntax_tree = CHKdoTreeCheck( syntax_tree);
    }
  }
  
  CheckDisableDbug( cyclephase);

  DBUG_RETURN( syntax_tree);
}


void PHrunOneCycleMore( void)
{
  DBUG_ENTER("PHrunOneCycleMore");
  
  go_one_cycle_more = TRUE;
  
  DBUG_VOID_RETURN;
}

  
