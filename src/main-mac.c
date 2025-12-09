/* File: main-mac.c */

/* Purpose: Support for MACINTOSH Angband */

/*
 * Adapted from "MacAngband 2.6.1" by Keith Randall
 *
 * See "term.c" for info on the "generic terminal" that we support.
 *
 * See "recall.c"/"moria1.c" for info on the "recall"/"choice" windows.
 *
 * Note that this file is the only one that requires "Non-ANSI"
 * extensions to C, in particular, the "Think C Language Extensions"
 * must be on, and the "enums are always ints" must be off.  Both of
 * these are to allow the "Events.h" header file to be parsed.
 */

#include "angband.h"


#ifdef MACINTOSH

#include <OSUtils.h>
#include <GestaltEqu.h>
#include <Palettes.h>
#include <Files.h>
#include <Fonts.h>
#include <Menus.h>
#include <Dialogs.h>
#include <StandardFile.h>
#include <ToolUtils.h>
#include <Desk.h>
#include <Resources.h>
#include <QuickDraw.h>
#include <Controls.h>



/*** Main program ***/


/*
 * Macintosh Main loop
 */
void main(void)
{
    /* Mac-Mega-Hack -- about 200K of extra stack */
    SetApplLimit(GetApplLimit()-200000L);
    MaxApplZone();

    /* Set up the Macintosh */
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    InitDialogs(0);
    InitCursor();


    /* Mark ourself as the file creator */
    _fcreator = 'A270';

    /* Default to saving a "text" file */
    _ftype = 'TEXT';
}






#endif /* MACINTOSH */

