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




#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "str.h"

#include "memory.h"
#include "dbug.h"


/*******************************************************************************
 *
 * Description: Copy string and allocate memory for new string.
 *
 * Parameters: - source, string to copy
 *
 * Return: - new copied string
 *
 *******************************************************************************/

char *STRcpy( const char *source)
{
  char *ret;
   
  DBUG_ENTER( "STRcpy");
   
  if (source != NULL) {
    ret = (char*) MEMmalloc( sizeof( char) * (STRlen( source) + 1));
    strcpy( ret, source);
  }
  else {
    ret = NULL;
  }

  DBUG_RETURN( ret);
}


/*******************************************************************************
 *
 * Description: Copy string and allocate memory for new string. 
 *              Copy only maxlen characters.
 *
 * Parameters: - source, string to copy
 *             - maxlen, number of characters to copy
 *
 * Return: - new copied string
 *
 *******************************************************************************/

char *STRncpy( const char *source, int maxlen)
{
  char *ret;
  int max;
   
  DBUG_ENTER( "STRncpy");
   
  if (source != NULL) {
    max = STRlen( source);
    if (max > maxlen) {
      max = maxlen;
    }

    ret = (char*) MEMmalloc( sizeof( char) * ( max + 1));
    strncpy( ret, source, max);

    /* make sure string ends with 0 */
    ret[max] = '\0';
  }
  else {
    ret = NULL;
  }

  DBUG_RETURN(ret);
}


/** <!--********************************************************************-->
 *                                                                             
 * @fn char *STRsubStr( const char *string, int start, int len)
 *                                                                             
 * @brief copy part of a string from start to start + len.
 *        if len is <0 then len is relative to the length of the string.
 *                                                                           
 *****************************************************************************/
char *STRsubStr( const char *string, int start, int len)
{
  int strlen = 0;
  char *ret = NULL;

  DBUG_ENTER( "STRsubStr");
  
  strlen = STRlen( string);

  if ( len < 0){
    len = strlen + len; /* + - => - */
  }
  
  if ( ( start + len) > strlen){ /* to long take what we can */
    len = strlen - start;
  } 
  
  if ( start > len){
    ret = STRnull();
  } else {
    ret = memcpy( MEMmalloc( sizeof( char) * ( len + 1)),
                  string + start, /* move to start of sub string */
                  len); 
    ret[len] = '\0';
  }

  DBUG_RETURN( ret);
}

/** <!--********************************************************************-->
 *                                                                             
 * @fn char *STRnull( )
 *                                                                             
 * @brief return an empty string
 *                                                                           
 *****************************************************************************/
char *STRnull( )
{
  char *ret = NULL;
  DBUG_ENTER( "STRnull");
  
  ret = MEMmalloc( sizeof( char) * 1);
  ret[0] = '\0';

  DBUG_RETURN( ret);
}


/*******************************************************************************
 *
 * Description: Concatenate two strings and allocate memory for new string.
 *
 * Parameters: - first, first string
 *             - second, second string
 *
 * Return: - new concatenated string
 *
 *******************************************************************************/

char *STRcat( const char *first, const char* second)
{
  char *result;

  DBUG_ENTER( "STRcat");
  
  if (first == NULL) {
    result = STRcpy(second);
  }
  else if (second == NULL) {
    result = STRcpy(first);
  }
  else {
    result = (char *) MEMmalloc( STRlen( first) + STRlen( second) + 1);

    strcpy( result, first);
    strcat( result, second);
  }
  
  DBUG_RETURN( result);
}

/*******************************************************************************
 *
 * Description: Concatenate N strings and allocate memory for new string.
 *
 * Parameters: - n, number of strings
 *             - ..., n amount of "const char *"-type strings
 *
 * Return: - new concatenated string
 *
 *******************************************************************************/

char *STRcatn(int n, ...)
{
  int i;
  int length;
  char *result;
  const char *ptr;
  va_list arg_list;

  DBUG_ENTER("STRcatn");

  DBUG_ASSERTF( n>2, ("STRcatn called with %d arguments", n));
  
  va_start( arg_list, n);

  length = 0;
  
  for (i = 0; i < n; ++i) {
    ptr = va_arg(arg_list, const char *);
    if (ptr != NULL) {
      length += STRlen(ptr);
    }
  }

  va_end( arg_list);

  if (length == 0) {
    result = NULL;
  }
  else {
    result = (char *) MEMmalloc(length + 1);
    result[0] = '\0';
  
    va_start( arg_list, n);

    for (i = 0; i < n; ++i) {
      ptr = va_arg( arg_list, const char *);
      if (ptr != NULL) {
        strcat( result, ptr);
      }
    }

    va_end( arg_list);
  }
  
  DBUG_RETURN(result);
}


