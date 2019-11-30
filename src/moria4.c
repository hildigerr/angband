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
 * This targetting code stolen from Morgul
 * Returns TRUE if the given position is the target. -CDW 
 */
int target_at(int row,int col)
{

    /* Compare the locations */
    if (target_mode && (row==target_row) && (col==target_col)) {
	return (TRUE);
    }

    /* Assume target is not at (row,col) */
    return (FALSE);
}



/*
 * Simple query -- is the "target" okay to use?
 * Obviously, if target mode is disabled, it is not.
 */
int target_okay()
{

    if ((target_mode)&&
    (((target_mon<MAX_M_IDX)&& m_list[target_mon].ml &&
    (los(char_row,char_col,m_list[target_mon].fy,m_list[target_mon].fx))||
    ((target_mon>=MAX_M_IDX) &&
    (los(char_row,char_col,target_row,target_col)))))) return (TRUE);

    /* The "target" is invalid */
    return (FALSE);
}






/*
 * Set a new target.
 *
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

    /* Go ahead and turn off target mode */
    target_mode = FALSE;

    /* Check monsters first */
    for (m_idx = 0; (m_idx < mfptr) && (!exit_1); m_idx++) {

	monster_type *m_ptr = &m_list[m_idx];

	/* Ignore "unseen" monsters */
	if (!m_ptr->ml ||
		!los(char_row,char_col,m_ptr->fy,m_ptr->fx)) continue;

	/* Access monster location */
	row = m_ptr->fy;
	col = m_ptr->fx;

	    move_cursor_relative(row,col);

	    /* Describe, prompt for recall */
	    sprintf(desc, "%s [(t)arget] [(p)osition] [(r)ecall] [ESC quits]",
		    c_list[m_list[m_idx].mptr].name);
	    prt(desc,0,0);
	    move_cursor_relative(row,col);

	    /* Get a command, processing recall requests */
	    query = inkey();
	    while ((query == 'r') || (query == 'R')) {

		/* Recall on screen */
		save_screen();
		query = roff_recall(m_list[m_idx].mptr);
		restore_screen();

		/* This is done by "good" restore_screen() */
		move_cursor_relative(row, col);
		query = inkey();
	    }


	/* Analyze (non "recall") command */
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

	    case 'P': case 'p':
		exit_1 = TRUE;
	    default:
		break;
	}
    }


    /* Now try a location */
    prt("Use cursor to designate target. [(t)arget]",0,0);

    /* Start on the player */
    row = char_row;
    col = char_col;

    /* Query until done */
    while (TRUE) {

	/* Light up the current location */
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

	/* Verify column */
	if ((col>=cur_width-1) || (col>panel_col_max)) col--;
	else if ((col<=0) || (col<panel_col_min)) col++;

	/* Verify row */
	if ((row>=cur_height-1) || (row>panel_row_max)) row--;
	else if ((row<=0) || (row<panel_row_min)) row++;
    }

    /* Assume no target */
    return (FALSE);
}

#endif /* TARGET */


/*
 * A more "correct" movement calculator.
 */
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





/*
 * Given a direction, apply "confusion" to it
 *
 * Mode is as in "get_a_dir()" below, using:
 *   0x01 = Apply total Confusion
 *   0x02 = ???
 *   0x04 = ???
 *   0x08 = ???
 */
