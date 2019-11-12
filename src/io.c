/*
 * io.c: terminal I/O code, uses the curses package 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#ifdef linux
#include <bsd/sgtty.h>
#endif

#ifdef MSDOS
#include <stdio.h>
#include <process.h>
#endif

#if defined(NLS) && defined(lint)
/* for AIX, don't let curses include the NL stuff */
#undef NLS
#endif

#if !defined(GEMDOS)

#ifdef MAC
#include <scrnmgr.h>
#else
#ifdef linux
#include <ncurses.h>
#else
#include <curses.h>
#endif
#endif

#else
#define ATARIST_MWC
#include "curses.h"
#include <osbind.h>
long                wgetch();
char               *getenv();
#endif

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#include <ctype.h>

#if defined(SYS_V) && defined(lint)
/*
 * for AIX, prevent hundreds of unnecessary lint errors, must define before
 * signal.h is included 
 */
#define _h_IEEETRAP
typedef struct {
    int stuff;
} fpvmach;

#endif

#if defined(MSDOS)
#if defined(ANSI)
#include "ms_ansi.h"
#endif
#else				   /* not msdos */
#if !defined(ATARIST_MWC) && !defined(MAC)
#ifndef VMS
#include <sys/ioctl.h>
#endif
#include <signal.h>
#endif
#endif

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/param.h>
#include <sys/file.h>
#include <sys/types.h>
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#else
#include "string.h"
#endif /* !ATARIST_MWC */
#if !defined(MAC) && !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
#include <termio.h>
#endif
#else
#ifndef VMS
#include <strings.h>
#include <sys/wait.h>
#endif /* !VMS */
#endif /* USG */

/* ARGH!  This is driving me up the wall!  Brute force never hurt... [cjh] */
#if defined(__MINT__) && !defined(_WAIT_H)
#include <wait.h>
#endif

#if defined(SYS_V) && defined(lint)
struct screen {
    int                 dumb;
};

#endif

/*
 * Fooling lint. Unfortunately, c defines all the TIO.	  -CJS- constants to
 * be long, and lint expects them to be int. Also, ioctl is sometimes called
 * with just two arguments. The following definition keeps lint happy. It may
 * need to be reset for different systems.	 
 */
#ifndef MAC
#ifdef lint
#ifdef Pyramid
/* Pyramid makes constants greater than 65535 into long! Gakk! -CJS- */
/* ARGSUSED */
/* VARARGS2 */
static 
    Ioctl(i, l, p) long l;
    char               *p;
{
    return 0;
}

#else
/* ARGSUSED */
/* VARARGS2 */
static 
    Ioctl(i, l, p) char *p;
{
    return 0;
}

#endif
#define ioctl	    Ioctl
#endif

#if !defined(USG) && defined(lint)
/*
 * This use_value hack is for curses macros which return a value, but don't
 * shut up about it when you try to tell them (void).	 
 */
/* only needed for Berkeley UNIX */
int                 Use_value;

#define use_value   Use_value +=
#else
#define use_value
#endif

#if defined(SYS_V) && defined(lint)
/*
 * This use_value2 hack is for curses macros which use a conditional
 * expression, and which say null effect even if you cast to (void). 
 */
/* only needed for SYS V */
int                 Use_value2;

#define use_value2  Use_value2 +=
#else
#define use_value2
#endif

#endif

#ifndef MAC
char *getenv();

#endif

#if !defined(MAC) && !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
#ifdef USG
static struct termio save_termio;

#else
#ifndef VMS
static struct ltchars save_special_chars;
static struct sgttyb save_ttyb;
static struct tchars save_tchars;
static int          save_local_chars;

#endif
#endif
#endif

#ifndef MAC
static int     curses_on = FALSE;
static WINDOW *savescr;	   /* Spare window for saving the screen.
				    * -CJS- */

#endif

#ifdef MAC
/* Attributes of normal and hilighted characters */
#define ATTR_NORMAL	attrNormal
#define ATTR_HILITED	attrReversed
#endif

#ifndef MAC
#ifdef SIGTSTP
/*
 * suspend()							   -CJS-
 * Handle the stop and start signals. This ensures that the log is up to
 * date, and that the terminal is fully reset and restored.  
 */
