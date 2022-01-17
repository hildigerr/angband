/* File: misc2.c */

/* Purpose: misc code for monsters */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

extern int peek;
extern int rating;

/*
 * Ludwig's Brainstorm
 */
static int test_place(int y, int x)
{
    /* Require legal grid */
    if (!in_bounds(y, x)) return (FALSE);
    
    /* Require "empty" floor grid */
    if (!empty_grid_bold(y, x)) return (FALSE);

    /* And do not use special walls */
    if (cave[y][x].fval == NULL_WALL) return (FALSE);

    /* Or the player himself */
    if ((y == char_row) && (x == char_col)) return (FALSE);

    /* Use it */
    return TRUE;
}



/*
 * Deletes a monster entry from the level		-RAK-	
 */
void delete_monster(int j)
{
    register monster_type *m_ptr;

    if (j < 2)
	return;			   /* trouble? abort! -CFT */
    m_ptr = &m_list[j];
    if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE) u_list[m_ptr->mptr].exist = 0;
    cave[m_ptr->fy][m_ptr->fx].m_idx = 0;
    if (m_ptr->ml)
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    if (j != m_max - 1) {
#ifdef TARGET
	/* This targetting code stolen from Morgul -CFT */
	/* Targetted monster dead or compacted.      CDW */
	if (j==target_mon)
	    target_mode = FALSE;

	/* Targetted monster moved to replace dead or compacted monster   CDW */
	if (target_mon==m_max-1)
	    target_mon = j;
#endif
	m_ptr = &m_list[m_max - 1];
	cave[m_ptr->fy][m_ptr->fx].m_idx = j;
	m_list[j] = m_list[m_max - 1];
    }
    m_max--;
    m_list[m_max] = blank_monster;
    if (mon_tot_mult > 0)
	mon_tot_mult--;
}


/*
 * The following two procedures implement the same function as delete
 * monster. However, they are used within creatures(), because deleting a
 * monster while scanning the m_list causes two problems, monsters might get
 * two turns, and m_ptr/monptr might be invalid after the delete_monster.
 * Hence the delete is done in two steps. 
 */
/*
 * fix1_delete_monster does everything delete_monster does except delete the
 * monster record and reduce m_max, this is called in breathe, and a couple
 * of places in creatures.c 
 */
void fix1_delete_monster(int j)
{
    register monster_type *m_ptr;

    if (j < 2)
	return;			   /* trouble? abort! -CFT */
#ifdef TARGET
    /* Targetted monster dead or compacted.      CDW */
    if (j==target_mon)
	target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon==m_max-1)
	target_mon = j;
#endif
    m_ptr = &m_list[j];
    if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE)
	if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE) u_list[m_ptr->mptr].exist = 0;
/* force the hp negative to ensure that the monster is dead, for example, if
 * the monster was just eaten by another, it will still have positive hit
 * points 
 */
    m_ptr->hp = (-1);
    cave[m_ptr->fy][m_ptr->fx].m_idx = 0;
    if (m_ptr->ml)
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    if (mon_tot_mult > 0)
	mon_tot_mult--;
}

/* fix2_delete_monster does everything in delete_monster that wasn't done by
 * fix1_monster_delete above, this is only called in creatures() 
 */
void fix2_delete_monster(int j)
{
    register monster_type *m_ptr;

    if (j < 2)
	return;			   /* trouble? abort! -CFT */
    
#ifdef TARGET
    /* Targetted monster dead or compacted. CDW */
    if (j==target_mon)
	target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon==m_max-1)
	target_mon = j; 
#endif

    m_ptr = &m_list[j];		   /* Fixed from a r_list ptr to a m_list ptr. -CFT */
    if (r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE) u_list[m_ptr->mptr].exist = 0;
    if (j != m_max - 1) {
	m_ptr = &m_list[m_max - 1];
	cave[m_ptr->fy][m_ptr->fx].m_idx = j;
	m_list[j] = m_list[m_max - 1];
    }
    m_list[m_max - 1] = blank_monster;
    m_max--;
}




/*
 * Link all free space in monster list together
 */
void wipe_m_list()
{
    register int i;

    for (i = 0; i < MAX_M_IDX; i++)
	if (m_list[i].r_idx) delete_monster(i);
    for (i = 0; i < MAX_M_IDX; i++) m_list[i] = blank_monster;

    /* delete_unique() Kludgey Fix ~Ludwig */
    for (i = 0; i < MAX_R_IDX; i++)
	if (r_list[i].cflags2 & MF2_UNIQUE)
	    u_list[i].exist = 0;

    /* XXX Should already be done */
    m_max = MIN_M_IDX;
}


/*
 * Compact monsters	-RAK-
 *
 * Return TRUE if any monsters were deleted, FALSE if could not delete any
 * monsters. 
 */
int compact_monsters(void)
{
    register int           i;
    int                    cur_dis, delete_any;
    monster_type	*m_ptr;
    monster_race	*r_ptr;

    msg_print("Compacting monsters...");

    /* Start 66 (that is, 72-6) units away */
    cur_dis = 66;
    delete_any = FALSE;

    /* Keep going until someone is deleted */
    while (!delete_any) {

	/* Check all the monsters */
	for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	    m_ptr = &m_list[i];
	    r_ptr = &r_list[m_ptr->r_idx];

	    if ((cur_dis < m_ptr->cdis) && (randint(3) == 1)) {

	    /* Don't compact Melkor! */
		if (r_ptr->cflags1 & CM_WIN) continue;

	    /* in case this is called from within creatures(), this is a
	     * horrible hack, the m_list/creatures() code needs to be
	     * rewritten 
	     */
		else if (hack_m_idx < i) {
		    delete_monster(i);
		    delete_any = TRUE;
		} else

		/* fix1_delete_monster() does not decrement m_max, so don't
		 * set delete_any if this was called 
		 */
		    fix1_delete_monster(i);
	    }
	}
	if (!delete_any) {
	    cur_dis -= 6;
	/* can't do anything else but abort, if can't delete any monsters */
	    if (cur_dis < 0)
		return FALSE;
	}
    }
    
    return TRUE;
}


/*
 * Returns a pointer to next free space			-RAK-
 */
int m_pop(void)
{
    if (m_max == MAX_M_IDX)
	if (!compact_monsters())
	    return (-1);
    return (m_max++);
}




/*
 * Places a monster at given location
 */
int place_monster(int y, int x, int r_idx, int slp)
{
    register int           cur_pos, j, ny, nx, count;
    register monster_type *m_ptr;
    register monster_race *r_ptr;
    char                   buf[100];

    /* Verify monster race */
    if ((r_idx < 0) || (r_idx >= MAX_R_IDX)) return FALSE;

    /* Verify location */
    if (!test_place(y, x)) return FALSE;

    /* Get the race */
    r_ptr = &r_list[r_idx];

    if (r_ptr->cflags2 & MF2_UNIQUE) {
	if (u_list[r_idx].exist) {
	    if (wizard) {
		(void)sprintf(buf, "Tried to create %s but exists.", r_ptr->name);
		msg_print(buf);
	    }
	    return FALSE;
	}
	if (u_list[r_idx].dead) {
	    if (wizard) {
		(void)sprintf(buf, "Tried to create %s but dead.", r_ptr->name);
		msg_print(buf);
	    }
	    return FALSE;
	}
	u_list[r_idx].exist = 1;
    }

    /* Get the next monster record */
    cur_pos = m_pop();		   /* from um55, paranoia error check... */

    /* Mega-Paranoia */
    if (cur_pos == -1) return FALSE;

    /* Note the monster */
    if ((wizard || peek) && (r_ptr->cflags2 & MF2_UNIQUE)) {
	msg_print(r_ptr->name);
    }
    
    /* Powerful monster */
    if (r_ptr->level > (unsigned)dun_level) {
	int                 c;

	/* Uniques get rating based on "out of depth" amount */
	rating += ((c = r_ptr->level - dun_level) > 30) ? 15 : c / 2;
	
	/* Normal monsters are worth "half" as much */
	if (r_ptr->cflags2 & MF2_UNIQUE) {
	    rating += (r_ptr->level - dun_level) / 2;
	}
    }

    /* Get a new monster record */
    m_ptr = &m_list[cur_pos];

    /* Place the monster at the location */
    m_ptr->fy = y;
    m_ptr->fx = x;

    /* Save the race */
    m_ptr->r_idx = r_idx;

    /* Assign maximal hitpoints */
    if ((r_ptr->cflags2 & MF2_MAX_HP) ) {
	m_ptr->hp = max_hp(r_ptr->hd);
    }
    else {
	m_ptr->hp = pdamroll(r_ptr->hd);
    }

    /* And start out fully healthy */
    m_ptr->maxhp = m_ptr->hp;

    /* Extract the monster base speed */
    m_ptr->mspeed = r_ptr->speed - 10;
    
    /* No "damage" yet */
    m_ptr->stunned = 0;
    m_ptr->confused = 0;
    m_ptr->monfear = 0;

    m_ptr->cdis = distance(char_row, char_col, y, x);

    /* Default to invisible */
    m_ptr->ml = FALSE;

    /* Update the cave */
    cave[y][x].m_idx = cur_pos;

    /* Update the monster sleep info */
    if (slp) {
	if (r_ptr->sleep == 0) {
	    m_ptr->csleep = 0;
	}
	else {
	    m_ptr->csleep = ((int)r_ptr->sleep * 2) +
			     randint((int)r_ptr->sleep * 10);
	}
    }

    /* to give the player a sporting chance, any monster that appears in */
    /* line-of-sight and can cast spells or breathe, should be asleep.   */
    /* This is an extension of Um55's sleeping dragon code...            */
 else
    if (((r_ptr->spells1 & (CAUSE_LIGHT|CAUSE_SERIOUS|HOLD_PERSON|
                                  BLINDNESS|CONFUSION|FEAR|SLOW|BREATH_L|
                                  BREATH_G|BREATH_A|BREATH_FR|BREATH_FI|
                                  FIRE_BOLT|FROST_BOLT|ACID_BOLT|MAG_MISS|
                                  CAUSE_CRIT|FIRE_BALL|FROST_BALL|MANA_BOLT))
          || (r_ptr->spells2 & (BREATH_CH|BREATH_SH|BREATH_SD|BREATH_CO|
                                  BREATH_DI|BREATH_LD|LIGHT_BOLT|LIGHT_BALL|
                                  ACID_BALL|TRAP_CREATE|RAZOR|MIND_BLAST|
                                  MISSILE|PLASMA_BOLT|NETHER_BOLT|ICE_BOLT|
                                  FORGET|BRAIN_SMASH|ST_CLOUD|TELE_LEV|
                                  WATER_BOLT|WATER_BALL|NETHER_BALL|BREATH_NE))
          || (r_ptr->spells3 & (BREATH_WA|BREATH_SL|BREATH_LT|BREATH_TI|
                                  BREATH_GR|BREATH_DA|BREATH_PL|ARROW|
                                  DARK_STORM|MANA_STORM)))
       && los(y, x, char_row, char_col)) {
      m_ptr->csleep = randint(4);   /* if asleep only to prevent
                                       * summon-breathe-breathe-breathe-die,
                                       * then don't sleep long -CFT */
    }

    /* Wake up... */
    else {
	m_ptr->csleep = 0;
    }

    update_mon(cur_pos);	   /* light up the monster if we can see it... -CFT */


    /* Unique kobolds, Liches, orcs, Ogres, Trolls, yeeks, and demons */
    /* get a "following" of escorts.  -DGK-    But not skeletons, */
    /* which include druj, which would make Cantoras amazingly tough -CFT */

    if (r_ptr->cflags2 & MF2_UNIQUE) {

	j = r_ptr->r_char;

	if ((j=='k')||(j=='L')||(j=='o')||(j=='O')||(j=='T')||(j=='y')||
	    (j=='I')||(j=='&')) {

	    /* Try for the highest level monster we can get */
	    for (z = MAX_R_IDX-1; z>=0; z--) {

		/* Find a similar, lower level, non-unique, monster */
		if ((r_list[z].r_char == j) &&
		    (r_list[z].level <= r_list[r_idx].level) &&
		    !(r_list[z].cflags2 & MF2_UNIQUE)) {

		    /* Try up to 50 nearby places */
		    count = 0;
		    do {
			ny = rand_spread(y, 3);
			nx = rand_spread(x, 3);
			count++;
		    } while (!test_place(ny,nx) && (count<51));

		    /* Certain monsters come in groups */
		    if ((j=='k') || (j=='y') || (j=='&') ||
			(r_list[z].cflags2 & MF2_GROUP)) {
			place_group(ny,nx,z,slp);
		    }

		    /* Otherwise, just use a single escort */
		    else {
			place_monster(ny,nx,z,slp);
		    }
		}
	    }
	}
    }

    /* Success */
    return TRUE;
}


/*
 * Places a "winning" monster at given location	    -RAK-
 */
int place_win_monster()
{
    register int y, x, cur_pos;
    register monster_type *mon_ptr;

    /* Hack -- caught by place_monster() */
    if (total_winner) return (FALSE);

	cur_pos = m_pop();
    /* paranoia error check, from um55 -CFT */
	if (cur_pos == -1) return FALSE;

    /* Attempt to place */
    if (wizard || peek) {
	msg_print("Placing win monster");
    }

	mon_ptr = &m_list[cur_pos];

	do {
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].m_idx != 0)
	       || (cave[y][x].i_idx != 0) ||
	       (distance(y, x, char_row, char_col) <= MAX_SIGHT));

	mon_ptr->fy = y;
	mon_ptr->fx = x;
	mon_ptr->r_idx = MAX_R_IDX - 2;
	if (r_list[mon_ptr->r_idx].cflags2 & MF2_MAX_HP)
	    mon_ptr->hp = max_hp(r_list[mon_ptr->r_idx].hd);
	else
	    mon_ptr->hp = pdamroll(r_list[mon_ptr->r_idx].hd);
	mon_ptr->mspeed = r_list[mon_ptr->r_idx].speed - 10;
	mon_ptr->stunned = 0;
	mon_ptr->cdis = distance(char_row, char_col, y, x);
	cave[y][x].m_idx = cur_pos;
	mon_ptr->csleep = 0;

    return TRUE;
}


static char *cap(char *str)
{
    if ((*str >= 'a') && (*str <= 'z'))
	*str = *str - 'a' + 'A';
    return str;
}

/*
 * Prepare the "ghost" monster_race info
 */
