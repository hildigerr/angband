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
 *
 * Note that we can easily allow the "term_win" structure to be
 * exported, allowing users to create various "user defined"
 * routines, such as "scrolling packages".  These routines would
 * not tie in the the "terminal capabilities", but they would
 * be completely portable...
 */



/*
 * A macro to determine if an attr/char pair looks "blank"
 * Note that the "black" attribute is "reserved", and that
 * currently, the "space" character is hard-coded as "blank".
 */
#define BLANK(A,C)	(((A)==0) || ((C)==' '))



/* The current "term" */
term *Term = NULL;




/*
 * Clear the entire screen
 */
errr Term_clear()
{
#ifdef MACINTOSH
    Rect area;

    area.left = area.top = 0;
    area.right = SCRN_COLS;
    area.bottom = SCRN_ROWS;
    DEraseScreen(&area);
#else
    touchwin(stdscr);
    (void)clear();
    refresh();
#endif

    /* Success */
    return (0);
}




/*
 * Flush the output
 */
errr Term_fresh()
{
    screen_change = TRUE;	   /* Let inven_command know something has changed. */
    (void)refresh();

    /* Success */
    return (0);
}


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




/*
 * Return the pending keypress, if any, or zero.
 */
int Term_kbhit()
{
    int i;

#if (defined (unix) || defined(ATARI_ST)) /* CFT's if/elif/else    */
    i = check_input(1);
#elif (defined(MSDOS) || defined(VMS)) /* stolen from Um55 src -CFT */
    i = kbhit();
#endif

    /* A key is ready */
    return (i);
}


