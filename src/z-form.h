/* File z-form.h */

#ifndef INCLUDED_Z_FORM_H
#define INCLUDED_Z_FORM_H

#include "h-include.h"

/*
 * Some useful functions dealing with varargs and formatting.
 */


/*
 * Include the correct vararg's support
 */
#ifndef __MAKEDEPEND__
# include <stdarg.h>
#endif



/**** Available Functions ****/

/* Vararg interface to plog() */
extern void plog_fmt(cptr fmt, ...);

/* Vararg interface to quit() */
extern void quit_fmt(cptr fmt, ...);


#endif

