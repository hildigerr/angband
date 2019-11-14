/* File: H-macros.h */

#ifndef INCLUDED_H_MACROS_H
#define INCLUDED_H_MACROS_H

/*
 * Basic macros (macros always in all capitals).
 *
 * These include: "Functions", "Aliases", and "Procedures"
 *
 */



/**** Simple "Functions" ****/


#ifndef MIN		/* Non-typed minimum value */
#define MIN(a,b)	(((a) > (b)) ? (b)  : (a))
#endif

#ifndef MAX		/* Non-typed maximum value */
#define MAX(a,b)	(((a) < (b)) ? (b)  : (a))
#endif

#ifndef ABS		/* Non-typed absolute value */
#define ABS(a)		(((a) < 0)   ? (-(a)) : (a))
#endif

#endif

