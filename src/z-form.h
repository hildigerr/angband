/* File z-form.h */

#ifndef INCLUDED_Z_FORM_H
#define INCLUDED_Z_FORM_H

#include "h-include.h"

/*
 * Some useful functions dealing with varargs and formatting.
 */


# include <stdarg.h>



/**** Available Functions ****/

/* Vararg interface to plog() */
extern void plog_fmt(const char *fmt, ...);

/* Vararg interface to quit() */
extern void quit_fmt(const char *fmt, ...);


#endif

