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
 * occur if deltaX and deltaY exceed 90.
 */
int los(int fromY, int fromX, int toY, int toX)
{
    register int tmp, deltaX, deltaY;

    deltaY = toY - fromY;
    deltaX = toX - fromX;

    /* Adjacent? */
    if ((deltaX < 2) && (deltaX > -2) && (deltaY < 2) && (deltaY > -2))
	return TRUE;

    /* Handle the cases where deltaX or deltaY == 0. */
    if (!deltaX) {

	register int p_y; /* y position -- loop variable	 */

	if (deltaY < 0) {
	    tmp = fromY;
	    fromY = toY;
	    toY = tmp;
	}
	for (p_y = fromY + 1; p_y < toY; p_y++) {
		if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
	
	return TRUE;
    }

    else if (!deltaY) {
    
	register int px; /* x position -- loop variable	 */

	if (deltaX < 0) {
	    tmp = fromX;
	    fromX = toX;
	    toX = tmp;
	}
	for (px = fromX + 1; px < toX; px++) {
		if (cave[fromY][px].fval >= MIN_CLOSED_SPACE) return FALSE;
	    }
		
	return TRUE;
    }


    /* Handle Knightlike shapes -CWS */
    if (MY_ABS(deltaX) == 1) {
	if (deltaY == 2) {
	    if (cave[fromY + 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (deltaY == (-2)) {
	    if (cave[fromY - 1][fromX].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }
    else if (MY_ABS(deltaY) == 1) {
	if (deltaX == 2) {
	    if (cave[fromY][fromX + 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
	else if (deltaX == (-2)) {
	    if (cave[fromY][fromX - 1].fval <= MAX_OPEN_SPACE) return TRUE;
	}
    }


/*
 * Now, we've eliminated all the degenerate cases. In the computations below,
 * dy (or dx) and m are multiplied by a scale factor, scale = abs(deltaX *
 * deltaY * 2), so that we can use integer arithmetic. 
 */

    {
	register int        px,	   /* x position			 */
	                    p_y,   /* y position			 */
			    scale,	/* above scale factor		 */
			    scale2;	/* above scale factor / 2	 */

	int		    xSign,	/* sign of deltaX		 */
			    ySign,	/* sign of deltaY		 */
			    m;		/* slope or 1/slope of LOS	 */

	scale2 = MY_ABS(deltaX * deltaY);
	scale = scale2 << 1;

	xSign = (deltaX < 0) ? -1 : 1;
	ySign = (deltaY < 0) ? -1 : 1;


	/* Travel from one end of the line to the other, */
	/* oriented along the longer axis. */

	if (MY_ABS(deltaX) >= MY_ABS(deltaY)) {

	    register int        dy;  /* "fractional" y position	 */

	/*
	 * We start at the border between the first and second tiles, where
	 * the y offset = .5 * slope.  Remember the scale factor.  We have: 
	 *
	 * m = deltaY / deltaX * 2 * (deltaY * deltaX) = 2 * deltaY * deltaY. 
	 */

	    dy = deltaY * deltaY;
	    m = dy << 1;
	    px = fromX + xSign;

	    /* Consider the special case where slope == 1. */
	    if (dy == scale2) {
		p_y = fromY + ySign;
		dy -= scale;
	    }
	    else {
		p_y = fromY;
	    }

	    while (toX - px) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		dy += m;
		if (dy < scale2) {
		    px += xSign;
		}
		else if (dy > scale2) {
		    p_y += ySign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dy -= scale;
		    px += xSign;
		}
		else {
		/*
		 * This is the case, dy == scale2, where the LOS exactly
		 * meets the corner of a tile. 
		 */
		    p_y += ySign;
		    dy -= scale;
		    px += xSign;
		}
	    }
	    return TRUE;
	}

	else {
	
	    register int        dx;	/* "fractional" x position	 */

	    dx = deltaX * deltaX;
	    m = dx << 1;

	    p_y = fromY + ySign;
	    if (dx == scale2) {
		px = fromX + xSign;
		dx -= scale;
	    }
	    else {
		px = fromX;
	    }

	    while (toY - p_y) {
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		dx += m;
		if (dx < scale2) {
		    p_y += ySign;
		}
		else if (dx > scale2) {
		    px += xSign;
		    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) return FALSE;
		    dx -= scale;
		    p_y += ySign;
		}
		else {
		    px += xSign;
		    dx -= scale;
		    p_y += ySign;
		}
	    }
	    return TRUE;
	}
    }
}