int 
suspend()
{
#ifdef USG
/*
 * for USG systems with BSDisms that have SIGTSTP defined, but don't actually
 * implement it 
 */
#else
    struct sgttyb  tbuf;
    struct ltchars lcbuf;
    struct tchars  cbuf;
    int            lbuf;
    long           time();

    py.misc.male |= 2;
    (void)ioctl(0, TIOCGETP, (char *)&tbuf);
    (void)ioctl(0, TIOCGETC, (char *)&cbuf);
    (void)ioctl(0, TIOCGLTC, (char *)&lcbuf);
    (void)ioctl(0, TIOCLGET, (char *)&lbuf);
    restore_term();
    (void)kill(0, SIGSTOP);
    curses_on = TRUE;
    (void)ioctl(0, TIOCSETP, (char *)&tbuf);
    (void)ioctl(0, TIOCSETC, (char *)&cbuf);
    (void)ioctl(0, TIOCSLTC, (char *)&lcbuf);
    (void)ioctl(0, TIOCLSET, (char *)&lbuf);
    (void)touchwin(curscr);
    (void)wrefresh(curscr);
    cbreak();
    noecho();
    py.misc.male &= ~2;
#endif
    return 0;
}

#endif
#endif

/* initializes curses routines */
void 
init_curses()
#ifdef MAC
{
/* Primary initialization is done in mac.c since game is restartable */
/* Only need to clear the screen here */
    Rect scrn;

    scrn.left = scrn.top = 0;
    scrn.right = SCRN_COLS;
    scrn.bottom = SCRN_ROWS;
    EraseScreen(&scrn);
    UpdateScreen();
}

#else
{
    int i, y, x;

#ifndef USG
    (void)ioctl(0, TIOCGLTC, (char *)&save_special_chars);
    (void)ioctl(0, TIOCGETP, (char *)&save_ttyb);
    (void)ioctl(0, TIOCGETC, (char *)&save_tchars);
    (void)ioctl(0, TIOCLGET, (char *)&save_local_chars);
#else
#if !defined(VMS) && !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
    (void)ioctl(0, TCGETA, (char *)&save_termio);
#endif
#endif

#ifdef ATARIST_MWC
    WINDOW *newwin();

    initscr();
    if (ERR)
#else
#if defined(USG) && !defined(PC_CURSES)	/* PC curses returns ERR */
    if (initscr() == NULL)
#else
    if (initscr() == ERR)
#endif
#endif
    {
	(void)printf("Error allocating screen in curses package.\n");
	exit(1);
    }
    if (LINES < 24 || COLS < 80) { /* Check we have enough screen. -CJS- */
	(void)printf(
	  "Your screen is too small for Angband; you need at least 80x24.\n");
	exit(1);
    }
#ifdef SIGTSTP
#ifdef __MINT__
    (void)signal(SIGTSTP, (__Sigfunc)suspend);
#else
    (void)signal(SIGTSTP, suspend);
#endif
#endif

    if ((savescr = newwin(0, 0, 0, 0)) == NULL) {
	(void)printf("Out of memory in starting up curses.\n");
	exit_game();
    }
    (void)clear();
    (void)refresh();
    moriaterm();

/* check tab settings, exit with error if they are not 8 spaces apart */
#ifdef ATARIST_MWC
    move(0, 0);
#else
    (void)move(0, 0);
#endif
    for (i = 1; i < 10; i++) {
#ifdef ATARIST_MWC
	addch('\t');
#else
	(void)addch('\t');
#endif
	getyx(stdscr, y, x);
	if (y != 0 || x != i * 8)
	    break;
    }
    if (i != 10) {
	msg_print("Tabs must be set 8 spaces apart.");
	exit_game();
    }
}

#endif

/* Set up the terminal into a suitable state for moria.	 -CJS- */
void 
moriaterm()
#ifdef MAC
/* Nothing to do on Mac */
{
}

#else
{
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
#ifdef USG
    struct termio  tbuf;

#else
    struct ltchars lbuf;
    struct tchars  buf;

#endif
#endif

    curses_on = TRUE;
#ifndef BSD4_3
    use_value crmode();

#else
    use_value cbreak();

#endif
    use_value noecho();

/* can not use nonl(), because some curses do not handle it correctly */
#ifdef MSDOS
    msdos_raw();
#else
#if !defined(ATARIST_MWC) && !defined(__MINT__)
#ifdef USG
    (void)ioctl(0, TCGETA, (char *)&tbuf);
/* disable all of the normal special control characters */
    tbuf.c_cc[VINTR] = (char)3;	   /* control-C */
    tbuf.c_cc[VQUIT] = (char)-1;
    tbuf.c_cc[VERASE] = (char)-1;
    tbuf.c_cc[VKILL] = (char)-1;
    tbuf.c_cc[VEOF] = (char)-1;

/* don't know what these are for */
    tbuf.c_cc[VEOL] = (char)-1;
    tbuf.c_cc[VEOL2] = (char)-1;

/* stuff needed when !icanon, i.e. cbreak/raw mode */
    tbuf.c_cc[VMIN] = 1;	   /* Input should wait for at least 1 char */
    tbuf.c_cc[VTIME] = 0;	   /* no matter how long that takes. */

    (void)ioctl(0, TCSETA, (char *)&tbuf);
#else
#ifndef VMS
/*
 * disable all of the special characters except the suspend char, interrupt
 * char, and the control flow start/stop characters 
 */
    (void)ioctl(0, TIOCGLTC, (char *)&lbuf);
    lbuf.t_suspc = (char)26;	   /* control-Z */
    lbuf.t_dsuspc = (char)-1;
    lbuf.t_rprntc = (char)-1;
    lbuf.t_flushc = (char)-1;
    lbuf.t_werasc = (char)-1;
    lbuf.t_lnextc = (char)-1;
    (void)ioctl(0, TIOCSLTC, (char *)&lbuf);

    (void)ioctl(0, TIOCGETC, (char *)&buf);
    buf.t_intrc = (char)3;	   /* control-C */
    buf.t_quitc = (char)-1;
    buf.t_startc = (char)17;	   /* control-Q */
    buf.t_stopc = (char)19;	   /* control-S */
    buf.t_eofc = (char)-1;
    buf.t_brkc = (char)-1;
    (void)ioctl(0, TIOCSETC, (char *)&buf);
#endif
#endif
#endif
#endif
}