void set_ghost(monster_race *g, cptr name, int gr, int gc, int lev)
{
    char ghost_race[20];
    char ghost_class[20];
    int  i;

    /* Allocate storage for name -TL -- braindamaged ghost name spoo -CWS */
    if (r_list[MAX_R_IDX - 1].name == NULL) {
	r_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	C_WIPE(r_list[MAX_R_IDX - 1].name, 101, char);
	*((char *) r_list[MAX_R_IDX - 1].name) = 'A';
    }

    switch (gr) {
      case 0:
      case 8:
	strcpy(ghost_race, "human");
	break;
      case 1:
      case 2:
      case 9:
	strcpy(ghost_race, "elf");
	break;
      case 3:
	strcpy(ghost_race, "hobbit");
	break;
      case 4:
	strcpy(ghost_race, "gnome");
	break;
      case 5:
	strcpy(ghost_race, "dwarf");
	break;
      case 6:
	strcpy(ghost_race, "orc");
	break;
      case 7:
	strcpy(ghost_race, "troll");
	break;
    }
    switch (gc) {
      case 0:
	strcpy(ghost_class, "warrior");
	break;
      case 1:
	strcpy(ghost_class, "mage");
	break;
      case 2:
	strcpy(ghost_class, "priest");
	break;
      case 3:
	strcpy(ghost_class, "rogue");
	break;
      case 4:
	strcpy(ghost_class, "ranger");
	break;
      case 5:
	strcpy(ghost_class, "paladin");
	break;
    }

    /* Save the level */
    g->level = lev;

    /* Never asleep (?) */
    g->sleep = 0;

    /* Very attentive (?) */
    g->aaf = 100;

    g->mexp = lev * 5 + 5;

    g->spells2 = NONE8;

    /* Town ghost */
    if (!dun_level) {

	/* A wanderer in the town */
	sprintf(g->name, "%s, the %s %s",
		cap(name), cap(ghost_race), cap(ghost_class));

	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_90 | MF1_HAS_60 | MF2_GOOD);

	if (lev > 10) g->cflags1 |= (MF1_HAS_1D2);
	if (lev > 18) g->cflags1 |= (MF1_HAS_2D2);
	if (lev > 23) g->cflags1 |= (MF1_HAS_4D2);
	if (lev > 40) g->cflags1 |= (MF2_SPECIAL);
	if (lev > 40) g->cflags1 &= (~MF1_HAS_4D2);

	/* Add some random resists -DGK */
	for (i = 0; i <= (lev / 5); i++) {
	    switch ((int) randint(13)) {
	      case 1:
	      case 2:
	      case 3:
		g->cflags2 |= (MF2_IM_FIRE);
	      case 4:
	      case 5:
	      case 6:
		g->cflags2 |= (MF2_IM_ACID);
	      case 7:
	      case 8:
	      case 9:
		g->cflags2 |= (MF2_IM_COLD);
	      case 10:
	      case 11:
	      case 12:
		g->cflags2 |= (MF2_IM_ELEC);
	      case 13:
		g->cflags2 |= (MF2_IM_POIS);
	    }
	}

	switch (gc) {
	  case 0:		   /* Warrior */
	    g->spells1 = NONE8;
	    break;
	  case 1:		   /* Mage */
	    g->spells1 |= (0x3L | BLINK | MAG_MISS |
			   SLOW | CONFUSION);
	    if (lev > 5) g->spells2 |= ST_CLOUD;
	    if (lev > 7) g->spells2 |= LIGHT_BOLT;
	    if (lev > 10) g->spells1 |= FROST_BOLT;
	    if (lev > 12) g->spells1 |= TELE;
	    if (lev > 15) g->spells1 |= ACID_BOLT;
	    if (lev > 20) g->spells1 |= FIRE_BOLT;
	    if (lev > 25) g->spells1 |= FROST_BALL;
	    if (lev > 25) g->spells2 |= HASTE;
	    if (lev > 30) g->spells1 |= FIRE_BALL;
	    if (lev > 40) g->spells1 |= MANA_BOLT;
	    break;
	  case 3:		   /* Rogue */
	    g->spells1 |= (0x5L | BLINK);
	    if (lev > 10) g->spells1 |= CONFUSION;
	    if (lev > 18) g->spells1 |= SLOW;
	    if (lev > 25) g->spells1 |= TELE;
	    if (lev > 30) g->spells1 |= HOLD_PERSON;
	    if (lev > 35) g->spells1 |= TELE_TO;
	    break;
	  case 4:		   /* Ranger */
	    g->spells1 |= (0x8L | MAG_MISS);
	    if (lev > 5) g->spells2 |= ST_CLOUD;
	    if (lev > 7) g->spells2 |= LIGHT_BOLT;
	    if (lev > 10) g->spells1 |= FROST_BOLT;
	    if (lev > 18) g->spells1 |= ACID_BOLT;
	    if (lev > 25) g->spells1 |= FIRE_BOLT;
	    if (lev > 30) g->spells1 |= FROST_BALL;
	    if (lev > 35) g->spells1 |= FIRE_BALL;
	    break;
	  case 2:		   /* Priest */
	  case 5:		   /* Paladin */
	    g->spells1 |= (0x4L | CAUSE_LIGHT | FEAR);
	    if (lev > 5) g->spells2 |= HEAL;
	    if (lev > 10) g->spells1 |= (CAUSE_SERIOUS | BLINDNESS);
	    if (lev > 18) g->spells1 |= HOLD_PERSON;
	    if (lev > 25) g->spells1 |= CONFUSION;
	    if (lev > 30) g->spells1 |= CAUSE_CRIT;
	    if (lev > 35) g->spells1 |= MANA_DRAIN;
	    break;
	}

	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_EVIL);

	if (gr == 6) g->cflags2 |= MF2_ORC;
	else if (gr == 7) g->cflags2 |= MF2_TROLL;

	g->ac = 15 + randint(15);
	if (gc == 0 || gc >= 3) g->ac += randint(60);

	/* High level mages and rogues are fast... */
	if ((gc == 1 || gc == 3) && lev > 25) g->speed = 12;

	else g->speed = 11;

	/* Use the letter 'p' */
	g->r_char = 'p';

	/* XXX */
	g->hd[1] = 1;

	g->damage[0] = 5 + ((lev > 18) ? 18 : lev);
	g->damage[1] = g->damage[0];

	switch (gc) {
	  case 0:
	    g->damage[2] = ((lev < 30) ? (5 + ((lev > 18) ? 18 : lev)) : 235);
	    g->damage[3] = g->damage[2];
	    break;
	  case 1:
	  case 2:
	    g->damage[2] = 0;
	    g->damage[3] = 0;
	    break;
	  case 3:
	    g->damage[2] = g->damage[3] = ((lev < 30) ? 149 : 232);
	    break;
	  case 4:
	  case 5:
	    g->damage[2] = g->damage[3] = g->damage[1];
	    break;
	}

	return;
    }

    /* Make a ghost with power based on the ghost level */
    switch ((int) (g->level / 4) + randint(3)) {

      case 1:
      case 2:
      case 3:
	sprintf(g->name, "%s, the Skeleton %s", name, ghost_race);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_90 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 26;
	g->speed = 11;
	g->r_char = 's';
	g->hd[1] = 1;
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;

      case 4:
      case 5:
	sprintf(g->name, "%s, the %s zombie", name, cap(ghost_race));
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_60 | MF1_HAS_90 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 30;
	g->speed = 11;
	g->r_char = 'z';
	g->hd[1] *= 2;
	g->damage[0] = 8;
	g->damage[1] = 0;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;

      case 6:
	sprintf(g->name, "%s, the Poltergeist", name);
	g->cflags1 |= (MF1_MV_INVIS | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF2_GOOD | MF1_HAS_1D2 | MF1_MV_75 | MF1_THRO_WALL);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 20;
	g->speed = 13;
	g->r_char = 'G';
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 93;
	g->damage[3] = 93;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 7:
      case 8:
	sprintf(g->name, "%s, the Mummified %s", name, cap(ghost_race));
	g->cflags1 |= (MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 35;
	g->speed = 11;
	g->r_char = 'M';
	g->hd[1] *= 2;
	g->damage[0] = 16;
	g->damage[1] = 16;
	g->damage[2] = 16;
	g->damage[3] = 0;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 9:
      case 10:
	sprintf(g->name, "%s%s spirit", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 20;
	g->speed = 11;
	g->r_char = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 19;
	g->damage[1] = 185;
	g->damage[2] = 99;
	g->damage[3] = 178;
	g->mexp = g->mexp * 3;
	break;

      case 11:
	sprintf(g->name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (0xFL | HOLD_PERSON | MANA_DRAIN | BLINDNESS);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 40;
	g->speed = 12;
	g->r_char = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = (g->mexp * 7) / 2;
	break;

      case 12:
	sprintf(g->name, "%s, the Vampire", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | MF2_GOOD);
	g->spells1 |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_SERIOUS);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA | MF2_HURT_LITE);
	g->ac = 40;
	g->speed = 11;
	g->r_char = 'V';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 3;
	break;

      case 13:
	sprintf(g->name, "%s%s Wraith", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_4D2 | MF1_HAS_2D2 | MF2_GOOD);
	g->spells1 |= (0x7L | HOLD_PERSON | FEAR | BLINDNESS | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA | MF2_HURT_LITE);
	g->ac = 60;
	g->speed = 12;
	g->r_char = 'W';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 5;
	break;

      case 14:
	sprintf(g->name, "%s, the Vampire Lord", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_SPECIAL);
	g->spells1 |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA | MF2_HURT_LITE);
	g->ac = 80;
	g->speed = 11;
	g->r_char = 'V';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 20;
	g->damage[3] = 198;
	g->mexp = g->mexp * 20;
	break;

      case 15:
	sprintf(g->name, "%s%s ghost", name,
		 (name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | MF2_SPECIAL);
	g->spells1 |= (0x5L | HOLD_PERSON | MANA_DRAIN | BLINDNESS | CONFUSION);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_IM_POIS | MF2_NO_INFRA);
	g->ac = 90;
	g->speed = 13;
	g->r_char = 'G';
	g->hd[1] *= 3;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 20;
	break;

      case 17:
	sprintf(g->name, "%s, the Lich", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | MF1_HAS_1D2 | MF2_SPECIAL);
	g->spells1 |= (0x3L | FEAR | CAUSE_CRIT | TELE_TO | BLINK |
		       S_UNDEAD | FIRE_BALL | FROST_BALL | HOLD_PERSON |
		       MANA_DRAIN | BLINDNESS | CONFUSION | TELE);
	g->spells2 |= (BRAIN_SMASH | RAZOR);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA | MF2_IM_POIS| MF2_INTELLIGENT);
	g->ac = 120;
	g->speed = 12;
	g->r_char = 'L';
	g->hd[1] *= 3;
	g->hd[0] *= 2;
	g->damage[0] = 181;
	g->damage[1] = 201;
	g->damage[2] = 214;
	g->damage[3] = 181;
	g->mexp = g->mexp * 50;
	break;

      default:
	sprintf(g->name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ |
		       MF1_HAS_1D2 | MF1_HAS_2D2 | MF2_SPECIAL);
	g->spells1 |= (0x2L | HOLD_PERSON | MANA_DRAIN | 
		       BLINDNESS | CONFUSION | TELE_TO);
	g->spells2 |= (NETHER_BOLT | NETHER_BALL | BRAIN_SMASH |
		       TELE_LEV);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA | MF2_INTELLIGENT);
	g->ac = 130;
	g->speed = 13;
	g->r_char = 'G';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 30;
	break;
    }
}



/*
 * Places a ghost somewhere.
 * Probably not the best possible algorithm.
 */
int place_ghost()
{
    register int           y, x, cur_pos;
    register monster_type  *m_ptr;
    monster_race           *ghost = &r_list[MAX_R_IDX - 1];
    char                   tmp[100];
    char                   name[100];
    int                    i, j, level;
    int                    ghost_race;
    int                    cl;
    FILE		   *fp;

    /* In the town, ghosts have the same level as the player */
    if (!dun_level) {

	/* You have to be level 5, and even then its only 10% */
	if (py.misc.lev < 5 || randint(10) > 1) return 0;

	/* Look for a proper bones file */
	sprintf(tmp, "%s/%d", ANGBAND_DIR_BONES, py.misc.lev);
	if ((fp = my_tfopen(tmp, "r")) != NULL) {

	/* Read the bones info */
	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
	    fclose(fp);
	    if (wizard) msg_print("Town:Failed to scan in info properly!");
	    return 0;
	}

	fclose(fp);

	    j = 1;
	    if (i > 255) {	   /* avoid wrap-around of byte hitdice, by
				    * factoring */
		j = i / 32;
		i = 32;
	    }
	    ghost->hd[0] = i;	   /* set_ghost may adj for race/class/lv */
	    ghost->hd[1] = j;
	    level = py.misc.lev;
	} else {
	    return 0;
	}
    }

    /* In the dungeon, ghosts have the same level as the level */    
    else {

	/* And even then, it only happens sometimes */
	if (14 > randint((dun_level / 2) + 11)) return 0;

	/* Or rather, 1/3 of that often :-) */
	if (randint(3) == 1) {

	/* Open the bones file */
	sprintf(tmp, "%s/%d", ANGBAND_DIR_BONES, dun_level);
	if ((fp = my_tfopen(tmp, "r")) != NULL) {

	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
	    fclose(fp);
	    if (wizard) msg_print("Ghost:Failed to scan in info properly!");
	    return 0;
	}
	fclose(fp);

		j = 1;
		if (i > 255) {	   /* avoid wrap-around of byte hitdice, by
				    * factoring */
		    j = i / 32;
		    i = 32;
		}
		ghost->hd[0] = i;  /* set_ghost may adj for race/class/lv */
		ghost->hd[1] = j;

	level = dun_level;

	    } else {
		return 0;
	    }
	} else {
	    return 0;
	}
    }

    /* Set up the ghost */
    set_ghost(ghost, name, ghost_race, cl, level);

    /* Note for wizard (special ghost name) */
    if (wizard || peek) msg_print(ghost->name);

    cur_pos = m_pop();
    m_ptr = &m_list[cur_pos];

    do {
	/* Pick a location */
	y = randint(cur_height - 2);
	x = randint(cur_width - 2);
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].m_idx != 0)
	     || (cave[y][x].i_idx != 0) ||
	     (distance(y, x, char_row, char_col) <= MAX_SIGHT));

    /*** Place the Ghost by Hand (so no-one else does it accidentally) ***/

    m_ptr->fy = y;
    m_ptr->fx = x;

    m_ptr->r_idx = (MAX_R_IDX - 1);

    /* Assign the hitpoints */
    m_ptr->hp = (s16b) ghost->hd[0] * (s16b) ghost->hd[1];

    /* the r_list speed value is 10 greater, so that it can be a byte */
    m_ptr->mspeed = r_list[mon_ptr->r_idx].speed - 10;
    
    m_ptr->stunned = 0;
    m_ptr->cdis = distance(char_row, char_col, y, x);
    cave[y][x].m_idx = cur_pos;
    m_ptr->csleep = 0;

    return TRUE;
}




/*
 * Mega-Hack -- allocation helper
 *
 * Number of monsters with level 0-N 
 */
s16b m_level[MAX_R_LEV+1];



/*
 * Return a monster suitable to be placed at a given level.  This makes high
 * level monsters (up to the given level) slightly more common than low level
 * monsters at any given level.   -CJS- 
 *
 * Code has been added to make it slightly more likely to get the higher level
 * monsters at higher dungeon levels.  Originally a uniform distribution over
 * all monsters of level less than or equal to the dungeon level.  The new
 * distribution makes a level n monster occur approx 2/n% of the time on
 * level n, and 1/n*n% are 1st level.
 */
int get_mons_num(int level)
{
    register int i, j, num;

    int          old = level;


    while (1) {

	if (level == 0) {
	    i = rand_int(m_level[0]);
	}

	else {

	    if (level > MAX_R_LEV) level = MAX_R_LEV;

	    /* Make a Nasty Monster */
	    if (randint(MON_NASTY) == 1) {
		/* Make low level monsters more likely at low levels */
		i = level / 4 + 1;
		if (i > 4) i = 4;
		level = level + MY_ABS(i) + 1;
		if (level > MAX_R_LEV) level = MAX_R_LEV;
	    }
	    else {
		/* Make high level monsters more likely at high levels */
		num = m_level[level] - m_level[0];
		i = randint(num) - 1;
		j = randint(num) - 1;
		if (j > i) i = j;
		level = r_list[i + m_level[0]].level;
	    }

	    /* Bizarre function */            
	    i = m_level[level] - m_level[level - 1];
	    if (i == 0) i++;
	    i = randint(i) - 1 + m_level[level - 1];
	}

	/* Uniques never appear out of "modified" depth */
	if ((r_list[i].level > old) &&
	    (r_list[i].cflags2 & MF2_UNIQUE)) {
	    continue;
	}

	/* Quest Monsters never appear out of depth */
	if ((r_list[i].level > dun_level) &&
	    (r_list[i].cflags2 & MF2_QUESTOR)) {
	    continue;
	}

    }

    /* Accept the monster */
    return i;
}


/*
 * Get a monster race index.  Method 2.
 */
int get_nmons_num(int level)
{
    register int i, j, num;
    int          old;

    old = level;


    while (1) {

	if (level == 0) {
	    i = rand_int(m_level[0]);
	}

	else {

	    if (level > MAX_R_LEV) level = MAX_R_LEV;

	    num = m_level[level] - m_level[0];

	    i = rand_int(num);
	    i += 15;
	    if (i >= num) i = num - 1;

	    j = rand_int(num);
	    if (j > i) i = j;

	    j = rand_int(num);
	    if (j > i) i = j;

	    level = r_list[i + m_level[0]].level;
	    i = m_level[level] - m_level[level - 1];
	    if (i == 0) i = 1;

	    i = randint(i) - 1 + m_level[level - 1];
	}

	if ((r_list[i].level > old) && (r_list[i].cflags2 & MF2_UNIQUE)) {
	    continue;
	}

	/* Quest monsters never appear out of depth */
	if ((r_list[i].level > dun_level) &&
	    (r_list[i].cflags2 & MF2_QUESTOR)) {
	    continue;
	}

    }

    /* Accept the monster */
    return i;
}


void place_group(int y, int x, int r_idx, int slp)
{
    /* prevent level rating from skyrocketing if they are out of depth... */
    int old = rating;
    int extra = 0;

    /* reduce size of group if out-of-depth */
    if (r_list[r_idx].level > (unsigned) dun_level) {
	extra = 0 - randint(r_list[r_idx].level - dun_level);
    }

    /* if monster is deeper than normal, then travel in bigger packs -CFT */
    else if (r_list[r_idx].level < (unsigned) dun_level) {
	extra = randint(dun_level - r_list[r_idx].level);
    }

    /* put an upper bounds on it... -CFT */
    if (extra > 12) extra = 12;

    switch (randint(13) + extra) {
      case 25:
	place_monster(y, x - 3, r_idx, 0);
      case 24:
	place_monster(y, x + 3, r_idx, 0);
      case 23:
	place_monster(y - 3, x, r_idx, 0);
      case 22:
	place_monster(y + 3, x, r_idx, 0);
      case 21:
	place_monster(y - 2, x + 1, r_idx, 0);
      case 20:
	place_monster(y + 2, x - 1, r_idx, 0);
      case 19:
	place_monster(y + 2, x + 1, r_idx, 0);
      case 18:
	place_monster(y - 2, x - 1, r_idx, 0);
      case 17:
	place_monster(y + 1, x + 2, r_idx, 0);
      case 16:
	place_monster(y - 1, x - 2, r_idx, 0);
      case 15:
	place_monster(y + 1, x - 2, r_idx, 0);
      case 14:
	place_monster(y - 1, x + 2, r_idx, 0);
      case 13:
	place_monster(y, x - 2, r_idx, 0);
      case 12:
	place_monster(y, x + 2, r_idx, 0);
      case 11:
	place_monster(y + 2, x, r_idx, 0);
      case 10:
	place_monster(y - 2, x, r_idx, 0);
      case 9:
	place_monster(y + 1, x + 1, r_idx, 0);
      case 8:
	place_monster(y + 1, x - 1, r_idx, 0);
      case 7:
	place_monster(y - 1, x - 1, r_idx, 0);
      case 6:
	place_monster(y - 1, x + 1, r_idx, 0);
      case 5:
	place_monster(y, x + 1, r_idx, 0);
      case 4:
	place_monster(y, x - 1, r_idx, 0);
      case 3:
	place_monster(y + 1, x, r_idx, 0);
      case 2:
	place_monster(y - 1, x, r_idx, 0);
	rating = old;
      case 1:
      default:			   /* just in case I screwed up -CFT */
	place_monster(y, x, r_idx, 0);
    }
}


