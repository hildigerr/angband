/* File: moria3.c */ 

/* Purpose: high level command processing */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * An enhanced look, with peripheral vision. Looking all 8	-CJS-
 * directions will see everything which ought to be visible.
 *
 * Can specify direction 5, which looks in all directions. 
 *
 * For the purpose of hindering vision, each place is regarded as a diamond just
 * touching its four immediate neighbours. A diamond is opaque if it is a
 * wall, or shut door, or something like that. A place is visible if any part
 * of its diamond is visible: i.e. there is a line from the view point to
 * part of the diamond which does not pass through any opaque diamonds. 
 *
 * Consider the following situation: 
 *
 * @....			    X	X   X	X   X .##..			   /
 * \ / \ / \ / \ / \ .....			  X @ X . X . X 1 X . X \ / \
 * / \ / \ / \ / X	X   X	X   X Expanded view, with	   / \ / \ /
 * \ / \ / \ diamonds inscribed	  X . X # X # X 2 X . X about each point,	  
 * \ / \ / \ / \ / \ / and some locations	    X	X   X	X   X
 * numbered.		   / \ / \ / \ / \ / \ X . X . X . X 3 X 4 X \ / \ /
 * \ / \ / \ / X	X   X	X   X - Location 1 is fully visible. -
 * Location 2 is visible, even though partially obscured. - Location 3 is
 * invisible, but if either # were transparent, it would be visible. -
 * Location 4 is completely obscured by a single #. 
 *
 * The function which does the work is look_ray. It sets up its own co-ordinate
 * frame (global variables map back to the dungeon frame) and looks for
 * everything between two angles specified from a central line. It is
 * recursive, and each call looks at stuff visible along a line parallel to
 * the center line, and a set distance away from it. A diagonal look uses
 * more extreme peripheral vision from the closest horizontal and vertical
 * directions; horizontal or vertical looks take a call for each side of the
 * central line. 
 */


/*
 * Globally accessed variables: gl_nseen counts the number of places where
 * something is seen. gl_rock indicates a look for rock or objects. 
 *
 * The others map co-ords in the ray frame to dungeon co-ords. 
 *
 * dungeon y = char_row	 + gl_fyx * (ray x)  + gl_fyy * (ray y)
 * dungeon x = char_col	 + gl_fxx * (ray x)  + gl_fxy * (ray y) 
 */
static int gl_fxx, gl_fxy, gl_fyx, gl_fyy;
static int gl_nseen, gl_noquery;
static int gl_rock;

/*
 * Intended to be indexed by dir/2, since is only relevant to horizontal or
 * vertical directions. 
 */
static int set_fxy[] = {0, 1, 0, 0, -1};
static int set_fxx[] = {0, 0, -1, 1, 0};
static int set_fyy[] = {0, 0, 1, -1, 0};
static int set_fyx[] = {0, 1, 0, 0, -1};

/*
 * Map diagonal-dir/2 to a normal-dir/2. 
 */
static int map_diag1[] = {1, 3, 0, 2, 4};
static int map_diag2[] = {2, 1, 0, 4, 3};

/*
 * Any sufficiently big number will do
 */
#define GRADF	10000



/*
 * Look at a monster
 */
static cptr look_mon_desc(int mnum)
{
    monster_type *m_ptr = &m_list[mnum];
    monster_race *r_ptr = &r_list[m_ptr->r_idx];

    bool          living = TRUE;
    int           perc;


    /* Determine if the monster is "living" (vs "undead") */
    if (r_ptr->cflags2 & MF2_UNDEAD) living = FALSE;
    if (r_ptr->cflags2 & MF2_DEMON) living = FALSE;    
    if (strchr("EgvX", r_ptr->r_char)) living = FALSE;    

    if (m_ptr->maxhp == 0) {	   /* then we're just going to fix it! -CFT */
	if ((r_ptr->cflags2 & MF2_MAX_HP) )
	    m_ptr->maxhp = max_hp(r_ptr->hd);
	else
	    m_ptr->maxhp = pdamroll(r_ptr->hd);
    }

    /* Healthy monsters */
    if (m_ptr->hp >= m_ptr->maxhp) {

	/* Paranoia */
	m_ptr->hp = m_ptr->maxhp;

	/* No damage */
	return (living ? "unhurt" : "undamaged");
    }


    /* Notice the "long arithmetic" */
    perc = (100L * m_ptr->hp) / m_ptr->maxhp;

    if (perc > 60) {
	return (living ? "somewhat wounded" : "somewhat damaged");
    }

    if (perc > 25) {
	return (living ? "wounded" : "damaged");
    }

    if (perc > 10) {
	return (living ? "badly wounded" : "badly damaged");
    }

    return (living ? "almost dead" : "almost destroyed");
}




static int look_see(int x, int y, int *transparent)
{
    const char         *dstring,*string;
    char               query = 'a';
    register cave_type *c_ptr;
    register int        i, j;
    bigvtype            out_val, tmp_str;
    inven_type		*i_ptr;

    /* Assume not transparent */
    *transparent = FALSE;

    /* Paranoia */    
    if (x < 0 || y < 0 || y > x) {
	(void)sprintf(tmp_str, "Illegal call to look_see(%d, %d)", x, y);
	msg_print(tmp_str);
	return FALSE;
    }

    /* Default to looking at an object */
    dstring = "You see";

    /* Looking under the player */
    if (!x && !y) dstring = "You are on";

    /* Something */
    j = char_col + gl_fxx * x + gl_fxy * y;
    i = char_row + gl_fyx * x + gl_fyy * y;
    x = j;
    y = i;

    /* Off screen, stop looking, nothing to see */
    if (!panel_contains(y, x)) {
	return FALSE;
    }

    /* Get the cave */
    c_ptr = &cave[y][x];

    /* Floor grids are transparent */
    if (floor_grid_bold(y,x)) *transparent = TRUE;

    /* Hack -- Don't look at a direct line of sight. */
    if (gl_noquery) return FALSE;

    /* Start the description */        
    out_val[0] = 0;

    /* Examine visible monsters */
    if (gl_rock == 0 && c_ptr->m_idx > 1 && m_list[c_ptr->m_idx].ml) {

	j = m_list[c_ptr->m_idx].r_idx;

	if (r_list[j].cflags2 & MF2_UNIQUE) {
	    (void)sprintf(out_val, "%s %s (%s).  [(r)ecall]",
			  dstring,
			  r_list[j].name,
			  look_mon_desc((int)c_ptr->m_idx));
	}
	else {
	    (void)sprintf(out_val, "%s %s %s (%s).  [(r)ecall]",
			  dstring,
			  (is_a_vowel(r_list[j].name[0]) ? "an" : "a"),
			  r_list[j].name,
			  look_mon_desc((int)c_ptr->m_idx));
	}

	dstring = "It is on";
	prt(out_val, 0, 0);
	move_cursor_relative(y, x);
	query = inkey();


	if (query == 'r' || query == 'R') {
		save_screen();
		query = roff_recall(j);
		restore_screen();
	}
    }


    /* Check for illumination */
    if (c_ptr->tl || c_ptr->pl || c_ptr->fm) {

	/* Is there an object there? */
	if (c_ptr->i_idx) {

	    i_ptr = &i_list[c_ptr->i_idx];

	    if (i_ptr->tval == TV_SECRET_DOOR)
		goto granite;

	    /* No rock, yes visible object */
	    if (gl_rock == 0 && i_ptr->tval != TV_INVIS_TRAP) {
		objdes(tmp_str, &i_list[c_ptr->i_idx], TRUE);
		(void)sprintf(out_val, "%s %s.  ---pause---", dstring, tmp_str);
		dstring = "It is in";
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();
	    }
	}

	/* Examine rocks. */
	if ((gl_rock || out_val[0]) && c_ptr->fval >= MIN_CLOSED_SPACE) {
	    switch (c_ptr->fval) {
	      case BOUNDARY_WALL:
	      case GRANITE_WALL:
	granite:
	    /* Granite is only interesting if it contains something. */
		if (out_val[0])
		    string = "a granite wall";
		else
		    string = NULL; /* In case we jump here */
		break;
	      case MAGMA_WALL:
		string = "some dark rock";
		break;
	      case QUARTZ_WALL:
		string = "a quartz vein";
		break;
	      default:
		string = NULL;
		break;
	    }

	    if (string) {
		(void)sprintf(out_val, "%s %s.  ---pause---", dstring, string);
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();
	    }
	}
    }

    if (out_val[0]) {
	gl_nseen++;
	if (query == ESCAPE) return TRUE;
    }

    return FALSE;
}


