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
 * This file contains functions to interpret command line options related 
 * to the phase mechanism, in particular the break option -b and the dbug
 * option -#.
 *
 *****************************************************************************/


#include "phase_options.h"

#include "types.h"
#include "dbug.h"
#include "str.h"
#include "ctinfo.h"
#include "globals.h"
#include "memory.h"
#include "phase_info.h"


static
compiler_phase_t SearchPhaseByName( char *name)
{
  compiler_phase_t phase;
  
  DBUG_ENTER("SearchPhaseByName");
  
  phase = PH_initial;
  
  do {
    phase++;
  } while ((phase < PH_final) 
           && ((PHIphaseType( phase) != PHT_phase)
               || !STReq( PHIphaseName( phase), name)));
  
  DBUG_RETURN( phase);
}

static
compiler_phase_t SearchPhaseByNumber( int num)
{
  compiler_phase_t phase;
  int cnt;
  
  DBUG_ENTER("SearchPhaseByNumber");
  
  phase = PH_initial;
  cnt = 0;
  
  do {
    phase++;
    if (PHIphaseType( phase) == PHT_phase) cnt++;
  } while ((phase < PH_final) && (cnt < num));
  
  DBUG_RETURN( phase);
}

static
compiler_phase_t SearchSubPhase( compiler_phase_t phase, char *name)
{
  compiler_phase_t subphase;

  DBUG_ENTER("SearchSubPhase");
  
  subphase = phase;
  
  do {
    do {
      subphase++;
    } while (PHIphaseType( subphase) > PHT_cycle);
  } while ((PHIphaseParent( subphase) == phase)
           && !STReq( PHIphaseName( subphase), name));
  
  if (PHIphaseParent( subphase) != phase) {
    subphase = PH_final;
  }
  
  DBUG_RETURN( subphase);
}

static
compiler_phase_t SearchCyclePhase( compiler_phase_t cycle, char *name)
{
  compiler_phase_t cyclephase;

  DBUG_ENTER("SearchCyclePhase");
  
  cyclephase = cycle;
  
  do {
      cyclephase++;
  } while ((PHIphaseParent( cyclephase) == cycle)
           && !STReq( PHIphaseName( cyclephase), name));
  
  if (PHIphaseParent( cyclephase) != cycle) {
    cyclephase = PH_final;
  }
  
  DBUG_RETURN( cyclephase);
}




void PHOinterpretBreakOption( char *option)
{
  compiler_phase_t phase;
  compiler_phase_t subphase;
  compiler_phase_t cyclephase;
  char *break_phase;
  char *break_subphase;
  char *break_cyclephase;
  char *break_cyclepass;
  char *rest;
  int num;
  
  DBUG_ENTER("PHOinterpretBreakOption");
  
  DBUG_PRINT( "PHO", ("Interpreting break option: %s", option));
  
  break_phase = STRtok( option, ":");
  
  num = strtol( break_phase, &rest, 10);

  if (rest == break_phase) {
    /*
     * The phase spec is not a number.
     */
    phase = SearchPhaseByName( break_phase);
  }
  else if (rest[0] == '\0') {
    /*
     * The phase spec is a number.
     */
    phase = SearchPhaseByNumber( num);
  }
  else {
    phase = PH_final;
  }

  if (phase == PH_final) {
    CTIerror( "Illegal compiler phase specification in break option: \n"
              "  -b %s\n"
              "See %s -h for a list of legal break options.",
              option, global.argv[0]);
  }
  else {
    global.break_after_phase = phase;
  }

  break_phase = MEMfree( break_phase);
  
  break_subphase = STRtok( NULL, ":");
  
  if (break_subphase != NULL) {
    subphase = SearchSubPhase( phase, break_subphase);

    if (subphase == PH_final) {
      CTIerror( "Illegal compiler subphase specification in break option:\n"
              "  -b %s\n"
              "See sac2c -h for a list of legal break options.",
                option);
    }
    else {
      global.break_after_subphase = subphase;
    }

    break_subphase = MEMfree( break_subphase);
  
    break_cyclephase = STRtok( NULL, ":");
  
    if (break_cyclephase != NULL) {
      cyclephase = SearchCyclePhase( subphase, break_cyclephase);

      if (cyclephase == PH_final) {
        CTIerror( "Illegal compiler cycle phase specification in break option: \n"
              "  -b %s\n"
              "See sac2c -h for a list of legal break options.",
                  option);
      }
      else {
        global.break_after_cyclephase = cyclephase;
      }

      break_cyclephase = MEMfree( break_cyclephase);
  
      break_cyclepass = STRtok( NULL, ":");
  
      if (break_cyclepass != NULL) {
        num = strtol( break_cyclepass, &rest, 10);

        if ((rest[0] == '\0') && (num>=1)) {
          global.break_cycle_specifier = num;
        }
        else {
          CTIerror( "Illegal compiler cycle pass specification in break option: \n"
              "  -b %s\n"
              "See sac2c -h for a list of legal break options.",
                    option);
        }

        break_cyclepass = MEMfree( break_cyclepass);
        
      }
    }
  }
  
  DBUG_VOID_RETURN;
}


