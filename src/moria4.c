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
    for (m_idx = 0; (m_idx < m_max) && (!exit_1); m_idx++) {

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
		    r_list[m_list[m_idx].r_idx].name);
	    prt(desc,0,0);
	    move_cursor_relative(row,col);

	    /* Get a command, processing recall requests */
	    query = inkey();
	    while ((query == 'r') || (query == 'R')) {

		/* Recall on screen */
		save_screen();
		query = roff_recall(m_list[m_idx].r_idx);
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
    if (py.confused > 0) {

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
int get_a_dir(cptr prompt, int *dir, int mode)
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
int get_dir(cptr prompt, int *dir)
{
    if (get_a_dir(prompt, dir, 0x0)) return (TRUE);

    /* Command aborted */
    return FALSE;
}


/*
 * Like get_dir(), but if "confused", pick randomly
 */

int get_dir_c(cptr prompt, int *dir)
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
    if (py.status & PY_SEARCH) return;

    py.speed += 1;
    py.status |= PY_SPEED;
    py.status |= PY_SEARCH;
    prt_state();
    prt_speed();
    py.food_digested++;
}

void search_off(void)
{
    if (!(py.status & PY_SEARCH)) return;

    check_view();
    py.speed -= 1;
    py.status |= PY_SPEED;
    py.status &= ~PY_SEARCH;
    prt_state();
    prt_speed();
    py.food_digested--;
}


/*
 * Something happens to disturb the player.		-CJS-
 * The first arg indicates a major disturbance, which affects search.
 *
 * The second arg indicates a light change.
 *
 * All disturbances cancel "repeated" commands.
 */
void disturb(int stop_search, int light_change)
{
    /* Always cancel repeated commands */
    if (command_rep) command_rep = 0;

    /* Always cancel Rest */
    if (py.rest) rest_off();

    /* Hack -- Cancel Search Mode if requested */
    if (stop_search) search_off();

    if (light_change || find_flag) {
	find_flag = FALSE;
	check_view();
    }
    
    /* flush the input */
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
    player_type *p_ptr = &py;
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
		i_ptr = &i_list[c_ptr->i_idx];

		/* Nothing there */
		if (c_ptr->i_idx == 0) {
		    /* Nothing */
		}

		/* Invisible trap? */
		else if (i_ptr->tval == TV_INVIS_TRAP) {
		    objdes(tmp_str2, i_ptr, TRUE);
		    (void)sprintf(tmp_str, "You have found %s.", tmp_str2);
		    msg_print(tmp_str);
		    i_ptr->tval = TV_VIS_TRAP;
		    lite_spot(i, j);
		    end_find();
		}

		/* Secret door?	*/
		else if (i_ptr->tval == TV_SECRET_DOOR) {
		    msg_print("You have found a secret door.");

		    i_ptr->index = OBJ_CLOSED_DOOR;

		    i_ptr->tval = k_list[OBJ_CLOSED_DOOR].tval;
		    i_ptr->tchar = k_list[OBJ_CLOSED_DOOR].tchar;

		    lite_spot(i, j);

		    end_find();
		}

		/* Chest?  Trapped?  Known? */
		else if (i_ptr->tval == TV_CHEST) {
		    if ((i_ptr->flags1 & CH_TRAPPED) > 1) {
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
    py.rest = 0;

    py.status &= ~PY_REST;
    prt_state();

    /* flush last message, or delete "press any key" message */
    msg_print(NULL);

    py.food_digested++;
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
    i_ptr = &i_list[c_ptr->i_idx];
    i = i_ptr->tval;

	end_find();

    /* Pick up gold */
    if (i_ptr->tval == TV_GOLD) {
	py.au += i_ptr->cost;
	objdes(tmp_str, i_ptr, TRUE);
	(void)sprintf(out_val,
		      "You have found %ld gold pieces worth of %s.",
		      (long)i_ptr->cost, tmp_str);
	prt_gold();
	delete_object(y, x);
	msg_print(out_val);
    }

    /* Can it be picked up? */
    else if (i <= TV_MAX_PICK_UP) {

	/* Pick up the item */
	if (pickup) {

	    /* Too many objects? */
	    if (!inven_check_num(i_ptr)) {
		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val, "You can't carry %s.", tmp_str);
		msg_print(out_val);
		return;
	    }
	    
	    /* Hack -- query every item */
	    else if (carry_query_flag) {	
		objdes(tmp_str, i_ptr, TRUE);
		(void)sprintf(out_val, "Pick up %s? ", tmp_str);
		pickup = get_check(out_val);
	    }
	    
	    /* Check to see if it will change the players speed. */
	    else if (!inven_check_weight(i_ptr)) {
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
	    }
	}
    }
}




/*
 * The cycle lists the directions in anticlockwise order, for over  -CJS- 
 * two complete cycles. The chome array maps a direction on to its position
 * in the cycle. 
 */
static int cycle[] = {1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1};
static int chome[] = {-1, 8, 9, 10, 7, -1, 11, 6, 5, 4};
static int find_openarea, find_breakright, find_breakleft, find_prevdir;
static int find_direction;/* Keep a record of which way we are going. */



/*
 * Hack -- Do we see a wall?  Used in running.		-CJS- 
 */
static int see_wall(int dir, int y, int x)
{
    char c;

    /* check to see if movement there possible */
    if (!mmove(dir, &y, &x)) return (TRUE);

#ifdef MSDOS
    if ((c = loc_symbol(y, x)) == wallsym || c == '%') return (TRUE);
#else
#ifdef ATARIST_MWC
    if ((c = loc_symbol(y, x)) == (unsigned char)240 || c == '%') return (TRUE);
#else
    if ((c = loc_symbol(y, x)) == '#' || c == '%') return (TRUE);
#endif
#endif

    return (FALSE);
}

/*
 * Hack -- Do we see anything? Used in running.		-CJS- 
 */
static int see_nothing(int dir, int y, int x)
{
   /* check to see if movement there possible */
    if (!mmove(dir, &y, &x)) return FALSE;

    if (loc_symbol(y, x) == ' ') return (TRUE);
    
    return (FALSE);
}



/*
 * Determine the next direction for a run, or if we should stop.  -CJS- 
 */
static void area_affect(int dir, int y, int x)
{
    int                  newdir = 0, t, inv, check_dir = 0, row, col;
    register int         i, max, option, option2;

    register cave_type *c_ptr;

    /* We must be able to see... */
    if (py.blind < 1) {

	option = 0;
	option2 = 0;
	dir = find_prevdir;
	max = (dir & 1) + 1;


	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++) {

	    newdir = cycle[chome[dir] + i];
	    row = y;
	    col = x;
	    if (mmove(newdir, &row, &col)) {

	    c_ptr = &cave[row][col];

	    /* Assume the new grid cannot be seen */
	    inv = TRUE;

	    /* Can we "see" (or "remember") the adjacent grid? */
	    if (player_light || c_ptr->tl || c_ptr->pl || c_ptr->fm) {

		/* Most (visible) objects stop the running */
		if (c_ptr->i_idx) {

		    /* Examine the object */
		    t = i_list[c_ptr->i_idx].tval;
		    if ((t != TV_INVIS_TRAP) &&
			(t != TV_SECRET_DOOR) &&
			(t != TV_OPEN_DOOR || !find_ignore_doors)) {

			end_find();
			return;
		    }
		}

	    /* notice visible monsters */
	    /* the monster should be visible since update_mon() checks
	     * for the special case of being in find mode */
	    if (c_ptr->m_idx > 1 && m_list[c_ptr->m_idx].ml) {
		end_find();
		return;
	    }

		/* The grid is "visible" */
		inv = FALSE;
	    }


	    /* If cannot see the grid, assume it is clear */
	    if (inv || floor_grid_bold(row, col)) {

		/* Certain somethings */
		if (find_openarea) {
		    if (i < 0) {
			if (find_breakright) {
			    end_find();
			    return;
			}
		    }
		    else if (i > 0) {
			if (find_breakleft) {
			    end_find();
			    return;
			}
		    }
		}

		/* The first new direction. */
		else if (option == 0) {
		    option = newdir;
		}

		/* Three new directions. Stop running. */
		else if (option2 != 0) {
		    end_find();             
		    return;
		}

		/* If not adjacent to prev, STOP */
		else if (option != cycle[chome[dir] + i - 1]) {
		    end_find();
		    return;
		}

		/* Two adjacent choices. Make option2 the diagonal, */
		/* and remember the other diagonal adjacent to the  */
		/* first option. */
		else {
		    if ((newdir & 1) == 1) {
			check_dir = cycle[chome[dir] + i - 2];
			option2 = newdir;
		    }
		    else {
			check_dir = cycle[chome[dir] + i + 1];
			option2 = option;
			option = newdir;
		    }
		}
	    }

	    /* We see an obstacle.  Break to one side. */
	    /* In open area, STOP if on a side previously open. */
	    else if (find_openarea) {
		if (i < 0) {
		    if (find_breakleft) {
			end_find();
			return;
		    }
		    find_breakright = TRUE;
		}
		else if (i > 0) {
		    if (find_breakright) {
			end_find();
			return;
		    }
		    find_breakleft = TRUE;
		}
	    }
	    }
	}


	/* choose a direction. */
	if (find_openarea == FALSE) {

	    /* There is only one option, or if two, then we always examine */
	    /* potential corners and never cut known corners, so you step */
	    /* into the straight option. */
	    if (option2 == 0 || (find_examine && !find_cut)) {
		if (option != 0) find_direction = option;
		if (option2 == 0) find_prevdir = option;
		else find_prevdir = option2;
	    }

	    /* Two options! */
	    else {

		row = y;
		col = x;
		(void)mmove(option, &row, &col);

		/* Don't see that it is closed off.  This could be a */
		/* potential corner or an intersection.  */
		if (!see_wall(option, row, col) ||
		    !see_wall(check_dir, row, col)) {

		    /* Can not see anything ahead and in the direction we */
		    /* are  turning, assume that it is a potential corner. */
		    if (find_examine && see_nothing(option, row, col) &&
			see_nothing(option2, row, col)) {
			find_direction = option;
			find_prevdir = option2;
		    }

		    /* STOP: we are next to an intersection or a room */
		    else {
			end_find();
		    }
		}

		/* This corner is seen to be enclosed; we cut the corner. */
		else if (find_cut) {
		    find_direction = option2;
		    find_prevdir = option2;
		}

		/* This corner is seen to be enclosed, and we */
		/* deliberately go the long way. */
		else {
		    find_direction = option;
		    find_prevdir = option2;
		}
	    }
	}
    }
}




/*
 * Moves player from one space to another. -RAK-
 *
 * Note that "moving" into a wall is a free move, and will NOT hit any monster
 * which is "hiding" in the walls.  The player must tunnel into the wall.
 * Otherwise, moving into a wall would have to always take a turn.
 * In fact, moving into a wall will not hit ANY monster in the wall.
 * After all, the monster has the entire wall as protection!
 */
void move_player(int dir, int do_pickup)
{
    int                 y, x;
    register cave_type *c_ptr;
    register inven_type	*i_ptr;

    /* Remember if the player was running */
    bool was_running = find_flag;

    /* Save info for dealing with traps and such */
    int old_row = char_row;
    int old_col = char_col;

    if (((py.confused > 0) || (py.stun > 0)) &&	/* Confused/Stunned?  */
    (randint(4) > 1) &&	   /* 75% random movement */
    (dir != 5)) {		   /* Never random if sitting */
	    dir = randint(9);
	    end_find();
    }


    /* Find the result of moving */
    y = char_row;
    x = char_col;
    if (mmove(dir, &y, &x)) {	   /* Legal move? */

    /* Examine the destination */    
    c_ptr = &cave[y][x];

    /* Player can only walk on floors */
    if (!floor_grid_bold(y,x)) {

	/* Get the "object" if any */
	i_ptr = &i_list[c_ptr->i_idx];

	/* Notice non-walls unless starting to "run" */
	if (!was_running && (c_ptr->i_idx)) {

	    /* Rubble */
	    if (i_ptr->tval == TV_RUBBLE) {
		msg_print("There is rubble blocking your way.");
	    }

	    /* Closed doors */
	    else if (i_ptr->tval == TV_CLOSED_DOOR) {
		msg_print("There is a closed door blocking your way.");
	    }
	}
	    else end_find();

	/* Free move */
	free_turn_flag = TRUE;
    }

    /* Attacking a creature! */
    else if (c_ptr->m_idx > 1) {

	/* Hitting a monster is disturbing */
	end_find();

		    /* if player can see monster, and was in find mode, then nothing */
		    if (was_running && m_list[c_ptr->m_idx].ml) {
			    /* did not do anything this turn */
			    free_turn_flag = TRUE;
		    }
		    else {
			    /* Handle fear */
			    if (py.afraid < 1)
				    py_attack(y, x);
			    else
				    msg_print("You are too afraid!");
		    }
    }

    /* Normal movement */
    else {

		    /* Make final assignments of char co-ords */
		    char_row = y;
		    char_col = x;

	/* Move "player" record to the new location */
	move_rec(old_row, old_col, char_row, char_col);

	/* Check for new panel (redraw map) */
	if (get_panel(char_row, char_col, FALSE)) prt_map();

	/* Check to see if he should stop running */
	if (find_flag) area_affect(dir, char_row, char_col);


	/* Check to see if he notices something  */
	/* "fos" may be negative if have good rings of searching */
	if ((py.fos <= 1) || (randint(py.fos) == 1) ||
	(py.status & PY_SEARCH))
	    search(char_row, char_col, py.srh);

		    /* A room of light should be lit. */
		    if ((c_ptr->fval == LIGHT_FLOOR) ||
		    (c_ptr->fval == NT_LIGHT_FLOOR)) {
			    if (!c_ptr->pl && !py.blind) light_room(char_row, char_col);
		    }

		    /* In doorway of light-room? */
		    else if (c_ptr->lr && (py.blind < 1)) {
			    register int        i, j;

			    byte lit = FALSE;	/* only call light_room once... -CFT */

			    for (i = (char_row - 1); !lit && i <= (char_row + 1); i++)
			    for (j = (char_col - 1); !lit && j <= (char_col + 1); j++) {

				    if (((cave[i][j].fval == LIGHT_FLOOR) ||
				    (cave[i][j].fval == NT_LIGHT_FLOOR)) &&
				    (!cave[i][j].pl)) {
					    /* move light 1st, or corridor may be perm lit */
					    move_light(old_row, old_col, char_row, char_col);
					    light_room(char_row, char_col);
					    lit = TRUE;	/* okay, we can stop now... -CFT */
				    }
			    }
		    }

		    /* Move the light source */
		    move_light(old_row, old_col, char_row, char_col);

	/* An object is beneath him. */
	if (c_ptr->i_idx != 0) {

	    /* Get the object */            
	    i_ptr = &i_list[c_ptr->i_idx];

	    /* Pre-handle open doors and stairs */
	    if ((i_ptr->tval == TV_UP_STAIR) ||
		(i_ptr->tval == TV_DOWN_STAIR)) {
		/* Nothing */
	    }

	    /* Pre-handle open doors */
	    else if (i_ptr->tval == TV_OPEN_DOOR) {
		/* Nothing */
	    }

	    /* Set off a trap */
	    else if ((i_ptr->tval == TV_VIS_TRAP) ||
		     (i_ptr->tval == TV_INVIS_TRAP)) {
		hit_trap(char_row, char_col);
	    }

	    /* Enter a store */
	    else if (i_ptr->tval == TV_STORE_DOOR) {
		 enter_store(i_ptr->sval - 101);
	    }

	    /* Note that we only carry things that can be "carried" */
	    else if (i_ptr->tval == TV_GOLD || !prompt_carry_flag) {
		carry(char_row, char_col, do_pickup);
	    }

	    /* Inform the user he could have carried it */
	    else if (prompt_carry_flag) {
		bigvtype            tmp_str, tmp2_str;
		objdes(tmp_str, i_ptr, TRUE);
		sprintf(tmp2_str, "You see %s.", tmp_str);
		msg_print(tmp2_str);
	    }


	    /* Get the object */            
	    i_ptr = &i_list[c_ptr->i_idx];

	    /* Hack -- if stepped on falling rock trap, the space will */
	    /* now contain rubble, so step back into a clear area */

	    /* Back away from rubble. */
	    if (i_ptr->tval == TV_RUBBLE) {

		/* Move back to the old location */
		move_rec(char_row, char_col, old_row, old_col);

				    move_light(char_row, char_col, old_row, old_col);

				    char_row = old_row;
				    char_col = old_col;

				    /* check to see if we have stepped back onto another trap, if so, set it off */
				    c_ptr = &cave[char_row][char_col];
				    if (c_ptr->i_idx != 0) {
					    if (i_ptr->tval == TV_INVIS_TRAP || i_ptr->tval == TV_VIS_TRAP
					    || i_ptr->tval == TV_STORE_DOOR)
						    hit_trap(char_row, char_col);
			    }
			    }
		    }
	    }

    }
}






/* The running algorithm:			-CJS- */


/*
   Overview: You keep moving until something interesting happens.
   If you are in an enclosed space, you follow corners. This is
   the usual corridor scheme. If you are in an open space, you go
   straight, but stop before entering enclosed space. This is
   analogous to reaching doorways. If you have enclosed space on
   one side only (that is, running along side a wall) stop if
   your wall opens out, or your open space closes in. Either case
   corresponds to a doorway.

   What happens depends on what you can really SEE. (i.e. if you
   have no light, then running along a dark corridor is JUST like
   running in a dark room.) The algorithm works equally well in
   corridors, rooms, mine tailings, earthquake rubble, etc, etc.

   These conditions are kept in static memory:
	find_openarea	 You are in the open on at least one
			 side.
	find_breakleft	 You have a wall on the left, and will
			 stop if it opens
	find_breakright	 You have a wall on the right, and will
			 stop if it opens

   To initialize these conditions is the task of find_init. If
   moving from the square marked @ to the square marked . (in the
   two diagrams below), then two adjacent sqares on the left and
   the right (L and R) are considered. If either one is seen to
   be closed, then that side is considered to be closed. If both
   sides are closed, then it is an enclosed (corridor) run.

	 LL		L
	@.	       L.R
	 RR	       @R

   Looking at more than just the immediate squares is
   significant. Consider the following case. A run along the
   corridor will stop just before entering the center point,
   because a choice is clearly established. Running in any of
   three available directions will be defined as a corridor run.
   Note that a minor hack is inserted to make the angled corridor
   entry (with one side blocked near and the other side blocked
   further away from the runner) work correctly. The runner moves
   diagonally, but then saves the previous direction as being
   straight into the gap. Otherwise, the tail end of the other
   entry would be perceived as an alternative on the next move.

	   #.#
	  ##.##
	  .@...
	  ##.##
	   #.#

   Likewise, a run along a wall, and then into a doorway (two
   runs) will work correctly. A single run rightwards from @ will
   stop at 1. Another run right and down will enter the corridor
   and make the corner, stopping at the 2.

	#@	  1
	########### ######
	2	    #
	#############
	#

   After any move, the function area_affect is called to
   determine the new surroundings, and the direction of
   subsequent moves. It takes a location (at which the runner has
   just arrived) and the previous direction (from which the
   runner is considered to have come). Moving one square in some
   direction places you adjacent to three or five new squares
   (for straight and diagonal moves) to which you were not
   previously adjacent.

       ...!	  ...	       EG Moving from 1 to 2.
       .12!	  .1.!		  . means previously adjacent
       ...!	  ..2!		  ! means newly adjacent
		   !!!

   You STOP if you can't even make the move in the chosen
   direction. You STOP if any of the new squares are interesting
   in any way: usually containing monsters or treasure. You STOP
   if any of the newly adjacent squares seem to be open, and you
   are also looking for a break on that side. (i.e. find_openarea
   AND find_break) You STOP if any of the newly adjacent squares
   do NOT seem to be open and you are in an open area, and that
   side was previously entirely open.

   Corners: If you are not in the open (i.e. you are in a
   corridor) and there is only one way to go in the new squares,
   then turn in that direction. If there are more than two new
   ways to go, STOP. If there are two ways to go, and those ways
   are separated by a square which does not seem to be open, then
   STOP.

   Otherwise, we have a potential corner. There are two new open
   squares, which are also adjacent. One of the new squares is
   diagonally located, the other is straight on (as in the
   diagram). We consider two more squares further out (marked
   below as ?).
	  .X
	 @.?
	  #?
   If they are both seen to be closed, then it is seen that no
   benefit is gained from moving straight. It is a known corner.
   To cut the corner, go diagonally, otherwise go straight, but
   pretend you stepped diagonally into that next location for a
   full view next time. Conversely, if one of the ? squares is
   not seen to be closed, then there is a potential choice. We check
   to see whether it is a potential corner or an intersection/room entrance.
   If the square two spaces straight ahead, and the space marked with 'X'
   are both blank, then it is a potential corner and enter if find_examine
   is set, otherwise must stop because it is not a corner.
*/



void find_step(void)
{
    /* Hack -- prevent infinite running */
    if (find_flag++ > 100) {
	msg_print("You stop running to catch your breath.");
	end_find();
    }

    else {
	move_player(find_direction, TRUE);
    }
}


void find_init(int dir)
{
    int          row, col, deepleft, deepright;
    register int i, shortleft, shortright;

    darken_player(char_row, char_col);
    old_lite = cur_lite;
    if (cur_lite >= 0)
	cur_lite = 1;

    row = char_row;
    col = char_col;
    if (!mmove(dir, &row, &col)) find_flag = FALSE;

    else {
	find_direction = dir;
	find_flag = 1;
	find_breakright = find_breakleft = FALSE;
	find_prevdir = dir;
	if (py.blind < 1) {
	    i = chome[dir];
	    deepleft = deepright = FALSE;
	    shortright = shortleft = FALSE;
	    if (see_wall(cycle[i + 1], char_row, char_col)) {
		find_breakleft = TRUE;
		shortleft = TRUE;
	    }
	    else if (see_wall(cycle[i + 1], row, col)) {
		find_breakleft = TRUE;
		deepleft = TRUE;
	    }
	    if (see_wall(cycle[i - 1], char_row, char_col)) {
		find_breakright = TRUE;
		shortright = TRUE;
	    }
	    else if (see_wall(cycle[i - 1], row, col)) {
		find_breakright = TRUE;
		deepright = TRUE;
	    }
	    if (find_breakleft && find_breakright) {
		find_openarea = FALSE;
		/* a hack to allow angled corridor entry */
		if (dir & 1) {
		    if (deepleft && !deepright)
			find_prevdir = cycle[i - 1];
		    else if (deepright && !deepleft)
			find_prevdir = cycle[i + 1];
		}
	    /* else if there is a wall two spaces ahead and seem to be in a
	     * corridor, then force a turn into the side corridor, must be
	     * moving straight into a corridor here 
	     */
		else if (see_wall(cycle[i], row, col)) {
		    if (shortleft && !shortright)
			find_prevdir = cycle[i - 2];
		    else if (shortright && !shortleft)
			find_prevdir = cycle[i + 2];
		}
	    }
	    else {
		find_openarea = TRUE;
	    }
	}
    }

/*
 * We must erase the player symbol '@' here, because sub3_move_light() does
 * not erase the previous location of the player when in find mode and when
 * find_prself is FALSE.  The player symbol is not draw at all in this case
 * while moving, so the only problem is on the first turn of find mode, when
 * the initial position of the character must be erased. Hence we must do the
 * erasure here.  
 */
    if (!light_flag && !find_prself)
#ifdef TC_COLOR
	lite_spot(char_row, char_col);
#else
	print(loc_symbol(char_row, char_col), char_row, char_col);
#endif

    move_player(dir, TRUE);
    if (find_flag == FALSE) command_rep = 0;
}


/*
 * Stop running.  Hack -- fix the lights.
 */
void end_find()
{
    /* Were we running? */
    if (find_flag) {

	find_flag = FALSE;

	cur_lite = old_lite;

	move_light(char_row, char_col, char_row, char_col);
    }
}