/*
 * Look at everything within a cone of vision between two ray
 * lines emanating from the player, and y or more places away
 * from the direct line of view. This is recursive.
 *
 * Rays are specified by gradients, y over x, multiplied by
 * 2*GRADF. This is ONLY called with gradients between 2*GRADF
 * (45 degrees) and 1 (almost horizontal).
 *
 * (y axis)/ angle from
 *   ^	  /	    ___ angle to
 *   |	 /	 ___
 *...|../.....___.................... parameter y (look at things in the
 *   | /   ___			      cone, and on or above this line)
 *   |/ ___
 *   @-------------------->   direction in which you are looking. (x axis)
 *   |
 *   | 
 */
static int look_ray(int y, int from, int to)
{
    register int        max_x, x;
    int                 transparent;

/*
 * from is the larger angle of the ray (larger than "to") since
 * we scan towards the center line.  If "from" is smaller than "to",
 * the ray does not exist
 */

    if (from <= to || y > MAX_SIGHT) return FALSE;

/*
 * Find first visible location along this line. Minimum x such that (2x-1)/x
 * < from/GRADF <=> x > GRADF(2x-1)/from. This may be called with y=0 whence
 * x will be set to 0. Thus we need a special fix. 
 */

    x = (long)GRADF * (2 * y - 1) / from + 1;

    if (x <= 0) x = 1;

/*
 * Find last visible location along this line. Maximum x such that (2x+1)/x >
 * to/GRADF <=> x < GRADF(2x+1)/to 
 */

    max_x = ((long)GRADF * (2 * y + 1) - 1) / to;
    if (max_x > MAX_SIGHT) max_x = MAX_SIGHT;
    if (max_x < x) return FALSE;

/*
 * Hack -- gl_noquery prevents doubling up on direct lines of sight. If
 * 'to' is greater than 1, we do not really look at stuff along the
 * direct line of sight, but we do have to see what is opaque for the
 * purposes of obscuring other objects. 
 */

    if ((y == 0 && to > 1) || (y == x && from < GRADF * 2)) {
	gl_noquery = TRUE;
    }
    else {
	gl_noquery = FALSE;
    }

    if (look_see(x, y, &transparent)) {
	return TRUE;
    }

    if (y == x) {
	gl_noquery = FALSE;
    }

    /* Hack */    
    if (transparent) goto init_transparent;

    /* Go until done */
    for (;;) {

	/* Look down the window we've found, allow abort */
	if (look_ray(y + 1, from, (int)((2 * y + 1) * (long)GRADF / x))) {
	    return TRUE;
	}

	/* Find the start of next window. */
	do {

	    /* All done (?) */
	    if (x == max_x) return FALSE;

	    /* See if this seals off the scan. (If y is zero, then it will.) */
	    from = (2 * y - 1) * (long)GRADF / x;
	    if (from <= to) return FALSE;

	    x++;

	    if (look_see(x, y, &transparent)) {
		return TRUE;
	    }
	}
	while (!transparent);

init_transparent:

	/* Find the end of this window of visibility. */
	do {
	    if (x == max_x) {
	    /* The window is trimmed by an earlier limit. */
		return look_ray(y + 1, from, to);
	    }

	    x++;

	    if (look_see(x, y, &transparent)) {
		return TRUE;
	    }
	}
	while (transparent);
    }
}



/*
 * Look at what we can see. This is a free move. 
 *
 * Prompts for a direction, and then looks at every object in turn within a cone
 * of vision in that direction. For each object, the cursor is moved over the
 * object, a description is given, and we wait for the user to type
 * something. Typing ESCAPE will abort the entire look. 
 *
 * Looks first at real objects and monsters, and looks at rock types only after
 * all other things have been seen.  Only looks at rock types if the
 * notice_seams option is set. 
 */
void do_cmd_look()
{
    register int        i, abort_look;
    int                 dir, dummy;

    /* Blind */
    if (p_ptr->blind > 0) {
	msg_print("You can't see a damn thing!");
    }

    /* Hallucinating */
    else if (p_ptr->image > 0) {
	msg_print("You can't believe what you are seeing! It's like a dream!");
    }

    /* Get a direction (or "5"), ignoring target and confusion */
    else if (get_a_dir("Look which direction? ", &dir, 0x04)) {

	abort_look = FALSE;
	gl_nseen = 0;
	gl_rock = 0;
	gl_noquery = FALSE;	   /* Have to set this up for the look_see */

	if (look_see(0, 0, &dummy)) {
	    abort_look = TRUE;
	}
	else {
	    do {
		abort_look = FALSE;

		/* Look at everything */
		if (dir == 5) {

		    for (i = 1; i <= 4; i++) {
			gl_fxx = set_fxx[i];
			gl_fyx = set_fyx[i];
			gl_fxy = set_fxy[i];
			gl_fyy = set_fyy[i];
			if (look_ray(0, 2 * GRADF - 1, 1)) {
			    abort_look = TRUE;
			    break;
			}
			gl_fxy = (-gl_fxy);
			gl_fyy = (-gl_fyy);
			if (look_ray(0, 2 * GRADF, 2)) {
			    abort_look = TRUE;
			    break;
			}
		    }
		}

		/* Straight directions */
		else if ((dir & 1) == 0) {
		    i = dir >> 1;
		    gl_fxx = set_fxx[i];
		    gl_fyx = set_fyx[i];
		    gl_fxy = set_fxy[i];
		    gl_fyy = set_fyy[i];
		    if (look_ray(0, GRADF, 1)) {
			abort_look = TRUE;
		    }
		    else {
			gl_fxy = (-gl_fxy);
			gl_fyy = (-gl_fyy);
			abort_look = look_ray(0, GRADF, 2);
		    }
		}

		/* Diagonals */
		else {
		    i = map_diag1[dir >> 1];
		    gl_fxx = set_fxx[i];
		    gl_fyx = set_fyx[i];
		    gl_fxy = (-set_fxy[i]);
		    gl_fyy = (-set_fyy[i]);
		    if (look_ray(1, 2 * GRADF, GRADF)) {
			abort_look = TRUE;
		    }
		    else {
			i = map_diag2[dir >> 1];
			gl_fxx = set_fxx[i];
			gl_fyx = set_fyx[i];
			gl_fxy = set_fxy[i];
			gl_fyy = set_fyy[i];
			abort_look = look_ray(1, 2 * GRADF - 1, GRADF);
		    }
		}
	    }
	    while (abort_look == FALSE && notice_seams && (++gl_rock < 2));

	    if (abort_look) {
		msg_print("--Aborting look--");
	    }
	    else {
		if (gl_nseen) {
		    if (dir == 5) {
			msg_print("That's all you see.");
		    }
		    else {
			msg_print("That's all you see in that direction.");
		    }
		}
		else if (dir == 5) {
		    msg_print("You see nothing of interest.");
		}
		else {
		    msg_print("You see nothing of interest in that direction.");
		}
	    }
	}
    }
}





