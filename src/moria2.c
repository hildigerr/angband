/* File: moria2.c */

/* Purpose: misc code, mainly to handle player commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static int summon_object(int, int, int, int, u32b);
static void py_attack(int, int);
static void inven_throw(int, struct inven_type *);
static void facts(struct inven_type *, int *, int *, int *, int *, int *);
static void drop_throw(int, int, struct inven_type *);
static void py_bash(int, int);
static int fearless(monster_race *);


/*
 * Moves creature record from one space to another	-RAK-	
 * Hack -- Any "monster" in the destination grid is forgotten.
 */
void move_rec(int y1, int x1, int y2, int x2)
{
    /* this always works correctly, even if y1==y2 and x1==x2 */
	int m_idx = cave[y1][x1].cptr;

	/* No monster is at the old location */
	cave[y1][x1].cptr = 0;

	/* Copy the monster index */
	cave[y2][x2].cptr = m_idx;
}



/*
 * Hack -- Check if a level is a "quest" level
 */
int is_quest(int level)
{
    if (level == Q_PLANE) return FALSE;
    if (quests[SAURON_QUEST] && (level == quests[SAURON_QUEST])) return TRUE;
    return FALSE;
}



/*
 * Player hit a real trap. -RAK-
 * No longer includes stores.
 */
void hit_trap(int y, int x)
{
    int                   i, ty, tx, num, dam;
    register cave_type   *c_ptr;
    register struct misc *p_ptr = &py.misc;
    register inven_type  *i_ptr;
    bigvtype              tmp;

    end_find();

    /* Get the cave grid */
    c_ptr = &cave[y][x];

    /* Get the trap */
    i_ptr = &i_list[c_ptr->tptr];

    /* Make the trap "visible" */
    i_ptr->tval = TV_VIS_TRAP;

    /* Paranoia -- redraw the grid */
    lite_spot(y, x);

    /* Roll for damage */
    dam = pdamroll(i_ptr->damage);

    /* Examine the trap sub-val */
    switch (i_ptr->sval) {

      case 1:			   /* Open pit */
	msg_print("You fell into a pit!");
	if (py.flags.ffall) {
	    msg_print("You gently float down.");
	}
	else {
	    objdes(tmp, i_ptr, TRUE);
	    take_hit(dam, tmp);
	}
	break;

      case 2:			   /* Arrow trap */
	if (test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
	    objdes(tmp, i_ptr, TRUE);
	    take_hit(dam, tmp);
	    msg_print("An arrow hits you.");
	}
	else {
	    msg_print("An arrow barely misses you.");
	}
	break;

      case 3:			   /* Covered pit */
	msg_print("You fell into a covered pit.");
	if (py.flags.ffall)
	    msg_print("You gently float down.");
	else {
	    objdes(tmp, i_ptr, TRUE);
	    take_hit(dam, tmp);
	}
	place_trap(y, x, 0);
	break;

      case 4:			   /* Trap door */
	if (!is_quest(dun_level)) {/* that would be too easy... -CFT */
	    msg_print("You fell through a trap door!");
	    new_level_flag = TRUE;
	    dun_level++;
	    if (py.flags.ffall) {
		msg_print("You gently float down.");
	}
	    else {
		objdes(tmp, i_ptr, TRUE);
		take_hit(dam, tmp);
	    }

	/* make sure can see the message before new level */
	    msg_print(NULL);
	}
	 /* end normal */ 
	else {			   /* it's a quest level, can't let them fall through */
	    msg_print("You fall into a spiked pit!");
	    if (py.flags.ffall)
		msg_print("You gently float down.");
	    else {
		dam = (dam * 3) / 2;	/* do a little extra damage for spikes */
		if (randint(3) == 1) {
		    msg_print("The spikes are poisoned!");
		    if (!(py.flags.immune_pois || py.flags.resist_pois ||
			  py.flags.oppose_pois))
			dam *= 2;  /* more damage from poison!  :-)  -CFT */
		    else
			msg_print("You are unaffected by the poison.");
		}
	    } /* no ffall */
	}
	break;

      case 5:			   /* Sleep gas */
	if (py.flags.paralysis == 0) {
	    msg_print("A strange white mist surrounds you!");
	    if (py.flags.free_act) {
		msg_print("You are unaffected.");
	    }
	    else {
		msg_print("You fall asleep.");
		py.flags.paralysis += randint(10) + 4;
	    }
	}
	break;

      case 6:			   /* Hid Obj */
	delete_object(y, x);
	place_object(y, x);
	msg_print("Hmmm, there was something under this rock.");
	break;

      case 7:			   /* STR Dart */
	if (test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
	    if (!py.flags.sustain_str) {
		(void)dec_stat(A_STR);
		objdes(tmp, i_ptr, TRUE);
		take_hit(dam, tmp);
		msg_print("A small dart weakens you!");
	    }
	    else {
		msg_print("A small dart hits you.");
	    }
	}
	else {
	    msg_print("A small dart barely misses you.");
	}
	break;

      case 8:			   /* Teleport */
	teleport_flag = TRUE;
	msg_print("You hit a teleport trap!");
    /* Light up the teleport trap, before we teleport away.  */
	move_light(y, x, y, x);
	break;

      case 9:			   /* Rockfall */
	take_hit(dam, "a falling rock");
	/* XXX XXX XXX Should move the player first! */
	/* XXX See the "move_player()" code */
	delete_object(y, x);
	place_rubble(y, x);
	msg_print("You are hit by falling rock.");
	break;

      case 10:			   /* Corrode gas */
	msg_print("A strange red gas surrounds you.");
	corrode_gas("corrosion gas");
	break;

      case 11:			   /* Summon mon */
	delete_object(y, x); /* Rune disappears.    */
	num = 2 + randint(3);
	for (i = 0; i < num; i++) {
	    ty = y;
	    tx = x;
	    (void)summon_monster(&ty, &tx, FALSE);
	}
	break;

      case 12:			   /* Fire trap */
	msg_print("You are enveloped in flames!");
	fire_dam(dam, "a fire trap");
	break;
      case 13:			   /* Acid trap */
	msg_print("You are splashed with acid!");
	acid_dam(dam, "an acid trap");
	break;

      case 14:			   /* Poison gas */
	if (!(py.flags.immune_pois || py.flags.resist_pois ||
	      py.flags.oppose_pois))
	    poison_gas(dam, "a poison gas trap");
	msg_print("A pungent green gas surrounds you!");
	break;

      case 15:			   /* Blind Gas */
	msg_print("A black gas surrounds you!");
	if (!py.flags.resist_blind) {
	    py.flags.blind += randint(50) + 50;
	}
	break;

      case 16:			   /* Confuse Gas */
	msg_print("A gas of scintillating colors surrounds you!");
	if ((!py.flags.resist_conf) && (!py.flags.resist_chaos)) {
	    py.flags.confused += randint(15) + 15;
	}
	break;

      case 17:			   /* Slow Dart */
	if (test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
	    objdes(tmp, i_ptr, TRUE);
	    take_hit(dam, tmp);
		msg_print("A small dart hits you!");
	    if (py.flags.free_act) {
		msg_print("You are unaffected.");
	    }
	    else {
		py.flags.slow += randint(20) + 10;
	    }
	}
	else {
	    msg_print("A small dart barely misses you.");
	}
	break;

      case 18:			   /* CON Dart */
	if (test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
	    if (!py.flags.sustain_con) {
	    (void)dec_stat(A_CON);
	    objdes(tmp, i_ptr, TRUE);
	    take_hit(dam, tmp);
		msg_print("A small dart saps your health!");
	    }
	    else {
		msg_print("A small dart hits you.");
	    }
	}
	else {
	    msg_print("A small dart barely misses you.");
	}
	break;

      case 19:			   /* Secret Door */
	break;
      case 99:			   /* Scare Mon */
	break;

      default:
	msg_print("Unknown trap value.");
	break;
    }
}


