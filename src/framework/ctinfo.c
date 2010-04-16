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



/**
 *
 * @file 
 *
 * This file provides the interface for producing any kind of output during
 * compilation. 
 *
 * We have 4 levels of verbosity controlled by the command line option -v
 * and the global variable verbose_level.
 *
 * Verbose level 0:
 *
 * Only error messages are printed.
 * 
 * Verbose level 1:
 *
 * Error messages and warnings are printed.
 * 
 * Verbose level 2:
 *
 * Error messages, warnings and basic compile time information, e.g. compiler
 * phases,  are printed.
 * 
 * Verbose level 3:
 *
 * Error messages, warnings and full compile time information are printed.
 * 
 * 
 * Default values are 1 for the product version and 3 for the developer version.
 *
 */



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "dbug.h"
#include "str.h"
#include "memory.h"
#include "globals.h"

#include "ctinfo.h"


static char *message_buffer=NULL;
static int message_buffer_size=0;
static int message_line_length=76;

static char *abort_message_header = "ABORT: ";
static char *error_message_header = "ERROR: ";
static char *warn_message_header = "WARNING: ";
static char *state_message_header = "";
static char *note_message_header = "  ";


static int errors=0;
static int warnings=0;

#define MAX_ITEM_NAME_LENGTH 255



/** <!--********************************************************************-->
 *
 * @fn void ProcessMessage( char *buffer, int line_length)
 *
 *   @brief  formats message according to line length
 *
 *           '@' characters are inserted into the buffer to represent line
 *           breaks.
 *           
 *   @param buffer  message buffer
 *   @param line_length maximum line length
 *
 ******************************************************************************/

