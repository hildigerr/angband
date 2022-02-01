/* File: io.c */

/* Purpose: mid-level I/O (uses term.c) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */


#if !defined(GEMDOS)
#ifdef MAC
#include <scrnmgr.h>
#endif
#endif

#include "angband.h"

#if !defined(MSDOS)
#if !defined(ATARIST_MWC) && !defined(MAC)
#include <signal.h>
#endif
#endif

#ifndef MAC
static WINDOW *savescr;	   /* Spare window for saving the screen.
				    * -CJS- */
#endif

#ifdef MAC
/* Attributes of normal and hilighted characters */
#define ATTR_NORMAL	attrNormal
#define ATTR_HILITED	attrReversed
#endif

#ifdef MAC

/* initializes curses routines */
void init_curses()
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

#endif


/* Dump IO to buffer					-RAK-	 */
void put_str(cptr out_str, int row, int col)

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
void put_qio()
{
    screen_change = TRUE;	   /* Let inven_command know something has
				    * changed. */
    (void)refresh();
}


/*
 * Returns a single character input from the terminal.	This silently -CJS-
 * consumes ^R to redraw the screen and reset the terminal, so that this
 * operation can always be performed at any input prompt.  inkey() never
 * returns ^R.	 
 */
char inkey()
{
#ifdef MAC
/* The Mac does not need ^R, so it just consumes it */
/* This routine does nothing special with direction keys */
/* Just returns their keypad ascii value (e.g. '0'-'9') */
/* Compare with inkeydir() below */
    char ch;
    int  dir;
    int  shift_flag, ctrl_flag;

    put_qio();
    command_rep = 0;

    do {
	macgetkey(&ch, FALSE);
    } while (ch == CTRL('R'));

    dir = extractdir(ch, &shift_flag, &ctrl_flag);
    if (dir != -1)
	ch = '0' + dir;

    return (ch);
#else
    int i;

    put_qio();			   /* Dump IO buffer		 */
    command_rep = 0;		   /* Just to be safe -CJS- */
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
#endif
}


#ifdef MAC
char inkeydir()
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
    command_rep = 0;

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
void flush()
{
#ifdef MAC
/* Removed put_qio() call.  Reduces flashing.  Doesn't seem to hurt. */
    FlushScreenKeys();
#else
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
#endif
}



/* Clears given line of text				-RAK-	 */
void erase_line(int row, int col)
{
#ifdef MAC
    Rect line;

    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);

    line.left = col;
    line.top = row;
    line.right = SCRN_COLS;
    line.bottom = row + 1;
    DEraseScreen(&line);
#else
    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);
    (void)move(row, col);
    clrtoeol();
#endif
}



/* Clears screen */
void clear_screen()
{
#ifdef MAC
    Rect area;

    if (msg_flag)
	msg_print(NULL);

    area.left = area.top = 0;
    area.right = SCRN_COLS;
    area.bottom = SCRN_ROWS;
    DEraseScreen(&area);
#else
    if (msg_flag)
	msg_print(NULL);
    touchwin(stdscr);
    (void)clear();
    refresh();
#endif
}


void clear_from(int row)
{
#ifdef MAC
    Rect area;

    area.left = 0;
    area.top = row;
    area.right = SCRN_COLS;
    area.bottom = SCRN_ROWS;
    DEraseScreen(&area);
#else
    (void)move(row, 0);
    clrtobot();
#endif
}


/* Outputs a char to a given interpolated y, x position	-RAK-	 */
/* sign bit of a character used to indicate standout mode. -CJS */
void print(int ch, int row, int col)
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




/* Print a message so as not to interrupt a counted command. -CJS- */
void count_msg_print(cptr p)
{
    int i;

    i = command_rep;
    msg_print(p);
    command_rep = i;
}


/* Outputs a line to a given y, x position		-RAK-	 */
void prt(cptr str_buff, int row, int col)
{
#ifdef MAC
    Rect line;

    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);

    line.left = col;
    line.top = row;
    line.right = SCRN_COLS;
    line.bottom = row + 1;
    DEraseScreen(&line);

    put_str(str_buff, row, col);
