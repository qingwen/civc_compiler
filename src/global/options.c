#include "main_args.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "usage.h"
#include "ctinfo.h"
#include "phase_options.h"


void OPTcheckOptions( int argc, char **argv)
{
  DBUG_ENTER("OPTcheckOptions");

  ARGS_BEGIN( argc, argv);

  ARGS_OPTION( "b", PHOinterpretBreakOption( ARG)) 

  ARGS_FLAG( "h", USGprintUsage(); exit(0););

  ARGS_OPTION( "o", global.outfile = STRcpy( ARG));

  ARGS_OPTION( "v", ARG_RANGE(global.verbosity, 0, 3));

  ARGS_FLAG( "tc", global.treecheck = TRUE);

  ARGS_OPTION( "#", DBUG_PUSH( STRcpy( ARG)));

  ARGS_ARGUMENT( global.infile = STRcpy( ARG); );
  
  ARGS_UNKNOWN( ARGS_ERROR( "Invalid command line entry"));

  ARGS_END();

  if (global.infile == NULL) {
    CTIabort( "No input file given.");
  }

  DBUG_VOID_RETURN;
}