/*
 * Return spell number and failure chance
 *
 * returns -1 if no spells in book
 * returns 1 if choose a spell in book to cast
 * returns 0 if don't choose a spell, i.e. exit with an escape 
 */
int cast_spell(cptr prompt, int item_val, int *sn, int *sc)
{
    u32b               j1, j2;
    register int         i, k;
    int                  spell[63], result;
    int                  first_spell;
    register spell_type *s_ptr;

    /* if a warrior, abort as if by ESC -CFT */
    if (!py.misc.pclass) return 0;

    /* Assume nothing to read */
    result = (-1);

    i = 0;

    j1 = inventory[item_val].flags;
    j2 = inventory[item_val].flags2;

    first_spell = bit_pos(&j1);
/* set j1 again, since bit_pos modified it */
    j1 = inventory[item_val].flags & spell_learned;

    s_ptr = magic_spell[py.misc.pclass - 1];

    while (j1) {
	k = bit_pos(&j1);
	if (s_ptr[k].slevel <= py.misc.lev) {
	    spell[i] = k;
	    i++;
	}
    }

    if (!(inventory[item_val].flags & spell_learned))
	first_spell = bit_pos(&j2) + 32;
    j2 = inventory[item_val].flags2 & spell_learned2;

    while (j2) {
	k = bit_pos(&j2);
	if (s_ptr[k + 32].slevel <= py.misc.lev) {
	    spell[i] = k + 32;
	    i++;
	}
    }

    /* No usable spells */
    if (i <= 0) return (-1);

    /* Ask the user for a spell choice */
    result = get_spell(spell, i, sn, sc, prompt, first_spell);

    /* Verify if needed */
    if (result && magic_spell[py.misc.pclass - 1][*sn].smana > py.misc.cmana) {
	if (class[py.misc.pclass].spell == MAGE) {
	    result = get_check("You summon your limited strength to cast this one! Confirm?");
	}
	else {
	    result = get_check("The gods may think you presumptuous for this! Confirm?");
	}

    return (result);
}



void check_unique(monster_type *m_ptr)
{
    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
	u_list[m_ptr->mptr].exist = 0;
}


void delete_unique()
{
    int i;

    for (i = 0; i < MAX_R_IDX; i++)
	if (c_list[i].cdefense & UNIQUE)
	    u_list[i].exist = 0;
}

/* Deletes a monster entry from the level		-RAK-	 */
void delete_monster(int j)
{
    register monster_type *m_ptr;

    if (j < 2)
	return;			   /* trouble? abort! -CFT */
    m_ptr = &m_list[j];
    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
	check_unique(m_ptr);
    cave[m_ptr->fy][m_ptr->fx].cptr = 0;
    if (m_ptr->ml)
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    if (j != mfptr - 1) {
#ifdef TARGET
	/* This targetting code stolen from Morgul -CFT */
	/* Targetted monster dead or compacted.      CDW */
	if (j==target_mon)
	    target_mode = FALSE;

	/* Targetted monster moved to replace dead or compacted monster   CDW */
	if (target_mon==mfptr-1)
	    target_mon = j;
#endif
	m_ptr = &m_list[mfptr - 1];
	cave[m_ptr->fy][m_ptr->fx].cptr = j;
	m_list[j] = m_list[mfptr - 1];
    }
    mfptr--;
    m_list[mfptr] = blank_monster;
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
 * monster record and reduce mfptr, this is called in breathe, and a couple
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
    if (target_mon==mfptr-1)
	target_mon = j;
#endif
    m_ptr = &m_list[j];
    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
	check_unique(m_ptr);
/* force the hp negative to ensure that the monster is dead, for example, if
 * the monster was just eaten by another, it will still have positive hit
 * points 
 */
    m_ptr->hp = (-1);
    cave[m_ptr->fy][m_ptr->fx].cptr = 0;
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
    if (target_mon==mfptr-1)
	target_mon = j; 
#endif

    m_ptr = &m_list[j];		   /* Fixed from a c_list ptr to a m_list ptr. -CFT */
    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
	check_unique(m_ptr);
    if (j != mfptr - 1) {
	m_ptr = &m_list[mfptr - 1];
	cave[m_ptr->fy][m_ptr->fx].cptr = j;
	m_list[j] = m_list[mfptr - 1];
    }
    m_list[mfptr - 1] = blank_monster;
    mfptr--;
}


/* Creates objects nearby the coordinates given		-RAK-	  */
static int summon_object(int y, int x, int num, int typ, u32b good)
{
    register int        i, j, k;
    register cave_type *c_ptr;
    int                 real_typ, res;

    if (typ == 1)
	real_typ = 1;		   /* typ == 1 -> objects */
    else
	real_typ = 256;		   /* typ == 2 -> gold */
    res = 0;
    do {
	i = 0;
	do {
	    j = y - 3 + randint(5);
	    k = x - 3 + randint(5);
	    if (in_bounds(j, k) && los(y, x, j, k)) {
		c_ptr = &cave[j][k];
		if (floor_grid_bold(j, k) && (c_ptr->tptr == 0)) {
		    if (typ == 3) {/* typ == 3 -> 50% objects, 50% gold */
			if (randint(100) < 50)
			    real_typ = 1;
			else
			    real_typ = 256;
		    }
		    if (real_typ == 1) {
			if (good)
			    place_good(j, k, good);
			else
			    place_object(j, k);
		    } else {
			if (good)
			    place_good(j, k, good);
			else
			    place_gold(j, k);
		    }
		    lite_spot(j, k);
		    if (test_lite(j, k))
			res += real_typ;
		    i = 20;
		}
	    }
	    i++;
	}
	while (i <= 20);
	num--;
    }
    while (num != 0);
    return res;
}


/* Deletes object from given location			-RAK-	 */
int delete_object(int y, int x)
{
    register int        delete;
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];
    if (c_ptr->fval == BLOCKED_FLOOR)
	c_ptr->fval = CORR_FLOOR;
    pusht(c_ptr->tptr);		   /* then eliminate it */
    c_ptr->tptr = 0;
    c_ptr->fm = FALSE;
    lite_spot(y, x);
    if (test_lite(y, x))
	delete = TRUE;
    else
	delete = FALSE;
    return (delete);
}


