/* File: cave.c */

/* Purpose: mid-level graphics -- colors and symbols and such */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * Is a given location "valid" for placing things?
 * Note that solid rock, doors, and rubble evaluate as "valid".
 * Note that artifacts, store doors, and stairs, do not.
 *
 * This function is usually "combined" with "floor_grid_bold(y,x)",
 * which checks to see if the grid is not a wall or blockage.
 */
bool valid_grid(int y, int x)
{
    cave_type *c_ptr;
    inven_type *i_ptr;

    /* Outer wall (and illegal grids) are not "valid" */
    if (!in_bounds(y,x)) return (FALSE);

    /* Get that grid */
    c_ptr = &cave[y][x];

    /* Internal Boundary walls are invalid */
    if (c_ptr->fval == BOUNDARY_WALL) return (FALSE);

    /* Nothing here, this is very desirable */
    if (c_ptr->i_idx == 0) return (TRUE);

    /* Something there */
    i_ptr = &i_list[c_ptr->i_idx];

    /* Stairs and store doors are very important */
    if (i_ptr->tval == TV_STORE_DOOR) return (FALSE);
    if (i_ptr->tval == TV_DOWN_STAIR) return (FALSE);
    if (i_ptr->tval == TV_UP_STAIR) return (FALSE);

    /* Artifacts are really important */
    if (artifact_p(i_ptr)) return (FALSE);

    /* Normal object may be destroyed */
    return (TRUE);
}



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


    /* XXX XXX XXX Paranoia -- require "safe" origin */
    if (!in_bounds(fromY, fromX)) return (FALSE);


    /* Directly South/North */
    if (!d_x) {

	register int p_y;

	/* South -- check for walls */
	if (d_y > 0) {
	    for (p_y = fromY + 1; p_y < toY; p_y++) {
		if (!floor_grid_bold(p_y,fromX)) return FALSE;
	    }
	}
	
	/* North -- check for walls */
	else {
	    for (p_y = fromY - 1; p_y > toY; p_y--) {
		if (!floor_grid_bold(p_y,fromX)) return FALSE;
	    }
	}
	
	/* Assume los */
	return TRUE;
    }

    /* Directly East/West */
    if (!d_y) {
    
	register int p_x;

	/* East -- check for walls */
	if (d_x > 0) {
	    for (p_x = fromX + 1; p_x < toX; p_x++) {
		if (!floor_grid_bold(fromY,p_x)) return FALSE;
	    }
	}

	/* West -- check for walls */
	else {
	    for (p_x = fromX - 1; p_x > toX; p_x--) {
		if (!floor_grid_bold(fromY,p_x)) return FALSE;
	    }
	}
		
	/* Assume los */
	return TRUE;
    }


    /* Handle Knightlike shapes -CWS */
    if (a_x == 1) {
	if (d_y == 2) {
	    if (floor_grid_bold(fromY + 1, fromX)) return TRUE;
	}
	else if (d_y == (-2)) {
	    if (floor_grid_bold(fromY - 1, fromX)) return TRUE;
	}
    }
    else if (a_y == 1) {
	if (d_x == 2) {
	    if (floor_grid_bold(fromY, fromX + 1)) return TRUE;
	}
	else if (d_x == (-2)) {
	    if (floor_grid_bold(fromY, fromX - 1)) return TRUE;
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
		if (!floor_grid_bold(p_y,p_x)) return FALSE;
		dy += m;
		if (dy < scale2) {
		    p_x += xSign;
		}
		else if (dy > scale2) {
		    p_y += ySign;
		    if (!floor_grid_bold(p_y,p_x)) return FALSE;
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
		if (!floor_grid_bold(p_y,p_x)) return FALSE;
		dx += m;
		if (dx < scale2) {
		    p_y += ySign;
		}
		else if (dx > scale2) {
		    p_x += xSign;
		    if (!floor_grid_bold(p_y,p_x)) return FALSE;
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
 * Prints the map of the dungeon
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
	if ((p_ptr->blind < 1) && !c_ptr->pl)
	    light_room(char_row, char_col);
    }
/* In doorway of light-room?		   */
    else if (c_ptr->lr && (p_ptr->blind < 1)) {
	for (i = (char_row - 1); i <= (char_row + 1); i++)
	    for (j = (char_col - 1); j <= (char_col + 1); j++) {
		d_ptr = &cave[i][j];
		if ((d_ptr->fval == LIGHT_FLOOR) && !d_ptr->pl)
		    light_room(i, j);
	    }
    }
}





/*
 * Map the current panel (plus some)
 */
void map_area(void)
{
    register cave_type *c_ptr;
    register int        i7, i8, x, y;
    int                 y1, y2, x1, x2;

    /* Pick an area to map */
    y1 = panel_row_min - randint(10);
    y2 = panel_row_max + randint(10);
    x1 = panel_col_min - randint(20);
    x2 = panel_col_max + randint(20);

    /* Scan that area */
    for (y = y1; y <= y2; y++) {
	for (x = x1; x <= x2; x++) {

	    if (in_bounds(y, x) && (cave[y][x].fval <= MAX_CAVE_FLOOR))

		for (i7 = y - 1; i7 <= y + 1; i7++) {
		    for (i8 = x - 1; i8 <= x + 1; i8++) {

			c_ptr = &cave[i7][i8];

			if (c_ptr->fval >= MIN_WALL)
			    c_ptr->pl = TRUE;

			else if ((c_ptr->i_idx != 0) &&
			     (i_list[c_ptr->i_idx].tval >= TV_MIN_VISIBLE) &&
			       (i_list[c_ptr->i_idx].tval <= TV_MAX_VISIBLE))
			    c_ptr->fm = TRUE;
		    }
		}
	}
    }

    /* Redraw the map */
    prt_map();
}



/*
 * Light up the dungeon.
 */
void wiz_lite(int light)
{
    register cave_type *c_ptr;
    register int        k, l, i, j;
    int                 flag;

    if (!light) {
	if (cave[char_row][char_col].pl) flag = FALSE;
	else flag = TRUE;
    } else {
	flag = (light > 0) ? 1 : 0;
    }

    /* Perma-light all open space and adjacent walls */
    for (i = 0; i < cur_height; i++) {
	for (j = 0; j < cur_width; j++) {

	    /* Process all non-walls */
	    if (cave[i][j].fval <= MAX_CAVE_FLOOR) {

		/* Perma-lite all grids touching those grids */
		for (k = i - 1; k <= i + 1; k++) {
		    for (l = j - 1; l <= j + 1; l++) {

			/* Get the grid */
			c_ptr = &cave[k][l];

			/* Perma-lite all the grid */
			c_ptr->pl = flag;

			if (!flag) c_ptr->fm = FALSE;
		    }
		}
	    }
	}
    }

    /* Redraw the map */    
    prt_map();
}



/* "symbol" definitions used by screen_map() */

#if defined(MSDOS) && defined(ANSI)

# define CH_TL (ansi ? 201 : '+')
# define CH_TR (ansi ? 187 : '+')
# define CH_BL (ansi ? 200 : '+')
# define CH_BR (ansi ? 188 : '+')
# define CH_HE (ansi ? 205 : '-')
# define CH_VE (ansi ? 186 : '|')

#else

# define CH_TL '+'
# define CH_TR '+'
# define CH_BL '+'
# define CH_BR '+'
# define CH_HE '-'
# define CH_VE '|'

#endif

/* Display highest priority object in the RATIO by RATIO area */
#define	RATIO 3

/* Display the entire map */
#define MAP_HGT (MAX_HEIGHT / RATIO)
#define MAP_WID (MAX_WIDTH / RATIO)

void screen_map(void)
{
    register int i, j;

    byte map[MAP_WID + 1];
    byte tmp;

    int   priority[256];

    int   row, orow, col, myrow = 0, mycol = 0;

#ifndef MAC
    char  prntscrnbuf[80];

#endif

    for (i = 0; i < 256; i++) priority[i] = 0;

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
    DWriteScreenCharAttr(CH_TL, ATTR_NORMAL);
    for (i = 0; i < MAP_WID; i++)
	DWriteScreenCharAttr(CH_HE, ATTR_NORMAL);
    DWriteScreenCharAttr(CH_TR, ATTR_NORMAL);
#else
    mvaddch(0, 0, CH_TL);

    for (i = 0; i < MAP_WID; i++)
	(void)addch(CH_HE);
    (void)addch(CH_TR);
#endif
    orow = (-1);
    map[MAP_WID] = '\0';
    for (i = 0; i < MAX_HEIGHT; i++) {
	row = i / RATIO;
	if (row != orow) {
	    if (orow >= 0) {
#ifdef MAC
		DSetScreenCursor(0, orow + 1);
		DWriteScreenCharAttr(CH_VE, ATTR_NORMAL);
		DWriteScreenString(map);
		DWriteScreenCharAttr(CH_VE, ATTR_NORMAL);
#else
	    /* can not use mvprintw() on ibmpc, because PC-Curses is horribly
	     * written, and mvprintw() causes the fp emulation library to be
	     * linked with PC-Moria, makes the program 10K bigger 
	     */
		(void)sprintf(prntscrnbuf, "%c%s%c",
			      CH_VE, map, CH_VE);
		mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
	    }
	    for (j = 0; j < MAP_WID; j++)
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
	DWriteScreenCharAttr(CH_VE, ATTR_NORMAL);
	DWriteScreenString(map);
	DWriteScreenCharAttr(CH_VE, ATTR_NORMAL);
#else
	(void)sprintf(prntscrnbuf, "%c%s%c",
		      CH_VE, map, CH_VE);
	mvaddstr(orow + 1, 0, prntscrnbuf);

#endif
    }
#ifdef MAC
    DSetScreenCursor(0, orow + 2);
    DWriteScreenCharAttr(CH_BL, ATTR_NORMAL);
    for (i = 0; i < MAP_WID; i++)
	DWriteScreenCharAttr(CH_HE, ATTR_NORMAL);
    DWriteScreenCharAttr(CH_BR, ATTR_NORMAL);
#else
    mvaddch(orow + 2, 0, CH_BL);

    for (i = 0; i < MAP_WID; i++)
	(void)addch(CH_HE);
    (void)addch(CH_BR);
#endif

#ifdef MAC
    DSetScreenCursor(23, 23);
    DWriteScreenStringAttr("Hit any key to continue", ATTR_NORMAL);
    if (mycol > 0)
	DSetScreenCursor(mycol, myrow);
#else
    mvaddstr(23, 23, "Hit any key to continue");

    if (mycol > 0)
	(void)move(myrow, mycol);
#endif
    (void)inkey();
    restore_screen();
}