#endif


/* Dump IO to buffer					-RAK-	 */
void 
put_str(out_str, row, col)
const char *out_str;
int         row, col;

#ifdef MAC
{
/* The screen manager handles writes past the edge ok */
    DSetScreenCursor(col, row);
    DWriteScreenStringAttr(out_str, ATTR_NORMAL);
}

#else
{
    vtype tmp_str;

/*
 * truncate the string, to make sure that it won't go past right edge of
 * screen 
 */
    if (col > 79)
	col = 79;
    (void)strncpy(tmp_str, out_str, 79 - col);
    tmp_str[79 - col] = '\0';

#ifndef ATARIST_MWC
    if (mvaddstr(row, col, tmp_str) == ERR)
#else
    mvaddstr(row, col, out_str);
    if (ERR)
#endif
    {
	abort();
    /* clear msg_flag to avoid problems with unflushed messages */
	msg_flag = 0;
	(void)sprintf(tmp_str, "error in put_str, row = %d col = %d\n",
		      row, col);
	prt(tmp_str, 0, 0);
	bell();
    /* wait so user can see error */
	(void)sleep(2);
    }
}

#endif


/* Dump the IO buffer to terminal			-RAK-	 */
void 
put_qio()
{
    screen_change = TRUE;	   /* Let inven_command know something has
				    * changed. */
    (void)refresh();
}

/* Put the terminal in the original mode.			   -CJS- */
void 
restore_term()
#ifdef MAC
/* Nothing to do on Mac */
{
}

#else
{
    if (!curses_on)
	return;
    put_qio();			   /* Dump any remaining buffer */
#ifdef MSDOS
    (void)sleep(2);		   /* And let it be read. */
#endif
#ifdef VMS
    pause_line(15);
#endif
/* this moves curses to bottom right corner */
    mvcur(curscr->_cury, curscr->_curx, LINES - 1, 0);
#ifdef VMS
    pause_line(15);
#endif
    endwin();			   /* exit curses */
    (void)fflush(stdout);
#ifdef MSDOS
    msdos_noraw();
    (void)clear();
#endif
/* restore the saved values of the special chars */
#ifdef USG
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
    (void)ioctl(0, TCSETA, (char *)&save_termio);
#endif
#else
#ifndef VMS
    (void)ioctl(0, TIOCSLTC, (char *)&save_special_chars);
    (void)ioctl(0, TIOCSETP, (char *)&save_ttyb);
    (void)ioctl(0, TIOCSETC, (char *)&save_tchars);
    (void)ioctl(0, TIOCLSET, (char *)&save_local_chars);
#endif
#endif
    curses_on = FALSE;
}

#endif


void 
shell_out()
#ifdef MAC
{
    alert_error("This command is not implemented on the Macintosh.");
}

