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
void target()
{
    int m_idx,exit,exit2;
    char query;
    vtype desc;

    exit = FALSE;
    exit2 = FALSE;

    if (py.flags.blind > 0)
	msg_print("You can't see anything to target!");

    /* Check monsters first */
    else {

    target_mode = FALSE;

    for (m_idx = 0; (m_idx < mfptr) && (!exit); m_idx++) {

	    if (m_list[m_idx].cdis<MAX_SIGHT) {

		if ((m_list[m_idx].ml) &&
		    (los(char_row,char_col,m_list[m_idx].fy,m_list[m_idx].fx))) {

		    move_cursor_relative(m_list[m_idx].fy,m_list[m_idx].fx);
		    sprintf(desc, "%s [(r)ecall] [(t)arget] [(l)ocation] [ESC quits]",
			    c_list[m_list[m_idx].mptr].name);
		    prt(desc,0,0);
		    move_cursor_relative(m_list[m_idx].fy,m_list[m_idx].fx);
		    query = inkey();
		    while ((query == 'r') || (query == 'R')) {
			save_screen();
			query = roff_recall(m_list[m_idx].mptr);
			restore_screen();
			move_cursor_relative(m_list[m_idx].fy, m_list[m_idx].fx);
			query = inkey();
		    }

		    switch (query) {

		    case ESCAPE:
			exit = TRUE;
			exit2 = TRUE;
			break;
		    case '.':	/* for NetHack players, '.' is used to select a target,
				   so I'm changing this... -CFT */

		    case 'T': case 't':
			target_mode = TRUE;
			target_mon  = m_idx;
			target_row  = m_list[m_idx].fy;
			target_col  = m_list[m_idx].fx;
			exit2 = TRUE;
		    case 'l': case'L':
			exit = TRUE;
		    default:
			break;
		    }
		}
	    }
    }

	if (exit2 == FALSE) {
	    prt("Use cursor to designate target. [(t)arget]",0,0);

	    target_row = char_row;
	    target_col = char_col;

	    for (exit = FALSE; exit==FALSE ;) {

		move_cursor_relative(target_row, target_col);

		query = inkey();

		if (rogue_like_commands==FALSE) {
		    switch (query) {

		    case '1': query = 'b'; break;
		    case '2': query = 'j'; break;
		    case '3': query = 'n'; break;
		    case '4': query = 'h'; break;
		    case '5': query = '.';
		    case '6': query = 'l'; break;
		    case '7': query = 'y'; break;
		    case '8': query = 'k'; break;
		    case '9': query = 'u'; break;

		    default: break;
		    }
		}

	    switch (query) {

	    case ESCAPE:
		case 'Q': case'q': exit = TRUE; break;

	    case '.':	/* for NetHack players, '.' is used to select a target,
				   so I'm changing this... -CFT */

	    case 'T': case 't':
		    if (distance(char_row,char_col,target_row,target_col)>MAX_SIGHT)
			prt(
			    "Target beyond range. Use cursor to designate target. [(t)arget].",
			    0,0);
		    else if (cave[target_row][target_col].fval>CORR_FLOOR)
			prt(
			    "Invalid target. Use cursor to designate target. [(t)arget].",
			    0,0);
		    else {
			target_mode = TRUE;
			target_mon  = MAX_M_IDX;
			exit = TRUE;
		    }
		    break;

	    case 'b':
		    target_col--;
	    case 'j':
		    target_row++;
		    break;
	    case 'n':
		    target_row++;
	    case 'l':
		    target_col++;
		    break;
	    case 'y':
		    target_row--;
	    case 'h':
		    target_col--;
		    break;
	    case 'u':
		    target_col++;
	    case 'k':
		    target_row--;
		    break;
		
	    default:
		    break;
		}

		if ((target_col>MAX_WIDTH-2) || (target_col>panel_col_max)) target_col--;
		else if ((target_col<1) || (target_col<panel_col_min))  target_col++;

		if ((target_row>MAX_HEIGHT-2) || (target_row>panel_row_max)) target_row--;
		else if ((target_row<1) || (target_row<panel_row_min)) target_row++;
	    }
	}

	if (target_mode==TRUE)
	    msg_print("Target selected.");
	else
	    msg_print("Aborting Target.");
    }
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





