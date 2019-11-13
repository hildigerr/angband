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

#endif

