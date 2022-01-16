/* File: creature.c */

/* Purpose: handle monster movement and attacks */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static int  check_mon_lite(int, int);
static void get_moves(int, int *);
static int  monster_critical(int, int, int);
static void make_attack(int);
static void make_move(int, int *, u32b *);
static void mon_cast_spell(int, int *);
static void mon_move(int, u32b *);
static void shatter_quake(int, int);
static void br_wall(int, int);

/* Updates screen when monsters move about		-RAK-	 */
void update_mon(int m_idx)
{
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race  *r_ptr;

#ifdef ATARIST_MWC
    u32b                 holder;
#endif

    /* Can the monster be sensed in any way? */
    int flag = FALSE;

    /* Get the monster */
    m_ptr = &m_list[m_idx];

    if ((m_ptr->cdis <= MAX_SIGHT) &&
	(!(py.flags.status & PY_BLIND) || py.flags.telepathy) &&
	(panel_contains((int)m_ptr->fy, (int)m_ptr->fx))) {

	/* Wizards see everything */
	if (wizard) flag = TRUE;

    /* if not mindless, telepathy sees -CWS */
	if (py.flags.telepathy) {
	    char c = c_list[m_ptr->r_idx].cchar;
	    const char *n = c_list[m_ptr->r_idx].name;
	    if (strchr("EMXgjvz.",c));
    /* don't show these ever (elementals & golems & vorticies & xorns
     * "minds" are too different) -CFT */

	    else if (strchr("FKaclt",c)) {
		if (randint(5)==1)
		    flag = TRUE;
    /* once in a while we see these almost mindless insects... -CFT */

	    } else if (c=='S' && strncmp(n, "Drider", 6) &&
		     !(c_list[m_ptr->r_idx].cdefense & UNIQUE)) {
		if (randint(5)==1) flag = TRUE;
    /* once in a while show spiders, scops.  But DO show drider,
     * Shelob, and Ungol. -CFT */

	    } else if (c=='m' && strncmp(n, "Death", 5));
    /* don't show any molds, except the Death mold -CFT */

	    else if (c=='s' && !strstr(n, "ruj") &&
		     strncmp(n, "Cantor", 6));
    /* don't show skeletons, but DO show druj and Cantoras -CFT */

	    else if (c=='i' && strncmp(n, "Blue", 4));
    /* don't show icky things, except Blue icky things.. -CFT */

	    else if (c=='w' && strncmp(n, "Were", 4) && strncmp(n, "Gian", 4)) {
		if (randint(5)==1) flag = TRUE;
    /* occas. show worms, except Purple worms and Wereworms -CFT */

	    } else if (c==',' && strncmp(n, "Magic", 5));
    /* don't show mushrooms, except magic 'shrooms -CFT */

	    else if (!(c_list[m_ptr->r_idx].cdefense & MINDLESS))
		flag = TRUE;
    /* if not mindless, they show up -CWS */
	}

	/* Normal sight.	     */
	if (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {
	    c_ptr = &cave[m_ptr->fy][m_ptr->fx];

	    r_ptr = &c_list[m_ptr->r_idx];
	/* moved here to allow infra to see invis -CFT */
	    if ((py.flags.see_infra > 0) &&
		(m_ptr->cdis <= py.flags.see_infra)) {
		if (NO_INFRA & r_ptr->cdefense)	/* changed to act sensibly -CFT */
		    c_recall[m_ptr->r_idx].r_cdefense |= NO_INFRA;
		else
		    flag = TRUE;   /* only can see if not NO_INFRA... */
	    }
	    if (c_ptr->pl || c_ptr->tl ||
		(find_flag && m_ptr->cdis <= cur_lite && player_light)) {

#ifdef ATARIST_MWC
		holder = CM_INVISIBLE;
		if ((holder & r_ptr->cmove) == 0)
#else
		if ((CM_INVISIBLE & r_ptr->cmove) == 0)
#endif
		    flag = TRUE;
		else if (py.flags.see_inv) {
		    flag = TRUE;
#ifdef ATARIST_MWC
		    c_recall[m_ptr->r_idx].r_cmove |= holder;
#else
		    c_recall[m_ptr->r_idx].r_cmove |= CM_INVISIBLE;
#endif
		}
	    }
	}
    }
/* Light it up.	 */
    if (flag) {
#ifdef TC_COLOR
	if (!no_color_flag)	   /* don't waste time if no color -CFT */
	    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
/* redraw, even if lit, so MHD's change color -CFT */
#endif
	if (!m_ptr->ml) {
	    disturb(1, 0);
	    m_ptr->ml = TRUE;
	    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
	    screen_change = TRUE;  /* notify inven_command */
	}
    }
/* Turn it off.	 */
    else if (m_ptr->ml) {
	m_ptr->ml = FALSE;
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
	screen_change = TRUE;	   /* notify inven_command */
    }
}


/* Given speed,  returns number of moves this turn.     -RAK-   */
/* NOTE: Player must always move at least once per iteration,
 *       a slowed player is handled by moving monsters faster
 *
 * monnum = index in m_list[] now passed in, so (turn+monnum) can
 *          be used to vary when monsters move. -CFT
 */

int movement_rate(int monnum)
{
  register int ps, ms, tm, i;

  ps = 1 - py.flags.speed;	/* this makes normal = 1, fast = 2,
                                 * v.fast = 3, slow = 0, v.slow = -1 -CFT */
  ms = m_list[monnum].cspeed;
  
  i = (int)(turn & 0xFF) + (int)monnum; /* 0xFF to prevent negative values -CFT */

  if (ps == ms)
      return 1;			/* same spd as player -CFT */

  if (ps<1 && ms<1) {		/* both slow, swap "reciprocals" -CFT */
      tm = 2 - ps;
      ps = 2 - ms;
      ms = tm;
  }

  if (ps < 1)			/* then mon must be fast, or above would */
      return ms * (2 - ps);	/* have happened -CFT */

  if (ms<1)			/* then player fast... move once in a while -CFT */
    return !(i % (ps * (2 - ms)));

/* player faster.         
 * This formula is not intuitive, but it effectively uses the turn counter
 * (offset by the monster index, so not every monster moves at same time)
 * to compute factional parts of movement ratios.. so that a monster 2/3 the
 * player's spd will move twice every 3 turns.  An earlier version of this
 * equation performed the same result on average, but it was prone to "clumps"
 * of speed... if the player was spd 4, and the monster spd 2, then for each
 * 4 turn cycle, the monster would move 0,0,1,1.  This equation will result
 * in 0,1,0,1, which is better. -CFT
 */

  if (ps>ms)
    return (((i*ms) % ps) < ms);

  if (!(tm = (ms % ps)))	/* divides evenly, simple case -CFT */
    return (ms / ps);

  /* Like the player-faster formula, this is NOT intuitive.  However, it
   * effectively uses the turn counter & monster index to decide when a
   *  monster should get an "extra" move.  It also prevents "clumps". -CFT
   */
  return ((ms / ps) + (((i*tm) % ps) < tm));
}

/* Makes sure a new creature gets lit up.			-CJS- */
static int check_mon_lite(int y, int x)
{
    register int m_idx;

    m_idx = cave[y][x].cptr;
    if (m_idx <= 1)
	return FALSE;
    else {
	update_mon(m_idx);
	return m_list[m_idx].ml;
    }
}


/*
 * Choose correct directions for monster movement	-RAK-	 
 */
static void get_moves(int m_idx, int *mm)
{
    int y, ay, x, ax, move_val = 0;

    /* Extract the "pseudo-direction" */
    y = m_list[m_idx].fy - char_row;
    x = m_list[m_idx].fx - char_col;

/* lvl  1..15 always afraid, 16..22 some, based on maxhp %8 :
 * lvl  16    7 of 8 monsters afraid  [  this is for a 50th  ]
 * lvl  22    1 of 8 monsters afraid  [  level player, lower ]
 * lvl  23++  no afraid monsters      [ level=less afraid m. ]
 */

    if (((s16b)(py.misc.lev - 34 - c_list[(m_list[m_idx]).r_idx].level +
		 ((m_list[m_idx].maxhp) % 8)) > 0)
	|| m_list[m_idx].monfear) { /* Run away!  Run away! -DGK */
    
	/* XXX Not very "bright" */
	y = (-y);
	x = (-x);
    }

    if (y < 0) {
	move_val = 8;
	ay = (-y);
    } else {
	move_val = 0;
	ay = y;
    }
    if (x > 0) {
	move_val += 4;
	ax = x;
    } else
	ax = (-x);

    /* Prevent the diamond maneuvre */
    if (ay > (ax << 1)) {
	move_val++;
	move_val++;
    }
    else if (ax > (ay << 1)) {
	move_val++;
    }

    /* Extract some directions */
    switch (move_val) {
      case 0:
	mm[0] = 9;
	if (ay > ax) {
	    mm[1] = 8;
	    mm[2] = 6;
	    mm[3] = 7;
	    mm[4] = 3;
	}
	else {
	    mm[1] = 6;
	    mm[2] = 8;
	    mm[3] = 3;
	    mm[4] = 7;
	}
	break;
      case 1:
      case 9:
	mm[0] = 6;
	if (y < 0) {
	    mm[1] = 3;
	    mm[2] = 9;
	    mm[3] = 2;
	    mm[4] = 8;
	}
	else {
	    mm[1] = 9;
	    mm[2] = 3;
	    mm[3] = 8;
	    mm[4] = 2;
	}
	break;
      case 2:
      case 6:
	mm[0] = 8;
	if (x < 0) {
	    mm[1] = 9;
	    mm[2] = 7;
	    mm[3] = 6;
	    mm[4] = 4;
	}
	else {
	    mm[1] = 7;
	    mm[2] = 9;
	    mm[3] = 4;
	    mm[4] = 6;
	}
	break;
      case 4:
	mm[0] = 7;
	if (ay > ax) {
	    mm[1] = 8;
	    mm[2] = 4;
	    mm[3] = 9;
	    mm[4] = 1;
	}
	else {
	    mm[1] = 4;
	    mm[2] = 8;
	    mm[3] = 1;
	    mm[4] = 9;
	}
	break;
      case 5:
      case 13:
	mm[0] = 4;
	if (y < 0) {
	    mm[1] = 1;
	    mm[2] = 7;
	    mm[3] = 2;
	    mm[4] = 8;
	}
	else {
	    mm[1] = 7;
	    mm[2] = 1;
	    mm[3] = 8;
	    mm[4] = 2;
	}
	break;
      case 8:
	mm[0] = 3;
	if (ay > ax) {
	    mm[1] = 2;
	    mm[2] = 6;
	    mm[3] = 1;
	    mm[4] = 9;
	}
	else {
	    mm[1] = 6;
	    mm[2] = 2;
	    mm[3] = 9;
	    mm[4] = 1;
	}
	break;
      case 10:
      case 14:
	mm[0] = 2;
	if (x < 0) {
	    mm[1] = 3;
	    mm[2] = 1;
	    mm[3] = 6;
	    mm[4] = 4;
	}
	else {
	    mm[1] = 1;
	    mm[2] = 3;
	    mm[3] = 4;
	    mm[4] = 6;
	}
	break;
      case 12:
	mm[0] = 1;
	if (ay > ax) {
	    mm[1] = 2;
	    mm[2] = 4;
	    mm[3] = 3;
	    mm[4] = 7;
	}
	else {
	    mm[1] = 4;
	    mm[2] = 2;
	    mm[3] = 7;
	    mm[4] = 3;
	}
	break;
    }
}

static int monster_critical(int dice, int sides, int dam)
{
    int total = dice * sides;

	int max = 0;
	
    if (dam == total && dam > 20) max = 1;

    if ((dam > (19 * total) / 20) && ((dam < 20) ? randint(20) == 1 : TRUE)) {

	if (dam > 20) {
	    while (randint(50) == 1) max++;
	}

	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
    }

    return 0;
}


/*
 * Make an attack on the player (chuckle.)		-RAK-	 
 */
static void make_attack(int m_idx)
{
    int                    attype, adesc, adice, asides;
    int                    i, j, damage, flag, attackn, notice, visible;
    int                    shatter = FALSE;
    int                    CUT = FALSE, STUN = FALSE;
    s32b                  gold;
    attid                  *attstr, *attstr_orig;
    vtype                  cdesc, tmp_str, ddesc;

    register monster_race	*r_ptr;
    monster_type		*m_ptr;
    register struct misc   *p_ptr;
    register struct flags  *f_ptr;
    register inven_type		*i_ptr;

    /* flag to see if blinked away (after steal) -CFT */
    byte                  blinked = 0;


    /* don't beat a dead body! */
    if (death) return;

    m_ptr = &m_list[m_idx];
    r_ptr = &c_list[m_ptr->r_idx];

    if (r_ptr->cdefense & DESTRUCT)
	shatter = TRUE;

    if (!m_ptr->ml)
	(void)strcpy(cdesc, "It ");
    else {
	if (c_list[m_ptr->r_idx].cdefense & UNIQUE)
	    (void)sprintf(cdesc, "%s ", r_ptr->name);
	else
	    (void)sprintf(cdesc, "The %s ", r_ptr->name);
    }

/* For "DIED_FROM" string	   */
    if (r_ptr->cdefense & UNIQUE)
	(void)sprintf(ddesc, "%s", r_ptr->name);
    else if (is_a_vowel(r_ptr->name[0]))
	(void)sprintf(ddesc, "an %s", r_ptr->name);
    else
	(void)sprintf(ddesc, "a %s", r_ptr->name);
/* End DIED_FROM		   */

    attackn = 0;
    attstr = r_ptr->damage;
    attstr_orig = attstr;

    /* if has no attacks (*attstr starts off 0), still loop once */
    /* to accumulate notices that it has no attacks - dbd */
    while ((*attstr != 0 || attstr == attstr_orig) && !death && !blinked) {
	attype = a_list[*attstr].attack_type;
	adesc = a_list[*attstr].attack_desc;
	adice = a_list[*attstr].attack_dice;
	asides = a_list[*attstr].attack_sides;
	attstr++;
	flag = FALSE;

	/* Random (100) + level > 50 chance for stop any attack added */
	if (((py.flags.protevil > 0) && (r_ptr->cdefense & EVIL) &&
	     ((py.misc.lev + 1) > r_ptr->level)) &&
	    (randint(100) + (py.misc.lev) > 50)) {

	    if (m_ptr->ml) c_recall[m_ptr->r_idx].r_cdefense |= EVIL;
	    attype = 99;
	    adesc = 99;
	}

	p_ptr = &py.misc;
	switch (attype) {
	  case 1:		   /* Normal attack  */
	    if (test_hit(60, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 2:		   /* Lose Strength */
	    if (test_hit(-3, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 3:		   /* Confusion attack */
	    if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 4:		   /* Fear attack    */
	    if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 5:		   /* Fire attack    */
	    if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 6:		   /* Acid attack    */
	    if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 7:		   /* Cold attack    */
	    if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 8:		   /* Lightning attack */
	    if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 9:		   /* Corrosion attack */
	    if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 10:		   /* Blindness attack */
	    if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 11:		   /* Paralysis attack */
	    if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 12:		   /* Steal Money    */
	    if ((test_hit(5, (int)r_ptr->level, 0, p_ptr->lev,
			  CLA_MISC_HIT))
		&& (p_ptr->au > 0))
		flag = TRUE;
	    break;
	  case 13:		   /* Steal Object   */
	    if ((test_hit(2, (int)r_ptr->level, 0, p_ptr->lev,
			  CLA_MISC_HIT))
		&& (inven_ctr > 0))
		flag = TRUE;
	    break;
	  case 14:		   /* Poison	       */
	    if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 15:		   /* Lose dexterity */
	    if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 16:		   /* Lose constitution */
	    if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 17:		   /* Lose intelligence */
	    if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 18:		   /* Lose wisdom */
	    if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 19:		   /* Lose experience */
	    if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 20:		   /* Aggravate monsters */
	    flag = TRUE;
	    break;
	  case 21:		   /* Disenchant	  */
	    if (test_hit(20, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 22:		   /* Eat food	  */
	    if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 23:		   /* Eat light	  */
	    if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			 CLA_MISC_HIT))
		flag = TRUE;
	    break;
	  case 24:		   /* Eat charges	  */
	    if ((test_hit(15, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			  CLA_MISC_HIT)))
		/* check to make sure an object (XXX drainable?) exists */
		if (inven_ctr > 0) flag = TRUE;
	    break;
	  case 25:		   /* Drain all stats   */
	    if ((test_hit(2, (int)r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac,
			  CLA_MISC_HIT)))
		flag = TRUE;
	    break;

	  /* Repelled */
	  case 99:
	    flag = TRUE;
	    break;

	  /* Monster has no physical attacks - dbd */
	  case 0:
	    flag = TRUE;
	    break;

	  default:
	    break;
	}

	/* Describe the attack */
	if (flag) {

	    disturb(1, 0);
	    (void)strcpy(tmp_str, cdesc);

	    /* No cut or stun yet */
	    CUT = STUN = FALSE;

	    switch (adesc) {
	      case 1:
		msg_print(strcat(tmp_str, "hits you."));
		CUT = TRUE;
		STUN = TRUE;
		break;
	      case 2:
		msg_print(strcat(tmp_str, "bites you."));
		CUT = TRUE;
		break;
	      case 3:
		msg_print(strcat(tmp_str, "claws you."));
		CUT = TRUE;
		break;
	      case 4:
		msg_print(strcat(tmp_str, "stings you."));
		break;
	      case 5:
		msg_print(strcat(tmp_str, "touches you."));
		break;
	      case 6:
		msg_print(strcat(tmp_str, "kicks you."));
		break;
	      case 7:
		msg_print(strcat(tmp_str, "gazes at you."));
		break;
	      case 8:
		msg_print(strcat(tmp_str, "breathes on you."));
		break;
	      case 9:
		msg_print(strcat(tmp_str, "spits on you."));
		break;
	      case 10:
		msg_print(strcat(tmp_str, "makes a horrible wail."));
		break;
	      case 11:
		msg_print(strcat(tmp_str, "embraces you."));
		break;
	      case 12:
		msg_print(strcat(tmp_str, "crawls on you."));
		break;
	      case 13:
		msg_print(strcat(tmp_str, "releases a cloud of spores."));
		break;
	      case 14:
		msg_print(strcat(tmp_str, "begs you for money."));
		break;
	      case 15:
		msg_print("You've been slimed!");
		break;
	      case 16:
		msg_print(strcat(tmp_str, "crushes you."));
		break;
	      case 17:
		msg_print(strcat(tmp_str, "tramples you."));
		STUN = TRUE;
		break;
	      case 18:
		msg_print(strcat(tmp_str, "drools on you."));
		break;
	      case 19:
		switch (randint(9)) {
		  case 1:
		    msg_print(strcat(tmp_str, "insults you!"));
		    break;
		  case 2:
		    msg_print(strcat(tmp_str, "insults your mother!"));
		    break;
		  case 3:
		    msg_print(strcat(tmp_str, "gives you the finger!"));
		    break;
		  case 4:
		    msg_print(strcat(tmp_str, "humiliates you!"));
		    break;
		  case 5:
		    msg_print(strcat(tmp_str, "wets on your leg!"));
		    break;
		  case 6:
		    msg_print(strcat(tmp_str, "defiles you!"));
		    break;
		  case 7:
		    msg_print(strcat(tmp_str, "dances around you!"));
		    break;
		  case 8:
		    msg_print(strcat(tmp_str, "makes obscene gestures!"));
		    break;
		  case 9:
		    msg_print(strcat(tmp_str, "moons you!!!"));
		    break;
		}
		break;
	      case 20:
		msg_print(strcat(tmp_str, "butts you."));
		STUN = TRUE;
		break;
	      case 21:
		msg_print(strcat(tmp_str, "charges you."));
		STUN = TRUE;
		break;
	      case 22:
		msg_print(strcat(tmp_str, "engulfs you."));
		break;
	      case 23:
		switch (randint(5)) {
		  case 1:
		    msg_print(strcat(tmp_str, "wants his mushrooms back. "));
		    break;
		  case 2:
		    msg_print(strcat(tmp_str, "tells you to get off his land. "));
		    break;
		  case 3:
		    msg_print(strcat(tmp_str, "looks for his dogs. "));
		    break;
		  case 4:
		    msg_print(strcat(tmp_str, "says 'Did you kill my Fang?' "));
		    break;
		  case 5:
		    msg_print(strcat(tmp_str,
			      "asks 'Do you want to buy any mushrooms?' "));
		    break;
		}
		break;
	      case 99:
		msg_print(strcat(tmp_str, "is repelled."));
		break;
	      case 0:
		/* no message for case 0 because no attacks - dbd */
	      default:
		break;
	    }

	    notice = TRUE;
	/*
	 * always fail to notice attack if creature invisible, set notice and
	 * visible here since creature may be visible when attacking and then
	 * teleport afterwards (becoming effectively invisible) 
	 */
	    if (!m_ptr->ml) {
		visible = FALSE;
		notice = FALSE;
	    }
	    else {
		visible = TRUE;
	    }

	    /* Roll out the damage */
	    damage = damroll(adice, asides);

	    switch (attype) {

	      /* No physical attacks */
	      case 0:
		/* notice eventually */
		if (!rand_int(10)) notice = TRUE;
		break;

	      /* Normal attack	 */
	      case 1:
		/* round half-way case down */
		damage -= ((((((p_ptr->pac + p_ptr->ptoac) > 150) ? 150 :
		     (p_ptr->pac + p_ptr->ptoac)) * 3) / 4) * damage) / 200;
		take_hit(damage, ddesc);
		if ((damage > 23) && shatter) {
		    /* Earthquake centered at the monster */
		    shatter_quake(m_ptr->fy, m_ptr->fx);
		}
		break;

	      /* Lose Strength */
	      case 2:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (f_ptr->sustain_str) {
		    msg_print("You feel weaker for a moment, but it passes.");
		}
		else if (rand_int(2)) {
		    msg_print("You feel weaker.");
		    (void)dec_stat(A_STR);
		}
		else {
		    notice = FALSE;
		}
		break;

	      /* Confusion attack */
	      case 3:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if ((!f_ptr->resist_conf) && (!f_ptr->resist_chaos)) {
		    if (randint(2) == 1) {
			if (f_ptr->confused < 1) {
			    msg_print("You feel confused.");
			    f_ptr->confused += randint((int)r_ptr->level);
			}
			else {
			    notice = FALSE;
			}
			f_ptr->confused += 3;
		    }
		    else {
			notice = FALSE;
		    }
		}
		break;

	      /* Fear attack */		
	      case 4:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (player_saves() ||
		    (py.misc.pclass == 1 && randint(3) == 1) ||
		    py.flags.resist_fear) {
		    msg_print("You stand your ground!");
		}
		else if (f_ptr->afraid < 1) {
		    msg_print("You are suddenly afraid!");
		    f_ptr->afraid += 3 + randint((int)r_ptr->level);
		}
		else {
		    f_ptr->afraid += 3;
		    notice = FALSE;
		}
		break;

	      /* Fire attack */
	      case 5:
		msg_print("You are enveloped in flames!");
		fire_dam(damage, ddesc);
		break;

	      /* Acid attack */
	      case 6:
		msg_print("You are covered in acid!");
		acid_dam(damage, ddesc);
		break;

	      /* Cold attack */
	      case 7:
		msg_print("You are covered with frost!");
		cold_dam(damage, ddesc);
		break;

	      /* Lightning attack */
	      case 8:
		msg_print("Lightning strikes you!");
		light_dam(damage, ddesc);
		break;

	      /* Hack -- Corrosion attack (really acid) */
	      case 9:
		msg_print("A stinging red gas swirls about you.");
		corrode_gas(ddesc);
		take_hit(damage, ddesc);
		break;

	      /* Blindness attack */
	      case 10:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (!f_ptr->resist_blind) {
		    if (f_ptr->blind < 1) {
			f_ptr->blind += 10 + randint((int)r_ptr->level);
			msg_print("Your eyes begin to sting.");
		    }
		    else {
			f_ptr->blind += 5;
			notice = FALSE;
		    }
		}
		break;

	      /* Paralysis attack */
	      case 11:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (player_saves())
		    msg_print("You resist the effects!");
		else if (f_ptr->paralysis < 1) {
		    if (f_ptr->free_act) {
			msg_print("You are unaffected.");
		    }
		    else {
			f_ptr->paralysis = randint((int)r_ptr->level) + 3;
			msg_print("You are paralysed.");
		    }
		}
		else {
		    notice = FALSE;
		}
		break;

	      /* Steal Money */
	      case 12:
		/* immune to steal at 18/150 */
		if ((py.flags.paralysis < 1) &&
		    (randint(168) < py.stats.use_stat[A_DEX])) {
		    msg_print("You quickly protect your money pouch!");
		}
		else {		   /* make this more sane.... -CWS */
		    vtype               t1;
		    gold = (p_ptr->au / 10) + randint(25);
		    if (gold > 5000) gold = 2000 + randint(1000) + (p_ptr->au / 20);
		    if (gold > p_ptr->au) gold = p_ptr->au;
		    p_ptr->au -= gold;
		    msg_print("Your purse feels lighter.");
			sprintf(t1, "%ld coin%s stolen!", (long)gold,
			    (gold > 1L) ? "s were" : " was");
		    msg_print(t1);
		    prt_gold();
		}
		if (rand_int(2)) {
		    msg_print("There is a puff of smoke!");
		    blinked = 1;   /* added -CFT */
		    teleport_away(m_idx, MAX_SIGHT);
		}
		break;

	      /* Steal Object */
	      case 13:
		/* immune to steal at 18/150 dexterity */
		if ((py.flags.paralysis < 1) &&
		    (randint(168) < py.stats.use_stat[A_DEX])) {
		    msg_print("You grab hold of your backpack!");
		}
		else {
		    vtype               t1, t2;

		    /* Steal a single item from the pack */
		    i = rand_int(inven_ctr);

		    /* Don't steal artifacts  -CFT */
		    if ((inventory[i].tval >= TV_MIN_WEAR) &&
			(inventory[i].tval <= TV_MAX_WEAR) &&
			(inventory[i].flags2 & TR_ARTIFACT))
			break;

		    /* Get a description */
		    objdes(t1, &inventory[i], FALSE);

		    /* stacked single items */
		    sprintf(t2, "%sour %s (%c) %s stolen!",
			  ((inventory[i].sval <= ITEM_SINGLE_STACK_MAX) &&
			   (inventory[i].number > 1))
			    ? "One of y" : "Y",
			    t1, i + 'a',
		    /* stacked group items */
			    ((inventory[i].sval > ITEM_SINGLE_STACK_MAX) &&
			     (inventory[i].number > 1))
			    ? "were" : "was");
		    msg_print(t2);

		    /* Steal the items */
		    inven_destroy(i);
		}

		/* Allow monster to "blink" away */
		if (randint(3) == 1) {
		    msg_print("There is a puff of smoke!");
		    blinked = 1;   /* added -CFT */
		    teleport_away(m_idx, MAX_SIGHT);
		}

		break;

	      /* Poison	 */
	      case 14:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (!f_ptr->immune_pois &&
		    !f_ptr->resist_pois &&
		    !f_ptr->oppose_pois) {
		    msg_print("You feel very sick.");
		    f_ptr->poisoned += randint((int)r_ptr->level) + 5;
		}
		else {
		    msg_print("The poison has no effect.");
		}
		break;

	      /* Lose dexterity */
	      case 15:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (f_ptr->sustain_dex) {
		    msg_print("You feel clumsy for a moment, but it passes.");
		}
		else {
		    msg_print("You feel more clumsy.");
		    (void)dec_stat(A_DEX);
		}
		break;

	      /* Lose constitution */
	      case 16:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (f_ptr->sustain_con) {
		    msg_print("Your body resists the effects of the disease.");
		}
		else {
		    msg_print("Your health is damaged!");
		    (void)dec_stat(A_CON);
		}
		break;

	      /* Lose intelligence */
	      case 17:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		msg_print("You have trouble thinking clearly.");
		if (f_ptr->sustain_int) {
		    msg_print("But your mind quickly clears.");
		}
		else {
		    (void)dec_stat(A_INT);
		}
		break;

	      /* Lose wisdom */
	      case 18:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (f_ptr->sustain_wis) {
		    msg_print("Your wisdom is sustained.");
		}
		else {
		    msg_print("Your wisdom is drained.");
		    (void)dec_stat(A_WIS);
		}
		break;

	      /* Lose experience  */
	      case 19:
		f_ptr = &py.flags;
		if (f_ptr->hold_life && rand_int(5)) {
		    msg_print("You keep hold of your life force!");
		}
		else {
		    if (f_ptr->hold_life) {
			msg_print("You feel your life slipping away!");
			lose_exp(damage + (py.misc.exp/1000) * MON_DRAIN_LIFE);
		    }
		    else {
			msg_print("You feel your life draining away!");
			lose_exp(damage + (py.misc.exp/100) * MON_DRAIN_LIFE);
		    }
		}
		break;

	      /* Aggravate monster */
	      case 20:
		(void)aggravate_monster(20);
		break;

	      /* Disenchant */
	      case 21:

		/* Allow complete resist */
		if (!py.flags.resist_disen) {
		    byte               chance = 0;

		    take_hit(damage, ddesc);
		    flag = FALSE;
		    switch (randint(7)) {
		      case 1:
			i = INVEN_WIELD;
			break;
		      case 2:
			i = INVEN_BODY;
			break;
		      case 3:
			i = INVEN_ARM;
			break;
		      case 4:
			i = INVEN_OUTER;
			break;
		      case 5:
			i = INVEN_HANDS;
			break;
		      case 6:
			i = INVEN_HEAD;
			break;
		      case 7:
			i = INVEN_FEET;
			break;
		    }
		    i_ptr = &inventory[i];
		    if (i_ptr->tval != TV_NOTHING) {
			if (i_ptr->flags2 & TR_ARTIFACT)
			    chance = randint(5);
			if ((i_ptr->tohit > 0) && (chance < 3)){
			    i_ptr->tohit -= randint(2);
			    /* don't send it below zero */
			    if (i_ptr->tohit < 0)
				i_ptr->tohit = 0;
			    flag = TRUE;
			}
			if ((i_ptr->todam > 0) && (chance < 3)) {
			    i_ptr->todam -= randint(2);
			    /* don't send it below zero */
			    if (i_ptr->todam < 0)
				i_ptr->todam = 0;
			    flag = TRUE;
			}
			if ((i_ptr->toac > 0) && (chance < 3)) {
			    i_ptr->toac  -= randint(2);
			    /* don't send it below zero */
			    if (i_ptr->toac < 0)
				i_ptr->toac = 0;
			    flag = TRUE;
			}
			if (flag || (chance > 2)) {
			    vtype t1, t2;
			    objdes(t1, &inventory[i], FALSE);
			    if (chance < 3)
				sprintf(t2, "Your %s (%c) %s disenchanted!", t1,
					i+'a'-INVEN_WIELD,
					(inventory[i].number != 1) ? "were":"was");
			    else
				sprintf(t2, "Your %s (%c) %s disenchantment!", t1,
					i+'a'-INVEN_WIELD,
					(inventory[i].number != 1) ? "resist":"resists");
			    msg_print (t2);
			    calc_bonuses ();
			}
			else
			    notice = FALSE;
		    }
		}
		break;

	      /* Eat food */
	      case 22:
		if (find_range(TV_FOOD, TV_NEVER, &i, &j)) {
		    inven_destroy(i);
		    msg_print("It got at your rations!");
		}
		else {
		    notice = FALSE;
		}
		break;

	      /* Eat light */
	      case 23:
		i_ptr = &inventory[INVEN_LIGHT];
		if ((i_ptr->p1 > 0) && ((i_ptr->flags2 & TR_ARTIFACT) == 0)) {
		    i_ptr->p1 -= (250 + randint(250));
		    if (i_ptr->p1 < 1)
			i_ptr->p1 = 1;
		    if (py.flags.blind < 1) {
			msg_print("Your light dims.");
		    }
		    else {
			notice = FALSE;
		    }
		}
		else {
		    notice = FALSE;
		}
		break;

	      /* Eat charges */
	      case 24:
		i = rand_int(inven_ctr);
		j = r_ptr->level;
		i_ptr = &inventory[i];
		if (((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND)) &&
		    (i_ptr->p1 > 0)) {
		    m_ptr->hp += j * i_ptr->p1;
		    i_ptr->p1 = 0;
		    if (!known2_p(i_ptr))
			add_inscribe(i_ptr, ID_EMPTY);
		    msg_print("Energy drains from your pack!");
		}
		else {
		    notice = FALSE;
		}
		break;

	      /* Drain all stats. Haha! SM */
	      case 25:
		f_ptr = &py.flags;
		take_hit(damage, ddesc);
		if (f_ptr->sustain_str) {
		    msg_print("You feel weaker for a moment, but it passes.");
		}
		else {
		    msg_print("You feel weaker.");
		    (void)dec_stat(A_STR);
		}
		if (f_ptr->sustain_dex) {
		    msg_print("You feel clumsy for a moment, but it passes.");
		}
		else {
		    msg_print("You feel more clumsy.");
		    (void)dec_stat(A_DEX);
		}
		if (f_ptr->sustain_con) {
		    msg_print("Your body resists the effects of the disease.");
		}
		else {
		    msg_print("Your health is damaged!");
		    (void)dec_stat(A_CON);
		}
		msg_print("You have trouble thinking clearly.");
		if (f_ptr->sustain_int) {
		    msg_print("But your mind quickly clears.");
		}
		else {
		    (void)dec_stat(A_INT);
		}
		if (f_ptr->sustain_wis) {
		    msg_print("Your wisdom is sustained.");
		}
		else {
		    msg_print("Your wisdom is drained.");
		    (void)dec_stat(A_WIS);
		}
		if (f_ptr->sustain_chr) {
		    msg_print("You keep your good looks.");
		}
		else {
		    msg_print("Your features are twisted.");
		    (void)dec_stat(A_CHR);
		}
		break;

	      case 99:
		notice = FALSE;
		break;

	      default:
		notice = FALSE;
		break;
	    }

	    if (CUT && STUN) {
		switch (randint(2)) {
		  case 1: CUT = FALSE; break;
		  case 2: STUN = FALSE; break;
		}
	    }

	    switch (monster_critical(adice, asides, damage)) {
	      case 0: break;
	      case 1:
		if (CUT) cut_player(randint(5));
		else if (STUN) stun_player(randint(5));
		break;
	      case 2:
		if (CUT) cut_player(randint(5) + 5);
		else if (STUN) stun_player(randint(5) + 5);
		break;
	      case 3:
		if (CUT) cut_player(randint(30) + 20);
		else if (STUN) stun_player(randint(20) + 10);
		break;
	      case 4:
		if (CUT) cut_player(randint(70) + 30);
		else if (STUN) stun_player(randint(40) + 30);
		break;
	      case 5:
		if (CUT) cut_player(randint(250) + 50);
		else if (STUN) stun_player(randint(50) + 40);
		break;
	      case 6:
		if (CUT) cut_player(300);
		else if (STUN) stun_player(randint(60) + 57);
		break;
	      default:
		if (CUT) cut_player(5000);
		else if (STUN) stun_player(100 + randint(10));
		break;
	    }

	    /* monster is only confused if it actually hits */
	    /* if no attacks, monster can't get confused -dbd */
	    if (!attype) {
		if (py.flags.confuse_monster && py.flags.protevil <= 0) {
		    msg_print("Your hands stop glowing.");
		    py.flags.confuse_monster = FALSE;
		    if ((randint(MAX_R_LEV) < r_ptr->level) ||
			(CHARM_SLEEP & r_ptr->cdefense)) {
			(void)sprintf(tmp_str, "%sis unaffected.", cdesc);
		    }
		    else {
			(void)sprintf(tmp_str, "%sappears confused.", cdesc);
			m_ptr->confused = TRUE;
		    }
		    msg_print(tmp_str);

		    if (visible && !death && !rand_int(4)) {
			c_recall[m_ptr->r_idx].r_cdefense |=
			    r_ptr->cdefense & CHARM_SLEEP;
		    }
		}
	    }

	/*
	 * increase number of attacks if notice true, or if had previously
	 * noticed the attack (in which case all this does is help player
	 * learn damage), note that in the second case do not increase
	 * attacks if creature repelled (no damage done) 
	 */
	    if ((notice ||
		 (c_recall[m_ptr->r_idx].r_attacks[attackn] != 0 &&
		  attype != 99))
		&& c_recall[m_ptr->r_idx].r_attacks[attackn] < MAX_UCHAR) {
		c_recall[m_ptr->r_idx].r_attacks[attackn]++;
	    }
	    if (visible && death && c_recall[m_ptr->r_idx].r_deaths < MAX_SHORT) {
		c_recall[m_ptr->r_idx].r_deaths++;
	    }
	}
	else {
	    if ((adesc >= 1 && adesc <= 3) || (adesc == 6)) {
		disturb(1, 0);
		(void)strcpy(tmp_str, cdesc);
		msg_print(strcat(tmp_str, "misses you."));
	    }
	}

	if (attackn < MAX_MON_NATTACK - 1) {
	    attackn++;
	}
	else {
	    break;
	}
    }
}



/*
 * Given five choices of moves, make the first legal one. -RAK-
 */
static void make_move(int m_idx, int *mm, u32b *rcmove)
{
    int do_turn, do_move, stuck_door;
    u32b                movebits;
    register cave_type    *c_ptr;
    register inven_type   *i_ptr;
    register monster_type *m_ptr;

    int                   i, newy, newx;

#ifdef ATARIST_MWC
    u32b              holder;
#endif

    i = 0;
    do_turn = FALSE;
    do_move = FALSE;


    /* Access the monster */
    m_ptr = &m_list[m_idx];
    movebits = c_list[m_ptr->r_idx].cmove;

    do {

	/* Get the position of the i'th move */
	newy = m_ptr->fy;
	newx = m_ptr->fx;
	(void)mmove(mm[i], &newy, &newx);

	/* Access that cave grid */
	c_ptr = &cave[newy][newx];

	/* Access that cave grid's contents */
	i_ptr = &i_list[c_ptr->i_idx];


	if ((i == 4) && (m_ptr->monfear) &&  /* cornered (or things in the way!) -CWS */
	    (!floor_grid_bold(newy, newx) || (c_ptr->cptr > 1))) {
	    monster_race      *r_ptr = &c_list[m_ptr->r_idx];
	    vtype               m_name, out_val;
	    
	    m_ptr->monfear = 0;
	    if (m_ptr->ml && los(char_row, char_col, m_ptr->fy, m_ptr->fx)) {
		monster_name(m_name, m_ptr, r_ptr);
		sprintf(out_val, "%s turns to fight!", m_name);
		msg_print(out_val);
	    }
	    break;		/* don't try to actually do anything -CWS */
	}

	if (c_ptr->fval != BOUNDARY_WALL) {
	/* Floor is open?		   */
	    if (floor_grid_bold(newy, newx))
		do_move = TRUE;
	/* Creature moves through walls? */
	    else if (movebits & CM_PHASE) {
		do_move = TRUE;
		*rcmove |= CM_PHASE;
	    } else if (c_list[m_ptr->r_idx].cdefense & BREAK_WALL) {
	    /* Crunch up those Walls Morgoth and Umber Hulks!!!! */

		do_move = TRUE;
		c_recall[m_ptr->r_idx].r_cdefense |= BREAK_WALL;
		if ((i_ptr->tval == TV_CLOSED_DOOR) ||
		    (i_ptr->tval == TV_SECRET_DOOR)) {	/* break the door -CFT  */
		    invcopy(i_ptr, OBJ_OPEN_DOOR);
		    i_ptr->p1 = (-1);          /* make it broken, not just open */
		    c_ptr->fval = CORR_FLOOR;	        /* change floor setting */
		    lite_spot(newy, newx);	        /* show broken door     */

		/* Message */
		msg_print("You hear a door burst open!");
		disturb(1, 0);
	    }

	    /* Smash through walls */
	    else
		(void)twall(newy, newx, 1, 0);
	    }

	/* Creature can open doors? */
	else if (c_ptr->i_idx) {

	    /* Creature can open doors. */
	    if (movebits & CM_OPEN_DOOR) {

		stuck_door = FALSE;

		if (i_ptr->tval == TV_CLOSED_DOOR) {

		    do_turn = TRUE;

		    /* XXX Hack -- scared monsters can open locked/stuck doors */
		    if ((m_ptr->monfear) && rand_int(2)) {
			i_ptr->p1 = 0;
		    }

		    /* Open doors */
		    if (i_ptr->p1 == 0) {
			do_move = TRUE;
		    }

		    /* Locked doors -- take a turn to unlock it */
		    else if (i_ptr->p1 > 0) {
			if (randint((m_ptr->hp + 1) * (50 + i_ptr->p1)) <
			    40 * (m_ptr->hp - 10 - i_ptr->p1))
				i_ptr->p1 = 0;
			}

		    /* Stuck doors */
		    else if (i_ptr->p1 < 0) {
			if (randint((m_ptr->hp + 1) * (50 - i_ptr->p1)) <
				40 * (m_ptr->hp - 10 + i_ptr->p1)) {
			    msg_print("You hear a door burst open!");
			    disturb(1, 0);
			    stuck_door = TRUE;
			    do_move = TRUE;
			}
		    }
		}

		/* Hack -- monsters open secret doors */
		else if (i_ptr->tval == TV_SECRET_DOOR) {
		    do_turn = TRUE;
		    do_move = TRUE;
		}


		/* Deal with doors in the way */
		if (do_move) {

		    /* XXX Should create a new object XXX */
		    invcopy(i_ptr, OBJ_OPEN_DOOR);

		    /* 50% chance of breaking door */
		    if (stuck_door) i_ptr->p1 = 0 - rand_int(2);

			c_ptr->fval = CORR_FLOOR;

		    /* Redraw door */
		    lite_spot(newy, newx);

		    *rcmove |= CM_OPEN_DOOR;

		    /* Hack -- should move into open doorway */
		    do_move = FALSE;
		}
	    }

	    /* Creature can not open doors, must bash them   */
	    else {

		if (i_ptr->tval == TV_CLOSED_DOOR) {
		    do_turn = TRUE;
		    if (randint((m_ptr->hp + 1) * (80 + MY_ABS(i_ptr->p1))) <
			40 * (m_ptr->hp - 20 - MY_ABS(i_ptr->p1))) {

			/* XXX Should create a new object XXX */
			invcopy(i_ptr, OBJ_OPEN_DOOR);

			/* 50% chance of breaking door */
			i_ptr->p1 = 0 - rand_int(2);
			    c_ptr->fval = CORR_FLOOR;

			/* Redraw */
			lite_spot(newy, newx);
			msg_print("You hear a door burst open!");
			disturb(1, 0);
		    }
		}
	    }
	}


	/* Hack -- check for Glyph of Warding */
	if (do_move && (c_ptr->i_idx != 0) &&
	    (i_ptr->tval == TV_VIS_TRAP) && (i_ptr->sval == 99)) {

	    /* Break the ward */
	    if (randint(OBJ_BREAK_GLYPH) < c_list[m_ptr->r_idx].level) {
		if ((newy == char_row) && (newx == char_col)) {
		    msg_print("The rune of protection is broken!");
		}
		delete_object(newy, newx);
		}
		else {
		    do_move = FALSE;
/* If the creature moves only to attack, don't let it move if the glyph
 * prevents it from attacking */
		    if (movebits & CM_ATTACK_ONLY)
			do_turn = TRUE;
		}
	    }

	/* Process player or OTHER monster in the way */
	if (do_move)
		if (c_ptr->cptr == 1) {
		/*
		 * if the monster is not lit, must call update_mon, it may be
		 * faster than character, and hence could have just moved
		 * next to character this same turn 
		 */
		    if (!m_ptr->ml)
			update_mon(m_idx);

		/* Do the attack */
		make_attack(m_idx);

		/* No move to do, but turn is done */
		do_move = FALSE;
		do_turn = TRUE;
	    }

	    /* Creature is attempting to move on other creature?	   */
	    else if ((c_ptr->cptr > 1) &&
			 ((newy != m_ptr->fy) ||
			  (newx != m_ptr->fx))) {

		/* Creature eats other creatures? */
#ifdef ATARIST_MWC
		    if ((movebits & (holder = CM_EATS_OTHER)) &&
#else
		    if ((movebits & CM_EATS_OTHER) &&
#endif
			(c_list[m_ptr->r_idx].mexp >
			 c_list[m_list[c_ptr->cptr].r_idx].mexp)) {
			if (m_list[c_ptr->cptr].ml)
#ifdef ATARIST_MWC
			    *rcmove |= holder;
#else
    			    *rcmove |= CM_EATS_OTHER;
#endif
			/* It ate an already processed monster. Handle normally. */
			if (m_idx < c_ptr->cptr)
			    delete_monster((int)c_ptr->cptr);
		    /*
		     * If it eats this monster, an already processed monster
		     * will take its place, causing all kinds of havoc. 
		     * Delay the kill a bit. 
		     */
			else
			    fix1_delete_monster((int)c_ptr->cptr);
		    } else
			do_move = FALSE;
		    }

	/* Creature has been allowed move */
	if (do_move) {

	    /* Move the creature */
	    move_rec(m_ptr->fy, m_ptr->fx, newy, newx);

		if (m_ptr->ml) {
		    m_ptr->ml = FALSE;
		    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
		}
		m_ptr->fy = newy;
		m_ptr->fx = newx;
		m_ptr->cdis = distance(char_row, char_col, newy, newx);

	    /* A turn was taken */
	    do_turn = TRUE;


	    /* Pick up or eat an object	*/
#ifdef ATARIST_MWC
		if (movebits & (holder = CM_PICKS_UP))
#else
		if (movebits & CM_PICKS_UP)
#endif
		{
/* used in code to prevent orcs from picking up Slay Orc weapons, etc -CFT */
		    u32b t;

		/* Check the grid */
		c_ptr = &cave[newy][newx];
		i_ptr = &i_list[c_ptr->i_idx];

		    if ((c_ptr->i_idx != 0)
			&& (i_ptr->tval <= TV_MAX_OBJECT)) {
#ifdef ATARIST_MWC
			*rcmove |= holder;
#else
			*rcmove |= CM_PICKS_UP;
#endif
			t = 0L;

			/* React to objects that hurt the monster */
			if (i_ptr->flags & TR1_SLAY_DRAGON) t |= DRAGON;
			if (i_ptr->flags & TR1_SLAY_X_DRAGON) t |= DRAGON;                            
			if (i_ptr->flags & TR1_SLAY_UNDEAD) t |= UNDEAD;
			if (i_ptr->flags2 & TR1_SLAY_DEMON) t |= DEMON;
			if (i_ptr->flags2 & TR1_SLAY_TROLL) t |= TROLL;
			if (i_ptr->flags2 & TR1_SLAY_GIANT) t |= GIANT;
			if (i_ptr->flags2 & TR1_SLAY_ORC) t |= ORC;

		    /* if artifact, or wearable & hurts this monster -CWS */
			if ((i_ptr->flags2 & TR_ARTIFACT) ||
			    ( (i_ptr->tval >= TV_MIN_WEAR) &&
			      (i_ptr->tval <= TV_MAX_WEAR) &&
			      (c_list[m_ptr->r_idx].cdefense & t) )) {

/* FIXME: should use new line-splitting code */

			    vtype               m_name, out_val, i_name;
			    int                 ii, split = (-1);

			    update_mon(m_idx);	/* make sure ml see right -CFT */
			    if ((m_ptr->ml) && los(char_row, char_col, m_ptr->fy,
						   m_ptr->fx)) {
			    /* if we can see it, tell us what happened -CFT */
				monster_name(m_name, m_ptr, &(c_list[m_ptr->r_idx]));

			    /* Acquire the object name */
			    objdes(i_name, &(i_list[c_ptr->i_idx]), TRUE);

				sprintf(out_val,
					"%s tries to pick up %s, but stops suddenly!",
					m_name, i_name);
				for (ii = 0; ii < 72 && out_val[ii]; ii++)
				    if (out_val[ii] == ' ')
					split = ii;
				if ((ii > 71) && (split != (-1))) {
				/* then we should probably split it -CFT */
				    out_val[split] = 0;
				    msg_print(out_val);
				    msg_print(&out_val[split + 1]);
				} else
			   /* if ii <= 71, then it'll fit nicely in 1 line.
			    * Or, we found no space to split at... -CFT */
				    msg_print(out_val);
			    } /* if can see */
			}

		    /* Let the creature "eat" it */
		    else {
			delete_object(newy, newx);
		    }
		    }
		}
	    }
        }
	i++;
    /* Up to 5 attempts at moving,   give up.	  */
    }
	while ((!do_turn) && (i < 5));
}



/*
 * Creatures can cast spells (and breathe) too.   -RAK-
 *
 * cast_spell = true if creature changes position
 * took_turn  = true if creature casts a spell		 
 */
static void mon_cast_spell(int m_idx, int *took_turn)
{
    u32b		i;
    int			y, x, chance, thrown_spell, r1;
    register int	k;
    int			spell_choice[64];
    bool		desperate = FALSE;
    vtype		cdesc, outval, ddesc;

    struct flags  *f_ptr;
    monster_type	*m_ptr;
    monster_race	*r_ptr;
    char                   sex;

    /* Extract the blind-ness -CFT */
    int blind = (py.flags.blind > 0);

    /* Already dead */
    if (death) return;

    /* Access the monster */
    m_ptr = &m_list[m_idx];
    r_ptr = &c_list[m_ptr->r_idx];

    sex = r_ptr->gender;

    /* Hack -- extract the "1 in x" chance of casting spell */
    chance = (int)(r_ptr->spells & CS_FREQ);

    /* Mega-Paranoia */
    if (chance == 0) {
	msg_print("CHANCE == 0");
	msg_print("caused by ....");
	msg_print(r_ptr->name);
	*took_turn = FALSE;
    }

    /* Failed to cast */
    else if (rand_int(chance)) *took_turn = FALSE;

    /* Must be within certain range */
    else if (m_ptr->cdis > MAX_SPELL_DIS) *took_turn = FALSE;

    /* Must have unobstructed Line-Of-Sight, from Monster to Player -CWS */
    else if (!los((int)m_ptr->fy, (int)m_ptr->fx, char_row, char_col)) *took_turn = FALSE;

    else {			   /* Creature is going to cast a spell	 */
	*took_turn = TRUE;
    /* Check to see if monster should be lit. */
	update_mon(m_idx);
    /* Describe the attack			       */
	if (m_ptr->ml) {
	    if (r_ptr->cdefense & UNIQUE)
		(void)sprintf(cdesc, "%s ", r_ptr->name);
	    else
		(void)sprintf(cdesc, "The %s ", r_ptr->name);
	} else
	    (void)strcpy(cdesc, "It ");
    /* For "DIED_FROM" string	 */
	if (UNIQUE & r_ptr->cdefense)
	    (void)sprintf(ddesc, "%s", r_ptr->name);
	else if (is_a_vowel(r_ptr->name[0]))
	    (void)sprintf(ddesc, "an %s", r_ptr->name);
	else
	    (void)sprintf(ddesc, "a %s", r_ptr->name);
    /* End DIED_FROM		       */

    /* Extract all possible spells into spell_choice */
    if ((r_ptr->cdefense & INTELLIGENT) &&
	(m_ptr->hp < ((r_ptr->hd[0] * r_ptr->hd[1]) / 10)) &&
	(r_ptr->spells & CS_INT1 || r_ptr->spells2 & CS_INT2 ||
	 r_ptr->spells3 & CS_INT3) && rand_int(2)) {

	desperate = TRUE;
	c_recall[m_ptr->r_idx].r_cdefense |= INTELLIGENT;
    }

    /* Extract the first set of spells */
    i = (r_ptr->spells & ~CS_FREQ);
    if (desperate) i &= CS_INT1;
	k = 0;
    while (i) spell_choice[k++] = bit_pos(&i);

    /* Extract the second set of spells */
    i = r_ptr->spells2;
    if (desperate) i &= CS_INT2;
    while (i) spell_choice[k++] = bit_pos(&i) + 32;

    /* Extract the third set of spells */
    i = r_ptr->spells3;
    if (desperate) i &= CS_INT3;
    while (i) spell_choice[k++] = bit_pos(&i) + 64;

    /* Choose a spell to cast */
    if (!k) thrown_spell = 200;
    else thrown_spell = spell_choice[rand_int(k)];
    thrown_spell++;

    /* all except teleport_away() and drain mana spells always disturb */
    if (thrown_spell > 6 && thrown_spell != 7) disturb(1, 0);

	/* Cast the spell. */
	switch (thrown_spell) {

	  case 5:		   /* Teleport Short */
	    (void)strcat(cdesc, "blinks away.");
	    msg_print(cdesc);
	    teleport_away(m_idx, 5);
	    break;

	  case 6:		   /* Teleport Long */
	    (void)strcat(cdesc, "teleports away.");
	    msg_print(cdesc);
	    teleport_away(m_idx, MAX_SIGHT);
	    break;

	  case 7:		   /* Teleport To	 */
	    (void)strcat(cdesc, "commands you to return!");
	    msg_print(cdesc);
	    teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	    break;

	  case 8:		   /* Light Wound	 */
	    if (!blind) strcat(cdesc, "points at you and curses.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    if (player_saves()) msg_print("You resist the effects of the spell.");
	    else take_hit(damroll(3, 8), ddesc);
	    break;

	  case 9:		   /* Serious Wound */
	    if (!blind) strcat(cdesc, "points at you and curses horribly.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    if (player_saves()) msg_print("You resist the effects of the spell.");
	    else take_hit(damroll(8, 8), ddesc);
	    break;

	  case 10:		   /* Hold Person	  */
	    if (!blind) strcat(cdesc, "gazes deep into your eyes!");
	    else strcat(cdesc, "mumbles, and you feel something holding you!");
	    msg_print(cdesc);
	    if (py.flags.free_act) {
		msg_print("You are unaffected.");
	    }
	    else if (player_saves()) {
		if (!blind) msg_print("You stare back unafraid!");
		else msg_print("You resist!");
	    }
	    else if (py.flags.paralysis > 0) {
		py.flags.paralysis += 2;
	    }
	    else {
		py.flags.paralysis = randint(5) + 4;
	    }
	    break;

	  case 11:		   /* Cause Blindness */
	    if (!blind) strcat(cdesc, "casts a spell, burning your eyes!");
	    else strcat(cdesc, "mumbles, and your eyes burn even more.");
	    msg_print(cdesc);
	    if ((player_saves()) || (py.flags.resist_blind)) {
		if (!blind) msg_print("You blink and your vision clears.");
		else msg_print("But the extra burning quickly fades away.");
	    }
	    else if (py.flags.blind > 0) {
		py.flags.blind += 6;
	    }
	    else {
		py.flags.blind += 12 + randint(3);
	    }
	    break;

	  case 12:		   /* Cause Confuse */
	    if (!blind) strcat(cdesc, "creates a mesmerising illusion.");
	    else strcat(cdesc, "mumbles, and you hear puzzling noises.");
	    msg_print(cdesc);
	    if ((player_saves()) ||
		(py.flags.resist_conf) ||
		(py.flags.resist_chaos)) {
		msg_print("You disbelieve the feeble spell.");
	    }
	    else if (py.flags.confused > 0) {
		py.flags.confused += 2;
	    }
	    else {
		py.flags.confused = randint(5) + 3;
	    }
	    break;

	  case 13:		   /* Cause Fear	  */
	    if (!blind) strcat(cdesc, "casts a fearful illusion.");
	    else strcat(cdesc, "mumbles, and you hear scary noises.");
	    msg_print(cdesc);
	    if (player_saves() || py.flags.resist_fear) {
		msg_print("You refuse to be frightened.");
	    }
	    else if (py.flags.afraid > 0) {
		py.flags.afraid += 2;
	    }
	    else {
		py.flags.afraid = randint(5) + 3;
	    }
	    break;

	  case 14:		   /* Summon Monster */
	    if (!blind) strcat(cdesc, "magically summons help!");
	    else strcat(cdesc, "mumbles, and you hear something appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    hack_m_idx = m_idx;
	    summon_monster(&y, &x, FALSE);
	    hack_m_idx = (-1);
	    update_mon((int)cave[y][x].cptr);
	    break;

	  case 15:		   /* Summon Undead */
	    if (!blind) strcat(cdesc, "magically summons help from beyond the grave!");
	    else strcat(cdesc, "mumbles, and you hear something creepy appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    hack_m_idx = m_idx;
	    summon_undead(&y, &x);
	    hack_m_idx = (-1);
	    update_mon((int)cave[y][x].cptr);
	    break;

	  case 16:		   /* Slow Person	 */
	    strcat(cdesc, "drains power from your muscles!");
	    msg_print(cdesc);
	    if (py.flags.free_act) {
		msg_print("You are unaffected.");
	    }
	    else if (player_saves()) {
		msg_print("Your body resists the spell.");
	    }
	    else if (py.flags.slow > 0) {
		py.flags.slow += 2;
	    }
	    else {
		py.flags.slow = randint(5) + 3;
	    }
	    break;

	  case 17:		   /* Drain Mana	 */
	    if (py.misc.cmana > 0) {
		disturb(1, 0);
		sprintf(outval, "%sdraws psychic energy from you!", cdesc);
		msg_print(outval);
		if (m_ptr->ml) {
		    sprintf(outval, "%sappears healthier.", cdesc);
		    msg_print(outval);
		}
		r1 = (randint((int)r_ptr->level) >> 1) + 1;
		if (r1 > py.misc.cmana) {
		    r1 = py.misc.cmana;
		    py.misc.cmana = 0;
		    py.misc.cmana_frac = 0;
		}
		else {
		    py.misc.cmana -= r1;
		}
		prt_cmana();
		m_ptr->hp += 6 * (r1);
	    }
	    break;

	  case 18:		   /* Summon Demon */
	    if (!blind) strcat(cdesc, "summons a hellish adversary!");
	    else strcat(cdesc, "mumbles, and you smell fire and brimstone nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    hack_m_idx = m_idx;
	    summon_demon(c_list[m_ptr->r_idx].level, &y, &x);
	    hack_m_idx = (-1);
	    update_mon((int)cave[y][x].cptr);
	    break;

	  case 19:		   /* Summon Dragon */
	    if (!blind) strcat(cdesc, "magically summons a Dragon!");
	    else strcat(cdesc, "mumbles, and you hear something large appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    hack_m_idx = m_idx;
	    summon_dragon(&y, &x);
	    hack_m_idx = (-1);
	    update_mon((int)cave[y][x].cptr);
	    break;

	  case 20:		   /* Breath Lightning */
	    if (!blind) strcat(cdesc, "breathes lightning.");
	    else strcat(cdesc, "breathes, and you get zapped.");
	    msg_print(cdesc);
	    breath(GF_ELEC, char_row, char_col,
		   ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3)),
		   ddesc, m_idx);
	    break;

	  case 21:		   /* Breath Gas	 */
	    if (!blind) strcat(cdesc, "breathes gas.");
	    else strcat(cdesc, "breathes, and you inhale noxious gases.");
	    msg_print(cdesc);
	    breath(GF_POIS, char_row, char_col,
		((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3)), ddesc, m_idx);
	    break;

	  case 22:		   /* Breath Acid	 */
	    if (!blind) strcat(cdesc, "breathes acid.");
	    else strcat(cdesc, "breathes, and your skin is burning.");
	    msg_print(cdesc);
	    breath(GF_ACID, char_row, char_col,
		   ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3)),
		   ddesc, m_idx);
	    break;

	  case 23:		   /* Breath Frost */
	    if (!blind) strcat(cdesc, "breathes frost.");
	    else strcat(cdesc, "breathes, and feel a frigid blast.");
	    msg_print(cdesc);
	    breath(GF_COLD, char_row, char_col,
		   ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3)),
		   ddesc, m_idx);
	    break;

	  case 24:		   /* Breath Fire	 */
	    if (!blind) strcat(cdesc, "breathes fire.");
	    else strcat(cdesc, "breathes, and you're on fire.");
	    msg_print(cdesc);
	    breath(GF_FIRE, char_row, char_col,
		   ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3)),
		   ddesc, m_idx);
	    break;

	  case 25:		   /* Fire Bolt */
	    if (!blind) strcat(cdesc, "casts a Fire bolt.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    bolt(GF_FIRE, char_row, char_col,
		 damroll(9, 8) + (c_list[m_ptr->r_idx].level / 3)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 26:		   /* Frost Bolt */
	    if (!blind) strcat(cdesc, "casts a Frost bolt.");
	    else strcat(cdesc, "mumbles, and you feel a frigid blast .");
	    msg_print(cdesc);
	    bolt(GF_COLD, char_row, char_col,
		 damroll(6, 8) + (c_list[m_ptr->r_idx].level / 3)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 27:		   /* Acid Bolt */
	    if (!blind) strcat(cdesc, "casts a Acid bolt.");
	    else strcat(cdesc, "mumbles, and your skin burns.");
	    msg_print(cdesc);
	    bolt(GF_ACID, char_row, char_col,
		 damroll(7, 8) + (c_list[m_ptr->r_idx].level / 3)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 28:		   /* Magic Missiles */
	    if (!blind) strcat(cdesc, "casts a Magic missile.");
	    else strcat(cdesc, "mumbles, and you feel an arrow hit you.");
	    msg_print(cdesc);
	    bolt(GF_MISSILE, char_row, char_col,
		 damroll(2, 6) + (c_list[m_ptr->r_idx].level / 3)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 29:		   /* Critical Wound	 */
	    if (!blind) strcat(cdesc, "points at you, incanting terribly!");
	    else strcat(cdesc, "mumbles loudly.");
	    msg_print(cdesc);
	    if (player_saves()) {
		msg_print("You resist the effects of the spell.");
	    }
	    else {
		take_hit(damroll(10, 15), ddesc);
	    }
	    break;

	  case 30:		   /* Fire Ball */
	    if (!blind) strcat(cdesc, "casts a Fire ball.");
	    else strcat(cdesc, "mumbles, and you're on fire.");
	    msg_print(cdesc);
	    breath(GF_FIRE, char_row, char_col,
		   randint((c_list[m_ptr->r_idx].level * 7) / 2) + 10,
		   ddesc, m_idx);
	    break;

	  case 31:		   /* Frost Ball */
	    if (!blind) strcat(cdesc, "casts a Frost ball.");
	    else strcat(cdesc, "mumbles, and you feel a frigid blast.");
	    msg_print(cdesc);
	    breath(GF_COLD, char_row, char_col,
		   randint((c_list[m_ptr->r_idx].level * 3) / 2) + 10,
		   ddesc, m_idx);
	    break;

	  case 32:		   /* Mana Bolt */
	    if (!blind) strcat(cdesc, "casts a Mana bolt.");
	    else strcat(cdesc, "mumbles, and you feel a magical blast.");
	    msg_print(cdesc);
	    bolt(GF_MISSILE, char_row, char_col,
	    randint((c_list[m_ptr->r_idx].level * 7) / 2) + 50, ddesc, m_ptr,
		 m_idx);
	    break;

	  case 33:
	    if (!blind) strcat(cdesc, "breathes chaos.");
	    else strcat(cdesc, "breathes, and you feel a strange flux.");
	    msg_print(cdesc);
	    breath(GF_CHAOS, char_row, char_col,
	    ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 34:
	    if (!blind) strcat(cdesc, "breathes shards.");
	    else strcat(cdesc, "breathes, and sharp fragments cut you.");
	    msg_print(cdesc);
	    breath(GF_SHARDS, char_row, char_col,
	    ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 35:
	    if (!blind) strcat(cdesc, "breathes sound.");
	    else strcat(cdesc, "breathes, and you are deafened.");
	    msg_print(cdesc);
	    breath(GF_SOUND, char_row, char_col,
		((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 36:
	    if (!blind) strcat(cdesc, "breathes confusion.");
	    else strcat(cdesc, "breathes, and you feel dizzy.");
	    msg_print(cdesc);
	    breath(GF_CONFUSION, char_row, char_col,
		((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 37:
	    if (!blind) strcat(cdesc, "breathes disenchantment.");
	    else strcat(cdesc, "breathes, and your equipment seems less powerful.");
	    msg_print(cdesc);
	    breath(GF_DISENCHANT, char_row, char_col,
		((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 38:
	    if (!blind) strcat(cdesc, "breathes nether.");
	    else strcat(cdesc, "breathes, and you feel an unholy aura.");
	    msg_print(cdesc);
	    breath(GF_NETHER, char_row, char_col,
		   ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 39:
	    if (!blind) strcat(cdesc, "casts a Lightning bolt.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    bolt(GF_ELEC, char_row, char_col,
		 damroll(4, 8) + (c_list[m_ptr->r_idx].level / 3)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 40:
	    if (!blind) strcat(cdesc, "casts a Lightning Ball.");
	    else strcat(cdesc, "mumbles, and you get zapped.");
	    msg_print(cdesc);
	    breath(GF_ELEC, char_row, char_col,
		randint((c_list[m_ptr->r_idx].level * 3) / 2) + 8, ddesc, m_idx);
	    break;

	  case 41:
	    if (!blind) strcat(cdesc, "casts an Acid Ball.");
	    else strcat(cdesc, "mumbles, and your skin is burning.");
	    msg_print(cdesc);
	    breath(GF_ACID, char_row, char_col,
		   randint(c_list[m_ptr->r_idx].level * 3) + 15, ddesc, m_idx);
	    break;

	  case 42:
	    if (!blind) strcat(cdesc, "casts a spell and cackles evilly.");
	    else strcat(cdesc, "mumbles, and then cackles evilly.");
	    msg_print(cdesc);
	    (void)trap_creation();
	    break;

	  case 43:
	    if (!blind) strcat(cdesc, "points at you, screaming the word DIE!");
	    else strcat(cdesc, "mumbles, and then screams 'DIE!'.");
	    msg_print(cdesc);
	    if (player_saves()) {
		msg_print("You laugh at the feeble spell.");
	    }
	    else {
		msg_print("You start to bleed!");
		take_hit(damroll(15, 15), ddesc);
		cut_player(m_ptr->hp);
	    }
	    break;

	  case 44:
	    if (!blind) {
		strcat(cdesc, "stares at you.");
		msg_print(cdesc);
	    }
	    else {
		msg_print("You feel something focusing on your mind.");
	    }

	    if (player_saves()) {
		msg_print("You resist the effects.");
	    }
	    else {
		msg_print("Your mind is blasted by psionic energy.");
		if ((!py.flags.resist_conf) && (!py.flags.resist_chaos)) {
		    if (py.flags.confused > 0) {
			py.flags.confused += 2;
		    }
		    else {
			py.flags.confused = randint(5) + 3;
		    }
		}
		take_hit(damroll(8, 8), ddesc);
	    }
	    break;

	  case 45:
	    )strcat(cdesc, "teleports you away.");
	    msg_print(cdesc);
	    (void)teleport(100);
	    break;

	  case 46:		   /* healing, added monster fear code -CWS */

	    if (!blind) {
		sprintf(outval, "%sconcentrates on %s wounds.", cdesc,
			      (sex == 'm' ? "his" : sex == 'f' ? "her"
			       : sex == 'p' ? "their" : "its"));
	    }
	    else {
		sprintf(outval, "%smumbles to itself.", cdesc);
	    }
	    msg_print(outval);
	    monster_is_afraid = 0;

	    if (m_ptr->maxhp == 0) {	/* then we're just going to fix it! -CFT */
		if ((c_list[m_ptr->r_idx].cdefense & MAX_HP) )
		    m_ptr->maxhp = max_hp(c_list[m_ptr->r_idx].hd);
		else
		    m_ptr->maxhp = pdamroll(c_list[m_ptr->r_idx].hd);
	    }
	    if (!blind) /* if we bother to say "mumbles" above, then shouldn't see it heal itself -CFT */
		strcat(cdesc, "looks ");
	    else
		strcat(cdesc, "sounds ");

	    /* Hack -- Already fully healed */
	    if (m_ptr->hp >= m_ptr->maxhp) {
/* need >= because, if we recalc-ed maxhp, we might have gotten a low roll,
 * which could be below hp -CFT
 */

		strcat(cdesc, "looks as healthy as can be.");
		msg_print(cdesc);

		/* can't be afraid at max hp's */
		if (m_ptr->monfear > 0) {
		    m_ptr->monfear = 0;
		    monster_is_afraid = (-1);
		}
	    }
	    else {

		m_ptr->hp += (c_list[m_ptr->r_idx].level) * 6;
		if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

		if (m_ptr->hp == m_ptr->maxhp) {
		    strcat(cdesc, "looks REALLY healthy!");

		    /* can't be afraid at max hp's */
		    if (m_ptr->monfear > 0) {
			m_ptr->monfear = 0;
			monster_is_afraid = (-1);
		    }
		}
		else {
		    strcat(cdesc, "looks healthier.");
		    if ((m_ptr->monfear > 0) && (m_ptr->maxhp / (m_ptr->hp + 1) < 3)) {
			m_ptr->monfear = 0;	/* has recovered 33% of it's
						 * hit points */
			monster_is_afraid = (-1);
		    }
		}
		msg_print(cdesc);

		/* no longer afraid -CWS */
		if (monster_is_afraid == -1) {
		    vtype               m_name, out_val;

		    monster_name(m_name, m_ptr, &c_list[m_ptr->r_idx]);
		    sprintf(out_val, "%s recovers %s courage.", m_name,
			    (sex == 'm' ? "his" : sex == 'f' ? "her" :
			     sex == 'p' ? "their" : "its"));
		    msg_print(out_val);
		}
	    }
	    break;

	  case 47:
	    if (!blind) {
		sprintf(outval, "%scasts a spell.", cdesc);
	    }
	    else {
		sprintf(outval, "%smumbles to %sself.", cdesc,
			      (sex == 'm' ? "him" : sex == 'f' ? "her" :
			       sex == 'p' ? "them" : "it"));
	    }
	    msg_print(outval);

	    if ((m_ptr->cspeed) <= ((int)(c_list[m_ptr->r_idx].speed) - 10)) {
		if ((c_list[m_ptr->r_idx].speed) <= 15) {
		strcat(cdesc, "starts moving faster.");
		msg_print(cdesc);
		m_ptr->cspeed += 1;
	    }
	    }

	    break;

	  case 48:
	    if (!blind) strcat(cdesc, "fires missiles at you.");
	    else strcat(cdesc, "sounds like it threw something.");
	    msg_print(cdesc);
	    bolt(GF_ARROW, char_row, char_col, damroll(6, 7), ddesc, m_ptr, m_idx);
	    break;

	  case 49:
	    if (!blind) strcat(cdesc, "casts a Plasma Bolt.");
	    else strcat(cdesc, "mumbles, and you are hit with a hellish blast.");
	    msg_print(cdesc);
	    bolt(GF_PLASMA, char_row, char_col,
		 10 + damroll(8, 7) + (c_list[m_ptr->r_idx].level),
		 ddesc, m_ptr, m_idx);
	    break;

	  case 50:
	    if (!blind) strcat(cdesc, "magically summons monsters!");
	    else strcat(cdesc, "mumbles, and you hear many things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    for (k = 0; k < 8; k++) {
		hack_m_idx = m_idx;
		summon_monster(&y, &x, FALSE);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 51:
	    if (!blind) strcat(cdesc, "casts a Nether Bolt.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    bolt(GF_NETHER, char_row, char_col,
		 30 + damroll(5, 5) + (c_list[m_ptr->r_idx].level * 3) / 2,
		 ddesc, m_ptr, m_idx);
	    break;

	  case 52:
	    if (!blind) strcat(cdesc, "casts an Ice Bolt.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    bolt(GF_COLD, char_row, char_col,
		 damroll(6, 6) + (c_list[m_ptr->r_idx].level)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 53:
	    if (!blind) strcat(cdesc, "gestures in shadow.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    (void)unlite_area(char_row, char_col);
	    break;

	  case 54:
	    strcat(cdesc, "tries to blank your mind.");
	    msg_print(cdesc);
	    if (player_saves() || rand_int(2)) {
		msg_print("You resist the spell.");
	    }
	    else if (lose_all_info()) {
		msg_print("Your memories fade away.");
	    }
	    break;

	  case 55:
	    if (!blind) {
		sprintf(outval, "%sconcentrates and %s eyes glow red.",
			      cdesc,
			      (sex == 'm' ? "his" : sex == 'f' ? "her" :
			       sex == 'p' ? "their" : "its"));
		msg_print(outval);
	    }
	    else {
		msg_print("You feel something focusing on your mind.");
	    }
	    if (player_saves()) {
		if (!blind) msg_print("You avert your gaze!");
		else msg_print("You resist the effects.");
	    }
	    else {
		msg_print("Your mind is blasted by psionic energy.");
		take_hit(damroll(12, 15), ddesc);
		if ((!py.flags.resist_conf) && (!py.flags.resist_chaos)) {
		    if (py.flags.confused > 0) {
			py.flags.confused += 2;
		    }
		    else {
			py.flags.confused = randint(5) + 3;
		    }
		}
		if (!py.flags.free_act) {
		    if (py.flags.paralysis > 0) {
			py.flags.paralysis += 2;
		    }
		    else {
			py.flags.paralysis = randint(5) + 4;
		    }
		    if (py.flags.slow > 0) {
			py.flags.slow += 2;
		    }
		    else {
			py.flags.slow = randint(5) + 3;
		    }
		}
		if (!py.flags.resist_blind) {
		    if (py.flags.blind > 0) {
			py.flags.blind += 6;
		    }
		    else {
			py.flags.blind += 12 + randint(3);
		    }
		}
	    }
	    break;

	  case 56:
	    if (!blind) strcat(cdesc, "casts a Stinking Cloud.");
	    else strcat(cdesc, "mumbles, and you smell a foul odor.");
	    msg_print(cdesc);
	    breath(GF_POIS, char_row, char_col,
		   damroll(12, 2), ddesc, m_idx);
	    break;

	  case 57:
	    if (!blind) strcat(cdesc, "gestures at you.");
	    else strcat(cdesc, "mumbles strangely.");
	    msg_print(cdesc);
	    if ((player_saves()) ||
	        (rand_int(3)) ||
	        (py.flags.resist_nexus)) {
		msg_print("You keep your feet firmly on the ground.");
	    }
	    else {
		k = dun_level;
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
		else if (k < dun_level) {
		    msg_print("You sink through the floor.");
		}
		else {
		    msg_print("You rise up through the ceiling.");
		}
		new_level_flag = TRUE;
	    }
	    break;

	  case 58:
	    f_ptr = (&py.flags);
	    if (!blind) strcat(cdesc, "casts a Water Bolt.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    bolt(GF_WATER, char_row, char_col,
		 damroll(10, 10) + (c_list[m_ptr->r_idx].level)
		 ,ddesc, m_ptr, m_idx);
	    break;

	  case 59:
	    f_ptr = &py.flags;
	    if (!blind) strcat(cdesc, "gestures fluidly.");
	    else strcat(cdesc, "mumbles.");
	    msg_print(cdesc);
	    msg_print("You are engulfed in a whirlpool.");
	    breath(GF_WATER, char_row, char_col,
		   randint((c_list[m_ptr->r_idx].level * 5) / 2) + 50,
		   ddesc, m_idx);
	    break;

	  case 60:
	    if (!blind) strcat(cdesc, "casts a Nether Ball.");
	    else strcat(cdesc, "mumbles, and you feel an unholy aura.");
	    msg_print(cdesc);
	    breath(GF_NETHER, char_row, char_col,
		   (50 + damroll(10, 10) + (c_list[m_ptr->r_idx].level)),
		   ddesc, m_idx);
	    break;

	  case 61:
	    if (!blind) strcat(cdesc, "summons an Angel.");
	    else strcat(cdesc, "mumbles, and you hear something appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    hack_m_idx = m_idx;
	    summon_angel(&y, &x);
	    hack_m_idx = (-1);
	    update_mon((int)cave[y][x].cptr);
	    break;

	  case 62:
	    if (!blind) strcat(cdesc, "magically summons Spiders.");
	    else strcat(cdesc, "mumbles, and you hear many things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    for (k = 0; k < 6; k++) {
		hack_m_idx = m_idx;
		summon_spider(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 63:
	    if (!blind) strcat(cdesc, "magically summons Hounds.");
	    else strcat(cdesc, "mumbles, and you hear many things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called,it needs m_idx */
	    for (k = 0; k < 8; k++) {
		hack_m_idx = m_idx;
		summon_hound(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 64:
	    if (!blind) strcat(cdesc, "breathes Nexus.");
	    else strcat(cdesc, "breathes, and you feel strange.");
	    msg_print(cdesc);
	    breath(GF_NEXUS, char_row, char_col,
		((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3)), ddesc, m_idx);
	    break;

	  case 65:
	    if (!blind) strcat(cdesc, "breathes elemental force.");
	    else strcat(cdesc, "breathes, and you are hit hard.");
	    msg_print(cdesc);

	    /* Breath "walls", at PLAYER location */
	    if (!rand_int(10)) {
		br_wall(m_ptr->fy, m_ptr->fx);
	    }

	    /* Normal breath */
	    else {
		breath(GF_FORCE, char_row, char_col,
		       ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6)),
		       ddesc, m_idx);
	    }
	    break;

	  case 66:
	    if (!blind) strcat(cdesc, "breathes inertia.");
	    else strcat(cdesc, "breathes.");
	    msg_print(cdesc);
	    breath(GF_INERTIA, char_row, char_col,
		   ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 67:
	    if (!blind) strcat(cdesc, "breathes light.");
	    else strcat(cdesc, "breathes.");
	    msg_print(cdesc);
	    breath(GF_LITE, char_row, char_col,
		((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 68:
	    if (!blind) strcat(cdesc, "breathes time.");
	    else strcat(cdesc, "breathes.");
	    msg_print(cdesc);
	    breath(GF_TIME, char_row, char_col,
		((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3)), ddesc, m_idx);
	    break;

	  case 69:		   /* gravity */
	    if (!blind) strcat(cdesc, "breathes gravity.");
	    else strcat(cdesc, "breathes, and you feel heavy.");
	    msg_print(cdesc);
	    breath(GF_GRAVITY, char_row, char_col,
		((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3)), ddesc, m_idx);
	    break;

	  case 70:		   /* darkness */
	    if (!blind) strcat(cdesc, "breathes darkness.");
	    else strcat(cdesc, "breathes.");
	    msg_print(cdesc);
	    breath(GF_DARK, char_row, char_col,
		((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 71:		   /* plasma */
	    if (!blind) strcat(cdesc, "breathes plasma.");
	    else strcat(cdesc, "breathes.");
	    msg_print(cdesc);
	    breath(GF_PLASMA, char_row, char_col,
		((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 72:
	    if (!blind) {
		strcat(cdesc, "fires an arrow at you.");
		msg_print(cdesc);
	    }
	    else {
		msg_print("You hear the 'twang' of a bowstring.");
	    }
	    bolt(GF_ARROW, char_row, char_col, damroll(1, 6), ddesc, m_ptr, m_idx);
	    break;

	  case 73:
	    if (!blind) strcat(cdesc, "magically summons mighty undead opponents.");
	    else strcat(cdesc, "mumbles, and you hear many creepy things appear.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 10; k++) {
		hack_m_idx = m_idx;
		summon_wraith(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    for (k = 0; k < 7; k++) {
		hack_m_idx = m_idx;
		summon_gundead(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 74:		   /* Big darkness storm */
	    if (!blind) strcat(cdesc, "casts a Darkness Storm.");
	    else strcat(cdesc, "mumbles powerfully.");
	    msg_print(cdesc);
	    breath(GF_DARK, char_row, char_col,
		((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6)), ddesc, m_idx);
	    break;

	  case 75:		   /* Mana storm */
	    if (!blind) strcat(cdesc, "invokes a Mana Storm.");
	    else strcat(cdesc, "mumbles, and you are hit by a storm of power.");
	    msg_print(cdesc);
	    breath(GF_MANA, char_row, char_col,
		   (c_list[m_ptr->r_idx].level * 5) + damroll(10, 10),
		   ddesc, m_idx);
	    break;

	  case 76:		   /* Summon reptiles */
	    if (!blind) strcat(cdesc, "magically summons reptiles.");
	    else strcat(cdesc, "mumbles, and you hear many things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 8; k++) {
		hack_m_idx = m_idx;
		summon_reptile(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 77:		   /* Summon ants */
	    if (!blind) strcat(cdesc, "magically summons ants.");
	    else strcat(cdesc, "mumbles, and you hear many things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 7; k++) {
		hack_m_idx = m_idx;
		summon_ant(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 78:		   /* Summon unique monsters */
	    if (!blind) strcat(cdesc, "summons special opponents!");
	    else strcat(cdesc, "mumbles, and you're worried by the things you hear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 5; k++) {
		hack_m_idx = m_idx;
		summon_unique(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    for (k = 0; k < 4; k++) {
		hack_m_idx = m_idx;
		summon_jabberwock(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 79:		   /* Summon greater undead */
	    if (!blind) strcat(cdesc, "summons the DEAD!");
	    else strcat(cdesc, "mumbles, and a chill runs down your spine.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 8; k++) {
		hack_m_idx = m_idx;
		summon_gundead(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  case 80:		   /* Summon ancient dragons */
	    if (!blind) strcat(cdesc, "summons ancient dragons.");
	    else strcat(cdesc, "mumbles, and you hear many huge things appear nearby.");
	    msg_print(cdesc);
	    y = char_row;
	    x = char_col;
	/* in case compact_monster() is called, it needs m_idx */
	    for (k = 0; k < 5; k++) {
		hack_m_idx = m_idx;
		summon_ancientd(&y, &x);
		hack_m_idx = (-1);
		update_mon((int)cave[y][x].cptr);
	    }
	    break;

	  default:
	    if (k != 200) strcat(cdesc, "casts a bugged spell.");
	    else strcat(cdesc, "had no spell to cast, tell someone NOW!");
	    msg_print(cdesc);
	}


	/* Remember what the monster did to us */
	/* If we can see him, or he cast "teleport away" or "teleport level" */
	if ((m_ptr->ml)	|| (thrown_spell == 45) || (thrown_spell == 57)) {

	    if (thrown_spell < 33) {
		c_recall[m_ptr->r_idx].r_spells |= 1L << (thrown_spell - 1);
	    }
	    else if (thrown_spell < 65) {
		c_recall[m_ptr->r_idx].r_spells2 |= 1L << (thrown_spell - 33);
	    }
	    else if (thrown_spell < 97) {
		c_recall[m_ptr->r_idx].r_spells3 |= 1L << (thrown_spell - 65);
	    }

	    if ((c_recall[m_ptr->r_idx].r_spells & CS_FREQ) != CS_FREQ)
		c_recall[m_ptr->r_idx].r_spells++;

	/* Take note of monsters that kill you */
	    if (death && c_recall[m_ptr->r_idx].r_deaths < MAX_SHORT)
		c_recall[m_ptr->r_idx].r_deaths++;
	}
    }
}


/*
 * Places creature adjacent to given location -RAK-
 * Rats and Flys are fun!
 */
int multiply_monster(int y, int x, int cr_index, int m_idx)
{
    register int        i, j, k;
    register cave_type *c_ptr;

    int result = FALSE;

#ifdef ATARIST_MWC
    u32b              holder;
#endif

    /* Try up to 18 times */
    i = 0; do {

	/* Pick a location near the given one */
	j = rand_spread(y, 1);
	k = rand_spread(x, 1);

    /*
     * don't create a new creature on top of the old one, that causes
     * invincible/invisible creatures to appear 
     */
	if (in_bounds(j, k) && (j != y || k != x)) {
	    c_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (c_ptr->i_idx == 0) &&
		(c_ptr->cptr != 1)) {
		if (c_ptr->cptr > 1) {	/* Creature there already?	 */
		/* Some critters are cannibalistic!	    */
		    if ((c_list[cr_index].cmove & CM_EATS_OTHER)
		/* Check the experience level -CJS- */
			&& c_list[cr_index].mexp >=
			c_list[m_list[c_ptr->cptr].r_idx].mexp) {
		    /* It ate an already processed monster.Handle normally. */
			if (m_idx < c_ptr->cptr)
			    delete_monster((int)c_ptr->cptr);
		    /*
		     * If it eats this monster, an already processed mosnter
		     * will take its place, causing all kinds of havoc. Delay
		     * the kill a bit. 
		     */
			else
			    fix1_delete_monster((int)c_ptr->cptr);

		    /* in case compact_monster() is called,it needs m_idx */
			hack_m_idx = m_idx;
			result = place_monster(j, k, cr_index, FALSE);
			hack_m_idx = (-1);
			if (!result)
			    return FALSE;
			mon_tot_mult++;
			return check_mon_lite(j, k);
		    }
		} else
		/* All clear,  place a monster	  */
		{
		/* in case compact_monster() is called,it needs m_idx */
		    hack_m_idx = m_idx;
		    result = place_monster(j, k, cr_index, FALSE);
		    hack_m_idx = (-1);
		    if (!result)
			return FALSE;
		    mon_tot_mult++;
		    return check_mon_lite(j, k);
		}
	    }
	}
	i++;
    }
    while (i <= 18);


    /* Nobody got made */
    return FALSE;
}


/*
 * Move a critter about the dungeon			-RAK-
 */
static void mon_move(int m_idx, u32b *rcmove)
{
    int			i, j, k, move_test, dir;
    monster_race	*r_ptr;
    monster_type	*m_ptr;
    int			mm[9];
    bigvtype               out_val, m_name;

    /* Get the monster and its race */
    m_ptr = &m_list[m_idx];
    r_ptr = &c_list[m_ptr->r_idx];


    /* reduce fear, tough monsters can unfear faster -CFT, hacked by DGK */
    if (m_ptr->monfear) {
	int t = (int)m_ptr->monfear;    /* use int so avoid unsigned wraparound -CFT */
	t -= randint(c_list[m_ptr->r_idx].level / 10);
	if (t <= 0) {
	    t = 0;
	    if (m_ptr->ml && los(char_row, char_col, m_ptr->fy, m_ptr->fx)) {
		monster_name(m_name, m_ptr, r_ptr);
		sprintf(out_val, "%s recovers its courage.", m_name);
		msg_print(out_val);
	    }
	}
	m_ptr->monfear = (byte) t;
    }

    /* Does the critter multiply? */
    if ((r_ptr->cmove & CM_MULTIPLY) &&
	(MAX_MON_MULT >= mon_tot_mult) &&
	(((py.flags.rest != -1) && ((py.flags.rest % MON_MULT_ADJ) == 0)) ||
	 ((py.flags.rest == -1) && (randint(MON_MULT_ADJ) == 1)))) {

	/* Count the adjacent monsters */
	for (k = 0, i = (int)m_ptr->fy - 1; i <= (int)m_ptr->fy + 1; i++) {
	    for (j = (int)m_ptr->fx - 1; j <= (int)m_ptr->fx + 1; j++) {
		if (in_bounds(i, j) && (cave[i][j].cptr > 1)) k++;
	    }
	}

    /* can't call randint with a value of zero, increment counter to allow
     * creature multiplication 
     */
	if (k == 0) k++;

	/* Hack -- multiply slower in crowded rooms */
	if ((k < 4) && (randint(k * MON_MULT_ADJ) == 1))

	    if (multiply_monster((int)m_ptr->fy, (int)m_ptr->fx, (int)m_ptr->r_idx, m_idx))

		*rcmove |= CM_MULTIPLY;
    }
    move_test = FALSE;

    /* Hack -- if in wall, must immediately escape to a clear area */
    if (!(r_ptr->cmove & CM_PHASE) &&
	(cave[m_ptr->fy][m_ptr->fx].fval >= MIN_WALL)) {

    /* If the monster is already dead, don't kill it again! This can happen
     * for monsters moving faster than the player.  They will get multiple
     * moves, but should not if they die on the first move.  This is only a
     * problem for monsters stuck in rock.  
     */
	if (m_ptr->hp < 0) return;

	k = 0;
	dir = 1;
    /* note direction of for loops matches direction of keypad from 1 to 9 */
    /* do not allow attack against the player */
	for (i = m_ptr->fy + 1; i >= (int)(m_ptr->fy - 1); i--)
	    for (j = m_ptr->fx - 1; j <= (int)(m_ptr->fx + 1); j++) {

		/* No staying still */
		if ((dir != 5) && 

		/* Require floor space */
		floor_grid_bold(i, j) &&

		/* Do not allow attack against the player */
		(cave[i][j].cptr != 1)) mm[k++] = dir;
		dir++;
	    }

	/* Attempt to "escape" */
	if (k) {

	    /* Pick a random direction to prefer */
	    dir = rand_int(k);

	    /* Prefer that direction */
	    i = mm[0];
	    mm[0] = mm[dir];
	    mm[dir] = i;

	    /* Move the monster */
	    make_move(m_idx, mm, rcmove);
	/* this can only fail if mm[0] has a rune of protection */
	}


	/* Hack -- if still in a wall, apply more damage, and dig out */
	if (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_WALL) {
/* in case the monster dies, may need to call fix1_delete_monster()
 * instead of delete_monsters() 
 */

	    /* XXX XXX XXX XXX The player may not have caused the rocks */

	    /* Apply damage, check for death */
	    hack_m_idx = m_idx;
	    i = mon_take_hit(m_idx, damroll(8, 8), FALSE);
	    hack_m_idx = (-1);
	    if (i >= 0) {
		msg_print("You hear a scream muffled by rock!");
	    }
	    else {
		(void)twall((int)m_ptr->fy, (int)m_ptr->fx, 1, 0);
	    }
	}


	/* monster movement finished */
	return;
    }


    /* Creature is confused?  Chance it becomes un-confused  */
    else if (m_ptr->confused) {
	mm[0] = randint(9);
	mm[1] = randint(9);
	mm[2] = randint(9);
	mm[3] = randint(9);
	mm[4] = randint(9);
    /* don't move him if he is not supposed to move! */
	if (!(r_ptr->cmove & CM_ATTACK_ONLY)) {
	    *rcmove |= CM_ATTACK_ONLY;
	    make_move(m_idx, mm, rcmove);
	}

    /* reduce conf, tough monsters can unconf faster -CFT */
	{			/* use int so avoid unsigned wraparound -CFT */
	    int t = (int)m_ptr->confused;

	    t -= randint(c_list[m_ptr->r_idx].level / 10);
	    if (t < 0)
		t = 0;
	    m_ptr->confused = (byte) t;
	}
	move_test = TRUE;
    }

    /* Creature may cast a spell */
    else if (r_ptr->spells != 0) {
	mon_cast_spell(m_idx, &move_test);
    }

    if (!move_test) {
	/* 75% random movement */
	if ((r_ptr->cmove & CM_75_RANDOM) && (randint(100) < 75)) {
	    mm[0] = randint(9);
	    mm[1] = randint(9);
	    mm[2] = randint(9);
	    mm[3] = randint(9);
	    mm[4] = randint(9);
	    *rcmove |= CM_75_RANDOM;
	    make_move(m_idx, mm, rcmove);
	}

	/* 40% random movement */
	else if ((r_ptr->cmove & CM_40_RANDOM) && (randint(100) < 40)) {
	    mm[0] = randint(9);
	    mm[1] = randint(9);
	    mm[2] = randint(9);
	    mm[3] = randint(9);
	    mm[4] = randint(9);
	    *rcmove |= CM_40_RANDOM;
	    make_move(m_idx, mm, rcmove);
	}

	/* 20% random movement */
	else if ((r_ptr->cmove & CM_20_RANDOM) && (randint(100) < 20)) {
	    mm[0] = randint(9);
	    mm[1] = randint(9);
	    mm[2] = randint(9);
	    mm[3] = randint(9);
	    mm[4] = randint(9);
	    *rcmove |= CM_20_RANDOM;
	    make_move(m_idx, mm, rcmove);
	}

	/* Normal movement */
	else if (r_ptr->cmove & CM_MOVE_NORMAL) {
	    if (randint(200) == 1) {
		mm[0] = randint(9);
		mm[1] = randint(9);
		mm[2] = randint(9);
		mm[3] = randint(9);
		mm[4] = randint(9);
	    } else
		get_moves(m_idx, mm);
	    *rcmove |= CM_MOVE_NORMAL;
	    make_move(m_idx, mm, rcmove);
	}

	/* Attack, but don't move */
	else if ((r_ptr->cmove & CM_ATTACK_ONLY) && (m_ptr->cdis < 2)) {
	    *rcmove |= CM_ATTACK_ONLY;
	    get_moves(m_idx, mm);
	    make_move(m_idx, mm, rcmove);
	} else if ((r_ptr->cmove & CM_ALL_MV_FLAGS) == 0 &&
		   (m_ptr->cdis < 2)) {

	    /* little hack for Quylthulgs, so that will eventually notice
	     * that they have no physical attacks */
	    if (c_recall[m_ptr->r_idx].r_attacks[0] < MAX_UCHAR)
		c_recall[m_ptr->r_idx].r_attacks[0]++;
	}
    }
}




/*
 * Creatures movement and attacking are done from here
 */
void creatures(int attack)
{
    int			i, k;

    monster_type	*m_ptr;
    monster_lore	*r_ptr;
    
    u32b                notice, rcmove;
    int                   wake, ignore;
    vtype                 cdesc;

    /* Process the monsters (backwards) */
    for (i = m_max - 1; i >= MIN_M_IDX && !death; i--) {

	/* Get the i'th monster */
	m_ptr = &m_list[i];


    /* Get rid of an eaten/breathed on monster.  Note: Be sure not to process
     * this monster. This is necessary because we can't delete monsters while
     * scanning the m_list here. 
     */
	/* Hack -- Remove dead monsters. */
	if (m_ptr->hp < 0) {

	    if (c_list[m_ptr->r_idx].cdefense & UNIQUE) u_list[m_ptr->mptr].exist = 0;
	    fix2_delete_monster(i);

	    /* Continue */
	    continue;
	}

	m_ptr->cdis = distance(char_row, char_col,
			       (int)m_ptr->fy, (int)m_ptr->fx);

	if (attack) {		   /* Attack is argument passed to CREATURE */
	    k = movement_rate(i);
	    if (k <= 0)
		update_mon(i);
	    else
		while (k > 0) {
		    k--;
		    wake = FALSE;
		    ignore = FALSE;
		    rcmove = 0;
		    if ((m_ptr->ml && /* check los so telepathy won't wake lice -CFT */
			 los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) ||
			(m_ptr->cdis <= c_list[m_ptr->r_idx].aaf)

			/* Monsters trapped in rock must be given a turn also,
			 * so that they will die/dig out immediately.  */
#ifdef ATARIST_MWC
		    || ((!(c_list[m_ptr->r_idx].cmove & (holder = CM_PHASE)))
#else
			|| ((!(c_list[m_ptr->r_idx].cmove & CM_PHASE))
#endif
		     && cave[m_ptr->fy][m_ptr->fx].fval >= MIN_WALL)) {

	    /* Handle "sleep" */
			if (m_ptr->csleep > 0)

			    if (py.flags.aggravate)
				m_ptr->csleep = 0;

			    else if ((py.flags.rest == 0 && py.flags.paralysis < 1) ||
		    !(rand_int(50))) {

				notice = rand_int(1024);
			
		    /* XXX See if monster "notices" player */
		    if ((notice * notice * notice) <=
			(1L << (29 - py.misc.stl))) {

			/* Hack -- amount of "waking" */
			m_ptr->csleep -= (100 / m_ptr->cdis);

			/* Still asleep */
			if (m_ptr->csleep > 0)

					ignore = TRUE;
				    else {
					wake = TRUE;
				    /* force it to be exactly zero */
					m_ptr->csleep = 0;
				    }
				}
			    }

			if (m_ptr->stunned != 0) {
/* NOTE: Balrog = 100*100 = 10000, it always recovers instantly */
			    if (randint(5000) < c_list[m_ptr->r_idx].level
				* c_list[m_ptr->r_idx].level)
				m_ptr->stunned = 0;
			    else
				m_ptr->stunned--;
			    if (m_ptr->stunned == 0) {
				if (!m_ptr->ml)
				    (void)strcpy(cdesc, "It ");
				else if (c_list[m_ptr->r_idx].cdefense & UNIQUE)
				    (void)sprintf(cdesc, "%s ",
						  c_list[m_ptr->r_idx].name);
				else
				    (void)sprintf(cdesc, "The %s ",
						  c_list[m_ptr->r_idx].name);
				msg_print(strcat(cdesc,
					    "recovers and glares at you."));
			    }
			}
			if ((m_ptr->csleep == 0) && (m_ptr->stunned == 0))
			    mon_move(i, &rcmove);
		    }
		    update_mon(i);
		    if (m_ptr->ml) {
			r_ptr = &c_recall[m_ptr->r_idx];
			if (wake) {
			    if (r_ptr->r_wake < MAX_UCHAR)
				r_ptr->r_wake++;
			} else if (ignore) {
			    if (r_ptr->r_ignore < MAX_UCHAR)
				r_ptr->r_ignore++;
			}
			r_ptr->r_cmove |= rcmove;
		    }
		}
	} else
	    update_mon(i);

    /* Get rid of an eaten/breathed on monster.  This is necessary because we
     * can't delete monsters while scanning the m_list here.  This monster
     * may have been killed during mon_move(). 
     */
	if (m_ptr->hp < 0) {
	    if (c_list[m_ptr->r_idx].cdefense & UNIQUE) u_list[m_ptr->mptr].exist = 0;
	    fix2_delete_monster(i);
	    continue;
	}
    }
/* End processing monsters	   */
}


/*
 * This is a fun one.  In a given block, pick some walls and
 * turn them into open spots.  Pick some open spots and turn
 * them into walls.  An "Earthquake" effect.	       -LVB-
 */
static void shatter_quake(int mon_y, int mon_x)
{
    register int           i, j, k, l;
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    int                    kill, damage = 0, tmp, y, x = 0;
    vtype                  out_val, m_name;
    int                    m_idx = cave[mon_y][mon_x].cptr;
    /* needed when we kill another monster */

    for (i = mon_y - 8; i <= mon_y + 8; i++)
	for (j = mon_x - 8; j <= mon_x + 8; j++)
	    if (in_bounds(i, j) && (randint(8) == 1)) {
		if ((i == mon_y) && (j == mon_x))
		    continue;
		c_ptr = &cave[i][j];
		if (c_ptr->cptr > 1) {
		    m_ptr = &m_list[c_ptr->cptr];
		    r_ptr = &c_list[m_ptr->r_idx];

		    if (!(r_ptr->cmove & CM_PHASE) &&
			!(r_ptr->cdefense & BREAK_WALL)) {
			if ((movement_rate(c_ptr->cptr) == 0) ||
			    (r_ptr->cmove & CM_ATTACK_ONLY))
			/* monster can not move to escape the wall */
			    kill = TRUE;
			else {
/* only kill if there is nowhere for the monster to escape to */
			    kill = TRUE;
			    for (y = i - 1; y <= i + 1; y++) {
				for (x = j - 1; x <= j + 1; x++) {
				    if (floor_grid_bold(y, x) &&
					!(y == i && x == j)) {
					kill = FALSE;
					break;
				    }
				}
				if (!kill)
				    break;
			    }
			}
			if (kill)
			    damage = 0x7fff;	/* this will kill everything */
			else
			    damage = damroll(4, 8);
			monster_name(m_name, m_ptr, r_ptr);
			(void)sprintf(out_val, "%s wails out in pain!", m_name);
			msg_print(out_val);
		    /* kill monster "by hand", so player doesn't get exp -CFT */
			m_ptr->hp = m_ptr->hp - damage;
			m_ptr->csleep = 0;

/* prevent unique monster from death by other monsters.  It causes trouble
 * (monster not marked as dead, quest monsters don't satisfy quest, etc).
 * So, we let then live, but extremely wimpy.  This isn't great, because
 * monster might heal itself before player's next swing... -CFT
 */
			if ((r_ptr->cdefense & UNIQUE) && (m_ptr->hp < 0))
			    m_ptr->hp = 0;
			if (m_ptr->hp < 0) {
			    u32b              temp, treas;

			    (void)sprintf(out_val, "%s is embedded in the rock.",
					  m_name);
			    msg_print(out_val);
			    object_level = (dun_level + r_ptr->level) >> 1;
			    treas = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
						  r_ptr->cmove, 0, 0);
			    if (m_ptr->ml) {
				temp = (c_recall[m_ptr->r_idx].r_cmove & CM_TREASURE)
				    >> CM_TR_SHIFT;
				if (temp > ((treas & CM_TREASURE) >> CM_TR_SHIFT))
				    treas = (treas & ~CM_TREASURE) | (temp << CM_TR_SHIFT);
				c_recall[m_ptr->r_idx].r_cmove = treas |
				    (c_recall[m_ptr->r_idx].r_cmove & ~CM_TREASURE);
			    }
			    if (m_idx < c_ptr->cptr)
				delete_monster((int)c_ptr->cptr);
			    else
				fix1_delete_monster((int)c_ptr->cptr);
			} /* if monster's hp < 0 */
		    }
		} else if (c_ptr->cptr == 1) {	/* Kill the dumb player! */
		    kill = TRUE;
		    for (y = i - 1; y <= i + 1; y++) {
			for (x = j - 1; x <= j + 1; x++) {
			    if (floor_grid_bold(y, x) &&
			    (cave[y][x].cptr == 0) && !(y == i && x == j)) {
				kill = FALSE;
				break;
			    }
			}
			if (!kill)
			    break;
		    }

		    switch (randint(3)) {
		      case 1:
			msg_print("The cave ceiling collapses!");
			break;
		      case 2:
			msg_print("The floor turns and crushes you!");
			break;
		      case 3:
			msg_print("You are pummeled with debris!");
			break;
		    }
		    if (kill) {
	msg_print("You are trapped and cannot move!  You are crushed beneath rock!");
	msg_print(NULL);
			damage = 320;
		    } else {
			switch (randint(3)) {
			  case 1:
			    msg_print("The rubble bashes you!");
			    damage = damroll(10, 4);
			    stun_player(randint(50));
			    break;
			  case 2:
			    msg_print("But you nimbly dodge the blast!");
			    damage = 0;
			    break;
			  case 3:
			    msg_print("The floor crushes you against the ceiling!");
			    damage = damroll(10, 4);
			    stun_player(randint(50));
			    break;
			}
			move_rec(char_row, char_col, y, x);
			for (k = char_row - 1; k <= char_row + 1; k++)
			    for (l = char_col - 1; l <= char_col + 1; l++) {
				c_ptr = &cave[k][l];
				c_ptr->tl = FALSE;
				lite_spot(k, l);
			    }
			lite_spot(char_row, char_col);
			char_row = y;
			char_col = x;
			check_view();
		    /* light creatures */
			creatures(FALSE);
		    }
		    take_hit(damage, "an Earthquake");
		}
		if (c_ptr->i_idx != 0)
		    if (((i_list[c_ptr->i_idx].tval >= TV_MIN_WEAR) &&
			 (i_list[c_ptr->i_idx].tval <= TV_MAX_WEAR) &&
			 (i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT)) ||
			(i_list[c_ptr->i_idx].tval == TV_UP_STAIR) ||
			(i_list[c_ptr->i_idx].tval == TV_DOWN_STAIR) ||
			(i_list[c_ptr->i_idx].tval == TV_STORE_DOOR))
			continue;  /* don't kill artifacts... */
		    else
			(void)delete_object(i, j);

		if ((c_ptr->fval >= MIN_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
		    c_ptr->fval = CORR_FLOOR;
		    c_ptr->pl = FALSE;
		    c_ptr->fm = FALSE;
		} else if ((c_ptr->fval <= MAX_CAVE_FLOOR) && (c_ptr->i_idx == 0)
			   && (c_ptr->cptr != 1)) {
		    /* don't bury player, it made him unattackable -CFT */
		    tmp = randint(10);
		    if (tmp < 6)
			c_ptr->fval = QUARTZ_WALL;
		    else if (tmp < 9)
			c_ptr->fval = MAGMA_WALL;
		    else
			c_ptr->fval = GRANITE_WALL;

		    c_ptr->fm = FALSE;
		}
		lite_spot(i, j);
	    }
}

/*
 * This is a fun one.  In a given block, pick some walls and
 * turn them into open spots.  Pick some open spots and turn
 * them into walls.  An "Earthquake" effect.	       -LVB- 
 */
static void br_wall(int mon_y, int mon_x)
{
    register int        k, l;
    register cave_type *c_ptr;
    int                 kill, damage = 0, tmp, y, x = 0;

    kill = TRUE;
    for (y = char_row - 1; y <= char_row + 1; y++) {
	for (x = char_col - 1; x <= char_col + 1; x++) {
	    if (floor_grid_bold(y,x) &&
	      (cave[y][x].cptr == 0) && !(y == char_row && x == char_col)) {
		kill = FALSE;
		break;
	    }
	}
	if (!kill)
	    break;
    }

    switch (randint(3)) {
      case 1:
	msg_print("The cave ceiling collapses!");
	break;
      case 2:
	msg_print("The floor turns and crushes you!");
	break;
      case 3:
	msg_print("You are pummeled with debris!");
	break;
    }
    if (kill) {
	msg_print("You are trapped, crushed and cannot move!");
	damage = 250;
    } else {
	switch (randint(3)) {
	  case 1:
	    msg_print("The rubble bashes you!");
	    damage = damroll(10, 4);
	    stun_player(randint(50));
	    break;
	  case 2:
	    msg_print("But you nimbly dodge the blast!");
	    damage = 0;
	    break;
	  case 3:
	    msg_print("The floor crushes you against the ceiling!");
	    damage = damroll(10, 4);
	    stun_player(randint(50));
	    break;
	}
	c_ptr = &cave[char_row][char_col];
	move_rec(char_row, char_col, y, x);
    /* don't destroy floor if stairs, shop, or artifact... */
	if ((c_ptr->fval <= MAX_CAVE_FLOOR) &&
	((c_ptr->i_idx == 0) || ((i_list[c_ptr->tptr].tval != TV_UP_STAIR) &&
			      (i_list[c_ptr->i_idx].tval != TV_DOWN_STAIR) &&
			      (i_list[c_ptr->i_idx].tval != TV_STORE_DOOR) &&
			      !((i_list[c_ptr->i_idx].tval >= TV_MIN_WEAR) &&
				(i_list[c_ptr->i_idx].tval <= TV_MAX_WEAR) &&
			    (i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT))))) {
	    if (c_ptr->i_idx)
		delete_object(char_row, char_col);
	    tmp = randint(10);
	    if (tmp < 6)
		c_ptr->fval = QUARTZ_WALL;
	    else if (tmp < 9)
		c_ptr->fval = MAGMA_WALL;
	    else
		c_ptr->fval = GRANITE_WALL;

	    c_ptr->fm = FALSE;
	}
	for (k = char_row - 1; k <= char_row + 1; k++)
	    for (l = char_col - 1; l <= char_col + 1; l++) {
		c_ptr = &cave[k][l];
		c_ptr->tl = FALSE;
		lite_spot(k, l);
	    }
	lite_spot(char_row, char_col);
	char_row = y;
	char_col = x;
    } /* !kill */
    check_view();
/* light creatures */
    creatures(FALSE);
    lite_spot(char_row, char_col);
    take_hit(damage, "an Earthquake");
}