#else
{
#ifdef USG
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
    struct termio       tbuf;

#endif
#else
    struct sgttyb       tbuf;
    struct ltchars      lcbuf;
    struct tchars       cbuf;
    int                 lbuf;

#endif
#ifdef MSDOS
    char               *comspec, key;

#else
#ifdef ATARIST_MWC
    char                comstr[80];
    char               *str;
    extern char       **environ;

#else
    int                 val;
    char               *str;

#endif
#endif

    save_screen();
/* clear screen and print 'exit' message */
    clear_screen();
#ifndef ATARIST_MWC
    put_str("[Entering shell, type 'exit' to resume your game.]\n", 0, 0);
#else
    put_str("[Escaping to shell]\n", 0, 0);
#endif
    put_qio();

#ifdef USG
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
    (void)ioctl(0, TCGETA, (char *)&tbuf);
#endif
#else
#ifndef VMS
    (void)ioctl(0, TIOCGETP, (char *)&tbuf);
    (void)ioctl(0, TIOCGETC, (char *)&cbuf);
    (void)ioctl(0, TIOCGLTC, (char *)&lcbuf);
    (void)ioctl(0, TIOCLGET, (char *)&lbuf);
#endif
#endif
/* would call nl() here if could use nl()/nonl(), see moriaterm() */
#ifndef BSD4_3
    use_value           nocrmode();

#else
    use_value           nocbreak();

#endif
#ifdef MSDOS
    use_value           msdos_noraw();

#endif
    use_value           echo();

    ignore_signals();
#ifdef MSDOS			   /* { */
    if ((comspec = getenv("COMSPEC")) == NULL
	|| spawnl(P_WAIT, comspec, comspec, (char *)NULL) < 0) {
	clear_screen();		   /* BOSS key if shell failed */
	put_str("M:\\> ", 0, 0);
	do {
	    key = inkey();
	} while (key != '!');
    }
#else				   /* MSDOS }{ */
#ifndef ATARIST_MWC
    val = fork();
    if (val == 0) {
#endif
	default_signals();
#ifdef USG
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
	(void)ioctl(0, TCSETA, (char *)&save_termio);
#endif
#else
#ifndef VMS
	(void)ioctl(0, TIOCSLTC, (char *)&save_special_chars);
	(void)ioctl(0, TIOCSETP, (char *)&save_ttyb);
	(void)ioctl(0, TIOCSETC, (char *)&save_tchars);
	(void)ioctl(0, TIOCLSET, (char *)&save_local_chars);
#endif
#endif
#ifndef MSDOS
    /* close scoreboard descriptor */
    /* it is not open on MSDOS machines */
#if 0
    /* this file is not open now, see init_file() in files.c */
	(void)close(highscore_fd);
#endif
#endif
	if ((str = getenv("SHELL")))
#ifndef ATARIST_MWC
	    (void)execl(str, str, (char *)0);
#else
	    system(str);
#endif
	else
#ifndef ATARIST_MWC
	    (void)execl("/bin/sh", "sh", (char *)0);
#endif
	msg_print("Cannot execute shell.");
#ifndef ATARIST_MWC
	exit(1);
    }
    if (val == -1) {
	msg_print("Fork failed. Try again.");
	return;
    }
#ifdef USG
    (void)wait((int *)0);
#else
    (void)wait((union wait *) 0);
#endif
#endif				   /* ATARIST_MWC */
#endif				   /* MSDOS } */
    restore_signals();
/* restore the cave to the screen */
    restore_screen();
#ifndef BSD4_3
    use_value           crmode();

#else
    use_value           cbreak();

#endif
    use_value           noecho();

/* would call nonl() here if could use nl()/nonl(), see moriaterm() */
#ifdef MSDOS
    msdos_raw();
#endif
/* disable all of the local special characters except the suspend char */
/* have to disable ^Y for tunneling */
#ifdef USG
#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
    (void)ioctl(0, TCSETA, (char *)&tbuf);
#endif
#else
#ifndef VMS
    (void)ioctl(0, TIOCSLTC, (char *)&lcbuf);
    (void)ioctl(0, TIOCSETP, (char *)&tbuf);
    (void)ioctl(0, TIOCSETC, (char *)&cbuf);
    (void)ioctl(0, TIOCLSET, (char *)&lbuf);
#endif
#endif
    (void)wrefresh(curscr);
}

#endif


/*
 * Returns a single character input from the terminal.	This silently -CJS-
 * consumes ^R to redraw the screen and reset the terminal, so that this
 * operation can always be performed at any input prompt.  inkey() never
 * returns ^R.	 
 */
char 
inkey()
#ifdef MAC
/* The Mac does not need ^R, so it just consumes it */
/* This routine does nothing special with direction keys */
/* Just returns their keypad ascii value (e.g. '0'-'9') */
/* Compare with inkeydir() below */
{
    char ch;
    int  dir;
    int  shift_flag, ctrl_flag;

    put_qio();
    command_count = 0;

    do {
	macgetkey(&ch, FALSE);
    } while (ch == CTRL('R'));

    dir = extractdir(ch, &shift_flag, &ctrl_flag);
    if (dir != -1)
	ch = '0' + dir;

    return (ch);
}