/*******************************************************************************
 *
 * Description: Compare two strings.
 *
 * Parameters: - first, first string to compare
 *             - second, second string to compare
 *
 * Return: - TRUE, string contents are equal
 *         - FALSE, string contents are not equal
 *
 *******************************************************************************/

bool STReq( const char *first, const char *second)
{
  bool res;

  DBUG_ENTER("STReq");

  if ((first == NULL) && (second == NULL)) {
    res = TRUE;
  }
  else if ((first == NULL) || (second == NULL)) {
    res = FALSE;
  }
  else {
    res = (0 == strcmp( first, second));
  }
  
  DBUG_RETURN( res);
}



/*******************************************************************************
 *
 * Description: Compare two strings in a case insensitive way.
 *
 * Parameters: - first, first string to compare
 *             - second, second string to compare
 *
 * Return: - TRUE, string contents are equal
 *         - FALSE, string contents are not equal
 *
 *******************************************************************************/

bool STReqci( const char *first, const char *second)
{
  bool res;
  int i;
  
  DBUG_ENTER("STReqci");

  if ((first == NULL) && (second == NULL)) {
    res = TRUE;
  }
  else if ((first == NULL) || (second == NULL)) {
    res = FALSE;
  }
  else {
    i = 0;
    while ((first[i] != '\0') && (second[i] != '\0') 
           && (tolower(first[i]) == tolower(second[i]))) {
      i+=1;
    }
    if ((first[i] == '\0') && (second[i] == '\0')) {
      res = TRUE;
    }
    else {
      res = FALSE;
    }
  }
  
  DBUG_RETURN( res);
}


/*******************************************************************************
 *
 * Description: Compare two strings.
 *
 * Parameters: - first, first string to compare
 *             - second, second string to compare
 *             - n, number of relevant characters
 *
 * Return: - TRUE, relevant prefixes of strings are equal
 *         - FALSE, relevant prefixes of strings are not equal
 *
 *******************************************************************************/

bool STReqn(const char *first, const char* second, int n)
{
  bool res;

  DBUG_ENTER("STReq");

  if ((first == NULL) && (second == NULL)) {
    res = TRUE;
  }
  else if ((first == NULL) || (second == NULL)) {
    if (n == 0)  {
      res = TRUE;
    }
    else {
      res = FALSE;
    }
  }
  else {
    res = (0 == strncmp( first, second, n));
  }
  
  DBUG_RETURN( res);
}


/*******************************************************************************
 *
 * Description: Checks if prefix is prefix of str
 *
 * Parameters: - prefix, first string to compare
 *             - str, second string to compare
 *
 * Return: - TRUE, prefix is prefix of str
 *         - FALSE, otherwise
 *
 *******************************************************************************/

bool STRprefix( const char *prefix, const char *str)
{
  bool res;
  
  DBUG_ENTER("STRprefix");
  
  if (prefix == NULL) {
    res = TRUE;
  }
  else {
    if (str == NULL) {
      res = FALSE;
    } else {
      int plen = STRlen( prefix);
  
      if (STRlen(str) < plen) {
        res = FALSE;
      } else {
        res = (0 == strncmp( prefix, str, STRlen( prefix)));
      }
    }
  }
  
  DBUG_RETURN( res);
}

/** <!--********************************************************************-->
 *                                                                             
 * @fn bool *STRsuffix( const char *suffix, const char *str) 
 *                                                                            
 * @brief return true if suffix is the end of str, else return false. 
 *                                                                             
 *****************************************************************************/
bool STRsuffix( const char *suffix, const char *str)
{ 
  bool res = FALSE;
  
  DBUG_ENTER( "STRsuffix");

  if ( STRlen( suffix) > STRlen( str)){
    res = FALSE;
  } else {
    str = str + STRlen( str) - STRlen( suffix);
    res = (0 == strcmp( str, suffix));
  }

  DBUG_RETURN( res);
} 

/*******************************************************************************
 *
 * Description: Checks if prefix is prefix of str
 *
 * Parameters: - sub, first string to compare
 *             - str, second string to compare
 *
 * Return: - TRUE, sub is substring of str
 *         - FALSE, otherwise
 *
 *******************************************************************************/

bool STRsub( const char *sub, const char *str)
{
  bool res;
  
  DBUG_ENTER("STRsub");
  
  if (sub == NULL) {
    res = TRUE;
  }
  else {
    if (str == NULL) {
      res = FALSE;
    }
    else {
      res = (NULL != strstr( str, sub));
    }
  }
  
  DBUG_RETURN( res);
}

  