/*
 * Support code for the "Locate ourself on the Map" command
 */
void do_cmd_view_map()
{
    /* Free move */
    free_turn_flag = TRUE;

    /* Look at the map */
    screen_map();
}


/*
 * Given an row (y) and col (x), recenter the "panel".
 * The map is reprinted if necessary, and "TRUE" is returned.
 */
static bool do_cmd_locate_aux(int y, int x)
{
    if (!get_panel(y, x, TRUE)) return (FALSE);

    /* Redraw the map */
    prt_map();

    /* The map was redrawn */
    return (TRUE);
}



/*
 * Support code for the "Locate ourself on the Map" command
 */

void do_cmd_locate()
{
    int dir_val, temp;
    int y, x;
    vtype out_val;
    vtype tmp_str;

    int cy, cx, p_y, p_x;


    /* Free move */
    free_turn_flag = TRUE;

    if ((p_ptr->blind > 0) || no_lite()) {
	msg_print("You can't see your map.");
	return;
    }

#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	    temp = target_mode;
	    target_mode = FALSE;
#endif

    /* Save character location */
    y = char_row;
    x = char_col;

    /* Move to a new panel */
    (void)do_cmd_locate_aux(y, x);

    /* Extract (original) panel info */
    cy = panel_row;
    cx = panel_col;


    /* Show panels until done */
    while (1) {

	p_y = panel_row;
	p_x = panel_col;

	/* Describe the location */
	if ((p_y == cy) && (p_x == cx)) {
	    tmp_str[0] = '\0';
	}
	else {
	    (void)sprintf(tmp_str, "%s%s of",
		(p_y < cy) ? " North" : (p_y > cy) ? " South" : "",
		(p_x < cx) ? " West" : (p_x > cx) ? " East" : "");
	}


	/* Prepare to ask which way to look */
	(void)sprintf(out_val,
	    "Map sector [%d,%d], which is%s your sector. Look which direction?",
	    p_y, p_x, tmp_str);

	/* Get a direction (or Escape) */
	if (!get_dir(out_val, &dir_val)) break;


	/* Keep "moving" until the panel changes */
/* -CJS- Should really use the move function, but what the hell. This is nicer,
 * as it moves exactly to the same place in another section. The direction
 * calculation is not intuitive. Sorry.
 */
	while (1) {

	    /* Apply the direction */
	    x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
	    y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;

	    /* No motion off map */
	    if (x < 0 || y < 0 || x >= cur_width || y >= cur_width) {
		msg_print("You've gone past the end of your map.");
		x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
		y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
		break;
	    }

	    /* Hack -- keep sliding until done (?) */
	    if (do_cmd_locate_aux(y, x)) break;
	}
    }


    /* Move to a new panel - but only if really necessary. */
    if (get_panel(char_row, char_col, FALSE))
	prt_map();

#ifdef TARGET
    target_mode = temp; /* restore target mode... */
#endif
}


/*
 * Chests have traps too.
 * Note: Chest traps are based on the FLAGS value
 */
static void chest_trap(int y, int x)
{
    register int        i, j, k;
    register inven_type *i_ptr = &i_list[cave[y][x].i_idx];

    if (i_ptr->flags1 & CH_LOSE_STR) {
	msg_print("A small needle has pricked you!");
	if (!p_ptr->sustain_str) {
	    (void)dec_stat(A_STR);
	    take_hit(damroll(1, 4), "a poison needle");
	    msg_print("You feel weakened!");
	}
	else {
	    msg_print("You are unaffected.");
	}
    }

    if (i_ptr->flags1 & CH_POISON) {
	msg_print("A small needle has pricked you!");
	take_hit(damroll(1, 6), "a poison needle");
	if (!(p_ptr->resist_pois ||
	      p_ptr->oppose_pois ||
	      p_ptr->immune_pois)) {
	    p_ptr->poisoned += 10 + randint(20);
	}
    }

    if (i_ptr->flags1 & CH_PARALYSED) {
	msg_print("A puff of yellow gas surrounds you!");
	if (p_ptr->free_act) {
	    msg_print("You are unaffected.");
	}
	else {
	    msg_print("You choke and pass out.");
	    p_ptr->paralysis = 10 + randint(20);
	}
    }

    if (i_ptr->flags1 & CH_SUMMON) {
	for (i = 0; i < 3; i++) {
	    j = y;
	    k = x;
	    (void)summon_monster(&j, &k, FALSE);
	}
    }

    if (i_ptr->flags1 & CH_EXPLODE) {
	msg_print("There is a sudden explosion!");
	(void)delete_object(y, x);
	take_hit(damroll(5, 8), "an exploding chest");
    }
}





/*
 * Opens a closed door or closed chest.		-RAK-
 * Note that failed opens take time, or ghosts could be found
 * Note unlocking a door is worth one XP, and unlocking a chest
 * is worth as many XP as the chest had "levels".
 */
