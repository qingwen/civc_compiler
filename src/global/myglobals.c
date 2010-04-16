#include "myglobals.h"
#include "dbug.h"


myglobals_t myglobal;


/*
 * Initialize my global variables from myglobals.mac
 */

void MYGLBinitializeGlobals( void)
{
  DBUG_ENTER("MYGLBinitializeGlobals");

#define GLOBALname( name) myglobal.name = 
#define GLOBALinit( init) init ;

#include "myglobals.mac"

#undef GLOBALinit
#undef GLOBALname

  DBUG_VOID_RETURN;
}
