/* File: cave.c */


/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
 * 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu. 
 *
 * Returns TRUE if a line of sight can be traced from x0, y0 to x1, y1. 
 *
 * The LOS begins at the center of the tile [x0, y0] and ends at the center of
 * the tile [x1, y1].  If los() is to return TRUE, all of the tiles this line
 * passes through must be transparent, WITH THE EXCEPTIONS of the starting
 * and ending tiles. 
 *
 * We don't consider the line to be "passing through" a tile if it only passes
 * across one corner of that tile. 
 */

/*
 * Because this function uses (short) ints for all calculations, overflow may
 * occur if d_x and d_y exceed 90.
 *
 * Also note that this function and the "move towards target" code do NOT
 * share the same properties.  Thus, you can see someone, target them, and
 * then fire a bolt at them, but the bolt may hit a wall, not them.  However,
 * by clever choice of target locations, you can sometimes throw a "curve".
 */
int los(int fromY, int fromX, int toY, int toX)
{
    register int p_x, p_y, d_x, d_y, a_x, a_y;


    /* Extract the offset */    
    d_y = toY - fromY;
    d_x = toX - fromX;

    /* Extract the absolute offset */
    a_y = MY_ABS(d_y);
    a_x = MY_ABS(d_x);


    /* Handle adjacent (or identical) grids */
    if ((a_x < 2) && (a_y < 2)) return (TRUE);


    /* XXX XXX Paranoia -- require "safe" origin */
    if (!in_bounds(fromY, fromX)) return (FALSE);


    /* Directly South/North */
    if (!d_x) {

	register int p_y;

	/* South -- check for walls */
	if (d_y > 0) {
	    for (p_y = fromY + 1; p_y < toY; p_y++) {
		if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
	
	/* North -- check for walls */
	else {
	    for (p_y = fromY - 1; p_y > toY; p_y--) {
		if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
	
	return TRUE;
    }

    /* Directly East/West */
    if (!d_y) {
    
	register int p_x;

	/* East -- check for walls */
	if (d_x > 0) {
	    for (p_x = fromX + 1; p_x < toX; p_x++) {
		if (cave[fromY][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}

	/* West -- check for walls */
	else {
	    for (p_x = fromX - 1; p_x > toX; p_x--) {
		if (cave[fromY][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	}
		
	/* Assume los */
	return TRUE;
    }


    /* Handle Knightlike shapes -CWS */
    if (a_x == 1) {
	if (d_y == 2) {
	    if (cave[fromY + 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (d_y == (-2)) {
	    if (cave[fromY - 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }
    else if (a_y == 1) {
	if (d_x == 2) {
	    if (cave[fromY][fromX + 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (d_x == (-2)) {
	    if (cave[fromY][fromX - 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }


/*
 * Now, we've eliminated all the degenerate cases. In the computations below,
 * dy (or dx) and m are multiplied by a scale factor, scale = abs(d_x *
 * d_y * 2), so that we can use integer arithmetic. 
 */

    {
	register int        scale,	/* a scale factor		 */
			    scale2;	/* above scale factor / 2	 */

	int		    xSign,	/* sign of d_x		 */
			    ySign,	/* sign of d_y		 */
			    m;		/* slope or 1/slope of LOS	 */

	scale2 = (a_x * a_y);
	scale = scale2 << 1;

	xSign = (d_x < 0) ? -1 : 1;
	ySign = (d_y < 0) ? -1 : 1;


	/* Travel from one end of the line to the other, */
	/* oriented along the longer axis. */

	if (a_x >= a_y) {

	    register int        dy;  /* "fractional" y position	 */

	/*
	 * We start at the border between the first and second tiles, where
	 * the y offset = .5 * slope.  Remember the scale factor.  We have: 
	 *
	 * m = d_y / d_x * 2 * (d_y * d_x) = 2 * d_y * d_y. 
	 */

	    dy = a_y * a_y;
	    m = dy << 1;
	    p_x = fromX + xSign;

	    /* Consider the special case where slope == 1. */
	    if (dy == scale2) {
		p_y = fromY + ySign;
		dy -= scale;
	    }
	    else {
		p_y = fromY;
	    }

	    /* Note (below) the case (dy == scale2), where */
	    /* the LOS exactly meets the corner of a tile. */
	    while (toX - p_x) {
		if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		dy += m;
		if (dy < scale2) {
		    p_x += xSign;
		}
		else if (dy > scale2) {
		    p_y += ySign;
		    if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dy -= scale;
		    p_x += xSign;
		}
		else {
		    p_y += ySign;
		    dy -= scale;
		    p_x += xSign;
		}
	    }
	    return TRUE;
	}

	else {
	
	    register int        dx;	/* "fractional" x position	 */

	    dx = a_x * a_x;
	    m = dx << 1;

	    p_y = fromY + ySign;
	    if (dx == scale2) {
		p_x = fromX + xSign;
		dx -= scale;
	    }
	    else {
		p_x = fromX;
	    }

	    /* Note (below) the case (dx == scale2), where */
	    /* the LOS exactly meets the corner of a tile. */
	    while (toY - p_y) {
		if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		dx += m;
		if (dx < scale2) {
		    p_y += ySign;
		}
		else if (dx > scale2) {
		    p_x += xSign;
		    if (cave[p_y][p_x].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dx -= scale;
		    p_y += ySign;
		}
		else {
		    p_x += xSign;
		    dx -= scale;
		    p_y += ySign;
		}
	    }
	}
    }

    /* Assume los */
    return TRUE;
}





/*
 * Tests a spot for light or field mark status		-RAK-	
 */
int test_lite(int y, int x)
{
    register cave_type *cave_ptr;

    cave_ptr = &cave[y][x];
    if (cave_ptr->pl || cave_ptr->tl || cave_ptr->fm)
	return (TRUE);
    else
	return (FALSE);
}


/*
 * Returns true if player has no light			-RAK-	
 */
int no_lite(void)
{
    register cave_type *c_ptr;

    c_ptr = &cave[char_row][char_col];
    if (!c_ptr->tl && !c_ptr->pl)
	return TRUE;
    return FALSE;
}



/* 
 * Lights up given location				-RAK-
 */
void lite_spot(int y, int x)
{
    if (panel_contains(y, x))
	print(loc_symbol(y, x), y, x);
}






/*
 * Moves the cursor to a given interpolated y, x position	-RAK-
 */

void move_cursor_relative(int row, int col)

#ifdef MAC
{
    /* Real co-ords convert to screen positions */
    row -= panel_row_prt;
    col -= panel_col_prt;

    DSetScreenCursor(col, row);
}

#else
{
    vtype tmp_str;

    /* Real co-ords convert to screen positions */
    row -= panel_row_prt;
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





/*
 * Prints the map of the dungeon			-RAK-
 */
void prt_map(void)
{
    register int i, j, k;
    register unsigned char tmp_char;

    k = 0;
    for (i = panel_row_min; i <= panel_row_max; i++) {	/* Top to bottom */
	k++;
	erase_line(k, 13);

	for (j = panel_col_min; j <= panel_col_max; j++) { /* Left to right */

	    tmp_char = loc_symbol(i, j);
	    if (tmp_char != ' ')
		print(tmp_char, i, j);
	}
    }
}





/*
 * We need to reset the view of things.			-CJS-
 */
void check_view(void)
{
    register int i, j;
    register cave_type *c_ptr, *d_ptr;

    c_ptr = &cave[char_row][char_col];
/* Check for new panel		   */
    if (get_panel(char_row, char_col, FALSE))
	prt_map();
/* Move the light source		   */
    move_light(char_row, char_col, char_row, char_col);
/* A room of light should be lit.	 */
    if (c_ptr->fval == LIGHT_FLOOR) {
	if ((py.flags.blind < 1) && !c_ptr->pl)
	    light_room(char_row, char_col);
    }
/* In doorway of light-room?		   */
    else if (c_ptr->lr && (py.flags.blind < 1)) {
	for (i = (char_row - 1); i <= (char_row + 1); i++)
	    for (j = (char_col - 1); j <= (char_col + 1); j++) {
		d_ptr = &cave[i][j];
		if ((d_ptr->fval == LIGHT_FLOOR) && !d_ptr->pl)
		    light_room(i, j);
	    }
    }
}





/*
 * Map the current area plus some			-RAK-
 */
void map_area(void)
{
    register cave_type *c_ptr;
    register int        i7, i8, x, y;
    int                 y1, y2, x1, x2;

    y1 = panel_row_min - randint(10);
    y2 = panel_row_max + randint(10);
    x1 = panel_col_min - randint(20);
    x2 = panel_col_max + randint(20);

    for (y = y1; y <= y2; y++) {
	for (x = x1; x <= x2; x++) {

	    if (in_bounds(y, x) && (cave[y][x].fval <= MAX_CAVE_FLOOR))

		for (i7 = y - 1; i7 <= y + 1; i7++) {
		    for (i8 = x - 1; i8 <= x + 1; i8++) {

			c_ptr = &cave[i7][i8];

			if (c_ptr->fval >= MIN_CAVE_WALL)
			    c_ptr->pl = TRUE;

			else if ((c_ptr->tptr != 0) &&
			     (i_list[c_ptr->tptr].tval >= TV_MIN_VISIBLE) &&
			       (i_list[c_ptr->tptr].tval <= TV_MAX_VISIBLE))
			    c_ptr->fm = TRUE;
		    }
		}
	}
    }
    prt_map();
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

void screen_map(void)
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
    priority['±'] = (-1);
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