void do_cmd_open()
{
    int				y, x, i, dir;
    int				flag;
    register cave_type		*c_ptr;
    register inven_type		*i_ptr;
    register monster_type *m_ptr;
    vtype                  m_name, out_val;


    /* Get a direction (or Escape) */
    if (!get_a_dir(NULL, &dir, 0)) {
    }

    else {

	/* Get requested grid */
	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

	/* Get the object (if it exists) */
	i_ptr = &i_list[c_ptr->i_idx];

	/* Nothing is there */
	if ((c_ptr->i_idx == 0) ||
	    ((i_ptr->tval != TV_CLOSED_DOOR) &&
	     (i_ptr->tval != TV_CHEST))) {
	    msg_print("I do not see anything you can open there.");
	    free_turn_flag = TRUE;
	}

	/* Monster in the way */
	else if (c_ptr->m_idx > 1) {

	    m_ptr = &m_list[c_ptr->m_idx];

	    if (m_ptr->ml) {

		if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)
		    (void)sprintf(m_name, "%s", r_list[m_ptr->r_idx].name);
		else
		    (void)sprintf(m_name, "The %s", r_list[m_ptr->r_idx].name);
	    } else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);

	}

	/* Closed door */
	else if (i_ptr->tval == TV_CLOSED_DOOR) {

	    /* Stuck */
	    if (i_ptr->pval < 0) {
		msg_print("It appears to be stuck.");
	    }

	    /* Locked */
	    else if (i_ptr->pval > 0) {

		i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
		    + (class_level_adj[p_ptr->pclass][CLA_DISARM]
		       * p_ptr->lev / 3);

		/* give a 1/50 chance of opening anything, anyway -CWS */
		if ((i - i_ptr->pval) < 2) i = i_ptr->pval + 2;

		if (p_ptr->confused > 0) {
		    msg_print("You are too confused to pick the lock.");
		}
		else if ((i - i_ptr->pval) > randint(100)) {
		    msg_print("You have picked the lock.");
		    p_ptr->exp++;
		    prt_experience();
		    i_ptr->pval = 0;
		}
		else {
		    count_msg_print("You failed to pick the lock.");
		}
	    }

	    /* In any case, if the door is unlocked, open it */
	    if (i_ptr->pval == 0) {

		invcopy(i_ptr, OBJ_OPEN_DOOR);

		/* The door is in a "corridor" */
		c_ptr->fval = CORR_FLOOR;

		/* Draw the door */
		lite_spot(y, x);

		/* Check the view */
		check_view();

		command_rep = 0;
	    }
	}

	/* Open a closed chest. */
	else if (i_ptr->tval == TV_CHEST) {

	    i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) +
		(class_level_adj[p_ptr->pclass][CLA_DISARM] *
		p_ptr->lev / 3);

	    /* Assume opened successfully */
	    flag = TRUE;

	    /* Attempt to unlock it */
	    if (i_ptr->flags1 & CH_LOCKED) {

		/* Assume locked, and thus not open */
		flag = FALSE;

		/* Too confused */
		if (p_ptr->confused > 0) {
		    msg_print("You are too confused to pick the lock.");
		}

		/* Pick the lock, leave the traps */
		else if ((i - (int)i_ptr->level) > randint(100)) {
		    msg_print("You have picked the lock.");
		    p_ptr->exp += i_ptr->level;
		    prt_experience();
		    flag = TRUE;
		}

		else {
		    count_msg_print("You failed to pick the lock.");
		}
	    }

	    /* Allowed to open */
	    if (flag) {

		    i_ptr->flags1 &= ~CH_LOCKED;
		    i_ptr->name2 = EGO_EMPTY;
		    known2(i_ptr);
		    i_ptr->cost = 0;
		}
		flag = FALSE;

	    /* Was chest still trapped?	 (Snicker)   */
		if ((i_ptr->flags1 & CH_LOCKED) == 0) {
		    chest_trap(y, x);
		    if (c_ptr->i_idx != 0)
			flag = TRUE;
		}

	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed.				   */
		if (flag) {
		/*
		 * clear the cursed chest/monster win flag, so that people
		 * can not win by opening a cursed chest 
		 */
		    i_ptr->flags3 &= ~TR3_CURSED;

		/* generate based on level chest was found on - dbd */
		    object_level = i_ptr->pval;

	        /* but let's not get too crazy with storebought chests -CWS */
		    if (i_ptr->ident & ID_STOREBOUGHT) {
			if (object_level > 20)
			    object_level = 20;
		    }

		    if (object_level < 0) /* perform some sanity checking -CWS */
			object_level = 0;
		    if (object_level > MAX_OBJ_LEVEL)
			object_level = MAX_OBJ_LEVEL;

		    coin_type = 0;
		    opening_chest = TRUE; /* don't generate another chest -CWS */
		    (void)monster_death(y, x, i_list[c_ptr->i_idx].flags1, 0, 0);
		    i_list[c_ptr->i_idx].flags1 = 0;
		    opening_chest = FALSE;
		}
	    }
    }
}


/*
 * Close an open door.
 */
void do_cmd_close()
{
    int                    y, x, dir;
    vtype                  out_val, m_name;
    register cave_type    *c_ptr;
    inven_type		  *i_ptr;
    register monster_type *m_ptr;

    /* Get a "desired" direction, or Abort */
    if (!get_a_dir(NULL, &dir, 0)) {
    }

    else {

	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);

	c_ptr = &cave[y][x];
	i_ptr = &i_list[c_ptr->i_idx];

	no_object = FALSE;

	if ((c_ptr->i_idx == 0) ||
	    (i_ptr->tval != TV_OPEN_DOOR)) {

	    msg_print("I do not see anything you can close there.");
	    free_turn_flag = TRUE;
	}

	/* Handle broken doors */
	else if (i_ptr->pval) {
	    msg_print("The door appears to be broken.");
	}

	/* Monster in the way */
		else if (c_ptr->m_idx != 0) {
		    m_ptr = &m_list[c_ptr->m_idx];
		    if (m_ptr->ml) {
			if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)
			    (void)sprintf(m_name, "%s", r_list[m_ptr->r_idx].name);
			else
			    (void)sprintf(m_name, "The %s", r_list[m_ptr->r_idx].name);
		    } else
			(void)strcpy(m_name, "Something");
		    (void)sprintf(out_val, "%s is in your way!", m_name);
		    msg_print(out_val);
		}

	/* Close it */
	else {

	    /* Hack -- kill the old object */
	    i_ptr = &i_list[c_ptr->i_idx];
	    invcopy(i_ptr, OBJ_CLOSED_DOOR);
	    c_ptr->fval = BLOCKED_FLOOR;

	    /* Redisplay */
	    lite_spot(y, x);
	}
    }
}


/*
 * Tunneling through wall
 * Used by TUNNEL and WALL_TO_MUD
 */
int twall(int y, int x, int t1, int t2)
{
    int		i, j;
    int		res, found;
    cave_type	*c_ptr;

    res = FALSE;

    /* Allow chaining of "probability" calls */
    if (t1 > t2) {

	c_ptr = &cave[y][x];


	if (c_ptr->i_idx) { /* secret door or rubble or gold -CFT */
	    if (i_list[c_ptr->i_idx].tval == TV_RUBBLE) {
		delete_object(y,x); /* blow it away... */
		if (randint(10)==1){
		    place_object(y,x); /* and drop a goodie! */
		}
	    }
	    else if (i_list[c_ptr->i_idx].tval >= TV_MIN_DOORS)
		delete_object(y,x); /* no more door... */
	} /* if object there.... */

	c_ptr->fm = FALSE;

	if (panel_contains(y, x))
	    if ((c_ptr->tl || c_ptr->pl) && c_ptr->i_idx != 0) {
		msg_print("You have found something!");
		c_ptr->fm = TRUE;
	    }

	/* should become a room space, check to see whether it should be
	 * LIGHT_FLOOR or DARK_FLOOR */
	if (c_ptr->lr) {

	    found = FALSE;

	    for (i = y - 1; i <= y + 1; i++) {
		for (j = x - 1; j <= x + 1; j++) {

		    if (cave[i][j].fval <= MAX_CAVE_ROOM) {

			/* Steal the floor type */
			c_ptr->fval = cave[i][j].fval;

			c_ptr->pl = cave[i][j].pl;

			found = TRUE;
			break;
		    }
		}
	    }


	/* Otherwise, make it a corridor */
	if (!found) {
	    c_ptr->fval = CORR_FLOOR;
	    c_ptr->pl = FALSE;
	}
	}

	else {
	    /* should become a corridor space */
	    c_ptr->fval = CORR_FLOOR;
	    c_ptr->pl = FALSE;
	}

	/* Redisplay the grid */
	lite_spot(y, x);

	/* Worked */
	res = TRUE;
    }

    return (res);
}