/* Allocates objects upon a creatures death		-RAK-	 */
/* Oh well,  another creature bites the dust.  Reward the victor */
/* based on flags set in the main creature record		 */
/*
 * Returns a mask of bits from the given flags which indicates what the
 * monster is seen to have dropped.  This may be added to monster memory. 
 */
u32b monster_death(int y, int x, u32b flags, u32b good, u32b win)
{
    register int i, number;
    u32b       dump, res;

#if defined(ATARIST_MWC)
    u32b              holder;	   /* avoid a compiler bug */
#endif

    if (win) {		   /* MORGOTH */
	register int        j, k;
	register cave_type *c_ptr;
	int                 crown = FALSE, grond = FALSE;

	i = 0;
	do {
	    j = y - 3 + randint(5);
	    k = x - 3 + randint(5);
	    if (in_bounds(j, k) && los(y, x, j, k)) {
		c_ptr = &cave[j][k];
		if (floor_grid_bold(j, k) && (c_ptr->tptr == 0)) {
		    if (!crown) {
			int                 cur_pos;
			inven_type         *t_ptr;

			crown = TRUE;
			cur_pos = i_pop();
			cave[j][k].tptr = cur_pos;
			invcopy(&i_list[cur_pos], 98);
			t_ptr = &i_list[cur_pos];
			t_ptr->flags |= (TR1_STR | TR1_DEX | TR1_CON | TR1_INT | TR1_WIS | TR1_CHR |
				       TR3_SEE_INVIS | TR3_CURSED | TR1_INFRA);
			t_ptr->flags2 |= (TR3_TELEPATHY | TR3_LITE | TR_ARTIFACT);
			t_ptr->ident |= ID_NOSHOW_TYPE;
			t_ptr->name2 = ART_MORGOTH;
			t_ptr->p1 = 125;
			t_ptr->cost = 10000000L;
			if (cave[j][k].cptr == 1)
			    msg_print("You feel something roll beneath your feet.");
		    } else {
			int                 cur_pos;
			inven_type         *t_ptr;

			grond = TRUE;
			cur_pos = i_pop();
			cave[j][k].tptr = cur_pos;
			invcopy(&i_list[cur_pos], 56);
			t_ptr = &i_list[cur_pos];
			t_ptr->name2 = ART_GROND;
			t_ptr->tohit = 5;
			t_ptr->todam = 25;
			t_ptr->damage[0] = 10;
			t_ptr->damage[1] = 8;
			t_ptr->weight = 600;
			t_ptr->flags = (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD |
					TR2_RES_FIRE | TR2_RES_COLD | TR2_RES_ELEC |
					TR2_RES_ACID | TR1_SLAY_ANIMAL | TR1_SPEED |
					TR1_SLAY_X_DRAGON | TR3_AGGRAVATE);
			t_ptr->flags2 = (TR1_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_ORC |
				    TR1_IMPACT | TR3_TELEPATHY | TR_ARTIFACT);
			t_ptr->p1 = (-1);
			t_ptr->toac = 10;
			t_ptr->cost = 500000L;
			t_ptr->ident |= ID_SHOW_HITDAM;
			GROND = 1;
			if (cave[j][k].cptr == 1)
			    msg_print("You feel something roll beneath your feet.");
		    }
		    lite_spot(j, k);
		}
	    }
	    i++;
	}
	while (!grond && i < 50);
    }
#if !defined(ATARIST_MWC)
    if (flags & CM_CARRY_OBJ)
	i = 1;
    else
	i = 0;
    if (flags & CM_CARRY_GOLD)
	i += 2;

    number = 0;
    if ((flags & CM_60_RANDOM) && (randint(100) < 60))
	number++;
    if ((flags & CM_90_RANDOM) && (randint(100) < 90))
	number++;
    if (flags & CM_1D2_OBJ)
	number += randint(2);
    if (flags & CM_2D2_OBJ)
	number += damroll(2, 2);
    if (flags & CM_4D2_OBJ)
	number += damroll(4, 2);
    if (number > 0)
	dump = summon_object(y, x, number, i, good);
    else
	dump = 0;
#else
    holder = CM_CARRY_OBJ;
    if (flags & holder)
	i = 1;
    else
	i = 0;
    holder = CM_CARRY_GOLD;
    if (flags & holder)
	i += 2;

    number = 0;
    holder = CM_60_RANDOM;
    if ((flags & holder) && (randint(100) < 60))
	number++;
    holder = CM_90_RANDOM;
    if ((flags & holder) && (randint(100) < 90))
	number++;
    holder = CM_1D2_OBJ;
    if (flags & holder)
	number += randint(2);
    holder = CM_2D2_OBJ;
    if (flags & holder)
	number += damroll(2, 2);
    holder = CM_4D2_OBJ;
    if (flags & holder)
	number += damroll(4, 2);
    if (number > 0)
	dump = summon_object(y, x, number, i, good);
    else
	dump = 0;


#endif

#if defined(ATARIST_MWC)
    holder = CM_WIN;
    if (flags & holder)
#else
    if (flags & CM_WIN)
#endif
    {
	total_winner = TRUE;
	prt_winner();
	msg_print("*** CONGRATULATIONS *** You have won the game.");
	msg_print("You cannot save this game, but you may retire when ready.");
    }
    if (dump) {
	res = 0;
	if (dump & 255)
#ifdef ATARIST_MWC
	{
	    holder = CM_CARRY_OBJ;
	    res |= holder;
	}
#else
	    res |= CM_CARRY_OBJ;
#endif
	if (dump >= 256)
#ifdef ATARIST_MWC
	{
	    holder = CM_CARRY_GOLD;
	    res |= holder;
	}
#else
	    res |= CM_CARRY_GOLD;
#endif
	dump = (dump % 256) + (dump / 256);	/* number of items */
	res |= dump << CM_TR_SHIFT;
    } else
	res = 0;

    return res;
}