/*
 * Allocates some random monsters   -RAK-	 
 * Place the monsters at least "dis" distance from the player.
 * Use "slp" to choose the initial "sleep" status
 */
void alloc_monster(int num, int dis, int slp)
{
    register int y, x, i;
    int          r_idx;

    /* Place the monsters */
    for (i = 0; i < num; i++) {

	/* Find a legal, distant, unoccupied, space */
	do {

	    /* Pick a location */
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while (cave[y][x].fval >= MIN_CLOSED_SPACE || (cave[y][x].m_idx != 0) ||
	       (distance(y, x, char_row, char_col) <= dis));

	do {
	/* Get a monster of the given level */
	r_idx = get_mons_num(dun_level);
	} while (randint(r_list[r_idx].rarity) > 1);

    /*
     * to give the player a sporting chance, any monster that appears in
     * line-of-sight and can cast spells or breathe, should be asleep. This
     * is an extension of Um55's sleeping dragon code... 
     */

	if (((r_list[r_idx].spells1 & (CAUSE_LIGHT | CAUSE_SERIOUS | HOLD_PERSON |
			    BLINDNESS | CONFUSION | FEAR | SLOW | BREATH_L |
			       BREATH_G | BREATH_A | BREATH_FR | BREATH_FI |
			     FIRE_BOLT | FROST_BOLT | ACID_BOLT | MAG_MISS |
			   CAUSE_CRIT | FIRE_BALL | FROST_BALL | MANA_BOLT))
	     || (r_list[r_idx].spells2 & (BREATH_CH | BREATH_SH | BREATH_SD | BREATH_CO |
			   BREATH_DI | BREATH_LD | LIGHT_BOLT | LIGHT_BALL |
			      ACID_BALL | TRAP_CREATE | RAZOR | MIND_BLAST |
			    MISSILE | PLASMA_BOLT | NETHER_BOLT | ICE_BOLT |
				FORGET | BRAIN_SMASH | ST_CLOUD | TELE_LEV |
			 WATER_BOLT | WATER_BALL | NETHER_BALL | BREATH_NE))
	     || (r_list[r_idx].spells3 & (BREATH_WA | BREATH_SL | BREATH_LT | BREATH_TI |
				 BREATH_GR | BREATH_DA | BREATH_PL | ARROW |
					DARK_STORM | MANA_STORM)))
	    && (los(y, x, char_row, char_col))) {
	    slp = TRUE;
	}

	if (!(r_list[r_idx].cflags2 & MF2_GROUP)) {
	    place_monster(y, x, r_idx, slp);
	}
	else {
	    place_group(y, x, r_idx, slp);
	}
    }
}


/*
 * Places a random creature at or adjacent to the given location
 */
int summon_monster(int *yp, int *xp, int slp)
{
    register int        i, y, x, r_idx;

    /* Try nine locations */
    for (i = 0; i < 9; i++) {

	/* Pick a nearby location */
	y = rand_spread(*yp, 1);
	x = rand_spread(*xp, 1);

	/* Require legal grid */
	if (!in_bounds(y, x)) continue;
	
	/* Require "empty" floor grids */
	if (!empty_grid_bold(y, x)) continue;
	
	/* Pick a monster race */
	r_idx = get_mons_num(dun_level + MON_SUMMON_ADJ);

	/* Place the monster */
	if (r_list[r_idx].cflags2 & MF2_GROUP) {
	    place_group(y, x, r_idx, slp);
	}
	else {
	    place_monster(y, x, r_idx, slp);
	}

	/* Save the location */
	*yp = y;
	*xp = x;
		
	/* Success */
	return (TRUE);
    }

    /* Nothing summoned */
    return (FALSE);
}


/*
 * Places undead adjacent to given location
 */
int summon_undead(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cflags2 & MF2_UNDEAD) && !(r_list[m].cflags2 & MF2_UNIQUE) &&
		(r_list[m].level < dun_level + 5)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    } while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

/*
 * Summon a demon.
 */
int summon_demon(int lev, int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cflags2 & MF2_DEMON && !(r_list[m].cflags2 & MF2_UNIQUE) &&
		(r_list[m].level <= lev)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    } while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

