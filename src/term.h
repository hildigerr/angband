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
 *
 *
 *	- Extra info (used by application)
 *
 *	- Extra data (used by implementation)
 *
 *	- Hook for init-ing the term
 *	- Hook for nuke-ing the term
 *
 *	- Hook for various actions
 *	- Hook for placing a cursor
 *	- Hook for erasing a block of characters
 *	- Hook for drawing a string of characters
 */

typedef struct _term term;

struct _term {

    bool initialized;
    bool unused_flag;

    term_win *old;

    term_win *scr;


    vptr info;

    vptr data;
    
    void (*init_hook)(term *t);
    void (*nuke_hook)(term *t);
    
    errr (*xtra_hook)(int n, int v);
    errr (*curs_hook)(int x, int y, int z);
    errr (*wipe_hook)(int x, int y, int w, int h);
    errr (*text_hook)(int x, int y, int n, byte a, cptr s);
};







/**** Available Constants ****/

/* Definitions for "Term_xtra" */
#define TERM_XTRA_NOISE 21	/* Make a noise */
#define TERM_XTRA_FLUSH 22	/* Flush output */

/* Max recursion depth of "screen memory" */
/* Note that unused screens waste only 32 bytes each */
#define MEM_SIZE 16



/**** Available Macros ****/

/* Access to the char/attr at a given location */
/* This can be used for both access AND assignment */
#define tw_a(W,X,Y) ((W)->a[(W)->w*(Y)+(X)])
#define tw_c(W,X,Y) ((W)->c[(W)->w*(Y)+(X)])



/**** Available Variables ****/

extern term *Term;


/**** Available Functions ****/

extern errr term_win_wipe(term_win*);
extern errr term_win_load(term_win*, term_win*);
extern errr term_win_nuke(term_win*);
extern errr term_win_init(term_win*, int, int);

extern errr Term_xtra(int n, int v);
extern errr Term_curs(int x, int y, int z);
extern errr Term_wipe(int x, int y, int w, int h);
extern errr Term_text(int x, int y, int n, byte a, cptr s);

extern int Term_kbhit(void);
extern errr Term_flush(void);
extern errr Term_fresh(void);
extern errr Term_bell(void);
extern errr Term_save(void);
extern errr Term_load(void);
extern errr Term_gotoxy(int x, int y);
extern errr Term_erase(int x1, int y1, int x2, int y2);
extern errr Term_clear(void);

extern errr Term_activate(term*);

extern errr term_nuke(term*);
extern errr term_init(term*, int w, int h, int k);

#endif


