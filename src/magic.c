/*
 * magic.c: code for mage spells 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

int door_creation();
int detect_magic();
int stair_creation();

/*
 * calculate number of spells player should have,
 * and learn/forget spells until that number is met -JEW- 
 */
void calc_spells(int stat)
{
    register int    i;
    register u32b mask;
    u32b          spell_flag;
    int             j, offset;
    int             num_allowed, new_spells, num_known, levels;
    vtype           tmp_str;
    const char	    *p;
    register struct misc *p_ptr = &py.misc;
    register spell_type  *msp_ptr;

    msp_ptr = &magic_spell[p_ptr->pclass - 1][0];
    if (stat == A_INT) {
	p = "spell";
	offset = SPELL_OFFSET;
    }
    else {
	p = "prayer";
	offset = PRAYER_OFFSET;
    }

    /* Check to see if know any spells greater than level, eliminate them */
    for (i = 31, mask = 0x80000000L; mask; mask >>= 1, i--) {
	if (mask & spell_learned) {
	    if (msp_ptr[i].slevel > p_ptr->lev) {
		spell_learned &= ~mask;
		spell_forgotten |= mask;
		(void)sprintf(tmp_str, "You have forgotten the %s of %s.", p,
			      spell_names[i + offset]);
		msg_print(tmp_str);
	    }
	}

	if (mask & spell_learned2) {
	    if (msp_ptr[i + 32].slevel > p_ptr->lev) {
		spell_learned2 &= ~mask;
		spell_forgotten2 |= mask;
		(void)sprintf(tmp_str, "You have forgotten the %s of %s.", p,
			      spell_names[i + offset + 32]);
		msg_print(tmp_str);
	    }
	}
    }


    /* calc number of spells allowed */
    levels = p_ptr->lev - class[p_ptr->pclass].first_spell_lev + 1;
    switch (stat_adj(stat)) {
      case 0:
	num_allowed = 0;
	break;
      case 1:
      case 2:
      case 3:
	num_allowed = 1 * levels;
	break;
      case 4:
      case 5:
	num_allowed = 3 * levels / 2;
	break;
      case 6:
	num_allowed = 2 * levels;
	break;
      default:
	num_allowed = 5 * levels / 2;
	break;
    }

    num_known = 0;
    for (mask = 0x1; mask; mask <<= 1) {
	if (mask & spell_learned) num_known++;
	if (mask & spell_learned2) num_known++;
    }

    new_spells = num_allowed - num_known;


    if (new_spells > 0) {

	/* remember forgotten spells while forgotten spells exist of new_spells
	 * positive, remember the spells in the order that they were learned  */
	for (i = 0; ((spell_forgotten | spell_forgotten2) && new_spells
		     && (i < num_allowed) && (i < 64)); i++) {
	    /* j is (i+1)th spell learned */
	    j = spell_order[i];

	    /* shifting by amounts greater than number of bits in long gives an
	     * undefined result, so don't shift for unknown spells */
	    if (j == 99) continue;	   /* don't process unknown spells... -CFT */

	    if (j < 32) {	   /* use spell_learned, spell_forgotten... -CFT */
		mask = 1L << j;	   /* bit in spell fields */
		if (mask & spell_forgotten) {
		    if (msp_ptr[j].slevel <= p_ptr->lev) {
			spell_forgotten &= ~mask;
			spell_learned |= mask;
			new_spells--;
			sprintf(tmp_str, "You have remembered the %s of %s.",
				p, spell_names[j + offset]);
			msg_print(tmp_str);
		    }
		    else {
			/* if was too high lv to remember */
			num_allowed++;
		    }
		} /* if mask&spell_forgotten */
	    }

	    /* j < 32 */
	    else {		   /* j > 31, use spell_learned2, spell_forgotten2... -CFT */
		mask = 1L << (j - 32);	/* bit in spell fields */
		if (mask & spell_forgotten2) {
		    if (msp_ptr[j].slevel <= p_ptr->lev) {
			spell_forgotten2 &= ~mask;
			spell_learned2 |= mask;
			new_spells--;
			sprintf(tmp_str, "You have remembered the %s of %s.",
				p, spell_names[j + offset]);
			msg_print(tmp_str);
		    }
		    else {
			/* if was too high lv to remember */
			num_allowed++;
		    }
		} /* if mask&spell_forgotten2 */
	    } /* j > 31 */
	} /* for loop... */


    if (new_spells > 0) {
	/* determine which spells player can learn */
	/*
	 * must check all spells here, in gain_spell() we actually check if
	 * the books are present 
	 */
	/* only bother with spells learnable by class -CFT */
	spell_flag = spellmasks[py.misc.pclass][0] & ~spell_learned;
	mask = 0x1;
	i = 0;
	for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++) {
	    if (spell_flag & mask) {
		spell_flag &= ~mask;
		if (msp_ptr[j].slevel <= p_ptr->lev)i++;
	    }
	}

	/* only bother with spells learnable by class -CFT */
	spell_flag = spellmasks[py.misc.pclass][1] & ~spell_learned2;
	mask = 0x1;
	for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++) {
	    if (spell_flag & mask) {
		spell_flag &= ~mask;
		if (msp_ptr[j + 32].slevel <= p_ptr->lev) i++;
	    }
	}

	if (new_spells > i) new_spells = i;
    }
    } else 

    if (new_spells < 0) {

	/* forget spells until new_spells zero or no more spells know, spells are
	 * forgotten in the opposite order that they were learned */
	for (i = 63; new_spells && (spell_learned | spell_learned2); i--) {
	    /* j is the (i+1)th spell learned */
	    j = spell_order[i];

	    /* shifting by amounts greater than number of bits in long gives an
	     * undefined result, so don't shift for unknown spells */
	    /* don't process unknown spells... -CFT */
	    if (j == 99) continue;

	    if (j < 32) {	   /* use spell_learned, spell_forgotten... -CFT */
		mask = 1L << j;	   /* bit in spell fields */
		if (mask & spell_learned) {
		    spell_learned &= ~mask;
		    spell_forgotten |= mask;
		    new_spells++;
		    sprintf(tmp_str, "You have forgotten the %s of %s.",
			    p, spell_names[j + offset]);
		    msg_print(tmp_str);
		} /* if mask&spell_learned */
	    }

	    /* j < 32 */ 
	    else {		   /* j > 31, use spell_learned2, spell_forgotten2... -CFT */
		mask = 1L << (j - 32);	/* bit in spell fields */
		if (mask & spell_learned2) {
		    spell_learned2 &= ~mask;
		    spell_forgotten2 |= mask;
		    new_spells++;
		    sprintf(tmp_str, "You have forgotten the %s of %s.",
			    p, spell_names[j + offset]);
		    msg_print(tmp_str);
		} /* if mask&spell_learned2 */
	    } /* j > 31 */
	} /* for loop... */

	new_spells = 0;		   /* we've forgotten, so we shouldn't be learning any... */
    }


    if (new_spells != py.flags.new_spells) {
	if (new_spells > 0 && py.flags.new_spells == 0) {
	    (void)sprintf(tmp_str, "You can learn some new %ss now.", p);
	    msg_print(tmp_str);
	}
	py.flags.new_spells = new_spells;
	py.flags.status |= PY_STUDY;
    }
}


