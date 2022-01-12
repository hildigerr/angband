/* File: H-macros.h */

#ifndef INCLUDED_H_MACROS_H
#define INCLUDED_H_MACROS_H

/*
 * Basic macros (macros always in all capitals).
 *
 * These include: "Functions", "Aliases", and "Procedures"
 *
 * Note that ABS() + POM() allows sprintf("%+d",x), which does not
 * always work, to be translated into sprintf("%c%d",POM(x),ABS(x))
 */


/**** Assertions and Preventions ****/

#ifdef CHECK_ASSERTIONS

/* Note that the following functions require "z-form.h" */

/* Allow Certain Conditions 'C' to be ABSOLUTELY ILLEGAL */
#define PREVENT(C) \
	if (C) core_fmt("PREVENT(Line %d in file %s)", __LINE__, __FILE__);

/* Allow Certain Conditions 'C' to be ABSOLUTELY NECESSARY */
#define ASSERT(C) \
	if (!(C)) core_fmt("ASSERT(Line %d in file %s)", __LINE__, __FILE__);

#else

/* Ignore Preventions */
#define PREVENT(C)

/* Ignore Assertions */
#define ASSERT(C)

#endif



/**** Simple "Functions" ****/

/* Force a character to lowercase/uppercase */
#define FORCELOWER(A)  ((isupper((A)))? tolower((A)): (A))
#define FORCEUPPER(A)  ((islower((A)))? toupper((A)): (A))

#ifndef STREQ	/* String Comparison (see also "streq()") */
#define STREQ(A, B)    (!strcmp((A), (B)))
#endif

#ifndef MIN		/* Non-typed minimum value */
#define MIN(a,b)	(((a) > (b)) ? (b)  : (a))
#endif

#ifndef MAX		/* Non-typed maximum value */
#define MAX(a,b)	(((a) < (b)) ? (b)  : (a))
#endif

#ifndef ABS		/* Non-typed absolute value */
#define ABS(a)		(((a) < 0)   ? (-(a)) : (a))
#endif

#ifndef POM		/* Used with "ABS" for printf("%+d",i) */
#define POM(a)		(((a) < 0)   ? '-' : '+')
#endif

#ifndef SGN		/* Un-typed "sign" function */
#define SGN(a)		(((a) < 0)   ? (-1) : ((a) != 0))
#endif

#ifndef SQR		/* Untyped "square" function */
#define SQR(a)		((a) * (a))
#endif

#endif

