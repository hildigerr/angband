/* File: moria4.c */

/* Purpose: more low level code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




void mmove2(int *y, int *x, int sourcey, int sourcex, int desty, int destx)
{
    int d_y, d_x, k, dist, max_dist, min_dist, shift;

    d_y = (*y < sourcey) ? sourcey - *y : *y - sourcey;
    d_x = (*x < sourcex) ? sourcex - *x : *x - sourcex;

    dist = (d_y > d_x) ? d_y : d_x;
    dist++;

    d_y = (desty < sourcey) ? sourcey - desty : desty - sourcey;
    d_x = (destx < sourcex) ? sourcex - destx : destx - sourcex;

    if (d_y > d_x) {
	max_dist = d_y;
	min_dist = d_x;
    }
    else {
	max_dist = d_x;
	min_dist = d_y;
    }

    for (k = 0, shift = max_dist >> 1; k < dist; k++, shift -= min_dist) {
	shift = (shift > 0) ? shift : shift + max_dist;
    }

    if (shift < 0) shift = 0;

    if (d_y > d_x) {
	d_y = (desty < sourcey) ? *y - 1 : *y + 1;
	if (shift)
	    d_x = *x;
	else
	    d_x = (destx < sourcex) ? *x - 1 : *x + 1;
    }
    else {
	d_x = (destx < sourcex) ? *x - 1 : *x + 1;
	if (shift)
	    d_y = *y;
	else
	    d_y = (desty < sourcey) ? *y - 1 : *y + 1;
    }

    *y = d_y;
    *x = d_x;
}



/*
 * Given direction "dir", returns new row, column location -RAK-
 * targeting code stolen from Morgul -CFT
 * 'dir=0' moves toward target				    CDW  
 */
int mmove(int dir, int *y, int *x)
{
    register int new_row = 0, new_col = 0;
    int          boolflag;

    switch (dir) {

#ifdef TARGET

      case 0:
	new_row = *y;
	new_col = *x;
	mmove2(&new_row, &new_col,
	       char_row, char_col,
	       target_row, target_col);

	break;

#endif /* TARGET */

      case 1:
	new_row = *y + 1;
	new_col = *x - 1;
	break;
      case 2:
	new_row = *y + 1;
	new_col = *x;
	break;
      case 3:
	new_row = *y + 1;
	new_col = *x + 1;
	break;
      case 4:
	new_row = *y;
	new_col = *x - 1;
	break;
      case 5:
	new_row = *y;
	new_col = *x;
	break;
      case 6:
	new_row = *y;
	new_col = *x + 1;
	break;
      case 7:
	new_row = *y - 1;
	new_col = *x - 1;
	break;
      case 8:
	new_row = *y - 1;
	new_col = *x;
	break;
      case 9:
	new_row = *y - 1;
	new_col = *x + 1;
	break;
    }
    boolflag = FALSE;
    if ((new_row >= 0) && (new_row < cur_height)
	&& (new_col >= 0) && (new_col < cur_width)) {
	*y = new_row;
	*x = new_col;
	boolflag = TRUE;
    }
    return (boolflag);
}