/*
 * Hack -- pass a fear code around
 * Used to redo monster fear messages -CWS
 */
static int monster_is_afraid = 0;


/*
 * return whether a monster is "fearless" and will never run away. -CWS
 */
static int fearless(monster_race *r_ptr)
{
    int flag = FALSE;

    /* NoMind --> NoFear */
    if (r_ptr->cdefense & MINDLESS) {
	flag = TRUE;
    }

    /* Undead --> (Spells = Mind --> Fear) + (NoSpells = NoMind --> NoFear) */
    if (r_ptr->cdefense & UNDEAD) {
	flag = (!(r_ptr->spells || r_ptr->spells2 || r_ptr->spells3));
    }

    /* The 'E' and 'g' monsters have NoFear *//* Demons have NoFear */
    if (r_ptr->cchar == 'E' || r_ptr->cchar == 'g' || r_ptr->cdefense & DEMON) {
	flag = TRUE;
    }

    /* But intelligence --> Fear */
    if (r_ptr->cdefense & INTELLIGENT) {
	flag = FALSE;
    }

    /* XXX Hack -- No "Normal Move" --> NoFear */
    if (!(r_ptr->cmove & CM_MOVE_NORMAL)) {
	flag = TRUE;
    }

    /* Result */    
    return (flag);
}


/*
 * Decreases monsters hit points and deletes monster if needed.
 * added fear (DGK) and check whether to print fear messages -CWS
 */
