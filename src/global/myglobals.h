#ifndef _CIVCC_MYGLOBALS_H_
#define _CIVCC_MYGLOBALS_H_

#include "types.h"

typedef struct MYGLOBALS_T {
  #define GLOBALtype( it_type) it_type
  #define GLOBALname( it_name) it_name ;
  #include "myglobals.mac"
  #undef GLOBALtype
  #undef GLOBALname
} myglobals_t;

extern myglobals_t myglobal;

extern void MYGLBinitializeGlobals( void);

#endif /* _CIVCC_MYGLOBALS_H_ */
