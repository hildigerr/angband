/* File: spells.c */

/* Purpose: player and creature spells, breaths, wands, scrolls, etc. */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

/* Following are spell procedure/functions			-RAK-	 */
/* These routines are commonly used in the scroll, potion, wands, and	 */
/* staves routines, and are occasionally called from other areas.	  */
/* Now included are creature spells also.		       -RAK    */


/* this fn only exists to avoid duplicating this code in the selfknowledge fn. -CFT */
static void pause_if_screen_full(int *i, int j)
{
    int t;

    if (*i == 22) {		   /* is screen full? */
	prt("-- more --", *i, j);
	inkey();
	for (t = 2; t < 23; t++)
	    erase_line(t, j);	   /* don't forget to erase extra */
	prt("Your Attributes: (continued)", 1, j + 5);
	*i = 2;
    }
}


void monster_name(char *m_name, monster_type *m_ptr, monster_race      *r_ptr)
{
    if (!m_ptr->ml)
	(void)strcpy(m_name, "It");
    else {
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    (void)sprintf(m_name, "%s", r_ptr->name);
	else
	    (void)sprintf(m_name, "The %s", r_ptr->name);
    }
}

void lower_monster_name(char *m_name, monster_type *m_ptr, monster_race *r_ptr)
{
    if (!m_ptr->ml)
	(void)strcpy(m_name, "it");
    else {
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    (void)sprintf(m_name, "%s", r_ptr->name);
	else
	    (void)sprintf(m_name, "the %s", r_ptr->name);
    }
}


/*
 * polymorph is now uniform for poly/mass poly/choas poly, and only
 * as deadly as chaos poly is.  This still makes polymorphing a bad
 * idea, but it won't be automatically fatal. -CFT 
 */
static int poly(int mnum)
{
    register monster_race *c_ptr = &r_list[m_list[mnum].r_idx];
    int y, x;
    int i,j,k;
    
    /* Uniques never polymorph */
    if (c_ptr->cflags2 & MF2_UNIQUE) return 0;

    /* Save the monster location */
    y = m_list[mnum].fy;
    x = m_list[mnum].fx;

    i = (randint(20)/randint(9))+1;
    k = j = c_ptr->level;
    if ((j -=i)<0) j = 0;
    if ((k +=i)>MAX_R_LEV) k = MAX_R_LEV;

    /* "Kill" the monster */
    delete_monster(mnum);

    do {
	i = randint(m_level[k]-m_level[j])-1+m_level[j];  /* new creature index */
    } while (r_list[i].cflags2 & MF2_UNIQUE);

    /* Place the new monster where the old one was */
    place_monster(y,x,i,FALSE);

    /* Success */
    return 1;
}



/*
 * Create a wall.		-RAK-	 
 */
int build_wall(int dir, int y, int x)
{
    register int            i;
    int                     build, damage, dist, flag;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   m_name, out_val;

    build = FALSE;
    dist = 0;
    flag = FALSE;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else {
	    if (c_ptr->m_idx > 1) {

	    /* stop the wall building */
		flag = TRUE;

		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_list[m_ptr->r_idx];

		if (!(r_ptr->cflags1 & CM_PHASE)) {

		    /* monster does not move, can't escape the wall */
		    if (r_ptr->cflags1 & CM_ATTACK_ONLY)
			damage = 250;
		    else
			damage = damroll(4, 8);

		    monster_name(m_name, m_ptr, r_ptr);
		    (void)sprintf(out_val, "%s wails out in pain!", m_name);
		    msg_print(out_val);

		    i = mon_take_hit((int)c_ptr->m_idx, damage, TRUE);
		    if (i >= 0) {
			(void)sprintf(out_val, "%s is embedded in the rock.",
				      m_name);
			msg_print(out_val);
			/* prt_experience(); */
		    }
		}

		/* Earth elementals and Xorn's actually GAIN hitpoints */
		else if (r_ptr->r_char == 'E' || r_ptr->r_char == 'X') {
		    m_ptr->hp += damroll(4, 8);
		}
	    }

	    if (!valid_grid(y,x)) continue;

	    delete_object(y, x);
	    c_ptr->fval = MAGMA_WALL;
	    c_ptr->fm = FALSE;
	    lite_spot(y, x);
	    i++;
	    build = TRUE;
	}
    }
    while (!flag);

    /* Success? */   
    return (build);
}



/*
 * Move the creature record to a new location		-RAK-	 
 */
void teleport_away(int monptr, int dis)
{
    register int           yn, xn, ctr;
    register monster_type *m_ptr;

    m_ptr = &m_list[monptr];

    /* Find a location */   
    ctr = 0;
    do {
	do {
	    yn = m_ptr->fy + (randint(2 * dis + 1) - (dis + 1));
	    xn = m_ptr->fx + (randint(2 * dis + 1) - (dis + 1));
	}
	while (!in_bounds(yn, xn));
	ctr++;
	if (ctr > 9) {
	    ctr = 0;
	    dis += 5;
	}
    }
    while (!empty_grid_bold(yn,xn));


    /* Move the monster */
    move_rec((int)m_ptr->fy, (int)m_ptr->fx, yn, xn);
    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    m_ptr->fy = yn;
    m_ptr->fx = xn;

    /* Pretend we were previously "unseen" */
    m_ptr->ml = FALSE;

    m_ptr->cdis = distance(char_row, char_col, yn, xn);

    /* Update the monster */
    update_mon(monptr);
}


/*
 * Teleport player to spell casting creature		-RAK-	 
 */
void teleport_to(int ny, int nx)
{
    int dis, ctr, y, x;

    dis = 1;
    ctr = 0;

    /* Pick a usable location */  
    do {
	do {			   /* bounds check added -CFT */

	    /* Pick a location */
	    y = ny + (randint(2 * dis + 1) - (dis + 1));
	    x = nx + (randint(2 * dis + 1) - (dis + 1));
	} while (!in_bounds(y, x));

	/* Count */
	ctr++;

	/* Try farther away */
	if (ctr > (4 * dis * dis + 4 * dis + 1)) {
	    ctr = 0;
	    dis++;
	}
    }
    while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].m_idx >= 2));

    /* Move the player */
    move_rec(char_row, char_col, y, x);

	darken_player(char_row, char_col);
    char_row = y;
    char_col = x;

    /* Check the view */
    check_view();

    /* light creatures */
    creatures(FALSE);
}


/*
 * Delete all creatures within max_sight distance	-RAK-
 * NOTE : Winning creatures cannot be genocided			
 * Wizards can genocide anything they can see
 */
int mass_genocide(int spell)
{
    register int        i, result;
    register monster_type *m_ptr;
    register monster_race *r_ptr;

    result = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	m_ptr = &m_list[i];
	r_ptr = &r_list[m_ptr->r_idx];

	if (((m_ptr->cdis <= MAX_SIGHT) &&
	     ((r_ptr->cflags1 & CM_WIN) == 0) &&
	     ((r_ptr->cflags2 & MF2_UNIQUE) == 0)) ||
	    (wizard && (m_ptr->cdis <= MAX_SIGHT))) {

	    /* Delete the monster */
	    delete_monster(i);

	    /* Cute visual feedback as the player slowly dies */
	    if (spell) {
		take_hit(randint(3), "the strain of casting Mass Genocide");
		prt_chp();
		put_qio();
#ifdef MSDOS
		delay(20* delay_spd);	/* milliseconds */
#else
		usleep(20000 * delay_spd);	/* useconds */
#endif
	    }

	    result = TRUE;
	}
    }

    return (result);
}

/*
 * Delete (not kill) all creatures of a given type from level.	-RAK-
 * NOTE : Winning creatures can not be genocided.
 */
int genocide(int spell)
{
    register int		i, killed;
    char			typ;
    register monster_type	*m_ptr;
    register monster_race	*r_ptr;
    vtype                   out_val;

    killed = FALSE;

    if (get_com("Which type of creature do you wish exterminated?", &typ))

	for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	    m_ptr = &m_list[i];
	    r_ptr = &r_list[m_ptr->r_idx];

	    if ((unsigned) typ == r_list[m_ptr->r_idx].r_char)

		/* Genocide it */
		if ((r_ptr->cflags1 & CM_WIN) == 0) {

		    /* Delete the monster */
		    delete_monster(i);

		    if (spell) {
			take_hit(randint(4), "the strain of casting Genocide");
			prt_chp();
			put_qio();
#ifdef MSDOS
			delay(20 * delay_spd);	/* milliseconds */
#else
			usleep(20000 * delay_spd);	/* useconds */
#endif
		    }
		    killed = TRUE;
		}

		/* Cannot genocide a Quest Monster */
		else {

			/* genocide is a powerful spell, so we will let the */
			/* player know the names of the creatures he did not */
			/* destroy, this message makes no sense otherwise */
		    if (r_ptr->cflags2 & MF2_UNIQUE)
			(void)sprintf(out_val, "%s is unaffected.", r_ptr->name);
		    else
			(void)sprintf(out_val, "The %s is unaffected.", r_ptr->name);
		    msg_print(out_val);
		}
	}
    return (killed);
}


/* Change speed of any creature .			-RAK-	 */
/* NOTE: cannot slow a winning creature (BALROG)		 */
int speed_monsters(int spd)
{
    register int        i, speed;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    vtype               out_val, m_name;

    speed = FALSE;

    for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	m_ptr = &m_list[i];

	r_ptr = &r_list[m_ptr->r_idx];
	monster_name(m_name, m_ptr, r_ptr);

	if (!los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	/* do nothing */
	    ;

	else if (spd > 0) {
	    m_ptr->mspeed += spd;
	    m_ptr->csleep = 0;
	    if (m_ptr->ml) {
		speed = TRUE;
		(void)sprintf(out_val, "%s starts moving faster.", m_name);
		msg_print(out_val);
	    }
	} else if ((r_ptr->level <
	    randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) &&
		   !(r_ptr->cflags2 & MF2_UNIQUE)) {

	    m_ptr->mspeed += spd;

	    m_ptr->csleep = 0;

	    if (m_ptr->ml) {
		(void)sprintf(out_val, "%s starts moving slower.", m_name);
		msg_print(out_val);
		speed = TRUE;
	    }
	}
	else if (m_ptr->ml) {
	    (void)sprintf(out_val, "%s is unaffected.", m_name);
	    msg_print(out_val);
	}
    }

    return (speed);
}


/*
 * Sleep any creature.		-RAK-	 
 */
int sleep_monsters2(void)
{
    register int        i, sleep;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    vtype               out_val, m_name;

    sleep = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	r_ptr = &r_list[m_ptr->r_idx];
	monster_name(m_name, m_ptr, r_ptr);

	if ((m_ptr->cdis > MAX_SIGHT) ||
	    !los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	/* do nothing */
	    ;

	else if ((r_ptr->level >
	    randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
	    (r_ptr->cflags2 & MF2_UNIQUE) || (r_ptr->cflags2 & MF2_CHARM_SLEEP)) {

	    if (m_ptr->ml) {
		if (r_ptr->cflags2 & MF2_CHARM_SLEEP) {
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_CHARM_SLEEP;
		}
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
	    }
	}
	else {
	    m_ptr->csleep = 500;
	    if (m_ptr->ml) {
		(void)sprintf(out_val, "%s falls asleep.", m_name);
		msg_print(out_val);
		sleep = TRUE;
	    }
	}
    }

    return (sleep);
}


/*
 * Crazy function -- polymorph all visible creatures.
 */
int mass_poly()
{
    register int i;
    int mass;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;

    mass = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	if (m_ptr->cdis <= MAX_SIGHT) {
	    r_ptr = &r_list[m_ptr->r_idx];
	    if (((r_ptr->cflags1 & CM_WIN) == 0) && !(r_ptr->cflags2 & MF2_UNIQUE)) {
		mass = poly(i);
	    }
	}
    }
    return(mass);
}


/*
 * Display evil creatures on current panel		-RAK-	 
 */
int detect_evil(void)
{
    register int        i, flag;
    register monster_type *m_ptr;

    flag = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
	    (MF2_EVIL & r_list[m_ptr->r_idx].cflags2)) {

	    /* Draw the monster (even if invisible) */
	    m_ptr->ml = TRUE;
	/* works correctly even if hallucinating */
	    print((char)r_list[m_ptr->r_idx].r_char, (int)m_ptr->fy,
		  (int)m_ptr->fx);

	    flag = TRUE;
	}
    }

    if (flag) {
	msg_print("You sense the presence of evil!");
	msg_print(NULL);

    /* must unlight every monster just lighted */
	creatures(FALSE);
    }

    return (flag);
}


/*
 * Change players hit points in some manner		-RAK-	 
 */
int hp_player(int num)
{
    register int          res;
    register struct misc *m_ptr;

    res = FALSE;

    m_ptr = &py.misc;

    if (m_ptr->chp < m_ptr->mhp) {
	m_ptr->chp += num;
	if (m_ptr->chp > m_ptr->mhp) {
	    m_ptr->chp = m_ptr->mhp;
	    m_ptr->chp_frac = 0;
	}
	prt_chp();

	num = num / 5;
	if (num < 3) {
	    if (num == 0)
		msg_print("You feel a little better.");
	    else
		msg_print("You feel better.");
	}
	else {
	    if (num < 7)
		msg_print("You feel much better.");
	    else
		msg_print("You feel very good.");
	}
	res = TRUE;
    }
    
    return (res);
}


/*
 * Cure players confusion				-RAK-	 
 */
int cure_confusion()
{
    register int           cure;
    register struct flags1 *f_ptr = &py.flags1;

    cure = FALSE;

    if (f_ptr->confused > 1) {
	f_ptr->confused = 1;
	cure = TRUE;
    }
    return (cure);
}


/*
 * Cure players blindness				-RAK-	 
 */
int cure_blindness(void)
{
    register int           cure;
    register struct flags1 *f_ptr = &py.flags1;

    cure = FALSE;

    if (f_ptr->blind > 1) {
	f_ptr->blind = 1;
	cure = TRUE;
    }
    return (cure);
}


/*
 * Cure poisoning					-RAK-	 
 */
int cure_poison(void)
{
    register int           cure;
    register struct flags1 *f_ptr = &py.flags1;

    cure = FALSE;

    if (f_ptr->poisoned > 1) {
	f_ptr->poisoned = 1;
	cure = TRUE;
    }
    return (cure);
}


/*
 * Cure the players fear				-RAK-	 
 */
int remove_fear()
{
    register int           result;
    register struct flags1 *f_ptr = &py.flags1;

    result = FALSE;

    if (f_ptr->afraid > 1) {
	f_ptr->afraid = 1;
	result = TRUE;
    }
    return (result);
}




/*
 * This is a fun one.  In a given block, pick some walls and
 * turn them into open spots.  Pick some open spots and turn
 * them into walls.  An "Earthquake" effect.	       -RAK-  
 */
void earthquake(void)
{
    register int        i, j;
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    int                 kill, damage, tmp, y, x;
    vtype               out_val, m_name;

    /* Shatter the terrain around the player */
    for (i = char_row - 10; i <= char_row + 10; i++)
	for (j = char_col - 10; j <= char_col + 10; j++)

	    if (((i != char_row) || (j != char_col)) &&
		in_bounds(i, j) && (distance(char_row, char_col, i, j)<=10) &&
		(randint(8) == 1)) {

	    /* Artifacts and Stairs and Stores resist */
	    if (!valid_grid(i,j)) continue;

		c_ptr = &cave[i][j];

		/* Delete the object */
		delete_object(i, j);

		/* Hurt the monster, if any */
		if (c_ptr->m_idx > 1) {

		    m_ptr = &m_list[c_ptr->m_idx];
		    r_ptr = &r_list[m_ptr->r_idx];

		    if (!(r_ptr->cflags1 & CM_PHASE) && !(r_ptr->cflags2 & MF2_BREAK_WALL)) {
			if ((movement_rate(c_ptr->m_idx) == 0) ||
			    (r_ptr->cflags1 & CM_ATTACK_ONLY))
			/* monster can not move to escape the wall */
			    kill = TRUE;

			/* Monster MAY have somewhere to flee */
			else {
			    kill = TRUE;
			    for (y = i - 1; y <= i + 1; y++)
				for (x = j - 1; x <= j + 1; x++)
				    if (cave[y][x].fval >= MIN_CLOSED_SPACE)
					kill = FALSE;
			}

			if (kill)
			    damage = 320;
			else
			    damage = damroll(3 + randint(3), 8+randint(5));

			monster_name(m_name, m_ptr, r_ptr);
			(void)sprintf(out_val, "%s wails out in pain!", m_name);
			msg_print(out_val);

			/* Do damage, get experience (?) */
			i = mon_take_hit((int)c_ptr->m_idx, damage, TRUE);
			if (i >= 0) {
			    (void)sprintf(out_val, "%s is embedded in the rock.",
					  m_name);
			    msg_print(out_val);
			/* prt_experience(); */
			}
		    }
		}

		if ((c_ptr->fval >= MIN_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
		    c_ptr->fval = CORR_FLOOR;
		    c_ptr->pl = FALSE;
		    c_ptr->fm = FALSE;
		} else if (c_ptr->fval <= MAX_CAVE_FLOOR) {
		    tmp = randint(10);
		    if (tmp < 6)
			c_ptr->fval = QUARTZ_WALL;
		    else if (tmp < 9)
			c_ptr->fval = MAGMA_WALL;
		    else
			c_ptr->fval = GRANITE_WALL;

		    c_ptr->fm = FALSE;
		}

		/* Redraw */
		lite_spot(i, j);
	    }
}


/*
 * Evil creatures don't like this.		       -RAK-   
 */
int protect_evil()
{
    register int           res;
    register struct flags1 *f_ptr = &py.flags1;

    if (f_ptr->protevil == 0)
	res = TRUE;
    else
	res = FALSE;
    f_ptr->protevil += randint(25) + 3 * py.misc.lev;
    return (res);
}


/*
 * Create some high quality mush for the player.	-RAK-
 * Nope, let's just fill him up and save everybody time... -CWS
 */
void create_food(void)
{
     msg_print("You feel full!");
	 msg_print(NULL);

#if defined(SATISFY_HUNGER)				/* new create food code -CWS */
	 py.flags1.food = PLAYER_FOOD_MAX;
#else
     /* add to food timer rather than create mush - cba */
     add_food(k_list[OBJ_MUSH].pval);
#endif

    /* Hack -- update the display */
     py.flags1.status &= ~(PY_WEAK | PY_HUNGRY);
     prt_hunger();
}


/*
 * Banish monsters -- a hack.
 */
int banish_creature(u32b cflag, int dist)
{
    register int           i;
    int                    dispel;
    register monster_type *m_ptr;

    dispel = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	if ((cflag & r_list[m_ptr->r_idx].cflags2) &&
	    (m_ptr->cdis <= MAX_SIGHT) &&
	    los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {
	    l_list[m_ptr->r_idx].r_cflags2 |= cflag;
	    (void)teleport_away(i, dist);
	    dispel = TRUE;
	}
    }

    return (dispel);
}

int probing(void)
{
    register int            i;
    int                     probe;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    register monster_lore   *mp;
    vtype                   out_val, m_name;

    msg_print("Probing...");
    probe = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	r_ptr = &r_list[m_ptr->r_idx];
	mp = &l_list[m_ptr->r_idx];
	if ((m_ptr->cdis <= MAX_SIGHT) &&
	    los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx) && 
	    (m_ptr->ml)) {
	    if (r_ptr->cflags2 & MF2_UNIQUE)
		sprintf(m_name, "%s", r_ptr->name);
	    else
		sprintf(m_name, "The %s", r_ptr->name);
	    sprintf(out_val, "%s has %d hit points.", m_name, m_ptr->hp);
	    move_cursor_relative(m_ptr->fy, m_ptr->fx);
	    msg_print(out_val);

	    /* let's make probing do good things to the monster memory -CWS */
	    mp->r_cflags2 = r_ptr->cflags2;
	    mp->r_cflags1 = (r_ptr->cflags1 & ~CM_TREASURE);

	    /* Probe worked */
	    probe = TRUE;
	}
    }

    if (probe)
	msg_print("That's all.");
    else
	msg_print("You find nothing to probe.");

    move_cursor_relative(char_row, char_col);
    return (probe);
}