int mon_take_hit(int monptr, int dam, int print_fear)
{
    register u32b         i;
    int                     found = FALSE;
    s32b                   new_exp, new_exp_frac;

    register monster_type  *m_ptr;
    register struct misc   *p_ptr;
    register monster_race *c_ptr;

    int                     m_take_hit = (-1);
    u32b                  tmp;
    int                     percentage;
    char                    m_name[80];
    vtype                   out_val;

    /* Get the creature */
    m_ptr = &m_list[monptr];

    /* Hurt it, and wake it up */
    m_ptr->hp -= dam;
    m_ptr->csleep = 0;

    c_ptr = &c_list[m_ptr->mptr];

    /* It is dead now */
    if (m_ptr->hp < 0) {

	/* Delete ghost file */
	if (m_ptr->mptr == (MAX_R_IDX - 1)) {

	    char                temp[100];

	    if (!dun_level) {
		sprintf(temp, "%s/%d", ANGBAND_DIR_BONES, c_list[m_ptr->mptr].level);
	    }
	    else {
		sprintf(temp, "%s/%d", ANGBAND_DIR_BONES, dun_level);
	    }

	    unlink(temp);
	}

	if (c_list[m_ptr->mptr].cdefense & QUESTOR) {
	    for (i = 0; i < DEFINED_QUESTS; i++) {	/* search for monster's
							 * lv, not... */
		if (quests[i] == c_list[m_ptr->mptr].level) {	/* ...cur lv. -CFT */
		    quests[i] = 0;
		    found = TRUE;
		    break;
		}
	    }
	    if (found) {
		if ((unsigned) dun_level != c_list[m_ptr->mptr].level) {
		    /* just mesg */
		    msg_print("Well done!!  Now continue onward towards Morgoth.");
		} else {	   /* stairs and mesg */
		    cave_type          *ca_ptr;
		    int                 cur_pos;

		    ca_ptr = &cave[m_ptr->fy][m_ptr->fx];
		    if (ca_ptr->tptr != 0) {	/* don't overwrite artifact -CFT */
			int                 ty = m_ptr->fy, tx = m_ptr->fx, ny, nx;

			while ((cave[ty][tx].tptr != 0) &&
			  (i_list[cave[ty][tx].tptr].tval >= TV_MIN_WEAR) &&
			  (i_list[cave[ty][tx].tptr].tval <= TV_MAX_WEAR) &&
			 (i_list[cave[ty][tx].tptr].flags2 & TR_ARTIFACT)) {
			    do { /* pick new possible spot */
				ny = ty + (byte) randint(3) - 2;
				nx = tx + (byte) randint(3) - 2;
			    } while (!in_bounds(ny, nx) ||
				     !floor_grid_bold(ny, nx));
			    ty = ny;	/* this is a new spot, not in a wall/door/etc */
			    tx = nx;
			} /* ok, to exit this, [ty][tx] must not be artifact
			   * -CFT */
			if (cave[ty][tx].tptr != 0)	/* so we can delete it -CFT */
			    (void)delete_object(ty, tx);
			ca_ptr = &cave[ty][tx];	/* put stairway here... */
		    }
		    cur_pos = i_pop();
		    ca_ptr->tptr = cur_pos;
		    invcopy(&i_list[cur_pos], OBJ_DOWN_STAIR);
		    msg_print("Well done!! Go for it!");
		    msg_print("A magical stairway appears...");
		} /* if-else for stairway */
	    } /* if found */
	} /* if quest monster */

	object_level = (dun_level + c_ptr->level) >> 1;
	coin_type = 0;
	get_coin_type(c_ptr);
	i = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
			  c_list[m_ptr->mptr].cmove,
			  (c_list[m_ptr->mptr].cdefense & (SPECIAL | GOOD)),
			  (c_list[m_ptr->mptr].cmove & WINNER));
	coin_type = 0;
	if ((py.flags.blind < 1 && m_ptr->ml) ||
	    (c_list[m_ptr->mptr].cmove & CM_WIN) ||
	    (c_list[m_ptr->mptr].cdefense & UNIQUE)) {
	    /* recall even invisible uniques */

	    tmp = (c_recall[m_ptr->mptr].r_cmove & CM_TREASURE) >> CM_TR_SHIFT;
	    if (tmp > ((i & CM_TREASURE) >> CM_TR_SHIFT))
		i = (i & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
	    c_recall[m_ptr->mptr].r_cmove =
		(c_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE) | i;
	    if (c_recall[m_ptr->mptr].r_kills < MAX_SHORT)
		c_recall[m_ptr->mptr].r_kills++;
	}
	c_ptr = &c_list[m_ptr->mptr];
	p_ptr = &py.misc;

	if (c_ptr->cdefense & UNIQUE) {
	    u_list[m_ptr->mptr].exist = 0;
	    u_list[m_ptr->mptr].dead = 1;
	}

	/* Give some experience */
	new_exp = ((long)c_ptr->mexp * c_ptr->level) / p_ptr->lev;
	new_exp_frac = ((((long)c_ptr->mexp * c_ptr->level) % p_ptr->lev)
			* 0x10000L / p_ptr->lev) + p_ptr->exp_frac;

	if (new_exp_frac >= 0x10000L) {
	    new_exp++;
	    p_ptr->exp_frac = new_exp_frac - 0x10000L;
	}
	else {
	    p_ptr->exp_frac = new_exp_frac;
	}

	p_ptr->exp += new_exp;

	/* When drained, the player advances at 10% the normal rate */
	if (p_ptr->exp < p_ptr->max_exp) {
	    p_ptr->max_exp += new_exp/10;
	}

	/* can't call prt_experience() here, as that would result in "new level"
	 * message appearing before "monster dies" message 
	 */
	m_take_hit = m_ptr->mptr;
	/* in case this is called from within creatures(), this is a horrible
	 * hack, the m_list/creatures() code needs to be rewritten 
	 */
	if (hack_m_idx < monptr)
	    delete_monster(monptr);
	else
	    fix1_delete_monster(monptr);
	monster_is_afraid = 0;
    } else {
	if (m_ptr->maxhp <= 0)	   /* Then fix it! -DGK */
	    m_ptr->maxhp = 1;
	percentage = (m_ptr->hp * 100L) / (m_ptr->maxhp);

	if (fearless(c_ptr)) {
	/* No monster, so no fear */
	monster_is_afraid = 0;

	/* Monster is dead */
	return (-1);
    }

	/*
	 * Run if at 10% or less of max hit points, or got hit for half its
	 * current hit points -DGK 
	 */
	if (!(m_ptr->monfear) &&
	    ((percentage <= 10 && randint(10) <= percentage) ||
	     (dam >= m_ptr->hp))) {

	    /* Hack -- note fear */             
	    monster_is_afraid = 1;

	    /* Take note */
	    if (print_fear && m_ptr->ml && los(char_row, char_col, m_ptr->fy, m_ptr->fx)) {
		monster_name(m_name, m_ptr, c_ptr);
		sprintf(out_val, "%s flees in terror!", m_name);
		msg_print(out_val);
	    }

	    /* Timed fear */
	    m_ptr->monfear = (randint(10) +
			      ((dam >= m_ptr->hp && percentage > 7) ?
			       20 : (11 - percentage) * 5));
	}

	/* Already afraid */
	else if (m_ptr->monfear) {

	    /* When hurt, get brave? */
	    m_ptr->monfear -= randint(dam);

	    /* No longer afraid */
	    if (m_ptr->monfear <= 0) {
		if (monster_is_afraid == 1) monster_is_afraid = (-1);
		m_ptr->monfear = 0;
		if (m_ptr->ml && print_fear) {
		    char                sex = c_ptr->gender;

		    monster_name(m_name, m_ptr, c_ptr);
		    sprintf(out_val, "%s recovers %s courage.", m_name,
			    (sex == 'm' ? "his" : sex == 'f' ? "her" :
			     sex == 'p' ? "their" : "its"));
		    msg_print(out_val);
		}
	    }
	}
	m_take_hit = (-1);
    }
    return (m_take_hit);
}