#else
    if (row == MSG_LINE && msg_flag)
	msg_print(NULL);
    (void)move(row, col);
    clrtoeol();
    put_str(str_buff, row, col);
#endif
}



/* move cursor to a given y, x position */
void move_cursor(int row, int col)
{
#ifdef MAC
    DSetScreenCursor(col, row);
#else
    (void)move(row, col);
#endif
}



/* Outputs message to top line of screen				 */
/* These messages are kept for later reference.	 */
void msg_print(cptr str_buff)
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
		command_rep = 0;
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
		command_rep = 0;
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
	    command_rep = 0;
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


/*
 * Let the player verify a choice.  -CJS-
 * Return TRUE on "yes", else FALSE
 */
int get_check(cptr prompt)
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

    /* Get an acceptable answer */
    do {
	res = inkey();
    }
    while (res == ' ');

    /* Erase the prompt */
    erase_line(0, 0);

    return ((res == 'Y' || res == 'y') ? TRUE : FALSE);
}


/*
 * Prompts (optional), then gets and stores a keypress
 * Returns false if <ESCAPE> is input
 */
int get_com(cptr prompt, char *command)
{
    if (prompt) prt(prompt, 0, 0);

    *command = inkey();

    erase_line(MSG_LINE, 0);

    return (*command != 0 && *command != ESCAPE);
}

#ifdef MAC
/* Same as get_com(), but translates direction keys from keypad */
int get_comdir(char *prompt, char *command)
{
    int res;

    if (prompt) prt(prompt, 0, 0);

    *command = inkeydir();

    return (*command != 0 && *command != ESCAPE);
}
#endif


/*
 * Gets a string terminated by <RETURN>
 * Function returns false if <ESCAPE> is input
 */
int get_string(char *buf, int row, int col, int len)
{
    register int i, x1, x2;
    char        *p = buf;
    int done;

    (void)move(row, col);
    for (i = len; i > 0; i--)
	(void)addch(' ');
    (void)move(row, col);

    /* Find the box bounds */
    x1 = col;
    x2 = x1 + len - 1;
    if (x2 > 79) {
	len = 80 - x1;
	x2 = 79;
    }

    /* Process input */    
    while (!done) {

	i = inkey();

	switch (i) {

	  case ESCAPE:
	    return (FALSE);

	  case CTRL('J'):
	  case CTRL('M'):
	    done = TRUE;
	    break;

	  case DELETE:
	  case CTRL('H'):
	    if (col > x1) {
		col--;
		put_str(" ", row, col);
		move_cursor(row, col);
		*--p = '\0';
	    }
	    break;

	  default:
	    if (!isprint(i) || col > x2)
		bell();
	    else {
#ifdef MAC
		DSetScreenCursor(col, row);
		DWriteScreenCharAttr((char)i, ATTR_NORMAL);
#else
		mvaddch(row, col, (char)i);
#endif
		*p++ = i;
		col++;
	    }
	    break;
	}
    }

    /* Remove trailing blanks */
    while (p > buf && p[-1] == ' ') p--;

    /* Terminate it */
    *p = '\0';

    /* Return the result */
    return (TRUE);
}


/*
 * Pauses for user response before returning		-RAK-	 
 */
void pause_line(int prt_line)
{
    int i;
    prt("[Press any key to continue.]", prt_line, 23);
    i = inkey();
    erase_line(prt_line, 0);
}




/*
 * Save and restore the screen -- no flushing
 */

void save_screen()
{
#ifdef MAC
    mac_save_screen();
#else
    overwrite(stdscr, savescr);
#endif
}

void restore_screen()
{
#ifdef MAC
    mac_restore_screen();
#else
    overwrite(savescr, stdscr);
    touchwin(stdscr);
#endif
}



void bell()
{
    put_qio();
#ifdef MAC
    mac_beep();
#else
    (void)write(1, "\007", 1);
#endif
}

