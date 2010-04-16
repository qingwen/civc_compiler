/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BRACKET_L = 258,
     BRACKET_R = 259,
     COMMA = 260,
     SEMICOLON = 261,
     BRACE_L = 262,
     BRACE_R = 263,
     MINUS = 264,
     PLUS = 265,
     STAR = 266,
     SLASH = 267,
     PERCENT = 268,
     LE = 269,
     LT = 270,
     GE = 271,
     GT = 272,
     EQ = 273,
     NE = 274,
     OR = 275,
     AND = 276,
     TRUEVAL = 277,
     FALSEVAL = 278,
     LET = 279,
     VOID = 280,
     INT = 281,
     NUM = 282,
     FLOAT = 283,
     ID = 284
   };
#endif
/* Tokens.  */
#define BRACKET_L 258
#define BRACKET_R 259
#define COMMA 260
#define SEMICOLON 261
#define BRACE_L 262
#define BRACE_R 263
#define MINUS 264
#define PLUS 265
#define STAR 266
#define SLASH 267
#define PERCENT 268
#define LE 269
#define LT 270
#define GE 271
#define GT 272
#define EQ 273
#define NE 274
#define OR 275
#define AND 276
#define TRUEVAL 277
#define FALSEVAL 278
#define LET 279
#define VOID 280
#define INT 281
#define NUM 282
#define FLOAT 283
#define ID 284




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 22 "src/scanparse/foo2.y"
{
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cflt;
 binop               cbinop;
 node               *node;
}
/* Line 1489 of yacc.c.  */
#line 116 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