/*
 * Tunnels through rubble and walls			-RAK-
 * Must take into account: secret doors,  special tools
 */
void tunnel(int dir)
{
    register int        i, tabil;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    int                 y, x;
    monster_type       *m_ptr;
    vtype		out_val, m_name;

    if ((p_ptr->confused > 0) && /* Confused?	     */
	(randint(4) > 1))	   /* 75% random movement   */
	dir = randint(9);

	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

/* Compute the digging ability of player; based on	   */
/* strength, and type of tool used			   */
    tabil = p_ptr->use_stat[A_STR];
    i_ptr = &inventory[INVEN_WIELD];

/* Don't let the player tunnel somewhere illegal, this is necessary to
 * prevent the player from getting a free attack by trying to tunnel
 * somewhere where it has no effect.  
 */
    if (c_ptr->fval < MIN_WALL
	&& (c_ptr->i_idx == 0 || (i_list[c_ptr->tptr].tval != TV_RUBBLE
			  && i_list[c_ptr->i_idx].tval != TV_SECRET_DOOR))) {

	if (c_ptr->i_idx == 0) {
	    free_turn_flag = TRUE;
	    msg_print("Tunnel through what?  Empty air?!?");
	} else {
	    msg_print("You can't tunnel through that.");
	    free_turn_flag = TRUE;
	}
	return;
    }

	if (c_ptr->m_idx > 1) {

	    m_ptr = &m_list[c_ptr->m_idx];
	    if (m_ptr->ml) {
		    if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)
				(void)sprintf(m_name, "%s", r_list[m_ptr->r_idx].name);
		    else
				(void)sprintf(m_name, "The %s", r_list[m_ptr->r_idx].name);
	    } else
		    (void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);

	    /* let the player attack the creature */
	    if (p_ptr->afraid < 1) py_attack(y, x);
	    else msg_print("You are too afraid!");
	}

	else if (i_ptr->tval != TV_NOTHING) {

	    if (i_ptr->flags1 & TR1_TUNNEL) {

		tabil += 25 + i_ptr->pval * 50;
	    }

	    else {

		tabil += (i_ptr->damage[0] * i_ptr->damage[1]) + i_ptr->tohit
		+ i_ptr->todam;

		/* divide by two so that digging without shovel isn't too easy */
		tabil >>= 1;
	    }

	    if (weapon_heavy) {
		tabil += (p_ptr->use_stat[A_STR] * 15) - i_ptr->weight;
		if (tabil < 0) tabil = 0;
	    }

	    /* Regular walls; Granite, magma intrusion, quartz vein  */
	    /* Don't forget the boundary walls, made of titanium (255) */

	switch (c_ptr->fval) {

	    case GRANITE_WALL:

		i = randint(1200) + 80;
		if (twall(y, x, tabil, i)) {
			msg_print("You have finished the tunnel.");
			check_view();
		}
		else {
		    count_msg_print("You tunnel into the granite wall.");
		}
	    break;

	    case MAGMA_WALL:

		i = randint(600) + 10;
		if (twall(y, x, tabil, i)) {
			msg_print("You have finished the tunnel.");
			check_view();
		}
		else {
		    count_msg_print("You tunnel into the magma intrusion.");
		}
	    break;

	    case QUARTZ_WALL:

		i = randint(400) + 10;
		if (twall(y, x, tabil, i)) {
			msg_print("You have finished the tunnel.");
			check_view();
		}
		else {
		    count_msg_print("You tunnel into the quartz vein.");
		}
	    break;

	    case BOUNDARY_WALL:
		msg_print("This seems to be permanent rock.");
		break;

	    default:
		/* Is there an object in the way?  (Rubble and secret doors) */
		if (c_ptr->i_idx != 0) {

	    /* Rubble. */
	    if (i_list[c_ptr->i_idx].tval == TV_RUBBLE) {
		if (tabil > randint(180)) {
		    delete_object(y, x);
		    msg_print("You have removed the rubble.");
		    if (randint(10) == 1) {
			place_object(y, x);
			if (test_lite(y, x)) {
			     msg_print("You have found something!");
			}
		    }
		    lite_spot(y, x);
		    check_view();
		} else
		    count_msg_print("You dig in the rubble.");
	    }

	    /* Secret doors. */
	    else if (i_list[c_ptr->i_idx].tval == TV_SECRET_DOOR) {
		count_msg_print("You tunnel into the granite wall.");
		search(char_row, char_col, p_ptr->srh);
	    }

	    else {
		msg_print("You can't tunnel through that.");
		free_turn_flag = TRUE;
	    }
		}
		else {
		msg_print("Tunnel through what?  Empty air?!?");
		free_turn_flag = TRUE;
		}
	    break;
	}
	} else
	msg_print("You dig with your hands, making no progress.");
}


/*
 * Disarms a trap, or chest	-RAK-	
 */
void do_cmd_disarm()
{
    int                 y, x, tmp, dir;
    register int        tot;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    monster_type       *m_ptr;
    vtype                m_name, out_val;


    if (!get_a_dir(NULL, &dir, 0)) {
    }

    else {

	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

	i_ptr = &i_list[c_ptr->i_idx];


	/* Nothing useful there */
	if ((c_ptr->i_idx == 0) ||
	    ((i_ptr->tval != TV_VIS_TRAP) &&
	     (i_ptr->tval != TV_CHEST))) {

	    msg_print("I do not see anything to disarm there.");
	    free_turn_flag = TRUE;
	}

	/* Monster in the way */
	else if (c_ptr->m_idx > 1) {

	    m_ptr = &m_list[c_ptr->m_idx];
	    if (m_ptr->ml)
		(void)sprintf(m_name, "The %s", r_list[m_ptr->r_idx].name);
	    else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);
	}

	/* Normal disarm */
	else {

	    tot = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) +
		  (class_level_adj[p_ptr->pclass][CLA_DISARM] *
		  p_ptr->lev / 3);

	    if ((p_ptr->blind > 0) || (no_lite())) {
		tot = tot / 10;
	    }
	    if (p_ptr->confused > 0) {
		tot = tot / 10;
	    }
	    if (p_ptr->image > 0) {
		tot = tot / 10;
	    }

	    /* Floor trap */
	    if (i_ptr->tval == TV_VIS_TRAP) {

		/* Success */
		if ((tot + 100 - i_ptr->level) > randint(100)) {
		    msg_print("You have disarmed the trap.");
		    p_ptr->exp += i_ptr->pval;
		    delete_object(y, x);
		    /* move the player onto the trap */
		    tmp = p_ptr->confused;
		    p_ptr->confused = 0;
		    move_player(dir, FALSE);
		    p_ptr->confused = tmp;
		    prt_experience();
		}

		/* Keep trying */
		else if ((tot > 5) && (randint(tot) > 5)) {
		    count_msg_print("You failed to disarm the trap.");
		}

		/* Oops */
		else {
		    msg_print("You set the trap off!");
		    /* Move the player onto the trap */
		    tmp = p_ptr->confused;
		    p_ptr->confused = 0;
		    move_player(dir, FALSE);
		    p_ptr->confused += tmp;
		}
	    }

	    /* Disarm chest */
	    else if (i_ptr->tval == TV_CHEST) {

		/* Must find the trap first. */
		if (!known2_p(i_ptr)) {
		    msg_print("I don't see a trap.");
		    free_turn_flag = TRUE;
		}

		/* No traps to find. */
		else if (!(i_ptr->flags1 & CH_TRAPPED)) {
		    msg_print("The chest was not trapped.");
		    free_turn_flag = TRUE;
		}

		/* Successful Disarm */
		else if ((tot - i_ptr->level) > randint(100)) {
		    i_ptr->flags1 &= ~CH_TRAPPED;
		    if (i_ptr->flags1 & CH_LOCKED)
			    i_ptr->name2 = EGO_LOCKED;
		    else
			    i_ptr->name2 = EGO_DISARMED;
		    msg_print("You have disarmed the chest.");
		    known2(i_ptr);
		    p_ptr->exp += i_ptr->level;
		    prt_experience();
		}

		/* Keep trying */
		else if ((tot > 5) && (randint(tot) > 5)) {
		    count_msg_print("You failed to disarm the chest.");
		}

		/* Oops */
		else {
		    msg_print("You set a trap off!");
		    known2(i_ptr);
		    chest_trap(y, x);
		}
		}
	    }
	}
    }
}


