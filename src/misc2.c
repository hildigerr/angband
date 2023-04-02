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

monster_type blank_monster = {0,0,0,0,0,0,0,FALSE,0,FALSE};

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
 * Deletes a monster entry from the level.
 */
static void delete_monster_fully(int j)
{
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register int fx, fy;

    /* Paranoia */
    if (j < MIN_M_IDX) return;

    /* Get the monster */
    m_ptr = &m_list[j];

    /* Get the monster location */
    fx = m_ptr->fx;
    fy = m_ptr->fy;

    /* One less of this monster on this level */
    l_list[m_ptr->r_idx].cur_num--;

    /* Forget that the monster is here */
    cave[fy][fx].m_idx = 0;

    /* Visual update */
    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);

    /* Do structure dumping */
    if (j != m_max - 1) {

#ifdef TARGET
    /* This targetting code stolen from Morgul -CFT */
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;
#endif

    /* One less monster */
    m_max--;

#ifdef TARGET
	/* Targetted monster moved to replace dead or compacted monster -CDW */
	if (target_mon == m_max) target_mon = j;
#endif

	/* Slide the last monster into the dead monster's slot */
	m_ptr = &m_list[m_max];
	c_ptr = &cave[m_ptr->fy][m_ptr->fx];
	c_ptr->m_idx = j;
	m_list[j] = m_list[m_max];
    }

    /* Wipe the monster record */
    m_list[m_max] = blank_monster;
    if (mon_tot_mult > 0) mon_tot_mult--;
}




/*
 * This routine "marks" a monster to be killed later.  The obvious method
 * is to set the hitpoints to be negative, though this is not the best method,
 * since negative hitpoints can be "cured".  We should really have a "nuke" flag.
 *
 * But anyway, this routine marks a monster to be deleted "later".  Currently,
 * the algorithm involves setting "hack_m_idx" to the index of the highest
 * monster that should be left alone.  This is done mostly in "process_monsters()",
 * where the monsters are processed from the last monster down, and we do not
 * want monsters that have already had a turn to replace monsters that have not
 * had a turn yet, or they will get two turns.  So we mark monsters as "dead",
 * and then, when it is actually their turn, we remove them.
 *
 * In fact, in general, it is a bad idea to delete "monsters" or any other type
 * of record "mid-processing", the standard technique is to mark things as dead,
 * and then to "garbage collect" at the end of each turn.
 *
 */
static void delete_monster_later(int j)
{
    register monster_type *m_ptr;
    register int fx, fy;

    /* Paranoia */
    if (j < MIN_M_IDX) return;

    /* Get the monster */
    m_ptr = &m_list[j];

    /* Get the cave */
    fy = m_ptr->fy;
    fx = m_ptr->fx;

#ifdef TARGET
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon == m_max-1) target_mon = j;
#endif

    /* Mark the monster as "dead" (non-optimal method) */
    m_ptr->hp = (-1);
    cave[m_ptr->fy][m_ptr->fx].m_idx = 0;
    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    if (mon_tot_mult > 0) mon_tot_mult--;
}


/*
 * Delete a monster, now if possible, later if necessary
 *
 * We use "hack_m_idx" to determine if the monster can be safely deleted now.
 * The "hack_m_idx" is set only in "process_monsters()" and is always "-1"
 * except during the execution of process_monsters().
 */
void delete_monster_idx(int i)
{
    /* Paranoia */
    if (i < MIN_M_IDX) return;

    /* This monster will be processed later */
    if (i < hack_m_idx) {

	/* Mark the monster as dead */
	delete_monster_later(i);
    }

    /* This monster might as well be deleted now */
    else {

	/* Actually delete the monster */
	delete_monster_fully(i);
    }
}


/*
 * Delete the monster, if any, at a given location
 */
void delete_monster(int y, int x)
{
    cave_type *c_ptr;

    /* Paranoia */
    if (!in_bounds(y,x)) return;

    /* Check the grid */
    c_ptr = &cave[y][x];

    /* Hack -- no such monster */
    if (c_ptr->m_idx < MIN_M_IDX) return;

    /* Delete the monster */
    delete_monster_idx(c_ptr->m_idx);
}




/*
 * Link all free space in monster list together
 */