/*
 * Attempts to destroy a type of creature.  Success depends on
 * the creatures level VS. the player's level		 -RAK-	 
 */
int dispel_creature(int cflag, int damage)
{
    register int	i;
    int			k, dispel;
    monster_type	*m_ptr;
    monster_race	*r_ptr;
    vtype		out_val, m_name;

    dispel = FALSE;

    /* Affect all nearby monsters within line of sight */
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	/* Get the monster */
	m_ptr = &m_list[i];

	if ((cflag & r_list[m_ptr->r_idx].cflags2) &&
	    (m_ptr->cdis <= MAX_SIGHT) &&
	    los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {

	/* Get the race */
	r_ptr = &r_list[m_ptr->r_idx];

	    /* Memorize the susceptibility */
	    l_list[m_ptr->r_idx].r_cflags2 |= cflag;

	    /* Get the name */
	    monster_name (m_name, m_ptr, r_ptr);

	    k = mon_take_hit(i, randint(damage), FALSE);
	    if (k >= 0)
		(void)sprintf(out_val, "%s dissolves!", m_name);
	    else
		(void)sprintf(out_val, "%s shudders.", m_name);
	    msg_print(out_val);

	    dispel = TRUE;
	    if (k >= 0) prt_experience();
	}
    }

    return (dispel);
}


/*
 * Attempt to turn (confuse) undead creatures.	-RAK-	 
 */
int turn_undead(void)
{
    register int            i, turn_und;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    turn_und = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	m_ptr = &m_list[i];

	r_ptr = &r_list[m_ptr->r_idx];
	if ((MF2_UNDEAD & r_ptr->cflags2)
	    && (m_ptr->cdis <= MAX_SIGHT)
	    && (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))) {
	    monster_name(m_name, m_ptr, r_ptr);
	    if (((py.misc.lev + 1) > r_ptr->level) ||
		(randint(5) == 1)) {
		if (m_ptr->ml) {
		    (void)sprintf(out_val, "%s runs frantically!", m_name);
		    msg_print(out_val);
		    turn_und = TRUE;
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_UNDEAD;
		}
		m_ptr->monfear = randint(py.misc.lev) * 2;
	    }
	    else if (m_ptr->ml) {
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
	    }
	}
    }

    return (turn_und);
}


/*
 * Leave a glyph of warding. Creatures will not pass over it. -RAK- 
 * 
 */
void warding_glyph(void)
{
    register int        i;
    register cave_type *c_ptr;

    c_ptr = &cave[char_row][char_col];
    if (c_ptr->i_idx == 0) {
	i = i_pop();
	c_ptr->i_idx = i;
	invcopy(&i_list[i], OBJ_SCARE_MON);
    }
}




/*
 * Lose experience					-RAK-	 
 */
void lose_exp(s32b amount)
{
    register int          i;
    register struct misc *m_ptr = &py.misc;
    register player_class  *c_ptr;

    if (amount > m_ptr->exp)
	m_ptr->exp = 0;
    else
	m_ptr->exp -= amount;

    prt_experience();

    i = 0;
    while (((player_exp[i] * m_ptr->expfact / 100) <= m_ptr->exp)
	   && (i < MAX_PLAYER_LEVEL))
	i++;

    /* increment i once more, because level 1 exp is stored in player_exp[0] */
    i++;

    if (i > MAX_PLAYER_LEVEL) i = MAX_PLAYER_LEVEL;

    if (m_ptr->lev != i) {

	m_ptr->lev = i;

	calc_hitpoints();

	c_ptr = &class[m_ptr->pclass];
	if (c_ptr->spell == MAGE) {
	    calc_spells(A_INT);
	    calc_mana(A_INT);
	}
    else if (c_ptr->spell == PRIEST) {
	    calc_spells(A_WIS);
	    calc_mana(A_WIS);
	}

	prt_level();
	prt_title();
    }
}


/*
 * Slow Poison						-RAK-	 
 */
int slow_poison()
{
    register int           slow;
    register struct flags1 *f_ptr = &py.flags1;

    slow = FALSE;

    if (f_ptr->poisoned > 0) {
	f_ptr->poisoned = f_ptr->poisoned / 2;
	if (f_ptr->poisoned < 1) f_ptr->poisoned = 1;
	slow = TRUE;
	msg_print("The effect of the poison has been reduced.");
    }
    return (slow);
}


/*
 * Bless						-RAK-	 
 */
void bless(int amount)
{
    py.flags1.blessed += amount;
}


/*
 * Detect Invisible for period of time			-RAK-	 
 */
void detect_inv2(int amount)
{
    py.flags1.detect_inv += amount;
}


/*
 * Routine used by "earthquakes"
 * Those routines will update the view, etc.
 */
static void replace_spot(int y, int x, int typ)
{
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];
    switch (typ) {
      case 1:
      case 2:
      case 3:
	c_ptr->fval = CORR_FLOOR;
	break;
      case 4:
      case 7:
      case 10:
	c_ptr->fval = GRANITE_WALL;
	break;
      case 5:
      case 8:
      case 11:
	c_ptr->fval = MAGMA_WALL;
	break;
      case 6:
      case 9:
      case 12:
	c_ptr->fval = QUARTZ_WALL;
	break;
    }

    /* No longer part of a room */
    c_ptr->pl = FALSE;
    c_ptr->fm = FALSE;
    c_ptr->lr = FALSE;

    /* Delete the object, if any */
    if (c_ptr->i_idx != 0)
	(void)delete_object(y, x);

    /* Delete the monster (if any) */
    if (c_ptr->m_idx > 1)
	delete_monster((int)c_ptr->m_idx);
}


/*
 * The spell of destruction.				-RAK-
 * NOTE : Winning creatures that are deleted will be considered
 * as teleporting to another level.  This will NOT win the game.
 */
void destroy_area(int y, int x)
{
    register int i, j, k;

    msg_print("There is a searing blast of light!");

    if (!py.flags1.resist_blind && !py.flags1.resist_lite)
	py.flags1.blind += 10 + randint(10);

    /* No destroying the town */
    if (dun_level > 0) {
	for (i = (y - 15); i <= (y + 15); i++)
	    for (j = (x - 15); j <= (x + 15); j++)
		if (valid_grid(i, j)) {
		    k = distance(i, j, y, x);
		    /* clear player's spot... from um55 -CFT */
		    if (k == 0) replace_spot(i, j, 1);
		    else if (k < 13) replace_spot(i, j, randint(6));
		    else if (k < 16) replace_spot(i, j, randint(9));
		}
    }

    /* We need to redraw the screen. -DGK */
    if (py.flags1.resist_blind || py.flags1.resist_lite) {

    /* Hack -- redraw the cave */
	draw_cave();
	creatures(FALSE);	   /* draw monsters */
    }
}


/*
 * Revamped!  Now takes item pointer, number of times to try enchanting,
 * and a flag of what to try enchanting.  Artifacts resist enchantment
 * some of the time, and successful enchantment to at least +0 might
 * break a curse on the item.  -CFT
 *
 * Enchants a plus onto an item.                        -RAK-   
 */
int enchant(inven_type *i_ptr, int n, byte eflag)
{
    register int chance, res = FALSE, i, a = i_ptr->flags2 & TR_ARTIFACT;
    int table[13] = {  10,  50, 100, 200, 300, 400,
			   500, 700, 950, 990, 992, 995, 997 };

    /* Try "n" times */
    for (i=0; i<n; i++) {

	chance = 0;

	if (eflag & ENCH_TOHIT) {

	    if (i_ptr->tohit < 1) chance = 0;
	    else if (i_ptr->tohit > 13) chance = 1000;
	    else chance = table[i_ptr->tohit-1];

	    if ((randint(1000)>chance) && (!a || randint(7)>3)) {

		i_ptr->tohit++;
		res = TRUE;

		/* only when you get it above -1 -CFT */
		if ((i_ptr->tohit >= 0) && (randint(4)==1) &&
		    (i_ptr->flags1 & TR3_CURSED)) {
		    msg_print("The curse is broken! ");
		    i_ptr->flags1 &= ~TR3_CURSED;
		    i_ptr->ident &= ~ID_DAMD;
		}
	    }
	}

	if (eflag & ENCH_TODAM) {

	    if (i_ptr->todam < 1) chance = 0;
	    else if (i_ptr->todam > 13) chance = 1000;
	    else chance = table[i_ptr->todam-1];

	    if ((randint(1000)>chance) && (!a || randint(7)>3)) {

		i_ptr->todam++;
		res = TRUE;

		/* only when you get it above -1 -CFT */
		if ((i_ptr->todam >= 0) && (randint(4)==1) &&
		    (i_ptr->flags1 & TR3_CURSED)) {
		    msg_print("The curse is broken! ");
		    i_ptr->flags1 &= ~TR3_CURSED;
		    i_ptr->ident &= ~ID_DAMD;
		}
	    }
	}

	if (eflag & ENCH_TOAC) {

	    if (i_ptr->toac < 1) chance = 0;
	    else if (i_ptr->toac > 13) chance = 1000;
	    else chance = table[i_ptr->toac-1];

	    if ((randint(1000)>chance) && (!a || randint(7)>3)) {

		i_ptr->toac++;
		res = TRUE;

		/* only when you get it above -1 -CFT */
		if ((i_ptr->toac >= 0) && (randint(4)==1) &&
		    (i_ptr->flags1 & TR3_CURSED)) {
		    msg_print("The curse is broken! ");
		    i_ptr->flags1 &= ~TR3_CURSED;
		    i_ptr->ident &= ~ID_DAMD;
		}
	    }
	}
    }

    if (res) calc_bonuses();

    return (res);
}


/*
 * Hack -- generate a "formattable" pain message
 * Should convert this to take a monster name
 */
cptr pain_message(int monptr, int dam)
{
    register monster_type	*m_ptr;
    monster_race		*c_ptr;
    int				percentage, oldhp, newhp;

    /* avoid potential div by 0 */
    if (dam == 0)
	return "%s is unharmed.";

    m_ptr = &m_list[monptr];
    c_ptr = &r_list[m_ptr->r_idx];

#ifdef MSDOS			   /* more fix -CFT */
    newhp = (s32b) (m_ptr->hp);
    oldhp = newhp + (s32b) dam;
#else
    newhp = m_ptr->hp;
    oldhp = newhp + dam;
#endif
    percentage = (newhp * 100) / oldhp;

    /* Non-verbal creatures like molds */
    if ((c_ptr->r_char == 'j') ||
	(c_ptr->r_char == 'Q') || (c_ptr->r_char == 'v') || (c_ptr->r_char == 'm') ||
	((c_ptr->r_char == 'e') && stricmp(c_ptr->name, "Beholder"))) {

	if (percentage > 95)
	    return "%s barely notices.";
	if (percentage > 75)
	    return "%s flinches.";
	if (percentage > 50)
	    return "%s squelches.";
	if (percentage > 35) {
	    if (randint(4) == 1) /* thanks to dbd@panacea.phys.utk.edu -CWS */
		return "%s quivers in pain.";
	    else
		return "%s imitates Bill Cosby in pain.";
	}
	if (percentage > 20)
	    return "%s writhes about.";
	if (percentage > 10)
	    return "%s writhes in agony.";
	return "%s jerks limply.";
    }

    /* Dogs and Hounds */
    else if (c_ptr->r_char == 'C' || c_ptr->r_char == 'Z') {

	if (percentage > 95)
	    return "%s shrugs off the attack.";
	if (percentage > 75)
	    return "%s snarls with pain.";
	if (percentage > 50)
	    return "%s yelps in pain.";
	if (percentage > 35)
	    return "%s howls in pain.";
	if (percentage > 20)
	    return "%s howls in agony.";
	if (percentage > 10)
	    return "%s writhes in agony.";
	return "%s yelps feebly.";
    }

    /* One type of monsters (ignore,squeal,shriek) */
    else if (c_ptr->r_char == 'K' || c_ptr->r_char == 'c' || c_ptr->r_char == 'a' ||
	c_ptr->r_char == 'U' || c_ptr->r_char == 'q' || c_ptr->r_char == 'R' ||
	c_ptr->r_char == 'X' || c_ptr->r_char == 'b' || c_ptr->r_char == 'F' ||
	c_ptr->r_char == 'J' || c_ptr->r_char == 'l' || c_ptr->r_char == 'r' ||
	c_ptr->r_char == 's' || c_ptr->r_char == 'S' || c_ptr->r_char == 't') {

	if (percentage > 95)
	    return "%s ignores the attack.";
	if (percentage > 75)
	    return "%s grunts with pain.";
	if (percentage > 50)
	    return "%s squeals in pain.";
	if (percentage > 35)
	    return "%s shrieks in pain.";
	if (percentage > 20)
	    return "%s shrieks in agony.";
	if (percentage > 10)
	    return "%s writhes in agony.";
	return "%s cries out feebly.";
    }

    /* Another type of monsters (shrug,cry,scream) */
    else {

	if (percentage > 95)
	    return "%s shrugs off the attack.";
	if (percentage > 75)
	    return "%s grunts with pain.";
	if (percentage > 50)
	    return "%s cries out in pain.";
	if (percentage > 35)
	    return "%s screams in pain.";
	if (percentage > 20)
	    return "%s screams in agony.";
	if (percentage > 10)
	    return "%s writhes in agony.";
	return "%s cries out feebly.";
    }
}



/*
 * Removes curses from items in inventory
 */
int remove_curse()
{
    register int         i, result;
    register inven_type *i_ptr;

    result = FALSE;

    /* Attempt to uncurse items being worn */
    for (i = INVEN_WIELD; i <= INVEN_OUTER; i++) {

	i_ptr = &inventory[i];

	if ((TR3_CURSED & i_ptr->flags1) &&
	    (i_ptr->name2 != EGO_MORGUL) &&
	    (i_ptr->name2 != ART_CALRIS) &&
	    (i_ptr->name2 != ART_MORMEGIL)) {
	    if (!(!stricmp(k_list[i_ptr->index].name, "Power") &&
		  (i_ptr->tval == TV_RING))) {

		i_ptr->flags1 &= ~TR3_CURSED;
		i_ptr->ident &= ~ID_DAMD;	/* DGK */
		i_ptr->inscrip[0] = '\0';
		calc_bonuses();
		result = TRUE;
	    }
	}
    }
    return (result);
}

int remove_all_curse()
{
    register int         i, result;
    register inven_type *i_ptr;

    result = FALSE;
    for (i = INVEN_WIELD; i <= INVEN_OUTER; i++) {
	i_ptr = &inventory[i];
	if (TR3_CURSED & i_ptr->flags1) {
	    if (!(!stricmp(k_list[i_ptr->index].name, "Power") &&
		  (i_ptr->tval == TV_RING))) {
		i_ptr->flags1 &= ~TR3_CURSED;
		i_ptr->ident &= ~ID_DAMD;	/* DGK */
		calc_bonuses();
		i_ptr->inscrip[0] = '\0';
		result = TRUE;
	    } else {
		msg_print("The One Ring resists all attempts to remove it!");
	    }
	}
    }
    return (result);
}



/*
 * Restores any drained experience			-RAK-	 
 */
int restore_level()
{
    register int          restore;
    register struct misc *m_ptr = &py.misc;

    restore = FALSE;

    if (m_ptr->max_exp > m_ptr->exp) {
	restore = TRUE;
	msg_print("You feel your life energies returning.");
    /* this while loop is not redundant, ptr_exp may reduce the exp level */
	while (m_ptr->exp < m_ptr->max_exp) {
	    m_ptr->exp = m_ptr->max_exp;
	    prt_experience();
	}
    }
    return (restore);
}


/*
 * self-knowledge... idea from nethack.  Useful for determining powers and
 * resistences of items.  It saves the screen, clears it, then starts listing
 * attributes, a screenful at a time.  (There are a LOT of attributes to
 * list.  It will probably take 2 or 3 screens for a powerful character whose
 * using several artifacts...) -CFT 
 */