#else
{
    int i;

    put_qio();			   /* Dump IO buffer		 */
    command_count = 0;		   /* Just to be safe -CJS- */
    while (TRUE) {
#ifdef MSDOS
	i = msdos_getch();
#else
	i = getch();
#endif

    /* some machines may not sign extend. */
	if (i == EOF) {
	    eof_flag++;
	/*
	 * avoid infinite loops while trying to call inkey() for a -more-
	 * prompt. 
	 */
	    msg_flag = FALSE;

	    (void)refresh();
	    if (!character_generated || character_saved)
		exit_game();
	    disturb(1, 0);
	    if (eof_flag > 100) {
	    /* just in case, to make sure that the process eventually dies */
		panic_save = 1;
		(void)strcpy(died_from, "(end of input: panic saved)");
		if (!save_player()) {
		    (void)strcpy(died_from, "panic: unexpected eof");
		    death = TRUE;
		}
		exit_game();
	    }
	    return ESCAPE;
	}
	if (i != CTRL('R'))
	    return (char)i;
	(void)wrefresh(curscr);
	moriaterm();
    }
}

#endif


#ifdef MAC
char 
inkeydir()
/* The Mac does not need ^R, so it just consumes it */
/* This routine translates the direction keys in rogue-like mode */
/* Compare with inkeydir() below */
{
    char        ch;
    int         dir;
    int         shift_flag, ctrl_flag;
    static char tab[9] = {
	'b', 'j', 'n',
	'h', '.', 'l',
	'y', 'k', 'u'
    };
    static char shifttab[9] = {
	'B', 'J', 'N',
	'H', '.', 'L',
	'Y', 'K', 'U'
    };
    static char ctrltab[9] = {
	CTRL('B'), CTRL('J'), CTRL('N'),
	CTRL('H'), '.', CTRL('L'),
	CTRL('Y'), CTRL('K'), CTRL('U')
    };

    put_qio();
    command_count = 0;

    do {
	macgetkey(&ch, FALSE);
    } while (ch == CTRL('R'));

    dir = extractdir(ch, &shift_flag, &ctrl_flag);

    if (dir != -1) {
	if (!rogue_like_commands) {
	    ch = '0' + dir;
	} else {
	    if (ctrl_flag)
		ch = ctrltab[dir - 1];
	    else if (shift_flag)
		ch = shifttab[dir - 1];
	    else
		ch = tab[dir - 1];
	}
    }
    return (ch);
}

#endif


/* Flush the buffer					-RAK-	 */
void 
flush()
#ifdef MAC
{
/* Removed put_qio() call.  Reduces flashing.  Doesn't seem to hurt. */
    FlushScreenKeys();
}

#else
{
#ifdef MSDOS
    while (kbhit())
	(void)getch();
#else
/*
 * the code originally used ioctls, TIOCDRAIN, or TIOCGETP/TIOCSETP, or
 * TCGETA/TCSETAF, however this occasionally resulted in loss of output, the
 * happened especially often when rlogin from BSD to SYS_V machine, using
 * check_input makes the desired effect a bit clearer 
 */
/* wierd things happen on EOF, don't try to flush input in that case */
    if (!eof_flag)
	while (check_input(0));
#endif

/* used to call put_qio() here to drain output, but it is not necessary */
}

#endif


/* Clears given line of text				-RAK-	 */
void 
erase_line(row, col)
int row, col;

#ifdef MAC
{
    Rect line;

    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);

    line.left = col;
    line.top = row;
    line.right = SCRN_COLS;
    line.bottom = row + 1;
    DEraseScreen(&line);
}

#else
{
    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);
    (void)move(row, col);
    clrtoeol();
}

#endif


/* Clears screen */
void 
clear_screen()
#ifdef MAC
{
    Rect area;

    if (msg_flag)
	msg_print(NULL);

    area.left = area.top = 0;
    area.right = SCRN_COLS;
    area.bottom = SCRN_ROWS;
    DEraseScreen(&area);
}

#else
{
    if (msg_flag)
	msg_print(NULL);
    touchwin(stdscr);
    (void)clear();
    refresh();
}

#endif

void 
clear_from(row)
int row;

#ifdef MAC
{
    Rect area;

    area.left = 0;
    area.top = row;
    area.right = SCRN_COLS;
    area.bottom = SCRN_ROWS;
    DEraseScreen(&area);
}

#else
{
    (void)move(row, 0);
    clrtobot();
}

#endif

/* Outputs a char to a given interpolated y, x position	-RAK-	 */
/* sign bit of a character used to indicate standout mode. -CJS */
void 
print(ch, row, col)
int ch, row, col;
{
    row -= panel_row_prt;	   /* Real co-ords convert to screen positions */
    col -= panel_col_prt;
#if 0
    if (ch & 0x80)
	standout();
#endif
    if (mvaddch(row, col, ch) == ERR) {
	vtype               tmp_str;

    /* clear msg_flag to avoid problems with unflushed messages */
	msg_flag = 0;
	(void)sprintf(tmp_str, "error in print, row = %d col = %d\n", row, col);
	prt(tmp_str, 0, 0);
	bell();
    /* wait so user can see error */
	(void)sleep(2);
	abort();
    }
#if 0
    if (ch & 0x80)
	standend();
#endif
}


