/* File z-util.h */

#ifndef INCLUDED_Z_UTIL_H
#define INCLUDED_Z_UTIL_H

#include "h-include.h"


/**** Available variables ****/

/* A pointer to the name of the program */
extern const char *argv0;


/**** Available Functions ****/

#ifndef HAS_STRICMP
extern int stricmp(const char *a, const char *b);
#endif

/* Print an error message */
extern void plog(const char *str);

/* Exit, perhaps with a message */
extern void quit(const char *str);

/* Dump core, with optional message */
extern void core(const cahr *str);



#endif