static
void ProcessMessage( char *buffer, int line_length)
{
  int index, column, last_space;

  DBUG_ENTER("ProcessMessage");

  index=0;
  last_space=0;
  column=0;
  
  while (buffer[index]!='\0') {
    if (buffer[index]=='\t') {
      buffer[index]=' ';
    }

    if (buffer[index]==' ') {
      last_space=index;
    }

    if (buffer[index]=='\n') {
      buffer[index]='@';
      column=0;
    }
    else {
      if (column==line_length) {
        if (buffer[last_space]==' ') {
          buffer[last_space]='@';
          column=index-last_space;
        }
        else {
          break;
        }
      }
    }

    index++;
    column++;
  }
  
  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void Format2Buffer( const char *format, va_list arg_p)
 *
 *   @brief The message specified by format string and variable number
 *          of arguments is "printed" into the global message buffer.
 *          It is taken care of buffer overflows.
 *
 *   @param format  format string like in printf family of functions
 *
 ******************************************************************************/

static
void Format2Buffer( const char *format, va_list arg_p)
{
  int len;
  va_list arg_p_copy;

  DBUG_ENTER("Format2Buffer");

  va_copy( arg_p_copy, arg_p);
  len = vsnprintf( message_buffer, message_buffer_size, format, arg_p_copy);
  va_end( arg_p_copy);

  if (len < 0) {
    DBUG_ASSERT((message_buffer_size == 0), "message buffer corruption");
    /*
     * Output error due to non-existing message buffer
     */

    len = 120;

    message_buffer = (char*) MEMmalloc( len+2);
    message_buffer_size = len+2;

    va_copy( arg_p_copy, arg_p);
    len = vsnprintf( message_buffer, message_buffer_size, format, arg_p_copy);
    va_end( arg_p_copy);
    DBUG_ASSERT((len >= 0), "message buffer corruption");
  }

  if (len >= message_buffer_size) {
    /* buffer too small  */

    MEMfree( message_buffer);
    message_buffer = (char*) MEMmalloc( len+2);
    message_buffer_size = len+2;

    va_copy( arg_p_copy, arg_p);
    len = vsnprintf( message_buffer, message_buffer_size, format, arg_p_copy);
    va_end( arg_p_copy);

    DBUG_ASSERT((len < message_buffer_size), "message buffer corruption");
  }

  DBUG_VOID_RETURN;
}

/** <!--********************************************************************-->
 *
 * @fn char *CTIgetErrorMessageVA( int line, const char *format, va_list arg_p)
 *
 *   @brief generates error message string
 *
 *          The message specified by format string and variable number
 *          of arguments is "printed" into the global message buffer.
 *          It is taken care of buffer overflows. Afterwards, the message
 *          is formatted to fit a certain line length and is printed to
 *          stderr.
 *
 *   @param format  format string like in printf family of functions
 *
 ******************************************************************************/

char *CTIgetErrorMessageVA( int line, const char *format, va_list arg_p)
{
  char *first_line, *res;

  DBUG_ENTER( "CTIgetErrorMessageVA");
  Format2Buffer( format, arg_p);
  ProcessMessage( message_buffer, 
                  message_line_length - strlen( error_message_header));

  first_line = (char *)MEMmalloc( 32 * sizeof( char));
  sprintf( first_line, "line %d @", line);
  res = STRcat( first_line, message_buffer);
  first_line = MEMfree( first_line);

  DBUG_RETURN( res);
}



/** <!--********************************************************************-->
 *
 * @fn void PrintMessage( const char *header, const char *format, va_list arg_p)
 *
 *   @brief prints message 
 *
 *          The message specified by format string and variable number
 *          of arguments is "printed" into the global message buffer.
 *          It is taken care of buffer overflows. Afterwards, the message
 *          is formatted to fit a certain line length and is printed to
 *          stderr.
 *           
 *   @param header  string which precedes each line of the message, e.g.
                    ERROR or WARNING.
 *   @param format  format string like in printf family of functions
 *
 ******************************************************************************/

static
void PrintMessage( const char *header, const char *format, va_list arg_p)
{                                                                        
  char *line;
  
  DBUG_ENTER("PrintMessage");

  Format2Buffer( format, arg_p);

  ProcessMessage( message_buffer, message_line_length - strlen( header));

  line = strtok( message_buffer, "@");                 
                                                                         
  while (line != NULL) {
    fprintf( stderr, "%s%s\n", header, line);                                      
    line = strtok( NULL, "@");                                      
  }                                                                      

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn static void CleanUp()
 *
 *   @brief  does som clean up upon termination
 *           
 *
 ******************************************************************************/

static
void CleanUp()
{
  DBUG_ENTER("CleanUp");
  
  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void AbortCompilation()
 *
 *   @brief  terminates the compilation process with a suitable error message.
 *
 ******************************************************************************/

static
void AbortCompilation()
{
  DBUG_ENTER("AbortCompilation");

  fprintf( stderr, "\n*** Compilation failed ***\n");               
  fprintf( stderr, "*** %d Error(s), %d Warning(s)\n\n",            
           errors, warnings);                 

  CleanUp();

  exit( 1);

  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void InternalCompilerErrorBreak( int sig)
 *
 *   @brief  interrupt handler for segmentation faults and bus errors
 *
 *           An error message is produced and a bug report is created which
 *           may be sent via email to an appropriate address.
 *           Temporary files are deleted and the compilation process 
 *           terminated.
 *           
 *           DBUG_ENTER/RETURN are omitted on purpose to reduce risk of 
 *           creating more errors during error handling.
 *
 *   @param sig  signal causing interrupt
 *
 ******************************************************************************/

static
void InternalCompilerErrorBreak( int sig)
{
  fprintf( stderr, 
           "\n\n"
           "OOOPS your program crashed the compiler 8-((\n\n");

  CleanUp();

  exit(1);
}


/** <!--********************************************************************-->
 *
 * @fn void UserForcedBreak( int sig)
 *
 *   @brief  interrupt handler for user-forced breaks like CTRL-C
 *
 *           Temporary files are deleted and the compilation process 
 *           terminated.
 *           
 *           DBUG_ENTER/RETURN are omitted on purpose to reduce risk of 
 *           creating more errors during error handling.
 *           
 *   @param sig  signal causing interrupt
 *
 ******************************************************************************/

static
void UserForcedBreak( int sig)
{
  CleanUp();
  exit(0);
}


/** <!--********************************************************************-->
 *
 * @fn void CTIinstallInterruptHandlers()
 *
 *   @brief  installs interrupt handlers
 *
 ******************************************************************************/

void CTIinstallInterruptHandlers()
{
  DBUG_ENTER("CTIinstallInterruptHandlers");

  signal( SIGSEGV, InternalCompilerErrorBreak); /* Segmentation Fault */
  signal( SIGBUS, InternalCompilerErrorBreak);  /* Bus Error */
  signal( SIGINT, UserForcedBreak);     /* Interrupt (Control-C) */

  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void CTIerror( const char *format, ...)
 *
 *   @brief  produces an error message without file name and line number.
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIerror( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIerror");

  va_start( arg_p, format);

  fprintf( stderr, "\n");
  PrintMessage( error_message_header, format, arg_p);

  va_end(arg_p);

  errors++; 

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIerrorLine( int line, const char *format, ...)
 *
 *   @brief  produces an error message preceded by file name and line number.
 *
 *           
 *   @param line  line number
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIerrorLine( int line, const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIerrorLine");

  va_start( arg_p, format);

  fprintf( stderr, "\n");
  fprintf( stderr, "%sline %d\n", 
           error_message_header, line);
  PrintMessage( error_message_header, format, arg_p);

  va_end(arg_p);

  errors++; 

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIerrorLineVA( int line, const char *format, va_list arg_p)
 *
 *   @brief  produces an error message preceded by file name and line number.
 *
 *         
 *   @param line  line number
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIerrorLineVA( int line, const char *format, va_list arg_p)
{ 
  DBUG_ENTER("CTIerrorLineVA");
 
  fprintf( stderr, "\n");
  fprintf( stderr, "%sline %d\n",
           error_message_header, line);
  PrintMessage( error_message_header, format, arg_p);

  errors++; 

  DBUG_VOID_RETURN;
}
  



/** <!--********************************************************************-->
 *
 * @fn void CTIerrorContinued( const char *format, ...)
 *
 *   @brief  continues an error message without file name and line number.
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIerrorContinued( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIerrorContinued");

  va_start( arg_p, format);

  PrintMessage( error_message_header, format, arg_p);

  va_end(arg_p);

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn int CTIgetErrorMessageLineLength( )
 *
 *   @brief  yields useful line length for error messages
 *
 *   @return line length
 *
 ******************************************************************************/

int CTIgetErrorMessageLineLength( )
{
  DBUG_ENTER("CTIgetErrorMessageLineLength");
  
  DBUG_RETURN( message_line_length - strlen( error_message_header));
}


/** <!--********************************************************************-->
 *
 * @fn void CTIabortOnBottom( const char *err_msg)
 *
 *   @brief   produces an error message from preprocessed error message
 *            containing @ symbols as line breaks.
 *
 *   @param err_msg  pre-processed error message
 *
 ******************************************************************************/

void CTIabortOnBottom( char *err_msg)
{
  char *line;

  DBUG_ENTER("CTIabortOnBottom");

  fprintf( stderr, "\n");

  line = strtok( err_msg, "@");

  while (line != NULL) {
    fprintf( stderr, "%s%s\n", error_message_header, line);
    line = strtok( NULL, "@");
  }

  errors++;

  AbortCompilation();

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIabort( const char *format, ...)
 *
 *   @brief   produces an error message without file name and line number
 *            and terminates the compilation process.
 *           
 *   @param line  line number
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIabort( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIabort");

  va_start( arg_p, format);

  fprintf( stderr, "\n");
  PrintMessage( abort_message_header, format, arg_p);

  va_end(arg_p);

  errors++; 

  AbortCompilation();

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIabortLine( int line, const char *format, ...)
 *
 *   @brief   produces an error message preceded by file name and line number
 *            and terminates the compilation process.
 *           
 *   @param line  line number
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIabortLine( int line, const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIabortLine");

  va_start( arg_p, format);

  fprintf( stderr, "\n");
  fprintf( stderr, "%sline %d\n",
           abort_message_header, line);
  PrintMessage( abort_message_header, format, arg_p);

  va_end(arg_p);

  errors++; 

  AbortCompilation();

  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIabortOnError()
 *
 *   @brief  terminates compilation process if errors have occurred.
 *
 ******************************************************************************/

void CTIabortOnError()
{
  DBUG_ENTER("CTIabortOnError");

  if (errors > 0) {
    AbortCompilation();
  }
  
  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void CTIwarnLine( int line, const char *format, ...)
 *
 *   @brief   produces a warning message preceded by file name and line number.
 *           
 *   @param line  line number
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIwarnLine( int line, const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIwarnLine");

  if (global.verbosity >= 1) {
    va_start( arg_p, format);

    fprintf(stderr, "%sline %d\n", warn_message_header, line);
    PrintMessage( warn_message_header, format, arg_p);

    va_end(arg_p);

    warnings++; 
  }
  
  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void CTIwarn( const char *format, ...)
 *
 *   @brief   produces a warning message without file name and line number.
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIwarn( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIwarn");

  if (global.verbosity >= 1) {
    va_start( arg_p, format);

    PrintMessage( warn_message_header, format, arg_p);

    va_end(arg_p);

    warnings++; 
  }
  
  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTIwarnContinued( const char *format, ...)
 *
 *   @brief  continues a warning message without file name and line number.
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIwarnContinued( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIwarnContinued");

  if (global.verbosity >= 1) {
    va_start( arg_p, format);

    PrintMessage( warn_message_header, format, arg_p);

    va_end(arg_p);
  }
  
  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn int CTIgetWarnMessageLineLength( )
 *
 *   @brief  yields useful line length for warning messages
 *
 *   @return line length
 *
 ******************************************************************************/

int CTIgetWarnMessageLineLength( )
{
  DBUG_ENTER("CTIgetWarnMessageLineLength");
  
  DBUG_RETURN( message_line_length - strlen( warn_message_header));
}



/** <!--********************************************************************-->
 *
 * @fn void CTIstate( const char *format, ...)
 *
 *   @brief  produces full compile time information output (verbose level 3)
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTIstate( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTIstate");

  if (global.verbosity >= 3) {
    va_start( arg_p, format);

    PrintMessage( state_message_header, format, arg_p);

    va_end(arg_p);
  }
  
  DBUG_VOID_RETURN;
}


/** <!--********************************************************************-->
 *
 * @fn void CTInote( const char *format, ...)
 *
 *   @brief  produces basic compile time information output (verbose level 2)
 *
 *   @param format  format string like in printf
 *
 ******************************************************************************/

void CTInote( const char *format, ...)
{ 
  va_list arg_p;

  DBUG_ENTER("CTInote");

  if (global.verbosity >= 2) {
    va_start( arg_p, format);

    PrintMessage( note_message_header, format, arg_p);

    va_end(arg_p);
  }
  
  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void CTIterminateCompilation()
 *
 *   @brief  terminates successful compilation process
 *
 ******************************************************************************/

void CTIterminateCompilation( node *syntax_tree)
{
  DBUG_ENTER("CTIterminateCompilation");
  
  /*
   *  At last, we display a success message.
   */

  CTIstate( " ");
  CTIstate( "*** Evaluation successful ***");
  
  CTIstate( "*** 0 error(s), %d warning(s)", warnings);     
  CTIstate( " ");

  exit( 0);

  DBUG_VOID_RETURN;
}



/** <!--********************************************************************-->
 *
 * @fn void CTIabortOutOfMemory( unsigned int request)
 *
 *   @brief   produces a specific "out of memory" error message
 *            without file name and line number and terminates the 
 *            compilation process.
 * 
 *            This very special function is needed because the normal 
 *            procedure of formatting a message may require further
 *            allocation of memory, which in this very case generates
 *            a vicious circle of error messages instead of terminating
 *            compilation properly.
 *           
 *   @param request size of requested memory
 *
 ******************************************************************************/

void CTIabortOutOfMemory( unsigned int request)
{ 
  DBUG_ENTER("CTIabortOutOfMemory");

  fprintf( stderr,
	   "\n"
	   "%sOut of memory:\n"
	   "%s %u bytes requested\n",
	   abort_message_header, 
	   abort_message_header, request);

  errors++; 

  AbortCompilation();

  DBUG_VOID_RETURN;
}