int summon_dragon(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cflags2 & MF2_DRAGON && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/* Summon ringwraiths */
int summon_wraith(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'W' && (r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_reptile(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'R' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_spider(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'S' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_angel(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'A' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

int summon_ant(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'a' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_unique(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (!(r_list[m].r_char == 'P') && (r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_jabberwock(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'J' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_gundead(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].r_char == 'L') || (r_list[m].r_char == 'V')
		|| (r_list[m].r_char == 'W')) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_ancientd(int *y, int *x)
int *y, *x;
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].r_char == 'D') {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

int summon_hound(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].r_char == 'C' || r_list[m].r_char == 'Z')
		&& !(r_list[m].cflags2 & MF2_UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}




/* Pushs a record back onto free space list		-RAK-	 */
/*
 * Delete_object() should always be called instead, unless the object in
 * question is not in the dungeon, e.g. in store1.c and files.c 
 */
void pusht(int my_x)
{
    s16b        x = (s16b) my_x;
    register int i, j;

    if (x != i_max - 1) {
	i_list[x] = i_list[i_max - 1];

    /* must change the i_idx in the cave of the object just moved */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++)
		if (cave[i][j].i_idx == i_max - 1)
		    cave[i][j].i_idx = x;
    }
    i_max--;
    invcopy(&i_list[i_max], OBJ_NOTHING);
}


int unique_weapon(inven_type *t_ptr)
{
    const char *name;

    name = k_list[t_ptr->index].name;
    if (!stricmp("& Longsword", name)) {
	switch (randint(15)) {
	  case 1:
	    if (RINGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Ringil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_RINGIL;
	    t_ptr->tohit = 22;
	    t_ptr->todam = 25;
	    t_ptr->damage[0] = 4;
	    t_ptr->flags1 = (TR3_SEE_INVIS | TR1_SLAY_UNDEAD | TR1_SLAY_EVIL | TR3_REGEN |
		     TR1_SPEED | TR2_RES_COLD | TR1_BRAND_COLD | TR2_FREE_ACT |
			    TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR1_SLAY_TROLL | TR3_LITE | TR3_ACTIVATE
			      | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->pval = 1;
	    t_ptr->cost = 300000L;
	    RINGIL = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	    if (ANDURIL)
		return 0;
	    if (wizard || peek)
		msg_print("Anduril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANDURIL;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags1 = (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR2_FREE_ACT |
		     TR_SUST_STAT | TR1_STR | TR2_RES_FIRE | TR1_BRAND_FIRE);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR3_ACTIVATE | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->pval = 4;
	    t_ptr->toac = 5;
	    t_ptr->cost = 80000L;
	    ANDURIL = 1;
	    return 1;
	  case 5:
	  case 6:
	  case 7:
	  case 8:
	    if (ANGUIREL)
		return 0;
	    if (wizard || peek)
		msg_print("Anguirel");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANGUIREL;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 12;
	    t_ptr->flags1 = (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR2_FREE_ACT | TR2_RES_ELEC
			    | TR1_STR | TR1_CON);
	    t_ptr->flags2 |= (TR_ARTIFACT |
		       TR3_LITENING | TR_LIGHT | TR1_SLAY_DEMON | TR2_RES_LITE);
	    t_ptr->pval = 2;
	    t_ptr->cost = 40000L;
	    ANGUIREL = 1;
	    return 1;
	  default:
	    if (ELVAGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Elvagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ELVAGIL;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 2;
	    t_ptr->todam = 7;
	    t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_CHR | TR1_DEX | TR1_STEALTH | TR3_FEATHER);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->pval = 2;
	    t_ptr->cost = 30000L;
	    ELVAGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Two-Handed Sword", name)) {
	switch (randint(8)) {
	  case 1:
	  case 2:
	    if (GURTHANG)
		return 0;
	    if (wizard || peek)
		msg_print("Gurthang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_GURTHANG;
	    t_ptr->tohit = 13;
	    t_ptr->todam = 17;
	    t_ptr->flags1 = (TR3_REGEN | TR1_SLAY_X_DRAGON | TR1_STR |
			    TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_ARTIFACT);
	    t_ptr->pval = 2;
	    t_ptr->cost = 100000L;
	    GURTHANG = 1;
	    return 1;
	  case 3:
	    if (ZARCUTHRA)
		return 0;
	    if (randint(3) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Zarcuthra");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ZARCUTHRA;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 19;
	    t_ptr->todam = 21;
	    t_ptr->flags1 = (TR1_SLAY_X_DRAGON | TR1_STR | TR1_SLAY_EVIL | TR1_SLAY_ANIMAL |
		  TR1_SLAY_UNDEAD | TR3_AGGRAVATE | TR1_CHR | TR1_BRAND_FIRE |
		  TR3_SEE_INVIS | TR2_RES_FIRE | TR2_FREE_ACT | TR1_INFRA);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_ORC | TR_SLAY_MF2_GIANT
			      | TR1_SLAY_DEMON | TR2_RES_CHAOS);
	    t_ptr->pval = 4;
	    t_ptr->damage[0] = 6;
	    t_ptr->damage[1] = 4;
	    t_ptr->cost = 200000L;
	    ZARCUTHRA = 1;
	    return 1;
	  default:
	    if (MORMEGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Mormegil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_MORMEGIL;
	    t_ptr->tohit = -40;
	    t_ptr->todam = -60;
	    t_ptr->flags1 = (TR1_SPEED | TR3_AGGRAVATE | TR3_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->pval = -1;
	    t_ptr->toac = -50;
	    t_ptr->cost = 10000L;
	    MORMEGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Broadsword", name)) {
	switch (randint(12)) {
	  case 1:
	  case 2:
	    if (ARUNRUTH)
		return 0;
	    if (wizard || peek)
		msg_print("Arunruth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ARUNRUTH;
	    t_ptr->tohit = 20;
	    t_ptr->todam = 12;
	    t_ptr->damage[0] = 3;
	    t_ptr->flags1 = (TR3_FEATHER | TR1_DEX |
			    TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_ORC | TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->pval = 4;
	    t_ptr->cost = 50000L;
	    ARUNRUTH = 1;
	    return 1;
	  case 3:
	  case 4:
	  case 5:
	  case 6:
	    if (GLAMDRING)
		return 0;
	    if (wizard || peek)
		msg_print("Glamdring");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_GLAMDRING;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags1 = (TR1_SLAY_EVIL | TR3_SLOW_DIGEST | TR1_SEARCH | TR1_BRAND_FIRE |
			    TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR3_LITE | TR2_RES_LITE);
	    t_ptr->pval = 3;
	    t_ptr->cost = 40000L;
	    GLAMDRING = 1;
	    return 1;
	  case 7:
	    if (AEGLIN)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_AEGLIN;
	    t_ptr->tohit = 12;
	    t_ptr->todam = 16;
	    t_ptr->flags1 = (TR3_SLOW_DIGEST | TR1_SEARCH | TR2_RES_ELEC);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR3_LITE | TR_LIGHTNING);
	    t_ptr->pval = 4;
	    t_ptr->cost = 45000L;
	    AEGLIN = 1;
	    return 1;
	  default:
	    if (ORCRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Orcrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ORCRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags1 = (TR1_SLAY_EVIL | TR3_SLOW_DIGEST | TR1_STEALTH | TR1_BRAND_COLD |
			    TR2_RES_COLD);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR3_LITE);
	    t_ptr->pval = 3;
	    t_ptr->cost = 40000L;
	    ORCRIST = 1;
	    return 1;
	}
    } else if (!stricmp("& Bastard Sword", name)) {
	if (CALRIS)
	    return 0;
	if (wizard || peek)
	    msg_print("Calris");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CALRIS;
	t_ptr->tohit = -20;
	t_ptr->todam = 20;
	t_ptr->damage[0] = 3;
	t_ptr->damage[1] = 7;
	t_ptr->flags1 = (TR1_SLAY_X_DRAGON | TR1_CON | TR3_AGGRAVATE |
			TR3_CURSED | TR1_SLAY_EVIL);
	t_ptr->flags2 |= (TR1_SLAY_DEMON | TR1_SLAY_TROLL | TR2_RES_DISEN
			  | TR_ARTIFACT);
	t_ptr->pval = 5;
	t_ptr->cost = 100000L;
	CALRIS = 1;
	return 1;
    } else if (!stricmp("& Main Gauche", name)) {
	if (randint(4) > 1)
	    return 0;
	if (MAEDHROS)
	    return 0;
	if (wizard || peek)
	    msg_print("Maedhros");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_MAEDHROS;
	t_ptr->tohit = 12;
	t_ptr->todam = 15;
	t_ptr->damage[0] = 2;
	t_ptr->damage[1] = 6;
	t_ptr->flags1 = (TR1_DEX | TR1_INT | TR2_FREE_ACT | TR3_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_GIANT | TR1_SLAY_TROLL);
	t_ptr->pval = 3;
	t_ptr->cost = 20000L;
	MAEDHROS = 1;
	return 1;
    } else if (!stricmp("& Glaive", name)) {
	if (randint(3) > 1)
	    return 0;
	if (PAIN)
	    return 0;
	if (wizard || peek)
	    msg_print("Pain!");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_PAIN;
	t_ptr->tohit = 0;
	t_ptr->todam = 30;
	t_ptr->damage[0] = 10;
	t_ptr->damage[1] = 6;
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 50000L;
	PAIN = 1;
	return 1;
    } else if (!stricmp("& Halberd", name)) {
	if (OSONDIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Osondir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_OSONDIR;
	t_ptr->tohit = 6;
	t_ptr->todam = 9;
	t_ptr->flags1 = (TR1_BRAND_FIRE | TR1_SLAY_UNDEAD | TR2_RES_FIRE |
			TR3_FEATHER | TR1_CHR | TR3_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_SOUND | TR1_SLAY_GIANT);
	t_ptr->pval = 3;
	t_ptr->cost = 22000L;
	OSONDIR = 1;
	return 1;
    } else if (!stricmp("& Lucerne Hammer", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TURMIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Turmil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TURMIL;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 6;
	t_ptr->flags1 = (TR1_WIS | TR3_REGEN | TR1_BRAND_COLD | TR2_RES_COLD | TR1_INFRA);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR3_LITE |
			  TR3_ACTIVATE | TR2_RES_LITE);
	t_ptr->pval = 4;
	t_ptr->cost = 30000L;
	t_ptr->toac = 8;
	TURMIL = 1;
	return 1;
    } else if (!stricmp("& Pike", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Til-i-arc");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TIL;
	t_ptr->tohit = 10;
	t_ptr->todam = 12;
	t_ptr->toac = 10;
	t_ptr->flags1 = (TR1_BRAND_COLD | TR1_BRAND_FIRE | TR2_RES_FIRE | TR2_RES_COLD |
			TR3_SLOW_DIGEST | TR1_INT | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_DEMON | TR_SLAY_MF2_GIANT | TR1_SLAY_TROLL);
	t_ptr->pval = 2;
	t_ptr->cost = 32000L;
	TIL = 1;
	return 1;
    } else if (!stricmp("& Mace of Disruption", name)) {
	if (randint(5) > 1)
	    return 0;
	if (DEATHWREAKER)
	    return 0;
	if (wizard || peek)
	    msg_print("Deathwreaker");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_DEATHWREAKER;
	t_ptr->tohit = 18;
	t_ptr->todam = 18;
	t_ptr->damage[1] = 12;
	t_ptr->flags1 = (TR1_STR | TR1_BRAND_FIRE | TR1_SLAY_EVIL | TR1_SLAY_DRAGON |
		   TR1_SLAY_ANIMAL | TR1_TUNNEL | TR3_AGGRAVATE | TR2_RES_FIRE);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_IM_FIRE | TR2_RES_CHAOS
			  | TR2_RES_DISEN | TR2_RES_DARK);
	t_ptr->pval = 6;
	t_ptr->cost = 400000L;
	DEATHWREAKER = 1;
	return 1;
    } else if (!stricmp("& Scythe", name)) {
	if (AVAVIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Avavir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AVAVIR;
	t_ptr->tohit = 8;
	t_ptr->todam = 8;
	t_ptr->toac = 10;
	t_ptr->flags1 = (TR1_DEX | TR1_CHR | TR2_FREE_ACT | TR2_RES_FIRE | TR2_RES_COLD |
			TR3_SEE_INVIS | TR1_BRAND_FIRE | TR1_BRAND_COLD);
	t_ptr->flags2 |= (TR_ARTIFACT | TR3_LITE | TR3_ACTIVATE | TR2_RES_LITE);
	t_ptr->pval = 3;
	t_ptr->cost = 18000L;
	AVAVIR = 1;
	return 1;
    } else if (!stricmp("& Mace", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TARATOL)
	    return 0;
	if (wizard || peek)
	    msg_print("Taratol");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TARATOL;
	t_ptr->tohit = 12;
	t_ptr->todam = 12;
	t_ptr->weight = 200;
	t_ptr->damage[1] = 7;
	t_ptr->flags1 = (TR1_SLAY_X_DRAGON | TR2_RES_ELEC);
	t_ptr->flags2 |= (TR_ARTIFACT | TR3_LITENING | TR3_ACTIVATE | TR2_RES_DARK);
	t_ptr->cost = 20000L;
	TARATOL = 1;
	return 1;
    } else if (!stricmp("& Lance", name)) {
	if (randint(3) > 1)
	    return 0;
	if (EORLINGAS)
	    return 0;
	if (wizard || peek)
	    msg_print("Lance of Eorlingas");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_EORLINGAS;
	t_ptr->tohit = 3;
	t_ptr->todam = 21;
	t_ptr->weight = 360;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_DEX);
	t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	t_ptr->pval = 2;
	t_ptr->damage[1] = 12;
	t_ptr->cost = 55000L;
	EORLINGAS = 1;
	return 1;
    } else if (!stricmp("& Broad Axe", name)) {
	if (BARUKKHELED)
	    return 0;
	if (wizard || peek)
	    msg_print("Barukkheled");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_BARUKKHELED;
	t_ptr->tohit = 13;
	t_ptr->todam = 19;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_CON);
	t_ptr->flags2 |= (TR1_SLAY_ORC | TR1_SLAY_TROLL | TR_SLAY_MF2_GIANT | TR_ARTIFACT);
	t_ptr->pval = 3;
	t_ptr->cost = 50000L;
	BARUKKHELED = 1;
	return 1;
    } else if (!stricmp("& Trident", name)) {
	switch (randint(3)) {
	  case 1:
	  case 2:
	    if (randint(3) > 1)
		return 0;
	    if (WRATH)
		return 0;
	    if (wizard || peek)
		msg_print("Wrath");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_WRATH;
	    t_ptr->tohit = 16;
	    t_ptr->todam = 18;
	    t_ptr->weight = 300;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 9;
	    t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_STR | TR1_DEX |
			     TR1_SLAY_UNDEAD);
	    t_ptr->flags2 |= (TR2_RES_DARK | TR2_RES_LITE | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->pval = 2;
	    t_ptr->cost = 90000L;
	    WRATH = 1;
	    return 1;
	  case 3:
	    if (randint(4) > 1)
		return 0;
	    if (ULMO)
		return 0;
	    if (wizard || peek)
		msg_print("Ulmo");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ULMO;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 19;
	    t_ptr->damage[0] = 4;
	    t_ptr->damage[1] = 10;
	    t_ptr->flags1 = (TR3_SEE_INVIS | TR2_FREE_ACT | TR1_DEX | TR3_REGEN |
			    TR3_SLOW_DIGEST | TR1_SLAY_ANIMAL | TR1_SLAY_DRAGON |
			    TR2_RES_ACID);
	    t_ptr->flags2 |= (TR2_IM_ACID | TR2_HOLD_LIFE | TR3_ACTIVATE
			    | TR2_RES_NETHER | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->pval = 4;
	    t_ptr->cost = 120000L;
	    ULMO = 1;
	    return 1;
	}
    } else if (!stricmp("& Scimitar", name)) {
	if (HARADEKKET)
	    return 0;
	if (wizard || peek)
	    msg_print("Haradekket");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_HARADEKKET;
	t_ptr->tohit = 9;
	t_ptr->todam = 11;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_DEX | TR1_SLAY_UNDEAD
			 | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_ATTACK_SPD);
	t_ptr->pval = 2;
	t_ptr->cost = 30000L;
	HARADEKKET = 1;
	return 1;
    } else if (!stricmp("& Lochaber Axe", name)) {
	if (MUNDWINE)
	    return 0;
	if (wizard || peek)
	    msg_print("Mundwine");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_MUNDWINE;
	t_ptr->tohit = 12;
	t_ptr->todam = 17;
	t_ptr->flags1 |= (TR1_SLAY_EVIL | TR2_RES_FIRE | TR2_RES_COLD
			 | TR2_RES_ELEC | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 30000L;
	MUNDWINE = 1;
	return 1;
    } else if (!stricmp("& Cutlass", name)) {
	if (GONDRICAM)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondricam");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GONDRICAM;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 11;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR3_FEATHER | TR3_REGEN | TR1_STEALTH | TR2_RES_FIRE |
			 TR2_RES_COLD | TR2_RES_ACID | TR2_RES_ELEC | TR1_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->pval = 3;
	t_ptr->cost = 28000L;
	GONDRICAM = 1;
	return 1;
    } else if (!stricmp("& Sabre", name)) {
	if (CARETH)
	    return 0;
	if (wizard || peek)
	    msg_print("Careth Asdriag");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CARETH;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->flags1 |= (TR1_SLAY_DRAGON | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR1_SLAY_GIANT | TR_SLAY_ORC | TR1_SLAY_TROLL | TR_ARTIFACT |
			  TR1_ATTACK_SPD);
	t_ptr->pval = 1;
	t_ptr->cost = 25000L;
	CARETH = 1;
	return 1;
    } else if (!stricmp("& Rapier", name)) {
	if (FORASGIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Forasgil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_FORASGIL;
	t_ptr->tohit = 12;
	t_ptr->todam = 19;
	t_ptr->flags1 |= (TR2_RES_COLD | TR1_BRAND_COLD | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR3_LITE | TR2_RES_LITE | TR_ARTIFACT);
	t_ptr->cost = 15000L;
	FORASGIL = 1;
	return 1;
    } else if (!stricmp("& Executioner's Sword", name)) {
	if (randint(2) > 1)
	    return 0;
	if (CRISDURIAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Crisdurian");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CRISDURIAN;
	t_ptr->tohit = 18;
	t_ptr->todam = 19;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD | TR1_SLAY_DRAGON);
	t_ptr->flags2 |= (TR1_SLAY_GIANT | TR_SLAY_ORC | TR1_SLAY_TROLL | TR_ARTIFACT);
	t_ptr->cost = 100000L;
	CRISDURIAN = 1;
	return 1;
    } else if (!stricmp("& Flail", name)) {
	if (TOTILA)
	    return 0;
	if (wizard || peek)
	    msg_print("Totila");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TOTILA;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->damage[1] = 9;
	t_ptr->flags1 = (TR1_STEALTH | TR2_RES_FIRE | TR1_BRAND_FIRE | TR1_SLAY_EVIL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR3_ACTIVATE | TR2_RES_CONF);
	t_ptr->pval = 2;
	t_ptr->cost = 55000L;
	TOTILA = 1;
	return 1;
    } else if (!stricmp("& Short sword", name)) {
	if (GILETTAR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gilettar");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GILETTAR;
	t_ptr->tohit = 3;
	t_ptr->todam = 7;
	t_ptr->flags1 = (TR3_REGEN | TR3_SLOW_DIGEST | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_ATTACK_SPD);
	t_ptr->pval = 2;
	t_ptr->cost = 15000L;
	GILETTAR = 1;
	return 1;
    } else if (!stricmp("& Katana", name)) {
	if (randint(3) > 1)
	    return 0;
	if (AGLARANG)
	    return 0;
	if (wizard || peek)
	    msg_print("Aglarang");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AGLARANG;
	t_ptr->tohit = 0;
	t_ptr->todam = 0;
	t_ptr->damage[0] = 6;
	t_ptr->damage[1] = 8;
	t_ptr->weight = 50;
	t_ptr->flags1 = (TR1_DEX | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->pval = 5;
	t_ptr->cost = 40000L;
	AGLARANG = 1;
	return 1;
    } else if (!stricmp("& Spear", name)) {
	switch (randint(6)) {
	  case 1:
	    if (AEGLOS)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglos");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_AEGLOS;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 25;
	    t_ptr->damage[0] = 1;
	    t_ptr->damage[1] = 20;
	    t_ptr->flags1 = (TR1_WIS | TR1_BRAND_COLD |
			    TR2_RES_COLD | TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_ORC | TR3_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->toac = 5;
	    t_ptr->pval = 4;
	    t_ptr->cost = 140000L;
	    AEGLOS = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	    if (NIMLOTH)
		return 0;
	    if (wizard || peek)
		msg_print("Nimloth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NIMLOTH;
	    t_ptr->tohit = 11;
	    t_ptr->todam = 13;
	    t_ptr->flags1 = (TR1_BRAND_COLD | TR2_RES_COLD | TR1_SLAY_UNDEAD |
			    TR3_SEE_INVIS | TR1_STEALTH);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->pval = 3;
	    t_ptr->cost = 30000L;
	    NIMLOTH = 1;
	    return 1;
	  case 6:
	    if (OROME)
		return 0;
	    if (wizard || peek)
		msg_print("Orome");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_OROME;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 15;
	    t_ptr->flags1 = (TR1_BRAND_FIRE | TR3_SEE_INVIS | TR1_SEARCH | TR1_INT |
			    TR2_RES_FIRE | TR3_FEATHER | TR1_INFRA);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR3_LITE | TR1_SLAY_GIANT | TR2_RES_LITE
			      | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->pval = 4;
	    t_ptr->cost = 60000L;
	    OROME = 1;
	    return 1;
	}
    } else if (!stricmp("& Dagger", name)) {
	switch (randint(11)) {
	  case 1:
	    if (ANGRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Angrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANGRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 5;
	    t_ptr->flags1 = (TR1_DEX | TR1_SLAY_EVIL | TR_SUST_STAT |
			    TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_ORC | TR2_RES_DARK | TR_ARTIFACT);
	    t_ptr->toac = 5;
	    t_ptr->pval = 4;
	    t_ptr->cost = 100000L;
	    ANGRIST = 1;
	    return 1;
	  case 2:
	  case 3:
	    if (NARTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Narthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NARTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags1 = (TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 12000;
	    NARTHANC = 1;
	    return 1;
	  case 4:
	  case 5:
	    if (NIMTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Nimthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NIMTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags1 = (TR1_BRAND_COLD | TR2_RES_COLD);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 11000L;
	    NIMTHANC = 1;
	    return 1;
	  case 6:
	  case 7:
	    if (DETHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Dethanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_DETHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags1 = (TR2_RES_ELEC);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR3_LITENING | TR_ARTIFACT);
	    t_ptr->cost = 13000L;
	    DETHANC = 1;
	    return 1;
	  case 8:
	  case 9:
	    if (RILIA)
		return 0;
	    if (wizard || peek)
		msg_print("Rilia");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_RILIA;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 4;
	    t_ptr->flags1 = TR2_RES_POIS;
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR2_RES_DISEN | TR_ARTIFACT);
	    t_ptr->cost = 15000L;
	    RILIA = 1;
	    return 1;
	  case 10:
	  case 11:
	    if (BELANGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Belangil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BELANGIL;
	    t_ptr->tohit = 6;
	    t_ptr->todam = 9;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 2;
	    t_ptr->flags1 = (TR1_BRAND_COLD | TR2_RES_COLD | TR3_REGEN | TR3_SLOW_DIGEST |
			    TR1_DEX | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->pval = 2;
	    t_ptr->cost = 40000L;
	    BELANGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Small sword", name)) {
	if (STING)
	    return 0;
	if (wizard || peek)
	    msg_print("Sting");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_STING;
	t_ptr->tohit = 7;
	t_ptr->todam = 8;
	t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_EVIL |
			 TR1_SLAY_UNDEAD | TR1_DEX | TR1_CON | TR1_STR |
			 TR2_FREE_ACT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR3_LITE | TR2_RES_LITE |
			  TR1_ATTACK_SPD);
	t_ptr->pval = 2;
	t_ptr->cost = 100000L;
	STING = 1;
	return 1;
    } else if (!stricmp("& Great Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(6) > 1)
		return 0;
	    if (DURIN)
		return 0;
	    if (wizard || peek)
		msg_print("Durin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_DURIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->toac = 15;
	    t_ptr->flags1 = (TR1_SLAY_X_DRAGON | TR1_CON | TR2_FREE_ACT |
			    TR2_RES_FIRE | TR2_RES_ACID);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR1_SLAY_TROLL | TR_SLAY_ORC | TR2_RES_DARK
			      | TR2_RES_LITE | TR2_RES_CHAOS | TR_ARTIFACT);
	    t_ptr->pval = 3;
	    t_ptr->cost = 150000L;
	    DURIN = 1;
	    return 1;
	  case 2:
	    if (randint(8) > 1)
		return 0;
	    if (EONWE)
		return 0;
	    if (wizard || peek)
		msg_print("Eonwe");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_EONWE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 18;
	    t_ptr->toac = 8;
	    t_ptr->flags1 = (TR_STATS | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD | TR1_BRAND_COLD |
			    TR2_FREE_ACT | TR3_SEE_INVIS | TR2_RES_COLD);
	    t_ptr->flags2 |= (TR2_IM_COLD | TR1_SLAY_ORC | TR3_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->pval = 2;
	    t_ptr->cost = 200000L;
	    EONWE = 1;
	    return 1;
	}
    } else if (!stricmp("& Battle Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (BALLI)
		return 0;
	    if (wizard || peek)
		msg_print("Balli Stonehand");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BALLI;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 11;
	    t_ptr->damage[0] = 3;
	    t_ptr->damage[1] = 6;
	    t_ptr->toac = 5;
	    t_ptr->flags1 = (TR3_FEATHER | TR2_RES_ELEC | TR3_SEE_INVIS | TR1_STR | TR1_CON
			    | TR2_FREE_ACT | TR2_RES_COLD | TR2_RES_ACID
			    | TR2_RES_FIRE | TR3_REGEN | TR1_STEALTH);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR1_SLAY_TROLL | TR_SLAY_ORC | TR2_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->pval = 3;
	    t_ptr->cost = 90000L;
	    BALLI = 1;
	    return 1;
	  case 2:
	    if (LOTHARANG)
		return 0;
	    if (wizard || peek)
		msg_print("Lotharang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_LOTHARANG;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->flags1 = (TR1_STR | TR1_DEX);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR1_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->pval = 1;
	    t_ptr->cost = 21000L;
	    LOTHARANG = 1;
	    return 1;
	}
    } else if (!stricmp("& War Hammer", name)) {
	if (randint(10) > 1)
	    return 0;
	if (AULE)
	    return 0;
	if (wizard || peek)
	    msg_print("Aule");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AULE;
	t_ptr->damage[0] = 5;
	t_ptr->damage[1] = 5;
	t_ptr->tohit = 19;
	t_ptr->todam = 21;
	t_ptr->toac = 5;
	t_ptr->flags1 = (TR1_SLAY_X_DRAGON | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD |
		    TR2_RES_FIRE | TR2_RES_ACID | TR2_RES_COLD | TR2_RES_ELEC |
			TR2_FREE_ACT | TR3_SEE_INVIS | TR1_WIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_DEMON | TR3_LITENING | TR2_RES_NEXUS);
	t_ptr->pval = 4;
	t_ptr->cost = 250000L;
	AULE = 1;
	return 1;
    } else if (!stricmp("& Beaked Axe", name)) {
	if (randint(2) > 1)
	    return 0;
	if (THEODEN)
	    return 0;
	if (wizard || peek)
	    msg_print("Theoden");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_THEODEN;
	t_ptr->tohit = 8;
	t_ptr->todam = 10;
	t_ptr->flags1 = (TR1_WIS | TR1_CON | TR1_SEARCH | TR3_SLOW_DIGEST | TR1_SLAY_DRAGON);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR3_ACTIVATE | TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->pval = 3;
	t_ptr->cost = 40000L;
	THEODEN = 1;
	return 1;
    } else if (!stricmp("& Two-Handed Great Flail", name)) {
	if (randint(5) > 1)
	    return 0;
	if (THUNDERFIST)
	    return 0;
	if (wizard || peek)
	    msg_print("Thunderfist");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_THUNDERFIST;
	t_ptr->tohit = 5;
	t_ptr->todam = 18;
	t_ptr->flags1 = (TR1_SLAY_ANIMAL | TR1_STR | TR1_BRAND_FIRE |
			TR2_RES_FIRE | TR2_RES_ELEC);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_ORC
			  | TR3_LITENING | TR2_RES_DARK);
	t_ptr->pval = 4;
	t_ptr->cost = 160000L;
	THUNDERFIST = 1;
	return 1;
    } else if (!stricmp("& Morningstar", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(2) > 1)
		return 0;
	    if (BLOODSPIKE)
		return 0;
	    if (wizard || peek)
		msg_print("Bloodspike");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BLOODSPIKE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 22;
	    t_ptr->flags1 = (TR1_SLAY_ANIMAL | TR1_STR | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_ORC | TR2_RES_NEXUS);
	    t_ptr->pval = 4;
	    t_ptr->cost = 30000L;
	    BLOODSPIKE = 1;
	    return 1;
	  case 2:
	    if (FIRESTAR)
		return 0;
	    if (wizard || peek)
		msg_print("Firestar");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_FIRESTAR;
	    t_ptr->tohit = 5;
	    t_ptr->todam = 7;
	    t_ptr->flags1 = (TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->toac = 2;
	    t_ptr->cost = 35000L;
	    FIRESTAR = 1;
	    return 1;
	}
    } else if (!stricmp("& Blade of Chaos", name)) {
	if (DOOMCALLER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Doomcaller");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_DOOMCALLER;
	t_ptr->tohit = 18;
	t_ptr->todam = 28;
	t_ptr->flags1 = (TR1_CON | TR1_SLAY_ANIMAL | TR1_SLAY_X_DRAGON |
		TR1_BRAND_COLD | TR1_SLAY_EVIL | TR2_FREE_ACT | TR3_SEE_INVIS |
		    TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC | TR2_RES_ACID |
			TR3_AGGRAVATE);
	t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_ORC | TR3_TELEPATHY | TR_ARTIFACT);
	t_ptr->pval = -5;
	t_ptr->cost = 200000L;
	DOOMCALLER = 1;
	return 1;
    } else if (!stricmp("& Quarterstaff", name)) {
	switch (randint(7)) {
	  case 1:
	  case 2:
	  case 3:
	    if (NAR)
		return 0;
	    if (wizard || peek)
		msg_print("Nar-i-vagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NAR;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->flags1 = (TR1_INT | TR1_SLAY_ANIMAL | TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->pval = 3;
	    t_ptr->cost = 70000L;
	    NAR = 1;
	    return 1;
	  case 4:
	  case 5:
	  case 6:
	    if (ERIRIL)
		return 0;
	    if (wizard || peek)
		msg_print("Eriril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ERIRIL;
	    t_ptr->tohit = 3;
	    t_ptr->todam = 5;
	    t_ptr->flags1 = (TR1_SLAY_EVIL | TR3_SEE_INVIS | TR1_INT | TR1_WIS);
	    t_ptr->flags2 |= (TR3_LITE | TR3_ACTIVATE | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->pval = 4;
	    t_ptr->cost = 20000L;
	    ERIRIL = 1;
	    return 1;
	  case 7:
	    if (OLORIN)
		return 0;
	    if (randint(2) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Olorin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_OLORIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 13;
	    t_ptr->damage[0] = 2;
	    t_ptr->damage[1] = 10;
	    t_ptr->flags1 = (TR1_SLAY_EVIL | TR3_SEE_INVIS | TR1_WIS | TR1_INT | TR1_CHR
			    | TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR2_HOLD_LIFE | TR1_SLAY_ORC | TR1_SLAY_TROLL
			      | TR3_ACTIVATE | TR2_RES_NETHER);
	    t_ptr->pval = 4;
	    t_ptr->cost = 130000L;
	    OLORIN = 1;
	    return 1;
	}
    }
    return 0;
}

int unique_armour(inven_type *t_ptr)
{
    const char *name;

    name = k_list[t_ptr->index].name;
    if (!strncmp("Adamantite", name, 10)) {
	if (SOULKEEPER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Soulkeeper");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_COLD);
	t_ptr->flags2 |= (TR2_HOLD_LIFE | TR3_ACTIVATE | TR2_RES_CHAOS | TR2_RES_DARK |
			  TR2_RES_NEXUS | TR2_RES_NETHER | TR_ARTIFACT);
	t_ptr->name2 = ART_SOULKEEPER;
	t_ptr->toac = 20;
	t_ptr->cost = 300000L;
	SOULKEEPER = 1;
	return 1;
    }
     /* etc..... */ 
    else if (!strncmp("Multi-Hued", name, 10)) {
	if (RAZORBACK)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Razorback");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ACID | TR2_RES_POIS |
			 TR2_RES_ELEC | TR2_FREE_ACT | TR3_SEE_INVIS | TR1_INT |
			 TR1_WIS | TR1_STEALTH | TR3_AGGRAVATE);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR3_LITE | TR2_IM_ELEC | TR2_RES_LITE |
			  TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 25;
	t_ptr->pval = -2;
	t_ptr->weight = 400;
	t_ptr->ac = 30;
	t_ptr->tohit = -3;
	t_ptr->cost = 400000L;
	t_ptr->name2 = ART_RAZORBACK;
	RAZORBACK = 1;
	return 1;
    } else if (!strncmp("Power Drag", name, 10)) {
	if (BLADETURNER)
	    return 0;
	if (wizard || peek)
	    msg_print("Bladeturner");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ACID | TR2_RES_POIS |
			 TR2_RES_ELEC | TR1_DEX | TR1_SEARCH | TR3_REGEN);
	t_ptr->flags2 |= (TR2_HOLD_LIFE | TR2_RES_CONF | TR2_RES_SOUND | TR2_RES_LITE
	      | TR2_RES_DARK | TR2_RES_CHAOS | TR2_RES_DISEN | TR_ARTIFACT
	       | TR2_RES_SHARDS | TR2_RES_BLIND | TR2_RES_NEXUS | TR2_RES_NETHER
			  | TR3_ACTIVATE);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 35;
	t_ptr->pval = -3;
	t_ptr->ac = 50;
	t_ptr->tohit = -4;
	t_ptr->weight = 500;
	t_ptr->cost = 500000L;
	t_ptr->name2 = ART_BLADETURNER;
	BLADETURNER = 1;
	return 1;
    } else if (!stricmp("& Pair of Hard Leather Boots", name)) {
	if (FEANOR)
	    return 0;
	if (randint(5) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Feanor");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR1_SPEED | TR1_STEALTH);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR2_RES_NEXUS | TR_ARTIFACT);
	t_ptr->name2 = ART_FEANOR;
	t_ptr->pval = 1;
	t_ptr->toac = 20;
	t_ptr->cost = 130000L;
	FEANOR = 1;
	return 1;
    } else if (!stricmp("& Pair of Soft Leather Boots", name)) {
	if (DAL)
	    return 0;
	if (wizard || peek)
	    msg_print("Dal-i-thalion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_FREE_ACT | TR1_DEX | TR_SUST_STAT | TR2_RES_ACID);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT | TR2_RES_NETHER | TR2_RES_CHAOS);
	t_ptr->name2 = ART_DAL;
	t_ptr->pval = 5;
	t_ptr->toac = 15;
	t_ptr->cost = 40000L;
	DAL = 1;
	return 1;
    } else if (!stricmp("& Small Metal Shield", name)) {
	if (THORIN)
	    return 0;
	if (randint(2) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Thorin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR1_CON | TR2_FREE_ACT | TR1_STR |
			 TR2_RES_ACID | TR1_SEARCH);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR2_RES_CHAOS | TR_ARTIFACT | TR2_IM_ACID);
	t_ptr->name2 = ART_THORIN;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 0;
	t_ptr->pval = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 60000L;
	THORIN = 1;
	return 1;
    } else if (!stricmp("Full Plate Armour", name)) {
	if (ISILDUR)
	    return 0;
	if (wizard || peek)
	    msg_print("Isildur");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 300;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR_ARTIFACT | TR2_RES_NEXUS);
	t_ptr->name2 = ART_ISILDUR;
	t_ptr->tohit = 0;
	t_ptr->toac = 25;
	t_ptr->cost = 40000L;
	ISILDUR = 1;
	return 1;
    } else if (!stricmp("Metal Brigandine Armour", name)) {
	if (ROHAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Rohirrim");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 200;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			 TR1_STR | TR1_DEX);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR2_RES_CONF | TR_ARTIFACT);
	t_ptr->name2 = EGO_ROHAN;
	t_ptr->tohit = 0;
	t_ptr->pval = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 30000L;
	ROHAN = 1;
	return 1;
    } else if (!stricmp("& Large Metal Shield", name)) {
	if (ANARION)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	else
	    good_item_flag = TRUE;
	if (wizard || peek)
	    msg_print("Anarion");
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			 TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_ANARION;
	t_ptr->pval = 10;
	t_ptr->ident |= ID_NOSHOW_P1;
	t_ptr->tohit = 0;
	t_ptr->toac = 20;
	t_ptr->cost = 160000L;
	ANARION = 1;
	return 1;
    } else if (!stricmp("& Set of Cesti", name)) {
	if (FINGOLFIN)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Fingolfin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR1_DEX | TR2_FREE_ACT);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_FINGOLFIN;
	t_ptr->ident |= ID_SHOW_HITDAM;
	t_ptr->pval = 4;
	t_ptr->tohit = 10;
	t_ptr->todam = 10;
	t_ptr->toac = 20;
	t_ptr->cost = 110000L;
	FINGOLFIN = 1;
	return 1;
    } else if (!stricmp("& Set of Leather Gloves", name)) {
	if (randint(3) == 1) {
	    if (CAMBELEG)
		return 0;
	    if (wizard || peek)
		msg_print("Cambeleg");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR1_STR | TR1_CON | TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMBELEG;
	    t_ptr->ident |= ID_SHOW_HITDAM;
	    t_ptr->pval = 2;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 8;
	    t_ptr->toac = 15;
	    t_ptr->cost = 36000L;
	    CAMBELEG = 1;
	    return 1;
	} else {
	    if (CAMMITHRIM)
		return 0;
	    if (wizard || peek)
		msg_print("Cammithrim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR_SUST_STAT | TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR3_LITE | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMMITHRIM;
	    t_ptr->ident |= ID_NOSHOW_P1;
	    t_ptr->pval = 5;
	    t_ptr->toac = 10;
	    t_ptr->cost = 30000L;
	    CAMMITHRIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Set of Gauntlets", name)) {
	switch (randint(6)) {
	  case 1:
	    if (PAURHACH)
		return 0;
	    if (wizard || peek)
		msg_print("Paurhach");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= TR2_RES_FIRE;
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURHACH;
	    t_ptr->toac = 15;
	    t_ptr->cost = 15000L;
	    PAURHACH = 1;
	    return 1;
	  case 2:
	    if (PAURNIMMEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnimmen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= TR2_RES_COLD;
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURNIMMEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 13000L;
	    PAURNIMMEN = 1;
	    return 1;
	  case 3:
	    if (PAURAEGEN)
		return 0;
	    if (wizard || peek)
		msg_print("Pauraegen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= TR2_RES_ELEC;
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURAEGEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 11000L;
	    PAURAEGEN = 1;
	    return 1;
	  case 4:
	    if (PAURNEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= TR2_RES_ACID;
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURNEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 12000L;
	    PAURNEN = 1;
	    return 1;
	  default:
	    if (CAMLOST)
		return 0;
	    if (wizard || peek)
		msg_print("Camlost");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR1_STR | TR1_DEX | TR3_AGGRAVATE | TR3_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMLOST;
	    t_ptr->toac = 0;
	    t_ptr->pval = -5;
	    t_ptr->tohit = -11;
	    t_ptr->todam = -12;
	    t_ptr->ident |= (ID_SHOW_HITDAM/* | ID_SHOW_P1*/);
	    t_ptr->cost = 0L;
	    CAMLOST = 1;
	    return 1;
	}
    } else if (!stricmp("Mithril Chain Mail", name)) {
	if (BELEGENNON)
	    return 0;
	if (wizard || peek)
	    msg_print("Belegennon");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD |
			 TR2_RES_ELEC | TR1_STEALTH);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_BELEGENNON;
	t_ptr->pval = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 105000L;
	BELEGENNON = 1;
	return 1;
    } else if (!stricmp("Mithril Plate Mail", name)) {
	if (CELEBORN)
	    return 0;
	if (wizard || peek)
	    msg_print("Celeborn");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 250;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			 TR1_STR | TR1_CHR);
	t_ptr->flags2 |= (TR3_ACTIVATE | TR2_RES_DISEN | TR2_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = ART_CELEBORN;
	t_ptr->pval = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 150000L;
	CELEBORN = 1;
	return 1;
    } else if (!stricmp("Augmented Chain Mail", name)) {
	if (randint(3) > 1)
	    return 0;
	if (CASPANION)
	    return 0;
	if (wizard || peek)
	    msg_print("Caspanion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_POIS | TR1_CON | TR1_WIS | TR1_INT);
	t_ptr->flags2 |= (TR2_RES_CONF | TR3_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_CASPANION;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 40000L;
	CASPANION = 1;
	return 1;
    } else if (!stricmp("Soft Leather Armour", name)) {
	if (HITHLOMIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Hithlomir");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			 TR1_STEALTH);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_DARK);
	t_ptr->name2 = ART_HITHLOMIR;
	t_ptr->pval = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HITHLOMIR = 1;
	return 1;
    } else if (!stricmp("Leather Scale Mail", name)) {
	if (THALKETTOTH)
	    return 0;
	if (wizard || peek)
	    msg_print("Thalkettoth");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags1 |= (TR2_RES_ACID | TR1_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_SHARDS);
	t_ptr->name2 = ART_THALKETTOTH;
	t_ptr->toac = 25;
	t_ptr->pval = 3;
	t_ptr->cost = 25000L;
	THALKETTOTH = 1;
	return 1;
    } else if (!stricmp("Chain Mail", name)) {
	if (ARVEDUI)
	    return 0;
	if (wizard || peek)
	    msg_print("Arvedui");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			 TR1_STR | TR1_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_NEXUS | TR2_RES_SHARDS);
	t_ptr->name2 = ART_ARVEDUI;
	t_ptr->pval = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 32000L;
	ARVEDUI = 1;
	return 1;
    } else if (!stricmp("& Hard Leather Cap", name)) {
	if (THRANDUIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thranduil");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR1_INT | TR1_WIS);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR2_RES_BLIND | TR_ARTIFACT);
	t_ptr->name2 = ART_THRANDUIL;
	t_ptr->pval = 2;
	t_ptr->toac = 10;
	t_ptr->cost = 50000L;
	THRANDUIL = 1;
	return 1;
    } else if (!stricmp("& Metal Cap", name)) {
	if (THENGEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thengel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR2_RES_ACID | TR1_WIS | TR1_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THENGEL;
	t_ptr->pval = 3;
	t_ptr->toac = 12;
	t_ptr->cost = 22000L;
	THENGEL = 1;
	return 1;
    } else if (!stricmp("& Steel Helm", name)) {
	if (HAMMERHAND)
	    return 0;
	if (wizard || peek)
	    msg_print("Hammerhand");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR1_STR | TR1_CON | TR1_DEX | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_NEXUS);
	t_ptr->name2 = ART_HAMMERHAND;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HAMMERHAND = 1;
	return 1;
    } else if (!stricmp("& Large Leather Shield", name)) {
	if (CELEGORM)
	    return 0;
	if (wizard || peek)
	    msg_print("Celegorm");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC);
	t_ptr->flags2 |= (TR2_RES_LITE | TR2_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = ART_CELEGORM;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	CELEGORM = 1;
	return 1;
    } else if (!stricmp("& Pair of Metal Shod Boots", name)) {
	if (THROR)
	    return 0;
	if (wizard || peek)
	    msg_print("Thror");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR1_CON | TR1_STR | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THROR;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	THROR = 1;
	return 1;
    } else if (!stricmp("& Iron Helm", name)) {
	if (randint(6) == 1) {
	    if (DOR_LOMIN)
		return 0;
	    if (wizard || peek)
		msg_print("Dor-Lomin");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR2_RES_ACID | TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
			     TR1_CON | TR1_DEX | TR1_STR | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR3_TELEPATHY | TR3_LITE | TR2_RES_LITE | TR2_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->name2 = ART_DOR_LOMIN;
	    t_ptr->pval = 4;
	    t_ptr->toac = 20;
	    t_ptr->cost = 300000L;
	    DOR_LOMIN = 1;
	    return 1;
	} else if (randint(2) == 1) {
	    if (HOLHENNETH)
		return 0;
	    if (wizard || peek)
		msg_print("Holhenneth");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR1_INT | TR1_WIS | TR3_SEE_INVIS | TR1_SEARCH | TR2_RES_ACID);
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR2_RES_BLIND | TR_ARTIFACT);
	    t_ptr->name2 = ART_HOLHENNETH;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->pval = 2;
	    t_ptr->toac = 10;
	    t_ptr->cost = 100000L;
	    HOLHENNETH = 1;
	    return 1;
	} else {
	    if (GORLIM)
		return 0;
	    if (wizard || peek)
		msg_print("Gorlim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags1 |= (TR1_INT | TR1_WIS | TR3_SEE_INVIS | TR1_SEARCH | TR3_CURSED
			     | TR3_AGGRAVATE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_GORLIM;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->pval = -125;
	    t_ptr->toac = 10;
	    t_ptr->cost = 0L;
	    GORLIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Golden Crown", name)) {
	if (randint(3) > 1)
	    return 0;
	if (GONDOR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondor");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GONDOR;
	t_ptr->flags1 = (TR1_STR | TR1_CON | TR1_WIS | TR3_SEE_INVIS | TR3_REGEN
			| TR2_RES_ACID | TR2_RES_FIRE);
	t_ptr->flags2 = (TR3_ACTIVATE | TR3_LITE | TR2_RES_LITE | TR2_RES_BLIND |
			 TR_ARTIFACT);
	t_ptr->pval = 3;
	t_ptr->toac = 15;
	t_ptr->cost = 100000L;
	GONDOR = 1;
	return 1;
    } else if (!stricmp("& Iron Crown", name)) {
	if (BERUTHIEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Beruthiel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags1 |= (TR1_STR | TR1_DEX | TR1_CON |
		      TR2_RES_ACID | TR3_SEE_INVIS | TR2_FREE_ACT | TR3_CURSED);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR_ARTIFACT);
	t_ptr->name2 = ART_BERUTHIEL;
	t_ptr->pval = -125;
	t_ptr->toac = 20;
	t_ptr->cost = 10000L;
	BERUTHIEL = 1;
	return 1;
    }
    return 0;
}

void give_1_hi_resist(register inven_type *t)		   /* JLS: gives one of the "new" resistances to */
{
    switch (randint(10)) {
      case 1:
	t->flags2 |= TR2_RES_CONF;
	break;
      case 2:
	t->flags2 |= TR2_RES_SOUND;
	break;
      case 3:
	t->flags2 |= TR2_RES_LITE;
	break;
      case 4:
	t->flags2 |= TR2_RES_DARK;
	break;
      case 5:
	t->flags2 |= TR2_RES_CHAOS;
	break;
      case 6:
	t->flags2 |= TR2_RES_DISEN;
	break;
      case 7:
	t->flags2 |= TR2_RES_SHARDS;
	break;
      case 8:
	t->flags2 |= TR2_RES_NEXUS;
	break;
      case 9:
	t->flags2 |= TR2_RES_BLIND;
	break;
      case 10:
	t->flags2 |= TR2_RES_NETHER;
	break;
    }
}

/* Chance of treasure having magic abilities		-RAK-	 */
/* Chance increases with each dungeon level			 */
void magic_treasure(int x, int level, int good, int not_unique)
{
    register inven_type *t_ptr;
    register u32b      chance, special, cursed, i;
    u32b               tmp;

    chance = OBJ_BASE_MAGIC + level;
    if (chance > OBJ_BASE_MAX)
	chance = OBJ_BASE_MAX;
    special = chance / OBJ_DIV_SPECIAL;
    cursed = (10 * chance) / OBJ_DIV_CURSED;
    t_ptr = &i_list[x];

/*
 * some objects appear multiple times in the k_list with different
 * levels, this is to make the object occur more often, however, for
 * consistency, must set the level of these duplicates to be the same as the
 * object with the lowest level 
 */

/* Depending on treasure type, it can have certain magical properties */
    switch (t_ptr->tval) {
      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:
	if ((t_ptr->index >= 389 && t_ptr->index <= 394)
	    || (t_ptr->index >= 408 && t_ptr->index <= 409)
	    || (t_ptr->index >= 415 && t_ptr->index <= 419)) {

	    byte               artifact = FALSE;

	/* all DSM are enchanted, I guess -CFT */
	    t_ptr->toac += m_bonus(0, 5, level) + randint(5);
	    rating += 30;
	    if ((magik(chance) && magik(special)) || (good == 666)) {
		t_ptr->toac += randint(5);	/* even better... */
		if ((randint(3) == 1 || good == 666) && !not_unique
		    && unique_armour(t_ptr))	/* ...but is it an artifact? */
		    artifact = TRUE;
	    }
	    if (!artifact) {	   /* assume cost&mesg done if it was an
				    * artifact */
		if (wizard || peek)
		    msg_print("Dragon Scale Mail");
		t_ptr->cost += ((s32b) t_ptr->toac * 500L);
	    }
	}
	 /* end if is a DSM */ 
	else if (magik(chance) || good) {
	    t_ptr->toac += randint(3) + m_bonus(0, 5, level);
	    if (!stricmp(k_list[t_ptr->index].name, "& Robe") &&
		((magik(special) && randint(30) == 1)
		 || (good == 666 && magik(special)))) {
		t_ptr->flags1 |= (TR2_RES_ELEC | TR2_RES_COLD | TR2_RES_ACID |
				 TR2_RES_FIRE | TR_SUST_STAT);
		if (wizard || peek)
		    msg_print("Robe of the Magi");
		rating += 30;
		t_ptr->flags2 |= TR2_HOLD_LIFE;
		t_ptr->ident |= ID_NOSHOW_P1;
		give_1_hi_resist(t_ptr);	/* JLS */
		t_ptr->pval = 10;
		t_ptr->toac += 10 + randint(5);
		t_ptr->name2 = EGO_MAGI;
		t_ptr->cost = 10000L + (t_ptr->toac * 100);
	    } else if (magik(special) || good == 666)
		switch (randint(9)) {
		  case 1:
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    t_ptr->flags1 |= (TR2_RES_ELEC | TR2_RES_COLD | TR2_RES_ACID |
				     TR2_RES_FIRE);
		    if (randint(3) == 1) {
			if (peek)
			    msg_print("Elvenkind");
			rating += 25;
			give_1_hi_resist(t_ptr);	/* JLS */
			t_ptr->flags1 |= TR1_STEALTH;
			t_ptr->pval = randint(3);
			t_ptr->name2 = EGO_ELVENKIND;
			t_ptr->toac += 15;
			t_ptr->cost += 15000L;
		    } else {
			if (peek)
			    msg_print("Resist");
			rating += 20;
			t_ptr->name2 = EGO_R;
			t_ptr->toac += 8;
			t_ptr->cost += 12500L;
		    }
		    break;
		  case 2:	   /* Resist Acid	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (!strncmp(k_list[t_ptr->index].name,
				 "Mithril", 7) ||
			!strncmp(k_list[t_ptr->index].name,
				 "Adamantite", 10))
			break;
		    if (peek)
			msg_print("Resist Acid");
		    rating += 15;
		    t_ptr->flags1 |= TR2_RES_ACID;
		    t_ptr->name2 = EGO_RESIST_A;
		    t_ptr->cost += 1000L;
		    break;
		  case 3:
		  case 4:	   /* Resist Fire	  */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Resist Fire");
		    rating += 17;
		    t_ptr->flags1 |= TR2_RES_FIRE;
		    t_ptr->name2 = EGO_RESIST_F;
		    t_ptr->cost += 600L;
		    break;
		  case 5:
		  case 6:	   /* Resist Cold	 */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Resist Cold");
		    rating += 16;
		    t_ptr->flags1 |= TR2_RES_COLD;
		    t_ptr->name2 = EGO_RESIST_C;
		    t_ptr->cost += 600L;
		    break;
		  case 7:
		  case 8:
		  case 9:	   /* Resist Lightning */
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Resist Lightning");
		    rating += 15;
		    t_ptr->flags1 |= TR2_RES_ELEC;
		    t_ptr->name2 = EGO_RESIST_E;
		    t_ptr->cost += 500L;
		    break;
		}
	} else if (magik(cursed)) {
	    t_ptr->toac = -randint(3) - m_bonus(0, 10, level);
	    t_ptr->cost = 0L;
	    t_ptr->flags1 |= TR3_CURSED;
	}
	break;

      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    t_ptr->tohit += randint(3) + m_bonus(0, 10, level);
	    t_ptr->todam += randint(3) + m_bonus(0, 10, level);
	/*
	 * the 3*special/2 is needed because weapons are not as common as
	 * before change to treasure distribution, this helps keep same
	 * number of ego weapons same as before, see also missiles 
	 */
	    if (magik(3*special/2)||good==666) { /* was 2 */
		if (!stricmp("& Whip", k_list[t_ptr->index].name)
		    && randint(2)==1) {
		    if (peek) msg_print("Whip of Fire");
		    rating += 20;
		    t_ptr->name2 = EGO_FIRE;
		    t_ptr->flags1 |= (TR1_BRAND_FIRE | TR2_RES_FIRE);
		    /* this should allow some WICKED whips -CFT */
		    while (randint(5*(int)t_ptr->damage[0])==1) {
			t_ptr->damage[0]++;
			t_ptr->cost += 2500;
			t_ptr->cost *= 2;
		    }
		    t_ptr->tohit += 5;
		    t_ptr->todam += 5;
		} else {
		    switch (randint(30)) {	/* was 16 */
		      case 1:	   /* Holy Avenger	 */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Holy Avenger");
			rating += 30;
			t_ptr->flags1 |= (TR3_SEE_INVIS | TR_SUST_STAT |
				      TR1_SLAY_UNDEAD | TR1_SLAY_EVIL | TR1_WIS);
			t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_BLESS_BLADE);
			t_ptr->tohit += 5;
			t_ptr->todam += 5;
			t_ptr->toac += randint(4);
		    /* the value in pval is used for strength increase */
		    /* pval is also used for sustain stat */
			t_ptr->pval = randint(4);
			t_ptr->name2 = EGO_HA;
			t_ptr->cost += t_ptr->pval * 500;
			t_ptr->cost += 10000L;
			t_ptr->cost *= 2;
			break;
		      case 2:	   /* Defender	 */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Defender");
			rating += 23;
			t_ptr->flags1 |= (TR3_FEATHER | TR2_RES_ELEC | TR3_SEE_INVIS
				   | TR2_FREE_ACT | TR2_RES_COLD | TR2_RES_ACID
				     | TR2_RES_FIRE | TR3_REGEN | TR1_STEALTH);
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->toac += 5 + randint(5);
			t_ptr->name2 = EGO_DF;
		    /* the value in pval is used for stealth */
			t_ptr->pval = randint(3);
			t_ptr->cost += t_ptr->pval * 500;
			t_ptr->cost += 7500L;
			t_ptr->cost *= 2;
			break;
		      case 3:
		      case 4:	   /* Flame Tongue  */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			rating += 20;
			t_ptr->flags1 |= (TR1_BRAND_FIRE | TR2_RES_FIRE);
			if (peek)
			    msg_print("Flame");
			t_ptr->tohit += 2;
			t_ptr->todam += 3;
			t_ptr->name2 = EGO_FT;
			t_ptr->cost += 3000L;
			break;
		      case 5:
		      case 6:	   /* Frost Brand   */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Frost");
			rating += 20;
			t_ptr->flags1 |= (TR1_BRAND_COLD | TR2_RES_COLD);
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_FB;
			t_ptr->cost += 2200L;
			break;
		      case 7:
		      case 8:	   /* Slay Animal  */
			t_ptr->flags1 |= TR1_SLAY_ANIMAL;
			rating += 15;
			if (peek)
			    msg_print("Slay Animal");
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = EGO_SLAY_A;
			t_ptr->cost += 2000L;
			break;
		      case 9:
		      case 10:	   /* Slay Dragon	 */
			t_ptr->flags1 |= TR1_SLAY_DRAGON;
			if (peek)
			    msg_print("Slay Dragon");
			rating += 18;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = EGO_SLAY_D;
			t_ptr->cost += 4000L;
			break;
		      case 11:
		      case 12:	   /* Slay Evil   */
			t_ptr->flags1 |= TR1_SLAY_EVIL;
			if (randint(3) == 1) {
			    t_ptr->flags1 |= (TR1_WIS);
			    t_ptr->flags2 |= (TR_BLESS_BLADE);
			    t_ptr->pval = m_bonus(0, 3, level);
			    t_ptr->cost += (200 * t_ptr->pval);
			}
			if (peek)
			    msg_print("Slay Evil");
			rating += 18;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->name2 = EGO_SLAY_E;
			t_ptr->cost += 4000L;
			break;
		      case 13:
		      case 14:	   /* Slay Undead	  */
			t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SLAY_UNDEAD);
			if (randint(3) == 1) {
			    t_ptr->flags2 |= (TR2_HOLD_LIFE);
			    t_ptr->cost += 1000;
			}
			if (peek)
			    msg_print("Slay Undead");
			rating += 18;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_SLAY_U;
			t_ptr->cost += 3000L;
			break;
		      case 15:
		      case 16:
		      case 17:	   /* Slay Orc */
			t_ptr->flags2 |= TR1_SLAY_ORC;
			if (peek)
			    msg_print("Slay Orc");
			rating += 13;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_SLAY_O;
			t_ptr->cost += 1200L;
			break;
		      case 18:
		      case 19:
		      case 20:	   /* Slay Troll */
			t_ptr->flags2 |= TR1_SLAY_TROLL;
			if (peek)
			    msg_print("Slay Troll");
			rating += 13;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_SLAY_T;
			t_ptr->cost += 1200L;
			break;
		      case 21:
		      case 22:
		      case 23:
			t_ptr->flags2 |= TR1_SLAY_GIANT;
			if (peek)
			    msg_print("Slay Giant");
			rating += 14;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_SLAY_G;
			t_ptr->cost += 1200L;
			break;
		      case 24:
		      case 25:
		      case 26:
			t_ptr->flags2 |= TR1_SLAY_DEMON;
			if (peek)
			    msg_print("Slay Demon");
			rating += 16;
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_SLAY_DEMON;
			t_ptr->cost += 1200L;
			break;
		      case 27:	   /* of Westernesse */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			if (peek)
			    msg_print("Westernesse");
			rating += 20;
			t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_DEX | TR1_CON | TR1_STR |
					 TR2_FREE_ACT);
			t_ptr->flags2 |= TR1_SLAY_ORC;
			t_ptr->tohit += randint(5) + 3;
			t_ptr->todam += randint(5) + 3;
			t_ptr->pval = 1;
			t_ptr->cost += 10000L;
			t_ptr->cost *= 2;
			t_ptr->name2 = EGO_WEST;
			break;
		      case 28:
		      case 29:	   /* Blessed Blade -DGK */
			if ((t_ptr->tval != TV_SWORD) &&
			    (t_ptr->tval != TV_POLEARM))
			    break;
			if (peek)
			    msg_print("Blessed");
			rating += 20;
			t_ptr->flags1 = TR1_WIS;
			t_ptr->flags2 = TR_BLESS_BLADE;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->pval = randint(3);
			t_ptr->name2 = EGO_BLESS_BLADE;
			t_ptr->cost += t_ptr->pval * 1000;
			t_ptr->cost += 3000L;
			break;
		      case 30:	   /* of Speed -DGK */
			if (((randint(2) == 1) || (good == 666))
			    && !not_unique && unique_weapon(t_ptr))
			    break;
			if (wizard || peek)
			    msg_print("Weapon of Extra Attacks");
			rating += 20;
			t_ptr->tohit += randint(5);
			t_ptr->todam += randint(3);
			t_ptr->flags2 = TR1_ATTACK_SPD;
			if (t_ptr->weight <= 80)
			    t_ptr->pval = randint(3);
			else if (t_ptr->weight <= 130)
			    t_ptr->pval = randint(2);
			else
			    t_ptr->pval = 1;
			t_ptr->name2 = EGO_ATTACKS;
			t_ptr->cost += (t_ptr->pval * 2000);
			t_ptr->cost *= 2;
			break;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    t_ptr->tohit = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->todam = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->flags1 |= TR3_CURSED;
	    if (level > (20 + randint(15)) && randint(10) == 1) {
		t_ptr->name2 = EGO_MORGUL;
		t_ptr->flags1 |= (TR3_SEE_INVIS | TR3_AGGRAVATE);
		t_ptr->tohit -= 15;
		t_ptr->todam -= 15;
		t_ptr->toac = -10;
		t_ptr->weight += 100;
	    }
	    t_ptr->cost = 0L;
	}
	break;
      case TV_BOW:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || good) {
	    t_ptr->tohit = randint(3) + m_bonus(0, 10, level);
	    t_ptr->todam = randint(3) + m_bonus(0, 10, level);
	    switch (randint(15)) {
	      case 1: case 2: case 3:
		if (((randint(3)==1)||(good==666)) && !not_unique &&
		    !stricmp(k_list[t_ptr->index].name, "& Long Bow") &&
		    (((i=randint(2))==1 && !BELEG) || (i==2 && !BARD))) {
		    switch (i) {
		    case 1:
			  if (BELEG)
			    break;
			if (wizard || peek)
			    msg_print("Belthronding");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = EGO_BELEG;
			t_ptr->ident |= ID_NOSHOW_TYPE;
			t_ptr->sval = 4; /* make do x5 damage!! -CFT */
			t_ptr->tohit = 20;
			t_ptr->todam = 22;
			t_ptr->pval = 3;
			t_ptr->flags1 |= (TR1_STEALTH | TR1_DEX);
			t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_DISEN);
			t_ptr->cost = 35000L;
			BELEG = 1;
			break;
		      case 2:
			if (BARD)
			    break;
			if (wizard || peek)
			    msg_print("Bard");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_BARD;
			t_ptr->sval = 3; /* make do x4 damage!! -CFT */
			t_ptr->tohit = 17;
			t_ptr->todam = 19;
			t_ptr->pval = 3;
			t_ptr->flags1 |= (TR2_FREE_ACT | TR1_DEX);
			t_ptr->flags2 |= (TR_ARTIFACT);
			t_ptr->cost = 20000L;
			BARD = 1;
			break;
		    }
		    break;
		}
		if (((randint(5) == 1) || (good == 666)) && !not_unique &&
		    !stricmp(k_list[t_ptr->index].name, "& Light Crossbow")
		    && !CUBRAGOL) {
		    if (CUBRAGOL)
			break;
		    if (wizard || peek)
			msg_print("Cubragol");
		    t_ptr->name2 = ART_CUBRAGOL;
		    t_ptr->sval = 11;
		    t_ptr->tohit = 10;
		    t_ptr->todam = 14;
		    t_ptr->pval = 1;
		    t_ptr->flags1 |= (TR1_SPEED | TR2_RES_FIRE);
		    t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
		    t_ptr->cost = 38000L;
		    CUBRAGOL = 1;
		    break;
		}
		t_ptr->name2 = EGO_MIGHT;
		if (peek)
		    msg_print("Bow of Might");
		rating += 15;
		t_ptr->sval++; /* make it do an extra multiple of damage */
		t_ptr->tohit += 5;
		t_ptr->todam += 10;
		break;
	      case 4: case 5: case 6: case 7: case 8:
		t_ptr->name2 = EGO_MIGHT;
		if (peek) msg_print("Bow of Might");
		rating += 11;
		t_ptr->tohit += 5;
		t_ptr->todam += 12;
		break;

	      case 9: case 10: case 11: case 12:
	      case 13: case 14: case 15:
		t_ptr->name2 = EGO_ACCURACY;
		rating += 11;
		if (peek)
		    msg_print("Accuracy");
		t_ptr->tohit += 12;
		t_ptr->todam += 5;
		break;
	    }
	} else if (magik(cursed)) {
	    t_ptr->tohit = (-m_bonus(5, 30, level));
	    t_ptr->todam = (-m_bonus(5, 20, level));	/* add damage. -CJS- */
	    t_ptr->flags1 |= TR3_CURSED;
	    t_ptr->cost = 0L;
	}
	break;

      case TV_DIGGING:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || (good == 666)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->pval += m_bonus(0, 5, level);
	    }
	    if (tmp == 2)	/* do not give additional plusses -CWS */
		;
	    else {
	    /* a cursed digging tool */
		t_ptr->pval = (-m_bonus(1, 15, level));
		t_ptr->cost = 0L;
		t_ptr->flags1 |= TR3_CURSED;
	    }
	}
	break;

      case TV_GLOVES:
	if (magik(chance) || good) {
	    t_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if ((((randint(2) == 1) && magik(5 * special / 2)) || (good == 666)) &&
		!stricmp(k_list[t_ptr->index].name,
			 "& Set of Leather Gloves") &&
		!not_unique && unique_armour(t_ptr));
	    else if ((((randint(4) == 1) && magik(special)) || (good == 666))
		     && !stricmp(k_list[t_ptr->index].name,
				 "& Set of Gauntlets") &&
		     !not_unique && unique_armour(t_ptr));
	    else if ((((randint(5) == 1) && magik(special)) || (good == 666))
		     && !stricmp(k_list[t_ptr->index].name,
				 "& Set of Cesti") &&
		     !not_unique && unique_armour(t_ptr));
	/* don't forget cesti -CFT */
	    else if (magik(special) || (good == 666)) {
		switch (randint(10)) {
		  case 1:
		  case 2:
		  case 3:
		    if (peek)
			msg_print("Free action");
		    rating += 11;
		    t_ptr->flags1 |= TR2_FREE_ACT;
		    t_ptr->name2 = EGO_FREE_ACTION;
		    t_ptr->cost += 1000L;
		    break;
		  case 4:
		  case 5:
		  case 6:
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    rating += 17;
		    if (peek)
			msg_print("Slaying");
		    t_ptr->tohit += 1 + randint(4);
		    t_ptr->todam += 1 + randint(4);
		    t_ptr->name2 = EGO_SLAYING;
		    t_ptr->cost += (t_ptr->tohit + t_ptr->todam) * 250;
		    break;
		  case 7:
		  case 8:
		  case 9:
		    t_ptr->name2 = EGO_AGILITY;
		    if (peek)
			msg_print("Agility");
		    rating += 14;
		    t_ptr->pval = 2 + randint(2);
		    t_ptr->flags1 |= TR1_DEX;
		    t_ptr->cost += (t_ptr->pval) * 400;
		    break;
		  case 10:
		    if (((randint(3) == 1) || (good == 666)) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Power");
		    rating += 22;
		    t_ptr->name2 = ART_POWER;
		    t_ptr->pval = 1 + randint(4);
		    t_ptr->tohit += 1 + randint(4);
		    t_ptr->todam += 1 + randint(4);
		    t_ptr->flags1 |= TR1_STR;
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    t_ptr->ident |= ID_NOSHOW_TYPE;
		    t_ptr->cost += (t_ptr->tohit + t_ptr->todam + t_ptr->pval) * 300;
		    break;
		}
	    }
	} else if (magik(cursed)) {
	    if (magik(special)) {
		if (randint(2) == 1) {
		    t_ptr->flags1 |= TR1_DEX;
		    t_ptr->name2 = EGO_CLUMSINESS;
		} else {
		    t_ptr->flags1 |= TR1_STR;
		    t_ptr->name2 = EGO_WEAKNESS;
		}
		t_ptr->pval = (randint(3) - m_bonus(0, 10, level));
	    }
	    t_ptr->toac = (-m_bonus(1, 20, level));
	    t_ptr->flags1 |= TR3_CURSED;
	    t_ptr->cost = 0;
	}
	break;

      case TV_BOOTS:
	if (magik(chance) || good) {
	    t_ptr->toac = randint(3) + m_bonus(1, 10, level);
	    if (magik(special) || (good == 666)) {
		tmp = randint(12);
		if (tmp == 1) {
		    if (!((randint(2) == 1) && !not_unique
			  && unique_armour(t_ptr))) {
			t_ptr->flags1 |= TR1_SPEED;
			if (wizard || peek)
			    msg_print("Boots of Speed");
			t_ptr->name2 = EGO_SPEED;
			rating += 30;
			t_ptr->pval = 1;
			t_ptr->cost += 300000L;
		    }
		} else if (stricmp("& Pair of Metal Shod Boots",
				   k_list[t_ptr->index].name))	/* not metal */
		    if (tmp > 6) {
			t_ptr->flags1 |= TR3_FEATHER;
			rating += 7;
			t_ptr->name2 = EGO_SLOW_DESCENT;
			t_ptr->cost += 250;
		    } else if (tmp < 5) {
			t_ptr->flags1 |= TR1_STEALTH;
			rating += 16;
			t_ptr->pval = randint(3);
			t_ptr->name2 = EGO_STEALTH;
			t_ptr->cost += 500;
		    } else {	   /* 5,6 */
			t_ptr->flags1 |= TR2_FREE_ACT;
			rating += 15;
			t_ptr->name2 = EGO_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    }
		else
		 /* is metal boots, different odds since no stealth */
		    if (tmp < 5) {
			t_ptr->flags1 |= TR2_FREE_ACT;
			rating += 15;
			t_ptr->name2 = EGO_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    } else {	   /* tmp > 4 */
			t_ptr->flags1 |= TR3_FEATHER;
			rating += 7;
			t_ptr->name2 = EGO_SLOW_DESCENT;
			t_ptr->cost += 250;
		    }
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags1 |= TR1_SPEED;
		t_ptr->name2 = EGO_SLOWNESS;
		t_ptr->pval = -1;
	    } else if (tmp == 2) {
		t_ptr->flags1 |= TR3_AGGRAVATE;
		t_ptr->name2 = EGO_NOISE;
	    } else {
		t_ptr->name2 = EGO_GREAT_MASS;
		t_ptr->weight = t_ptr->weight * 5;
	    }
	    t_ptr->cost = 0;
	    t_ptr->toac = (-m_bonus(2, 20, level));
	    t_ptr->flags1 |= TR3_CURSED;
	}
	break;

      case TV_HELM:		   /* Helms */
	if ((t_ptr->sval >= 6) && (t_ptr->sval <= 8)) {
	/* give crowns a higher chance for magic */
	    chance += t_ptr->cost / 100;
	    special += special;
	}
	if (magik(chance) || good) {
	    t_ptr->toac = randint(3) + m_bonus(0, 10, level);
	    if (magik(special) || (good == 666)) {
		if (t_ptr->sval < 6) {
		    tmp = randint(14);
		    if (tmp < 3) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Intelligence");
			    t_ptr->pval = randint(2);
			    rating += 13;
			    t_ptr->flags1 |= TR1_INT;
			    t_ptr->name2 = EGO_INTELLIGENCE;
			    t_ptr->cost += t_ptr->pval * 500;
			}
		    } else if (tmp < 6) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Wisdom");
			    rating += 13;
			    t_ptr->pval = randint(2);
			    t_ptr->flags1 |= TR1_WIS;
			    t_ptr->name2 = EGO_WISDOM;
			    t_ptr->cost += t_ptr->pval * 500;
			}
		    } else if (tmp < 10) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    t_ptr->pval = 1 + randint(4);
			    rating += 11;
			    t_ptr->flags1 |= TR1_INFRA;
			    t_ptr->name2 = EGO_INFRAVISION;
			    t_ptr->cost += t_ptr->pval * 250;
			}
		    } else if (tmp < 12) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Light");
			    t_ptr->flags2 |= (TR2_RES_LITE | TR3_LITE);
			    rating += 6;
			    t_ptr->name2 = EGO_LIGHT;
			    t_ptr->cost += 500;
			}
		    } else if (tmp < 14) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Helm of Seeing");
			    t_ptr->flags1 |= TR3_SEE_INVIS;
			    t_ptr->flags2 |= TR2_RES_BLIND;
			    rating += 8;
			    t_ptr->name2 = EGO_SEEING;
			    t_ptr->cost += 1000;
			}
		    } else {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Telepathy");
			    rating += 20;
			    t_ptr->flags2 |= TR3_TELEPATHY;
			    t_ptr->name2 = EGO_TELEPATHY;
			    t_ptr->cost += 50000L;
			}
		    }
		} else {
		    switch (randint(6)) {
		      case 1:
			if (!(((randint(2) == 1) || (good == 666)) &&
			      !not_unique && unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Crown of Might");
			    rating += 19;
			    t_ptr->pval = randint(3);
			    t_ptr->flags1 |= (TR2_FREE_ACT | TR1_CON |
					     TR1_DEX | TR1_STR);
			    t_ptr->name2 = EGO_MIGHT;
			    t_ptr->cost += 1000 + t_ptr->pval * 500;
			}
			break;
		      case 2:
			if (peek)
			    msg_print("Lordliness");
			t_ptr->pval = randint(3);
			rating += 17;
			t_ptr->flags1 |= (TR1_CHR | TR1_WIS);
			t_ptr->name2 = EGO_LORDLINESS;
			t_ptr->cost += 1000 + t_ptr->pval * 500;
			break;
		      case 3:
			if (peek)
			    msg_print("Crown of the Magi");
			rating += 15;
			t_ptr->pval = randint(3);
			t_ptr->flags1 |= (TR2_RES_ELEC | TR2_RES_COLD
				      | TR2_RES_ACID | TR2_RES_FIRE | TR1_INT);
			t_ptr->name2 = EGO_MAGI;
			t_ptr->cost += 3000 + t_ptr->pval * 500;
			break;
		      case 4:
			rating += 8;
			if (peek)
			    msg_print("Beauty");
			t_ptr->pval = randint(4);
			t_ptr->flags1 |= TR1_CHR;
			t_ptr->name2 = EGO_BEAUTY;
			t_ptr->cost += 750;
			break;
		      case 5:
			if (peek)
			    msg_print("Seeing");
			rating += 8;
			t_ptr->pval = 5 * (1 + randint(4));
			t_ptr->flags1 |= (TR3_SEE_INVIS | TR1_SEARCH);
			t_ptr->name2 = EGO_SEEING;
			t_ptr->cost += 1000 + t_ptr->pval * 100;
			break;
		      case 6:
			t_ptr->flags1 |= TR3_REGEN;
			rating += 10;
			if (peek)
			    msg_print("Regeneration");
			t_ptr->name2 = EGO_REGENERATION;
			t_ptr->cost += 1500;
			break;

		    }
		}
	    }
	} else if (magik(cursed)) {
	    t_ptr->toac -= m_bonus(1, 20, level);
	    t_ptr->flags1 |= TR3_CURSED;
	    t_ptr->cost = 0;
	    if (magik(special))
		switch (randint(7)) {
		  case 1:
		    t_ptr->pval = -randint(5);
		    t_ptr->flags1 |= TR1_INT;
		    t_ptr->name2 = EGO_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    t_ptr->pval = -randint(5);
		    t_ptr->flags1 |= TR1_WIS;
		    t_ptr->name2 = EGO_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    t_ptr->pval = -randint(5);
		    t_ptr->flags1 |= TR1_STR;
		    t_ptr->name2 = EGO_WEAKNESS;
		    break;
		  case 6:
		    t_ptr->flags1 |= TR3_TELEPORT;
		    t_ptr->name2 = EGO_TELEPORTATION;
		    break;
		  case 7:
		    t_ptr->pval = -randint(5);
		    t_ptr->flags1 |= TR1_CHR;
		    t_ptr->name2 = EGO_UGLINESS;
		    break;
		}
	}
	break;

      case TV_RING:		   /* Rings	      */
	if (!((randint(10) == 1) && !not_unique && unique_armour(t_ptr))) {
	    switch (t_ptr->sval) {
	      case 0:
	      case 1:
	      case 2:
	      case 3:		   /* 132-135 */
		if (magik(cursed)) {
		    t_ptr->pval = -m_bonus(1, 10, level);
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    t_ptr->pval = m_bonus(1, 6, level);
		    t_ptr->cost += t_ptr->pval * 100;
		}
		break;
	      case 4:		   /* 136 */
		if (magik(cursed)) {
		    t_ptr->pval = -randint(3);
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    if (peek)
			msg_print("Ring of Speed");
		    rating += 35;
		    if (randint(888) == 1)
			t_ptr->pval = 2;
		    else
			t_ptr->pval = 1;
		}
		break;
	      case 5:
		t_ptr->pval = 5 * m_bonus(1, 10, level);
		t_ptr->cost += t_ptr->pval * 30;
		if (magik(cursed)) {
		    t_ptr->pval = -t_ptr->pval;
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 14:
	      case 15:
	      case 16:		   /* Flames, Acid, Ice */
		t_ptr->toac = m_bonus(1, 10, level);
		t_ptr->toac += 5 + randint(7);
		t_ptr->cost += t_ptr->toac * 100;
		break;
              case 17:
              case 18:		   /* WOE, Stupidity */
		t_ptr->toac = (-5) - m_bonus(1,10,level);
		t_ptr->pval = (-randint(4));
		break;
	      case 19:		   /* Increase damage	      */
		t_ptr->todam = m_bonus(1, 10, level);
		t_ptr->todam += 3 + randint(10);
		t_ptr->cost += t_ptr->todam * 100;
		if (magik(cursed)) {
		    t_ptr->todam = -t_ptr->todam;
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 20:		   /* Increase To-Hit	      */
		t_ptr->tohit = m_bonus(1, 10, level);
		t_ptr->tohit += 3 + randint(10);
		t_ptr->cost += t_ptr->tohit * 100;
		if (magik(cursed)) {
		    t_ptr->tohit = -t_ptr->tohit;
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 21:		   /* Protection	      */
		t_ptr->toac = m_bonus(0, 10, level);
		t_ptr->toac += 4 + randint(5);
		t_ptr->cost += t_ptr->toac * 100;
		if (magik(cursed)) {
		    t_ptr->toac = -t_ptr->toac;
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 24:
	      case 25:
	      case 26:
	      case 27:
	      case 28:
	      case 29:
		t_ptr->ident |= ID_NOSHOW_P1;
		break;
	      case 30:		   /* Slaying	      */
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->todam = m_bonus(1, 10, level);
		t_ptr->todam += 2 + randint(3);
		t_ptr->tohit = m_bonus(1, 10, level);
		t_ptr->tohit += 2 + randint(3);
		t_ptr->cost += (t_ptr->tohit + t_ptr->todam) * 100;
		if (magik(cursed)) {
		    t_ptr->tohit = -t_ptr->tohit;
		    t_ptr->todam = -t_ptr->todam;
		    t_ptr->flags1 |= TR3_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      default:
		break;
	    }
	}
	break;

      case TV_AMULET:		   /* Amulets	      */
	if (t_ptr->sval < 2) {
	    if (magik(cursed)) {
		t_ptr->pval = -m_bonus(1, 5, level);
		t_ptr->flags1 |= TR3_CURSED;
		t_ptr->cost = -t_ptr->cost;
	    } else {
		t_ptr->pval = m_bonus(1, 5, level);
		t_ptr->cost += t_ptr->pval * 100;
	    }
	} else if (t_ptr->sval == 2) { /* searching */
	    t_ptr->pval = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		t_ptr->pval = -t_ptr->pval;
		t_ptr->cost = -t_ptr->cost;
		t_ptr->flags1 |= TR3_CURSED;
	    } else
		t_ptr->cost += 20 * t_ptr->pval;
	} else if (t_ptr->sval == 8) {
	    rating += 25;
	    t_ptr->pval = 5 * (randint(2) + m_bonus(0, 10, level));
	    t_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    t_ptr->cost += 20 * t_ptr->pval + 50 * t_ptr->toac;
	    if (t_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		t_ptr->flags1 |= TR3_CURSED;
	} else if (t_ptr->sval == 9) {
	/* amulet of DOOM */
	    t_ptr->pval = (-randint(5) - m_bonus(2, 10, level));
	    t_ptr->toac = (-randint(3) - m_bonus(0, 6, level));
	    t_ptr->flags1 |= TR3_CURSED;
	}
	break;

    /* Subval should be even for store, odd for dungeon */
    /* Dungeon found ones will be partially charged	 */
      case TV_LITE:
	if ((t_ptr->sval % 2) == 1) {
	    t_ptr->pval = randint(t_ptr->pval);
	    t_ptr->sval -= 1;
	}
	break;

      case TV_WAND:
	switch (t_ptr->sval) {
	  case 0:
	    t_ptr->pval = randint(10) + 6;
	    break;
	  case 1:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 4:
	    t_ptr->pval = randint(4) + 3;
	    break;
	  case 5:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 6:
	    t_ptr->pval = randint(20) + 12;
	    break;
	  case 7:
	    t_ptr->pval = randint(20) + 12;
	    break;
	  case 8:
	    t_ptr->pval = randint(10) + 6;
	    break;
	  case 9:
	    t_ptr->pval = randint(12) + 6;
	    break;
	  case 10:
	    t_ptr->pval = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->pval = randint(3) + 3;
	    break;
	  case 12:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 13:
	    t_ptr->pval = randint(10) + 6;
	    break;
	  case 14:
	    t_ptr->pval = randint(5) + 3;
	    break;
	  case 15:
	    t_ptr->pval = randint(5) + 3;
	    break;
	  case 16:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->pval = randint(5) + 4;
	    break;
	  case 18:
	    t_ptr->pval = randint(8) + 4;
	    break;
	  case 19:
	    t_ptr->pval = randint(6) + 2;
	    break;
	  case 20:
	    t_ptr->pval = randint(4) + 2;
	    break;
	  case 21:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 22:
	    t_ptr->pval = randint(5) + 2;
	    break;
	  case 23:
	    t_ptr->pval = randint(12) + 12;
	    break;
	  case 24:
	    t_ptr->pval = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->pval = randint(3) + 1;
	    break;
	  case 26:
	    t_ptr->pval = randint(3) + 1;
	    break;
	  case 27:
	    t_ptr->pval = randint(2) + 1;
	    break;
	  case 28:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  default:
	    break;
	}
	break;

      case TV_STAFF:
	switch (t_ptr->sval) {
	  case 0:
	    t_ptr->pval = randint(20) + 12;
	    break;
	  case 1:
	    t_ptr->pval = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->pval = randint(20) + 12;
	    break;
	  case 4:
	    t_ptr->pval = randint(15) + 6;
	    break;
	  case 5:
	    t_ptr->pval = randint(4) + 5;
	    break;
	  case 6:
	    t_ptr->pval = randint(5) + 3;
	    break;
	  case 7:
	    t_ptr->pval = randint(3) + 1;
	    t_ptr->level = 10;
	    break;
	  case 8:
	    t_ptr->pval = randint(3) + 1;
	    break;
	  case 9:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 10:
	    t_ptr->pval = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 12:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 13:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 14:
	    t_ptr->pval = randint(10) + 12;
	    break;
	  case 15:
	    t_ptr->pval = randint(3) + 4;
	    break;
	  case 16:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->pval = randint(5) + 6;
	    break;
	  case 18:
	    t_ptr->pval = randint(3) + 4;
	    break;
	  case 19:
	    t_ptr->pval = randint(10) + 12;
	    break;
	  case 20:
	    t_ptr->pval = randint(3) + 4;
	    break;
	  case 21:
	    t_ptr->pval = randint(3) + 4;
	    break;
	  case 22:
	    t_ptr->pval = randint(10) + 6;
	    t_ptr->level = 5;
	    break;
	  case 23:
	    t_ptr->pval = randint(2) + 1;
	    break;
	  case 24:
	    t_ptr->pval = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->pval = randint(2) + 2;
	    break;
	  case 26:
	    t_ptr->pval = randint(15) + 5;
	    break;
	  case 27:
	    t_ptr->pval = randint(2) + 2;
	    break;
	  case 28:
	    t_ptr->pval = randint(5) + 5;
	    break;
	  case 29:
	    t_ptr->pval = randint(2) + 1;
	    break;
	  case 30:
	    t_ptr->pval = randint(6) + 2;
	    break;
	  default:
	    break;
	}
	break;

      case TV_CLOAK:
	if (magik(chance) || good) {
	    int                 made_art_cloak;

	    made_art_cloak = 0;
	    t_ptr->toac += 1 + m_bonus(0, 20, level);
	    if (magik(special) || (good == 666)) {
		if (!not_unique &&
		    !stricmp(k_list[t_ptr->index].name, "& Cloak")
		    && randint(10) == 1) {
		    switch (randint(9)) {
		      case 1:
		      case 2:
			if (COLLUIN)
			    break;
			if (wizard || peek)
			    msg_print("Colluin");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_COLLUIN;
			t_ptr->toac = 15;
			t_ptr->flags1 |= (TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_POIS |
					 TR2_RES_ELEC | TR2_RES_ACID);
			t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
			t_ptr->cost = 10000L;
			made_art_cloak = 1;
			COLLUIN = 1;
			break;
		      case 3:
		      case 4:
			if (HOLCOLLETH)
			    break;
			if (wizard || peek)
			    msg_print("Holcolleth");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_HOLCOLLETH;
			t_ptr->toac = 4;
			t_ptr->pval = 2;
			t_ptr->flags1 |= (TR1_INT | TR1_WIS | TR1_STEALTH |
					 TR2_RES_ACID);
			t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
			t_ptr->ident |= ID_NOSHOW_TYPE;
			t_ptr->cost = 13000L;
			made_art_cloak = 1;
			HOLCOLLETH = 1;
			break;
		      case 5:
			if (THINGOL)
			    break;
			if (wizard || peek)
			    msg_print("Thingol");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_THINGOL;
			t_ptr->toac = 18;
			t_ptr->flags1 = (TR1_DEX | TR1_CHR | TR2_RES_FIRE |
				   TR2_RES_ACID | TR2_RES_COLD | TR2_FREE_ACT);
			t_ptr->flags2 = (TR3_ACTIVATE | TR_ARTIFACT);
			t_ptr->pval = 3;
			t_ptr->cost = 35000L;
			made_art_cloak = 1;
			THINGOL = 1;
			break;
		      case 6:
		      case 7:
			if (THORONGIL)
			    break;
			if (wizard || peek)
			    msg_print("Thorongil");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_THORONGIL;
			t_ptr->toac = 10;
			t_ptr->flags1 = (TR3_SEE_INVIS | TR2_FREE_ACT |
					TR2_RES_ACID);
			t_ptr->flags2 |= (TR_ARTIFACT);
			t_ptr->cost = 8000L;
			made_art_cloak = 1;
			THORONGIL = 1;
			break;
		      case 8:
		      case 9:
			if (COLANNON)
			    break;
			if (wizard || peek)
			    msg_print("Colannon");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_COLANNON;
			t_ptr->toac = 15;
			t_ptr->flags1 |= (TR1_STEALTH | TR2_RES_ACID);
			t_ptr->flags2 |= (TR3_ACTIVATE | TR_ARTIFACT);
			t_ptr->pval = 3;
			t_ptr->cost = 11000L;
			made_art_cloak = 1;
			COLANNON = 1;
			break;
		    }

		} else if (!not_unique &&
			   !stricmp(k_list[t_ptr->index].name,
				    "& Shadow Cloak")
			   && randint(20) == 1) {
		    switch (randint(2)) {
		      case 1:
			if (LUTHIEN)
			    break;
			if (wizard || peek)
			    msg_print("Luthien");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_LUTHIEN;
			t_ptr->toac = 20;
			t_ptr->flags1 = (TR2_RES_FIRE | TR2_RES_COLD |
				    TR1_INT | TR1_WIS | TR1_CHR | TR2_RES_ACID);
			t_ptr->flags2 = (TR3_ACTIVATE | TR_ARTIFACT);
			t_ptr->pval = 2;
			t_ptr->cost = 45000L;
			made_art_cloak = 1;
			LUTHIEN = 1;
			break;
		      case 2:
			if (TUOR)
			    break;
			if (wizard || peek)
			    msg_print("Tuor");
			else
			    good_item_flag = TRUE;
			t_ptr->name2 = ART_TUOR;
			t_ptr->toac = 12;
			t_ptr->flags1 = (TR1_STEALTH |
				  TR2_FREE_ACT | TR3_SEE_INVIS | TR2_RES_ACID);
			t_ptr->flags2 |= (TR2_IM_ACID | TR_ARTIFACT);
			t_ptr->pval = 4;
			t_ptr->cost = 35000L;
			made_art_cloak = 1;
			TUOR = 1;
			break;
		    }
		}
		if (!made_art_cloak) {
		    if (randint(2) == 1) {
			t_ptr->name2 = EGO_PROTECTION;
			t_ptr->toac += m_bonus(0, 10, level) + (5 + randint(3));
			t_ptr->cost += 250L;
			rating += 8;
		    } else if (randint(10) < 10) {
			t_ptr->toac += m_bonus(3, 10, level);
			t_ptr->pval = randint(3);
			t_ptr->flags1 |= TR1_STEALTH;
			t_ptr->name2 = EGO_STEALTH;
			t_ptr->cost += 500 + (50 * t_ptr->pval);
			rating += 9;
		    } else {
			t_ptr->toac += 10 + randint(10);
			t_ptr->pval = randint(3);
			t_ptr->flags1 |= (TR1_STEALTH | TR2_RES_ACID);
			t_ptr->name2 = EGO_AMAN;
			t_ptr->cost += 4000 + (100 * t_ptr->toac);
			rating += 16;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags1 |= TR3_AGGRAVATE;
		t_ptr->name2 = EGO_IRRITATION;
		t_ptr->toac -= m_bonus(1, 10, level);
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->tohit -= m_bonus(1, 10, level);
		t_ptr->todam -= m_bonus(1, 10, level);
		t_ptr->cost = 0;
	    } else if (tmp == 2) {
		t_ptr->name2 = EGO_VULNERABILITY;
		t_ptr->toac -= m_bonus(10, 20, level + 50);
		t_ptr->cost = 0;
	    } else {
		t_ptr->name2 = EGO_ENVELOPING;
		t_ptr->toac -= m_bonus(1, 10, level);
		t_ptr->ident |= ID_SHOW_HITDAM;
		t_ptr->tohit -= m_bonus(2, 15, level + 10);
		t_ptr->todam -= m_bonus(2, 15, level + 10);
		t_ptr->cost = 0;
	    }
	    t_ptr->flags1 |= TR3_CURSED;
	}
	break;

      case TV_CHEST:
	switch (randint(level + 4)) {
	  case 1:
	    t_ptr->flags1 = 0;
	    t_ptr->name2 = EGO_EMPTY;
	    break;
	  case 2:
	    t_ptr->flags1 |= CH_LOCKED;
	    t_ptr->name2 = EGO_LOCKED;
	    break;
	  case 3:
	  case 4:
	    t_ptr->flags1 |= (CH_LOSE_STR | CH_LOCKED);
	    t_ptr->name2 = EGO_POISON_NEEDLE;
	    break;
	  case 5:
	  case 6:
	    t_ptr->flags1 |= (CH_POISON | CH_LOCKED);
	    t_ptr->name2 = EGO_POISON_NEEDLE;
	    break;
	  case 7:
	  case 8:
	  case 9:
	    t_ptr->flags1 |= (CH_PARALYSED | CH_LOCKED);
	    t_ptr->name2 = EGO_GAS_TRAP;
	    break;
	  case 10:
	  case 11:
	    t_ptr->flags1 |= (CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = EGO_EXPLOSION_DEVICE;
	    break;
	  case 12:
	  case 13:
	  case 14:
	    t_ptr->flags1 |= (CH_SUMMON | CH_LOCKED);
	    t_ptr->name2 = EGO_SUMMONING_RUNES;
	    break;
	  case 15:
	  case 16:
	  case 17:
	    t_ptr->flags1 |= (CH_PARALYSED | CH_POISON | CH_LOSE_STR |
			     CH_LOCKED);
	    t_ptr->name2 = EGO_MULTIPLE_TRAPS;
	    break;
	  default:
	    t_ptr->flags1 |= (CH_SUMMON | CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = EGO_MULTIPLE_TRAPS;
	    break;
	}
	if (not_unique)		/* if bought from store - dbd */
	    t_ptr->pval = randint(t_ptr->level);
	else			/* store the level chest's found on - dbd */
	    t_ptr->pval = dun_level;
	break;

      case TV_SPIKE:
	t_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    t_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	t_ptr->pval = missile_ctr;
	break;

    case TV_BOLT: case TV_ARROW: case TV_SHOT:
     /* this fn makes ammo for player's missile weapon more common -CFT */
      magic_ammo(t_ptr, good, chance, special, cursed, level);
      break;

      case TV_FOOD:
    /* make sure all food rations have the same level */
	if (t_ptr->sval == 90)
	    t_ptr->level = 0;
    /* give all elvish waybread the same level */
	else if (t_ptr->sval == 92)
	    t_ptr->level = 6;
	break;

      case TV_SCROLL1:
    /* give all identify scrolls the same level */
	if (t_ptr->sval == 67)
	    t_ptr->level = 1;
    /* scroll of light */
	else if (t_ptr->sval == 69)
	    t_ptr->level = 0;
    /* scroll of trap detection */
	else if (t_ptr->sval == 80)
	    t_ptr->level = 5;
    /* scroll of door/stair location */
	else if (t_ptr->sval == 81)
	    t_ptr->level = 5;
	break;

      case TV_POTION1:		   /* potions */
    /* cure light */
	if (t_ptr->sval == 76)
	    t_ptr->level = 0;
	break;

      default:
	break;
    }
}



static void magic_ammo(inven_type *t_ptr, int good, int chance, int special, int cursed, int level)
{
    register inven_type *i_ptr = NULL;
    register int         i;

    /* if wielding a bow as main/aux weapon, then ammo will be "right" ammo
     * more often than not of the time -CFT */
    if (inventory[INVEN_WIELD].tval == TV_BOW) i_ptr=&inventory[INVEN_WIELD];
    else if (inventory[INVEN_AUX].tval == TV_BOW) i_ptr=&inventory[INVEN_AUX];

    if (i_ptr && (randint(2)==1)){
	if ((t_ptr->tval == TV_SHOT) &&
	    (i_ptr->sval >= 20) && (i_ptr->sval <= 21));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_ARROW) &&
		 (i_ptr->sval >= 1) && (i_ptr->sval <= 4));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_BOLT) &&
		 (i_ptr->sval >= 10) && (i_ptr->sval <= 12));
	/* right type, do nothing */
	else if ((i_ptr->sval >= 20) && (i_ptr->sval <= 21))
	    invcopy(t_ptr, 83); /* this should be treasure list index of shots -CFT */
	else if ((i_ptr->sval >= 1) && (i_ptr->sval <= 4))
	    invcopy(t_ptr, 78); /* this should be index of arrows -CFT */
	else			/* xbow */
	    invcopy(t_ptr, 80); /* this should be index of bolts -CFT */
    }

    t_ptr->number = 0;
    for (i = 0; i < 7; i++)
	t_ptr->number += randint(6);
    if (missile_ctr == MAX_SHORT)
	missile_ctr = -MAX_SHORT - 1;
    else
	missile_ctr++;
    t_ptr->pval = missile_ctr;

    /* always show tohit/todam values if identified */
    t_ptr->ident |= ID_SHOW_HITDAM;
    if (magik(chance)||good) {
	t_ptr->tohit = randint(5) + m_bonus(1, 15, level);
	t_ptr->todam = randint(5) + m_bonus(1, 15, level);
	/* see comment for weapons */
	if (magik(5*special/2)||(good==666))
	    switch(randint(11)) {
	      case 1: case 2: case 3:
		t_ptr->name2 = EGO_WOUNDING; /* swapped with slaying -CFT */
		t_ptr->tohit += 5;
		t_ptr->todam += 5;
		t_ptr->damage[0] ++; /* added -CFT */
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 4: case 5:
		t_ptr->flags1 |= (TR1_BRAND_FIRE|TR2_RES_FIRE); /* RF so won't burn */
		t_ptr->tohit += 2;
		t_ptr->todam += 4;
		t_ptr->name2 = EGO_FIRE;
		t_ptr->cost += 25;
		rating += 6;
		break;
	      case 6: case 7:
		t_ptr->flags1 |= TR1_SLAY_EVIL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = EGO_SLAY_EVIL;
		t_ptr->cost += 25;
		rating += 7;
		break;
	      case 8: case 9:
		t_ptr->flags1 |= TR1_SLAY_ANIMAL;
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		t_ptr->name2 = EGO_SLAY_ANIMAL;
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 10:
		t_ptr->flags1 |= TR1_SLAY_DRAGON;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = EGO_DRAGON_SLAYING;
		t_ptr->cost += 35;
		rating += 9;
		break;
	      case 11:
		t_ptr->tohit += 10; /* reduced because of dice bonus -CFT */
		t_ptr->todam += 10;
		t_ptr->name2 = EGO_SLAYING; /* swapped w/ wounding -CFT */
		t_ptr->damage[0] += 2; /* added -CFT */
		t_ptr->cost += 45;
		rating += 10;
		break;
	    }
	while (magik(special)) { /* added -CFT */
	    t_ptr->damage[0]++;
	    t_ptr->cost += t_ptr->damage[0]*5;
	}
    }
    else if (magik(cursed)) {
	t_ptr->tohit = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->todam = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->flags1 |= TR3_CURSED;
	t_ptr->cost = 0;
	if (randint(5)==1) {
	    t_ptr->name2 = EGO_BACKBITING;
	    t_ptr->tohit -= 20;
	    t_ptr->todam -= 20;
	}
    }
}