/*
 * gain spells when player wants to		- jw 
 */
void gain_spells(void)
{
    char                query;
    int                 stat, diff_spells, new_spells;
    int                 spells[63], offset, last_known;
    register int        i, j;
    register u32b     spell_flag = 0, spell_flag2 = 0, mask;
    vtype               tmp_str;
    struct misc         *p_ptr;
    register spell_type *msp_ptr;

    if (!py.misc.pclass) {
	msg_print("A warrior learn magic???  HA!");
	return;
    }

    i = 0;
    if (py.flags.blind > 0) {
	msg_print("You can't see to read your spell book!");
    }

    else if (no_lite()) {
	msg_print("You have no light to read by.");
    }
    else if (py.flags.confused > 0) {
	msg_print("You are too confused.");
    }
    else i = 1;
    if (i == 0) return;

    new_spells = py.flags.new_spells;
    diff_spells = 0;
    p_ptr = &py.misc;
    msp_ptr = &magic_spell[p_ptr->pclass - 1][0];
    if (class[p_ptr->pclass].spell == MAGE) {
	stat = A_INT;
	offset = SPELL_OFFSET;
    } else {
	stat = A_WIS;
	offset = PRAYER_OFFSET;
    }


    for (last_known = 0; last_known < 64; last_known++) {
	if (spell_order[last_known] == 99) break;
    }

    if (!new_spells) {
	(void)sprintf(tmp_str, "You can't learn any new %ss!",
		      (stat == A_INT ? "spell" : "prayer"));
	msg_print(tmp_str);
	free_turn_flag = TRUE;
    }
    else {

	/* Determine which spells player can learn */
	/* mages need the book to learn a spell, priests do not need the book */
	spell_flag = 0;
	spell_flag2 = 0;
	for (i = 0; i < inven_ctr; i++) {
	    if (((stat == A_INT) && (inventory[i].tval == TV_MAGIC_BOOK)) ||
		((stat == A_WIS) && (inventory[i].tval == TV_PRAYER_BOOK))) {
		spell_flag |= inventory[i].flags1;
		spell_flag2 |= inventory[i].flags2;
	    }
	}
    }

    /* clear bits for spells already learned */
    spell_flag &= ~spell_learned;
    spell_flag2 &= ~spell_learned2;

    mask = 0x1;
    i = 0;
    for (j = 0, mask = 0x1; (spell_flag | spell_flag2); mask <<= 1, j++) {
	if (spell_flag & mask) {
	    spell_flag &= ~mask;
	    if (msp_ptr[j].slevel <= p_ptr->lev) {
		spells[i] = j;
		i++;
	    }
	}
	if (spell_flag2 & mask) {
	    spell_flag2 &= ~mask;
	    if (msp_ptr[j + 32].slevel <= p_ptr->lev) {
		spells[i] = j + 32;
		i++;
	    }
	}
    }

    if (new_spells > i) {
	msg_print("You seem to be missing a book.");
	diff_spells = new_spells - i;
	new_spells = i;
    }

    if (new_spells == 0);

    else if (stat == A_INT) {

	/* get to choose which mage spells will be learned */
	save_screen();

	print_spells(spells, i, FALSE, -1);
	while (new_spells && get_com("Learn which spell?", &query)) {
	    j = query - 'a';

	/* test j < 23 in case i is greater than 22, only 22 spells are
	 * actually shown on the screen, so limit choice to those */
	    if (j >= 0 && j < i && j < 22) {

		new_spells--;
		if (spells[j] < 32) {
		    spell_learned |= 1L << spells[j];
		}
		else {
		    spell_learned2 |= 1L << (spells[j] - 32);
		}
		spell_order[last_known++] = spells[j];

		for (; j <= i - 1; j++) spells[j] = spells[j + 1];
		i--;
		erase_line(j + 1, 31);
		print_spells(spells, i, FALSE, -1);
	    } else
	    bell();
	}

	restore_screen();
    }

    else {

	/* pick a prayer at random */
	while (new_spells) {
	    j = randint(i) - 1;
	    if (spells[j] < 32) {
		spell_learned |= 1L << spells[j];
	    }
	    else {
		spell_learned2 |= 1L << (spells[j] - 32);
	    }
	    spell_order[last_known++] = spells[j];
	    (void)sprintf(tmp_str,
			  "You have learned the prayer of %s.",
			  spell_names[spells[j] + offset]);
	    msg_print(tmp_str);
	    for (; j <= i - 1; j++) {
		spells[j] = spells[j + 1];
	    }
	    i--;
	    new_spells--;
	}
    }

    py.flags.new_spells = new_spells + diff_spells;

    if (py.flags.new_spells == 0) {
	py.flags.status |= PY_STUDY;
    }

    /* set the mana for first level characters when they learn first spell */
    if (py.misc.mana == 0) calc_mana(stat);
}