/* Moves the cursor to a given interpolated y, x position	-RAK-	 */
void 
move_cursor_relative(row, col)
int row, col;

#ifdef MAC
{
    row -= panel_row_prt;	   /* Real co-ords convert to screen
				    * positions */
    col -= panel_col_prt;

    DSetScreenCursor(col, row);
}

#else
{
    vtype tmp_str;

    row -= panel_row_prt;	   /* Real co-ords convert to screen
				    * positions */
    col -= panel_col_prt;
    if (move(row, col) == ERR) {
	abort();
    /* clear msg_flag to avoid problems with unflushed messages */
	msg_flag = 0;
	(void)sprintf(tmp_str,
		      "error in move_cursor_relative, row = %d col = %d\n",
		      row, col);
	prt(tmp_str, 0, 0);
	bell();
    /* wait so user can see error */
	(void)sleep(2);
    }
}

#endif


/* Print a message so as not to interrupt a counted command. -CJS- */
void 
count_msg_print(p)
const char *p;
{
    int i;

    i = command_count;
    msg_print(p);
    command_count = i;
}


/* Outputs a line to a given y, x position		-RAK-	 */
void 
prt(str_buff, row, col)
const char *str_buff;
int         row, col;

#ifdef MAC
{
    Rect line;

    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);

    line.left = col;
    line.top = row;
    line.right = SCRN_COLS;
    line.bottom = row + 1;
    DEraseScreen(&line);

    put_str(str_buff, row, col);
}

#else
{
    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);
    (void)move(row, col);
    clrtoeol();
    put_str(str_buff, row, col);
}

#endif


/* move cursor to a given y, x position */
void 
move_cursor(row, col)
int row, col;

#ifdef MAC
{
    DSetScreenCursor(col, row);
}

#else
{
    (void)move(row, col);
}

#endif


/* Outputs message to top line of screen				 */
/* These messages are kept for later reference.	 */
void 
msg_print(str_buff)
const char *str_buff;
{
    char   in_char;
    static len = 0;

    if (msg_flag) {
	if (str_buff && (len + strlen(str_buff)) > 72) {
	/* ensure that the complete -more- message is visible. */
	    if (len > 73)
		len = 73;
	    put_str(" -more-", MSG_LINE, len);
	/* let sigint handler know that we are waiting for a space */
	    wait_for_more = 1;
	    do {
		in_char = inkey();
	    } while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') &&
		     (in_char != '\r') && (!quick_messages));
	    len = 0;
	    wait_for_more = 0;
	    (void)move(MSG_LINE, 0);
	    clrtoeol();

	/* Make the null string a special case.  -CJS- */
	    if (str_buff) {
		put_str(str_buff, MSG_LINE, 0);
		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';
		len = strlen(str_buff) + 1;
		msg_flag = TRUE;
	    } else {
		len = 0;
		msg_flag = FALSE;
	    }
	} else {
	    if (!str_buff) {
		if (len > 73)
		    len = 73;
		put_str(" -more-", MSG_LINE, len);
	    /* let sigint handler know that we are waiting for a space */
		wait_for_more = 1;
		do {
		    in_char = inkey();
		} while ((in_char != ' ') && (in_char != ESCAPE)
			 && (in_char != '\n') && (in_char != '\r') && (!quick_messages));
		wait_for_more = 0;
		len = 0;
		(void)move(MSG_LINE, 0);
		clrtoeol();
		msg_flag = FALSE;
	    } else {
		put_str(str_buff, MSG_LINE, len);
		len += strlen(str_buff) + 1;
		command_count = 0;
		last_msg++;
		if (last_msg >= MAX_SAVE_MSG)
		    last_msg = 0;
		(void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
		old_msg[last_msg][VTYPESIZ - 1] = '\0';
		msg_flag = TRUE;
	    }
	}
    } else {
	(void)move(MSG_LINE, 0);
	clrtoeol();
	if (str_buff) {
	    put_str(str_buff, MSG_LINE, 0);
	    command_count = 0;
	    len = strlen(str_buff) + 1;
	    last_msg++;
	    if (last_msg >= MAX_SAVE_MSG)
		last_msg = 0;
	    (void)strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
	    old_msg[last_msg][VTYPESIZ - 1] = '\0';
	    msg_flag = TRUE;
	} else {
	    msg_flag = FALSE;
	    len = 0;
	}
    }
}

