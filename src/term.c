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
 * Completely "erase" a "term_win".  Mark as "changed".
 * Note that the "cursor visibility" is turned off,
 * and the cursor is moved to the top left corner.
 */
errr term_win_wipe(term_win *t)
{
    int y, x;

    /* Cursor to the top left, and invisible */
    t->cv = t->cu = t->cx = t->cy = 0;

    /* Scan every row */
    for (y = 0; y < t->h; y++)
    {
	/* Wipe this row */
	for (x = 0; x < t->w; x++)
	{
	    tw_a(t,x,y) = 0;
	    tw_c(t,x,y) = ' ';
	}

	/* This row has changed */
	t->x1[y] = 0;
	t->x2[y] = t->w - 1;
    }

    /* Every row has changed */
    t->y1 = 0;
    t->y2 = t->h - 1;

    /* Success */
    return (0);
}




/*
 * Load a "term_win()" from another
 */
errr term_win_load(term_win *t, term_win *s)
{
    int y, x;

    int w = MIN(t->w, s->w);
    int h = MIN(t->h, s->h);
    
    /* Copy all the data from "s" into "t" */
    for (y = 0; y < h; y++)
    {
	for (x = 0; x < w; x++)
	{
	    tw_a(t,x,y) = tw_a(s,x,y);
	    tw_c(t,x,y) = tw_c(s,x,y);
	}
    }

    /* Load the "cursor state" */
    t->cx = s->cx;
    t->cy = s->cy;
    t->cu = s->cu;
    t->cv = s->cv;

    /* XXX Hack -- prevent cursor errors */
    if (t->cx > t->w - 1) t->cx = t->w - 1;
    if (t->cy > t->h - 1) t->cy = t->h - 1;

    /* Every row may have changed */
    t->y1 = 0;
    if (t->y2 < h - 1) t->y2 = h - 1;

    /* Every col of every row may have changed */
    for (y = 0; y < h; y++)
    {
	t->x1[y] = 0;
	if (t->x2[y] < w - 1) t->x2[y] = w - 1;
    }

    /* Success */
    return (0);
}


/*
 * Resize a term_win -- Hack -- not very efficient.
 */
static errr term_win_resize(term_win *tw, int w, int h)
{
    term_win hack;
    
    /* Ignore non-changes */
    if ((tw->w == w) && (tw->h == h)) return (1);

    /* Hack -- steal the term_win via structure copy */
    hack = (*tw);

    /* Init ourself to the new size */
    term_win_init(tw, w, h);

    /* Recopy the contents */
    term_win_load(tw, &hack);    
    
    /* Hack -- nuke the copy of our old contents */
    term_win_nuke(&hack);

    return (0);
}



/*
 * Nuke a term_win
 */
errr term_win_nuke(term_win *t)
{
    /* Free the arrays */
    C_KILL(t->a, t->w * t->h, byte);
    C_KILL(t->c, t->w * t->h, char);
    C_KILL(t->x1, t->w, byte);
    C_KILL(t->x2, t->w, byte);
    
    return (0);
}


/*
 * Initialize a "term_win" (using the given screen size)
 */
errr term_win_init(term_win *t, int w, int h)
{
    /* Save the size */
    t->w = w;
    t->h = h;

    /* Allocate the main arrays */
    C_MAKE(t->a, w*h, byte);
    C_MAKE(t->c, w*h, char);

    /* Allocate the change arrays */
    C_MAKE(t->x1, w, byte);
    C_MAKE(t->x2, w, byte);

    /* XXX Allocate the "max used col" array */
    /* C_MAKE(t->rm, w, byte); */

    /* Wipe it */
    term_win_wipe(t);

    /* Success */
    return (0);
}



/*
 * Erase part of the screen (given top left, and size)
 * Note that these changes are NOT flushed immediately
 * This function is only used below.
 */