/*
 * Player attacks a (poor, defenseless) creature	-RAK-	 
 */
void py_attack(int y, int x)
{
    register int        k, blows;
    int                 crptr, monptr, tot_tohit, base_tohit;
    vtype               m_name, out_val;
    register inven_type    *i_ptr;
    register struct misc   *p_ptr;

    crptr = cave[y][x].cptr;
    monptr = m_list[crptr].mptr;

    m_list[crptr].csleep = 0;
    i_ptr = &inventory[INVEN_WIELD];

    /* Does the player know what he's fighting?	   */
    if (!m_list[crptr].ml)
	(void)strcpy(m_name, "it");
    else {
	if (c_list[monptr].cdefense & UNIQUE)
	    (void)sprintf(m_name, "%s", c_list[monptr].name);
	else
	    (void)sprintf(m_name, "the %s", c_list[monptr].name);
    }

    /* Proper weapon */
    if (i_ptr->tval != TV_NOTHING) {

	/* Calculate blows */
	blows = attack_blows((int)i_ptr->weight, &tot_tohit);
    }

    /* Fists */
    else {

	/* Two blows */
	blows = 2;

	/* Hard to hit */
	tot_tohit = (-3);
    }

    if ((i_ptr->tval >= TV_SHOT) && (i_ptr->tval <= TV_ARROW))
    /* Fix for arrows */
	blows = 1;

    p_ptr = &py.misc;
    tot_tohit += p_ptr->ptohit;

    /* If creature is lit, use base rates, else, make it harder to hit */
    if (m_list[crptr].ml) {
	base_tohit = p_ptr->bth;
    }
    else {
	base_tohit = (p_ptr->bth / 2) - (tot_tohit * (BTH_PLUS_ADJ - 1)) -
	             (p_ptr->lev * class_level_adj[p_ptr->pclass][CLA_BTH] / 2);
    }

    /* Assume no fear messages need to be redone */
    monster_is_afraid = 0;

    /* Loop for number of blows, trying to hit the critter. */
    do {
	
	/* We hit it! */
	if (test_hit(base_tohit, (int)p_ptr->lev, tot_tohit,
		     (int)c_list[monptr].ac, CLA_BTH)) {

	    if (!wizard) {
		(void) sprintf(out_val, "You hit %s.", m_name);
		msg_print(out_val);
	    }

	    /* Normal weapon. */
	    if (i_ptr->tval != TV_NOTHING) {
		k = pdamroll(i_ptr->damage);
		k = tot_dam(i_ptr, k, monptr);
		k = critical_blow((int)i_ptr->weight, tot_tohit, k, CLA_BTH);
	    }

	    /* Bare hands */
	    else {
		k = damroll(1, 1);
		k = critical_blow(1, 0, k, CLA_BTH);
	    }

	    /* Apply the player damage bonuses */
	    k += p_ptr->ptodam;

	    if (wizard) {
		(void)sprintf(out_val,
			      "You hit %s with %d hp, doing %d+%d damage.",
			      m_name, m_list[crptr].hp, (k - p_ptr->ptodam),
			      p_ptr->ptodam);
		msg_print(out_val);
	    }

	    if (k < 0) k = 0;

	    /* Confusion attack */
	    if (py.flags.confuse_monster) {
		py.flags.confuse_monster = FALSE;
		msg_print("Your hands stop glowing.");
		if ((c_list[monptr].cdefense & CHARM_SLEEP) ||
		    (randint(MAX_R_LEV) < c_list[monptr].level)) {
		    (void)sprintf(out_val, "%s is unaffected.", m_name);
		}
		else {
		    (void)sprintf(out_val, "%s appears confused.", m_name);
		    m_list[crptr].confused = TRUE;
		}

		/* Uppercase and display the sentence */
		if ((out_val[0] >= 'a') && (out_val[0] <= 'z'))
		    out_val[0] -= 32;
		msg_print(out_val);

		if (m_list[crptr].ml && randint(4) == 1) {
		    c_recall[monptr].r_cdefense |=
			c_list[monptr].cdefense & CHARM_SLEEP;
		}
	    }

	    if (k < 0) k = 0;		   /* no neg damage! */

	    /* Is it dead yet? */
	    if (mon_take_hit(crptr, k, FALSE) >= 0) {

		if ((c_list[monptr].cdefense & (DEMON|UNDEAD|MINDLESS)) ||
		    (c_list[monptr].cchar == 'E') ||
		    (c_list[monptr].cchar == 'v') ||
		    (c_list[monptr].cchar == 'g')) {
		    (void)sprintf(out_val, "You have destroyed %s.", m_name);
		}
		else {
		    (void)sprintf(out_val, "You have slain %s.", m_name);
		}
		msg_print(out_val);
		prt_experience();

		/* No more attacks */
		blows = 0;
	    }

	    if ((i_ptr->tval >= TV_SHOT)
		&& (i_ptr->tval <= TV_ARROW)) {	/* Use missiles up */
		i_ptr->number--;
		inven_weight -= i_ptr->weight;
		py.flags.status |= PY_STR_WGT;
		if (i_ptr->number == 0) {
		    equip_ctr--;
		    py_bonuses(i_ptr, -1);
		    invcopy(i_ptr, OBJ_NOTHING);
		    calc_bonuses();
		}
	    }
	}

	else {
	    (void)sprintf(out_val, "You miss %s.", m_name);
	    msg_print(out_val);
	}

	blows--;
    }
    while (blows > 0);

    /* Hack -- delay the fear messages until here */

    if (!m_list[crptr].ml)
	(void)strcpy(m_name, "It");
    else {
	if (c_list[monptr].cdefense & UNIQUE)
	    (void)sprintf(m_name, "%s", c_list[monptr].name);
	else
	    (void)sprintf(m_name, "The %s", c_list[monptr].name);
    }

    if (monster_is_afraid == 1) {
	sprintf(out_val, "%s flees in terror!", m_name);
	msg_print(out_val);
    }
    if (monster_is_afraid == -1) {
	char                sex = c_list[monptr].gender;

	sprintf(out_val, "%s recovers %s courage.", m_name,
		(sex == 'm' ? "his" : sex == 'f' ? "her" :
		 sex == 'p' ? "their" : "its"));
	msg_print(out_val);
    }
}






