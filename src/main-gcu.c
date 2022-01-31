/* File: main-gcu.c */

/* Purpose: Somewhat generic Unix Curses support for Angband */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * Note that this package is not intended to support non-Unix machines,
 * nor is it intended to support VMS or __MINT__ or other bizarre setups.
 * The original "main-cur.c" can deal with those situations, but note that
 * "main-cur.c" is hopelessly out of date.  Also, this package assumes
 * that the underlying "curses" handles "nonl()" correctly, as well as
 * "cbreak()", but see the "OPTION" below.
 */


/*
 * Include curses first, since it messes with "bool"
 */
#include <curses.h>


/*
 * Hack -- undo some silly curses stuff
 */
#ifdef bool
# undef bool
#else
# define bool bool_hack
#endif


/*
 * Now, include the angband header file
 */
#include "angband.h"


/*
 * Hack -- try to guess which systems use what commands
 * Hack -- allow one of the "USE_Txxxxx" flags to be pre-set.
 * Mega-Hack -- try to guess when "POSIX" is available.
 * If the user defines two of these, we will probably crash.
 */
#if !defined(USE_TPOSIX)
# if !defined(USE_TERMIO) && !defined(USE_TCHARS)
#  if defined(_POSIX_VERSION)
#   define USE_TPOSIX
#  else
#   if defined(USG) || defined(linux) || defined(SOLARIS)
#    define USE_TERMIO
#   else
#    define USE_TCHARS
#   endif
#  endif
# endif
#endif

/*
 * Hack -- Amiga uses "fake curses" and cannot do any of this stuff
 */
#if defined(AMIGA)
# undef USE_TPOSIX
# undef USE_TERMIO
# undef USE_TCHARS
#endif




/*
 * POSIX stuff
 */
#ifdef USE_TPOSIX
# include <sys/ioctl.h>
# include <termios.h>
#endif

/*
 * One version needs this file
 */
#ifdef USE_TERMIO
# include <sys/ioctl.h>
# include <termio.h>
#endif

/*
 * The other needs this file
 */
#ifdef USE_TCHARS
# include <sys/ioctl.h>
# include <sys/resource.h>
# include <sys/param.h>
# include <sys/file.h>
# include <sys/types.h>
#endif