/* Used to verify a choice - user gets the chance to abort choice.  -CJS- */
int 
get_check(prompt)
const char *prompt;
{
    int res, y, x;

    prt(prompt, 0, 0);
#ifdef MAC
    GetScreenCursor(&x, &y);
#else
    getyx(stdscr, y, x);
#if defined(lint)
/* prevent message 'warning: y is unused' */
    x = y;
#endif
    res = y;
#endif

    if (x > 73)
#ifdef ATARIST_MWC
	move(0, 73);
#else
	(void)move(0, 73);
#endif
#ifdef MAC
    DWriteScreenStringAttr(" [y/n]", ATTR_NORMAL);
#else
    (void)addstr(" [y/n]");
#endif
    do {
	res = inkey();
    }
    while (res == ' ');
    erase_line(0, 0);
    if (res == 'Y' || res == 'y')
	return TRUE;
    else
	return FALSE;
}

/* Prompts (optional) and returns ord value of input char	 */
/* Function returns false if <ESCAPE> is input	 */
int 
get_com(prompt, command)
const char *prompt;
char       *command;
{
    int res;

    if (prompt)
	prt(prompt, 0, 0);
    *command = inkey();
    if (*command == 0 || *command == ESCAPE)
	res = FALSE;
    else
	res = TRUE;
    erase_line(MSG_LINE, 0);
    return (res);
}

#ifdef MAC
/* Same as get_com(), but translates direction keys from keypad */
int 
get_comdir(prompt, command)
char *prompt;
char *command;
{
    int res;

    if (prompt)
	prt(prompt, 0, 0);
    *command = inkeydir();
    if (*command == 0 || *command == ESCAPE)
	res = FALSE;
    else
	res = TRUE;
    erase_line(MSG_LINE, 0);
    return (res);
}

#endif


/* Gets a string terminated by <RETURN>		 */
/* Function returns false if <ESCAPE> is input	 */
int 
get_string(in_str, row, column, slen)
char *in_str;
int   row, column, slen;
{
    register int start_col, end_col, i;
    char        *p;
    int          flag, aborted;

    aborted = FALSE;
    flag = FALSE;
    (void)move(row, column);
    for (i = slen; i > 0; i--)
	(void)addch(' ');
    (void)move(row, column);
    start_col = column;
    end_col = column + slen - 1;
    if (end_col > 79) {
	slen = 80 - column;
	end_col = 79;
    }
    p = in_str;
    do {
	i = inkey();
	switch (i) {
	  case ESCAPE:
	    aborted = TRUE;
	    break;
	  case CTRL('J'):
	  case CTRL('M'):
	    flag = TRUE;
	    break;
	  case DELETE:
	  case CTRL('H'):
	    if (column > start_col) {
		column--;
		put_str(" ", row, column);
		move_cursor(row, column);
		*--p = '\0';
	    }
	    break;
	  default:
	    if (!isprint(i) || column > end_col)
		bell();
	    else {
#ifdef MAC
		DSetScreenCursor(column, row);
		DWriteScreenCharAttr((char)i, ATTR_NORMAL);
#else
		use_value2          mvaddch(row, column, (char)i);

#endif
		*p++ = i;
		column++;
	    }
	    break;
	}
    }
    while ((!flag) && (!aborted));
    if (aborted)
	return (FALSE);
/* Remove trailing blanks	 */
    while (p > in_str && p[-1] == ' ')
	p--;
    *p = '\0';
    return (TRUE);
}


/* Pauses for user response before returning		-RAK-	 */
void 
pause_line(prt_line)
int prt_line;
{
    prt("[Press any key to continue.]", prt_line, 23);
    (void)inkey();
    erase_line(prt_line, 0);
}


/* Pauses for user response before returning		-RAK-	 */
/* NOTE: Delay is for players trying to roll up "perfect"	 */
/* characters.  Make them wait a bit.			 */
void 
pause_exit(prt_line, delay)
int prt_line, delay;
{
    char dummy;

    prt("[Press any key to continue, or Q to exit.]", prt_line, 10);
    dummy = inkey();
    if (dummy == 'Q') {
	erase_line(prt_line, 0);
#ifndef MSDOS			   /* PCs are slow enough as is  -dgk */
	if (delay > 0)
	    (void)sleep((unsigned)delay);
#else
    /* prevent message about delay unused */
	dummy = delay;
#endif
#ifdef MAC
	enablefilemenu(FALSE);
	exit_game();
	enablefilemenu(TRUE);
#else
	exit_game();
#endif
    }
    erase_line(prt_line, 0);
}

#ifdef MAC
void 
save_screen()
{
    mac_save_screen();
}

void 
restore_screen()
{
    mac_restore_screen();
}

#else
void 
save_screen()
{
    overwrite(stdscr, savescr);
}

void 
restore_screen()
{
    overwrite(savescr, stdscr);
    touchwin(stdscr);
}

#endif