static void inven_throw(int item_val, inven_type *t_ptr)
{
    register inven_type *i_ptr;

    i_ptr = &inventory[item_val];
    *t_ptr = *i_ptr;
    if (i_ptr->number > 1) {
	t_ptr->number = 1;
	i_ptr->number--;
	inven_weight -= i_ptr->weight;
	py.flags.status |= PY_STR_WGT;
    } else
	inven_destroy(item_val);
}


/*
 * Obtain the "facts" about a thrown object (or missile)
 */
static void facts(inven_type *i_ptr, \
		  int *tbth, int *tpth, int *tdam, int *tdis, int *thits)
{
    register int tmp_weight;

    if (i_ptr->weight < 1) tmp_weight = 1;
    else tmp_weight = i_ptr->weight;

    /* Throwing objects			 */
    *tdam = pdamroll(i_ptr->damage) + i_ptr->todam;
    *tbth = py.misc.bthb * 75 / 100;
    *tpth = py.misc.ptohit + i_ptr->tohit;

    /* Add this back later if the correct throwing device. -CJS- */
    if (inventory[INVEN_WIELD].tval != TV_NOTHING)
	*tpth -= inventory[INVEN_WIELD].tohit;

    *tdis = (((py.stats.use_stat[A_STR] + 20) * 10) / tmp_weight);
    if (*tdis > 10)
	*tdis = 10;

    /*  Default to single shot or throw */
    *thits = 1;

/* multiply damage bonuses instead of adding, when have proper missile/weapon
 * combo, this makes them much more useful 
 */

/* Using Bows,  slings,  or crossbows	 */
    if (inventory[INVEN_WIELD].tval == TV_BOW)

	/* Analyze the launcher */
	switch (inventory[INVEN_WIELD].sval) {

	  /* Sling and ammo */
	  case 20:
	    if (i_ptr->tval != TV_SHOT) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 2;
	    *tdis = 20;
	    break;

	  /* Sling of Might and ammo */
	  case 21:
	    if (i_ptr->tval != TV_SHOT) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 3;
	    *tdis = 20;
	    break;

	  /* Short Bow and Arrow */
	  case 1:
	    if (i_ptr->tval != TV_ARROW) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 2;
	    *tdis = 25;
	    break;

	  /* Long Bow and Arrow	 */
	  case 2:
	    if (i_ptr->tval != TV_ARROW) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 3;
	    *tdis = 30;
	    break;

	  /* C Bow, BARD, L bow of M and Arrow*/
	  case 3:
	    if (i_ptr->tval != TV_ARROW) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 4;
	    *tdis = 35;
	    break;

	  /* C Bow of M, BELEG and Arrow*/
	  case 4:
	    if (i_ptr->tval != TV_ARROW) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 5;
	    *tdis = 35;
	    break;

	  /* Light Crossbow and Bolt */
	  case 10:
	    if (i_ptr->tval != TV_BOLT) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 3;
	    *tdis = 25;
	    break;

	  /* Heavy Crossbow and Bolt */
	  case 11:
	    if (i_ptr->tval != TV_BOLT) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 4;
	    *tdis = 35;
	    break;

	  /* H xbow of M and Bolt*/
	  case 12:
	    if (i_ptr->tval != TV_BOLT) break;
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 5;
	    *tdis = 35;
	    break;
	    
	}
}


static void drop_throw(int y, int x, inven_type *t_ptr)
{
    register int i, j, k;
    int flag, cur_pos;
    bigvtype out_val, tmp_str;
    register cave_type *c_ptr;

    flag = FALSE;
    i = y;
    j = x;
    k = 0;
    if (randint(5) > 1) {
	do {
	    if (in_bounds(i, j)) {
		c_ptr = &cave[i][j];
		if (floor_grid_bold(i, j) && c_ptr->tptr == 0)
		    flag = TRUE;
	    }
	    if (!flag) {
		i = y + randint(3) - 2;
		j = x + randint(3) - 2;
		k++;
	    }
	}
	while ((!flag) && (k <= 9));
    }
    if (!flag && (t_ptr->tval >= TV_MIN_WEAR) &&
	(t_ptr->tval <= TV_MAX_WEAR) && (t_ptr->flags2 & TR_ARTIFACT)) {
	k = 0;  i = y;  j = x;
	do {		/* pick place w/o an object, unless doesn't seem to be one */
	    y = i;  x = j;
	    do {		/* pick place in bounds and not in wall */
		i = y + randint(3) -2;
		j = x + randint(3) -2;
	    } while (!in_bounds(i,j) || !floor_grid_bold(i, j));
	    k++;
	    if (!(cur_pos = cave[i][j].tptr) || (k>64))
		flag = TRUE;
	    if (flag && (((i_list[cur_pos].flags2 & TR_ARTIFACT) &&
			  ((cur_pos = i_list[cur_pos].tval) >= TV_MIN_WEAR) &&
			  (cur_pos <= TV_MAX_WEAR)) ||
			 (cur_pos == TV_STORE_DOOR) ||
			 (cur_pos == TV_UP_STAIR) ||
			 (cur_pos == TV_DOWN_STAIR)))
		flag = FALSE;
/* the above may seem convoluted, but it basically says: try up to 64 spaces,
 * if an open one, place the item.  If none, clobber the item at 64th, but keep looking
 * if that item is an artifact, store door, or stairs -CFT
 */
	    
	    if (k>888) flag = TRUE; /* if this many tries, TOO BAD! -CFT */
	} while (!flag);
    } /* if not flag and is artifact */
    if (flag)
    {
	if (cave[i][j].tptr)	/* we must have crushed something; waste it -CFT */
	    delete_object(i,j);
	cur_pos = i_pop();
	cave[i][j].tptr = cur_pos;
	i_list[cur_pos] = *t_ptr;
	lite_spot(i, j);
    }
    else
    {
	objdes(tmp_str, t_ptr, FALSE);
	(void) sprintf(out_val, "The %s disappears.", tmp_str);
	msg_print(out_val);
    }
}

