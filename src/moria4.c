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





/*** Targetting Code ***/


#ifdef TARGET



/*
 * This targetting code stolen from Morgul -CFT 
 * Assuming target_mode == TRUE, returns if the position is the target. -CDW 
 */
int target_at(int row,int col)
{

    if (target_mode && (row==target_row) && (col==target_col)) {
	return (TRUE);
    }
    return (FALSE);
}


/*
 * This targetting code stolen from Morgul -CFT
 * Targetting routine 					CDW
 */
int target_set()
{
    int row, col;
    int m_idx,exit_1;
    char query;
    vtype desc;

    exit_1 = FALSE;

    /* Check monsters first */

    target_mode = FALSE;

    for (m_idx = 0; (m_idx < mfptr) && (!exit_1); m_idx++) {

	monster_type *m_ptr = &m_list[m_idx];

	if (!m_ptr->ml ||
		!los(char_row,char_col,m_ptr->fy,m_ptr->fx)) continue;

	/* Access monster location */
	row = m_ptr->fy;
	col = m_ptr->fx;

	    move_cursor_relative(row,col);
	    sprintf(desc, "%s [(r)ecall] [(t)arget] [(l)ocation] [ESC quits]",
		    c_list[m_list[m_idx].mptr].name);
	    prt(desc,0,0);
	    move_cursor_relative(row,col);
	    query = inkey();
	    while ((query == 'r') || (query == 'R')) {
		save_screen();
		query = roff_recall(m_list[m_idx].mptr);
		restore_screen();
		move_cursor_relative(row, col);
		query = inkey();
	    }

	switch (query) {

	    case ESCAPE:
		return (FALSE);

	    case 'T': case 't':
	    case '5': case '.':
	    case '0':
		target_mode = TRUE;
		target_mon  = m_idx;
		target_row  = row;
		target_col  = col;
		return (TRUE);

	    case 'l': case'L':
		exit_1 = TRUE;
	    default:
		break;
	}
    }

    prt("Use cursor to designate target. [(t)arget]",0,0);

    row = char_row;
    col = char_col;

    for (exit_1 = FALSE; exit_1==FALSE ;) {

	move_cursor_relative(row, col);

	/* Get a command, and convert it to standard form */
	query = inkey();

	/* Analyze the keypress */
	switch (query) {

	    case ESCAPE: break;
	    case 'Q': case 'q': query = ESCAPE; break;

	    /* Nowhere means "target" */
	    case '5': case '.': query = '0'; break;

	    /* Target means "target" */
	    case 'T': case 't': case '0': query = '0'; break;

	    /* Real directions */
	    case '1': case 'B': case 'b': query = '1'; break;
	    case '2': case 'J': case 'j': query = '2'; break;
	    case '3': case 'N': case 'n': query = '3'; break;
	    case '4': case 'H': case 'h': query = '4'; break;
	    case '6': case 'L': case 'l': query = '6'; break;
	    case '7': case 'Y': case 'y': query = '7'; break;
	    case '8': case 'K': case 'k': query = '8'; break;
	    case '9': case 'U': case 'u': query = '9'; break;

	    /* Ignore all other keys */
	    default: query = ' '; break;
	}

	/* Analyze the command */
	switch (query) {

	    case ESCAPE:
		return (FALSE);

	    case '0':
		target_mode = TRUE;
		target_mon  = MAX_M_IDX;
		target_row  = row;
		target_col  = col;
		return (TRUE);

	    case '1':
		col--;
	    case '2':
		row++;
		break;
	    case '3':
		row++;
	    case '6':
		col++;
		break;
	    case '7':
		row--;
	    case '4':
		col--;
		break;
	    case '9':
		col++;
	    case '8':
		row--;
		break;
		
	    default:
		break;
	}

	if ((col>=cur_width-1) || (col>panel_col_max)) col--;
	else if ((col<=0) || (col<panel_col_min)) col++;

	if ((row>=cur_height-1) || (row>panel_row_max)) row--;
	else if ((row<=0) || (row<panel_row_min)) row++;
    }

    return (FALSE);
}

#endif /* TARGET */



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