void 
bell()
{
    put_qio();
#ifdef MAC
    mac_beep();
#else
    (void)write(1, "\007", 1);
#endif
}

/* definitions used by screen_map() */
/* index into border character array */
#define TL 0			   /* top left */
#define TR 1
#define BL 2
#define BR 3
#define HE 4			   /* horizontal edge */
#define VE 5

/* character set to use */
#ifdef MSDOS
# ifdef ANSI
#   define CH(x)	(ansi ? screen_border[0][x] : screen_border[1][x])
# else
#   define CH(x)	(screen_border[1][x])
# endif
#else
#   define CH(x)	(screen_border[0][x])
#endif

/* Display highest priority object in the RATIO by RATIO area */
#define	RATIO 3

void 
screen_map()
{
    register int i, j;
    static byte screen_border[2][6] = {
    {'+', '+', '+', '+', '-', '|'},	/* normal chars */
    {201, 187, 200, 188, 205, 186}	/* graphics chars */
    };
    byte map[MAX_WIDTH / RATIO + 1];
    byte tmp;
    int   priority[256];
    int   row, orow, col, myrow = 0, mycol = 0;

#ifndef MAC
    char  prntscrnbuf[80];

#endif

    for (i = 0; i < 256; i++)
	priority[i] = 0;
    priority['<'] = 5;
    priority['>'] = 5;
    priority['@'] = 10;
#ifdef MSDOS
    priority[wallsym] = (-5);
    priority[floorsym] = (-10);
    priority['�'] = (-1);
#else
    priority['#'] = (-5);
    priority['.'] = (-10);
    priority['x'] = (-1);
#endif
    priority['\''] = (-3);
    priority[' '] = (-15);

    save_screen();
    clear_screen();
#ifdef MAC
    DSetScreenCursor(0, 0);
    DWriteScreenCharAttr(CH(TL), ATTR_NORMAL);
    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	DWriteScreenCharAttr(CH(HE), ATTR_NORMAL);
    DWriteScreenCharAttr(CH(TR), ATTR_NORMAL);
#else
    use_value2          mvaddch(0, 0, CH(TL));

    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	(void)addch(CH(HE));
    (void)addch(CH(TR));
#endif
    orow = (-1);
    map[MAX_WIDTH / RATIO] = '\0';
    for (i = 0; i < MAX_HEIGHT; i++) {
	row = i / RATIO;
	if (row != orow) {
	    if (orow >= 0) {
#ifdef MAC
		DSetScreenCursor(0, orow + 1);
		DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
		DWriteScreenString(map);
		DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
#else
	    /* can not use mvprintw() on ibmpc, because PC-Curses is horribly
	     * written, and mvprintw() causes the fp emulation library to be
	     * linked with PC-Moria, makes the program 10K bigger 
	     */
		(void)sprintf(prntscrnbuf, "%c%s%c",
			      CH(VE), map, CH(VE));
		use_value2          mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
	    }
	    for (j = 0; j < MAX_WIDTH / RATIO; j++)
		map[j] = ' ';
	    orow = row;
	}
	for (j = 0; j < MAX_WIDTH; j++) {
	    col = j / RATIO;
	    tmp = loc_symbol(i, j);
	    if (priority[map[col]] < priority[tmp])
		map[col] = tmp;
	    if (map[col] == '@') {
		mycol = col + 1;   /* account for border */
		myrow = row + 1;
	    }
	}
    }
    if (orow >= 0) {
#ifdef MAC
	DSetScreenCursor(0, orow + 1);
	DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
	DWriteScreenString(map);
	DWriteScreenCharAttr(CH(VE), ATTR_NORMAL);
#else
	(void)sprintf(prntscrnbuf, "%c%s%c",
		      CH(VE), map, CH(VE));
	use_value2          mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
    }
#ifdef MAC
    DSetScreenCursor(0, orow + 2);
    DWriteScreenCharAttr(CH(BL), ATTR_NORMAL);
    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	DWriteScreenCharAttr(CH(HE), ATTR_NORMAL);
    DWriteScreenCharAttr(CH(BR), ATTR_NORMAL);
#else
    use_value2          mvaddch(orow + 2, 0, CH(BL));

    for (i = 0; i < MAX_WIDTH / RATIO; i++)
	(void)addch(CH(HE));
    (void)addch(CH(BR));
#endif

#ifdef MAC
    DSetScreenCursor(23, 23);
    DWriteScreenStringAttr("Hit any key to continue", ATTR_NORMAL);
    if (mycol > 0)
	DSetScreenCursor(mycol, myrow);
#else
    use_value2          mvaddstr(23, 23, "Hit any key to continue");

    if (mycol > 0)
	(void)move(myrow, mycol);
#endif
    (void)inkey();
    restore_screen();
}
