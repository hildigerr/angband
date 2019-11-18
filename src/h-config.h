/* File: h-config.h */

#ifndef INCLUDED_H_CONFIG_H
#define INCLUDED_H_CONFIG_H

/*
 * Choose the hardware, operating system, and compiler.
 * Also, choose various "system level" compilation options.
 * A lot of these definitions take effect in "h-system.h"
 */


/*
 * no system definitions are needed for 4.3BSD, SUN OS, DG/UX
 */

/*
 * OPTION: Compile on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
 * Do not define it if you are on any kind of SUN OS.
 */
/* #define ultrix */

/*
 * OPTION: Compile on a Macintosh with MPW C 3.0
 */
/* #define MAC */

/*
 * OPTION: Compile on a SYS V version of UNIX
 */
/* #define SYS_V */

/*
 * OPTION: Compile on a SYS III version of UNIX
 */
/* #define SYS_III */

/*
 * OPTION: Compile on an ATARI ST with Mark Williams C
 * Warning: the support for the Atart ST is a total hack
 */
/* #define ATARIST_MWC */

/*
 * OPTION: Compile on a HPUX version of UNIX
 */
/* #define HPUX */

/*
 * OPTION: Compile on Solaris, treat it as System V
 */
#if defined(SOLARIS)
# define SYS_V
#endif

/*
 * OPTION: Compile on Pyramid, treat it as Ultrix
 */
#if defined(Pyramid)
# define ultrix
#endif

/*
 * Extract the "MSDOS" flag from the compiler
 */
#ifdef __MSDOS__
# ifndef MSDOS
#  define MSDOS
# endif
#endif



/*
 * Note that most compilers will do better WITHOUT "register" suggestions.
 * If your compiler is ancient or horrible, comment out the "define" below.
 */
#define register /* nothing */


/*
 * Note that old compilers do not understand "const".
 * If you have an good compiler, comment out the "define" below.
 */
#if !defined(__GNUC__) && !defined(__STDC__)
# define const /* nothing */
#endif


/*
 * L64 is defined if a long is 64-bit
 * The only such platform that angband is ported to is currently
 * DEC Alpha AXP running OSF/1 (OpenVMS uses 32-bit longs).
 */
#if defined(__alpha) && defined(__osf__)
# define L64		/* 64 bit longs */
#endif


/*
 * Prevent "lint" messages 
 */
#if defined(lint)
# if defined(SYS_V)
#  define RTPC_NO_NLS
# endif
#endif



/*
 * Lots of systems use USG
 */
#if defined(MAC) || defined(MSDOS) || \
    defined(SYS_III) || defined(SYS_V) || defined(HPUX) || \
    defined(ATARIST_MWC) || defined (__MINT__)
# undef USG
# define USG
#endif



/*
 * Every system seems to use its own symbol as a path separator.
 * Default to the standard Unix slash, but attempt to change this
 * for various other systems.
 */
#undef PATH_SEP
#define PATH_SEP "/"
#if defined(MSDOS) || defined(OS2) || defined(WINNT) || defined(__EMX__)
# undef PATH_SEP
# define PATH_SEP "\\"
#endif
#if defined(ATARIST_MWC) || defined(ATARI) || defined(ATARIST)
# undef PATH_SEP
# define PATH_SEP "\\"
#endif
#ifdef VMS
# undef PATH_SEP
# define PATH_SEP "."
#endif
#ifdef MAC
# undef PATH_SEP
# define PATH_SEP ":"
#endif



/* substitute strchr for index on USG versions of UNIX */
#if defined(SYS_V) || defined(MSDOS) || defined(MAC) || vms
#define index strchr
#endif


/*
 * OPTION: Define "HAS_STRICMP" only if stricmp() exists.
 */
#if defined (NeXT) || defined(HPUX) || defined(ultrix) || \
    defined(NCR3K) || defined(linux) || defined(ibm032) || \
    defined(__386BSD__) || defined(SOLARIS) || defined (__osf__)
# define stricmp strcasecmp
# define HAS_STRICMP
#endif

/*
 * Note that "Turbo C" defines "stricmp"
 */
#ifdef __TURBOC__
#define HAS_STRICMP
#endif



#endif