/*******************************************************************************
 *
 * Description: Yield length of string
 *
 *  Mostly to provide a complete interface and to avoid using standard
 *  string facilities throughout the compiler.
 *
 *******************************************************************************/

int STRlen( const char *s)
{
  int len;
  
  DBUG_ENTER("STRlen");

  if (s == NULL) {
    len = 0;
  }
  else {
    len = strlen( s);
  }
  
  DBUG_RETURN( len);
}


/*******************************************************************************
 *
 * Description: Tokenize string. On first call the str will be copied to internal
 *              static variable, next calls str should be NULL. With last call the 
 *              allocated memory of the copy will be freed.
 *
 *              In contrast to strtok, STRtok leaves the argument string untouched
 *              and always allocates the tokens in fresh memory.
 *
 * Parameters: - str, string to tokenize
 *             - tok, tokenizer
 *
 * Return: - pointer to the next token
 *         - NULL, no more tokens
 *
 *******************************************************************************/

static
bool CharInString( char c, const char *str)
{
  int i;
  bool res;
  
  DBUG_ENTER("CharInString");
  
  if ((str == NULL) || (c == '\0')) {
    res = FALSE;
  }
  else {
    i = 0;
    while ((str[i] != '\0') && (str[i] != c)) {
      i += 1;
    }
    res = str[i] != '\0';
  }
  
  DBUG_RETURN( res);
}

char *STRtok( const char *first, const char *sep)
{
  static char *keep_string = NULL;
  static char *current = NULL;
  char *ret;
  int i;
  
  DBUG_ENTER( "STRtok");

  if (first != NULL) {
    if (keep_string != NULL) {
      keep_string = MEMfree( keep_string);
    }
    keep_string = STRcpy( first);
    current = keep_string;
  }
  
  if (current == NULL) {
    ret = NULL;
  }
  else {
    i = 0;
    while ((current[i] != '\0') && !CharInString( current[i], sep)) {
      i += 1;
    }

    if (current[i] == '\0') {
      ret = STRcpy( current);
      current = NULL;
    }
    else {
      current[i] = '\0';
      ret = STRcpy( current);
      current += i+1;
    }
  }
  
  DBUG_RETURN( ret);
}


/*******************************************************************************
 *
 * Description: 
 *
 *  yields either the argument string if it is not NULL or an empty constant
 *  string otherwise.
 *
 * This is helpful for instance when printing strings with the %s conversion
 * specifier and the string to print may be NULL.
 *
 *******************************************************************************/

char *STRonNull( char *alt, char *str)
{
  char *res;
  
  DBUG_ENTER("STRonNull");
  
  if (str == NULL) {
    res = alt;
  }
  else {
    res = str;
  }
  
  DBUG_RETURN( res);
}



/*******************************************************************************
 *
 * Description: Convert integer to string in decimal representation.
 *
 * Parameters: - number to convert
 *
 * Return: - new allocated string representation of number
 *
 *******************************************************************************/

char *STRitoa( int number)
{
  char *str;
  int num;
  
  DBUG_ENTER("STRitoa");

  str = (char *) MEMmalloc( sizeof(int) * 4);
  num = snprintf( str, (sizeof(int) * 4) - 1, "%d", number);
  DBUG_ASSERT( num<(sizeof(int) * 4) - 1, "Trouble in STRitoa");
  
  DBUG_RETURN( str);
}



/** <!-- ****************************************************************** -->
 * @brief Substitute all occurrences of token in str with subst
 * 
 * @param str The string in which to make substitutions
 * @param token a string to substitute occurrences of in str
 * @param subst a string to substitute tokens with 
 * 
 * @return A new String or NULL if str is NULL
 ******************************************************************************/
char *STRsubstToken( const char *str, const char *token, const char *subst)
{
  int occurrences, tlen, slen;
  const char *found; 
  char *pos;
  char *result;
  
  DBUG_ENTER( "STRsubstToken");

  /* Find number of occurrences of token in str */
  occurrences = 0;
  tlen = STRlen( token);
  slen = STRlen( subst);
  found = strstr(str, token);

  while (found != NULL) {
    occurrences++;
    found = strstr( found + tlen, token);
  }

  /* Make substitutions */
  result = MEMmalloc( (STRlen( str) + 
                       (occurrences * (STRlen(subst) - tlen)) +
                       1) * sizeof(char));

  pos = result;
  while( *str != '\0') {
    if (STRprefix( token, str)) {
      strncpy( pos, subst, slen);
      pos += slen;
      str += tlen;
    }
    else {
      *(pos++) = *(str++);
    }
  }
  *pos = '\0';

  DBUG_RETURN( result);
}