void wipe_m_list()
{
    register int i;

    /* Delete the existing monsters (backwards!) */
    for (i = m_max-1; i >= MIN_M_IDX; i--) delete_monster_idx(i);

    /* XXX Should already be done */
    m_max = MIN_M_IDX;
}


/*
 * Attempt to Compact some monsters (safely)	-RAK-
 *
 * XXX Base the saving throw on a combination of
 * monster level and current "desperation".
 */
static void compact_monsters(void)
{
    register int           i;
    int                    cur_dis, orig;
    monster_type	*m_ptr;
    monster_race	*r_ptr;

    msg_print("Compacting monsters...");

    /* Remember how many monsters we started with */
    orig = m_max;

    /* Start 66 (that is, 72-6) units away */
    cur_dis = 66;

    /* Keep going until someone is deleted */
    while (m_max == orig) {

	/* Nothing to compact (!!!) */
	if (cur_dis < 0) return;

	/* Come closer to the player */
	cur_dis -= 6;

	/* Check all the monsters */
	for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	    m_ptr = &m_list[i];
	    r_ptr = &r_list[m_ptr->r_idx];

	    /* Ignore nearby monsters */
	    if (m_ptr->cdis < cur_dis) continue;

	    /* Don't compact Melkor! */
	    if (r_ptr->cflags1 & MF1_WINNER) continue;

	    /* All monsters get a saving throw */
	    if (randint(3) == 1) {

		/* Delete the monster */
		delete_monster_idx(i);
	    }
	}
    }
}


/*
 * Allow "dungeon.c" to pre-emptively compact the monster list
 */
void tighten_m_list(void)
{
    /* If not much space left, try compacting */
    if (MAX_M_IDX - m_max < 10) compact_monsters();
}


/*
 * Returns a pointer to next free space			-RAK-
 */
