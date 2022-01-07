/* File: misc2.c */
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
    if (cave[y][x].fval >= MIN_CLOSED_SPACE) return (FALSE);
    if (cave[y][x].cptr != 0) return (FALSE);

    /* And do not use special walls */
    if (cave[y][x].fval == NULL_WALL) return (FALSE);

    /* Or the player himself */
    if ((y == char_row) && (x == char_col)) return (FALSE);

    /* Use it */
    return TRUE;
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
    register monster_type *mon_ptr;

    msg_print("Compacting monsters...");

    cur_dis = 66;
    delete_any = FALSE;
    do {
	for (i = mfptr - 1; i >= MIN_M_IDX; i--) {
	    mon_ptr = &m_list[i];
	    if ((cur_dis < mon_ptr->cdis) && (randint(3) == 1)) {
	    /* Don't compact Melkor! */
		if (c_list[mon_ptr->mptr].cmove & CM_WIN)
		/* do nothing */
		    ;

	    /* in case this is called from within creatures(), this is a
	     * horrible hack, the m_list/creatures() code needs to be
	     * rewritten 
	     */
		else if (hack_monptr < i) {
		    delete_monster(i);
		    delete_any = TRUE;
		} else

		/* fix1_delete_monster() does not decrement mfptr, so don't
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
    while (!delete_any);
    return TRUE;
}


/*
 * Returns a pointer to next free space			-RAK-
 */
int m_pop(void)
{
    if (mfptr == MAX_M_IDX)
	if (!compact_monsters())
	    return (-1);
    return (mfptr++);
}




/*
 * Places a monster at given location			-RAK-
 */
int place_monster(int y, int x, int r_idx, int slp)
{
    register int           cur_pos, j, ny, nx, count;
    register monster_type *m_ptr;
    char                   buf[100];

    if ((r_idx < 0) || (r_idx >= MAX_R_IDX))
	return FALSE;		/* another paranoia check -CFT */

    if (!test_place(y, x))
	return FALSE;		/* YA paranoia check -CFT */

    if (c_list[r_idx].cdefense & UNIQUE) {
	if (u_list[r_idx].exist) {
	    if (wizard) {
		(void)sprintf(buf, "Tried to create %s but exists.", c_list[r_idx].name);
		msg_print(buf);
	    }
	    return FALSE;
	}
	if (u_list[r_idx].dead) {
	    if (wizard) {
		(void)sprintf(buf, "Tried to create %s but dead.", c_list[r_idx].name);
		msg_print(buf);
	    }
	    return FALSE;
	}
	u_list[r_idx].exist = 1;
    }
    cur_pos = m_pop();		   /* from um55, paranoia error check... */
    if (cur_pos == -1)
	return FALSE;

    if ((wizard || peek) && (c_list[r_idx].cdefense & UNIQUE))
	msg_print(c_list[r_idx].name);
    if (c_list[r_idx].level > (unsigned)dun_level) {
	int                 c;

	rating += ((c = c_list[r_idx].level - dun_level) > 30) ? 15 : c / 2;
	if (c_list[r_idx].cdefense & UNIQUE)
	    rating += (c_list[r_idx].level - dun_level) / 2;
    }
    m_ptr = &m_list[cur_pos];
    m_ptr->fy = y;
    m_ptr->fx = x;
    m_ptr->mptr = r_idx;
    if ((c_list[r_idx].cdefense & MAX_HP) )
	m_ptr->hp = max_hp(c_list[r_idx].hd);
    else
	m_ptr->hp = pdamroll(c_list[r_idx].hd);
    m_ptr->maxhp = m_ptr->hp;
    m_ptr->cspeed = c_list[r_idx].speed - 10;
    m_ptr->stunned = 0;
    m_ptr->confused = 0;
    m_ptr->monfear = 0;
    m_ptr->cdis = distance(char_row, char_col, y, x);
    m_ptr->ml = FALSE;
    cave[y][x].cptr = cur_pos;

    if (slp) {
	if (c_list[r_idx].sleep == 0)
	    m_ptr->csleep = 0;
	else
	    m_ptr->csleep = ((int)c_list[r_idx].sleep * 2) +
			     randint((int)c_list[r_idx].sleep * 10);
    } else
  /* to give the player a sporting chance, any monster that appears in
         line-of-sight and can cast spells or breathe, should be asleep.
          This is an extension of Um55's sleeping dragon code... */
    if (((c_list[r_idx].spells & (CAUSE_LIGHT|CAUSE_SERIOUS|HOLD_PERSON|
                                  BLINDNESS|CONFUSION|FEAR|SLOW|BREATH_L|
                                  BREATH_G|BREATH_A|BREATH_FR|BREATH_FI|
                                  FIRE_BOLT|FROST_BOLT|ACID_BOLT|MAG_MISS|
                                  CAUSE_CRIT|FIRE_BALL|FROST_BALL|MANA_BOLT))
          || (c_list[r_idx].spells2 & (BREATH_CH|BREATH_SH|BREATH_SD|BREATH_CO|
                                  BREATH_DI|BREATH_LD|LIGHT_BOLT|LIGHT_BALL|
                                  ACID_BALL|TRAP_CREATE|RAZOR|MIND_BLAST|
                                  MISSILE|PLASMA_BOLT|NETHER_BOLT|ICE_BOLT|
                                  FORGET|BRAIN_SMASH|ST_CLOUD|TELE_LEV|
                                  WATER_BOLT|WATER_BALL|NETHER_BALL|BREATH_NE))
          || (c_list[r_idx].spells3 & (BREATH_WA|BREATH_SL|BREATH_LT|BREATH_TI|
                                  BREATH_GR|BREATH_DA|BREATH_PL|ARROW|
                                  DARK_STORM|MANA_STORM)))
       && (los(y,x, char_row, char_col)))
      m_ptr->csleep = randint(4);   /* if asleep only to prevent
                                       * summon-breathe-breathe-breathe-die,
                                       * then don't sleep long -CFT */
    else {
	m_ptr->csleep = 0;
    }

    update_mon(cur_pos);	   /* light up the monster if we can see it... -CFT */

/* Unique kobolds, Liches, orcs, Ogres, Trolls, yeeks, and & demons -DGK
 * But not skeletons, because that would include druj, making Cantoras
 * amazingly tough -CFT
 */
    if (c_list[r_idx].cdefense & UNIQUE) {
	j = c_list[r_idx].cchar;
	if ((j=='k')||(j=='L')||(j=='o')||(j=='O')||(j=='T')||(j=='y')||
	    (j=='I')||(j=='&')) {
	    for (z = MAX_R_IDX-1; z>=0; z--) {
		if ((c_list[z].cchar == j) &&
		    (c_list[z].level <= c_list[r_idx].level) &&
		    !(c_list[z].cdefense & UNIQUE)) {
		    count = 0;
		    do {
			ny=y+randint(7)-4;
			nx=x+randint(7)-4;
			count++;
		    } while (!test_place(ny,nx) && (count<51));
		    if ((j=='k')||(j=='y')||(j=='&')||(c_list[z].cdefense & GROUP))
			place_group(ny,nx,z,slp);
		    else
			place_monster(ny,nx,z,slp);
		}
	}
    }
    return TRUE;
}


/*
 * Places a "winning" monster at given location	    -RAK-
 */
int place_win_monster()
{
    register int           y, x, cur_pos;
    register monster_type *mon_ptr;

    if (!total_winner) {
	cur_pos = m_pop();
    /* paranoia error check, from um55 -CFT */
	if (cur_pos == -1)
	    return FALSE;

	if (wizard || peek)
	    msg_print("Placing win monster");

	mon_ptr = &m_list[cur_pos];
	do {
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	       || (cave[y][x].tptr != 0) ||
	       (distance(y, x, char_row, char_col) <= MAX_SIGHT));
	mon_ptr->fy = y;
	mon_ptr->fx = x;
	mon_ptr->mptr = MAX_R_IDX - 2;
	if (c_list[mon_ptr->mptr].cdefense & MAX_HP)
	    mon_ptr->hp = max_hp(c_list[mon_ptr->mptr].hd);
	else
	    mon_ptr->hp = pdamroll(c_list[mon_ptr->mptr].hd);
	mon_ptr->cspeed = c_list[mon_ptr->mptr].speed - 10;
	mon_ptr->stunned = 0;
	mon_ptr->cdis = distance(char_row, char_col, y, x);
	cave[y][x].cptr = cur_pos;
	mon_ptr->csleep = 0;
    }
    return TRUE;
}


static char *cap(char *str)
{
    if ((*str >= 'a') && (*str <= 'z'))
	*str = *str - 'a' + 'A';
    return str;
}


void set_ghost(monster_race *g, char *name, int gr, int gc, int lev)
{
    char ghost_race[20];
    char ghost_class[20];
    int  i;

    /* Allocate storage for name -TL -- braindamaged ghost name spoo -CWS */
    if (c_list[MAX_R_IDX - 1].name == NULL) {
	c_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	C_WIPE(c_list[MAX_R_IDX - 1].name, 101, char);
	*((char *) c_list[MAX_R_IDX - 1].name) = 'A';
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
    g->level = lev;
    g->sleep = 0;
    g->aaf = 100;
    g->mexp = lev * 5 + 5;
    g->spells2 = NONE8;
    if (!dun_level) {
	sprintf((char *)g->name, "%s, the %s %s", cap(name),
		cap(ghost_race), cap(ghost_class));
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_90 | HAS_60 | GOOD);

	if (lev > 10) g->cmove |= (HAS_1D2);
	if (lev > 18) g->cmove |= (HAS_2D2);
	if (lev > 23) g->cmove |= (HAS_4D2);
	if (lev > 40) {
	    g->cmove |= (SPECIAL);
	    g->cmove &= (~HAS_4D2);
	}

	for (i = 0; i <= (lev / 5); i++) { /* Add some random resists -DGK */
	    switch ((int) randint(13)) {
	      case 1:
	      case 2:
	      case 3:
		g->cdefense |= (IM_FIRE);
	      case 4:
	      case 5:
	      case 6:
		g->cdefense |= (IM_ACID);
	      case 7:
	      case 8:
	      case 9:
		g->cdefense |= (IM_FROST);
	      case 10:
	      case 11:
	      case 12:
		g->cdefense |= (IM_LIGHTNING);
	      case 13:
		g->cdefense |= (IM_POISON);
	    }
	}

	switch (gc) {
	  case 0:		   /* Warrior */
	    g->spells = NONE8;
	    break;
	  case 1:		   /* Mage */
	    g->spells |= (0x3L | BLINK | MAG_MISS |
			   SLOW | CONFUSION);
	    if (lev > 5) g->spells2 |= ST_CLOUD;
	    if (lev > 7) g->spells2 |= LIGHT_BOLT;
	    if (lev > 10) g->spells |= FROST_BOLT;
	    if (lev > 12) g->spells |= TELE;
	    if (lev > 15) g->spells |= ACID_BOLT;
	    if (lev > 20) g->spells |= FIRE_BOLT;
	    if (lev > 25) g->spells |= FROST_BALL;
	    if (lev > 25) g->spells2 |= HASTE;
	    if (lev > 30) g->spells |= FIRE_BALL;
	    if (lev > 40) g->spells |= MANA_BOLT;
	    break;
	  case 3:		   /* Rogue */
	    g->spells |= (0x5L | BLINK);
	    if (lev > 10) g->spells |= CONFUSION;
	    if (lev > 18) g->spells |= SLOW;
	    if (lev > 25) g->spells |= TELE;
	    if (lev > 30) g->spells |= HOLD_PERSON;
	    if (lev > 35) g->spells |= TELE_TO;
	    break;
	  case 4:		   /* Ranger */
	    g->spells |= (0x8L | MAG_MISS);
	    if (lev > 5) g->spells2 |= ST_CLOUD;
	    if (lev > 7) g->spells2 |= LIGHT_BOLT;
	    if (lev > 10) g->spells |= FROST_BOLT;
	    if (lev > 18) g->spells |= ACID_BOLT;
	    if (lev > 25) g->spells |= FIRE_BOLT;
	    if (lev > 30) g->spells |= FROST_BALL;
	    if (lev > 35) g->spells |= FIRE_BALL;
	    break;
	  case 2:		   /* Priest */
	  case 5:		   /* Paladin */
	    g->spells |= (0x4L | CAUSE_LIGHT | FEAR);
	    if (lev > 5) g->spells2 |= HEAL;
	    if (lev > 10) g->spells |= (CAUSE_SERIOUS | BLINDNESS);
	    if (lev > 18) g->spells |= HOLD_PERSON;
	    if (lev > 25) g->spells |= CONFUSION;
	    if (lev > 30) g->spells |= CAUSE_CRIT;
	    if (lev > 35) g->spells |= MANA_DRAIN;
	    break;
	}

	g->cdefense |= (CHARM_SLEEP | EVIL);

	if (gr == 6) g->cdefense |= ORC;
	else if (gr == 7) g->cdefense |= TROLL;

	g->ac = 15 + randint(15);
	if (gc == 0 || gc >= 3) g->ac += randint(60);
	/* High level mages and rogues are fast... */
	if ((gc == 1 || gc == 3) && lev > 25) g->speed = 12;
	else g->speed = 11;
	g->cchar = 'p';
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
	  case 5:
	  case 4:
	    g->damage[2] = g->damage[3] = g->damage[1];
	    break;
	}
	return;
    }
    switch ((int) (g->level / 4) + randint(3)) {
      case 1:
      case 2:
      case 3:
	sprintf((char *)g->name, "%s, the Skeleton %s", name, ghost_race);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_90 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA);
	if (gr == 6) g->cdefense |= ORC;
	else if (gr == 7) g->cdefense |= TROLL;
	g->ac = 26;
	g->speed = 11;
	g->cchar = 's';
	g->hd[1] = 1;
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;
      case 4:
      case 5:
	sprintf((char *)g->name, "%s, the %s zombie", name, cap(ghost_race));
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_60 | HAS_90 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | NO_INFRA);
	if (gr == 6) g->cdefense |= ORC;
	else if (gr == 7) g->cdefense |= TROLL;
	g->ac = 30;
	g->speed = 11;
	g->cchar = 'z';
	g->hd[1] *= 2;
	g->damage[0] = 8;
	g->damage[1] = 0;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;
      case 6:
	sprintf((char *) g->name, "%s, the Poltergeist", name);
	g->cmove |= (MV_INVIS | MV_ATT_NORM | CARRY_OBJ |
		     GOOD | HAS_1D2 | MV_75 | THRO_WALL);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA);
	g->ac = 20;
	g->speed = 13;
	g->cchar = 'G';
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 93;
	g->damage[3] = 93;
	g->mexp = (g->mexp * 3) / 2;
	break;
      case 7:
      case 8:
	sprintf((char *)g->name, "%s, the Mummified %s", name, cap(ghost_race));
	g->cmove |= (MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA);
	if (gr == 6) g->cdefense |= ORC;
	else if (gr == 7) g->cdefense |= TROLL;
	g->ac = 35;
	g->speed = 11;
	g->cchar = 'M';
	g->hd[1] *= 2;
	g->damage[0] = 16;
	g->damage[1] = 16;
	g->damage[2] = 16;
	g->damage[3] = 0;
	g->mexp = (g->mexp * 3) / 2;
	break;
      case 9:
      case 10:
	sprintf((char *)g->name, "%s%s spirit", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA);
	g->ac = 20;
	g->speed = 11;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 19;
	g->damage[1] = 185;
	g->damage[2] = 99;
	g->damage[3] = 178;
	g->mexp = g->mexp * 3;
	break;
      case 11:
	sprintf((char *)g->name, "%s%s ghost", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (0xFL | HOLD_PERSON | MANA_DRAIN | BLINDNESS);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA);
	g->ac = 40;
	g->speed = 12;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = (g->mexp * 7) / 2;
	break;
      case 12:
	sprintf((char *) g->name, "%s, the Vampire", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | GOOD);
	g->spells |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_SERIOUS);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA | HURT_LIGHT);
	g->ac = 40;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 3;
	break;
      case 13:
	sprintf((char *)g->name, "%s%s Wraith", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_4D2 | HAS_2D2 | GOOD);
	g->spells |= (0x7L | HOLD_PERSON | FEAR | BLINDNESS | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA |
			HURT_LIGHT);
	g->ac = 60;
	g->speed = 12;
	g->cchar = 'W';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 5;
	break;
      case 14:
	sprintf((char *) g->name, "%s, the Vampire Lord", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | SPECIAL);
	g->spells |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA | HURT_LIGHT);
	g->ac = 80;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 20;
	g->damage[3] = 198;
	g->mexp = g->mexp * 20;
	break;
      case 15:
	sprintf((char *)g->name, "%s%s ghost", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | SPECIAL);
	g->spells |= (0x5L | HOLD_PERSON | MANA_DRAIN | BLINDNESS | CONFUSION);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | IM_POISON | NO_INFRA);
	g->ac = 90;
	g->speed = 13;
	g->cchar = 'G';
	g->hd[1] *= 3;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 20;
	break;
      case 17:
	sprintf((char *)g->name, "%s, the Lich", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | HAS_1D2 | SPECIAL);
	g->spells |= (0x3L | FEAR | CAUSE_CRIT | TELE_TO | BLINK | S_UNDEAD | FIRE_BALL |
		      FROST_BALL | HOLD_PERSON | MANA_DRAIN | BLINDNESS | CONFUSION | TELE);
	g->spells2 |= (BRAIN_SMASH | RAZOR);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA | IM_POISON
			| INTELLIGENT);
	g->ac = 120;
	g->speed = 12;
	g->cchar = 'L';
	g->hd[1] *= 3;
	g->hd[0] *= 2;
	g->damage[0] = 181;
	g->damage[1] = 201;
	g->damage[2] = 214;
	g->damage[3] = 181;
	g->mexp = g->mexp * 50;
	break;
      case 18:
      default:
	sprintf((char *)g->name, "%s%s ghost", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ |
		     HAS_1D2 | HAS_2D2 | SPECIAL);
	g->spells |= (0x2L | HOLD_PERSON | MANA_DRAIN | BLINDNESS | CONFUSION | TELE_TO);
	g->spells2 |= (NETHER_BOLT | NETHER_BALL | BRAIN_SMASH | TELE_LEV);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA |
			INTELLIGENT);
	g->ac = 130;
	g->speed = 13;
	g->cchar = 'G';
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
 * Places a monster at given location			-RAK-
 */