void confuse_dir (int *dir, int mode)
{
    /* Check for confusion */
    if (py.flags.confused > 0) {

	/* Does the confusion get a chance to activate? */
	if ((mode & 0x01) {

	    /* Warn the user */
	    msg_print("You are confused.");

	    /* Pick a random (valid) direction */
	    do {
		*dir = randint(9);
	    } while (dir == 5);
	}
    }
}



/* Prompts for a direction				-RAK-	 */
/* Direction memory added, for repeated commands.  -CJS */
/* This targetting code stolen from Morgul -CFT */
/* Aggle.  Gotta be target mode and either (valid monster and line of sight*/
/* to monster) or (not valid monster and line of sight to position).  CDW */
/* Also, for monster targetting, monster must be lit!  Otherwise player can
   "lock phasers" on an invis monster while a potion of see inv lasts,
   and then continue to hit it when the see inv goes away.  Also,
   targetting mode shouldn't help the player shoot a monster in a
   dark room.  If he can't see it, he shouldn't be able to aim... -CFT
 *
 * Modes are composed of the or-ing of these bit flags:
 *   0x01 =  ??
 *   0x02 = ???
 *   0x04 = Allow the "here" direction ('5')
 *   0x08 = ??? 
 *   0x10 = ???
 *   0x20 = ???
 *   0x40 = ???
 *   0x80 = ???
 */
int get_a_dir(const char *prompt, int *dir, int mode)
{
    char        command;
    char	pbuf[80];
    int         save;
    static char prev_dir;  /* Direction memory. -CJS- */

    if (default_dir) {		   /* used in counted commands. -CJS- */
	*dir = prev_dir;
    }
#ifdef TARGET

    else if (target_okay()) {
      /* It don't get no better than this */
	*dir = 0;
    }
#endif

    else {

	/* Ask until satisfied */    
	while (1) {

	    if (prompt) {
		strcpy(pbuf, prompt);
	    }
	    else {
		sprintf(pbuf, "Which direction?");
	    }

	save = command_rep;	   /* Don't end a counted command. -CJS- */
#ifdef MAC
	    if (!get_comdir(pbuf, &command)) command = ESCAPE;
#else
	    if (!get_com(pbuf, &command)) command = ESCAPE;
#endif
	    if (command == ESCAPE) {
		    free_turn_flag = TRUE;
		    return (FALSE);
		}
		command_rep = save;

	    /* Convert various keys to "standard" keys */
	    switch (command) {

		/* Standard directions */
		case '1': case 'B': case 'b': command = '1'; break;
		case '2': case 'J': case 'j': command = '2'; break;
		case '3': case 'N': case 'n': command = '3'; break;
		case '4': case 'H': case 'h': command = '4'; break;
		case '6': case 'L': case 'l': command = '6'; break;
		case '7': case 'Y': case 'y': command = '7'; break;
		case '8': case 'K': case 'k': command = '8'; break;
		case '9': case 'U': case 'u': command = '9'; break;

		/* Central "direction" often means "target" */
		case '5': case '.': command = '5'; break;
	    }

	    /* Hack -- Perhaps accept '5' as itself */
	    if ((mode & 0x04) && (command == '5')) {
		*dir = 5; break;
	    }


		if (command >= '1' && command <= '9' && command != '5') {
		    prev_dir = command - '0';
		    *dir = prev_dir;
		    break;
		}
	    bell();
	}
    }

    /* Confuse the direction */
    confuse_dir(dir, mode);

    /* A "valid" direction was entered */    
    return (TRUE);
}





/*
 * See "get_a_dir" above
 */
int get_dir(const char *prompt, int *dir)
{
    if (get_a_dir(prompt, dir, 0x0)) return (TRUE);

    /* Command aborted */
    return FALSE;
}


/*
 * Like get_dir(), but if "confused", pick randomly
 */

int get_dir_c(const char *prompt, int *dir)
{
    if (get_a_dir(prompt, dir, 0x1)) return (TRUE);

    /* Command aborted */
    return FALSE;
}





/*
 * Search Mode enhancement				-RAK-
 */
void search_on()
{
    py.flags.speed += 1;
    py.flags.status |= PY_SPEED;
    py.flags.status |= PY_SEARCH;
    prt_state();
    prt_speed();
    py.flags.food_digested++;
}

void search_off(void)
{
    check_view();
    py.flags.speed -= 1;
    py.flags.status |= PY_SPEED;
    py.flags.status &= ~PY_SEARCH;
    prt_state();
    prt_speed();
    py.flags.food_digested--;
}


/*
 * Something happens to disturb the player.		-CJS-
 * The first arg indicates a major disturbance, which affects search.
 *
 * The second arg indicates a light change.
 */
void disturb(int stop_search, int light_change)
{
    command_rep = 0;

    if (stop_search && (py.flags.status & PY_SEARCH)) search_off();

    if (py.flags.rest != 0) rest_off();

    if (light_change || find_flag) {
	find_flag = FALSE;
	check_view();
    }

    flush();
}


/*
 * Searches for hidden things.			-RAK-	
 */
void search(int y, int x, int chance)
{
    register int           i, j;
    register cave_type    *c_ptr;
    register inven_type   *i_ptr;
    register struct flags *p_ptr = &py.flags;
    bigvtype               tmp_str, tmp_str2;

    if ((p_ptr->blind > 0) || no_lite()) chance = chance / 10;
    if (p_ptr->confused > 0) chance = chance / 10;
    if (p_ptr->image > 0) chance = chance / 10;

    /* Search the nearby grids, which are always in bounds */
    for (i = (y - 1); i <= (y + 1); i++) {
	for (j = (x - 1); j <= (x + 1); j++) {

	    /* Sometimes, notice things */
	    if (randint(100) < chance) {

		c_ptr = &cave[i][j];

		/* Nothing there */
		if (c_ptr->tptr == 0) {
		    /* Nothing */
		}

		i_ptr = &i_list[c_ptr->tptr];

		/* Invisible trap? */
		else if (i_ptr->tval == TV_INVIS_TRAP) {
		    objdes(tmp_str2, i_ptr, TRUE);
		    (void)sprintf(tmp_str, "You have found %s.", tmp_str2);
		    msg_print(tmp_str);
		    change_trap(i, j);
		    end_find();
		}

		/* Secret door?	*/
		else if (i_ptr->tval == TV_SECRET_DOOR) {
		    msg_print("You have found a secret door.");

			change_trap(i, j);
			end_find();
		}

		/* Chest?  Trapped?  Known? */
		else if (i_ptr->tval == TV_CHEST) {
		    if ((i_ptr->flags & CH_TRAPPED) > 1) {
			if (!known2_p(i_ptr)) {
			known2(i_ptr);
			msg_print("You have discovered a trap on the chest!");
			} else
			msg_print("The chest is trapped!");
		    }
		}
	    }
	}
    }
}



void rest_off()
{
    py.flags.rest = 0;

    py.flags.status &= ~PY_REST;
    prt_state();

    /* flush last message, or delete "press any key" message */
    msg_print(NULL);

    py.flags.food_digested++;
}





/*
 * Player is on an object.  Many things can happen based -RAK-
 * on the TVAL of the object.  Traps are set off, money and most
 * objects are picked up.  Some objects, such as open doors, just
 * sit there.
 */
void carry(int y, int x, int pickup)
{
    register int         locn, i;
    bigvtype             out_val, tmp_str;
    register cave_type  *c_ptr;
    register inven_type *i_ptr;

    c_ptr = &cave[y][x];
    i_ptr = &i_list[c_ptr->tptr];
    i = i_ptr->tval;

    if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP || i == TV_STORE_DOOR) { hit_trap(y, x); return; }

	end_find();

    /* There's GOLD in them thar hills! */
    if (i_ptr->tval == TV_GOLD) {
	py.misc.au += i_ptr->cost;
	objdes(tmp_str, i_ptr, TRUE);
	(void)sprintf(out_val,
		      "You have found %ld gold pieces worth of %s.",
		      (long)i_ptr->cost, tmp_str);
	prt_gold();
	delete_object(y, x);
	msg_print(out_val);
    }

    else if (i <= TV_MAX_PICK_UP) {

	if (pickup && inven_check_num(i_ptr)) { /* Too many objects? */

	    if (carry_query_flag) {	/* Okay,  pick it up  */

		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val, "Pick up %s? ", tmp_str);
		pickup = get_check(out_val);
	    }
	    
	    /* Check to see if it will change the players speed. */
	    if (pickup && !inven_check_weight(i_ptr)) {
		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val,
				  "Exceed your weight limit to pick up %s? ",
				  tmp_str);
		pickup = get_check(out_val);
	    }

	    /* Attempt to pick up an object. */
	    if (pickup) {
		locn = inven_carry(i_ptr);
		objdes(tmp_str, &inventory[locn], TRUE);
		(void)sprintf(out_val, "You have %s. (%c)",
			      tmp_str, locn + 'a');
		msg_print(out_val);
		delete_object(y, x);
		}
	    } else if (pickup) {   /* only if was trying to pick it up... -CFT */
		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val, "You can't carry %s.", tmp_str);
		msg_print(out_val);
	    }
	}
    }
}




