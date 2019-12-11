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
static const char *look_mon_desc(int mnum)
{
    monster_type *m_ptr = &m_list[mnum];
    monster_race *r_ptr = &c_list[m_ptr->mptr];

    bool          living = TRUE;
    int           perc;


    /* Determine if the monster is "living" (vs "undead") */
    if (r_ptr->cdefense & UNDEAD) living = FALSE;
    if (r_ptr->cdefense & DEMON) living = FALSE;    
    if (strchr("EgvX", r_ptr->cchar)) living = FALSE;    

    if (m_ptr->maxhp == 0) {	   /* then we're just going to fix it! -CFT */
	if ((r_ptr->cdefense & MAX_HP) )
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
    if (gl_rock == 0 && c_ptr->cptr > 1 && m_list[c_ptr->cptr].ml) {

	j = m_list[c_ptr->cptr].mptr;

	if (c_list[j].cdefense & UNIQUE) {
	    (void)sprintf(out_val, "%s %s (%s).  [(r)ecall]",
			  dstring,
			  c_list[j].name,
			  look_mon_desc((int)c_ptr->cptr));
	}
	else {
	    (void)sprintf(out_val, "%s %s %s (%s).  [(r)ecall]",
			  dstring,
			  (is_a_vowel(c_list[j].name[0]) ? "an" : "a"),
			  c_list[j].name,
			  look_mon_desc((int)c_ptr->cptr));
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
	if (c_ptr->tptr != 0) {

	    if (i_list[c_ptr->tptr].tval == TV_SECRET_DOOR)
		goto granite;

	    /* No rock, yes visible object */
	    if (gl_rock == 0 && i_list[c_ptr->tptr].tval != TV_INVIS_TRAP) {
		objdes(tmp_str, &i_list[c_ptr->tptr], TRUE);
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
    if (py.flags.blind > 0) {
	msg_print("You can't see a damn thing!");
    }

    /* Hallucinating */
    else if (py.flags.image > 0) {
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
 * Chests have traps too. -RAK-
 * Note: Chest traps are based on the FLAGS value
 */
static void chest_trap(int y, int x)
{
    register int        i, j, k;
    register inven_type *i_ptr = &i_list[cave[y][x].tptr];

    if (i_ptr->flags & CH_LOSE_STR) {
	msg_print("A small needle has pricked you!");
	if (!py.flags.sustain_str) {
	    (void)dec_stat(A_STR);
	    take_hit(damroll(1, 4), "a poison needle");
	    msg_print("You feel weakened!");
	}
	else {
	    msg_print("You are unaffected.");
	}
    }

    if (i_ptr->flags & CH_POISON) {
	msg_print("A small needle has pricked you!");
	take_hit(damroll(1, 6), "a poison needle");
	if (!(py.flags.resist_pois ||
	      py.flags.oppose_pois ||
	      py.flags.immune_pois)) {
	    py.flags.poisoned += 10 + randint(20);
	}
    }

    if (i_ptr->flags & CH_PARALYSED) {
	msg_print("A puff of yellow gas surrounds you!");
	if (py.flags.free_act) {
	    msg_print("You are unaffected.");
	}
	else {
	    msg_print("You choke and pass out.");
	    py.flags.paralysis = 10 + randint(20);
	}
    }

    if (i_ptr->flags & CH_SUMMON) {
	for (i = 0; i < 3; i++) {
	    j = y;
	    k = x;
	    (void)summon_monster(&j, &k, FALSE);
	}
    }

    if (i_ptr->flags & CH_EXPLODE) {
	msg_print("There is a sudden explosion!");
	(void)delete_object(y, x);
	take_hit(damroll(5, 8), "an exploding chest");
    }
}





/*
 * Opens a closed door or closed chest.		-RAK-
 */
void do_cmd_open()
{
    int				y, x, i, dir;
    int				flag;
    register cave_type		*c_ptr;
    register inven_type		*i_ptr;
    register struct misc  *p_ptr = &py.misc;
    register monster_type *m_ptr;
    vtype                  m_name, out_val;


    if (!get_a_dir(NULL, &dir, 0)) {
    }

    else {

	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);
	c_ptr = &cave[y][x];

	i_ptr = &i_list[c_ptr->tptr];


	if ((c_ptr->tptr == 0) ||
	    ((i_ptr->tval != TV_CLOSED_DOOR) &&
	     (i_ptr->tval != TV_CHEST))) {
	    msg_print("I do not see anything you can open there.");
	    free_turn_flag = TRUE;
	}

	else if (c_ptr->cptr > 1) {

	    m_ptr = &m_list[c_ptr->cptr];

	    if (m_ptr->ml) {

		if (c_list[m_ptr->mptr].cdefense & UNIQUE)
		    (void)sprintf(m_name, "%s", c_list[m_ptr->mptr].name);
		else
		    (void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
	    } else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);

	}

	/* Closed door */
	else if (i_ptr->tval == TV_CLOSED_DOOR) {

	    /* It's stuck */
	    if (i_ptr->p1 < 0) {
		msg_print("It appears to be stuck.");
	    }

	    else if (i_ptr->p1 > 0) {

		i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
		    + (class_level_adj[p_ptr->pclass][CLA_DISARM]
		       * p_ptr->lev / 3);

		/* give a 1/50 chance of opening anything, anyway -CWS */
		if ((i - i_ptr->p1) < 2) i = i_ptr->p1 + 2;

		if (py.flags.confused > 0) {
		    msg_print("You are too confused to pick the lock.");
		}
		else if ((i - i_ptr->p1) > randint(100)) {
		    msg_print("You have picked the lock.");
		    py.misc.exp++;
		    prt_experience();
		    i_ptr->p1 = 0;
		}
		else {
		    count_msg_print("You failed to pick the lock.");
		}
	    }

	    if (i_ptr->p1 == 0) {

		invcopy(i_ptr, OBJ_OPEN_DOOR);

		c_ptr->fval = CORR_FLOOR;

		lite_spot(y, x);

		check_view();

		command_rep = 0;
	    }
	}

	/* Open a closed chest. */
	else if (i_ptr->tval == TV_CHEST) {

	    i = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) +
		(class_level_adj[p_ptr->pclass][CLA_DISARM] *
		p_ptr->lev / 3);

	    flag = TRUE;

	    if (i_ptr->flags & CH_LOCKED) {

		flag = FALSE;

		if (py.flags.confused > 0) {
		    msg_print("You are too confused to pick the lock.");
		}

		else if ((i - (int)i_ptr->level) > randint(100)) {
		    msg_print("You have picked the lock.");
		    py.misc.exp += i_ptr->level;
		    prt_experience();
		    flag = TRUE;
		}

		else {
		    count_msg_print("You failed to pick the lock.");
		}
	    }

	    if (flag) {

		    i_ptr->flags &= ~CH_LOCKED;
		    i_ptr->name2 = EGO_EMPTY;
		    known2(i_ptr);
		    i_ptr->cost = 0;
		}
		flag = FALSE;

	    /* Was chest still trapped?	 (Snicker)   */
		if ((i_ptr->flags & CH_LOCKED) == 0) {
		    chest_trap(y, x);
		    if (c_ptr->tptr != 0)
			flag = TRUE;
		}

	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed.				   */
		if (flag) {
		/*
		 * clear the cursed chest/monster win flag, so that people
		 * can not win by opening a cursed chest 
		 */
		    i_ptr->flags &= ~TR_CURSED;

		/* generate based on level chest was found on - dbd */
		    object_level = i_ptr->p1;

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
		    (void)monster_death(y, x, i_list[c_ptr->tptr].flags, 0, 0);
		    i_list[c_ptr->tptr].flags = 0;
		    opening_chest = FALSE;
		}
	    }
    }
}


/*
 * Closes an open door. -RAK-
 */
void do_cmd_close()
{
    int                    y, x, dir;
    vtype                  out_val, m_name;
    register cave_type    *c_ptr;
    inven_type		  *i_ptr;
    register monster_type *m_ptr;


    if (!get_a_dir(NULL, &dir, 0)) {
    }

    else {

	y = char_row;
	x = char_col;
	(void)mmove(dir, &y, &x);

	c_ptr = &cave[y][x];
	i_ptr = &i_list[c_ptr->tptr];

	no_object = FALSE;

	if ((c_ptr->tptr == 0) ||
	    (i_ptr->tval != TV_OPEN_DOOR)) {

	    msg_print("I do not see anything you can close there.");
	    free_turn_flag = TRUE;
	}

		else if (c_ptr->cptr == 0)
		    if (i_ptr->p1 == 0) {
			invcopy(&i_list[c_ptr->tptr], OBJ_CLOSED_DOOR);
			c_ptr->fval = BLOCKED_FLOOR;
			lite_spot(y, x);
		    } else
			msg_print("The door appears to be broken.");
		else {
		    m_ptr = &m_list[c_ptr->cptr];
		    if (m_ptr->ml) {
			if (c_list[m_ptr->mptr].cdefense & UNIQUE)
			    (void)sprintf(m_name, "%s", c_list[m_ptr->mptr].name);
			else
			    (void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
		    } else
			(void)strcpy(m_name, "Something");
		    (void)sprintf(out_val, "%s is in your way!", m_name);
		    msg_print(out_val);
		}
    }
}


/*
 * Disarms a trap -RAK-	
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

	i_ptr = &i_list[c_ptr->tptr];


	if ((c_ptr->tptr == 0) ||
	    ((i_ptr->tval != TV_VIS_TRAP) &&
	     (i_ptr->tval != TV_CHEST))) {

	    msg_print("I do not see anything to disarm there.");
	    free_turn_flag = TRUE;
	}

	else if (c_ptr->cptr > 1) {

	    m_ptr = &m_list[c_ptr->cptr];
	    if (m_ptr->ml)
		(void)sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
	    else
		(void)strcpy(m_name, "Something");
	    (void)sprintf(out_val, "%s is in your way!", m_name);
	    msg_print(out_val);
	}

	else {

	    tot = py.misc.disarm + 2 * todis_adj() + stat_adj(A_INT) +
		  (class_level_adj[py.misc.pclass][CLA_DISARM] *
		  py.misc.lev / 3);

	    if ((py.flags.blind > 0) || (no_lite())) {
		tot = tot / 10;
	    }
	    if (py.flags.confused > 0) {
		tot = tot / 10;
	    }
	    if (py.flags.image > 0) {
		tot = tot / 10;
	    }

	    /* Floor trap */
	    if (i_ptr->tval == TV_VIS_TRAP) {

		if ((tot + 100 - i_ptr->level) > randint(100)) {
		    msg_print("You have disarmed the trap.");
		    py.misc.exp += i_ptr->p1;
		    delete_object(y, x);
		    /* make sure we move onto the trap even if confused */
		    tmp = py.flags.confused;
		    py.flags.confused = 0;
		    move_player(dir, FALSE);
		    py.flags.confused = tmp;
		    prt_experience();
		}

		/* avoid randint(0) call */
		else if ((tot > 5) && (randint(tot) > 5)) {
		    count_msg_print("You failed to disarm the trap.");
		}

		else {
		    msg_print("You set the trap off!");
		    /* make sure we move onto the trap even if confused */
		    tmp = py.flags.confused;
		    py.flags.confused = 0;
		    move_player(dir, FALSE);
		    py.flags.confused += tmp;
		}
	    }

	    else if (i_ptr->tval == TV_CHEST) {

		if (!known2_p(i_ptr)) {
		    msg_print("I don't see a trap.");
		    free_turn_flag = TRUE;
		}

		else if (i_ptr->flags & CH_TRAPPED) {
		if ((tot - i_ptr->level) > randint(100)) {
		    i_ptr->flags &= ~CH_TRAPPED;
		    if (i_ptr->flags & CH_LOCKED)
			    i_ptr->name2 = EGO_LOCKED;
		    else
			    i_ptr->name2 = EGO_DISARMED;
		    msg_print("You have disarmed the chest.");
		    known2(i_ptr);
		    py.misc.exp += i_ptr->level;
		    prt_experience();
		}

		else if ((tot > 5) && (randint(tot) > 5)) {
		    count_msg_print("You failed to disarm the chest.");
		}

		else {
		    msg_print("You set a trap off!");
		    known2(i_ptr);
		    chest_trap(y, x);
		}
		}

		else {
		    msg_print("The chest was not trapped.");
		    free_turn_flag = TRUE;
		}
	    }
	}
    }
}


