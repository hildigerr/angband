/* File z-util.h */

#ifndef INCLUDED_Z_UTIL_H
#define INCLUDED_Z_UTIL_H

#include "h-include.h"


/**** Available variables ****/

/* A cptr to the name of the program */
extern cptr argv0;


/**** Available Functions ****/

/* Test suffix */
extern bool suffix(cptr big, cptr small);

#ifndef HAS_STRICMP
extern int stricmp(cptr a, cptr b);
#endif

/* Print an error message */
extern void plog(cptr str);

/* Exit, perhaps with a message */
extern void quit(cptr str);

/* Dump core, with optional message */
extern void core(cptr str);



#endif