/*
 * Bash open a door or chest				-RAK-
 *
 * Note: Affected by strength and weight of character 
 *
 * For a closed door, pval is positive if locked; negative if stuck.
 * A disarm spell unlocks and unjams doors! 
 *
 * For an open door, pval is positive for a broken door. 
 *
 * A closed door can be opened - harder if locked. Any door might be 
 * bashed open (and thereby broken). Bashing a door is (potentially)
 * faster! You move into the door way. To open a stuck door, it must 
 * be bashed. A closed door can be jammed (see do_cmd_spike()).
 *
 * Creatures can also open doors. A creature with open door ability will
 * (if not in the line of sight) move though a closed or secret door with
 * no changes.  If in the line of sight, closed door are openned, & secret
 * door revealed.  Whether in the line of sight or not, such a creature may
 * unlock or unstick a door. 
 *
 * A creature with no such ability will attempt to bash a non-secret door. 
 */
void bash()
{
    int                 y, x, tmp, dir;
    register cave_type  *c_ptr;
    register inven_type *i_ptr;

#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir, so we save
			       target_mode, then turn it off -CFT */
#endif

	/* Bash location */
	y = char_row;
	x = char_col;

#ifdef TARGET
    target_mode = FALSE;
#endif

    if (get_dir_c(NULL, &dir)) {
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

	if (c_ptr->m_idx > 1) {
	    if (p_ptr->afraid > 0) {
		msg_print("You are too afraid!");
	    }
	    else {
		py_bash(y, x);
	    }
	}

	/* Request to bash something */
	else if (c_ptr->i_idx != 0) {

	    /* What is there */
	    i_ptr = &i_list[c_ptr->i_idx];

	    /* Bash a closed door */
	    if (i_ptr->tval == TV_CLOSED_DOOR) {

		count_msg_print("You smash into the door!");

		tmp = p_ptr->use_stat[A_STR] + p_ptr->wt / 2;

		/* Use (roughly) similar method as for monsters. */
		if (randint(tmp * (20 + MY_ABS(i_ptr->pval))) <
			10 * (tmp - MY_ABS(i_ptr->pval))) {

		    msg_print("The door crashes open!");

		    /* Hack -- drop on the old object */
		    invcopy(&i_list[c_ptr->i_idx], OBJ_OPEN_DOOR);

		    /* 50% chance of breaking door */
		    i_ptr->pval = 1 - randint(2);
		    c_ptr->fval = CORR_FLOOR;

		    if (p_ptr->confused == 0)
			move_player(dir, FALSE);
		    else
			lite_spot(y, x);

		    /* Check the view */
		    check_view();
		}

		else if (randint(150) > p_ptr->use_stat[A_DEX]) {
		    msg_print("You are off-balance.");
		    p_ptr->paralysis = 1 + randint(2);
		}

		else if (command_rep == 0)
		    msg_print("The door holds firm.");
	    }

	    /* Semi-Hack -- Bash a Chest */
	    else if (i_ptr->tval == TV_CHEST) {
		if (randint(10) == 1) {
		    msg_print("You have destroyed the chest and its contents!");
		    i_ptr->k_idx = OBJ_RUINED_CHEST;
		    i_ptr->flags1 = 0;
		}
		else if ((CH_LOCKED & i_ptr->flags1) && (randint(10) == 1)) {
		    msg_print("The lock breaks open!");
		    i_ptr->flags1 &= ~CH_LOCKED;
		}
		else {
		    count_msg_print("The chest holds firm.");
		}
	    }

	    else {
	    /*
	     * Can't give free turn, or else player could try directions
	     * until he found invisible creature 
	     */
		msg_print("You bash it, but nothing interesting happens.");
	    }
	}

	else {
	/* Empty Air */
	    if (c_ptr->fval < MIN_WALL) {
	    msg_print("You bash at empty space.");
	}

	/* Walls and secret doors yield same message */
	else {
	    msg_print("You bash it, but nothing interesting happens.");
	}
	}
    }
#ifdef TARGET
    target_mode = temp;
#endif
}


/*
 * Jam a closed door with a spike -RAK-
 */
void do_cmd_spike()
{
    int                  y, x, dir, i, j;
    register cave_type  *c_ptr;
    register inven_type *t_ptr;
    char		tmp_str[80];

#ifdef TARGET
    int temp = target_mode; /* targetting will screw up get_dir.. -CFT */
#endif /* TARGET */

    free_turn_flag = TRUE;
    y = char_row;
    x = char_col;
#ifdef TARGET
    target_mode = FALSE; /* turn off target mode, restore later */
#endif

    if (get_dir(NULL, &dir)) {
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

	if (c_ptr->i_idx != 0) {

	    t_ptr = &i_list[c_ptr->i_idx];

	    if (t_ptr->tval == TV_CLOSED_DOOR)
		if (c_ptr->m_idx == 0) {

		    if (find_range(TV_SPIKE, TV_NEVER, &i, &j)) {

	free_turn_flag = FALSE;

	    /* Successful jamming */
	    count_msg_print("You jam the door with a spike.");

	    /* Make locked to stuck. */
	    if (t_ptr->pval > 0) t_ptr->pval = (-t_ptr->p1);

	    /* Successive spikes have a progressively smaller effect. */
	    /* Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ... */
	    t_ptr->pval -= 1 + 190 / (10 - t_ptr->pval);

	    /* Use up a single spike */
	    inven_item_increase(i, -1);
	    inven_item_optimize(i);

		    } else
			msg_print("But you have no spikes.");
		} else {
		    free_turn_flag = FALSE;
		    (void)sprintf(tmp_str, "The %s is in your way!",
				  r_list[m_list[c_ptr->m_idx].r_idx].name);
		    msg_print(tmp_str);
		}
	    else if (t_ptr->tval == TV_OPEN_DOOR)
		msg_print("The door must be closed first.");
	    else
		msg_print("That isn't a door!");
	} else
	    msg_print("That isn't a door!");
#ifdef TARGET
	target_mode = temp;
#endif
    }
}


