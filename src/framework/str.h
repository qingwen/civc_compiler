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



#ifndef _STR_H_
#define _STR_H_

#include "types.h"

extern char *STRcpy(const char *source);
extern char *STRncpy(const char *source, int maxlen);
extern char *STRcat(const char *first, const char* second);
extern char *STRcatn(int n, ...);
extern char *STRtok(const char *str, const char *tok);
extern bool STReq(const char *first, const char* second);
extern bool STReqci(const char *first, const char* second);
extern bool STRprefix( const char *prefix, const char *str);
extern bool STRsuffix( const char *suffix, const char *str);
extern bool STReqn(const char *first, const char* second, int n);
extern bool STRsub(const char *sub, const char *str);
extern int  STRlen(const char *str);
extern char *STRonNull( char *alt,  char *str);
extern char *STRsubStr( const char *string, int start, int len);
extern char *STRnull( );
extern char *STRitoa(int number);
extern char *STRsubstToken( const char *str, const char *token, const char *subst);

#define F_PTR "%p"

#endif /* _STR_H_ */
