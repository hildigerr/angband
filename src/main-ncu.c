/* File: main-ncu.c */

/* Purpose: Actual Unix "ncurses" support for Angband */
/* Author: wiebelt@mathematik.hu-berlin.de (Bernd "Bernardo" Wiebelt) */

#define bool bool_hack
#include "angband.h"
#undef bool


/*
 * This module may very well work with modern implementations of
 * normal "terminfo based curses" as well.  Let us know...
 */

#ifdef USE_NCU



/*
 * OPTION: You may have to use "#include <ncurses.h>" instead
 */
#include <curses.h>  



/*
 * Are we "active"?  Currently unused.
 */
static int ncurses_on = FALSE;


/*
 * Currently, only a single "term" is supported here
 */
static term term_screen_body;


/*
 * Nuke NCurses
 */
static void Term_nuke_ncu(term *t)
{
    if (!ncurses_on) return;
        
    /* Clear the screen */
    touchwin(stdscr);
    (void)clear();

    /* Refresh */
    refresh();
    
    /* We may want to "undo" the following things */
    /* cbreak(); noecho(); nonl(); nodelay(stdscr, TRUE); */

    /* We are now off */
    ncurses_on = FALSE;

    /* Shut down */
    endwin();
}


/*
 * Init NCurses
 */
static void Term_init_ncu(term *t)
{
    /* Prepare to be interactive */
    cbreak(); noecho(); nonl(); nodelay(stdscr, TRUE);

    /* Note that we are on */
    ncurses_on = TRUE;
}


/*
 * Prepare "ncurses" for use by the file "term.c"
 */
errr init_ncu(void)
{
    int err;
    
    term *t = &term_screen_body;


    /* Initialize, check for errors */
    err = (initscr() == NULL);

    /* Quit on error */
    if (err) quit("ncurses initialization failed");

    /* Check we have enough screen. */
    err = ((LINES < 24) || (COLS < 80));

    /* Quit with message */
    if (err) quit("ncurses screen must be at least 80x24");


    /* Initialize the term */
    term_init(t, 80, 24, 64);

    /* Stick in some hooks */
    t->nuke_hook = Term_nuke_ncu;
    t->init_hook = Term_init_ncu;

    /* Extra data -- unused */
    /* t->data = NULL; */

    /* Save the term */
    term_screen = t;
    
    /* Activate it */
    Term_activate(t);


    /* Success */
    return (0);
}

#endif /* USE_NCU */