/*
 * Throw an object across the dungeon.
 * Flasks of oil do "fire damage" (is this still true?)
 * Extra damage and chance of hitting when missiles are used
 * with correct weapon (xbow + bolt, bow + arrow, sling + shot).
 * Note: Some characters will now get multiple shots per turn -EAM
 */
void do_cmd_fire()
{
    int dir, item_val, tbth, tpth, tdam, tdis;
    int y, x, oldy, oldx, cur_dis;
    int flag, visible;
    int thits, max_shots;
    int ok_throw = FALSE; /* used to prompt user with, so doesn't throw wrong thing */
    bigvtype            out_val, tmp_str;
    inven_type          throw_obj;
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register int        i;
    char                tchar;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	free_turn_flag = TRUE;
    }

    else if (get_item(&item_val, "Fire/Throw which one?", 0, inven_ctr - 1)) {

	inven_type *t = &inventory[item_val];
	
	if ((t->tval == TV_FLASK) || (t->tval == TV_SHOT) ||
	    (t->tval == TV_ARROW) || (t->tval == TV_BOLT) ||
	    (t->tval == TV_SPIKE) || (t->tval == TV_MISC))
	    ok_throw = TRUE;
	else if (((t->tval == TV_FOOD) || (t->tval == TV_POTION1) ||
		  (t->tval == TV_POTION2)) && known1_p(t) &&
		 /* almost all potions do 1d1 damage when thrown.  I want the code
		    to ask before throwing away potions of DEX, *Healing*, etc.
		    This also means it will ask before throwing potions of slow
		    poison, and other low value items that the player is likely to
		    not care about.  This code will mean that mushrooms/molds of
		    unhealth, potions of detonations and death are the only
		    always-throwable food/potions.  (plus known bad ones, in a
		    later test...) -CFT */
		 (t->damage[0] > 1) && (t->damage[1] > 1))
	    ok_throw = TRUE; /* if it's a mushroom or potion that does
                                damage when thrown... */
	else if (!known2_p(t) && (t->ident & ID_DAMD))
	    ok_throw = TRUE;  /* Not IDed, but user knows it's cursed... */
	else if ((t->tval >= TV_MIN_WEAR) && (t->tval <= TV_MAX_WEAR) &&
		 (t->flags3 & TR3_CURSED) && known2_p(t))
	    ok_throw = TRUE; /* if user wants to throw cursed, let him */
	else if ((k_list[t->k_idx].cost <= 0) && known1_p(t) &&
		 !(known2_p(t) && (t->cost > 0)))
	    ok_throw = TRUE;
	else if ((t->cost <= 0) && known2_p(t))
	    ok_throw = TRUE; /* it's junk, let him throw it */
	else if ((t->tval >= TV_HAFTED) &&
		 (t->tval <= TV_DIGGING) && !(t->name2))
	    ok_throw = TRUE; /* non ego/art weapons are okay to just throw, since
				they are damaging (Moral of story: wield your weapon
				if you're worried that you might throw it away!) */
	else { /* otherwise double-check with user before throwing -CFT */
	    objdes(tmp_str, t, TRUE);
	    sprintf(out_val, "Really throw %s?", tmp_str);
	    ok_throw = get_check(out_val);
	}
    } /* if selected an item to throw */

    if (ok_throw) { /* can only be true if selected item, and it either looked
		     * okay, or user said yes... */
	if (get_dir_c(NULL, &dir)) {
	    inven_item_describe(item_val);
	    }
	    max_shots = inventory[item_val].number;
	    inven_throw(item_val, &throw_obj);
	    facts(&throw_obj, &tbth, &tpth, &tdam, &tdis, &thits);
	    if (thits > max_shots)
		thits = max_shots;
	    tchar = throw_obj.tchar;
	/* EAM Start loop over multiple shots */
	    while (thits-- > 0) {
		if (inventory[INVEN_WIELD].sval == 12)
		    tpth -= 10;
		flag = FALSE;
		y = char_row;
		x = char_col;
		oldy = char_row;
		oldx = char_col;
		cur_dis = 0;
		do {
		    (void)mmove(dir, &y, &x);
		    cur_dis++;
		    lite_spot(oldy, oldx);
		    if (cur_dis > tdis)
			flag = TRUE;
		    c_ptr = &cave[y][x];
		    if (floor_grid_bold(y, x) && (!flag)) {
			if (c_ptr->m_idx > 1) {
			    flag = TRUE;
			    m_ptr = &m_list[c_ptr->m_idx];
			    tbth = tbth - cur_dis;
			/* if monster not lit, make it much more difficult to
			 * hit, subtract off most bonuses, and reduce bthb
			 * depending on distance 
			 */
			    if (!m_ptr->ml)
				tbth = (tbth / (cur_dis + 2))
				    - (p_ptr->lev *
				       class_level_adj[p_ptr->pclass][CLA_BTHB] / 2)
				    - (tpth * (BTH_PLUS_ADJ - 1));
			    if (test_hit(tbth, (int)p_ptr->lev, tpth,
				   (int)r_list[m_ptr->r_idx].ac, CLA_BTHB)) {
				i = m_ptr->r_idx;
				objdes(tmp_str, &throw_obj, FALSE);
			    /* Does the player know what he's fighting?	   */
				if (!m_ptr->ml) {
				    (void)sprintf(out_val,
					   "The %s finds a mark.", tmp_str);
				    visible = FALSE;
				} else {
				    if (r_list[i].cflags2 & MF2_UNIQUE)
					(void)sprintf(out_val, "The %s hits %s.",
						   tmp_str, r_list[i].name);
				    else
					(void)sprintf(out_val, "The %s hits the %s.",
						   tmp_str, r_list[i].name);
				    visible = TRUE;
				}
				msg_print(out_val);
				tdam = tot_dam(&throw_obj, tdam, i);
				tdam = critical_blow((int)throw_obj.weight,
						     tpth, tdam, CLA_BTHB);
				if (tdam < 0)
				    tdam = 0;
			    /*
			     * always print fear msgs, so player can stop
			     * shooting -CWS 
			     */
				i = mon_take_hit((int)c_ptr->m_idx, tdam, TRUE);
				if (i < 0) {
				    char                buf[100];
				    char                cdesc[100];
				    if (visible) {
					if (r_list[i].cflags2 & MF2_UNIQUE)
					    sprintf(cdesc, "%s", r_list[m_ptr->r_idx].name);
					else
					    sprintf(cdesc, "The %s", r_list[m_ptr->r_idx].name);
				    } else
					strcpy(cdesc, "It");
				    (void)sprintf(buf,
						  pain_message((int)c_ptr->m_idx,
							       (int)tdam), cdesc);
				    msg_print(buf);
				}
				if (i >= 0) {
				    if (!visible)
					msg_print("You have killed something!");
				    else {
					if (r_list[i].cflags2 & MF2_UNIQUE)
					    (void)sprintf(out_val, "You have killed %s.",
							  r_list[i].name);
					else
					    (void)sprintf(out_val, "You have killed the %s.",
							  r_list[i].name);
					msg_print(out_val);
				    }
				    prt_experience();
				}
				if (stays_when_throw(&throw_obj))
/* should it land on floor?  Or else vanish forever? */
				    drop_throw(oldy, oldx, &throw_obj);
			    }
			    else
				drop_throw(oldy, oldx, &throw_obj);
			}
			else
			{   /* do not test c_ptr->fm here */
			    if (panel_contains(y, x) && (p_ptr->blind < 1)
				&& (c_ptr->tl || c_ptr->pl)) {
				print(tchar, y, x);
				put_qio();	/* show object moving */
#ifdef MSDOS
				delay(8 * delay_spd);	/* milliseconds */
#else
				usleep(8000 * delay_spd);	/* useconds */
#endif
			    }
			}
		    } else {
			flag = TRUE;
			drop_throw(oldy, oldx, &throw_obj);
		    }
		    oldy = y;
		    oldx = x;
		}
		while (!flag);
		if (thits > 0) {   /* triple crossbow check -- not really needed */
		    if (inventory[INVEN_WIELD].sval != 12) {
			(void)sprintf(out_val, "Keep shooting?");
			if (get_check(out_val)) {
			    inven_item_describe(item_val);
			    inven_throw(item_val, &throw_obj);
			} else
			    thits = 0;
		    } else {
			inven_item_describe(item_val);
			inven_throw(item_val, &throw_obj);
		    }
		}

                /* If we're going to fire again, reroll damage for the
                   next missile. This makes each missile's damage more
                   random, AND it doesn't allow damage bonuses to accumulate!
                */
		if (thits > 0) {
                    int dummy; /* ignore everything except tdam */
                    facts(&throw_obj, &dummy, &dummy, &tdam, &dummy, &dummy);
                }
	    }
	} /* EAM end loop over multiple shots */
    }
}