/*
 * Gain some mana if you know at least one spell	-RAK-	 
 */
void calc_mana(int stat)
{
    register int          new_mana, levels;
    register struct misc *p_ptr = &py.misc;
    register s32b        value;
    register int          i;
    register inven_type  *i_ptr;
    int                   amrwgt, maxwgt;

    if (spell_learned != 0 || spell_learned2 != 0) {
	levels = p_ptr->lev - class[p_ptr->pclass].first_spell_lev + 1;
	switch (stat_adj(stat)) {
	  case 0:
	    new_mana = 0;
	    break;
	  case 1:
	  case 2:
	    new_mana = 1 * levels;
	    break;
	  case 3:
	    new_mana = 3 * levels / 2;
	    break;
	  case 4:
	    new_mana = 2 * levels;
	    break;
	  case 5:
	    new_mana = 5 * levels / 2;
	    break;
	  case 6:
	    new_mana = 3 * levels;
	    break;
	  case 7:
	    new_mana = 4 * levels;
	    break;
	  case 8:
	    new_mana = 9 * levels / 2;
	    break;
	  case 9:
	    new_mana = 5 * levels;
	    break;
	  case 10:
	    new_mana = 11 * levels / 2;
	    break;
	  case 11:
	    new_mana = 6 * levels;
	    break;
	  case 12:
	    new_mana = 13 * levels / 2;
	    break;
	  case 13:
	    new_mana = 7 * levels;
	    break;
	  case 14:
	    new_mana = 15 * levels / 2;
	    break;
	  default:
	    new_mana = 8 * levels;
	    break;
	}

	/* increment mana by one, so that first level chars have 2 mana */
	if (new_mana > 0) new_mana++;

	if ((inventory[INVEN_HANDS].tval != TV_NOTHING) &&
	    !((inventory[INVEN_HANDS].flags & TR_FREE_ACT) ||
	      ((inventory[INVEN_HANDS].flags & TR_DEX) &&
	       (inventory[INVEN_HANDS].p1 > 0)))

/* gauntlets of dex (or free action - DGK) can hardly interfere w/ spellcasting!
 * But cursed ones can! -CFT */

	    &&(py.misc.pclass == 1 ||
		py.misc.pclass == 3 ||
		py.misc.pclass == 4)) {
		new_mana = (3 * new_mana) / 4;
	}

	/* Start of **NEW ENCUMBRANCE CALCULATION**    -DGK- */
	amrwgt = 0;
	for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
	    i_ptr = &inventory[i];
	    switch (i) {
	      case INVEN_HEAD:
	      case INVEN_BODY:
	      case INVEN_ARM:
	      case INVEN_HANDS:
	      case INVEN_FEET:
	      case INVEN_OUTER:
		amrwgt += i_ptr->weight;
	    }
	}

	switch (py.misc.pclass) {
	  case 1:
	    maxwgt = 300;
	    break;
	  case 2:
	    maxwgt = 350;
	    break;
	  case 3:
	    maxwgt = 350;
	    break;
	  case 4:
	    maxwgt = 400;
	    break;
	  case 5:
	    maxwgt = 400;
	    break;
	  default:
	    maxwgt = 0;
	}

	if (amrwgt > maxwgt) {
	    new_mana -= ((amrwgt - maxwgt) / 10);
	}
    /* end of new mana calc */

    /* if low int/wis, gloves, and lots of heavy armor, new_mana could be
     * negative.  This would be very unlikely, except when int/wis was high
     * enough to compensate for armor, but was severly drained by an annoying
     * monster.  Since the following code blindly assumes that new_mana is >=
     * 0, we must do the work and return here. -CFT 
     */
	if (new_mana < 1) {
	    p_ptr->cmana = p_ptr->cmana_frac = p_ptr->mana = 0;
	    py.flags.status |= PY_MANA;
	    return;		   /* now return before we reach code that
				    * assumes new_mana is positive.... */
	}

	/* mana can be zero when creating character */
	if (p_ptr->mana != new_mana) {

	    if (p_ptr->mana != 0) {
	    /*
	     * change current mana proportionately to change of max mana,
	     * divide first to avoid overflow, little loss of accuracy 
	     */
		value = ((((long)p_ptr->cmana << 16) + p_ptr->cmana_frac) /
			 p_ptr->mana * new_mana);
		p_ptr->cmana = value >> 16;
		p_ptr->cmana_frac = value & 0xFFFF;
	    }
	    else {
		p_ptr->cmana = new_mana;
		p_ptr->cmana_frac = 0;
	    }

	    p_ptr->mana = new_mana;

	    /* can't print mana here, may be in store or inventory mode */
	    py.flags.status |= PY_MANA;
	}
    }

    else if (p_ptr->mana != 0) {
	p_ptr->mana = 0;
	p_ptr->cmana = 0;
	/* can't print mana here, may be in store or inventory mode */
	py.flags.status |= PY_MANA;
    }
}