void self_knowledge()
{
    int    i, j;
    u32b f = 0L, f2 = 0L;


    /* Acquire item flags (from worn items) */
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {

	/* Only examine real items */   
	if (inventory[i].tval != TV_NOTHING) {

	    /* Certain fields depend on a positive "pval" */
	    if (inventory[i].pval < 0) /* don't adjust TR_STATS if pval is negative -CWS */
		f |= (inventory[i].flags1 & ~(TR_STATS | TR1_SEARCH | TR1_STEALTH) );
	    else
		f |= inventory[i].flags1;
	    f2 |= inventory[i].flags2;
	}
    }

    save_screen();

/* map starts at 13, but I want a couple of spaces.
 * This means must start by erasing map...
 */
    j = 15;
    for (i = 1; i < 23; i++)
	erase_line(i, j - 2);	   /* erase a couple of spaces to left */

    i = 1;
    prt("Your Attributes:", i++, j + 5);

    if (py.flags1.blind > 0)
	prt("You cannot see.", i++, j);
    if (py.flags1.confused > 0)
	prt("You are confused.", i++, j);
    if (py.flags1.afraid > 0)
	prt("You are terrified.", i++, j);
    if (py.flags1.cut > 0)
	prt("You are bleeding.", i++, j);
    if (py.flags1.stun > 0)
	prt("You are stunned and reeling.", i++, j);
    if (py.flags1.poisoned > 0)
	prt("You are poisoned.", i++, j);
    if (py.flags1.image > 0)
	prt("You are hallucinating.", i++, j);
    if (py.flags1.aggravate)
	prt("You aggravate monsters.", i++, j);
    if (py.flags1.teleport)
	prt("Your position is very uncertain.", i++, j);

    if (py.flags1.blessed > 0)
	prt("You feel rightous.", i++, j);
    if (py.flags1.hero > 0)
	prt("You feel heroic.", i++, j);
    if (py.flags1.shero > 0)
	prt("You are in a battle rage.", i++, j);
    if (py.flags1.protevil > 0)
	prt("You are protected from evil.", i++, j);
    if (py.flags1.shield > 0)
	prt("You are protected by a mystic shield.", i++, j);
    if (py.flags1.invuln > 0)
	prt("You are temporarily invulnerable.", i++, j);
    if (py.flags1.confuse_monster)
	prt("Your hands are glowing dull red.", i++, j);
    if (py.flags1.new_spells > 0)
	prt("You can learn some more spells.", i++, j);
    if (py.flags1.word_recall > 0)
	prt("You will soon be recalled.", i++, j);

    if (f & TR1_STEALTH)
	prt("You are magically stealthy.", i++, j);
    if (f & TR1_SEARCH) {
	prt("You are magically perceptive.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if ((py.flags1.see_infra) || (py.flags1.tim_infra)) {
	prt("Your eyes are sensitive to infrared light.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if ((py.flags1.see_inv) || (py.flags1.detect_inv)) {
	prt("You can see invisible creatures.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.ffall) {
	prt("You land gently.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.free_act) {
	prt("You have free action.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.regenerate) {
	prt("You regenerate quickly.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.slow_digest) {
	prt("Your appetite is small.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.telepathy) {
	prt("You have ESP.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.hold_life) {
	prt("You have a firm hold on your life force.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.light) {
	prt("You are carrying a permanent light.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_fear) {
	prt("You are completely fearless.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_blind) {
	prt("Your eyes are resistant to blindness.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.immune_fire) {
	prt("You are completely immune to fire.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_fire) && (py.flags1.oppose_fire)) {
	prt("You resist fire exceptionally well.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_fire) || (py.flags1.oppose_fire)) {
	prt("You are resistant to fire.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.immune_cold) {
	prt("You are completely immune to cold.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_cold) && (py.flags1.oppose_cold)) {
	prt("You resist cold exceptionally well.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_cold) || (py.flags1.oppose_cold)) {
	prt("You are resistant to cold.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.immune_acid) {
	prt("You are completely immune to acid.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_acid) && (py.flags1.oppose_acid)) {
	prt("You resist acid exceptionally well.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_acid) || (py.flags1.oppose_acid)) {
	prt("You are resistant to acid.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.immune_pois) {
	prt("You are completely immune to poison.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_pois) && (py.flags1.oppose_pois)) {
	prt("You resist poison exceptionally well.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_pois) || (py.flags1.oppose_pois)) {
	prt("You are resistant to poison.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.immune_elec) {
	prt("You are completely immune to lightning.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_elec) && (py.flags1.oppose_elec)) {
	prt("You resist lightning exceptionally well.", i++, j);
	pause_if_screen_full(&i, j);
    } else if ((py.flags1.resist_elec) || (py.flags1.oppose_elec)) {
	prt("You are resistant to lightning.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_lite) {
	prt("You are resistant to bright light.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_dark) {
	prt("You are resistant to darkness.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_conf) {
	prt("You are resistant to confusion.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_sound) {
	prt("You are resistant to sonic attacks.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_disen) {
	prt("You are resistant to disenchantment.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_chaos) {
	prt("You are resistant to chaos.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_shards) {
	prt("You are resistant to blasts of shards.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_nexus) {
	prt("You are resistant to nexus attacks.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.resist_nether) {
	prt("You are resistant to nether forces.", i++, j);
	pause_if_screen_full(&i, j);
    }

#if 0
/* Are these needed?  The player can see this...  For now, in here for
 * completeness... -CFT 
 */
    if (f & TR1_STR) {
	prt("You are magically strong.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_INT) {
	prt("You are magically intelligent.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_WIS) {
	prt("You are magically wise.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_DEX) {
	prt("You are magically agile.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_CON) {
	prt("You are magically tough.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_CHR) {
	prt("You are magically popular.", i++, j);
	pause_if_screen_full(&i, j);
    }

#endif

    if (py.flags1.sustain_str) {
	prt("You will not become weaker.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.sustain_int) {
	prt("You will not become dumber.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.sustain_wis) {
	prt("You will not become less wise.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.sustain_con) {
	prt("You will not become out of shape.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.sustain_dex) {
	prt("You will not become clumsy.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (py.flags1.sustain_chr) {
	prt("You will not become less popular.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (inventory[INVEN_LEFT].flags2 & TR1_ATTACK_SPD ||
	inventory[INVEN_RIGHT].flags2 & TR1_ATTACK_SPD) {
	prt("You can strike at your foes with uncommon speed.", i++, j);
	pause_if_screen_full(&i, j);
    }

/* this IS a bit redundant, but it prevents flags1 from other items from
 * affecting the weapon stats... -CFT
 */
    if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
	f = inventory[INVEN_WIELD].flags1;
	f2 = inventory[INVEN_WIELD].flags2;
    } else {
	f = 0L;
	f2 = 0L;
    }
    if (f & TR3_CURSED) {
	if (inventory[INVEN_WIELD].name2 == EGO_MORGUL)
	    prt("Your weapon is truly foul.", i++, j);
	else if (inventory[INVEN_WIELD].name2 == ART_CALRIS)
	    prt("Your bastard sword is wickedly accursed.", i++, j);
	else if (inventory[INVEN_WIELD].name2 == ART_MORMEGIL)
	    prt("Your two-handed sword radiates an aura of unspeakable evil.", i++, j);
	else
	    prt("Your weapon is accursed.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_TUNNEL) {
	prt("Your weapon is an effective digging tool.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR_BLESS_BLADE) {
	prt("Your weapon has been blessed by the gods.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_ATTACK_SPD) {
	prt("Your weapon strikes with uncommon speed.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_SLAY_ORC) {
	prt("Your weapon is especially deadly against orcs.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_SLAY_TROLL) {
	prt("Your weapon is especially deadly against trolls.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_SLAY_GIANT) {
	prt("Your weapon is especially deadly against giants.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_SLAY_ANIMAL) {
	prt("Your weapon is especially deadly against natural creatures.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_SLAY_X_DRAGON) {
	prt("Your weapon is a great bane of dragons.", i++, j);
	pause_if_screen_full(&i, j);
    } else if (f & TR1_SLAY_DRAGON) {
	prt("Your weapon is especially deadly against dragons.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_SLAY_DEMON) {
	prt("Your weapon strikes at demons with holy wrath.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_SLAY_UNDEAD) {
	prt("Your weapon strikes at undead with holy wrath.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_SLAY_EVIL) {
	prt("Your weapon fights against evil with holy fury.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_BRAND_COLD) {
	prt("Your frigid weapon freezes your foes.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f & TR1_BRAND_FIRE) {
	prt("Your flaming weapon burns your foes.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR3_LITENING) {
	prt("Your weapon electrocutes your foes.", i++, j);
	pause_if_screen_full(&i, j);
    }
    if (f2 & TR1_IMPACT)
	prt("The unbelievable impact of your weapon can cause earthquakes.", i++, j);

    /* Pause */
    pause_line(i);

    /* Restore the screen */
    restore_screen();
}





/*
 * Teleport the player a level (or three:-)
 */
void tele_level()
{
    if (dun_level == Q_PLANE)
	dun_level = 0;
    else if (is_quest(dun_level))
	dun_level -= 1;
    else
	dun_level += (-3) + 2 * randint(2);
    if (dun_level < 0)
	dun_level = 0;

    /* New level */
    new_level_flag = TRUE;
}



/*
 * Sleep creatures adjacent to player			-RAK-	 
 */
int sleep_monsters1(int y, int x)
{
    register int            i, j;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    int                     sleep;
    vtype                   out_val, m_name;

    sleep = FALSE;
    for (i = y - 1; i <= y + 1; i++)
	for (j = x - 1; j <= x + 1; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->m_idx > 1) {

		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_list[m_ptr->r_idx];
		monster_name(m_name, m_ptr, r_ptr);

		if ((r_ptr->level >
		     randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
		    (MF2_CHARM_SLEEP & r_ptr->cflags2) || (r_ptr->cflags2 & MF2_UNIQUE)) {
		    if (m_ptr->ml && (r_ptr->cflags2 & MF2_CHARM_SLEEP))
			l_list[m_ptr->r_idx].r_cflags2 |= MF2_CHARM_SLEEP;
		    (void)sprintf(out_val, "%s is unaffected.", m_name);
		    msg_print(out_val);
		}
		else {
		    sleep = TRUE;
		    m_ptr->csleep = 500;
		    (void)sprintf(out_val, "%s falls asleep.", m_name);
		    msg_print(out_val);
		}

	    }
	}
    return (sleep);
}



/*
 * Forget everything
 */
int lose_all_info(void)
{
    int                 i;

    for (i = 0; i <= INVEN_AUX; i++) {

	if (inventory[i].tval != TV_NOTHING)
	    inventory[i].ident &= ~(ID_KNOWN2);
    }

    /* Forget the map */
    wiz_lite(-1);

    return (0);
}

/*
 * Detect any treasure on the current panel		-RAK-	 
 */
int detect_treasure(void)
{
    register int        i, j, detect;
    register cave_type *c_ptr;

    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];
	    if ((c_ptr->i_idx != 0) && (i_list[c_ptr->tptr].tval == TV_GOLD) &&
		!test_lite(i, j)) {

		/* Hack -- memorize the item */
		c_ptr->fm = TRUE;

		/* Redraw */
		lite_spot(i, j);
		detect = TRUE;
	    }
	}
    return (detect);
}



/*
 * Detect magic items.
 *
 * This will light up all spaces with "magic" items, including potions, scrolls,
 * rods, wands, staves, amulets, rings, and "enchanted" items.
 *
 * This excludes all foods and spell books. -- JND  
 */
int detect_magic()
{
    register int i, j, detect;
    register cave_type *c_ptr;
    register inven_type *t_ptr;
    int                  Tval;

    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];

	    if ((c_ptr->i_idx != 0) && (i_list[c_ptr->tptr].tval < TV_MAX_OBJECT)
		&& !test_lite(i, j)) {

	    /* Get the item */
		t_ptr = &i_list[c_ptr->i_idx];

	    /* Examine the tval */ 
		Tval = t_ptr->tval;

	    /* Is it a weapon or armor or light? */
	    /* If so, check for plusses on weapons and armor ... */
	    /* ... and check whether it is an artifact! ;)  */
	    /* Is it otherwise magical? */
	    if (((Tval > 9) && (Tval < 39)) &&
		(((t_ptr->tohit > 0) || (t_ptr->todam) || (t_ptr->toac) ||
		(t_ptr->flags2 & TR_ARTIFACT)) ||
		((Tval > 39) && (Tval < 77)))) {

		c_ptr->fm = TRUE;

		/* Redraw */
		lite_spot(i, j);
		detect = TRUE;
		}
	    }
	}

    /* Return result */    
    return (detect);
}



int detect_enchantment()
{
    register int i, j, detect, tv;
    register cave_type *c_ptr;
    
    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];
	    tv = i_list[c_ptr->i_idx].tval;
	    if ((c_ptr->i_idx != 0) && !test_lite(i, j) &&
		( ((tv > TV_MAX_ENCHANT) && (tv < TV_FLASK)) || /* misc items */
		 (tv == TV_MAGIC_BOOK) || (tv == TV_PRAYER_BOOK) || /* books */
		 ((tv >= TV_MIN_WEAR) && (tv <= TV_MAX_ENCHANT) && /* armor/weap */
		  ((i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT) || /* if Art., or */
		   (i_list[c_ptr->i_idx].tohit>0) || /* has pluses, then show */
		   (i_list[c_ptr->i_idx].todam>0) ||
		   (i_list[c_ptr->i_idx].toac>0))) )){
		c_ptr->fm = TRUE;
		lite_spot(i, j);
		detect = TRUE;
	    }
	}
    return(detect);
}


/*
 * Detect everything
 */
int detection(void)
{
    register int           i, detect;
    register monster_type *m_ptr;

    /* Detect the easy things */
    detect_treasure();
    detect_object();
    detect_trap();
    detect_sdoor();

    /* Illuminate all monsters in the current panel */
    detect = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx)) {

	    /* Draw the monster (even if invisible) */
	    m_ptr->ml = TRUE;
	    /* works correctly even if hallucinating */
	    print((char)r_list[m_ptr->r_idx].r_char, (int)m_ptr->fy, (int)m_ptr->fx);

	    detect = TRUE;
	}
    }

    /* Describe the result, then fix the monsters */
    if (detect) {

	msg_print("You sense the presence of monsters!");
	msg_print(NULL);

	/* Fix the monsters */
	creatures(FALSE);
    }

    /* XXX Only returns true if monsters were detected */
    return (detect);
}

/*
 * Detect all objects on the current panel		-RAK-	 
 */
int detect_object(void)
{
    register int        i, j, detect;
    register cave_type *c_ptr;

    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {

	    c_ptr = &cave[i][j];
	    if ((c_ptr->i_idx != 0) && (i_list[c_ptr->tptr].tval < TV_MAX_OBJECT)
		&& !test_lite(i, j)) {

	    /* Hack -- memorize it */
		c_ptr->fm = TRUE;

	    /* Redraw */
		lite_spot(i, j);
		detect = TRUE;
	    }
	}

    return (detect);
}


/*
 * Locates and displays traps on current panel		-RAK-	 
 */
int detect_trap(void)
{
    register int         i, j;
    int                  detect;
    register cave_type  *c_ptr;
    register inven_type *t_ptr;

    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->i_idx != 0)

	    /* Notice traps */
		if (i_list[c_ptr->i_idx].tval == TV_INVIS_TRAP) {
		    c_ptr->fm = TRUE;

		    t_ptr = &i_list[c_ptr->i_idx];
		    t_ptr->tval = TV_VIS_TRAP;

		/* Redraw */
		lite_spot(i, j);
		detect = TRUE;
	    }

	    /* Identify chests */
	    else if (i_list[c_ptr->i_idx].tval == TV_CHEST) {
		    t_ptr = &i_list[c_ptr->i_idx];
		    known2(t_ptr);
		}
	}

    return (detect);
}



/*
 * Create stairs
 * Assume the player grid is never blocked.
 */
void stair_creation()
{
    register cave_type *c_ptr;
    register int        cur_pos;

    c_ptr = &cave[char_row][char_col];

    /* Do not destroy useful stuff */
    if (valid_grid(char_row, char_col)) {

	if (c_ptr->i_idx != 0)
	    (void)delete_object(char_row, char_col);

	cur_pos = i_pop();
	c_ptr->i_idx = cur_pos;
	if ((randint(2) == 1 || is_quest(dun_level)) && (dun_level > 0))
	    invcopy(&i_list[cur_pos], OBJ_UP_STAIR);
	else
	    invcopy(&i_list[cur_pos], OBJ_DOWN_STAIR);
    } else
	msg_print("The object resists the spell.");
}


/*
 * Locates and displays all stairs and secret doors on current panel -RAK-	
 */
int detect_sdoor()
{
    register int        i, j, detect;
    register cave_type *c_ptr;

    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->i_idx != 0)

	    /* Secret doors  */
		if (i_list[c_ptr->i_idx].tval == TV_SECRET_DOOR) {
		    c_ptr->fm = TRUE;

		    /* change secret door to closed door */
		    i_list[c_ptr->i_idx].index = OBJ_CLOSED_DOOR;

		    i_list[c_ptr->i_idx].tval = k_list[OBJ_CLOSED_DOOR].tval;
		    i_list[c_ptr->i_idx].tchar = k_list[OBJ_CLOSED_DOOR].tchar;

		/* Redraw */
		lite_spot(i, j);
		detect = TRUE;
	    }

	    /* Staircases */
		else if (((i_list[c_ptr->i_idx].tval == TV_UP_STAIR) ||
			  (i_list[c_ptr->i_idx].tval == TV_DOWN_STAIR)) &&
			 !c_ptr->fm) {

		/* Hack -- memorize it */
		c_ptr->fm = TRUE;

		/* Redraw */
		lite_spot(i, j);
		detect = TRUE;
	    }
	}

    return (detect);
}


/*
 * Locates and displays all invisible creatures on current panel -RAK-
 */
int detect_invisible()
{
    register int           i, flag;
    register monster_type *m_ptr;
    register monster_race *r_ptr;

    flag = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	r_ptr = &r_list[m_ptr->r_idx];
	if (panel_contains(m_ptr->fy, m_ptr->fx) &&
	    (r_ptr->cflags1 & CM_INVISIBLE )) {

	    /* Draw the monster (even if invisible) */
	    m_ptr->ml = TRUE;
	    /* works correctly even if hallucinating */
	    print((char)r_ptr->r_char, (int)m_ptr->fy, (int)m_ptr->fx);

	    /* Something was detected */
	    flag = TRUE;
	}
    }

    if (flag) {

	msg_print("You sense the presence of invisible creatures!");
	msg_print(NULL);

	/* Fix the monsters */
	creatures(FALSE);
    }

    return (flag);
}






/*
 * Identify an object	-RAK-	 
 */
int ident_spell()
{
    int			item_val;
    bigvtype            out_val, tmp_str;
    register int        ident;
    register inven_type *i_ptr;

    ident = FALSE;
    switch (get_item(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE, 0))
	{
	case TRUE:
	    ident = TRUE;
	    identify(&item_val);
	    i_ptr = &inventory[item_val];
	    known2(i_ptr);
	    objdes(tmp_str, i_ptr, TRUE);
	    if (item_val >= INVEN_WIELD) {
		calc_bonuses();
		
		(void)sprintf(out_val, "%s: %s. ",
			      describe_use(item_val), tmp_str);
	    }  else
		(void)sprintf(out_val, "(%c) %s. ", item_val + 97, tmp_str);
	    msg_print(out_val);
	    break;
	case FUZZY:
	    ident = TRUE;
	    i_ptr = &i_list[cave[char_row][char_col].i_idx];
	    /* that piece of code taken from desc.c:identify()
	     * no use to convert type for calling identify since obj
	     * on floor can't stack
	     */

	    if ((i_ptr->flags1 & TR3_CURSED) && (i_ptr->tval != TV_MAGIC_BOOK) &&
		(i_ptr->tval != TV_PRAYER_BOOK))
		add_inscribe(i_ptr, ID_DAMD);
	    if (!known1_p(i_ptr))
		known1(i_ptr);
	    /* end of identify-code */
	    known2(i_ptr);
	    objdes(tmp_str, i_ptr, TRUE);
	    (void) sprintf(out_val, "%c %s", item_val+97, tmp_str);
	    msg_print(out_val+2);
	    break;
	default:
	    break;	    
	}
    return (ident);
}


/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
void identify_pack()
{
    int                 i;
    inven_type         *i_ptr;

    /* Simply identify and know every item */
    for (i = 0; i <= INVEN_AUX; i++) {
	if (inventory[i].tval != TV_NOTHING)
	    identify(&i);
	i_ptr = &inventory[i];
	known2(i_ptr);
    }
}


/*
 * Get all the monsters on the level pissed off.	-RAK-	 
 */
int aggravate_monster(int dis_affect)
{
    register int           i, aggravate;
    register monster_type *m_ptr;

    aggravate = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {
	m_ptr = &m_list[i];
	m_ptr->csleep = 0;
	if ((m_ptr->cdis <= dis_affect) && (m_ptr->mspeed < 2)) {
	    m_ptr->mspeed++;
	    aggravate = TRUE;
	}
    }
    if (aggravate)
	msg_print("You hear a sudden stirring in the distance!");
    return (aggravate);
}


/*
 * Display all creatures on the current panel		-RAK-	 
 */
int detect_monsters(void)
{
    register int        i, detect;
    register monster_type *m_ptr;

    detect = FALSE;
    for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	m_ptr = &m_list[i];

	if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
	    ((CM_INVISIBLE & r_list[m_ptr->r_idx].cflags1) == 0)) {

	    /* Draw the monster (unless invisible) */
	    m_ptr->ml = TRUE;
	    /* works correctly even if hallucinating */
	    print((char)r_list[m_ptr->r_idx].r_char, (int)m_ptr->fy, (int)m_ptr->fx);

	    detect = TRUE;
	}
    }

    if (detect) {

	/* Describe, and wait for acknowledgement */
	msg_print("You sense the presence of monsters!");
	msg_print(NULL);

	/* Fix the monsters */
	creatures(FALSE);
    }

    return (detect);
}



/*
 * Lightning ball in all directions    -SM-   
 */
void starball(int y, int x)
{
    register int i;

    for (i = 1; i <= 9; i++)
	if (i != 5)
	    fire_ball(GF_ELEC, i, y, x, 150, 3);
}




/*
 * Light line in all directions				-RAK-	 
 */
void starlite(int y, int x)
{
    register int i;

    if (py.flags1.blind < 1)
	msg_print("The end of the staff bursts into a blue shimmering light.");
    for (i = 1; i <= 9; i++)
	if (i != 5)
	    lite_line(i, y, x);
}



int recharge(int num)
{
    int                 i, j, k, l, item_val;
    register int        res;
    inven_type		*i_ptr;
    int                 found = FALSE;

    /* No range found yet */
    res = FALSE;

    /* Check for wands */
    if (find_range(TV_STAFF, TV_WAND, &i, &j))
	found = TRUE;

    /* Hack -- Check for rods */
    if (find_range(TV_ROD, TV_NEVER, &k, &l))
	found = TRUE;

    /* Quick check */
    if (!found)
	msg_print("You have nothing to recharge.");

    /* Ask for it */
    else if (get_item(&item_val, "Recharge which item?",
		      (k > -1) ? k : i, (j > -1) ? j : l, 0)) {

    /* Get the item */
    i_ptr = &inventory[item_val];

	res = TRUE;
	if (i_ptr->tval == TV_ROD) {
	    /* now allow players to speed up recharge time of rods -CFT */
	    u16b              t_o = i_ptr->timeout, t;

	    if (randint((100 - i_ptr->level + num) / 5) == 1) {	/* not today... */
		msg_print("The recharge backfires, and drains the rod further!");
		if (t_o < 32000)   /* don't overflow... */
		    i_ptr->timeout = (t_o + 100) * 2;
	    } else {
		t = (u16b) (num * damroll(2, 4));	/* rechange amount */
		if (t_o < t)
		    i_ptr->timeout = 0;
		else
		    i_ptr->timeout = t_o - t;
	    }
	}
	 /* if recharge rod... */ 
	else {			   /* recharge wand/staff */
	/* recharge I = recharge(20) = 1/6 failure for empty 10th level wand   */
	/* recharge II = recharge(60) = 1/10 failure for empty 10th level wand */
	/* make it harder to recharge high level, and highly charged wands     */

	    if (randint((num + 100 - (int)i_ptr->level - (10 * i_ptr->pval)) / 15) == 1) {
		msg_print("There is a bright flash of light.");
		inven_destroy(item_val);
	    } else {
		num = (num / (i_ptr->level + 2)) + 1;
		i_ptr->pval += 2 + randint(num);
		if (known2_p(i_ptr))
		    clear_known2(i_ptr);
		clear_empty(i_ptr);
	    }
	}
    }
    return (res);
}








/* Shoot a ball in a given direction.  Note that balls have an  */
/* area affect.                                       -RAK-   */
void fire_ball(int typ, int dir, int y, int x, int dam_hp, int max_dis)
{
    register int        i, j;
    int                 dam, thit, tkill, k, tmp, monptr;
    int                 oldy, oldx, dist, flag;
    int                 (*destroy) ();
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    int                 ny, nx;
    char                bolt_char;

    thit = 0;
    tkill = 0;

    switch (typ) {
      case GF_FIRE:
	destroy = set_fire_destroy;
	break;
      case GF_ACID:
	destroy = set_acid_destroy;
	break;
      case GF_COLD:
      case GF_SHARDS:
      case GF_ICE:
      case GF_FORCE:
      case GF_SOUND:
	destroy = set_frost_destroy;	/* just potions and flasks -DGK */
	break;
      case GF_ELEC:
	destroy = set_lightning_destroy;
	break;
      case GF_PLASMA:		   /* DGK */
	destroy = set_plasma_destroy;	/* fire+lightning -DGK */
	break;
      case GF_METEOR:		   /* DGK */
	destroy = set_meteor_destroy;	/* fire+shards -DGK */
	break;
      case GF_MANA:		   /* DGK */
	destroy = set_mana_destroy;	/* everything -DGK */
	break;
      case GF_HOLY_ORB:	   /* DGK */
	destroy = set_holy_destroy;	/* cursed stuff -DGK */
	break;
      default:
	destroy = set_null;
	break;
    }

    flag = FALSE;
    oldy = y;
    oldx = x;
    dist = 0;
    do {
	ny = y;
	nx = x;

/* we don't call mmove if targetting and at target.  This allow player to target
 * a ball spell at his position, to explode it around himself -CFT
 */
	if (dir || !target_at(y,x))	    
	    (void)mmove(dir, &y, &x);

    /* choose the right shape for the bolt... -CFT */
	if (ny == y)
	    bolt_char = '-';
	else if (nx == x)
	    bolt_char = '|';
	else if ((ny - y) == (nx - x))
	    bolt_char = '\\';
	else
	    bolt_char = '/';

	dist++;
	lite_spot(oldy, oldx);
	if (dist > OBJ_BOLT_RANGE)
	    flag = TRUE;
	else {
	    c_ptr = &cave[y][x];
	/* targeting code stolen from Morgul -CFT */

	/* This test has been overhauled (twice):  basically, it now says: if
	 * ((spell hits a wall) OR ((spell hits a creature) and ((not
	 * targetting) or (at the target anyway) or (no line-of-sight to
	 * target, so aiming unusable) or ((aiming at a monster) and (that
	 * monster is unseen, so aiming unusable)))) OR ((we are targetting)
	 * and (at the target location))) THEN the ball explodes... -CFT  
	 */

#ifndef TARGET
	    if ((c_ptr->fval >= MIN_CLOSED_SPACE) ||
		((c_ptr->m_idx > 1))) {
		flag = TRUE;	   /* THEN we decide to explode here. -CFT */
		if (c_ptr->fval >= MIN_CLOSED_SPACE) {
		    y = oldy;
		    x = oldx;
		}
#else
	    if ((c_ptr->fval >= MIN_CLOSED_SPACE) ||
		((c_ptr->m_idx > 1) &&
		 (!target_mode || target_at(y, x) ||
		  !los(target_row, target_col, char_row, char_col) ||
		  ((target_mon < MAX_M_IDX) && !m_list[target_mon].ml))) ||
		(target_mode && target_at(y, x))) {
	        flag = TRUE;	   /* THEN we decide to explode here. -CFT */
	        if (c_ptr->fval >= MIN_CLOSED_SPACE) {
		    y = oldy;
		    x = oldx;
	        }
#endif
	    /* The ball hits and explodes.               */
	    /* The explosion.                            */
		for (i = y - max_dis; i <= y + max_dis; i++)
		    for (j = x - max_dis; j <= x + max_dis; j++)
			if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) &&
			 los(char_row, char_col, i, j) && los(y, x, i, j) &&
			    floor_grid_bold(i, j) &&
			    panel_contains(i, j) && (py.flags1.blind < 1)) {
#ifdef TC_COLOR
			    if (!no_color_flag)
				textcolor(bolt_color(typ));
#endif
			    print('*', i, j);
#ifdef TC_COLOR
			    /* prob don't need here, but... -CFT */
			    if (!no_color_flag)
				textcolor(LIGHTGRAY);
#endif
			}
		if (py.flags1.blind < 1) {
		    put_qio();
#ifdef MSDOS
		    delay(25 * delay_spd);	/* milliseconds */
#else
		    usleep(25000 * delay_spd);	/* useconds */
#endif
		}

	    /* now erase the ball, since effects below may use msg_print, and
	     * pause indefinitely, so we want ball gone before then -CFT 
	     */
		for (i = y - max_dis; i <= y + max_dis; i++)
		    for (j = x - max_dis; j <= x + max_dis; j++)
			if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) &&
			 los(char_row, char_col, i, j) && los(y, x, i, j) &&
			    floor_grid_bold(i, j) &&
			    panel_contains(i, j) && (py.flags1.blind < 1)) {
			    lite_spot(i, j);	/* draw what is below the '*' */
			}
		put_qio();

	    /* First go over the area of effect, and destroy items...  Any
	     * preexisting items will be affected, but items dropped by
	     * killed monsters are assummed to have been "shielded" from the
	     * effects the the monster's corpse.  This means that you no
	     * longer have to be SO paranoid about using fire/frost/acid
	     * balls. -CFT 
	     */
		for (i = y - max_dis; i <= y + max_dis; i++)
		    for (j = x - max_dis; j <= x + max_dis; j++)
			if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
			    && los(y, x, i, j) && (cave[i][j].i_idx != 0) &&
			    (*destroy) (&i_list[cave[i][j].i_idx]))
			    (void)delete_object(i, j);
		/* burn/corrode or OW destroy items in area of effect */
	    /* now go over area of affect and DO something to monsters... */
		for (i = y - max_dis; i <= y + max_dis; i++)
		    for (j = x - max_dis; j <= x + max_dis; j++)
			if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
			    && los(y, x, i, j)) {
			    c_ptr = &cave[i][j];
			    if (floor_grid_bold(i, j)) {
				if (c_ptr->m_idx > 1) {
				    dam = dam_hp;
				    m_ptr = &m_list[c_ptr->m_idx];
				    spell_hit_monster(m_ptr, typ, &dam,
				      (distance(i, j, y, x) + 1), &ny, &nx, TRUE);
				    c_ptr = &cave[ny][nx];
			/* may be new location if teleported by gravity warp... */
				    m_ptr = &m_list[c_ptr->m_idx];
			/* and even if not, may be new monster if chaos polymorphed */
				    r_ptr = &r_list[m_ptr->r_idx];
				    monptr = c_ptr->m_idx;

				/*
				 * lite up creature if visible, temp set pl
				 * so that update_mon works 
				 */
				    tmp = c_ptr->pl;
				    c_ptr->pl = TRUE;
				    update_mon((int)c_ptr->m_idx);

				    thit++;
				    if (dam < 1)
					dam = 1;	/* protect vs neg damage -CFT */
				    k = mon_take_hit((int)c_ptr->m_idx, dam, TRUE);
				    if (k >= 0)
					tkill++;
				    c_ptr->pl = tmp;
				}
				lite_spot(i, j);	/* erase the ball... */
			    }
			}
	    /* show ball of whatever */
		put_qio();

	    /* End  explosion.                   */
		if (tkill == 1) {
		    msg_print("There is a scream of agony!");
		} else if (tkill > 1) {
		    msg_print("There are several screams of agony!");
		}
		if (tkill >= 0)
		    prt_experience();
	    /* End ball hitting.                 */
	    } else if (panel_contains(y, x) && (py.flags1.blind < 1)) {
#ifdef TC_COLOR
		if (!no_color_flag)
		    textcolor(bolt_color(typ));
#endif
		print(bolt_char, y, x);
		put_qio();
#ifdef TC_COLOR
		if (!no_color_flag)
		    textcolor(LIGHTGRAY);
#endif
#ifdef MSDOS
		delay(8 * delay_spd);	/* milliseconds */
#else
		usleep(8000 * delay_spd);	/* useconds */
#endif
	    }
	    oldy = y;
	    oldx = x;
#ifdef TARGET
	    if (target_mode && target_at(y,x))
		flag = TRUE; /* must have hit "targetted" area -CFT */
#endif
	}
    } while (!flag);
}


/* Shoot a bolt in a given direction			-RAK-	 */
void fire_bolt(int typ, int dir, int y, int x, int dam_hp)
{
    int                 i, oldy, oldx, dist, flag;
/*    u32b              harm_type = 0; */
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    vtype               out_val, m_name;
    int                 dam = dam_hp;
    int                 ny, nx;
    char                bolt_char;

    flag = FALSE;
    oldy = y;
    oldx = x;
    dist = 0;
    do {
	ny = y;
	nx = x;
	(void)mmove(dir, &y, &x);
    /* choose the right shape for the bolt... -CFT */
	if (ny == y)
	    bolt_char = '-';
	else if (nx == x)
	    bolt_char = '|';
	else if ((ny - y) == (nx - x))
	    bolt_char = '\\';
	else
	    bolt_char = '/';

	dist++;
	c_ptr = &cave[y][x];
	lite_spot(oldy, oldx);
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else {
	    if (c_ptr->m_idx > 1) {
		flag = TRUE;
		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_list[m_ptr->r_idx];
	    /*
	     * light up monster and draw monster, temporarily set pl so that
	     * update_mon() will work 
	     */
		i = c_ptr->pl;
		c_ptr->pl = TRUE;
		update_mon((int)c_ptr->m_idx);
		c_ptr->pl = i;
	    /* draw monster and clear previous bolt */
		put_qio();

		spell_hit_monster(m_ptr, typ, &dam, 0, &ny, &nx, TRUE);
		c_ptr = &cave[ny][nx];	/* may be new location if teleported
					 * by gravity warp... */
		m_ptr = &m_list[c_ptr->m_idx];	/* and even if not, may be
						 * new monster if chaos
						 * polymorphed */
		r_ptr = &r_list[m_ptr->r_idx];
		monster_name(m_name, m_ptr, r_ptr);

		if ((dam > 0) && (m_ptr->hp >= dam)) {
		    (void)sprintf(out_val,
			       pain_message((int)c_ptr->m_idx, dam), m_name);
		    msg_print(out_val);
		}

		i = mon_take_hit((int)c_ptr->m_idx, dam, TRUE);

		if (i >= 0)
		    prt_experience();

	    } else if (panel_contains(y, x) && (py.flags1.blind < 1)) {
		print(bolt_char, y, x);
	    /* show the bolt */
		put_qio();
#ifdef MSDOS
		delay(8 * delay_spd);	/* milliseconds */
#else
		usleep(8000 * delay_spd);	/* useconds */
#endif
	    }
	}
	oldy = y;
	oldx = x;
#ifdef TARGET
	if (target_mode && target_at(y,x))
	    flag = TRUE; /* must have hit "targeted" area -CFT */
#endif
    }
    while (!flag);
    lite_spot(oldy, oldx);	   /* just in case, clear any leftover bolt images -CFT */
}


/* This fn provides the ability to have a spell blast a line of creatures
   for damage.  It should look pretty neat, too... -CFT */
void line_spell(int typ, int dir, int y, int x, int dam)
{
    int ny,nx, dis = 0, flag = FALSE;
    int t, tdam;
    monster_type *m_ptr;
    cave_type *c_ptr;
    byte path[OBJ_BOLT_RANGE+5][3]; /* pre calculate "flight" path, makes bolt
					calc faster because fns more likely to be in mem.
					Also allows redraw at reasonable spd -CFT */  

    path[0][0] = y;  path[0][1] = x; /* orig point */
    do {
	(void)mmove(dir, &y, &x);
	dis++;
	path[dis][0] = y;  path[dis][1] = x;
	if ((dis>OBJ_BOLT_RANGE) || (cave[y][x].fval >= MIN_CLOSED_SPACE))
	    flag = TRUE;
    } while (!flag);

    flag = FALSE;
    dis = 0;
    do {
	dis++;
	y = path[dis][0];  x = path[dis][1];
	c_ptr = &cave[y][x];
	if ((dis > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;	/* then stop */
	else {
	    if (c_ptr->m_idx > 1) { /* hit a monster! */
		tdam = dam;
		m_ptr = &m_list[c_ptr->m_idx];

		if (!(py.flags1.status & PY_BLIND) && panel_contains(y,x)){
		    /* temp light monster to show it... */
		    t = c_ptr->pl;
		    c_ptr->pl = TRUE;
		    update_mon((int)c_ptr->m_idx);
		    c_ptr->pl = t;
		    put_qio();	/* draw monster */
		}

		/* check resists */
		spell_hit_monster(m_ptr, typ, &tdam, 1, &ny, &nx, TRUE);
		c_ptr = &cave[ny][nx]; /* may be new loc if tele by grav warp */

		(void) mon_take_hit((int)c_ptr->m_idx, tdam, TRUE); /* hurt it */
	    }
	    if (!(py.flags1.status & PY_BLIND)) {
		for(t=1;t<=dis;t++)
		    if (panel_contains(path[t][0],path[t][1])){
#ifdef TC_COLOR
			if (!no_color_flag) textcolor(bolt_color(typ));
#endif
			print(bolt_char(path[t][0],path[t][1],path[t-1][0], path[t-1][1]),
			      path[t][0], path[t][1]);
#ifdef TC_COLOR
			if (!no_color_flag) textcolor(LIGHTGRAY);
#endif
		    }
		put_qio();	/* show line */
#ifdef MSDOS
		delay(8 * delay_spd);
#else
		usleep(8000 * delay_spd);
#endif      
	    } /* if !blind */
	} /* if hit monster */
    } while (!flag);		/* end of effects loop */
  
    if (!(py.flags1.status & PY_BLIND)) { /* now erase it -CFT */
	for(t=1;t<=dis;t++){	/* erase piece-by-piece... */
	    lite_spot(path[t][0], path[t][1]);
	    for(tdam=t+1;tdam<dis;tdam++){
		if (panel_contains(path[tdam][0], path[tdam][1])){
#ifdef TC_COLOR
		    if (!no_color_flag) textcolor(bolt_color(typ));
#endif
		    print(bolt_char(path[tdam][0],path[tdam][1],path[tdam-1][0],
				    path[tdam-1][1]), path[tdam][0], path[tdam][1]);
#ifdef TC_COLOR
		    if (!no_color_flag) textcolor(LIGHTGRAY);
#endif
		}
	    }
	    put_qio();
#ifdef MSDOS
	    delay(8 * delay_spd);
#else
	    usleep(8000 * delay_spd);
#endif      
	} /* for each piece */
    } /* if !blind */
}


/* Leave a line of light in given dir, blue light can sometimes	 */
/* hurt creatures.				       -RAK-   */
void lite_line(int dir, int y, int x)
{
    register cave_type *c_ptr;
    int                 dist, flag;

    dist = (-1);
    flag = FALSE;
    do {
    /* put mmove at end because want to light up current spot */
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	if (!c_ptr->pl && !c_ptr->tl) {
	/* set pl so that lite_spot will work */
	    c_ptr->pl = TRUE;
	    if (c_ptr->fval == LIGHT_FLOOR) {
		if (panel_contains(y, x))
		    light_room(y, x);
	    } else
		lite_spot(y, x);
	}
    /* set pl in case tl was true above */
	c_ptr->pl = TRUE;
	mon_light_dam(y, x, damroll(6, 8));
	(void)mmove(dir, &y, &x);
    }
    while (!flag);
}


/* Drains life; note it must be living.		-RAK-	 */
int drain_life(int dir, int y, int x, int dam)
{
    register int            i;
    int                     flag, dist, drain;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    drain = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    flag = TRUE;
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    if (((r_ptr->cflags2 & MF2_UNDEAD) == 0) &&
		((r_ptr->cflags2 & MF2_DEMON) == 0) &&
		(r_ptr->r_char != 'E' && r_ptr->r_char != 'g' && r_ptr->r_char != 'v')) {
		drain = TRUE;
		monster_name(m_name, m_ptr, r_ptr);
		i = mon_take_hit((int)c_ptr->m_idx, dam, TRUE);
		if (i >= 0) {
		    (void)sprintf(out_val, "%s dies in a fit of agony.", m_name);
		    msg_print(out_val);
		    prt_experience();
		} else {
		    (void)sprintf(out_val,
			       pain_message((int)c_ptr->m_idx, dam), m_name);
		    msg_print(out_val);
		}
	    } else {
		if (r_ptr->cflags2 & MF2_UNDEAD)
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_UNDEAD;
		else
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_DEMON;
	    }
	}
    }
    while (!flag);
    return (drain);
}


/* Turn stone to mud, delete wall.			-RAK-	 */
int wall_to_mud(int dir, int y, int x)
{
    int                     i, wall, dist;
    bigvtype                out_val, tmp_str;
    register int            flag;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   m_name;

    wall = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
    /* note, this ray can move through walls as it turns them to mud */
	if (dist == OBJ_BOLT_RANGE)
	    flag = TRUE;
	if (c_ptr->fval == BOUNDARY_WALL) {
	    flag = TRUE;
	    if (test_lite(y, x))
		msg_print("The wall resists your spell.");
	} else if ((c_ptr->fval >= MIN_WALL)) {
	    flag = TRUE;
	    (void)twall(y, x, 1, 0);
	    if (test_lite(y, x)) {
		msg_print("The wall turns into mud.");
		check_view();
		wall = TRUE;
	    }
	} else if ((c_ptr->i_idx != 0) && (c_ptr->fval >= MIN_CLOSED_SPACE)) {
	    flag = TRUE;
	    if (panel_contains(y, x) && test_lite(y, x)) {		
		objdes(tmp_str, &i_list[c_ptr->i_idx], FALSE);
		if ((i_list[c_ptr->i_idx].tval == TV_RUBBLE) && (randint(10)==1)) {
		    delete_object(y,x);
		    place_object(y,x);
		    lite_spot(y,x);
		    (void) sprintf(out_val,
				   "The %s turns into mud, revealing an object!",\
				   tmp_str);
		}
		else {
		    (void) delete_object(y, x);
		    (void) sprintf(out_val, "The %s turns into mud.", tmp_str);
		}
		msg_print(out_val);
		wall = TRUE;
	    }
	}
	
	if (c_ptr->m_idx > 1) {
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    if (MF2_HURT_ROCK & r_ptr->cflags2) {
		monster_name(m_name, m_ptr, r_ptr);
		flag = m_ptr->ml;
		i = mon_take_hit((int)c_ptr->m_idx, (20 + randint(30)), TRUE);
		if (flag) {
		    if (i >= 0) {
			l_list[i].r_cflags2 |= MF2_HURT_ROCK;
			(void)sprintf(out_val, "%s dissolves!", m_name);
			msg_print(out_val);
			prt_experience();	/* print msg before calling prt_exp */
		    } else {
			l_list[m_ptr->r_idx].r_cflags2 |= MF2_HURT_ROCK;
			(void)sprintf(out_val, "%s grunts in pain!", m_name);
			msg_print(out_val);
		    }
		}
		flag = TRUE;
	    }
	}
    }
    while (!flag);
    return (wall);
}


/* Destroy all traps and doors in a given direction	-RAK-	 */
int td_destroy2(int dir, int y, int x)
{
    register int         destroy2, dist;
    register cave_type  *c_ptr;
    register inven_type *t_ptr;

    destroy2 = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
    /* must move into first closed spot, as it might be a secret door */
	if (c_ptr->i_idx != 0) {
	    t_ptr = &i_list[c_ptr->i_idx];
	    if (t_ptr->tval == TV_CHEST) /* let's untrap it instead -CWS */
		t_ptr->flags1 &= ~(CH_TRAPPED | CH_LOCKED);
	    else if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_VIS_TRAP) ||
		     (t_ptr->tval == TV_OPEN_DOOR) || (t_ptr->tval == TV_CLOSED_DOOR)
		     || (t_ptr->tval == TV_SECRET_DOOR)) {
		if (delete_object(y, x)) {
		    msg_print("There is a bright flash of light!");
		    destroy2 = TRUE;
		}
	    }
	}
    }
    while ((dist <= OBJ_BOLT_RANGE) || floor_grid_bold(y, x));
    return (destroy2);
}


/* Disarms all traps/chests in a given direction	-RAK-	 */
int disarm_all(int dir, int y, int x)
{
    register cave_type  *c_ptr;
    register inven_type *t_ptr;
    register int         disarm, dist;

    disarm = FALSE;
    dist = (-1);
    do {
    /* put mmove at end, in case standing on a trap */
	dist++;
	c_ptr = &cave[y][x];

    /* note, must continue upto and including the first non open space,
     * because secret doors have fval greater than MAX_OPEN_SPACE 
     */
	if (c_ptr->i_idx != 0) {
	    t_ptr = &i_list[c_ptr->i_idx];
	    if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_VIS_TRAP)) {
		if (delete_object(y, x))
		    disarm = TRUE;
	    } else if (t_ptr->tval == TV_CLOSED_DOOR)
		t_ptr->pval = 0;	   /* Locked or jammed doors become merely closed. */
	    else if (t_ptr->tval == TV_SECRET_DOOR) {
		c_ptr->fm = TRUE;

		/* change secret door to closed door */
		i_list[c_ptr->i_idx].index = OBJ_CLOSED_DOOR;
		i_list[c_ptr->i_idx].tval = k_list[OBJ_CLOSED_DOOR].tval;
		i_list[c_ptr->i_idx].tchar = k_list[OBJ_CLOSED_DOOR].tchar;
		lite_spot(y, x);

		disarm = TRUE;
	    } else if ((t_ptr->tval == TV_CHEST) && (t_ptr->flags1 != 0)) {
		msg_print("Click!");
		t_ptr->flags1 &= ~(CH_TRAPPED | CH_LOCKED);
		disarm = TRUE;
		t_ptr->name2 = EGO_UNLOCKED;
		known2(t_ptr);
	    }
	}
	(void)mmove(dir, &y, &x);
    }
    while ((dist <= OBJ_BOLT_RANGE) && floor_grid_bold(y, x));
    return (disarm);
}

/* Destroys any adjacent door(s)/trap(s)		-RAK-	 */
int td_destroy()
{
    register int        i, j, destroy;
    register cave_type *c_ptr;

    destroy = FALSE;
    for (i = char_row - 1; i <= char_row + 1; i++)
	for (j = char_col - 1; j <= char_col + 1; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->i_idx != 0) {
		if (((i_list[c_ptr->i_idx].tval >= TV_INVIS_TRAP) &&
		     (i_list[c_ptr->i_idx].tval <= TV_CLOSED_DOOR) &&
		     (i_list[c_ptr->i_idx].tval != TV_RUBBLE)) ||
		    (i_list[c_ptr->i_idx].tval == TV_SECRET_DOOR)) {
		    if (delete_object(i, j))
			destroy = TRUE;
		} else if (i_list[c_ptr->i_idx].tval == TV_CHEST) {
		/* destroy traps on chest and unlock */
		    i_list[c_ptr->i_idx].flags1 &= ~(CH_TRAPPED | CH_LOCKED);
		    i_list[c_ptr->i_idx].name2 = EGO_DISARMED;
		    msg_print("You have disarmed the chest.");
		    known2(&i_list[c_ptr->i_idx]);
		    destroy = TRUE;
		}
	    }
	}
    return (destroy);
}

/* Surround the player with doors.			-RAK-	 */
int door_creation()
{
    register int        i, j, door;
    int                 k;
    register cave_type *c_ptr;

    door = FALSE;
    for (i = char_row - 1; i <= char_row + 1; i++)
	for (j = char_col - 1; j <= char_col + 1; j++)
	    if ((i != char_row) || (j != char_col)) {
		c_ptr = &cave[i][j];
		if (c_ptr->fval <= MAX_CAVE_FLOOR) {
		    if ((c_ptr->i_idx == 0) ||
			((i_list[c_ptr->i_idx].tval != TV_UP_STAIR) 
				/* if not stairs or a store */
			 &&(i_list[c_ptr->i_idx].tval != TV_DOWN_STAIR)
			 && (i_list[c_ptr->i_idx].tval != TV_STORE_DOOR)) ||
			(i_list[c_ptr->i_idx].tval < TV_MIN_WEAR) ||
			(i_list[c_ptr->i_idx].tval > TV_MAX_WEAR) ||
			!(i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT)) {
				/* if no artifact here -CFT */
			door = TRUE;
			if (c_ptr->i_idx != 0)
			    (void)delete_object(i, j);
			k = i_pop();
			c_ptr->fval = BLOCKED_FLOOR;
			c_ptr->i_idx = k;
			invcopy(&i_list[k], OBJ_CLOSED_DOOR);
			lite_spot(i, j);
		    } else
			msg_print("The object resists the spell.");
		}
	    }
    return (door);
}


/* Surround the fool with traps (chuckle)		-RAK-	 */
int trap_creation()
{
    register int        i, j, trap;
    register cave_type *c_ptr;

    trap = FALSE;
    for (i = char_row - 1; i <= char_row + 1; i++)
	for (j = char_col - 1; j <= char_col + 1; j++) {
	    if ((i == char_row) && (j == char_col))
		continue;	   /* no trap under player, from um55 -CFT */
	    c_ptr = &cave[i][j];
	    if (c_ptr->fval <= MAX_CAVE_FLOOR) {
		if ((c_ptr->i_idx == 0) ||
		    ((i_list[c_ptr->i_idx].tval != TV_UP_STAIR)
		     /* if not stairs or a store */
		     &&(i_list[c_ptr->i_idx].tval != TV_DOWN_STAIR)
		     && (i_list[c_ptr->i_idx].tval != TV_STORE_DOOR)) ||
		    (i_list[c_ptr->i_idx].tval < TV_MIN_WEAR) ||
		    (i_list[c_ptr->i_idx].tval > TV_MAX_WEAR) ||
		    !(i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT)) {
				/* if no artifact here -CFT */
		    trap = TRUE;
		    if (c_ptr->i_idx != 0)
			(void)delete_object(i, j);
		    place_trap(i, j, randint(MAX_TRAP) - 1);
		/* don't let player gain exp from the newly created traps */
		    i_list[c_ptr->i_idx].pval = 0;
		/* open pits are immediately visible, so call lite_spot */
		    lite_spot(i, j);
		} else
		    msg_print("The object resists the spell.");
	    }
	}
    return (trap);
}


/* Increase or decrease a creatures hit points		-RAK-	 */
int hp_monster(int dir, int y, int x, int dam)
{
    register int        i;
    int                 flag, dist, monster;
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    vtype               out_val, m_name;

    monster = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    flag = TRUE;
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    monster_name(m_name, m_ptr, r_ptr);
	    monster = TRUE;
	    i = mon_take_hit((int)c_ptr->m_idx, dam, TRUE);
	    if (i >= 0) {
		(void)sprintf(out_val, "%s dies in a fit of agony.", m_name);
		msg_print(out_val);
		prt_experience();
	    } else if (dam > 0) {
		(void)sprintf(out_val,
			      pain_message((int)c_ptr->m_idx, dam), m_name);
		msg_print(out_val);
	    }
	}
    }
    while (!flag);
    return (monster);
}


/* Increase or decrease a creatures speed		-RAK-	 */
/* NOTE: cannot slow a winning creature (BALROG)		 */
int speed_monster(int dir, int y, int x, int spd)
{
    int                    flag, dist, speed;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    speed = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    flag = TRUE;
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    monster_name(m_name, m_ptr, r_ptr);
	    if (spd > 0) {
		m_ptr->mspeed += spd;
		m_ptr->csleep = 0;
		(void)sprintf(out_val, "%s starts moving faster.", m_name);
		msg_print(out_val);
		speed = TRUE;
	    } else if ((r_ptr->level >
			randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
		       (r_ptr->cflags2 & MF2_UNIQUE)) {
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
		m_ptr->csleep = 0;
	    } else {
		m_ptr->mspeed += spd;
		m_ptr->csleep = 0;
		(void)sprintf(out_val, "%s starts moving slower.", m_name);
		msg_print(out_val);
		speed = TRUE;
	    }
	}
    }
    while (!flag);
    return (speed);
}


/* Sleep a creature.					-RAK-	 */
int sleep_monster(int dir, int y, int x)
{
    int                     flag, dist, sleep;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    sleep = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];

	    flag = TRUE;
	    monster_name(m_name, m_ptr, r_ptr);
	    if ((r_ptr->level >
	    randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
	    (r_ptr->cflags2 & MF2_UNIQUE) || (r_ptr->cflags2 & MF2_CHARM_SLEEP)) {
		if (m_ptr->ml && (r_ptr->cflags2 & MF2_CHARM_SLEEP))
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_CHARM_SLEEP;
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
	    } else {
		m_ptr->csleep = 500;
		sleep = TRUE;
		(void)sprintf(out_val, "%s falls asleep.", m_name);
		msg_print(out_val);
	    }
	}
    }
    while (!flag);
    return (sleep);
}


/* Confuse a creature					-RAK-	 */
int confuse_monster(int dir, int y, int x, int lvl)
{
    int                     flag, dist, confuse;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    confuse = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    monster_name(m_name, m_ptr, r_ptr);
	    flag = TRUE;
	    if ((r_ptr->level >
	    randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
		(r_ptr->cflags2 & MF2_UNIQUE ||
		 r_ptr->spells2 & (MS2_BR_CONF | MS2_BR_CHAO))) {
		if (m_ptr->ml && (r_ptr->cflags2 & MF2_CHARM_SLEEP))
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_CHARM_SLEEP;
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
		m_ptr->csleep = 0;
	    } else {
		if (m_ptr->confused < 230)
		    m_ptr->confused += (byte) (damroll(3, (lvl / 2)) + 1);
		confuse = TRUE;
		m_ptr->csleep = 0;
		(void)sprintf(out_val, "%s appears confused.", m_name);
		msg_print(out_val);
	    }
	}
    }
    while (!flag);
    return (confuse);
}


/* Scare a creature -DGK */
int fear_monster(int dir, int y, int x, int lvl)
{
    int                     flag, dist, fear;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;

    fear = FALSE;
    flag = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    monster_name(m_name, m_ptr, r_ptr);
	    flag = TRUE;
	    if ((r_ptr->level >
	    randint((py.misc.lev - 10) < 1 ? 1 : (py.misc.lev - 10)) + 10) ||
		(r_ptr->cflags2 & MF2_UNIQUE)) {
		if (m_ptr->ml && (r_ptr->cflags2 & MF2_CHARM_SLEEP))
		    l_list[m_ptr->r_idx].r_cflags2 |= MF2_CHARM_SLEEP;
		(void)sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
		m_ptr->csleep = 0;
	    } else {
		if (m_ptr->monfear < 175)
		    m_ptr->monfear += (byte) (damroll(3, (lvl / 2)) + 1);
		fear = TRUE;
		m_ptr->csleep = 0;
		(void)sprintf(out_val, "%s flees in terror!", m_name);
		msg_print(out_val);
	    }
	}
    }
    while (!flag);
    return (fear);
}


/* polymorph now safer.  not safe, just safer -CFT */
/* Polymorph a monster                                  -RAK-   */
/* NOTE: cannot polymorph a winning creature (BALROG)            */
int poly_monster(int dir, int y, int x)
{
    int                     dist, flag, flag2, p;
    register cave_type     *c_ptr;
    register monster_race *r_ptr;
    register monster_type  *m_ptr;
    vtype                   out_val, m_name;
    
    p = FALSE;
    flag = FALSE;
    flag2= FALSE;
    dist = 0;
    do {
	(void) mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_ptr = &m_list[c_ptr->m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    if ((r_ptr->level < randint((py.misc.lev-10)<1?1:(py.misc.lev-10))+10)
                && !(r_ptr->cflags2 & MF2_UNIQUE)) {
		poly(c_ptr->m_idx);
		if (panel_contains(y, x) && (c_ptr->tl || c_ptr->pl))
		    p = TRUE;
	    } else {
		monster_name (m_name, m_ptr, r_ptr);
		(void) sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
	    }
	}
    }
    while (!flag);
    return(p);
}


/* Replicate a creature					-RAK-	 */
int clone_monster(int dir, int y, int x)
{
    register cave_type *c_ptr;
    register int        dist, flag;

    dist = 0;
    flag = FALSE;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_list[c_ptr->m_idx].csleep = 0;
	/* monptr of 0 is safe here, since can't reach here from creatures */
	    return multiply_monster(y, x, (int)m_list[c_ptr->m_idx].r_idx, 0);
	}
    }
    while (!flag);
    return (FALSE);
}


/* Teleport all creatures in a given direction away	-RAK-	 */
int teleport_monster(int dir, int y, int x)
{
    register int        flag, result, dist;
    register cave_type *c_ptr;

    flag = FALSE;
    result = FALSE;
    dist = 0;
    do {
	(void)mmove(dir, &y, &x);
	dist++;
	c_ptr = &cave[y][x];
	if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	    flag = TRUE;
	else if (c_ptr->m_idx > 1) {
	    m_list[c_ptr->m_idx].csleep = 0;	/* wake it up */
	    teleport_away((int)c_ptr->m_idx, MAX_SIGHT * 5);
	    result = TRUE;
	}
    }
    while (!flag);
    return (result);
}


/* Split out of lite_line.       -DGK */
void mon_light_dam(int y, int x, int dam)
{
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;
    int                     i;

    c_ptr = &cave[y][x];
    if (c_ptr->m_idx > 1) {
	m_ptr = &m_list[c_ptr->m_idx];
	r_ptr = &r_list[m_ptr->r_idx];
	monster_name(m_name, m_ptr, r_ptr);
	m_ptr->csleep = 0;
	if (MF2_HURT_LITE & r_ptr->cflags2) {
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_HURT_LITE;
	    i = mon_take_hit((int)c_ptr->m_idx, dam, FALSE);
	    if (i >= 0) {
		(void)sprintf(out_val, "%s shrivels away in the light!", m_name);
		msg_print(out_val);
		prt_experience();
	    } else {
		(void)sprintf(out_val, "%s cringes from the light!", m_name);
		msg_print(out_val);
	    }
	}
    }
}


int lite_area(int y, int x, int dam, int rad)	   /* Expanded -DGK */
{
    register int i, j;
    int          min_i, max_i, min_j, max_j;

    if (rad < 1) rad = 1;	/* sanity check -CWS */
    if (py.flags1.blind < 1)
	msg_print("You are surrounded by a white light.");

    if ((cave[y][x].fval == LIGHT_FLOOR) && (panel_contains(y, x)))
	light_room(y, x);

    if (cave[y][x].lr && (dun_level > 0) && !(cave[y][x].pl))
		light_room(y, x); /* dbd - fix lighting radius */

    /* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
    min_i = MY_MAX(0, (y - rad));
    max_i = MY_MIN(cur_height - 1, (y + rad));
    min_j = MY_MAX(0, (x - rad));
    max_j = MY_MIN(cur_width - 1, (x + rad));

    for (i = min_i; i <= max_i; i++)
	for (j = min_j; j <= max_j; j++)
	    if (los(y, x, i, j) && (distance(y, x, i, j) <= rad)) {
		if (cave[i][j].lr && (dun_level > 0))
		    light_room(i, j);
		cave[i][j].pl = TRUE;
		lite_spot(i, j);
		if (dam)
		    mon_light_dam(i, j, dam / (rad == 0 ? 1 : rad));
	    }
    return (TRUE);
}


/* Darken an area, opposite of light area		-RAK-	 */
int unlite_area(int y, int x)
{
    register int        i, j, unlight;
    register cave_type *c_ptr;
    int                 min_i, max_i, min_j, max_j;

    unlight = FALSE;
    if (cave[y][x].lr && (dun_level > 0)) {
	darken_room(y, x);
	unlight = TRUE;
/* this isn't really good, as it returns true, even if rm was already dark, but
 * at least scrolls of darkness will be IDed when used -CFT
 */
    } else {
	min_i = MY_MAX(0, (y - 3));
	max_i = MY_MIN(cur_height - 1, (y + 3));
	min_j = MY_MAX(0, (x - 3));
	max_j = MY_MIN(cur_width - 1, (x + 3));
	
	/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
	
	for (i = min_i; i <= max_i; i++)
	    for (j = min_j; j <= max_j; j++) {
		c_ptr = &cave[i][j];
		if ((c_ptr->fval == CORR_FLOOR) && (c_ptr->pl || c_ptr->lr)) {
		    /* pl could have been set by star-lite wand, etc */
		    c_ptr->pl = FALSE;
		    c_ptr->tl = FALSE;
		    unlight = TRUE;
		}
	    }
    }
    if (unlight && py.flags1.blind <= 0)
	msg_print("Darkness surrounds you.");

    return (unlight);
}


#define NO_RES 0
#define SOME_RES 1
#define RESIST 2
#define IMMUNE 3
#define SUSCEPT 4
#define CHANGED 5
#define CONFUSED 6
#define MORE_CONF 7
#define DAZED 8
#define MORE_DAZED 16
#define DEAD 32

/* This function will process a bolt/ball/breath spell hitting a monster.
 * It checks for resistances, and reduces damage accordingly, and also
 * adds in what "special effects" apply to the monsters.  'rad' is used to
 * indicate the distance from "ground 0" for ball spells.  For bolts, rad
 * should be a 0 (this flags1 off some of the messages).  dam is changed
 * to reflect resistances and range. -CFT
 */

static void spell_hit_monster(monster_type *m_ptr, int typ, int *dam, int rad, int *y, int *x, byte by_player)
{
    register monster_race *r_ptr;
    int blind = (py.flags1.status & PY_BLIND) ? 1 : 0;
    int res;			/* controls messages, using above #defines -CFT */
    vtype cdesc, outval;

    if (rad)
	*dam /= rad;		/* adjust damage for range... */

    *y = m_ptr->fy;		/* these only change if mon gets teleported */
    *x = m_ptr->fx; 
    r_ptr = &r_list[m_ptr->r_idx];
    if (m_ptr->ml){
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    sprintf(cdesc, "%s ", r_ptr->name);
	else
	    sprintf(cdesc, "The %s ", r_ptr->name);
    }
    else
	strcpy(cdesc, "It ");

    res = NO_RES;		/* assume until we know different -CFT */
    switch ( typ ){		/* check for resists... */
      case GF_MISSILE:	/* pure damage, no resist possible */
	break;
      case GF_ELEC:
	if (r_ptr->cflags2 & MF2_IM_ELEC) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_ELEC;
        }
	break;
      case GF_POIS:
	if (r_ptr->cflags2 & MF2_IM_POIS) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_POIS;
        }
	break;
      case GF_ACID:
	if (r_ptr->cflags2 & MF2_IM_ACID) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_ACID;
        }
	break;
      case GF_COLD:
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_COLD;
        }
	break;
      case GF_FIRE:
	if (r_ptr->cflags2 & MF2_IM_FIRE) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_FIRE;
        }
	break;
      case GF_HOLY_ORB:
	if (r_ptr->cflags2 & MF2_EVIL) {
	    *dam *= 2;
	    res = SUSCEPT;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_EVIL;
        }
	break;
      case GF_ARROW:		/* for now, no defense... maybe it should have a
				   chance of missing? -CFT */
	break;
      case GF_PLASMA:		/* maybe MF2_IM_ELEC (ball lightning is supposed
				   to be plasma) or MF2_IM_FIRE (since it's hot)? -CFT */
	if (!strncmp("Plasma", r_ptr->name, 6) ||
	    (r_ptr->spells3 & MS3_BR_PLAS)){ /* if is a "plasma" monster,
					      or can breathe plasma, then
					      we assume it should be immune.
					      plasma bolts don't count, since
					      mage-types could have them, and
					      not deserve plasma-resist -CFT */
	    res = RESIST;
	    *dam *= 3;		/* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
	}
	break;
      case GF_NETHER:		/* I assume nether is an evil, necromantic force,
				   so it doesn't hurt undead, and hurts evil less -CFT */
	if (r_ptr->cflags2 & MF2_UNDEAD) {
	    res = IMMUNE;
	    *dam = 0;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_UNDEAD;
        }
	else if (r_ptr->spells2 & MS2_BR_LIFE) { /* if can breath nether, should get
						  good resist to damage -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
	}
	else if (r_ptr->cflags2 & MF2_EVIL) {
	    *dam /= 2;	/* evil takes *2 for holy, so /2 for this... -CFT */
	    res = SOME_RES;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_EVIL;
        }
	break;
      case GF_WATER:	/* water elementals should resist.  anyone else? -CFT */
	if ((r_ptr->r_char == 'E') && (r_ptr->name[0] == 'W')){
	    res = IMMUNE;
	    *dam = 0; /* water spirit, water ele, and Waldern -CFT */
        }
	break;
      case GF_CHAOS:
	if (r_ptr->spells2 & MS2_BR_CHAO){ /* assume anything that breathes
					    choas is chaotic enough to deserve resistance... -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's gonna die */
	    !(r_ptr->spells2 & MS2_BR_CHAO) &&
	    !(r_ptr->cflags2 & MF2_UNIQUE) &&
	    (randint(90) > r_ptr->level)) { /* then we'll polymorph it -CFT */
	    res = CHANGED;
	    if (poly(cave[*y][*x].m_idx))
		*dam = 0; /* new monster was not hit by choas breath.  This also
			     makes things easier to handle */
	} /* end of choas-poly.  If was poly-ed don't bother confuse... it's
	     too hectic to keep track of... -CFT */
	else if (!(r_ptr->cflags2 & MF2_CHARM_SLEEP) &&
		 !(r_ptr->spells2 & MS2_BR_CHAO) && /* choatics hard to confuse */
		 !(r_ptr->spells2 & MS2_BR_CONF)){   /* so are bronze dragons */
	    if (m_ptr->confused > 0) { 
		res = MORE_CONF;
		if (m_ptr->confused < 240){ /* make sure not to overflow -CFT */
		    m_ptr->confused += 7/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = CONFUSED;
		m_ptr->confused = (randint(11)+5)/(rad>0 ? rad : 1);
	    }
	}
	break;
      case GF_SHARDS:
	if (r_ptr->spells2 & MS2_BR_SHAR){ /* shard breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_SOUND:
      if (r_ptr->spells2 & MS2_BR_SOUN){ /* ditto for sound -CFT */
	  res = RESIST;
	  *dam *= 2;
	  *dam /= (randint(6)+6);
      }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's dead */
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)) { /* sound and impact breathers
	  					should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res = MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)*2)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = DAZED;
		m_ptr->confused = (randint(15)+10)/(rad>0 ? rad : 1);
	    }
  	}
	break;
      case GF_CONFUSION:
	if (r_ptr->spells2 & MS2_BR_CONF){ 
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_CHARM_SLEEP){
	    res = SOME_RES;
	    *dam /= 2; /* only some resist, but they also avoid confuse -CFT */
        }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's dead */
	    !(r_ptr->cflags2 & MF2_CHARM_SLEEP) &&
	    !(r_ptr->spells2 & MS2_BR_CHAO) && /* choatics hard to confuse */
	    !(r_ptr->spells2 & MS2_BR_CONF)) {  /* so are bronze dragons */
	    if (m_ptr->confused > 0) { 
		res = MORE_CONF;
		if (m_ptr->confused < 240){ /* make sure not to overflow -CFT */
		    m_ptr->confused += 7/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = CONFUSED;
		m_ptr->confused = (randint(11)+5)/(rad>0 ? rad : 1);
	    }
	}
        break;
      case GF_DISENCHANT:
	if ((r_ptr->spells2 & MS2_BR_DISE) ||
	    !strncmp("Disen", r_ptr->name, 5)) {
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_NEXUS:
	if ((r_ptr->spells2 & MS2_BR_NETH) ||
	    !strncmp("Nexus", r_ptr->name, 5)) {
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_FORCE:
	if (r_ptr->spells3 & MS3_BR_WALL){ /* breath ele force resists
					    ele force -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	if ((*dam <= m_ptr->hp) &&
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)){ /* sound and impact breathers
					       should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res = MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)+1)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = DAZED;
		m_ptr->confused = randint(15)/(rad>0 ? rad : 1);
	    }
	}
	break;
      case GF_INERTIA:
	if (r_ptr->spells3 & MS3_BR_SLOW){ /* if can breath inertia, then
					    resist it. */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_LITE:
	if (r_ptr->spells3 & MS3_BR_LITE){ /* breathe light to res light */
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_HURT_LITE){
	    res = SUSCEPT;
	    *dam *= 2; /* hurt bad by light */
        }
	else if (r_ptr->spells3 & MS3_BR_DARK){ /* breathe dark gets hurt */
	    res = SUSCEPT;
	    *dam = (*dam * 3)/2;
        }
	break;
      case GF_DARK:
	if (r_ptr->spells2 & MS3_BR_DARK){ /* shard breathers resist -CFT */
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_HURT_LITE){
	    res = SOME_RES;
	    *dam /= 2; /* hurt bad by light, so not hurt bad by dark */
        }
	else if (r_ptr->spells3 & MS3_BR_LITE){ /* breathe light gets hurt */
	    res = SUSCEPT;
	    *dam = (*dam * 3)/2;
        }
	break;
      case GF_TIME:
	if (r_ptr->spells3 & MS3_BR_TIME){ /* time breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_GRAVITY:
	if (r_ptr->spells3 & MS3_BR_GRAV){ /* breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	else {
	    if (*dam <= m_ptr->hp) {
		teleport_away(cave[m_ptr->fy][m_ptr->fx].m_idx, 5);
		*y = m_ptr->fy; /* teleported, so let outside world know monster moved! */
		*x = m_ptr->fx; 
	    }
        }
	break;
      case GF_MANA: /* raw blast of power. no way to resist, is there? */
	break;
      case GF_METEOR: /* GF_METEOR is basically a powerful magic-missile
			 ball spell.  I only made it a different type
			 so I could make it a different color -CFT */
	break;
      case GF_ICE: /* ice is basically frost + cuts + stun -CFT */
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_COLD;
        }
	if ((*dam <= m_ptr->hp) &&
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)){  /* sound and impact breathers
	  					should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res += MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)+1)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res += DAZED;
		m_ptr->confused = randint(15)/(rad>0 ? rad : 1);
	    }
	}
	break;
      default:
	msg_print("Unknown typ in spell_hit_monster.  This may mean trouble.");
    } /* end switch for saving throws and extra effects */
    
    if (res == CHANGED)
	sprintf(outval, "%schanges!",cdesc);
    else if ((*dam > m_ptr->hp) &&
	     (by_player || !(r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE))) {
	res = DEAD;
	if ((r_list[m_ptr->r_idx].cflags2 & (MF2_DEMON|MF2_UNDEAD|MF2_MINDLESS)) ||
	    (r_list[m_ptr->r_idx].r_char == 'E') ||
	    (r_list[m_ptr->r_idx].r_char == 'v') ||
	    (r_list[m_ptr->r_idx].r_char == 'g') ||
	    (r_list[m_ptr->r_idx].r_char == 'X'))
	    sprintf(outval, "%sis destroyed.", cdesc);
	else
	    sprintf(outval, "%sdies.", cdesc);
    }
    else switch (res) {
      case NO_RES:
	sprintf(outval, "%sis hit.",cdesc);
	break;
      case SOME_RES:
	sprintf(outval, "%sresists somewhat.",cdesc);
	break;
      case RESIST:
	sprintf(outval, "%sresists.",cdesc);
	break;
      case IMMUNE:
	sprintf(outval, "%sis immune.",cdesc);
	break;
      case SUSCEPT:
	sprintf(outval, "%sis hit hard.",cdesc);
	break;
      case CONFUSED:
	sprintf(outval, "%sis confused.",cdesc);
	break;
      case MORE_CONF:
	sprintf(outval, "%sis more confused.",cdesc);
	break;
      case DAZED:
	sprintf(outval, "%sis dazed.",cdesc);
	break;
      case MORE_DAZED:
	sprintf(outval, "%sis more dazed.",cdesc);
	break;
      case (DAZED+RESIST):
	  sprintf(outval, "%sresists, but is dazed anyway.",cdesc);
	break;
      case (MORE_DAZED+RESIST):
	  sprintf(outval, "%sresists, but still is more dazed.",cdesc);
	break;
      default:
	sprintf(outval,"%sis affected in a mysterious way.",cdesc);
    }
    if (rad || (res != NO_RES)) { /* don't show normal hit msgs for bolts -CFT */
	if (!blind)
	    msg_print(outval);
    }	
}


/* Shoot a bolt in a given direction                    -RAK-   */
/* heavily modified to include exotic bolts -CFT */
void bolt(int typ, int y, int x, int dam_hp, char *ddesc, monster_type *ptr, int monptr)
{
    int                 i = ptr->fy, j = ptr->fx;
    int                 dam;
    u32b              tmp, treas;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    char                bolt_char;
    int                 blind = (py.flags1.status & PY_BLIND) ? 1 : 0;
    int                 ny, nx, sourcey, sourcex, dist;
    vtype               m_name, out_val;

    sourcey = i;
    sourcex = j;
    dist = 0;
    do {
    /* This is going along a badly angled line so call mmove2 direct */
	ny = i;
	nx = j;
	mmove2(&i, &j, sourcey, sourcex, char_row, char_col);
	dist++;

    /* choose the right shape for the bolt... -CFT */
	if (ny == i)
	    bolt_char = '-';
	else if (nx == j)
	    bolt_char = '|';
	else if ((ny - i) == (nx - j))
	    bolt_char = '\\';
	else
	    bolt_char = '/';

	if (in_bounds(i, j) && los(y, x, i, j)) {
	    c_ptr = &cave[i][j];
	    if (floor_grid_bold(i, j)) {
		if (panel_contains(i, j) && !(py.flags1.status & PY_BLIND)) {
		    print(bolt_char, i, j);
		    put_qio();
#ifdef MSDOS
		    delay(8 * delay_spd);	/* milliseconds */
#else
		    usleep(8000 * delay_spd);	/* useconds */
#endif
		    lite_spot(i, j);
		}
		if (c_ptr->m_idx > 1 && c_ptr->m_idx != monptr) {
		    m_ptr = &m_list[c_ptr->m_idx];
		    dam = dam_hp;

		    spell_hit_monster(m_ptr, typ, &dam, 0, &ny, &nx, FALSE);
				/* process hit effects */
		    /* may be new location if teleported by gravity warp... */
		    c_ptr = &cave[ny][nx];
		    /* and even if not, may be new monster if chaos polymorphed */
		    m_ptr = &m_list[c_ptr->m_idx];
		    r_ptr = &r_list[m_ptr->r_idx];
		    monster_name(m_name, m_ptr, r_ptr);

		    if (dam < 1)
			dam = 1;   /* protect vs neg damage -CFT */
		    m_ptr->hp = m_ptr->hp - dam;
		    m_ptr->csleep = 0;

/* prevent unique monster from death by other monsters.  It causes trouble
 * (monster not marked as dead, quest monsters don't satisfy quest, etc).
 * So, we let them live, but extremely wimpy. -CFT
 */
		    if ((r_ptr->cflags2 & MF2_UNIQUE) && (m_ptr->hp < 0))
			m_ptr->hp = 0;

		    if (m_ptr->hp < 0) {
			object_level = (dun_level + r_ptr->level) >> 1;
			coin_type = 0;
			get_coin_type(r_ptr);
			treas = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
					      r_ptr->cflags1, 0, 0);
			coin_type = 0;
			if (m_ptr->ml || (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)) {
			    tmp = (l_list[m_ptr->r_idx].r_cflags1 & CM_TREASURE)
				>> CM_TR_SHIFT;
			    if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT))
				treas = (treas & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
			    l_list[m_ptr->r_idx].r_cflags1 = treas |
				(l_list[m_ptr->r_idx].r_cflags1 & ~CM_TREASURE);
			}
			if (monptr < c_ptr->m_idx)
			    delete_monster((int)c_ptr->m_idx);
			else
			    fix1_delete_monster((int)c_ptr->m_idx);
		    } else {
			(void)sprintf(out_val, pain_message((int)c_ptr->m_idx, dam),
				      m_name);
			msg_print(out_val);
		    }
		    break;
		} else if (c_ptr->m_idx == 1) {
		    if (dam_hp < 1)
			dam_hp = 1;
		    m_ptr = &m_list[monptr];
		    switch (typ) {
		      case GF_ELEC:
			if (blind)
			    msg_print("You are hit by electricity!");
			light_dam(dam_hp, ddesc);
			break;
		      case GF_POIS:
			if (blind)
			    msg_print("You are hit by a blast of noxious gases!");
			poison_gas(dam_hp, ddesc);
			break;
		      case GF_ACID:
			if (blind)
			    msg_print("You are hit by a jet of acidic fluid!");
			acid_dam(dam_hp, ddesc);
			break;
		      case GF_COLD:
			if (blind)
			    msg_print("You are hit by something cold!");
			cold_dam(dam_hp, ddesc);
			break;
		      case GF_FIRE:
			if (blind)
			    msg_print("You are hit by something hot!");
			fire_dam(dam_hp, ddesc);
			break;
		      case GF_MISSILE:
			if (blind)
			    msg_print("You are hit by something!");
			take_hit(dam_hp, ddesc);
			break;
		      case GF_HOLY_ORB:
			if (blind)
			    msg_print("You are hit by something!");
			dam_hp /= 2;	/* player should take less damage
					 * -CFT */
			take_hit(dam_hp, ddesc);
			break;
		      case GF_ARROW:	/* maybe can miss? */
			if (blind)
			    msg_print("You are hit by something!");
			take_hit(dam_hp, ddesc);
			break;
		      case GF_PLASMA:	/* no resist to plasma? */
			if (blind)
			    msg_print("You are hit by something!");
			take_hit(dam_hp, ddesc);
			break;
		      case GF_NETHER:
			if (blind)
			    msg_print("You are hit by an unholy blast!");
			if (py.flags1.resist_nether) {
			    dam_hp *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .858 to .5 -CFT */
			} else {   /* no resist */
			    if (py.flags1.hold_life && randint(5) > 1)
				msg_print("You keep hold of your life force!");
			    else if (py.flags1.hold_life) {
				msg_print("You feel your life slipping away!");
				lose_exp(200 + (py.misc.exp / 1000) * MON_DRAIN_LIFE);
			    } else {
				msg_print("You feel your life draining away!");
				lose_exp(200 + (py.misc.exp / 100) * MON_DRAIN_LIFE);
			    }
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_WATER:
			if (blind)
			    msg_print("You are hit by a jet of water!");
			if (!py.flags1.resist_sound)
			    stun_player(randint(15));
			take_hit(dam_hp, ddesc);
			break;
		      case GF_CHAOS:
			if (blind)
			    msg_print("You are hit by wave of entropy!");
			if (py.flags1.resist_chaos) {
			    dam_hp *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .858 to .5 -CFT */
			}
			if ((!py.flags1.resist_conf) && (!py.flags1.resist_chaos)) {
			    if (py.flags1.confused > 0)
				py.flags1.confused += 12;
			    else
				py.flags1.confused = randint(20) + 10;
			}
			if (!py.flags1.resist_chaos)
			    py.flags1.image += randint(10);
			take_hit(dam_hp, ddesc);
			break;
		      case GF_SHARDS:
			if (blind)
			    msg_print("You are cut by sharp fragments!");
			if (py.flags1.resist_shards) {
			    dam_hp *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .858 to .5 -CFT */
			} else {
			    cut_player(dam_hp);	/* ouch! */
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_SOUND:
			if (blind)
			    msg_print("You are deafened by a blast of noise!");
			if (py.flags1.resist_sound) {
			    dam_hp *= 5;
			    dam_hp /= (randint(6) + 6);
			} else {
			    stun_player(randint((dam_hp > 60) ? 25 : (dam_hp / 3 + 5)));
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_CONFUSION:
			if (blind)
			    msg_print("You are hit by a wave of dizziness!");
			if (py.flags1.resist_conf) {
			    dam_hp *= 5;
			    dam_hp /= (randint(6) + 6);
			}
			if (!py.flags1.resist_conf && !py.flags1.resist_chaos) {
			    if (py.flags1.confused > 0)
				py.flags1.confused += 8;
			    else
				py.flags1.confused = randint(15) + 5;
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_DISENCHANT:
			if (blind)
			    msg_print("You are hit by something!");
			if (py.flags1.resist_disen) {
			    dam_hp *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .858 to .5 -CFT */
			} else {
			    byte               disenchant = FALSE;
			    byte               chance;
			    int                 t = 0;
			    inven_type         *i_ptr;

			    switch (randint(7)) {
			      case 1:
				t = INVEN_BODY;
				break;
			      case 2:
				t = INVEN_BODY;
				break;
			      case 3:
				t = INVEN_ARM;
				break;
			      case 4:
				t = INVEN_OUTER;
				break;
			      case 5:
				t = INVEN_HANDS;
				break;
			      case 6:
				t = INVEN_HEAD;
				break;
			      case 7:
				t = INVEN_FEET;
				break;
			    }
			    i_ptr = &inventory[t];
			    chance = 1;
			    if (i_ptr->flags2 & TR_ARTIFACT)	/* Artifacts have 2/3 */
				chance = randint(3);	/* chance to resist -DGK */
			    if ((i_ptr->tohit > 0) && (chance == 1)) {
				i_ptr->tohit -= randint(2);
			    /* don't send it below zero */
				if (i_ptr->tohit < 0)
				    i_ptr->tohit = 0;
				disenchant = TRUE;
			    }
			    if ((i_ptr->todam > 0) && (chance == 1)) {
				i_ptr->todam -= randint(2);
			    /* don't send it below zero */
				if (i_ptr->todam < 0)
				    i_ptr->todam = 0;
				disenchant = TRUE;
			    }
			    if ((i_ptr->toac > 0) && (chance == 1)) {
				i_ptr->toac -= randint(2);
			    /* don't send it below zero */
				if (i_ptr->toac < 0)
				    i_ptr->toac = 0;
				disenchant = TRUE;
			    }
			    if (disenchant || (chance != 1)) {
				vtype               t1, t2;

				objdes(t1, &inventory[t], FALSE);
				if (chance != 1)
				    sprintf(t2, "Your %s (%c) %s disenchanted!", t1,
					    i + 'a' - INVEN_WIELD,
					    (inventory[i].number != 1) ? "were" : "was");
				else
				    sprintf(t2, "Your %s (%c) %s disenchantment!", t1,
					    i + 'a' - INVEN_WIELD,
					    (inventory[i].number != 1) ?
					    "resist" : "resists");
				msg_print(t2);
				calc_bonuses();
			    }
			}
			take_hit(dam_hp, ddesc);
			break;
/* no spec. effects from nexus bolt, only breath -CFT */
		      case GF_NEXUS:
			if (blind)
			    msg_print("You are hit by something strange!");
			if (py.flags1.resist_nexus) {
			    dam_hp *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .858 to .5 -CFT */
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_FORCE:
			if (blind)
			    msg_print("You are hit hard by a sudden force!");
			if (!py.flags1.resist_sound)
			    stun_player(randint(15) + 1);
			take_hit(dam_hp, ddesc);
			break;
		      case GF_INERTIA:
			if (blind)
			    msg_print("You are hit by something!");
			if ((py.flags1.slow > 0) && (py.flags1.slow < 32000))
			    py.flags1.slow += randint(5);
			else {
			    msg_print("You feel less able to move.");
			    py.flags1.slow = randint(5) + 3;
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_LITE:
			if (blind)
			    msg_print("You are hit by something!");
			if (py.flags1.resist_lite) {
			    dam_hp *= 4;	/* these 2 lines give avg dam
						 * of .444, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .556 to .333 -CFT */
			} else if (!blind && !py.flags1.resist_blind) {
			    msg_print("You are blinded by the flash!");
			    py.flags1.blind += randint(5) + 2;
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_DARK:
			if (blind)
			    msg_print("You are hit by something!");
			if (py.flags1.resist_dark) {
			    dam_hp *= 4;	/* these 2 lines give avg dam
						 * of .444, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .556 to .333 -CFT */
			} else {
			    if (!blind && !py.flags1.resist_blind) {
				msg_print("The darkness prevents you from seeing!");
				py.flags1.blind += randint(5) + 2;
			    }
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_TIME:	/* only some effects from time bolt
					 * -CFT */
			if (blind)
			    msg_print("You are hit by something!");
			if (randint(2) == 1) {
			    msg_print("You feel life has clocked back.");
			    lose_exp(m_ptr->hp + (py.misc.exp / 300) * MON_DRAIN_LIFE);
			} else {
			    int                 t = 0;

			    switch (randint(6)) {
			      case 1:
				t = A_STR;
				msg_print("You're not as strong as you used to be...");
				break;
			      case 2:
				t = A_INT;
				msg_print("You're not as bright as you used to be...");
				break;
			      case 3:
				t = A_WIS;
				msg_print("You're not as wise as you used to be...");
				break;
			      case 4:
				t = A_DEX;
				msg_print("You're not as agile as you used to be...");
				break;
			      case 5:
				t = A_CON;
				msg_print("You're not as hale as you used to be...");
				break;
			      case 6:
				t = A_CHR;
				msg_print("You're not as beautiful as you used to be...");
				break;
			    }
			    py.stats.cur_stat[t] = (py.stats.cur_stat[t] * 3) / 4;
			    if (py.stats.cur_stat[t] < 3)
				py.stats.cur_stat[t] = 3;
			    set_use_stat(t);
			    prt_stat(t);
			}
			take_hit(dam_hp, ddesc);
			break;
		      case GF_GRAVITY:
			if (blind)
			    msg_print("You are hit by a surge of gravity!");
			if ((!py.flags1.resist_sound) && (!py.flags1.ffall))	/* DGK */
			    stun_player(randint(15) + 1);
			if (py.flags1.ffall) {	/* DGK */
			    dam_hp *= 3;	/* these 2 lines give avg dam
						 * of .25, ranging from */
			    dam_hp /= (randint(6) + 6);	/* .427 to .25 -CFT */
			} else {   /* DGK */
			    if ((py.flags1.slow > 0) && (py.flags1.slow < 32000))
				py.flags1.slow += randint(5);
			    else {
				msg_print("You feel less able to move.");
				py.flags1.slow = randint(5) + 3;
			    }
			} /* DGK */
			take_hit(dam_hp, ddesc);
			break;
		      case GF_MANA:
			if (blind)
			    msg_print("You are hit by a beam of power!");
			take_hit(dam_hp, ddesc);
			break;
		      case GF_METEOR:
			if (blind)
			    msg_print("You are hit by something!");
			take_hit(dam_hp, ddesc);
			break;
		      case GF_ICE:
			if (blind)
			    msg_print("You are hit by something cold and sharp!");
			cold_dam(dam_hp, ddesc);
			if (!py.flags1.resist_sound)
			    stun_player(randint(15) + 1);
			if (!py.flags1.resist_shards)
			    cut_player(damroll(8, 10));
			break;
		      default:
			msg_print("Unknown typ in bolt().  This may mean trouble.");
		    }
		    disturb(1, 0);
		    break;
		}
	    }
	}
    } while ((i != char_row) || (j != char_col));
}


/* Breath weapon works like a fire_ball, but affects the player. */
/* Note the area affect.                              -RAK-   */
void breath(int typ, int y, int x, int dam_hp, char *ddesc, int monptr)
{
    register int        i, j;
    int                 dam, max_dis;
    u32b              tmp, treas;
    int                 (*destroy) ();
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    int                 ny, nx;
    int                 blind = (py.flags1.status & PY_BLIND) ? 1 : 0;
    char                ch;

    m_ptr = &m_list[monptr];
    r_ptr = &r_list[m_ptr->r_idx];
    ch = r_ptr->r_char;
    if ((ch == 'v' || ch == 'D' || ch == 'E' || ch == '&' || ch == 'A') ||
	((ch == 'd' || ch == 'R') && r_ptr->cflags2 & MF2_UNIQUE))
	max_dis = 3;
    else
	max_dis = 2;

    switch (typ) {
      case GF_FIRE:
	destroy = set_fire_destroy;
	break;
      case GF_ACID:
	destroy = set_acid_destroy;
	break;
      case GF_COLD:
      case GF_SHARDS:
      case GF_ICE:
      case GF_FORCE:
      case GF_SOUND:
	destroy = set_frost_destroy;	/* just potions and flasks -DGK */
	break;
      case GF_ELEC:
	destroy = set_lightning_destroy;
	break;
      case GF_PLASMA:		   /* DGK */
	destroy = set_plasma_destroy;	/* fire+lightning -DGK */
	break;
      case GF_METEOR:		   /* DGK */
	destroy = set_meteor_destroy;	/* fire+shards -DGK */
	break;
      case GF_MANA:		   /* DGK */
	destroy = set_mana_destroy;	/* everything -DGK */
	break;
      case GF_HOLY_ORB:	   /* DGK */
	destroy = set_holy_destroy;	/* cursed stuff -DGK */
	break;
      default:
	destroy = set_null;
	break;
    }

    if (!(py.flags1.status & PY_BLIND)) { /* only bother if the player can see */
	for (i = y - max_dis; i <= y + max_dis; i++)
	    for (j = x - max_dis; j <= x + max_dis; j++)
		if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) &&
		    los(y, x, i, j) && floor_grid_bold(i, j) &&
		    panel_contains(i, j)) {
#ifdef TC_COLOR
		    if (!no_color_flag)
			textcolor(bolt_color(typ));
#endif
		    print('*', i, j);
#ifdef TC_COLOR
		    if (!no_color_flag)
			textcolor(LIGHTGRAY);	/* prob don't need here, but... -CFT */
#endif
		}
	put_qio();
#ifdef MSDOS
	delay(25 * delay_spd);	   /* milliseconds */
#else
	usleep(25000 * delay_spd); /* useconds */
#endif
 
/* now erase the ball, since effects below may use msg_print, and pause
 * indefinitely, so we want ball gone before then -CFT 
 */
	for (i = y - max_dis; i <= y + max_dis; i++)
	    for (j = x - max_dis; j <= x + max_dis; j++)
		if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) &&
		    los(y, x, i, j) && floor_grid_bold(i, j) &&
		    panel_contains(i, j))
		    lite_spot(i, j);   /* draw what is below the '*' */
	put_qio();
    }

/* first, go over area of affect and destroy preexisting items. This change
 * means that any treasure dropped by killed monsters is safe from the effects
 * of this ball (but not from any later balls/breathes, even if they happen
 * before the player gets a chance to pick up that scroll of *Acquirement*). 
 * The assumption is made that this treasure was shielded from the effects by
 * the corpse of the killed monster. -CFT 
 */
    for (i = y - max_dis; i <= y + max_dis; i++)
	for (j = x - max_dis; j <= x + max_dis; j++)
	    if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
		&& los(y, x, i, j) && (cave[i][j].i_idx != 0)
		&& (*destroy) (&i_list[cave[i][j].i_idx]))
		delete_object(i, j);

/* now go over area of affect and DO something to monsters */
    for (i = y - max_dis; i <= y + max_dis; i++)
	for (j = x - max_dis; j <= x + max_dis; j++)
	    if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
		&& los(y, x, i, j)) {

		c_ptr = &cave[i][j];
		if ((c_ptr->i_idx != 0) && (*destroy) (&i_list[c_ptr->tptr]))
		    (void)delete_object(i, j);
		if (floor_grid_bold(i, j)) {
		    if ((c_ptr->m_idx > 1) && (c_ptr->m_idx != monptr)) {
			dam = dam_hp;
			m_ptr = &m_list[c_ptr->m_idx];
			spell_hit_monster(m_ptr, typ, &dam, distance(i, j, y, x) + 1,
					  &ny, &nx, FALSE);
			c_ptr = &cave[ny][nx];	/* may be new location if teleported
                                                 * by gravity warp... */
			m_ptr = &m_list[c_ptr->m_idx];	/* and even if not, may be new
							 * monster if chaos polymorphed */
			r_ptr = &r_list[m_ptr->r_idx];

		    /*
		     * can not call mon_take_hit here, since player does not
		     * get experience for kill 
		     */
			if (dam < 1)
			    dam = 1;
			m_ptr->hp = m_ptr->hp - dam;
			m_ptr->csleep = 0;

/* prevent unique monster from death by other monsters.  It causes trouble (monster not
 * marked as dead, quest monsters don't satisfy quest, etc).  So, we let
 * them live, but extremely wimpy.  This isn't great, because monster might heal
 * itself before player's next swing... -CFT
 */
			if ((r_ptr->cflags2 & MF2_UNIQUE) && (m_ptr->hp < 0))
			    m_ptr->hp = 0;

			if (m_ptr->hp < 0) {
			    object_level = (dun_level + r_ptr->level) >> 1;
				coin_type = 0;
				get_coin_type(r_ptr);
			    treas = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
						  r_ptr->cflags1, 0, 0);
				coin_type = 0;
				/* recall even invisible uniques -CWS */
			    if (m_ptr->ml || (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)) {
				tmp = (l_list[m_ptr->r_idx].r_cflags1 & CM_TREASURE)
				    >> CM_TR_SHIFT;
				if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT))
				    treas = (treas & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
				l_list[m_ptr->r_idx].r_cflags1 = treas |
				    (l_list[m_ptr->r_idx].r_cflags1 & ~CM_TREASURE);
			    }
			/* It ate an already processed monster.  Handle normally. */
			    if (monptr < c_ptr->m_idx)
				delete_monster((int)c_ptr->m_idx);

/* If it eats this monster, an already processed monster will take its place,
 * causing all kinds of havoc. Delay the kill a bit.
 */
			    else
				fix1_delete_monster((int)c_ptr->m_idx);
			}
		    } else if (c_ptr->m_idx == 1) {
			dam = (dam_hp / (distance(i, j, y, x) + 1));
			m_ptr = &m_list[monptr];
		    /* let's do at least one point of damage */
		    /* prevents randint(0) problem with poison_gas, also */
			if (dam <= 0)
			    dam = 1;
			if (dam > 1600)
			    dam = 1600;
			switch (typ) {
			  case GF_ELEC:
			    light_dam(dam, ddesc);
			    break;
			  case GF_POIS:
			    poison_gas(dam, ddesc);
			    break;
			  case GF_ACID:
			    acid_dam(dam, ddesc);
			    break;
			  case GF_COLD:
			    cold_dam(dam, ddesc);
			    break;
			  case GF_FIRE:
			    fire_dam(dam, ddesc);
			    break;
			  case GF_MISSILE:
			    take_hit(dam, ddesc);
			    break;
			  case GF_HOLY_ORB:
			    dam /= 2;	/* player should take less damage
					 * from "good" power-CFT */
			    take_hit(dam, ddesc);
			    break;
			  case GF_ARROW:	/* maybe can miss? */
			    take_hit(dam, ddesc);
			    break;
			  case GF_PLASMA:	/* no resist to plasma? */
			    take_hit(dam, ddesc);
			    if (!py.flags1.resist_sound)
				stun_player(randint(
				(dam_hp > 40) ? 35 : (dam_hp * 3 / 4 + 5)));
			    break;
			  case GF_NETHER:
			    if (py.flags1.resist_nether) {
				dam *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(5) + 6);	/* .858 to .5 -CFT */
			    } else {	/* no resist */
				if (py.flags1.hold_life && randint(3) > 1)
				    msg_print("You keep hold of your life force!");
				else if (py.flags1.hold_life) {
				    msg_print("You feel your life slipping away!");
				    lose_exp(200 + (py.misc.exp/1000) * MON_DRAIN_LIFE);
				} else {
				    msg_print("You feel your life draining away!");
				    lose_exp(200 + (py.misc.exp/100) * MON_DRAIN_LIFE);
				}
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_WATER:
			    if (!py.flags1.resist_sound)
				stun_player(randint(55));
			    if (!player_saves() && !py.flags1.resist_conf
				&& !py.flags1.resist_chaos) {
				if ((py.flags1.confused > 0) &&
				    (py.flags1.confused < 32000))
				    py.flags1.confused += 6;
				else
				    py.flags1.confused = randint(8) + 6;
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_CHAOS:
			    if (py.flags1.resist_chaos) {
				dam *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(6) + 6);	/* .858 to .5 -CFT */
			    }
			    if ((!py.flags1.resist_conf) &&
				(!py.flags1.resist_chaos)) {
				if (py.flags1.confused > 0)
				    py.flags1.confused += 12;
				else
				    py.flags1.confused = randint(20) + 10;
			    }
			    if (!py.flags1.resist_chaos)
				py.flags1.image += randint(10);
			    if (!py.flags1.resist_nether && !py.flags1.resist_chaos) {
				if (py.flags1.hold_life && randint(3) > 1)
				    msg_print("You keep hold of your life force!");
				else if (py.flags1.hold_life) {
				    msg_print("You feel your life slipping away!");
				    lose_exp(500 + (py.misc.exp/1000) * MON_DRAIN_LIFE);
				} else {
				    msg_print("You feel your life draining away!");
				    lose_exp(5000 + (py.misc.exp/100) * MON_DRAIN_LIFE);
				}
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_SHARDS:
			    if (py.flags1.resist_shards) {
				dam *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(6) + 6);	/* .858 to .5 -CFT */
			    } else {
				cut_player(dam);	/* ouch! */
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_SOUND:
			    if (py.flags1.resist_sound) {
				dam *= 5;
				dam /= (randint(6) + 6);
			    } else {
				stun_player(randint((dam > 90) ? 35 : (dam / 3 + 5)));
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_CONFUSION:
			    if (py.flags1.resist_conf) {
				dam *= 5;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(6) + 6);	/* .858 to .5 -CFT */
			    }
			    if (!py.flags1.resist_conf && !py.flags1.resist_chaos) {
				if (py.flags1.confused > 0)
				    py.flags1.confused += 12;
				else
				    py.flags1.confused = randint(20) + 10;
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_DISENCHANT:
			    if (py.flags1.resist_disen) {
				dam *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(6) + 6);	/* .858 to .5 -CFT */
			    } else {
				byte               disenchant = FALSE;
				byte               chance;
				int                 t = 0;
				inven_type         *i_ptr;

				switch (randint(7)) {
				  case 1:
				    t = INVEN_BODY;
				    break;
				  case 2:
				    t = INVEN_BODY;
				    break;
				  case 3:
				    t = INVEN_ARM;
				    break;
				  case 4:
				    t = INVEN_OUTER;
				    break;
				  case 5:
				    t = INVEN_HANDS;
				    break;
				  case 6:
				    t = INVEN_HEAD;
				    break;
				  case 7:
				    t = INVEN_FEET;
				    break;
				}
				i_ptr = &inventory[t];
				chance = 1;
				/* Artifacts have 2/3 chance to resist -DGK */
				if (i_ptr->flags2 & TR_ARTIFACT)
				    chance = randint(3);
				if ((i_ptr->tohit > 0) && (chance == 1)) {
				    i_ptr->tohit -= randint(2);
				/* don't send it below zero */
				    if (i_ptr->tohit < 0)
					i_ptr->tohit = 0;
				    disenchant = TRUE;
				}
				if ((i_ptr->todam > 0) && (chance == 1)) {
				    i_ptr->todam -= randint(2);
				/* don't send it below zero */
				    if (i_ptr->todam < 0)
					i_ptr->todam = 0;
				    disenchant = TRUE;
				}
				if ((i_ptr->toac > 0) && (chance == 1)) {
				    i_ptr->toac -= randint(2);
				/* don't send it below zero */
				    if (i_ptr->toac < 0)
					i_ptr->toac = 0;
				    disenchant = TRUE;
				}
				if (disenchant || (chance != 1)) {
				    vtype               t1, t2;

				    objdes(t1, &inventory[t], FALSE);
				    if (chance != 1)
					sprintf(t2, "Your %s (%c) %s disenchanted!", t1,
						i + 'a' - INVEN_WIELD,
						(inventory[i].number != 1) ?
						"were" : "was");
				    else
					sprintf(t2, "Your %s (%c) %s disenchantment!",
						t1,
						i + 'a' - INVEN_WIELD,
						(inventory[i].number != 1) ?
						"resist" : "resists");
				    msg_print(t2);
				    calc_bonuses();
				}
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_NEXUS:
			    /* no spec. effects from nexus bolt, only breath -CFT */
			    if (py.flags1.resist_nexus) {
				dam *= 6;	/* these 2 lines give avg dam
						 * of .655, ranging from */
				dam /= (randint(6) + 6);	/* .858 to .5 -CFT */
			    } else {	/* special effects */
				switch (randint(7)) {
				  case 1:
				  case 2:
				  case 3:
				    teleport(200);
				    break;
				  case 4:
				  case 5:
				    teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
				    break;
				  case 6:
				    if (player_saves())
					msg_print("You resist the effects.");
				    else {
					int                 k = dun_level;

					if (dun_level == Q_PLANE)
					    dun_level = 0;
					else if (is_quest(dun_level))
					    dun_level -= 1;
					else
					    dun_level += (-3) + 2 * randint(2);
					if (dun_level < 0)
					    dun_level = 0;
					if (k == Q_PLANE)
			    msg_print("You warp through a cross-dimension gate.");
					else if (k < dun_level)
					    msg_print("You sink through the floor.");
					else
				    msg_print("You rise up through the ceiling.");
					new_level_flag = TRUE;
				    }
				    break;
				  case 7:
				    if (player_saves() && randint(2) == 1)
					msg_print("You resist the effects.");
				    else {
					int      max1, cur1, max2, cur2, ii, jj;

					msg_print("Your body starts to scramble...");
					ii = randint(6) - 1;
					do {
					    jj = randint(6) - 1;
					} while (ii == jj);
					max1 = py.stats.max_stat[ii];
					cur1 = py.stats.cur_stat[ii];
					max2 = py.stats.max_stat[jj];
					cur2 = py.stats.cur_stat[jj];
					py.stats.max_stat[ii] = max2;
					py.stats.cur_stat[ii] = cur2;
					py.stats.max_stat[jj] = max1;
					py.stats.cur_stat[jj] = cur1;
					set_use_stat(ii);
					set_use_stat(jj);
					prt_stat(ii);
					prt_stat(jj);
				    }
				} /* switch for effects */
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_FORCE:
			    if (!py.flags1.resist_sound)
				stun_player(randint(20));
			    take_hit(dam, ddesc);
			    break;
			  case GF_INERTIA:
			    if ((py.flags1.slow > 0) && (py.flags1.slow < 32000))
				py.flags1.slow += randint(5);
			    else {
				msg_print("You feel less able to move.");
				py.flags1.slow = randint(5) + 3;
			    }
			    take_hit(dam, ddesc);
			    break;
			  case GF_LITE:
			    if (py.flags1.resist_lite) {
				dam *= 4;
				dam /= (randint(6) + 6);
			    } else if (!blind && !py.flags1.resist_blind) {
				msg_print("You are blinded by the flash!");
				py.flags1.blind += randint(6) + 3;
			    }
			    lite_area(char_row, char_col, 0, max_dis);
			    take_hit(dam, ddesc);
			    break;
			  case GF_DARK:
			    if (py.flags1.resist_dark) {
				dam *= 4;
				dam /= (randint(6) + 6);
			    } else {
				if (!blind) 
				    msg_print("The darkness prevents you from seeing!");
				py.flags1.blind += randint(5) + 2;
			    }
			    unlite_area(char_row, char_col);
			    take_hit(dam, ddesc);
			    break;
			  case GF_TIME:	/* only some effects from
						 * time bolt -CFT */
			    switch (randint(10)) {
			      case 1:
			      case 2:
			      case 3:
			      case 4:
			      case 5:
				msg_print("You feel life has clocked back.");
				lose_exp(m_ptr->hp + (py.misc.exp / 300) * MON_DRAIN_LIFE);
				break;
			      case 6:
			      case 7:
			      case 8:
			      case 9:
				{
				    int                 t = 0;

				    switch (randint(6)) {
				      case 1:
					t = A_STR;
				msg_print("You're not as strong as you used to be...");
					break;
				      case 2:
					t = A_INT;
				msg_print("You're not as bright as you used to be...");
					break;
				      case 3:
					t = A_WIS;
				msg_print("You're not as wise as you used to be...");
					break;
				      case 4:
					t = A_DEX;
				msg_print("You're not as agile as you used to be...");
					break;
				      case 5:
					t = A_CON;
				msg_print("You're not as hale as you used to be...");
					break;
				      case 6:
					t = A_CHR;
			msg_print("You're not as beautiful as you used to be...");
					break;
				    }
			    py.stats.cur_stat[t] = (py.stats.cur_stat[t] * 3) / 4;
				    if (py.stats.cur_stat[t] < 3)
					py.stats.cur_stat[t] = 3;
				    set_use_stat(t);
				    prt_stat(t);
				}
				break;
			      case 10:
				{
				    int                 ii;

				    for (ii = 0; ii < 6; ii++) {
				py.stats.cur_stat[ii] = (py.stats.cur_stat[ii] * 3) / 4;
					if (py.stats.cur_stat[ii] < 3)
					    py.stats.cur_stat[ii] = 3;
					set_use_stat(ii);
					prt_stat(ii);
				    }
				}
				msg_print("You're not as strong as you used to be...");
				msg_print("You're not as bright as you used to be...");
				msg_print("You're not as wise as you used to be...");
				msg_print("You're not as agile as you used to be...");
				msg_print("You're not as hale as you used to be...");
			msg_print("You're not as beautiful as you used to be...");
				break;
			    } /* randint(10) for effects */
			    take_hit(dam, ddesc);
			    break;
			  case GF_GRAVITY:
			    if ((!py.flags1.resist_sound) && (!py.flags1.ffall))	/* DGK */
				stun_player(randint((dam > 90) ? 35 : (dam / 3 + 5)));
			    if (py.flags1.ffall) {	/* DGK */
				dam_hp *= 3;	/* these 2 lines give avg dam
						 * of .33, ranging from */
				dam_hp /= (randint(6) + 6);	/* .427 to .25 -CFT */
			    } else {	/* DGK */
				if ((py.flags1.slow > 0) && (py.flags1.slow < 32000))
				    py.flags1.slow += randint(5);
				else {
				    msg_print("You feel less able to move.");
				    py.flags1.slow = randint(5) + 3;
				}
			    } /* DGK */
			    msg_print("Gravity warps around you.");
			    teleport(5);
			    take_hit(dam, ddesc);
			    break;
			  case GF_MANA:
			    take_hit(dam, ddesc);
			    break;
			  case GF_METEOR:
			    take_hit(dam, ddesc);
			    break;
			  case GF_ICE:
			    cold_dam(dam, ddesc);
			    if (!py.flags1.resist_sound)
				stun_player(randint(25));
			    if (!py.flags1.resist_shards)
				cut_player(damroll(8, 10));
			    break;
			  default:
		    msg_print("Unknown typ in breath().  This may mean trouble.");
			}
		    }
		}
	    }
/* show the ball of gas */
    put_qio();

/* erase ball and redraw */
    for (i = (y - max_dis); i <= (y + max_dis); i++)
	for (j = (x - max_dis); j <= (x + max_dis); j++)
	    if (in_bounds(i, j) && panel_contains(i, j) &&
		(distance(y, x, i, j) <= max_dis))
		lite_spot(i, j);
}