/* This is another adaptation of DGK's Fangband code to help throw item
   stay around (like Artifacts!) -CFT */
static int stays_when_throw(inven_type *i_ptr)
{
  if ((i_ptr->tval >= TV_MIN_WEAR) && (i_ptr->tval <= TV_MAX_WEAR) &&
      (i_ptr->flags2 & TR_ARTIFACT))
    return TRUE;

  /* for non-artifacts, drop_throw() still loses 20% of them... */
  if ((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_STAFF))
    return TRUE;
  switch (i_ptr->tval){
    case TV_CHEST: case TV_SHOT: case TV_ROD: case TV_FOOD:
    case TV_MAGIC_BOOK: case TV_PRAYER_BOOK:
      return TRUE;
    case TV_MISC: case TV_SPIKE: case TV_WAND: case TV_BOLT:
    case TV_ARROW: case TV_LITE: case TV_SCROLL1: case TV_SCROLL2:
      return (randint(2)==1);
  }
  return FALSE;
}


/*
 * Make a bash attack on someone.  -CJS-
 *  Used to be part of bash above. 
 */
void py_bash(int y, int x)
{
    int                     monster, k, avg_max_hp, base_tohit, monptr;
    register monster_race *c_ptr;
    register monster_type  *m_ptr;
    vtype                   m_name, out_val;

    monster = cave[y][x].cptr;
    m_ptr = &m_list[monster];
    monptr = m_ptr->mptr;
    c_ptr = &c_list[monptr];
    m_ptr->csleep = 0;

/* Does the player know what he's fighting?	   */
    if (!m_ptr->ml)
	(void)strcpy(m_name, "it");
    else {
	if (c_list[monptr].cdefense & UNIQUE)
	    (void)sprintf(m_name, "%s", c_list[monptr].name);
	else
	    (void)sprintf(m_name, "the %s", c_list[monptr].name);
    }

    /* Attempt to bash */
    base_tohit = (py.stats.use_stat[A_STR] +
		  inventory[INVEN_ARM].weight / 2 +
		  py.misc.wt / 10);

    /* Harder to bash invisible monsters */
    if (!m_ptr->ml) {
	base_tohit = (base_tohit / 2) - 
	     (py.stats.use_stat[A_DEX] * (BTH_PLUS_ADJ - 1)) -
	     (py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTH] / 2);
    }

    /* Hack -- test for contact */
    if (test_hit(base_tohit, (int)py.misc.lev,
		 (int)py.stats.use_stat[A_DEX], (int)c_ptr->ac, CLA_BTH)) {

	(void)sprintf(out_val, "You hit %s.", m_name);
	msg_print(out_val);
	k = pdamroll(inventory[INVEN_ARM].damage);
	k = critical_blow((int)(inventory[INVEN_ARM].weight / 4 +
				py.stats.use_stat[A_STR]), 0, k, CLA_BTH);
	k += py.misc.wt / 60 + 3;

	/* No negative damage */
	if (k < 0) k = 0;

	/* See if we done it in.				     */
	if (mon_take_hit(monster, k, TRUE) >= 0) {

	    if ((c_list[monptr].cdefense & (DEMON|UNDEAD|MINDLESS)) ||
		(c_list[monptr].cchar == 'E') ||
		(c_list[monptr].cchar == 'v') ||
		(c_list[monptr].cchar == 'g')) {
		(void)sprintf(out_val, "You have destroyed %s.", m_name);
	    }
	    else {
		(void)sprintf(out_val, "You have slain %s.", m_name);
	    }
	    msg_print(out_val);
	    prt_experience();
	}

	else {
	    m_name[0] = toupper((int)m_name[0]);	/* Capitalize */

	    /* Powerful monsters cannot be stunned */
	    avg_max_hp = ((c_ptr->cdefense & MAX_HP) ?
			   (c_ptr->hd[0] * c_ptr->hd[1]) :
			   ((c_ptr->hd[0] * (c_ptr->hd[1] + 1)) >> 1));

	    /* Apply saving throw */
	    if ((100 + randint(400) + randint(400)) >
		(m_ptr->hp + avg_max_hp)) {
		m_ptr->stunned += randint(3) + 1;
		if (m_ptr->stunned > 24) m_ptr->stunned = 24;
		sprintf(out_val, "%s appears stunned!", m_name);
	    }
	    else {
		sprintf(out_val, "%s ignores your bash!", m_name);
	    }
	    msg_print(out_val);
	}
    }
    else {
	(void)sprintf(out_val, "You miss %s.", m_name);
	msg_print(out_val);
    }

    /* Stumble */
    if (randint(150) > py.stats.use_stat[A_DEX]) {
	msg_print("You are off balance.");
	py.flags.paralysis = 1 + randint(2);
    }
}