/* Throw a magic spell					-RAK-	 */
void 
cast()
{
    int                    i, j, item_val, dir;
    int                    choice, chance, result;
    register struct flags *f_ptr;
    register struct misc  *p_ptr;
    register spell_type   *m_ptr;

    free_turn_flag = TRUE;
    if (py.flags.blind > 0)
	msg_print("You can't see to read your spell book!");
    else if (no_lite())
	msg_print("You have no light to read by.");
    else if (py.flags.confused > 0)
	msg_print("You are too confused.");
    else if (class[py.misc.pclass].spell != MAGE)
	msg_print("You can't cast spells!");
    else if (!find_range(TV_MAGIC_BOOK, TV_NEVER, &i, &j))
	msg_print("But you are not carrying any spell-books!");
    else if (get_item(&item_val, "Use which spell-book?", i, j, 0)) {
	result = cast_spell("Cast which spell?", item_val, &choice, &chance);
	if (py.flags.stun > 50)
	    chance += 25;
	else if (py.flags.stun > 0)
	    chance += 15;
	if (result < 0)
	    msg_print("You don't know any spells in that book.");
	else if (result > 0) {
	    m_ptr = &magic_spell[py.misc.pclass - 1][choice];
	    free_turn_flag = FALSE;

	    if (randint(100) <= chance)	/* changed -CFT */
		msg_print("You failed to get the spell off!");
	    else {
		chance = py.misc.lev/
		    (py.misc.pclass==1 ? 2 : (py.misc.pclass==4 ? 4 : 5))
                        + stat_adj(A_INT); /* does missile spell do line? -CFT */
		
		/* Spells.  */
		switch (choice + 1) {
		  case 1:
		    if (get_dir(NULL, &dir))
			if (randint(100) < (chance-10))
			    line_spell(GF_MISSILE, dir, char_row, char_col,
				       damroll(3 + ((py.misc.lev - 1) / 5), 4) );
			else
			    fire_bolt(GF_MISSILE, dir, char_row, char_col,
				      damroll(3 + ((py.misc.lev - 1) / 5), 4) );
		    break;
		  case 2:
		    (void)detect_monsters();
		    break;
		  case 3:
		    teleport(10);
		    break;
		  case 4:
		    (void)lite_area(char_row, char_col,
		       damroll(2, (py.misc.lev / 2)), (py.misc.lev / 10) + 1);
		    break;
		  case 5:	   /* treasure detection */
		    (void)detect_treasure();
		    break;
		  case 6:
		    (void)hp_player(damroll(4, 4));
		    if (py.flags.cut > 0) {
			py.flags.cut -= 15;
			if (py.flags.cut < 0)
			    py.flags.cut = 0;
			msg_print("Your wounds heal.");
		    }
		    break;
		  case 7:	   /* object detection */
		    (void)detect_object();
		    break;
		  case 8:
		    (void)detect_sdoor();
		    (void)detect_trap();
		    break;
		  case 9:
		    if (get_dir(NULL, &dir))
			fire_ball(GF_POIS, dir, char_row, char_col,
				  10 + (py.misc.lev / 2), 2);
		    break;
		  case 10:
		    if (get_dir(NULL, &dir))
			(void)confuse_monster(dir, char_row, char_col, py.misc.lev);
		    break;
		  case 11:
		    if (get_dir(NULL, &dir))
			if (randint(100) < (chance-10))
			    line_spell(GF_ELEC, dir, char_row, char_col,
				       damroll(3+((py.misc.lev-5)/4),8));
			else
			    fire_bolt(GF_ELEC, dir, char_row, char_col,
				      damroll(3+((py.misc.lev-5)/4),8));
		    break;
		  case 12:
		    (void)td_destroy();
		    break;
		  case 13:
		    if (get_dir(NULL, &dir))
			(void)sleep_monster(dir, char_row, char_col);
		    break;
		  case 14:
		    (void)cure_poison();
		    break;
		  case 15:
		    teleport((int)(py.misc.lev * 5));
		    break;
		  case 16:
		    if (get_dir(NULL, &dir)) {
			msg_print("A line of blue shimmering light appears.");
			lite_line(dir, char_row, char_col);
		    }
		    break;
		  case 17:
		    if (get_dir(NULL, &dir))
			if (randint(100) < (chance-10))
			    line_spell(GF_COLD, dir, char_row, char_col,
				       damroll(5+((py.misc.lev-5)/4),8));
			else
			    fire_bolt(GF_COLD, dir, char_row, char_col,
				      damroll(5+((py.misc.lev-5)/4),8));
		    break;
		  case 18:
		    if (get_dir(NULL, &dir))
			(void)wall_to_mud(dir, char_row, char_col);
		    break;
		  case 19:
		    create_food();
		    break;
		  case 20:
		    (void)recharge(5);
		    break;
		  case 21:
		    (void)sleep_monsters1(char_row, char_col);
		    break;
		  case 22:
		    if (get_dir(NULL, &dir))
			(void)poly_monster(dir, char_row, char_col);
		    break;
		  case 23:
		    (void)ident_spell();
		    break;
		  case 24:
		    (void)sleep_monsters2();
		    break;
		  case 25:
		    if (get_dir(NULL, &dir))
			if (randint(100) < chance)
			    line_spell(GF_FIRE, dir, char_row, char_col,
				       damroll(8+((py.misc.lev-5)/4),8));
			else
			    fire_bolt(GF_FIRE, dir, char_row, char_col,
				      damroll(8+((py.misc.lev-5)/4),8));
		    break;
		  case 26:
		    if (get_dir(NULL, &dir))
			(void)speed_monster(dir, char_row, char_col, -1);
		    break;
		  case 27:
		    if (get_dir(NULL, &dir))
			fire_ball(GF_COLD, dir, char_row, char_col,
				  30 + (py.misc.lev), 2);
		    break;
		  case 28:
		    (void)recharge(40);
		    break;
		  case 29:
		    if (get_dir(NULL, &dir))
			(void)teleport_monster(dir, char_row, char_col);
		    break;
		  case 30:
		    f_ptr = &py.flags;
		    if (f_ptr->fast <= 0)
			f_ptr->fast += randint(20) + py.misc.lev;
		    else
			f_ptr->fast += randint(5);
		    break;
		  case 31:
		    if (get_dir(NULL, &dir))
			fire_ball(GF_FIRE, dir, char_row, char_col
				  , 55 + (py.misc.lev), 2);
		    break;
		  case 32:
		    destroy_area(char_row, char_col);
		    break;
		  case 33:
		    (void)genocide(TRUE);
		    break;
		  case 34:	   /* door creation */
		    (void)door_creation();
		    break;
		  case 35:	   /* Stair creation */
		    (void)stair_creation();
		    break;
		  case 36:	   /* Teleport level */
		    (void)tele_level();
		    break;
		  case 37:	   /* Earthquake */
		    earthquake();
		    break;
		  case 38:	   /* Word of Recall */
		    if (py.flags.word_recall == 0) {
			py.flags.word_recall = 15 + randint(20);
			msg_print("The air about you becomes charged...");
		    } else {
			py.flags.word_recall = 0;
			msg_print("A tension leaves the air around you...");
		    }
		    break;
		  case 39:	   /* Acid Bolt */
		    if (get_dir(NULL, &dir))
			if (randint(100) < (chance-5))
			    line_spell(GF_ACID, dir, char_row, char_col,
				       damroll(6+((py.misc.lev-5)/4), 8));
			else
			    fire_bolt(GF_ACID, dir, char_row, char_col,
				      damroll(6+((py.misc.lev-5)/4), 8));
		    break;
		  case 40:	   /* Cloud kill */
		    if (get_dir(NULL, &dir))
			fire_ball(GF_POIS, dir, char_row, char_col,
				  20 + (py.misc.lev / 2), 3);
		    break;
		  case 41:	   /* Acid Ball */
		    if (get_dir(NULL, &dir))
			fire_ball(GF_ACID, dir, char_row, char_col,
				  40 + (py.misc.lev), 2);
		    break;
		  case 42:	   /* Ice Storm */
		    if (get_dir(NULL, &dir))
			fire_ball(GF_COLD, dir, char_row, char_col,
				  70 + (py.misc.lev), 3);
		    break;
		  case 43:	   /* Meteor Swarm */
		    if (get_dir(NULL, &dir))
			fire_ball(GF_METEOR, dir, char_row, char_col,
				  65 + (py.misc.lev), 3);
		    break;
		  case 44:	   /* Hellfire */
		    if (get_dir(NULL, &dir))
			fire_ball(GF_HOLY_ORB, dir, char_row, char_col, 300, 2);
		    break;
		  case 45:	   /* Detect Evil */
		    (void)detect_evil();
		    break;
		  case 46:	   /* Detect Enchantment */
		    (void)detect_magic();
		    break;
		  case 47:
		    recharge(100);
		    break;
		  case 48:
		    (void)genocide(TRUE);
		    break;
		  case 49:
		    (void)mass_genocide(TRUE);
		    break;
		  case 50:
		    py.flags.oppose_fire += randint(20) + 20;
		    break;
		  case 51:
		    py.flags.oppose_cold += randint(20) + 20;
		    break;
		  case 52:
		    py.flags.oppose_acid += randint(20) + 20;
		    break;
		  case 53:
		    py.flags.oppose_pois += randint(20) + 20;
		    break;
		  case 54:
		    py.flags.oppose_fire += randint(20) + 20;
		    py.flags.oppose_cold += randint(20) + 20;
		    py.flags.oppose_elec += randint(20) + 20;
		    py.flags.oppose_pois += randint(20) + 20;
		    py.flags.oppose_acid += randint(20) + 20;
		    break;
		  case 55:
		    py.flags.hero += randint(25) + 25;
		    break;
		  case 56:
		    py.flags.shield += randint(20) + 30;
		    calc_bonuses();
		    prt_pac();
		    calc_mana(A_INT);
		    msg_print("A mystic shield forms around your body!");
		    break;
		  case 57:
		    py.flags.shero += randint(25) + 25;
		    break;
		  case 58:
		    if (py.flags.fast <= 0)
			py.flags.fast += randint(30) + 30 + py.misc.lev;
		    else
			py.flags.fast += randint(5);
		    break;
		  case 59:
		    py.flags.invuln += randint(8) + 8;
		    break;
		  default:
		    break;
		}
	    /* End of spells.				     */
		if (!free_turn_flag) {
		    p_ptr = &py.misc;
		    if (choice < 32) {
			if ((spell_worked & (1L << choice)) == 0) {
			    p_ptr->exp += m_ptr->sexp << 2;
			    spell_worked |= (1L << choice);
			    prt_experience();
			}
		    } else {
			if ((spell_worked2 & (1L << (choice - 32))) == 0) {
			    p_ptr->exp += m_ptr->sexp << 2;
			    spell_worked2 |= (1L << (choice - 32));
			    prt_experience();
			}
		    }
		}
	    }
	    p_ptr = &py.misc;
	    if (!free_turn_flag) {
		if (m_ptr->smana > p_ptr->cmana) {
		    msg_print("You faint from the effort!");
		    py.flags.paralysis =
			randint((int)(5 * (m_ptr->smana - p_ptr->cmana)));
		    p_ptr->cmana = 0;
		    p_ptr->cmana_frac = 0;
		    if (randint(3) == 1) {
			msg_print("You have damaged your health!");
			(void)dec_stat(A_CON);
		    }
		} else
		    p_ptr->cmana -= m_ptr->smana;
		prt_cmana();
	    }
	}
    }
}