/*
 * Stay still (but check for treasure, traps, re-light, etc)
 */

void do_cmd_stay(int pickup)
{
    /* Actually "move" the character (overkill, but it works) */
    move_player(5, pickup);
}


/*
 * Simple command to "search" for one turn
 */
void do_cmd_search(void)
{
    /* Use the current location, and ability */
    search(char_row, char_col, p_ptr->srh);
}




/*
 * Resting allows a player to safely restore his hp	-RAK-	 
 */
void do_cmd_rest(void)
{
    int   rest_num;
    vtype rest_str;
    char ch;

    if (command_rep > 0) {
	rest_num = command_rep;
	command_rep = 0;
    } else {

	/* Assume no rest */
	rest_num = 0;

	/* Ask the question (perhaps a "prompt" routine would be good) */
	prt("Rest for how long? ('*' for HP/mana; '&' as needed) : ", 0, 0);
	if (get_string(rest_str, 0, 54, 5)) {
	    if (sscanf(rest_str, "%c", &ch) == 1) {
		if (ch == '*') {
		    rest_num = (-1);
		}
		else if (ch == '&') {
		    rest_num = (-2);
		}
		else {
			rest_num = atoi(rest_str);
		    if (rest_num > 30000) rest_num = 30000;
		}
	    }
	}
    }

    /* Induce Rest */
    if (rest_num != 0) {

	search_off();

	p_ptr->rest = rest_num;
	p_ptr->status |= PY_REST;
	prt_state();
	p_ptr->food_digested--;

	prt("Press any key to stop resting...", 0, 0);
	put_qio();
    }

    /* Rest was cancelled */    
    else {
	erase_line(MSG_LINE, 0);
	free_turn_flag = TRUE;
    }    
}



void do_cmd_feeling()
{
    /* No useful feeling in town */
    if (!dun_level) {
	msg_print("You feel there is something special about the town level.");
	return;
    }
    else if (unfelt)
	msg_print("Looks like any other level.");
    else

    switch(feeling) {
      case 0:
	msg_print("Looks like any other level.");
	break;
      case 1:
	msg_print("You feel there is something special about this level.");
	break;
      case 2:
	msg_print("You have a superb feeling about this level.");
	break;
      case 3:
	msg_print("You have an excellent feeling that your luck is turning...");
	break;
      case 4:
	msg_print("You have a very good feeling.");
	break;
      case 5:
	msg_print("You have a good feeling.");
	break;
      case 6:
	msg_print("You feel strangely lucky.");
	break;
      case 7:
	msg_print("You feel your luck is turning...");
	break;
      case 8:
	msg_print("You like the look of this place.");
	break;
      case 9:
	msg_print("This level can't be all bad...");
	break;
      default:
	msg_print("What a boring place...");
	break;
    }
}




/*
 * Append an additional comment to an object description.	-CJS-
 */
void add_inscribe(inven_type *i_ptr, int type)
{
    i_ptr->ident |= (byte) type;
}

/* Replace any existing comment in an object description with a new one. CJS */
void inscribe(inven_type *i_ptr, cptr str)
{
    (void)strcpy(i_ptr->inscrip, str);
}


/*
 * Add a comment to an object description.		-CJS-
 */
void scribe_object(void)
{
    int   item_val, j;
    vtype out_val, tmp_str;

    if (inven_ctr > 0 || equip_ctr > 0) {
	if (get_item(&item_val, "Which one? ", 0, INVEN_ARRAY_SIZE)) {
	    objdes(tmp_str, &inventory[item_val], TRUE);
	    (void)sprintf(out_val, "Inscribing %s.", tmp_str);
	    msg_print(out_val);
	    if (inventory[item_val].inscrip[0] != '\0')
		(void)sprintf(out_val, "Replace \"%s\" with the inscription: ",
			      inventory[item_val].inscrip);
	    else
		(void)strcpy(out_val, "Inscription: ");
	    j = 78 - strlen(tmp_str);
	    if (j > 12)
		j = 12;
	    prt(out_val, 0, 0);
	    if (get_string(out_val, 0, strlen(out_val), j))
		inscribe(&inventory[item_val], out_val);
	}
    } else
	msg_print("You are not carrying anything to inscribe.");
}


/*
 * print out the status of uniques - cba 
 *
 * XXX This routine may induce a blank final screen.
 */
void do_cmd_check_uniques()
{
    int      i, j, k, t;
    bigvtype msg;

    save_screen();

    j = 15;

    for (i = 1; i < 23; i++) erase_line(i, j - 2);

    i = 1;
    prt("Uniques:", i++, j + 5);

    /* Note -- skip the ghost */
    for (k = 0; k < MAX_R_IDX-1; k++) {

	/* Only print Uniques */
	if ((strlen(r_list[k].name) > 0) && (r_list[k].cflags2 & MF2_UNIQUE)) {

	    bool dead = (l_list[k].max_num == 0);

	    /* Wizards know everything, players know kills */
	    if (wizard || dead) {

		/* Print a message */            
		sprintf(msg, "%s is %s.", r_list[k].name,
			dead ? "dead" : "alive");            
		prt(msg, i++, j);

		/* is screen full? */
		if (i == 22) {
		    prt("-- more --", i, j);
		    inkey();
		    for (t = 2; t < 23; t++) erase_line(t, j);
		    prt("Uniques: (continued)", 1, j + 5);
		    i = 2;
		}
	    }
	}
    }

    /* Pause */
    pause_line(i);

    /* Restore the screen */
    restore_screen();
}



