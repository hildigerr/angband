/* config.h: configuration definitions
 *
 * Copyright (c) 1989 James E. Wilson
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* Person to bother if something goes wrong.
 * Recompile files.c and misc2.c if these change.
 */

#define WIZARD	"root"

/* There's a bug that results in invisible monsters for some reason.  I have a
 * workaround that may fix this, but it is a HACK and may result in other
 * problems, as I have not tested it enough.  Comment out the
 * "#define GROSS_HACK" to disable this.  (this is in creature.c at line 73)
 ******
 * This should no longer be needed for Angband 2.6, but it doesn't hurt to
 * leave it in. -CWS
 */

#define GROSS_HACK


/* Other miscellaneous defines that can be configured as the local maintainer
 * wishes.
 */

#define SET_UID		         /* define on multi-user systems                 */
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

/* Define OLD_FILEPATHS to use the old, nasty way of putting complete
 * paths directly into Angband.  Define NEW_FILEPATHS to use the new,
 * studly method (which uses the ANGBAND_PATH environment variable to
 * look for the files).                                         [cjh]
 */

#undef  OLD_FILEPATHS
#define NEW_FILEPATHS


/* Change this to something sensible, if necessary.  Angband will look
 * in this directory if the ANGBAND_PATH environment variable isn't   
 * set, or if the files aren't found in ANGBAND_PATH.            [cjh]
 */

#ifdef NEW_FILEPATHS
#define DEFAULT_PATH "/User/games/lib/angband"
#endif

#ifdef OLD_FILEPATHS

/* Try to fix filename inclusion in a portable fashion.
 * John Whitly@cs.Buffalo.edu says this works under gcc 2.5.5, but my
 * older version chokes.  I dunno. -CWS
 */

#ifdef __STDC__
#define LIBDIR(FILE) "/User/games/lib/angband/" #FILE
#else
#define LIBDIR(FILE) "/User/games/lib/angband/FILE"
#endif

/* probably unix */
#define ANGBAND_TST       LIBDIR(test)
#define ANGBAND_HOURS       LIBDIR(files/hours)
#define ANGBAND_NEWS       LIBDIR(files/news.hlp)
#define ANGBAND_TOP       LIBDIR(files/newscores)
#define ANGBAND_DIR_BONES     LIBDIR(bones)
#define ANGBAND_R_HELP      LIBDIR(files/cmds_r.hlp)
#define ANGBAND_O_HELP LIBDIR(files/cmds_o.hlp)
#define ANGBAND_W_HELP  LIBDIR(files/cmds_w.hlp)
#define ANGBAND_OWIZ_HELP LIBDIR(files/owizcmds.hlp)
#define ANGBAND_WELCOME   LIBDIR(files/welcome.hlp)
#define ANGBAND_LOG       LIBDIR(files/ANGBAND.log)
#define ANGBAND_VERSION       LIBDIR(files/version.hlp)
#define ANGBAND_LOAD      LIBDIR(files/loadcheck)
#define ANGBAND_WIZ       LIBDIR(files/wizards.txt)
#define ANGBAND_DIR_SAVE       LIBDIR(save)
#endif /* OLD_FILEPATHS */


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




#if defined(SOLARIS)
#include <netdb.h>
#endif


/****************************************************************************
 * System dependent defines follow, you should not need to change anything  *
 * below (if you have a supported system).  If you run into problems during *
 * compilation, you might want to check the defines below.                  *
 ****************************************************************************/







/* fix systems lacking usleep() -CWS 
 *
 * Note that Solaris 2.x users without the BSD compatibilty kit need to
 * define this as well.
 */

#if defined(HPUX) || defined(ultrix)
#define NEEDS_USLEEP
#endif

#ifdef NEEDS_USLEEP
#define usleep microsleep

#ifdef __STDC__
int microsleep(unsigned long);
#else
int microsleep();
#endif /* __STDC__ */

#endif



#ifdef SYS_III
char *index();
#endif





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

#if vms
#define getch _getch
#define unlink delete
#define lstat stat
#define exit uexit
#endif

#ifdef SECURE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif


/*****************************************************************************/

/* Here's some functions that've been macroized rather than being called
 * from everywhere.  They're short enough so that inlining them will probably
 * result in a smaller executable, and speed things up, to boot. -CWS
 */

#define MY_MAX(a,b) ((a) > (b) ? (a) : (b))
#define MY_MIN(a,b) ((a) < (b) ? (a) : (b))


/* Checks a co-ordinate for in bounds status		-RAK-	*/

#define in_bounds(y, x) \
   ((((y) > 0) && ((y) < cur_height-1) && ((x) > 0) && ((x) < cur_width-1)) ? \
    (TRUE) : (FALSE))


/* Checks if we can see this point (includes map edges) -CWS */
#define in_bounds2(y, x) \
   ((((y) >= 0) && ((y) < cur_height) && ((x) >= 0) && ((x) < cur_width)) ? \
    (TRUE) : (FALSE))


/* Tests a given point to see if it is within the screen -RAK-
 * boundaries.
 */

#define panel_contains(y, x) \
  ((((y) >= panel_row_min) && ((y) <= panel_row_max) && \
    ((x) >= panel_col_min) && ((x) <= panel_col_max)) ? (TRUE) : (FALSE))


/* Generates a random integer X where 1<=X<=MAXVAL	-RAK-	*/

#define randint(maxval) (((maxval) < 1) ? (1) : ((random() % (maxval)) + 1))

/* You would think that most compilers can do an integral abs() quickly,
 * wouldn't you?  Nope.  [But fabs is a lot worse on most machines!] -CWS
 */

#define MY_ABS(x) (((x)<0) ? (-x) : (x))

/*****************************************************************************/
