/* File: h-defines.h */

#ifndef INCLUDED_H_DEFINES_H
#define INCLUDED_H_DEFINES_H

/*
 * Define some simple constants
 */


/* Hack */
#ifndef NULL
# ifdef __STDC__
#  define NULL ((void *)0)
# else
#  define NULL (char *)0
# endif /* __STDC__ */
#endif /* NULL */


/* The constants "TRUE" and "FALSE" */

#undef TRUE
#define TRUE	1

#undef FALSE
#define FALSE	0


#endif