static void EraseScreen(int ex, int ey, int ew, int eh)
{
    register int x, y;

    /* Drop "black spaces" everywhere */
    int na = 0;
    int nc = ' ';

    /* Paranoia -- nothing selected */
    if (ew <= 0) return;
    if (eh <= 0) return;

    /* Paranoia -- nothing visible */
    if (ex >= Term->scr->w) return;
    if (ey >= Term->scr->h) return;

    /* Force legal location */
    if (ex < 0) ex = 0;
    if (ey < 0) ey = 0;

    /* Force legal size */
    if (ex + ew > Term->scr->w) ew = Term->scr->w - ex;
    if (ey + eh > Term->scr->h) eh = Term->scr->h - ey;

    /* Scan every row */
    for (y = ey; y < ey + eh; y++)
    {
	register int x1 = -1, x2;

	/* Scan every column */
	for (x = ex; x < ex + ew; x++)
	{
	    int oa = tw_a(Term->scr,x,y);
	    int oc = tw_c(Term->scr,x,y);

	    /* Hack -- Ignore "non-changes" */
	    if ((oa == na) && (oc == nc)) continue;

	    /* Save the "literal" information */
	    tw_a(Term->scr,x,y) = na;
	    tw_c(Term->scr,x,y) = nc;

	    /* Hack -- ignore "double blanks" */
	    if (BLANK(na,nc) && BLANK(oa,oc)) continue;

	    /* Note the "range" of screen updates */
	    if (x1 < 0) x1 = x;
	    x2 = x;
	}

	/* Expand the "change area" as needed */
	if (x1 >= 0)
	{
	    /* Check for new min/max row info */
	    if (y < Term->scr->y1) Term->scr->y1 = y;
	    if (y > Term->scr->y2) Term->scr->y2 = y;

	    /* Check for new min/max col info in this row */
	    if (x1 < Term->scr->x1[y]) Term->scr->x1[y] = x1;
	    if (x2 > Term->scr->x2[y]) Term->scr->x2[y] = x2;
	}
    }
}


/*
 * Clear from (x1,y1) to (x2,y2), inclusive, and move to (x1,y1)
 *
 * I do not know how efficient this is expected to be...
 * Nor do I know how efficient the OutputFlush() will be.
 */
errr Term_erase(int x1, int y1, int x2, int y2)
{
    /* We always leave the cursor at the top-left edge */
    Term_gotoxy(x1,y1);

    /* Queue the "erase" for later */
    EraseScreen(x1,y1,1+x2-x1,1+y2-y1);

    /* Success */
    return (0);
}


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
 * Save the current screen contents
 */
errr Term_save(void)
{
#ifdef MACINTOSH
    mac_save_screen();
#else
    overwrite(stdscr, savescr);
#endif

    /* Success */
    return (0);
}


/*
 * Restore screen contents saved above.
 *
 */
errr Term_load(void)
{
#ifdef MACINTOSH
    mac_restore_screen();
#else
    overwrite(savescr, stdscr);
    touchwin(stdscr);
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


/*
 * Place the cursor at a given location
 * Note -- "illegal" requests do not move the cursor.
 */
errr Term_gotoxy(int x, int y)
{
    /* Verify */
    if ((x < 0) || (x >= Term->scr->w)) return (-1);
    if ((y < 0) || (y >= Term->scr->h)) return (-1);

    /* Remember the cursor */    
    Term->scr->cx = x;
    Term->scr->cy = y;

    /* The cursor is not useless */
    Term->scr->cu = 0;

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




/*
 * Activate a new Term (and deactivate the current Term)
 *
 * This function is extremely important, and also somewhat bizarre.
 * It is the only function that should "modify" the value of "Term".
 *
 * To "create" a valid "term", one should do a "term_init()", then set
 * the various flags and hooks, and then do a "Term_activate()".
 */
errr Term_activate(term *t)
{
    /* Already done */
    if (Term == t) return (1);

    /* Hack -- Call the special "init" hook */
    if (!t->initialized) {
	if (t->init_hook) (*t->init_hook)(t);
	t->initialized = TRUE;
    }
    
    /* Remember the Term */
    Term = t;

    /* Success */
    return (0);
}



/*
 * Nuke a term
 */
errr term_nuke(term *t)
{
    /* Hack -- Call the special "nuke" hook */
    if (t->initialized) {
	if (t->nuke_hook) (*t->nuke_hook)(t);
	t->initialized = FALSE;
    }

    /* Free the arrays */
    term_win_nuke(t->old);
    term_win_nuke(t->scr);

    /* Success */
    return (0);
}


/*
 * Initialize a term, using a screen of the given size.
 */
errr term_init(term *t, int w, int h, int k)
{
    /* Hack -- clear the term */
    WIPE(t, term);
    
    /* Initialize the default "physical" screen */
    MAKE(t->old, term_win);
    term_win_init(t->old, w, h);

    /* Initialize the default "current" screen */
    MAKE(t->scr, term_win);
    term_win_init(t->scr, w, h);

    /* Success */
    return (0);
}


