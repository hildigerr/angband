/* File: h-types.h */

#ifndef INCLUDED_H_TYPES_H
#define INCLUDED_H_TYPES_H

/*
 * Basic "types".
 *
 * Note the attempt to make all basic types have 4 letters.
 * This improves readibility and standardizes the code.
 *
 * Note that on some machines, apparently "signed char" is illegal.
 *
 * It must be true that char/byte takes exactly 1 byte
 * It must be true that sind/uind takes exactly 2 bytes
 * It must be true that sbig/ubig takes exactly 4 bytes
 *
 * On Sparc's, a sint takes 4 bytes (2 is legal)
 * On Sparc's, a uint takes 4 bytes (2 is legal)
 * On Sparc's, a long takes 4 bytes (8 is legal)
 * On Sparc's, a huge takes 4 bytes (8 is legal)
 * On Sparc's, a vptr takes 4 bytes (8 is legal)
 * On Sparc's, a real takes 8 bytes (4 is legal)
 *
 * Note that some files have already been included by "h-include.h"
 * These include <stdio.h> and <sys/types>, which define some types
 * In particular, uint is defined so we do not have to define it
 *
 * Also, see <limits.h> for min/max values for sind, uind, long, huge
 * (SHRT_MIN, SHRT_MAX, USHRT_MAX, LONG_MIN, LONG_MAX, ULONG_MAX)
 * These limits should be verified and coded into "h-constant.h".
 */



/*** Special 4 letter names for some standard types ***/


/* A standard pointer (to "void" because ANSI C says so) */
typedef void *vptr;

/* A simple pointer (to unmodifiable strings) */
typedef const char *cptr;


/* Since float's are silly, hard code real numbers as doubles */
typedef double real;


/* Error codes for function return values */
/* Success = 0, Failure = -N, Problem = +N */
typedef int errr;


/*
 * Some annoying machines define "uint" in some "include" file
 * Note that this "redefinition" should work on any machine.
 */
#if !defined(MACINTOSH) && !defined(__EMX__)
# define uint uint_hack
#endif

/*
 * Some annoying machines (Windows with Turbo C) reserve "huge".
 * Note that this "redefinition" should work on any machine.
 */
#if defined(_Windows)
# define huge huge_hack
#endif


/* Note that "signed char" is not always "defined" */
/* A (possibly signed) char (a byte) */
/* typedef char char; */

/* An unsigned byte of memory */
typedef unsigned char byte;

/* Simple True/False type */
typedef char bool;


/* A signed, standard integer (at least 2 bytes) */
typedef int sint;

/* An unsigned, "standard" integer (usually pre-defined) */
typedef unsigned int uint;


/* The largest signed integer there is (pre-defined) */
/* typedef long long; */

/* The largest unsigned integer there is */
typedef unsigned long huge;


/* Signed/Unsigned 16 bit value */
typedef signed short s16b;
typedef unsigned short u16b;

/* Signed/Unsigned 32 bit value */
#ifdef L64	/* 64 bit longs */
typedef signed int s32b;
typedef unsigned int u32b;
#else
typedef signed long s32b;
typedef unsigned long u32b;
#endif



#endif