#ifndef DBUG_OFF

static
compiler_phase_t SearchPhaseIdent( char *ident)
{
  compiler_phase_t phase;
  
  DBUG_ENTER("SearchPhaseIdent");
  
  phase = PH_final;

  do {
    phase++;
  } while ((phase < PH_final) && !STReq( PHIphaseIdent( phase), ident));
  
  DBUG_RETURN( phase);
}


void PHOinterpretDbugOption( char *option)
{
  char *tok;
  compiler_phase_t phase;
  
  DBUG_ENTER("PHOinterpretDbugOption");
  
  tok = STRtok( option, "/");
  
  DBUG_ASSERT( tok!=NULL, "Corruption in dbug option");
  
  if (tok[0] != '\0') {
    phase = SearchPhaseIdent( tok);
  
    if (phase==PH_final) {
      CTIerror( "Illegal start compiler phase specification in dbug option: \n"
                "  -# %s\n"
                "See %s -h for a list of legal break options.",
                option, global.argv[0]);
    }
    else {
      global.my_dbug_from = phase;
    }
  }
  
  tok = MEMfree( tok);
  
  tok = STRtok( NULL, "/");
  
  if (tok == NULL) {
    CTIerror( "Missing stop compiler phase specification in dbug option: \n"
              "  -# %s\n"
              "See %s -h for a list of legal break options.",
              option, global.argv[0]);
  }
  else {
    if (tok[0] != '\0') {
      phase = SearchPhaseIdent( tok);
  
      if (phase==PH_final) {
        CTIerror( "Illegal start compiler phase specification in dbug option: \n"
                  "  -# %s\n"
                  "See %s -h for a list of legal break options.",
                  option, global.argv[0]);
      }
      else if (phase < global.my_dbug_from) {
        CTIerror( "Stop phase is before start phase in dbug option: \n"
                  "  -# %s\n"
                  "See %s -h for sequence of phases.",
                  option, global.argv[0]);
      }
      else {
        global.my_dbug_to = phase;
      }
    }
    
    tok = MEMfree( tok);
  
    tok = STRtok( NULL, "/");
  
    if (tok == NULL) {
      CTIerror( "Missing dbug string in dbug option: \n"
                "  -# %s\n"
                "See %s -h for syntac of dbug option.",
                option, global.argv[0]);
    }
    else {
      global.my_dbug_str = tok;
      global.my_dbug = 1;
    }
  }
  
  DBUG_VOID_RETURN;
}

#endif /* DBUG_OFF */




#define PHASEname(name) \
  cnt += 1; \
  printf( "\n    %-3s | %-2d", #name, cnt);     

#define PHASEtext(text) \
  printf( " : " text "\n");  


#define SUBPHASEname(name) \
  printf( "      %-8s", #name); 

#define SUBPHASEtext(text) \
  printf( " : " text "\n");  


#define CYCLEname(name) \
  printf( "      %-8s", #name); 

#define CYCLEtext(text) \
  printf( " : " text "\n");  


#define CYCLEPHASEname(name) \
  printf( "        %-8s", #name); 

#define CYCLEPHASEtext(text) \
  printf( " : " text "\n");  


#define CYCLEPHASEFUNname(name) \
  printf( "        %-8s", #name); 

#define CYCLEPHASEFUNtext(text) \
  printf( " : " text " (fun based)\n");  


void PHOprintPhases( void)
{
  int cnt = 0;

  DBUG_ENTER("PHOprintPhases");
  
#include "phase.mac"

  DBUG_VOID_RETURN;
}

#undef PHASname
#undef PHASEtext
#undef SUBPHASEname 
#undef SUBPHASEtext
#undef CYCLEname
#undef CYCLEtext
#undef CYCLEPHASEname
#undef CYCLEPHASEtext 
#undef CYCLEPHASEFUNname
#undef CYCLEPHASEFUNtext 












