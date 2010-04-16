
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "dbug.h"
#include "usage.h"
#include "phase_options.h"


static
void PrintGeneralInfo(void)
{
  DBUG_ENTER("PrintGeneralInfo");
  
  printf( "\n\n"
          "      civcc  --  Programming C,  the civilised way\n"
          "    ------------------------------------------------\n");
          
  printf( "\n\nSPECIAL OPTIONS:\n\n"
          
          "    -h              Display this helptext.\n");
  
  printf( "\n\nGENERAL OPTIONS:\n\n"
          
          "    <filename>      Name of program file to compile.\n\n"
          "    -o <filename>   Name of output file.\n\n"
          "    -#d,<id>        Print debugging information for tag <id>.\n"
          "                    Supported tags are:\n\n"
          
          "                    MAKE - prints debug information of tree constructors.\n"
          "                    FREE - prints debug information of tree destructors.\n");

  DBUG_VOID_RETURN;
}

static
void PrintAuthorInfo(void)
{
  DBUG_ENTER("PrintAuthorInfo");
  
  printf( "\n\nAUTHORS:\n\n"

          "SAC Development Team for the Compiler Framework.\n\n"
          "???? for the CivC compiler.\n\n");
  

  DBUG_VOID_RETURN;
}

static
void PrintBreakOptions( void)
{
  DBUG_ENTER("PrintBreakOptions");
  
  printf( "\n\nBREAK OPTIONS:\n\n"

          "    Break options allow you to stop the compilation process\n"
          "    after a particular phase, subphase or cycle optimisation.\n"
          "    By default the intermediate programm will be printed, \n"
          "    but this behaviour may be influenced by the following\n"
          "    compiler options:\n"
          "\n"
          "    -noPAB          Deactivates printing after break.\n"
          "    -doPAB          Activates printing after break.\n"
          "\n"
          "    -b<spec>        Break after the compilation stage given\n"
          "                    by <spec>, where <spec> follows the pattern\n"
          "                    <phase>:<subphase>:<cyclephase>:<pass>.\n"
          "                    The first three are from the list of\n"
          "                    encodings below. The last one is a natural\n"
          "                    number. Alternatively, a number can be used\n"
          "                    for the phase, as well.\n"
          );

  DBUG_VOID_RETURN;
}


static
void PrintBreakoptionSpecifier( void)
{
  DBUG_ENTER("PrintBreakoptionSpecifier");
  
  printf( "\n\nBREAK OPTION SPECIFIERS:\n");

  PHOprintPhases();

  DBUG_VOID_RETURN;
}


void USGprintUsage()
{
  DBUG_ENTER("USGprintUsage");

  PrintGeneralInfo();
  PrintBreakOptions();
  PrintBreakoptionSpecifier();
  PrintAuthorInfo();
  
  DBUG_VOID_RETURN;
}
