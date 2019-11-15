/* File: z-virt.h */

#ifndef INCLUDED_Z_VIRT_H
#define INCLUDED_Z_VIRT_H

#include "h-include.h"

/*
 * Memory management routines.
 *
 * Note that it is assumed that "memset()" will function correctly,
 * in particular, that it returns its first argument.
 */


/**** Available Routines ****/

#ifndef HAS_MEMSET
extern char *memset(char*, int, unsigned long);
#endif



#endif