int place_ghost()
{
    register int           y, x, cur_pos;
    register monster_type  *m_ptr;
    monster_race         *ghost = &c_list[MAX_R_IDX - 1];
    char                   tmp[100];
    char                   name[100];
    int                    i, j, level;
    int                    ghost_race;
    int                    cl;

    if (!dun_level) {
	FILE *fp;

	if (py.misc.lev < 5 || randint(10) > 1)
	    return 0;
	sprintf(tmp, "%s/%d", ANGBAND_DIR_BONES, py.misc.lev);
	if ((fp = my_tfopen(tmp, "r")) != NULL) {
	    if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
		fclose(fp);
		if (wizard)
		    msg_print("Town:Failed to scan in info properly!");
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
    } else {
	if (14 > randint((dun_level / 2) + 11))
	    return 0;
	if (randint(3) == 1) {
	    FILE *fp;

	    sprintf(tmp, "%s/%d", ANGBAND_DIR_BONES, dun_level);
	    if ((fp = my_tfopen(tmp, "r")) != NULL) {
		if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
		    fclose(fp);
		    if (wizard)
			msg_print("Ghost:Failed to scan in info properly!");
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
    set_ghost(ghost, name, ghost_race, cl, level);
    if (wizard || peek)
	msg_print(ghost->name);
    cur_pos = m_pop();
    m_ptr = &m_list[cur_pos];

    do {
	y = randint(cur_height - 2);
	x = randint(cur_width - 2);
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	     || (cave[y][x].tptr != 0) ||
	     (distance(y, x, char_row, char_col) <= MAX_SIGHT));
    m_ptr->fy = y;
    m_ptr->fx = x;
    m_ptr->mptr = (MAX_R_IDX - 1);
    m_ptr->hp = (s16b) ghost->hd[0] * (s16b) ghost->hd[1];
/* the c_list speed value is 10 greater, so that it can be a byte */
    m_ptr->cspeed = c_list[mon_ptr->mptr].speed - 10;
    m_ptr->stunned = 0;
    m_ptr->cdis = distance(char_row, char_col, y, x);
    cave[y][x].cptr = cur_pos;
    m_ptr->csleep = 0;
    return 1;
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

again:
    if (level == 0)
	i = randint(m_level[0]) - 1;
    else {
	if (level > MAX_R_LEV)
	    level = MAX_R_LEV;
	if (randint(MON_NASTY) == 1) {
	    i = level / 4 + 1;	   /* be a little more civilized about monster depths */
	    if (i > 4)		   /* for the first levels -CWS */
		i = 4;
	    level = level + MY_ABS(i) + 1;
	    if (level > MAX_R_LEV)
		level = MAX_R_LEV;
	} else {


	    num = m_level[level] - m_level[0];
	    i = randint(num) - 1;
	    j = randint(num) - 1;
	    if (j > i)
		i = j;
	    level = c_list[i + m_level[0]].level;
	}
	i = m_level[level] - m_level[level - 1];
	if (i == 0)
	    i++;
	i = randint(i) - 1 + m_level[level - 1];
    }
    if ((c_list[i].level > old) && (c_list[i].cdefense & UNIQUE))
	goto again;
    if ((c_list[i].level > (unsigned) dun_level) &&
	(c_list[i].cdefense & QUESTOR))
	goto again;
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

again:
    if (level == 0)
	i = randint(m_level[0]) - 1;
    else {
	if (level > MAX_R_LEV)
	    level = MAX_R_LEV;
	num = m_level[level] - m_level[0];
	i = randint(num) - 1;
	i += 15;
	if (i >= num)
	    i = num - 1;
	j = randint(num) - 1;
	if (j > i)
	    i = j;
	j = randint(num) - 1;
	if (j > i)
	    i = j;
	level = c_list[i + m_level[0]].level;
	i = m_level[level] - m_level[level - 1];
	if (i == 0)
	    i = 1;
	i = randint(i) - 1 + m_level[level - 1];
    }
    if ((c_list[i].level > old) && (c_list[i].cdefense & UNIQUE))
	goto again;
    if (( c_list[i].level > (unsigned) dun_level) &&
	(c_list[i].cdefense & QUESTOR))
	goto again;
    return i;
}


void place_group(int y, int x, int r_idx, int slp)
{
/* prevent level rating from skyrocketing if they are out of depth... */
    int old = rating;
    int extra = 0;

    if (c_list[r_idx].level > (unsigned) dun_level)
	extra = 0 - randint(c_list[r_idx].level - dun_level);
				/* reduce size of group if out-of-depth */
    else if (c_list[r_idx].level < (unsigned) dun_level)
/* if monster is deeper than normal, then travel in bigger packs -CFT */
	extra = randint(dun_level - c_list[r_idx].level);

    if (extra > 12)
	extra = 12;		/* put an upper bounds on it... -CFT */
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

    for (i = 0; i < num; i++) {
	do {
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while (cave[y][x].fval >= MIN_CLOSED_SPACE || (cave[y][x].cptr != 0) ||
	       (distance(y, x, char_row, char_col) <= dis));
	do {
	r_idx = get_mons_num(dun_level);
	} while (randint(c_list[r_idx].rarity) > 1);

    /*
     * to give the player a sporting chance, any monster that appears in
     * line-of-sight and can cast spells or breathe, should be asleep. This
     * is an extension of Um55's sleeping dragon code... 
     */

	if (((c_list[r_idx].spells & (CAUSE_LIGHT | CAUSE_SERIOUS | HOLD_PERSON |
			    BLINDNESS | CONFUSION | FEAR | SLOW | BREATH_L |
			       BREATH_G | BREATH_A | BREATH_FR | BREATH_FI |
			     FIRE_BOLT | FROST_BOLT | ACID_BOLT | MAG_MISS |
			   CAUSE_CRIT | FIRE_BALL | FROST_BALL | MANA_BOLT))
	     || (c_list[r_idx].spells2 & (BREATH_CH | BREATH_SH | BREATH_SD | BREATH_CO |
			   BREATH_DI | BREATH_LD | LIGHT_BOLT | LIGHT_BALL |
			      ACID_BALL | TRAP_CREATE | RAZOR | MIND_BLAST |
			    MISSILE | PLASMA_BOLT | NETHER_BOLT | ICE_BOLT |
				FORGET | BRAIN_SMASH | ST_CLOUD | TELE_LEV |
			 WATER_BOLT | WATER_BALL | NETHER_BALL | BREATH_NE))
	     || (c_list[r_idx].spells3 & (BREATH_WA | BREATH_SL | BREATH_LT | BREATH_TI |
				 BREATH_GR | BREATH_DA | BREATH_PL | ARROW |
					DARK_STORM | MANA_STORM)))
	    && (los(y, x, char_row, char_col)))
	    slp = TRUE;

	if (!(c_list[r_idx].cdefense & GROUP)) {
	    place_monster(y, x, r_idx, slp);
	}
	else {
	    place_group(y, x, r_idx, slp);
	}
    }
}


/*
 * Places creature adjacent to given location -RAK-
 */
int summon_monster(int *y, int *x, int slp)
{
    register int        i, j, k;
    int                 l, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = get_mons_num(dun_level + MON_SUMMON_ADJ);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (foor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
		if (c_list[l].cdefense & GROUP)
		    place_group(j, k, l, slp);
		else
		    place_monster(j, k, l, slp);
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


/* Places undead adjacent to given location		-RAK-	 */
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
	    if ((c_list[m].cdefense & UNDEAD) && !(c_list[m].cdefense & UNIQUE) &&
		(c_list[m].level < dun_level + 5)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* As for summon undead */
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
	    if (c_list[m].cdefense & DEMON && !(c_list[m].cdefense & UNIQUE) &&
		(c_list[m].level <= lev)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* As for summon demon:-) ~Ludwig */
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
	    if (c_list[m].cdefense & DRAGON && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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
	    if (c_list[m].cchar == 'W' && (c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon reptiles */
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
	    if (c_list[m].cchar == 'R' && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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


/* As for summon dragon, but keys on character ~Decado */
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
	    if (c_list[m].cchar == 'S' && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* As for summon dragon, but keys on character ~Decado */
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
	    if (c_list[m].cchar == 'A' && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon ants */
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
	    if (c_list[m].cchar == 'a' && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon uniques */
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
	    if (!(c_list[m].cchar == 'P') && (c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon jabberwocks, for extra effect to the summon_unique spell */
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
	    if (c_list[m].cchar == 'J' && !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon greater undead */
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
	    if ((c_list[m].cchar == 'L') || (c_list[m].cchar == 'V')
		|| (c_list[m].cchar == 'W')) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* Summon ancient dragons */
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
	    if (c_list[m].cchar == 'D') {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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

/* As for summon hound, but keys on character ~Decado */
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
	    if ((c_list[m].cchar == 'C' || c_list[m].cchar == 'Z')
		&& !(c_list[m].cdefense & UNIQUE)) {
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
	    if (floor_grid_bold(j, k) && (cave_ptr->cptr == 0)) {
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


/* If too many objects on floor level, delete some of them */
static void compact_objects()
{
    register int        i, j;
    int                 ctr, cur_dis, chance;
    register cave_type *cave_ptr;

    msg_print("Compacting objects...");

    ctr = 0;
    cur_dis = 66;
    do {
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++) {
		cave_ptr = &cave[i][j];
		if ((cave_ptr->tptr != 0)
		    && (distance(i, j, char_row, char_col) > cur_dis)) {
		    switch (i_list[cave_ptr->tptr].tval) {
		      case TV_VIS_TRAP:
			chance = 15;
			break;
		      case TV_INVIS_TRAP:
		      case TV_RUBBLE:
		      case TV_OPEN_DOOR:
		      case TV_CLOSED_DOOR:
			chance = 5;
			break;
		      case TV_UP_STAIR:
		      case TV_DOWN_STAIR:
		      case TV_STORE_DOOR:
			chance = 0;
			break;
		      case TV_SECRET_DOOR:	/* secret doors */
			chance = 3;
			break;
		      default:
			if ((i_list[cave_ptr->tptr].tval >= TV_MIN_WEAR) &&
			    (i_list[cave_ptr->tptr].tval <= TV_MAX_WEAR) &&
			    (i_list[cave_ptr->tptr].flags2 & TR_ARTIFACT))
			    chance = 0;	/* don't compact artifacts -CFT */
			else
			    chance = 10;
		    }
		    if (randint(100) <= chance) {
			(void)delete_object(i, j);
			ctr++;
		    }
		}
	    }
	if (ctr == 0)
	    cur_dis -= 6;
    }
    while (ctr <= 0);
    if (cur_dis < 66)
	prt_map();
}

/* Gives pointer to next free space			-RAK-	 */
int i_pop()
{
    if (tcptr == MAX_TALLOC)
	compact_objects();
    return (tcptr++);
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

    if (x != tcptr - 1) {
	i_list[x] = i_list[tcptr - 1];

    /* must change the tptr in the cave of the object just moved */
	for (i = 0; i < cur_height; i++)
	    for (j = 0; j < cur_width; j++)
		if (cave[i][j].tptr == tcptr - 1)
		    cave[i][j].tptr = x;
    }
    tcptr--;
    invcopy(&i_list[tcptr], OBJ_NOTHING);
}


/* Boolean : is object enchanted	  -RAK- */
int magik(int chance)
{
    if (randint(100) <= chance)
	return (TRUE);
    else
	return (FALSE);
}


/* Enchant a bonus based on degree desired -RAK- */
/*
 * Lets just change this to make sense.  Now it goes from base to limit,
 * roughly proportional to the level.... -CWS 
 */

int m_bonus(int base, int limit, int level)
{
    register int x, stand_dev, tmp, diff = limit - base;

/* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100)) + OBJ_STD_MIN;
/* check for level > max_std to check for overflow... */
    if (stand_dev > 40)
	stand_dev = 40;
/* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);
    x = (tmp * diff / 150) + (level * limit / 200) + base;
    if (x < base)
	return (base);
    else
	return (x);
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
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_UNDEAD | TR_SLAY_EVIL | TR_REGEN |
		     TR_SPEED | TR_RES_COLD | TR_FROST_BRAND | TR_FREE_ACT |
			    TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_LIGHT | TR_ACTIVATE
			      | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->p1 = 1;
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
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_EVIL | TR_FREE_ACT |
		     TR_SUST_STAT | TR_STR | TR_RES_FIRE | TR_FLAME_TONGUE);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_ACTIVATE | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SEE_INVIS | TR_SLAY_EVIL | TR_FREE_ACT | TR_RES_ELEC
			    | TR_STR | TR_CON);
	    t_ptr->flags2 |= (TR_ARTIFACT |
		       TR_LIGHTNING | TR_LIGHT | TR_SLAY_DEMON | TR_RES_LT);
	    t_ptr->p1 = 2;
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
	    t_ptr->flags |= (TR_SEE_INVIS | TR_CHR | TR_DEX | TR_STEALTH | TR_FFALL);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 2;
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
	    t_ptr->flags = (TR_REGEN | TR_SLAY_X_DRAGON | TR_STR |
			    TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_ARTIFACT);
	    t_ptr->p1 = 2;
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
	    t_ptr->flags = (TR_SLAY_X_DRAGON | TR_STR | TR_SLAY_EVIL | TR_SLAY_ANIMAL |
		  TR_SLAY_UNDEAD | TR_AGGRAVATE | TR_CHR | TR_FLAME_TONGUE |
		  TR_SEE_INVIS | TR_RES_FIRE | TR_FREE_ACT | TR_INFRA);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC | TR_SLAY_GIANT
			      | TR_SLAY_DEMON | TR_RES_CHAOS);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SPEED | TR_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = -1;
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
	    t_ptr->flags = (TR_FFALL | TR_DEX |
			    TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SLOW_DIGEST | TR_SEARCH | TR_FLAME_TONGUE |
			    TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_RES_LT);
	    t_ptr->p1 = 3;
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
	    t_ptr->flags = (TR_SLOW_DIGEST | TR_SEARCH | TR_RES_ELEC);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_LIGHTNING);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SLOW_DIGEST | TR_STEALTH | TR_FROST_BRAND |
			    TR_RES_COLD);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT);
	    t_ptr->p1 = 3;
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
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_CON | TR_AGGRAVATE |
			TR_CURSED | TR_SLAY_EVIL);
	t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_RES_DISENCHANT
			  | TR_ARTIFACT);
	t_ptr->p1 = 5;
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
	t_ptr->flags = (TR_DEX | TR_INT | TR_FREE_ACT | TR_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 3;
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
	t_ptr->flags = (TR_FLAME_TONGUE | TR_SLAY_UNDEAD | TR_RES_FIRE |
			TR_FFALL | TR_CHR | TR_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_SOUND | TR_SLAY_GIANT);
	t_ptr->p1 = 3;
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
	t_ptr->flags = (TR_WIS | TR_REGEN | TR_FROST_BRAND | TR_RES_COLD | TR_INFRA);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT |
			  TR_ACTIVATE | TR_RES_LT);
	t_ptr->p1 = 4;
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
	t_ptr->flags = (TR_FROST_BRAND | TR_FLAME_TONGUE | TR_RES_FIRE | TR_RES_COLD |
			TR_SLOW_DIGEST | TR_INT | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_DEMON | TR_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 2;
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
	t_ptr->flags = (TR_STR | TR_FLAME_TONGUE | TR_SLAY_EVIL | TR_SLAY_DRAGON |
		   TR_SLAY_ANIMAL | TR_TUNNEL | TR_AGGRAVATE | TR_RES_FIRE);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_IM_FIRE | TR_RES_CHAOS
			  | TR_RES_DISENCHANT | TR_RES_DARK);
	t_ptr->p1 = 6;
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
	t_ptr->flags = (TR_DEX | TR_CHR | TR_FREE_ACT | TR_RES_FIRE | TR_RES_COLD |
			TR_SEE_INVIS | TR_FLAME_TONGUE | TR_FROST_BRAND);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHT | TR_ACTIVATE | TR_RES_LT);
	t_ptr->p1 = 3;
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
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_RES_ELEC);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHTNING | TR_ACTIVATE | TR_RES_DARK);
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_DEX);
	t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_CON);
	t_ptr->flags2 |= (TR_SLAY_ORC | TR_SLAY_TROLL | TR_SLAY_GIANT | TR_ARTIFACT);
	t_ptr->p1 = 3;
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
	    t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_STR | TR_DEX |
			     TR_SLAY_UNDEAD);
	    t_ptr->flags2 |= (TR_RES_DARK | TR_RES_LT | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
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
	    t_ptr->flags = (TR_SEE_INVIS | TR_FREE_ACT | TR_DEX | TR_REGEN |
			    TR_SLOW_DIGEST | TR_SLAY_ANIMAL | TR_SLAY_DRAGON |
			    TR_RES_ACID);
	    t_ptr->flags2 |= (TR_IM_ACID | TR_HOLD_LIFE | TR_ACTIVATE
			    | TR_RES_NETHER | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_DEX | TR_SLAY_UNDEAD
			 | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ATTACK_SPD);
	t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_SLAY_EVIL | TR_RES_FIRE | TR_RES_COLD
			 | TR_RES_ELEC | TR_RES_ACID);
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_FFALL | TR_REGEN | TR_STEALTH | TR_RES_FIRE |
			 TR_RES_COLD | TR_RES_ACID | TR_RES_ELEC | TR_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_SLAY_DRAGON | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_SLAY_GIANT | TR_SLAY_ORC | TR_SLAY_TROLL | TR_ARTIFACT |
			  TR_ATTACK_SPD);
	t_ptr->p1 = 1;
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
	t_ptr->flags |= (TR_RES_COLD | TR_FROST_BRAND | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_LIGHT | TR_RES_LT | TR_ARTIFACT);
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL | TR_SLAY_UNDEAD | TR_SLAY_DRAGON);
	t_ptr->flags2 |= (TR_SLAY_GIANT | TR_SLAY_ORC | TR_SLAY_TROLL | TR_ARTIFACT);
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
	t_ptr->flags = (TR_STEALTH | TR_RES_FIRE | TR_FLAME_TONGUE | TR_SLAY_EVIL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ACTIVATE | TR_RES_CONF);
	t_ptr->p1 = 2;
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
	t_ptr->flags = (TR_REGEN | TR_SLOW_DIGEST | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ATTACK_SPD);
	t_ptr->p1 = 2;
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
	t_ptr->flags = (TR_DEX | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 5;
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
	    t_ptr->flags = (TR_WIS | TR_FROST_BRAND |
			    TR_RES_COLD | TR_FREE_ACT | TR_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD | TR_SLAY_UNDEAD |
			    TR_SEE_INVIS | TR_STEALTH);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
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
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_SEE_INVIS | TR_SEARCH | TR_INT |
			    TR_RES_FIRE | TR_FFALL | TR_INFRA);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_SLAY_GIANT | TR_RES_LT
			      | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_DEX | TR_SLAY_EVIL | TR_SUST_STAT |
			    TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_DARK | TR_ARTIFACT);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags = (TR_RES_ELEC);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHTNING | TR_ARTIFACT);
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
	    t_ptr->flags = TR_POISON;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_DISENCHANT | TR_ARTIFACT);
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
	    t_ptr->flags = (TR_FROST_BRAND | TR_RES_COLD | TR_REGEN | TR_SLOW_DIGEST |
			    TR_DEX | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_EVIL |
			 TR_SLAY_UNDEAD | TR_DEX | TR_CON | TR_STR |
			 TR_FREE_ACT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_ORC | TR_LIGHT | TR_RES_LT |
			  TR_ATTACK_SPD);
	t_ptr->p1 = 2;
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
	    t_ptr->flags = (TR_SLAY_X_DRAGON | TR_CON | TR_FREE_ACT |
			    TR_RES_FIRE | TR_RES_ACID);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_DARK
			      | TR_RES_LT | TR_RES_CHAOS | TR_ARTIFACT);
	    t_ptr->p1 = 3;
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
	    t_ptr->flags = (TR_STATS | TR_SLAY_EVIL | TR_SLAY_UNDEAD | TR_FROST_BRAND |
			    TR_FREE_ACT | TR_SEE_INVIS | TR_RES_COLD);
	    t_ptr->flags2 |= (TR_IM_COLD | TR_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
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
	    t_ptr->flags = (TR_FFALL | TR_RES_ELEC | TR_SEE_INVIS | TR_STR | TR_CON
			    | TR_FREE_ACT | TR_RES_COLD | TR_RES_ACID
			    | TR_RES_FIRE | TR_REGEN | TR_STEALTH);
	    t_ptr->flags2 |= (TR_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->p1 = 3;
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
	    t_ptr->flags = (TR_STR | TR_DEX);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_SLAY_TROLL | TR_SLAY_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 1;
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
	t_ptr->flags = (TR_SLAY_X_DRAGON | TR_SLAY_EVIL | TR_SLAY_UNDEAD |
		    TR_RES_FIRE | TR_RES_ACID | TR_RES_COLD | TR_RES_ELEC |
			TR_FREE_ACT | TR_SEE_INVIS | TR_WIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_DEMON | TR_LIGHTNING | TR_RES_NEXUS);
	t_ptr->p1 = 4;
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
	t_ptr->flags = (TR_WIS | TR_CON | TR_SEARCH | TR_SLOW_DIGEST | TR_SLAY_DRAGON);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->p1 = 3;
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
	t_ptr->flags = (TR_SLAY_ANIMAL | TR_STR | TR_FLAME_TONGUE |
			TR_RES_FIRE | TR_RES_ELEC);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC
			  | TR_LIGHTNING | TR_RES_DARK);
	t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SLAY_ANIMAL | TR_STR | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_SLAY_TROLL | TR_SLAY_ORC | TR_RES_NEXUS);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	t_ptr->flags = (TR_CON | TR_SLAY_ANIMAL | TR_SLAY_X_DRAGON |
		TR_FROST_BRAND | TR_SLAY_EVIL | TR_FREE_ACT | TR_SEE_INVIS |
		    TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC | TR_RES_ACID |
			TR_AGGRAVATE);
	t_ptr->flags2 |= (TR_SLAY_TROLL | TR_SLAY_ORC | TR_TELEPATHY | TR_ARTIFACT);
	t_ptr->p1 = -5;
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
	    t_ptr->flags = (TR_INT | TR_SLAY_ANIMAL | TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
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
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SEE_INVIS | TR_INT | TR_WIS);
	    t_ptr->flags2 |= (TR_LIGHT | TR_ACTIVATE | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->p1 = 4;
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
	    t_ptr->flags = (TR_SLAY_EVIL | TR_SEE_INVIS | TR_WIS | TR_INT | TR_CHR
			    | TR_FLAME_TONGUE | TR_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_HOLD_LIFE | TR_SLAY_ORC | TR_SLAY_TROLL
			      | TR_ACTIVATE | TR_RES_NETHER);
	    t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_COLD);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR_ACTIVATE | TR_RES_CHAOS | TR_RES_DARK |
			  TR_RES_NEXUS | TR_RES_NETHER | TR_ARTIFACT);
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
	t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_RES_ACID | TR_POISON |
			 TR_RES_ELEC | TR_FREE_ACT | TR_SEE_INVIS | TR_INT |
			 TR_WIS | TR_STEALTH | TR_AGGRAVATE);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_IM_LIGHT | TR_RES_LT |
			  TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 25;
	t_ptr->p1 = -2;
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
	t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_RES_ACID | TR_POISON |
			 TR_RES_ELEC | TR_DEX | TR_SEARCH | TR_REGEN);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR_RES_CONF | TR_RES_SOUND | TR_RES_LT
	      | TR_RES_DARK | TR_RES_CHAOS | TR_RES_DISENCHANT | TR_ARTIFACT
	       | TR_RES_SHARDS | TR_RES_BLIND | TR_RES_NEXUS | TR_RES_NETHER
			  | TR_ACTIVATE);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 35;
	t_ptr->p1 = -3;
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
	t_ptr->flags |= (TR_RES_ACID | TR_SPEED | TR_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_NEXUS | TR_ARTIFACT);
	t_ptr->name2 = ART_FEANOR;
	t_ptr->p1 = 1;
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
	t_ptr->flags |= (TR_FREE_ACT | TR_DEX | TR_SUST_STAT | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT | TR_RES_NETHER | TR_RES_CHAOS);
	t_ptr->name2 = ART_DAL;
	t_ptr->p1 = 5;
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
	t_ptr->flags |= (TR_CON | TR_FREE_ACT | TR_STR |
			 TR_RES_ACID | TR_SEARCH);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_RES_CHAOS | TR_ARTIFACT | TR_IM_ACID);
	t_ptr->name2 = ART_THORIN;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 0;
	t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_ARTIFACT | TR_RES_NEXUS);
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			 TR_STR | TR_DEX);
	t_ptr->flags2 |= (TR_RES_SOUND | TR_RES_CONF | TR_ARTIFACT);
	t_ptr->name2 = EGO_ROHAN;
	t_ptr->tohit = 0;
	t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			 TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_ANARION;
	t_ptr->p1 = 10;
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
	t_ptr->flags |= (TR_RES_ACID | TR_DEX | TR_FREE_ACT);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_FINGOLFIN;
	t_ptr->ident |= ID_SHOW_HITDAM;
	t_ptr->p1 = 4;
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
	    t_ptr->flags |= (TR_STR | TR_CON | TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMBELEG;
	    t_ptr->ident |= ID_SHOW_HITDAM;
	    t_ptr->p1 = 2;
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
	    t_ptr->flags |= (TR_SUST_STAT | TR_FREE_ACT);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR_RES_LT | TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMMITHRIM;
	    t_ptr->ident |= ID_NOSHOW_P1;
	    t_ptr->p1 = 5;
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
	    t_ptr->flags |= TR_RES_FIRE;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags |= TR_RES_COLD;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags |= TR_RES_ELEC;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags |= TR_RES_ACID;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags |= (TR_STR | TR_DEX | TR_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMLOST;
	    t_ptr->toac = 0;
	    t_ptr->p1 = -5;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD |
			 TR_RES_ELEC | TR_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_BELEGENNON;
	t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			 TR_STR | TR_CHR);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_DISENCHANT | TR_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = ART_CELEBORN;
	t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_RES_ACID | TR_POISON | TR_CON | TR_WIS | TR_INT);
	t_ptr->flags2 |= (TR_RES_CONF | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_CASPANION;
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			 TR_STEALTH);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_DARK);
	t_ptr->name2 = ART_HITHLOMIR;
	t_ptr->p1 = 4;
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
	t_ptr->flags |= (TR_RES_ACID | TR_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_SHARDS);
	t_ptr->name2 = ART_THALKETTOTH;
	t_ptr->toac = 25;
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			 TR_STR | TR_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_NEXUS | TR_RES_SHARDS);
	t_ptr->name2 = ART_ARVEDUI;
	t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_RES_ACID | TR_INT | TR_WIS);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_RES_BLIND | TR_ARTIFACT);
	t_ptr->name2 = ART_THRANDUIL;
	t_ptr->p1 = 2;
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
	t_ptr->flags |= (TR_RES_ACID | TR_WIS | TR_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THENGEL;
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_STR | TR_CON | TR_DEX | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_NEXUS);
	t_ptr->name2 = ART_HAMMERHAND;
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC);
	t_ptr->flags2 |= (TR_RES_LT | TR_RES_DARK | TR_ARTIFACT);
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
	t_ptr->flags |= (TR_CON | TR_STR | TR_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THROR;
	t_ptr->p1 = 3;
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
	    t_ptr->flags |= (TR_RES_ACID | TR_RES_FIRE | TR_RES_COLD | TR_RES_ELEC |
			     TR_CON | TR_DEX | TR_STR | TR_SEE_INVIS);
	    t_ptr->flags2 |= (TR_TELEPATHY | TR_LIGHT | TR_RES_LT | TR_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->name2 = ART_DOR_LOMIN;
	    t_ptr->p1 = 4;
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
	    t_ptr->flags |= (TR_INT | TR_WIS | TR_SEE_INVIS | TR_SEARCH | TR_RES_ACID);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_RES_BLIND | TR_ARTIFACT);
	    t_ptr->name2 = ART_HOLHENNETH;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->p1 = 2;
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
	    t_ptr->flags |= (TR_INT | TR_WIS | TR_SEE_INVIS | TR_SEARCH | TR_CURSED
			     | TR_AGGRAVATE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_GORLIM;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->p1 = -125;
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
	t_ptr->flags = (TR_STR | TR_CON | TR_WIS | TR_SEE_INVIS | TR_REGEN
			| TR_RES_ACID | TR_RES_FIRE);
	t_ptr->flags2 = (TR_ACTIVATE | TR_LIGHT | TR_RES_LT | TR_RES_BLIND |
			 TR_ARTIFACT);
	t_ptr->p1 = 3;
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
	t_ptr->flags |= (TR_STR | TR_DEX | TR_CON |
		      TR_RES_ACID | TR_SEE_INVIS | TR_FREE_ACT | TR_CURSED);
	t_ptr->flags2 |= (TR_TELEPATHY | TR_ARTIFACT);
	t_ptr->name2 = ART_BERUTHIEL;
	t_ptr->p1 = -125;
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
	t->flags2 |= TR_RES_CONF;
	break;
      case 2:
	t->flags2 |= TR_RES_SOUND;
	break;
      case 3:
	t->flags2 |= TR_RES_LT;
	break;
      case 4:
	t->flags2 |= TR_RES_DARK;
	break;
      case 5:
	t->flags2 |= TR_RES_CHAOS;
	break;
      case 6:
	t->flags2 |= TR_RES_DISENCHANT;
	break;
      case 7:
	t->flags2 |= TR_RES_SHARDS;
	break;
      case 8:
	t->flags2 |= TR_RES_NEXUS;
	break;
      case 9:
	t->flags2 |= TR_RES_BLIND;
	break;
      case 10:
	t->flags2 |= TR_RES_NETHER;
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
		t_ptr->flags |= (TR_RES_ELEC | TR_RES_COLD | TR_RES_ACID |
				 TR_RES_FIRE | TR_SUST_STAT);
		if (wizard || peek)
		    msg_print("Robe of the Magi");
		rating += 30;
		t_ptr->flags2 |= TR_HOLD_LIFE;
		t_ptr->ident |= ID_NOSHOW_P1;
		give_1_hi_resist(t_ptr);	/* JLS */
		t_ptr->p1 = 10;
		t_ptr->toac += 10 + randint(5);
		t_ptr->name2 = EGO_MAGI;
		t_ptr->cost = 10000L + (t_ptr->toac * 100);
	    } else if (magik(special) || good == 666)
		switch (randint(9)) {
		  case 1:
		    if ((randint(3) == 1 || good == 666) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    t_ptr->flags |= (TR_RES_ELEC | TR_RES_COLD | TR_RES_ACID |
				     TR_RES_FIRE);
		    if (randint(3) == 1) {
			if (peek)
			    msg_print("Elvenkind");
			rating += 25;
			give_1_hi_resist(t_ptr);	/* JLS */
			t_ptr->flags |= TR_STEALTH;
			t_ptr->p1 = randint(3);
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
		    t_ptr->flags |= TR_RES_ACID;
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
		    t_ptr->flags |= TR_RES_FIRE;
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
		    t_ptr->flags |= TR_RES_COLD;
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
		    t_ptr->flags |= TR_RES_ELEC;
		    t_ptr->name2 = EGO_RESIST_E;
		    t_ptr->cost += 500L;
		    break;
		}
	} else if (magik(cursed)) {
	    t_ptr->toac = -randint(3) - m_bonus(0, 10, level);
	    t_ptr->cost = 0L;
	    t_ptr->flags |= TR_CURSED;
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
		    t_ptr->flags |= (TR_FLAME_TONGUE | TR_RES_FIRE);
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
			t_ptr->flags |= (TR_SEE_INVIS | TR_SUST_STAT |
				      TR_SLAY_UNDEAD | TR_SLAY_EVIL | TR_WIS);
			t_ptr->flags2 |= (TR_SLAY_DEMON | TR_BLESS_BLADE);
			t_ptr->tohit += 5;
			t_ptr->todam += 5;
			t_ptr->toac += randint(4);
		    /* the value in p1 is used for strength increase */
		    /* p1 is also used for sustain stat */
			t_ptr->p1 = randint(4);
			t_ptr->name2 = EGO_HA;
			t_ptr->cost += t_ptr->p1 * 500;
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
			t_ptr->flags |= (TR_FFALL | TR_RES_ELEC | TR_SEE_INVIS
				   | TR_FREE_ACT | TR_RES_COLD | TR_RES_ACID
				     | TR_RES_FIRE | TR_REGEN | TR_STEALTH);
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->toac += 5 + randint(5);
			t_ptr->name2 = EGO_DF;
		    /* the value in p1 is used for stealth */
			t_ptr->p1 = randint(3);
			t_ptr->cost += t_ptr->p1 * 500;
			t_ptr->cost += 7500L;
			t_ptr->cost *= 2;
			break;
		      case 3:
		      case 4:	   /* Flame Tongue  */
			if (((randint(2) == 1) || (good == 666)) && !not_unique &&
			    unique_weapon(t_ptr))
			    break;
			rating += 20;
			t_ptr->flags |= (TR_FLAME_TONGUE | TR_RES_FIRE);
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
			t_ptr->flags |= (TR_FROST_BRAND | TR_RES_COLD);
			t_ptr->tohit += 2;
			t_ptr->todam += 2;
			t_ptr->name2 = EGO_FB;
			t_ptr->cost += 2200L;
			break;
		      case 7:
		      case 8:	   /* Slay Animal  */
			t_ptr->flags |= TR_SLAY_ANIMAL;
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
			t_ptr->flags |= TR_SLAY_DRAGON;
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
			t_ptr->flags |= TR_SLAY_EVIL;
			if (randint(3) == 1) {
			    t_ptr->flags |= (TR_WIS);
			    t_ptr->flags2 |= (TR_BLESS_BLADE);
			    t_ptr->p1 = m_bonus(0, 3, level);
			    t_ptr->cost += (200 * t_ptr->p1);
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
			t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_UNDEAD);
			if (randint(3) == 1) {
			    t_ptr->flags2 |= (TR_HOLD_LIFE);
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
			t_ptr->flags2 |= TR_SLAY_ORC;
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
			t_ptr->flags2 |= TR_SLAY_TROLL;
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
			t_ptr->flags2 |= TR_SLAY_GIANT;
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
			t_ptr->flags2 |= TR_SLAY_DEMON;
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
			t_ptr->flags |= (TR_SEE_INVIS | TR_DEX | TR_CON | TR_STR |
					 TR_FREE_ACT);
			t_ptr->flags2 |= TR_SLAY_ORC;
			t_ptr->tohit += randint(5) + 3;
			t_ptr->todam += randint(5) + 3;
			t_ptr->p1 = 1;
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
			t_ptr->flags = TR_WIS;
			t_ptr->flags2 = TR_BLESS_BLADE;
			t_ptr->tohit += 3;
			t_ptr->todam += 3;
			t_ptr->p1 = randint(3);
			t_ptr->name2 = EGO_BLESS_BLADE;
			t_ptr->cost += t_ptr->p1 * 1000;
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
			t_ptr->flags2 = TR_ATTACK_SPD;
			if (t_ptr->weight <= 80)
			    t_ptr->p1 = randint(3);
			else if (t_ptr->weight <= 130)
			    t_ptr->p1 = randint(2);
			else
			    t_ptr->p1 = 1;
			t_ptr->name2 = EGO_ATTACKS;
			t_ptr->cost += (t_ptr->p1 * 2000);
			t_ptr->cost *= 2;
			break;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    t_ptr->tohit = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->todam = (-randint(3) - m_bonus(1, 20, level));
	    t_ptr->flags |= TR_CURSED;
	    if (level > (20 + randint(15)) && randint(10) == 1) {
		t_ptr->name2 = EGO_MORGUL;
		t_ptr->flags |= (TR_SEE_INVIS | TR_AGGRAVATE);
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
			t_ptr->p1 = 3;
			t_ptr->flags |= (TR_STEALTH | TR_DEX);
			t_ptr->flags2 |= (TR_ARTIFACT | TR_RES_DISENCHANT);
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
			t_ptr->p1 = 3;
			t_ptr->flags |= (TR_FREE_ACT | TR_DEX);
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
		    t_ptr->p1 = 1;
		    t_ptr->flags |= (TR_SPEED | TR_RES_FIRE);
		    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0L;
	}
	break;

      case TV_DIGGING:
    /* always show tohit/todam values if identified */
	t_ptr->ident |= ID_SHOW_HITDAM;
	if (magik(chance) || (good == 666)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->p1 += m_bonus(0, 5, level);
	    }
	    if (tmp == 2)	/* do not give additional plusses -CWS */
		;
	    else {
	    /* a cursed digging tool */
		t_ptr->p1 = (-m_bonus(1, 15, level));
		t_ptr->cost = 0L;
		t_ptr->flags |= TR_CURSED;
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
		    t_ptr->flags |= TR_FREE_ACT;
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
		    t_ptr->p1 = 2 + randint(2);
		    t_ptr->flags |= TR_DEX;
		    t_ptr->cost += (t_ptr->p1) * 400;
		    break;
		  case 10:
		    if (((randint(3) == 1) || (good == 666)) && !not_unique &&
			unique_armour(t_ptr))
			break;
		    if (peek)
			msg_print("Power");
		    rating += 22;
		    t_ptr->name2 = ART_POWER;
		    t_ptr->p1 = 1 + randint(4);
		    t_ptr->tohit += 1 + randint(4);
		    t_ptr->todam += 1 + randint(4);
		    t_ptr->flags |= TR_STR;
		    t_ptr->ident |= ID_SHOW_HITDAM;
		    t_ptr->ident |= ID_NOSHOW_TYPE;
		    t_ptr->cost += (t_ptr->tohit + t_ptr->todam + t_ptr->p1) * 300;
		    break;
		}
	    }
	} else if (magik(cursed)) {
	    if (magik(special)) {
		if (randint(2) == 1) {
		    t_ptr->flags |= TR_DEX;
		    t_ptr->name2 = EGO_CLUMSINESS;
		} else {
		    t_ptr->flags |= TR_STR;
		    t_ptr->name2 = EGO_WEAKNESS;
		}
		t_ptr->p1 = (randint(3) - m_bonus(0, 10, level));
	    }
	    t_ptr->toac = (-m_bonus(1, 20, level));
	    t_ptr->flags |= TR_CURSED;
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
			t_ptr->flags |= TR_SPEED;
			if (wizard || peek)
			    msg_print("Boots of Speed");
			t_ptr->name2 = EGO_SPEED;
			rating += 30;
			t_ptr->p1 = 1;
			t_ptr->cost += 300000L;
		    }
		} else if (stricmp("& Pair of Metal Shod Boots",
				   k_list[t_ptr->index].name))	/* not metal */
		    if (tmp > 6) {
			t_ptr->flags |= TR_FFALL;
			rating += 7;
			t_ptr->name2 = EGO_SLOW_DESCENT;
			t_ptr->cost += 250;
		    } else if (tmp < 5) {
			t_ptr->flags |= TR_STEALTH;
			rating += 16;
			t_ptr->p1 = randint(3);
			t_ptr->name2 = EGO_STEALTH;
			t_ptr->cost += 500;
		    } else {	   /* 5,6 */
			t_ptr->flags |= TR_FREE_ACT;
			rating += 15;
			t_ptr->name2 = EGO_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    }
		else
		 /* is metal boots, different odds since no stealth */
		    if (tmp < 5) {
			t_ptr->flags |= TR_FREE_ACT;
			rating += 15;
			t_ptr->name2 = EGO_FREE_ACTION;
			t_ptr->cost += 500;
			t_ptr->cost *= 2;
		    } else {	   /* tmp > 4 */
			t_ptr->flags |= TR_FFALL;
			rating += 7;
			t_ptr->name2 = EGO_SLOW_DESCENT;
			t_ptr->cost += 250;
		    }
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags |= TR_SPEED;
		t_ptr->name2 = EGO_SLOWNESS;
		t_ptr->p1 = -1;
	    } else if (tmp == 2) {
		t_ptr->flags |= TR_AGGRAVATE;
		t_ptr->name2 = EGO_NOISE;
	    } else {
		t_ptr->name2 = EGO_GREAT_MASS;
		t_ptr->weight = t_ptr->weight * 5;
	    }
	    t_ptr->cost = 0;
	    t_ptr->toac = (-m_bonus(2, 20, level));
	    t_ptr->flags |= TR_CURSED;
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
			    t_ptr->p1 = randint(2);
			    rating += 13;
			    t_ptr->flags |= TR_INT;
			    t_ptr->name2 = EGO_INTELLIGENCE;
			    t_ptr->cost += t_ptr->p1 * 500;
			}
		    } else if (tmp < 6) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Wisdom");
			    rating += 13;
			    t_ptr->p1 = randint(2);
			    t_ptr->flags |= TR_WIS;
			    t_ptr->name2 = EGO_WISDOM;
			    t_ptr->cost += t_ptr->p1 * 500;
			}
		    } else if (tmp < 10) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    t_ptr->p1 = 1 + randint(4);
			    rating += 11;
			    t_ptr->flags |= TR_INFRA;
			    t_ptr->name2 = EGO_INFRAVISION;
			    t_ptr->cost += t_ptr->p1 * 250;
			}
		    } else if (tmp < 12) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Light");
			    t_ptr->flags2 |= (TR_RES_LT | TR_LIGHT);
			    rating += 6;
			    t_ptr->name2 = EGO_LIGHT;
			    t_ptr->cost += 500;
			}
		    } else if (tmp < 14) {
			if (!((randint(2) == 1) && !not_unique &&
			      unique_armour(t_ptr))) {
			    if (peek)
				msg_print("Helm of Seeing");
			    t_ptr->flags |= TR_SEE_INVIS;
			    t_ptr->flags2 |= TR_RES_BLIND;
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
			    t_ptr->flags2 |= TR_TELEPATHY;
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
			    t_ptr->p1 = randint(3);
			    t_ptr->flags |= (TR_FREE_ACT | TR_CON |
					     TR_DEX | TR_STR);
			    t_ptr->name2 = EGO_MIGHT;
			    t_ptr->cost += 1000 + t_ptr->p1 * 500;
			}
			break;
		      case 2:
			if (peek)
			    msg_print("Lordliness");
			t_ptr->p1 = randint(3);
			rating += 17;
			t_ptr->flags |= (TR_CHR | TR_WIS);
			t_ptr->name2 = EGO_LORDLINESS;
			t_ptr->cost += 1000 + t_ptr->p1 * 500;
			break;
		      case 3:
			if (peek)
			    msg_print("Crown of the Magi");
			rating += 15;
			t_ptr->p1 = randint(3);
			t_ptr->flags |= (TR_RES_ELEC | TR_RES_COLD
				      | TR_RES_ACID | TR_RES_FIRE | TR_INT);
			t_ptr->name2 = EGO_MAGI;
			t_ptr->cost += 3000 + t_ptr->p1 * 500;
			break;
		      case 4:
			rating += 8;
			if (peek)
			    msg_print("Beauty");
			t_ptr->p1 = randint(4);
			t_ptr->flags |= TR_CHR;
			t_ptr->name2 = EGO_BEAUTY;
			t_ptr->cost += 750;
			break;
		      case 5:
			if (peek)
			    msg_print("Seeing");
			rating += 8;
			t_ptr->p1 = 5 * (1 + randint(4));
			t_ptr->flags |= (TR_SEE_INVIS | TR_SEARCH);
			t_ptr->name2 = EGO_SEEING;
			t_ptr->cost += 1000 + t_ptr->p1 * 100;
			break;
		      case 6:
			t_ptr->flags |= TR_REGEN;
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
	    t_ptr->flags |= TR_CURSED;
	    t_ptr->cost = 0;
	    if (magik(special))
		switch (randint(7)) {
		  case 1:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_INT;
		    t_ptr->name2 = EGO_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_WIS;
		    t_ptr->name2 = EGO_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_STR;
		    t_ptr->name2 = EGO_WEAKNESS;
		    break;
		  case 6:
		    t_ptr->flags |= TR_TELEPORT;
		    t_ptr->name2 = EGO_TELEPORTATION;
		    break;
		  case 7:
		    t_ptr->p1 = -randint(5);
		    t_ptr->flags |= TR_CHR;
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
		    t_ptr->p1 = -m_bonus(1, 10, level);
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    t_ptr->p1 = m_bonus(1, 6, level);
		    t_ptr->cost += t_ptr->p1 * 100;
		}
		break;
	      case 4:		   /* 136 */
		if (magik(cursed)) {
		    t_ptr->p1 = -randint(3);
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		} else {
		    if (peek)
			msg_print("Ring of Speed");
		    rating += 35;
		    if (randint(888) == 1)
			t_ptr->p1 = 2;
		    else
			t_ptr->p1 = 1;
		}
		break;
	      case 5:
		t_ptr->p1 = 5 * m_bonus(1, 10, level);
		t_ptr->cost += t_ptr->p1 * 30;
		if (magik(cursed)) {
		    t_ptr->p1 = -t_ptr->p1;
		    t_ptr->flags |= TR_CURSED;
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
		t_ptr->p1 = (-randint(4));
		break;
	      case 19:		   /* Increase damage	      */
		t_ptr->todam = m_bonus(1, 10, level);
		t_ptr->todam += 3 + randint(10);
		t_ptr->cost += t_ptr->todam * 100;
		if (magik(cursed)) {
		    t_ptr->todam = -t_ptr->todam;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 20:		   /* Increase To-Hit	      */
		t_ptr->tohit = m_bonus(1, 10, level);
		t_ptr->tohit += 3 + randint(10);
		t_ptr->cost += t_ptr->tohit * 100;
		if (magik(cursed)) {
		    t_ptr->tohit = -t_ptr->tohit;
		    t_ptr->flags |= TR_CURSED;
		    t_ptr->cost = -t_ptr->cost;
		}
		break;
	      case 21:		   /* Protection	      */
		t_ptr->toac = m_bonus(0, 10, level);
		t_ptr->toac += 4 + randint(5);
		t_ptr->cost += t_ptr->toac * 100;
		if (magik(cursed)) {
		    t_ptr->toac = -t_ptr->toac;
		    t_ptr->flags |= TR_CURSED;
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
		    t_ptr->flags |= TR_CURSED;
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
		t_ptr->p1 = -m_bonus(1, 5, level);
		t_ptr->flags |= TR_CURSED;
		t_ptr->cost = -t_ptr->cost;
	    } else {
		t_ptr->p1 = m_bonus(1, 5, level);
		t_ptr->cost += t_ptr->p1 * 100;
	    }
	} else if (t_ptr->sval == 2) { /* searching */
	    t_ptr->p1 = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		t_ptr->p1 = -t_ptr->p1;
		t_ptr->cost = -t_ptr->cost;
		t_ptr->flags |= TR_CURSED;
	    } else
		t_ptr->cost += 20 * t_ptr->p1;
	} else if (t_ptr->sval == 8) {
	    rating += 25;
	    t_ptr->p1 = 5 * (randint(2) + m_bonus(0, 10, level));
	    t_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    t_ptr->cost += 20 * t_ptr->p1 + 50 * t_ptr->toac;
	    if (t_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		t_ptr->flags |= TR_CURSED;
	} else if (t_ptr->sval == 9) {
	/* amulet of DOOM */
	    t_ptr->p1 = (-randint(5) - m_bonus(2, 10, level));
	    t_ptr->toac = (-randint(3) - m_bonus(0, 6, level));
	    t_ptr->flags |= TR_CURSED;
	}
	break;

    /* Subval should be even for store, odd for dungeon */
    /* Dungeon found ones will be partially charged	 */
      case TV_LITE:
	if ((t_ptr->sval % 2) == 1) {
	    t_ptr->p1 = randint(t_ptr->p1);
	    t_ptr->sval -= 1;
	}
	break;

      case TV_WAND:
	switch (t_ptr->sval) {
	  case 0:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 1:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 4:
	    t_ptr->p1 = randint(4) + 3;
	    break;
	  case 5:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 6:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 7:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 8:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 9:
	    t_ptr->p1 = randint(12) + 6;
	    break;
	  case 10:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->p1 = randint(3) + 3;
	    break;
	  case 12:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 13:
	    t_ptr->p1 = randint(10) + 6;
	    break;
	  case 14:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 15:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 16:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->p1 = randint(5) + 4;
	    break;
	  case 18:
	    t_ptr->p1 = randint(8) + 4;
	    break;
	  case 19:
	    t_ptr->p1 = randint(6) + 2;
	    break;
	  case 20:
	    t_ptr->p1 = randint(4) + 2;
	    break;
	  case 21:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 22:
	    t_ptr->p1 = randint(5) + 2;
	    break;
	  case 23:
	    t_ptr->p1 = randint(12) + 12;
	    break;
	  case 24:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 26:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 27:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 28:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  default:
	    break;
	}
	break;

      case TV_STAFF:
	switch (t_ptr->sval) {
	  case 0:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 1:
	    t_ptr->p1 = randint(8) + 6;
	    break;
	  case 2:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 3:
	    t_ptr->p1 = randint(20) + 12;
	    break;
	  case 4:
	    t_ptr->p1 = randint(15) + 6;
	    break;
	  case 5:
	    t_ptr->p1 = randint(4) + 5;
	    break;
	  case 6:
	    t_ptr->p1 = randint(5) + 3;
	    break;
	  case 7:
	    t_ptr->p1 = randint(3) + 1;
	    t_ptr->level = 10;
	    break;
	  case 8:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 9:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 10:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 11:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 12:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 13:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 14:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 15:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 16:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 17:
	    t_ptr->p1 = randint(5) + 6;
	    break;
	  case 18:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 19:
	    t_ptr->p1 = randint(10) + 12;
	    break;
	  case 20:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 21:
	    t_ptr->p1 = randint(3) + 4;
	    break;
	  case 22:
	    t_ptr->p1 = randint(10) + 6;
	    t_ptr->level = 5;
	    break;
	  case 23:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 24:
	    t_ptr->p1 = randint(3) + 1;
	    break;
	  case 25:
	    t_ptr->p1 = randint(2) + 2;
	    break;
	  case 26:
	    t_ptr->p1 = randint(15) + 5;
	    break;
	  case 27:
	    t_ptr->p1 = randint(2) + 2;
	    break;
	  case 28:
	    t_ptr->p1 = randint(5) + 5;
	    break;
	  case 29:
	    t_ptr->p1 = randint(2) + 1;
	    break;
	  case 30:
	    t_ptr->p1 = randint(6) + 2;
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
			t_ptr->flags |= (TR_RES_FIRE | TR_RES_COLD | TR_POISON |
					 TR_RES_ELEC | TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
			t_ptr->p1 = 2;
			t_ptr->flags |= (TR_INT | TR_WIS | TR_STEALTH |
					 TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
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
			t_ptr->flags = (TR_DEX | TR_CHR | TR_RES_FIRE |
				   TR_RES_ACID | TR_RES_COLD | TR_FREE_ACT);
			t_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 3;
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
			t_ptr->flags = (TR_SEE_INVIS | TR_FREE_ACT |
					TR_RES_ACID);
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
			t_ptr->flags |= (TR_STEALTH | TR_RES_ACID);
			t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 3;
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
			t_ptr->flags = (TR_RES_FIRE | TR_RES_COLD |
				    TR_INT | TR_WIS | TR_CHR | TR_RES_ACID);
			t_ptr->flags2 = (TR_ACTIVATE | TR_ARTIFACT);
			t_ptr->p1 = 2;
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
			t_ptr->flags = (TR_STEALTH |
				  TR_FREE_ACT | TR_SEE_INVIS | TR_RES_ACID);
			t_ptr->flags2 |= (TR_IM_ACID | TR_ARTIFACT);
			t_ptr->p1 = 4;
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
			t_ptr->p1 = randint(3);
			t_ptr->flags |= TR_STEALTH;
			t_ptr->name2 = EGO_STEALTH;
			t_ptr->cost += 500 + (50 * t_ptr->p1);
			rating += 9;
		    } else {
			t_ptr->toac += 10 + randint(10);
			t_ptr->p1 = randint(3);
			t_ptr->flags |= (TR_STEALTH | TR_RES_ACID);
			t_ptr->name2 = EGO_AMAN;
			t_ptr->cost += 4000 + (100 * t_ptr->toac);
			rating += 16;
		    }
		}
	    }
	} else if (magik(cursed)) {
	    tmp = randint(3);
	    if (tmp == 1) {
		t_ptr->flags |= TR_AGGRAVATE;
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
	    t_ptr->flags |= TR_CURSED;
	}
	break;

      case TV_CHEST:
	switch (randint(level + 4)) {
	  case 1:
	    t_ptr->flags = 0;
	    t_ptr->name2 = EGO_EMPTY;
	    break;
	  case 2:
	    t_ptr->flags |= CH_LOCKED;
	    t_ptr->name2 = EGO_LOCKED;
	    break;
	  case 3:
	  case 4:
	    t_ptr->flags |= (CH_LOSE_STR | CH_LOCKED);
	    t_ptr->name2 = EGO_POISON_NEEDLE;
	    break;
	  case 5:
	  case 6:
	    t_ptr->flags |= (CH_POISON | CH_LOCKED);
	    t_ptr->name2 = EGO_POISON_NEEDLE;
	    break;
	  case 7:
	  case 8:
	  case 9:
	    t_ptr->flags |= (CH_PARALYSED | CH_LOCKED);
	    t_ptr->name2 = EGO_GAS_TRAP;
	    break;
	  case 10:
	  case 11:
	    t_ptr->flags |= (CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = EGO_EXPLOSION_DEVICE;
	    break;
	  case 12:
	  case 13:
	  case 14:
	    t_ptr->flags |= (CH_SUMMON | CH_LOCKED);
	    t_ptr->name2 = EGO_SUMMONING_RUNES;
	    break;
	  case 15:
	  case 16:
	  case 17:
	    t_ptr->flags |= (CH_PARALYSED | CH_POISON | CH_LOSE_STR |
			     CH_LOCKED);
	    t_ptr->name2 = EGO_MULTIPLE_TRAPS;
	    break;
	  default:
	    t_ptr->flags |= (CH_SUMMON | CH_EXPLODE | CH_LOCKED);
	    t_ptr->name2 = EGO_MULTIPLE_TRAPS;
	    break;
	}
	if (not_unique)		/* if bought from store - dbd */
	    t_ptr->p1 = randint(t_ptr->level);
	else			/* store the level chest's found on - dbd */
	    t_ptr->p1 = dun_level;
	break;

      case TV_SPIKE:
	t_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    t_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	t_ptr->p1 = missile_ctr;
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
    t_ptr->p1 = missile_ctr;

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
		t_ptr->flags |= (TR_FLAME_TONGUE|TR_RES_FIRE); /* RF so won't burn */
		t_ptr->tohit += 2;
		t_ptr->todam += 4;
		t_ptr->name2 = EGO_FIRE;
		t_ptr->cost += 25;
		rating += 6;
		break;
	      case 6: case 7:
		t_ptr->flags |= TR_SLAY_EVIL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = EGO_SLAY_EVIL;
		t_ptr->cost += 25;
		rating += 7;
		break;
	      case 8: case 9:
		t_ptr->flags |= TR_SLAY_ANIMAL;
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		t_ptr->name2 = EGO_SLAY_ANIMAL;
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 10:
		t_ptr->flags |= TR_SLAY_DRAGON;
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
	t_ptr->flags |= TR_CURSED;
	t_ptr->cost = 0;
	if (randint(5)==1) {
	    t_ptr->name2 = EGO_BACKBITING;
	    t_ptr->tohit -= 20;
	    t_ptr->todam -= 20;
	}
    }
}
