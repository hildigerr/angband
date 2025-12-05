/* File: term.h */

#ifndef INCLUDED_TERM_H
#define INCLUDED_TERM_H

#include "h-include.h"



/*
 * A term_win is a "window" for a Term
 *
 *  - Window "flags" (unused)
 *
 *	- Window "offset" (unused)
 *
 *	- Window Size (max 256x256)
 *
 *	- Cursor Useless/Visible codes
 *	- Cursor Location (see "Useless")
 *
 *	- Min/Max modified rows (per screen)
 *
 *  - Min/Max modified col (per row)
 *
 *  - Min/Max column with "useful" info (per row)
 *
 *	- Attribute array (see "tw_a()")
 *	- Character array (see "tw_c()")
 *
 * Note that "y1<=y2" iff any changes have occured on the screen.
 * Note that "r1[y]<=r2[y]" iff any changes have occured in row "y".
 * Note that "blanks" written past "rm[y]" in row "y" are ignored.
 */

typedef struct _term_win term_win;

struct _term_win {

    huge flags;

    short x, y;

    byte w, h;

    bool cu, cv;
    byte cx, cy;

    byte y1, y2;

    byte *x1;
    byte *x2;

    byte *a;
    char *c;
};



/*
 * An actual "term" structure
 *
 *	- Have we been activated for the first time
 *	- Unused
 *
 *	- Current screen image
 *
 *	- Desired screen image
 */

typedef struct _term term;

struct _term {

    bool initialized;
    bool unused_flag;

    term_win *old;

    term_win *scr;
};







/**** Available Macros ****/

/* Access to the char/attr at a given location */
/* This can be used for both access AND assignment */
#define tw_a(W,X,Y) ((W)->a[(W)->w*(Y)+(X)])
#define tw_c(W,X,Y) ((W)->c[(W)->w*(Y)+(X)])



/**** Available Variables ****/

extern term *Term;


/**** Available Functions ****/

extern int Term_kbhit(void);
extern errr Term_flush(void);
extern errr Term_fresh(void);
extern errr Term_bell(void);
extern errr Term_gotoxy(int x, int y);
extern errr Term_clear(void);

#endif


