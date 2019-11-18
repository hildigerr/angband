/* config.h: configuration definitions
 *
 * Copyright (c) 1989 James E. Wilson
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* Person to bother if something goes wrong.
 */

#define WIZARD	"root"



/* Other miscellaneous defines that can be configured as the local maintainer
 * wishes.
 */

#undef CHECK_HOURS            /* define if checking the 'hours' file          */
#define ALLOW_FIDDLING       /* Allow the players to copy save files         */
#define ALLOW_SCORE          /* Allow the user to check his score (v-key)    */
#define ALLOW_ARTIFACT_CHECK /* Allow the user to check artifacts            */
#define ALLOW_CHECK_UNIQUES  /* Allow player to check (dead) unique monsters */
#define TARGET               /* Enable targeting mode                        */
#define AUTOROLLER           /* Allow autorolling of characters              */
#undef NICE		             /* Be nice to other users during autorolling    */
#define SATISFY_HUNGER		 /* Do 'satisfy hunger' rather then 'create food'
								to reduce the tedium for handling food. -CWS */


/*****************************************************
 * files used by moria, set these to valid pathnames *
 *****************************************************/


/*
 * OPTION: Set the "default" path to the angband "lib" directory.
 * Angband will use this value if it cannot getenv("ANGBAND_PATH").
 * The final slash is optional in either case.  Not used on Macintosh.
 * By default, the system expects the "angband" program to be located
 * in the same directory as the "lib" directory.  This can be changed.
 * Note that the "ANGBAND_PATH" environment variable over-rides this.
 */
#define DEFAULT_PATH "./lib/"


/* this sets the default user interface
 * to use the original key bindings (keypad for movement) set ROGUE_LIKE
 * to FALSE, to use the rogue-like key bindings (vi style movement)
 * set ROGUE_LIKE to TRUE
 * if you change this, you only need to recompile main.c */

#define ROGUE_LIKE TRUE


/* for the AFS distributed file system, define this to ensure that
   the program is secure with respect to the setuid code, this prohibits
   inferior shells, also does not relinquish setuid priviledges at the start,
   but instead calls the AFS library routines bePlayer(), beGames(),
   and Authenticate() */

/* #define SECURE */





/****************************************************************************
 * System dependent defines follow, you should not need to change anything  *
 * below (if you have a supported system).  If you run into problems during *
 * compilation, you might want to check the defines below.                  *
 ****************************************************************************/












#ifdef MAC
#ifdef RSRC
#define MACRSRC		/* i.e., we're building the resources */
#else
#define MACGAME		/* i.e., we're building the game */
#endif
#endif

#ifdef MAC
/* Screen dimensions */
#define SCRN_ROWS	24
#define SCRN_COLS	80
#endif


#ifdef SECURE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif


/*
 * Make sure that "usleep()" works.
 *
 * In general, this is only referenced by "Unix" machines.
 */
#if !defined(HPUX) && !defined(ultrix) && !defined(SOLARIS)
# define HAS_USLEEP
#endif



