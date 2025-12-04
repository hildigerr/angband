/* File: term.c */

/* Purpose: generic, efficient, modular, terminal routines */

#include "term.h"

#include "z-virt.h"


/*
 * Some ideas may have been adapted from MacAngband 2.6.1
 *
 * Note that there is a rather large difference between the newer
 * "graphic based" machines and the older "text based" terminals.
 * Luckily, the only significant one for a game like "Angband"
 * is that the older machines have a "hardware cursor".
 *
 * There are also a few "tuning" issues, for example, the older
 * machines traditionally have very "expensive" routines to check
 * for a keypress without actually "waiting" for it.  So these
 * routines must be explicitly "requested".
 *
 * New machines:
 *   movement" across the screen is relatively cheap
 *   changing "colors" may take a little bit of time
 *   characters must be "erased" from the screen
 *   drawing characters may or may not first erase behind them
 *   drawing a character on the cursor will clear the cursor
 *
 * Old machines:
 *   expensive cursor movement
 *   hardware cursor (moves at every "print")
 *   printing characters erases the previous contents
 *   may have "fast" routines for "clear to end of line"
 *   may have "fast" routines for "clear entire screen"
 */


/*
 * Make an "alert sound" on the Term
 */
errr Term_bell()
{
#ifdef MACINTOSH
    mac_beep();
#else
    (void)write(1, "\007", 1);
#endif

    /* Success */
    return (0);
}



/*** Input routines ***/


/*
 * Flush and forget the input
 */
errr Term_flush()
{
#ifdef MACINTOSH
    FlushScreenKeys();
#else
  #ifdef MSDOS
    while (kbhit()) (void)getch();
  #else
  /* the code originally used ioctls, TIOCDRAIN, or TIOCGETP/TIOCSETP, or
   * TCGETA/TCSETAF, however this occasionally resulted in loss of output, the
   * happened especially often when rlogin from BSD to SYS_V machine, using
   * check_input makes the desired effect a bit clearer
   *
   * wierd things happen on EOF, don't try to flush input in that case */
    if (!eof_flag) while (check_input(0));
  #endif
#endif
    /* Success */
    return (0);
}



/*
 * Place the cursor at a given location
 */
errr Term_gotoxy(int x, int y)
{
#ifdef MACINTOSH
    DSetScreenCursor(col, row);
#else
    (void)move(row, col);
#endif
}