int m_pop(void)
{
    /* Out of space?  Compact. */
    if (m_max == MAX_M_IDX) compact_monsters();

    /* XXX XXX XXX XXX Out of memory! */    
    if (m_max == MAX_M_IDX) return (-1);

    /* Return (and increase) free slot */
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

    /* See if we can make any more of them */
    if (l_list[r_idx].cur_num >= l_list[r_idx].max_num) {

	/* Note for wizard */
	if (wizard) {
	    (void)sprintf(buf, "Ignoring %s monster (%s).",
			  (l_list[r_idx].max_num ? "excessive" : "dead"),
			  r_ptr->name);
	    msg_print(buf);
	}

	/* Cannot create */
	return FALSE;
    }


    /* Count the monsters on the level */
    l_list[r_idx].cur_num++;

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
    if (((r_ptr->spells1 & (MS1_CAUSE_1|MS1_CAUSE_2|MS1_HOLD|
                                  MS1_BLIND|MS1_CONF|MS1_FEAR|MS1_SLOW|MS1_BR_ELEC|
                                  MS1_BR_POIS|MS1_BR_ACID|MS1_BR_COLD|MS1_BR_FIRE|
                                  MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID|MS1_ARROW_1|
                                  MS1_CAUSE_3|MS1_BA_FIRE|MS1_BA_COLD|MS1_BO_MANA))
          || (r_ptr->spells2 & (MS2_BR_CHAO|MS2_BR_SHAR|MS2_BR_SOUN|MS2_BR_CONF|
                                  MS2_BR_DISE|MS2_BR_LIFE|MS2_BO_ELEC|MS2_BA_ELEC|
                                  MS2_BA_ACID|MS2_TRAP_CREATE|MS2_RAZOR|MS2_MIND_BLAST|
                                  MS2_ARROW_2|MS2_BO_PLAS|MS2_BO_NETH|MS2_BO_ICEE|
                                  MS2_FORGET|MS2_BRAIN_SMASH|MS2_BA_POIS|MS2_TELE_LEVEL|
                                  MS2_BO_WATE|MS2_BA_WATE|MS2_BA_NETH|MS2_BR_NETH))
          || (r_ptr->spells3 & (MS3_BR_WALL|MS3_BR_SLOW|MS3_BR_LITE|MS3_BR_TIME|
                                  MS3_BR_GRAV|MS3_BR_DARK|MS3_BR_PLAS|MS3_ARROW_3|
                                  MS3_DARK_STORM|MS3_MANA_STORM)))
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



/*
 * XXX Note that g->name is set during "init_r_list()"
 *
 * I am a little concerned about the reliance of "ghost fields"
 * In particular, shouldn't we clear most of them first?
 * I worry, because "cflags2" is set pretty high in the
 * default initializations.
 */

static cptr ghost_race_names[] = {
    "human", "elf", "elf", "hobbit", "gnome",
    "dwarf", "orc", "troll", "human", "elf"
};

static cptr ghost_class_names[] = {
    "warrior", "mage", "priest",
    "rogue", "ranger", "paladin"
};


/*
 * Prepare the "ghost" monster_race info
 *
 * XXX This code has only recently been debugged,
 * so it may have brand new bugs now.
 *
 * Even if not, it really needs to be re-writtem, there are redundancies
 * and incorrectnesses everywhere.  And the savefile ruins everything.
 * Actually, the new savefile is "much better".  It may fix the problems.
 */
static void set_ghost(monster_race *g, cptr pn, int gr, int gc, int lev)
{
    int  i;

    char name[20];
    char gr_name[20];
    char gc_name[20];

    /* Extract the basic ghost name */
    strcpy(name, pn);

    /* Extract the race and class names */
    strcpy(gr_name, ghost_race_names[gr]);
    strcpy(gc_name, ghost_class_names[gc]);

    /* Capitalize the name */
    if (islower(name[0])) name[0] = toupper(name[0]);

    /* Capitalize the race/class */
    if (islower(gr_name[0])) gr_name[0] = toupper(gr_name[0]);
    if (islower(gc_name[0])) gc_name[0] = toupper(gc_name[0]);

    /* Forget any flags a previous ghost had */
    g->cflags1 = g->cflags2 = 0L;

    /* Forget any spells a previous ghost had */
    g->spells1 = g->spells2 = g->spells3 = 0L;

    /* Save the level, extract the experience */
    g->level = lev;
    g->mexp = lev * 5 + 5;

    /* Never asleep (?) */
    g->sleep = 0;

    /* Very attentive (?) */
    g->aaf = 100;


    /* Initialize some of the flags */
    g->cflags1 |= (MF1_MV_ATT_NORM | MF1_CARRY_OBJ);
    g->cflags2 |= (MF2_GOOD);
    g->cflags2 |= (MF2_UNIQUE | MF2_CHARM_SLEEP | MF2_EVIL);


    /* Town ghost */
    if (!dun_level) {

	/* A wanderer in the town */
	sprintf(ghost_name, "%s, the %s %s",
		name, gr_name, gc_name);

	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_90 | MF1_HAS_60);

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
	    break;
	  case 1:		   /* Mage */
	    g->spells1 |= (0x3L | MS1_BLINK | MS1_ARROW_1 |
			   MS1_SLOW | MS1_CONF);
	    if (lev > 5) g->spells2 |= MS2_BA_POIS;
	    if (lev > 7) g->spells2 |= MS2_BO_ELEC;
	    if (lev > 10) g->spells1 |= MS1_BO_COLD;
	    if (lev > 12) g->spells1 |= MS1_TELEPORT;
	    if (lev > 15) g->spells1 |= MS1_BO_ACID;
	    if (lev > 20) g->spells1 |= MS1_BO_FIRE;
	    if (lev > 25) g->spells1 |= MS1_BA_COLD;
	    if (lev > 25) g->spells2 |= MS2_HASTE;
	    if (lev > 30) g->spells1 |= MS1_BA_FIRE;
	    if (lev > 40) g->spells1 |= MS1_BO_MANA;
	    break;
	  case 3:		   /* Rogue */
	    g->spells1 |= (0x5L | MS1_BLINK);
	    if (lev > 10) g->spells1 |= MS1_CONF;
	    if (lev > 18) g->spells1 |= MS1_SLOW;
	    if (lev > 25) g->spells1 |= MS1_TELEPORT;
	    if (lev > 30) g->spells1 |= MS1_HOLD;
	    if (lev > 35) g->spells1 |= MS1_TELE_TO;
	    break;
	  case 4:		   /* Ranger */
	    g->spells1 |= (0x8L | MS1_ARROW_1);
	    if (lev > 5) g->spells2 |= MS2_BA_POIS;
	    if (lev > 7) g->spells2 |= MS2_BO_ELEC;
	    if (lev > 10) g->spells1 |= MS1_BO_COLD;
	    if (lev > 18) g->spells1 |= MS1_BO_ACID;
	    if (lev > 25) g->spells1 |= MS1_BO_FIRE;
	    if (lev > 30) g->spells1 |= MS1_BA_COLD;
	    if (lev > 35) g->spells1 |= MS1_BA_FIRE;
	    break;
	  case 2:		   /* Priest */
	  case 5:		   /* Paladin */
	    g->spells1 |= (0x4L | MS1_CAUSE_1 | MS1_FEAR);
	    if (lev > 5) g->spells2 |= MS2_HEAL;
	    if (lev > 10) g->spells1 |= (MS1_CAUSE_2 | MS1_BLIND);
	    if (lev > 18) g->spells1 |= MS1_HOLD;
	    if (lev > 25) g->spells1 |= MS1_CONF;
	    if (lev > 30) g->spells1 |= MS1_CAUSE_3;
	    if (lev > 35) g->spells1 |= MS1_MANA_DRAIN;
	    break;
	}

	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;

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


    /* Initialize some more of the flags */    
    g->cflags2 |= (MF2_UNDEAD | MF2_NO_INFRA | MF2_IM_POIS);


    /* Make a ghost with power based on the ghost level */
    switch ((int) (g->level / 4) + randint(3)) {

      case 1:
      case 2:
      case 3:
	sprintf(ghost_name, "%s, the Skeleton %s", name, gr_name);
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_90);
	g->cflags2 |= (MF2_IM_COLD);
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
	sprintf(ghost_name, "%s, the %s zombie", name, gr_name);
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_60 | MF1_HAS_90);
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
	sprintf(ghost_name, "%s, the Poltergeist", name);
	g->cflags1 |= (MF1_MV_INVIS | MF1_HAS_1D2 | MF1_MV_75 | MF1_THRO_WALL);
	g->cflags2 |= (MF2_IM_COLD);
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
	sprintf(ghost_name, "%s, the Mummified %s", name, gr_name);
	g->cflags1 |= (MF1_HAS_1D2);
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
	sprintf(ghost_name, "%s%s spirit", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_HAS_1D2);
	g->cflags2 |= (MF2_IM_COLD);
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
	sprintf(ghost_name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_HAS_1D2);
	g->cflags2 |= (MF2_IM_COLD);
	g->spells1 |= (0xFL | MS1_HOLD | MS1_MANA_DRAIN | MS1_BLIND);
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
	sprintf(ghost_name, "%s, the Vampire", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_2D2);
	g->cflags2 |= (MF2_HURT_LITE);
	g->spells1 |= (0x8L | MS1_HOLD | MS1_FEAR | MS1_TELE_TO | MS1_CAUSE_2);
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
	sprintf(ghost_name, "%s%s Wraith", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_4D2 | MF1_HAS_2D2);
	g->cflags2 |= (MF2_IM_COLD | MF2_HURT_LITE);
	g->spells1 |= (0x7L | MS1_HOLD | MS1_FEAR | MS1_BLIND | MS1_CAUSE_3);
	g->spells2 |= (MS2_BO_NETH);
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
	sprintf(ghost_name, "%s, the Vampire Lord", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_1D2);
	g->cflags2 |= (MF2_HURT_LITE | MF2_SPECIAL);
	g->spells1 |= (0x8L | MS1_HOLD | MS1_FEAR | MS1_TELE_TO | MS1_CAUSE_3);
	g->spells2 |= (MS2_BO_NETH);
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
	sprintf(ghost_name, "%s%s ghost", name,
		 (name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_HAS_2D2);
	g->cflags2 |= (MF2_SPECIAL | MF2_IM_COLD);
	g->spells1 |= (0x5L | MS1_HOLD | MS1_MANA_DRAIN | MS1_BLIND | MS1_CONF);
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
	sprintf(ghost_name, "%s, the Lich", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_HAS_2D2 | MF1_HAS_1D2);
	g->cflags2 |= (MF2_SPECIAL | MF2_IM_COLD | MF2_INTELLIGENT);
	g->spells1 |= (0x3L | MS1_FEAR | MS1_CAUSE_3 | MS1_TELE_TO | MS1_BLINK |
		       MS1_S_UNDEAD | MS1_BA_FIRE | MS1_BA_COLD | MS1_HOLD |
		       MS1_MANA_DRAIN | MS1_BLIND | MS1_CONF | MS1_TELEPORT);
	g->spells2 |= (MS2_BRAIN_SMASH | MS2_RAZOR);
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
	sprintf(ghost_name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL |
		       MF1_HAS_1D2 | MF1_HAS_2D2);
	g->cflags2 |= (MF2_SPECIAL | MF2_IM_COLD | MF2_INTELLIGENT);
	g->spells1 |= (0x2L | MS1_HOLD | MS1_MANA_DRAIN | 
		       MS1_BLIND | MS1_CONF | MS1_TELE_TO);
	g->spells2 |= (MS2_BO_NETH | MS2_BA_NETH | MS2_BRAIN_SMASH |
		       MS2_TELE_LEVEL);
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
	if (p_ptr->lev < 5 || randint(10) > 1) return 0;

	/* Look for a proper bones file */
	sprintf(tmp, "%s/%d", ANGBAND_DIR_BONES, p_ptr->lev);
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
	    level = p_ptr->lev;
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
static u16b r_level[MAX_R_LEV+1];



/*
 * Major Hack -- Initializes r_level array
 */
static void init_r_level()
{
    register int i, k;
    static bool done = FALSE;

    /* Only initialize once */    
    if (done) return;

    /* Start with no monsters per level */
    for (i = 0; i <= MAX_R_LEV; i++) r_level[i] = 0;

    /* Count the NORMAL monsters on each level */
    k = MAX_R_IDX - WIN_MON_TOT;
    for (i = 0; i < k; i++) r_level[r_list[i].level]++;

    /* Deduce the monsters on or below each level */
    for (i = 1; i <= MAX_R_LEV; i++) r_level[i] += r_level[i-1];

    /* Only do this once */
    done = TRUE;
}





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

    init_r_level();

    while (1) {

	if (level == 0) {
	    i = rand_int(r_level[0]);
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
		num = r_level[level] - r_level[0];
		i = randint(num) - 1;
		j = randint(num) - 1;
		if (j > i) i = j;
		level = r_list[i + r_level[0]].level;
	    }

	    /* Bizarre function */            
	    i = r_level[level] - r_level[level - 1];
	    if (i == 0) i++;
	    i = randint(i) - 1 + r_level[level - 1];
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

    init_r_level();

    while (1) {

	if (level == 0) {
	    i = rand_int(r_level[0]);
	}

	else {

	    if (level > MAX_R_LEV) level = MAX_R_LEV;

	    num = r_level[level] - r_level[0];

	    i = rand_int(num);
	    i += 15;
	    if (i >= num) i = num - 1;

	    j = rand_int(num);
	    if (j > i) i = j;

	    j = rand_int(num);
	    if (j > i) i = j;

	    level = r_list[i + r_level[0]].level;
	    i = r_level[level] - r_level[level - 1];
	    if (i == 0) i = 1;

	    i = randint(i) - 1 + r_level[level - 1];
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

	if (((r_list[r_idx].spells1 & (MS1_CAUSE_1 | MS1_CAUSE_2 | MS1_HOLD |
			    MS1_BLIND | MS1_CONF | MS1_FEAR | MS1_SLOW | MS1_BR_ELEC |
			       MS1_BR_POIS | MS1_BR_ACID | MS1_BR_COLD | MS1_BR_FIRE |
			     MS1_BO_FIRE | MS1_BO_COLD | MS1_BO_ACID | MS1_ARROW_1 |
			   MS1_CAUSE_3 | MS1_BA_FIRE | MS1_BA_COLD | MS1_BO_MANA))
	     || (r_list[r_idx].spells2 & (MS2_BR_CHAO | MS2_BR_SHAR | MS2_BR_SOUN | MS2_BR_CONF |
			   MS2_BR_DISE | MS2_BR_LIFE | MS2_BO_ELEC | MS2_BA_ELEC |
			      MS2_BA_ACID | MS2_TRAP_CREATE | MS2_RAZOR | MS2_MIND_BLAST |
			    MS2_ARROW_2 | MS2_BO_PLAS | MS2_BO_NETH | MS2_BO_ICEE |
				MS2_FORGET | MS2_BRAIN_SMASH | MS2_BA_POIS | MS2_TELE_LEVEL |
			 MS2_BO_WATE | MS2_BA_WATE | MS2_BA_NETH | MS2_BR_NETH))
	     || (r_list[r_idx].spells3 & (MS3_BR_WALL | MS3_BR_SLOW | MS3_BR_LITE | MS3_BR_TIME |
				 MS3_BR_GRAV | MS3_BR_DARK | MS3_BR_PLAS | MS3_ARROW_3 |
					MS3_DARK_STORM | MS3_MANA_STORM)))
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
 * We modify the given location to hold the location used, if any.
 * We return TRUE if a monster (or group of monsters) was summoned.
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
 * The things we can summon with the function below
 */

#define SUMMON_UNDEAD	11
#define SUMMON_DEMON	12
#define SUMMON_DRAGON	13
#define SUMMON_REPTILE	14
#define SUMMON_SPIDER	15
#define SUMMON_ANGEL	16
#define SUMMON_ANT	17
#define SUMMON_HOUND	18
#define SUMMON_JABBER	19
#define SUMMON_UNIQUE	31
#define SUMMON_WRAITH	32
#define SUMMON_GUNDEAD	51
#define SUMMON_ANCIENTD	52


/*
 * Hack -- in summon_specific() below, if the type is SUMMON_DEMON,
 * do not accept any demon whose level exceeds "summon_level"
 */
static int summon_level;


/* 
 * Check if monster race "m" is "okay" for summon type "type"
 */
static bool summon_specific_okay(int type, int m)
{
    bool okay = FALSE;
    
    /* Check our requirements */
    switch (type) {

	    case SUMMON_UNDEAD:
		okay = ((r_list[m].cflags2 & MF2_UNDEAD) &&
		    !(r_list[m].cflags2 & MF2_UNIQUE) &&
		    (r_list[m].level < dun_level + 5));
		break;

	    case SUMMON_DEMON:
		okay = ((r_list[m].cflags2 & MF2_DEMON) &&
		    !(r_list[m].cflags2 & MF2_UNIQUE) &&
		    (r_list[m].level <= summon_level));
		break;

	    case SUMMON_DRAGON:
		okay = ((r_list[m].cflags2 & MF2_DRAGON) &&
		        !(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_REPTILE:
		okay = ((r_list[m].r_char == 'R') &&
			!(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_SPIDER:
		okay = ((r_list[m].r_char == 'S') &&
			!(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_ANGEL:
		okay = ((r_list[m].r_char == 'A') &&
			!(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_ANT:
		okay = ((r_list[m].r_char == 'a') &&
			!(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_HOUND:
		okay = (((r_list[m].r_char == 'C') || (r_list[m].r_char == 'Z')) &&
			!(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_JABBER:
		okay = ((r_list[m].r_char == 'J') &&
		        !(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_UNIQUE:
		okay = (!(r_list[m].r_char == 'P') &&
			(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_WRAITH:
		okay = ((r_list[m].r_char == 'W') &&
			(r_list[m].cflags2 & MF2_UNIQUE));
		break;

	    case SUMMON_GUNDEAD:
		okay = ((r_list[m].r_char == 'L') ||
			(r_list[m].r_char == 'V') ||
			(r_list[m].r_char == 'W'));
		break;

	    case SUMMON_ANCIENTD:
		okay = (r_list[m].r_char == 'D');
		break;
    }

    /* Return the result */
    return (okay);
}


/*
 * Place a monster (of the specified "type") adjacent to the given
 * location, and re-set the given location to the location of the
 * summoned monster.  Return TRUE iff a monster was actually summoned.
 */
static int summon_specific(int *yp, int *xp, int type)
{
    register int        i, j, k;
    int                 l, m, ctr, summon = FALSE;
    register cave_type *cave_ptr;


    for (l = r_level[MAX_R_LEV]; l != 0;) {
	m = randint(l) - 1;

	for (ctr = 0; ctr <= 19;) {

	    if (summon_specific_okay(type, m)) {
		ctr = 20;
		l = 0;
	    }
	    else {
		m++;
		if (m > l)  ctr = 20;
		else ctr++;
	    }
	}
    }

    for (i = 0; i <= 9; i++) {

	j = *yp - 2 + randint(3);
	k = *xp - 2 + randint(3);

	if (in_bounds(j, k)) continue;

	    cave_ptr = &cave[j][k];

	    if (floor_grid_bold(j, k) && (cave_ptr->m_idx == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*yp = j;
		*xp = k;
	    }
    }
    return (summon);
}








/*
 * Summon a demon.  Hack -- enforce max-level 
 */
int summon_demon(int lev, int *y, int *x)
{
    int summon;
    summon_level = lev;
    summon = summon_specific(y, x, SUMMON_DEMON);
    return (summon);
}

/*
 * Summon things (see above)
 */

int summon_undead(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_UNDEAD);
    return (summon);
}

int summon_dragon(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_DRAGON);
    return (summon);
}

int summon_reptile(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_REPTILE);
    return (summon);
}

int summon_spider(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_SPIDER);
    return (summon);
}

int summon_angel(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_ANGEL);
    return (summon);
}

int summon_ant(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_ANT);
    return (summon);
}

int summon_hound(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_HOUND);
    return (summon);
}

int summon_jabberwock(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_JABBER);
    return (summon);
}

int summon_unique(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_UNIQUE);
    return (summon);
}

int summon_wraith(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_WRAITH);
    return (summon);
}

int summon_gundead(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_GUNDEAD);
    return (summon);
}

int summon_ancientd(int *y, int *x)
{
    int summon = summon_specific(y, x, SUMMON_ANCIENTD);
    return (summon);
}




/*
 */
void monster_name(char *m_name, monster_type *m_ptr)
{
    monster_race *r_ptr;

    /* Extract the monster race */
    r_ptr = &(r_list[m_ptr->r_idx]);

    if (!m_ptr->ml)
	(void)strcpy(m_name, "It");
    else {
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    (void)sprintf(m_name, "%s", r_ptr->name);
	else
	    (void)sprintf(m_name, "The %s", r_ptr->name);
    }
}



int unique_weapon(inven_type *t_ptr)
{
    const char *name;

    name = k_list[t_ptr->k_idx].name;
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
		       TR1_BRAND_ELEC | TR_LIGHT | TR1_SLAY_DEMON | TR2_RES_LITE);
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
	    t_ptr->flags1 = (TR3_REGEN | TR1_KILL_DRAGON | TR1_STR |
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
	    t_ptr->flags1 = (TR1_KILL_DRAGON | TR1_STR | TR1_SLAY_EVIL | TR1_SLAY_ANIMAL |
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
	t_ptr->flags1 = (TR1_KILL_DRAGON | TR1_CON | TR3_AGGRAVATE |
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
	t_ptr->flags1 = (TR1_KILL_DRAGON | TR2_RES_ELEC);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_BRAND_ELEC | TR3_ACTIVATE | TR2_RES_DARK);
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
	    t_ptr->flags2 |= (TR3_ACTIVATE | TR1_BRAND_ELEC | TR_ARTIFACT);
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
	    t_ptr->flags1 = (TR1_KILL_DRAGON | TR1_CON | TR2_FREE_ACT |
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
	t_ptr->flags1 = (TR1_KILL_DRAGON | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD |
		    TR2_RES_FIRE | TR2_RES_ACID | TR2_RES_COLD | TR2_RES_ELEC |
			TR2_FREE_ACT | TR3_SEE_INVIS | TR1_WIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_DEMON | TR1_BRAND_ELEC | TR2_RES_NEXUS);
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
			  | TR1_BRAND_ELEC | TR2_RES_DARK);
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
	t_ptr->flags1 = (TR1_CON | TR1_SLAY_ANIMAL | TR1_KILL_DRAGON |
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

    name = k_list[t_ptr->k_idx].name;
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



