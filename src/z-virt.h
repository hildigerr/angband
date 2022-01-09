/* File: z-virt.h */

#ifndef INCLUDED_Z_VIRT_H
#define INCLUDED_Z_VIRT_H

#include "h-include.h"

/*
 * Memory management routines.
 *
 * The string_make() and string_free() routines handle dynamic strings.
 * A dynamic string is a string allocated at run-time, but not really
 * intended to be modified once allocated.
 *
 * Note that it is assumed that "memset()" will function correctly,
 * in particular, that it returns its first argument.
 */


/**** Available Routines ****/

/* De-allocate a given amount of memory */
int rnfree(vptr p, huge len);

/* Panic, attempt to Allocate 'len' bytes */
extern vptr rpanic(huge len);

/* Allocate (and return) 'len', or dump core */
extern vptr ralloc(huge len);

/* Create a "dynamic string" */
const char *string_make(const char *str);

/* Free a string allocated with "string_make()" */
int string_free(const char* str);

#ifndef HAS_MEMSET
extern char *memset(char*, int, huge);
#endif




/**** Memory Macros ****/


/* Wipe an array of N things of type T at location P, return T */
#define C_WIPE(P,N,T) \
	memset((char*)(P),0,C_SIZE(N,T))



#endif



