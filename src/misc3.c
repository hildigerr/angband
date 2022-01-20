/* File: misc3.c */ 

/* Purpose: misc code for objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/* include before angband.h because param.h defines NULL incorrectly */
#ifndef USG
#include <sys/param.h>
#endif

#include "angband.h"


#ifdef ATARIST_MWC
char               *index();
#endif


extern int rating;


/*
 * Pushs a record back onto free space list		-RAK-
 *
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


/*
 * Deletes object from given location			-RAK-	
 */
int delete_object(int y, int x)
{
    register int        delete;
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];
    if (c_ptr->fval == BLOCKED_FLOOR)
	c_ptr->fval = CORR_FLOOR;
    pusht(c_ptr->i_idx);		   /* then eliminate it */
    c_ptr->i_idx = 0;
    c_ptr->fm = FALSE;
    lite_spot(y, x);
    if (test_lite(y, x))
	delete = TRUE;
    else
	delete = FALSE;
    return (delete);
}



/*
 * Link all free space in treasure list together
 * (called only from "generate.c")
 */
void wipe_i_list()
{
    register int i;

    /* Wipe the object list */
    for (i = 0; i < MAX_I_IDX; i++) {

	/* Blank the object */
	invcopy(&i_list[i], OBJ_NOTHING);
    }

    /* No "real" items */
    i_max = MIN_I_IDX;
}



/*
 * When too many objects gather on the floor, delete some of them
 *
 * Note that the player could intentionally collect so many artifacts,
 * and create so many stairs, that we become unable to compact.
 * This may cause the program to crash.
 */
static void compact_objects()
{
    register int        i, j;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    int                 num;
    int			cur_dis, chance;


    /* Debugging message */
    msg_print("Compacting objects...");

    for (num = 0, cur_dis = 66; num <= 0;) {

	/* Examine the dungeon */
	for (i = 0; i < cur_height; i++) {
	    for (j = 0; j < cur_width; j++) {

		/* Do not consider artifacts or stairs */
		if (!valid_grid(i,j)) continue;

		/* Get the location */
		c_ptr = &cave[i][j];

		/* Do not even consider empty grids */
		if (c_ptr->i_idx == 0) continue;

		/* Get the object */
		i_ptr = &i_list[c_ptr->i_idx];

		/* Nearby objects start out "immune" */
		if (distance(i, j, char_row, char_col) < cur_dis) continue;

		/* Every object gets a "saving throw" */
		switch (i_ptr->tval) {
		    case TV_VIS_TRAP:
			chance = 15;
			break;
		    case TV_RUBBLE:
		    case TV_INVIS_TRAP:
		    case TV_OPEN_DOOR:
		    case TV_CLOSED_DOOR:
			chance = 5;
			break;
		    case TV_SECRET_DOOR:
			chance = 3;
			break;
		    default:
			chance = 10;
		}

		/* Apply the saving throw */
		if (randint(100) > chance) continue;

		/* Delete it */
		delete_object(i, j);

		/* Count it */
		num++;
	    }
	}
	if (num == 0) cur_dis -= 6;
    }

    if (cur_dis < 66)

    /* Redraw */
    prt_map();
}


/*
 * Acquires and returns the index of a "free" item.
 */
int i_pop(void)
{
    /* Compact if needed */
    if (i_max == MAX_I_IDX) compact_objects();

    /* Return the next free space */
    return (i_max++);
}



/*
 * Boolean : is object enchanted	  -RAK- 
 */
int magik(int chance)
{
    if (randint(100) <= chance) return (TRUE);

    return (FALSE);
}


/*
 * Enchant a bonus based on degree desired -RAK-
 *
 * Lets just change this to make sense.  Now it goes from base to limit,
 * roughly proportional to the level.... -CWS
 */
int m_bonus(int base, int limit, int level)
{
    register int x, stand_dev, tmp, diff = limit - base;

#ifdef USE_FLOATING_POINT

    /* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100.0)) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* Call an odd function */
    tmp = randnor(0, stand_dev);

    /* Extract a weird value */
    x = (tmp * diff / 150.0) + (level * limit / 200.0) + base;

#else

    /* XXX XXX Hack -- this may not be what was desired */
    stand_dev = (OBJ_STD_ADJ * level / 100) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* Call an odd function */
    tmp = randnor(0, stand_dev);

    /* Extract a weird value */
    x = (tmp * diff / 150) + (level * limit / 200) + base;

#endif

    /* Enforce minimum value */
    if (x < base) return (base);

    /* Return the extracted value */
    return (x);
}


/*
 * Give an item one of the "powerful resistances"
 */
static void give_1_hi_resist(inven_type *i_ptr)
{
    switch (randint(10)) {
	case 1: i_ptr->flags2 |= TR2_RES_CONF; break;
	case 2: i_ptr->flags2 |= TR2_RES_SOUND; break;
	case 3: i_ptr->flags2 |= TR2_RES_LITE; break;
	case 4: i_ptr->flags2 |= TR2_RES_DARK; break;
	case 5: i_ptr->flags2 |= TR2_RES_CHAOS; break;
	case 6: i_ptr->flags2 |= TR2_RES_NETHER; break;
	case 7: i_ptr->flags2 |= TR2_RES_SHARDS; break;
	case 8: i_ptr->flags2 |= TR2_RES_NEXUS; break;
	case 9: i_ptr->flags2 |= TR2_RES_BLIND; break;
	case 10: i_ptr->flags2 |= TR2_RES_DISEN; break;
    }
}


/*
 * Charge a wand  XXX Redo this cleanly!
 */
static void charge_wand(inven_type *i_ptr)
{
    switch (i_ptr->sval) {
      case 0:
	i_ptr->pval = randint(10) + 6;
	break;
      case 1:
	i_ptr->pval = randint(8) + 6;
	break;
      case 2:
	i_ptr->pval = randint(5) + 6;
	break;
      case 3:
	i_ptr->pval = randint(8) + 6;
	break;
      case 4:
	i_ptr->pval = randint(4) + 3;
	break;
      case 5:
	i_ptr->pval = randint(8) + 6;
	break;
      case 6:
	i_ptr->pval = randint(20) + 12;
	break;
      case 7:
	i_ptr->pval = randint(20) + 12;
	break;
      case 8:
	i_ptr->pval = randint(10) + 6;
	break;
      case 9:
	i_ptr->pval = randint(12) + 6;
	break;
      case 10:
	i_ptr->pval = randint(10) + 12;
	break;
      case 11:
	i_ptr->pval = randint(3) + 3;
	break;
      case 12:
	i_ptr->pval = randint(8) + 6;
	break;
      case 13:
	i_ptr->pval = randint(10) + 6;
	break;
      case 14:
	i_ptr->pval = randint(5) + 3;
	break;
      case 15:
	i_ptr->pval = randint(5) + 3;
	break;
      case 16:
	i_ptr->pval = randint(5) + 6;
	break;
      case 17:
	i_ptr->pval = randint(5) + 4;
	break;
      case 18:
	i_ptr->pval = randint(8) + 4;
	break;
      case 19:
	i_ptr->pval = randint(6) + 2;
	break;
      case 20:
	i_ptr->pval = randint(4) + 2;
	break;
      case 21:
	i_ptr->pval = randint(8) + 6;
	break;
      case 22:
	i_ptr->pval = randint(5) + 2;
	break;
      case 23:
	i_ptr->pval = randint(12) + 12;
	break;
      case 24:
	i_ptr->pval = randint(3) + 1;
	break;
      case 25:
	i_ptr->pval = randint(3) + 1;
	break;
      case 26:
	i_ptr->pval = randint(3) + 1;
	break;
      case 27:
	i_ptr->pval = randint(2) + 1;
	break;
      case 28:
	i_ptr->pval = randint(8) + 6;
	break;
      default:
	break;
    }
}


/*
 * Charge a staff  XXX Redo this cleanly!
 */
static void charge_staff(inven_type *i_ptr)
{        
    switch (i_ptr->sval) {
      case 0:
	i_ptr->pval = randint(20) + 12;
	break;
      case 1:
	i_ptr->pval = randint(8) + 6;
	break;
      case 2:
	i_ptr->pval = randint(5) + 6;
	break;
      case 3:
	i_ptr->pval = randint(20) + 12;
	break;
      case 4:
	i_ptr->pval = randint(15) + 6;
	break;
      case 5:
	i_ptr->pval = randint(4) + 5;
	break;
      case 6:
	i_ptr->pval = randint(5) + 3;
	break;
      case 7:
	i_ptr->pval = randint(3) + 1;
	i_ptr->level = 10;
	break;
      case 8:
	i_ptr->pval = randint(3) + 1;
	break;
      case 9:
	i_ptr->pval = randint(5) + 6;
	break;
      case 10:
	i_ptr->pval = randint(10) + 12;
	break;
      case 11:
	i_ptr->pval = randint(5) + 6;
	break;
      case 12:
	i_ptr->pval = randint(5) + 6;
	break;
      case 13:
	i_ptr->pval = randint(5) + 6;
	break;
      case 14:
	i_ptr->pval = randint(10) + 12;
	break;
      case 15:
	i_ptr->pval = randint(3) + 4;
	break;
      case 16:
	i_ptr->pval = randint(5) + 6;
	break;
      case 17:
	i_ptr->pval = randint(5) + 6;
	break;
      case 18:
	i_ptr->pval = randint(3) + 4;
	break;
      case 19:
	i_ptr->pval = randint(10) + 12;
	break;
      case 20:
	i_ptr->pval = randint(3) + 4;
	break;
      case 21:
	i_ptr->pval = randint(3) + 4;
	break;
      case 22:
	i_ptr->pval = randint(10) + 6;
	i_ptr->level = 5;
	break;
      case 23:
	i_ptr->pval = randint(2) + 1;
	break;
      case 24:
	i_ptr->pval = randint(3) + 1;
	break;
      case 25:
	i_ptr->pval = randint(2) + 2;
	break;
      case 26:
	i_ptr->pval = randint(15) + 5;
	break;
      case 27:
	i_ptr->pval = randint(2) + 2;
	break;
      case 28:
	i_ptr->pval = randint(5) + 5;
	break;
      case 29:
	i_ptr->pval = randint(2) + 1;
	break;
      case 30:
	i_ptr->pval = randint(6) + 2;
	break;
      default:
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
	charge_wand(t_ptr);
	break;


      case TV_STAFF:
	charge_staff(t_ptr);
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



/*
 * Places a particular trap at location y, x		-RAK-	 
 */
void place_trap(int y, int x, int sval)
{
    register int cur_pos;


    /* Do not hurt artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;


    /* Don't put traps under player/monsters, it's annoying -CFT */
    if (cave[y][x].m_idx >= MIN_M_IDX) return;


    /* Delete whatever is there */
    delete_object(y, x);

    /* Make a new object */
    cur_pos = i_pop();
    cave[y][x].i_idx = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_TRAP_LIST + sval);
}


/*
 * Places rubble at location y, x			-RAK-	
 */
void place_rubble(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;

    /* Do not hurt artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;
    
    /* Delete whatever is there */
    delete_object(y, x);

    cur_pos = i_pop();
    c_ptr = &cave[y][x];
    c_ptr->i_idx = cur_pos;
    c_ptr->fval = BLOCKED_FLOOR;
    invcopy(&i_list[cur_pos], OBJ_RUBBLE);
}


/*
 * if killed a 'Creeping _xxx_ coins'... -CWS
 */
void get_coin_type(monster_race *r_ptr)
{
    cptr name;

    name = r_ptr->name;
    if (!stricmp(name, "Creeping copper coins")) coin_type = 2;
    if (!stricmp(name, "Creeping silver coins")) coin_type = 5;
    if (!stricmp(name, "Creeping gold coins")) coin_type = 10;
    if (!stricmp(name, "Creeping mithril coins"))coin_type = 16;
    if (!stricmp(name, "Creeping adamantite coins")) coin_type = 17;
}

/*
 * Places a treasure (Gold or Gems) at given row, column -RAK-	
 */
void place_gold(int y, int x)
{
    register int        i, cur_pos;
    register inven_type *i_ptr;

    /* Do not hurt illegals, artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;

    /* Delete the object under us (acidic gold?) */
	delete_object(y, x);

    /* Make it */
    cur_pos = i_pop();

    /* Pick a Treasure variety */
    i = ((randint(object_level + 2) + 2) / 2) - 1;

    /* Apply "extra" magic */
    if (randint(GREAT_OBJ) == 1) {
	i += randint(object_level + 1);
    }

    /* Do not create "illegal" Treasure Types */
    if (i >= MAX_GOLD) i = MAX_GOLD - 1;

    if (coin_type) {			/* if killed a Creeping _xxx_ coins... */
	if (coin_type > MAX_GOLD - 1)
	    coin_type = 0;		/* safety check -CWS */
	i = coin_type;
    }

    cave[y][x].i_idx = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_GOLD_LIST + i);
    i_ptr = &i_list[cur_pos];
    i_ptr->cost += (8L * (long)randint((int)i_ptr->cost)) + randint(8);

    /* average the values to make Creeping _xxx_ coins not give too great treasure drops */
    if (coin_type) {
	i_ptr->cost = ((8L * (long)randint((int)k_list[OBJ_GOLD_LIST + i].cost))
		       + (i_ptr->cost)) >> 1;
    }

    /* Under the player */
    if (cave[y][x].m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}


/*
 * Returns the array number of a random object -RAK-
 */
int get_obj_num(int level, int good)
{
    register int i, j;

    do {
	if (level == 0)
	    i = randint(t_level[0]) - 1;
	else {
	    if (level >= MAX_OBJ_LEVEL)
		level = MAX_OBJ_LEVEL;
	    else if (randint(GREAT_OBJ) == 1) {
		level = level * MAX_OBJ_LEVEL / randint(MAX_OBJ_LEVEL) + 1;
		if (level > MAX_OBJ_LEVEL)
		    level = MAX_OBJ_LEVEL;
	    }
	/*
	 * This code has been added to make it slightly more likely to get
	 * the higher level objects.	Originally a uniform distribution
	 * over all objects less than or equal to the dungeon level.  This
	 * distribution makes a level n objects occur approx 2/n% of the time
	 * on level n, and 1/2n are 0th level. 
	 */

	    if (randint(2) == 1)
		i = randint(t_level[level]) - 1;
	    else {		   /* Choose three objects, pick the highest level. */
		i = randint(t_level[level]) - 1;
		j = randint(t_level[level]) - 1;
		if (i < j)
		    i = j;
		j = randint(t_level[level]) - 1;
		if (i < j)
		    i = j;
		j = k_list[sorted_objects[i]].level;
		if (j == 0)
		    i = randint(t_level[0]) - 1;
		else
		    i = randint(t_level[j] - t_level[j - 1]) - 1 + t_level[j - 1];
	    }
	}
    } while (((k_list[sorted_objects[i]].rare ?
	       (randint(k_list[sorted_objects[i]].rare) - 1) : 0) && !good)
	     || (k_list[sorted_objects[i]].rare == 255));
    return (i);
}



int special_place_object(int y, int x)
{
    register int	cur_pos, tmp;
    char         str[100];
    int          done = 0;


    /* Is this cave grid sacred? */
    if (!valid_grid(y,x)) return (FALSE);

    /* Delete anything that is there */
    delete_object(y, x);
    str[0] = 0;

again:
    if (done > 20)
	return 0;
    tmp = randint(MAX_OBJECTS - (OBJ_SPECIAL - 1)) + (OBJ_SPECIAL - 1) - 1;
    switch (tmp) {
      case (OBJ_SPECIAL - 1):
	done++;
	if (randint(30) > 1)
	    goto again;
	if (NARYA)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Narya");
	else
	    good_item_flag = TRUE;
	NARYA = TRUE;
	break;
      case (OBJ_SPECIAL):
	done++;
	if (randint(35) > 1)
	    goto again;
	if (NENYA)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Nenya");
	else
	    good_item_flag = TRUE;
	NENYA = TRUE;
	break;
      case (OBJ_SPECIAL + 1):
	done++;
	if (randint(40) > 1)
	    goto again;
	if (VILYA)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(70) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Vilya");
	else
	    good_item_flag = TRUE;
	VILYA = TRUE;
	break;
      case (OBJ_SPECIAL + 2):
	done++;
	if (randint(60) > 1)
	    goto again;
	if (POWER)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(100) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Power (The One Ring)");
	else
	    good_item_flag = TRUE;
	POWER = TRUE;
	break;
      case (OBJ_SPECIAL + 3):
	done++;
	if (PHIAL)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(30) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Phial of Galadriel");
	else
	    good_item_flag = TRUE;
	PHIAL = TRUE;
	break;
      case (OBJ_SPECIAL + 4):
	done++;
	if (randint(10) > 1)
	    goto again;
	if (INGWE)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Amulet of Ingwe");
	else
	    good_item_flag = TRUE;
	INGWE = TRUE;
	break;
      case (OBJ_SPECIAL + 5):
	done++;
	if (randint(6) > 1)
	    goto again;
	if (CARLAMMAS)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(35) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Amulet of Carlammas");
	else
	    good_item_flag = TRUE;
	CARLAMMAS = TRUE;
	break;
      case (OBJ_SPECIAL + 6):
	done++;
	if (randint(8) > 1)
	    goto again;
	if (ELENDIL)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(30) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Star of Elendil");
	else
	    good_item_flag = TRUE;
	ELENDIL = TRUE;
	break;
      case (OBJ_SPECIAL + 7):
	done++;
	if (randint(18) > 1)
	    goto again;
	if (THRAIN)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Arkenstone of Thrain");
	else
	    good_item_flag = TRUE;
	THRAIN = TRUE;
	break;
      case (OBJ_SPECIAL + 8):
	done++;
	if (randint(25) > 1)
	    goto again;
	if (TULKAS)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(65) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Ring of Tulkas");
	else
	    good_item_flag = TRUE;
	TULKAS = TRUE;
	break;
      case (OBJ_SPECIAL + 9):
	done++;
	if (randint(25) > 1)
	    goto again;
	if (NECKLACE)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(60) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Necklace of the Dwarves");
	else
	    good_item_flag = TRUE;
	NECKLACE = TRUE;
	break;
      case (OBJ_SPECIAL + 10):
	done++;
	if (randint(20) > 1)
	    goto again;
	if (BARAHIR)
	    goto again;
	if ((k_list[tmp].level - 40) > object_level)
	    goto again;
	if ((k_list[tmp].level > object_level) && (randint(50) > 1))
	    goto again;
	if ((wizard || peek))
	    sprintf(str, "Ring of Barahir");
	else
	    good_item_flag = TRUE;
	BARAHIR = TRUE;
	break;
    }
    if (strlen(str) > 0 && (wizard || peek))
	msg_print(str);

    /* Make the object, using the index from above */
    cur_pos = i_pop();

    cave[y][x].i_idx = cur_pos;
    invcopy(&i_list[cur_pos], tmp);
    i_list[cur_pos].timeout = 0;
    i_list[cur_pos].ident |= ID_NOSHOW_TYPE; /* don't show (+x of yyy) for these */
    if (k_list[tmp].level > object_level) {
	rating += 2 * (k_list[sorted_objects[tmp]].level - object_level);
    }

    /* Is it on the player? */
    if (cave[y][x].m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }

    return (-1);
}


/*
 * Attempts to places a random object at the given location -RAK-
 */
void place_object(int y, int x)
{
    register int cur_pos, tmp;

    /* Certain locations are not valid */
    if (!valid_grid(y,x)) return;

    /* Delete anything already there */
    delete_object(y, x);
    
    if (randint(MAX_OBJECTS)>OBJ_SPECIAL && randint(10)==1)
	if (special_place_object(y,x)==(-1))
	    return;

    /* Make it */
    cur_pos = i_pop();
    cave[y][x].i_idx = cur_pos;

    do {	   /* don't generate another chest if opening_chest is true -CWS */
	tmp = get_obj_num(dun_level, FALSE);
    } while (opening_chest && (k_list[sorted_objects[tmp]].tval == TV_CHEST));
	
    invcopy(&i_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, dun_level, FALSE, 0);
    if (k_list[sorted_objects[tmp]].level > dun_level)
	rating += k_list[sorted_objects[tmp]].level - dun_level;
    if (peek) {
	if (k_list[sorted_objects[tmp]].level > dun_level) {
	    char buf[200];
	    byte temp;
	    
	    temp=i_list[cur_pos].ident;
	    i_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &i_list[cur_pos], TRUE);
	    i_list[cur_pos].ident = temp;
	    msg_print(buf);
	}
    }

    /* Under the player */
    if (cave[y][x].m_idx == 1) {
	msg_print ("You feel something roll beneath your feet.");
    }
}


/*
 * Places a "GOOD" object at given row, column co-ordinate ~Ludwig 
 */
void place_good(int y, int x, u32b good)
{
    register int cur_pos, tmp;
    int          tv, is_good = FALSE;

    /* Do not hurt artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;

    /* Delete anything already there */
    delete_object(y, x);

    /* Hack -- much higher chance of doing "Special Objects" */
    if (randint(10) == 1) {
	if (special_place_object(y, x) == (-1)) return;
    }

    cur_pos = i_pop();
    cave[y][x].i_idx = cur_pos;
    do {

	/* Pick a random object, based on "object_level" */
	tmp = get_obj_num((object_level + 10), TRUE);

	/* Examine the object */
	tv = k_list[sorted_objects[tmp]].tval;

	if ((tv == TV_HELM) || (tv == TV_SHIELD) ||
	    (tv == TV_CLOAK) || (tv == TV_HAFTED) || (tv == TV_POLEARM) ||
	    (tv == TV_BOW) || (tv == TV_BOLT) || (tv == TV_ARROW) ||
	    (tv == TV_BOOTS) || (tv == TV_GLOVES))
	    is_good = TRUE;
	if ((tv == TV_SWORD) &&
	    strncmp("& Broken", k_list[sorted_objects[tmp]].name, 8))
	    is_good = TRUE;	   /* broken swords/daggers are NOT good!
				    * -CFT */
	if ((tv == TV_HARD_ARMOR) &&
	    strncmp("Rusty", k_list[sorted_objects[tmp]].name, 5))
	    is_good = TRUE;	   /* rusty chainmail is NOT good! -CFT */
	if ((tv == TV_SOFT_ARMOR) &&
	 stricmp("some filthy rags", k_list[sorted_objects[tmp]].name))
	    is_good = TRUE;	   /* nor are rags! -CFT */
	if ((tv == TV_MAGIC_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Raal's */
	    (k_list[sorted_objects[tmp]].sval > ((good & MF2_SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
	if ((tv == TV_PRAYER_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Wrath of
					 * God */
	    (k_list[sorted_objects[tmp]].sval > ((good & MF2_SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
    } while (!is_good);
    invcopy(&i_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, object_level, (good & MF2_SPECIAL) ? 666 : 1, 0);
    if (peek) {
	if (k_list[sorted_objects[tmp]].level > object_level) {
	    char                buf[200];
	    byte               t;

	    t = i_list[cur_pos].ident;
	    i_list[cur_pos].ident |= ID_STOREBOUGHT;
	    objdes(buf, &i_list[cur_pos], TRUE);
	    i_list[cur_pos].ident = t;
	    msg_print(buf);
	}
    }

    if (cave[y][x].m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}




/*
 * Create up to "num" objects near the given coordinates
 */
void random_object(int y, int x, int num)
{
    register int        i, j, k;

    /* Attempt to place 'num' objects */
    for (; num > 0; --num) {

	/* Try up to 11 spots looking for empty space */
	for (i = 0; i < 11; ++i) {

	    do {

	    /* Pick a random location */
	    j = rand_spread(y, 2);
	    k = rand_spread(x, 3);

	    /* Require legal grid */
	    } while (!in_bounds(j,k));
	    
	    /* Require "clean" floor space */
	    if (!clean_grid_bold(j,k)) continue;
		object_level = dun_level;

	    /* Place something */
	    if (randint(100) < 75) {
		place_object(j, k);
	    }
	    else {
		place_gold(j, k);
	    }

		i = 9;
	}
    }
}


/*
 * Same as above, but always "special"
 * Only really called by "scroll of *acquirement*"
 */
void special_random_object(int y, int x, int num)
{
    register int        i, j, k;

    object_level = dun_level;

    /* Place them */
    for (; num > 0; --num) {

	/* Try up to 11 spots looking for empty space */
	for (i = 0; i < 12; ++i) {

	    
	    /* Pick a random spot */
	    j = rand_spread(y, d);
	    k = rand_spread(x, d);
	    
	    /* Must have a clean grid */
	    if (!clean_grid_bold(j, k)) continue;

	    /* Perhaps attempt to place a "Special Object" */
	    if (randint(5) == 1) {
		if (!special_place_object(j, k))
			place_good(j, k, MF2_SPECIAL);
		} else {
		    place_good(j, k, MF2_SPECIAL);
		}
		i = 9;
	}
    }
}




/* Destroy an item in the inventory			-RAK-	 */
void inven_destroy(int item_val)
{
    register int         j;
    register inven_type *i_ptr;

    i_ptr = &inventory[item_val];
    if ((i_ptr->number > 1) && (i_ptr->sval <= ITEM_SINGLE_STACK_MAX)) {
	i_ptr->number--;
	inven_weight -= i_ptr->weight;
    } else {
	inven_weight -= i_ptr->weight * i_ptr->number;
	for (j = item_val; j < inven_ctr - 1; j++)
	    inventory[j] = inventory[j + 1];
	invcopy(&inventory[inven_ctr - 1], OBJ_NOTHING);
	inven_ctr--;
    }
    py.flags1.status |= PY_STR_WGT;
}


/*
 * Copies the object in the second argument over the first argument. However,
 * the second always gets a number of one except for ammo etc. 
 */
void take_one_item(inven_type *s_ptr, inven_type *i_ptr)
{
    *s_ptr = *i_ptr;
    if ((s_ptr->number > 1) && (s_ptr->sval >= ITEM_SINGLE_STACK_MIN)
	&& (s_ptr->sval <= ITEM_SINGLE_STACK_MAX))
	s_ptr->number = 1;
}


/* return FALSE if picking up an object would change the players speed */
int inven_check_weight(inven_type *i_ptr)
{
    register int i, new_inven_weight;

    i = weight_limit();
    new_inven_weight = i_ptr->number * i_ptr->weight + inven_weight;
    if (i < new_inven_weight)
	i = new_inven_weight / (i + 1);
    else
	i = 0;

    if (pack_heavy != i)
	return FALSE;
    else
	return TRUE;
}


/* Are we strong enough for the current pack and weapon?  -CJS-	 */
void check_strength()
{
    register int         i;
    register inven_type *i_ptr;
    static int           notlike = FALSE;

    i_ptr = &inventory[INVEN_WIELD];
    if (i_ptr->tval != TV_NOTHING
	&& (py.stats.use_stat[A_STR] * 15 < i_ptr->weight)) {
	if (weapon_heavy == FALSE) {
	    msg_print("You have trouble wielding such a heavy weapon.");
	    weapon_heavy = TRUE;
	    calc_bonuses();
	}
    } else if (weapon_heavy == TRUE) {
	weapon_heavy = FALSE;
	if (i_ptr->tval != TV_NOTHING)
	    msg_print("You are strong enough to wield your weapon.");
	else
	    msg_print("You feel relieved to put down your heavy weapon.");
	calc_bonuses();
    }
    i = weight_limit();
    if (i < inven_weight)
	i = inven_weight / (i + 1);
    else
	i = 0;
    if (pack_heavy != i) {
	if (pack_heavy < i)
	    msg_print("Your pack is so heavy that it slows you down.");
	else
	    msg_print("You move more easily under the weight of your pack.");
	py.flags1.speed += i - pack_heavy;
	py.flags1.status |= PY_SPEED;
	pack_heavy = i;
    }
    py.flags1.status &= ~PY_STR_WGT;

    if (py.misc.pclass == 2 && !notlike) {
        if ((i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
            && ((i_ptr->flags2 & TR_BLESS_BLADE) == 0)) {
            notlike = TRUE;
            msg_print("You do not feel comfortable with your weapon.");
        }
    } else if (py.misc.pclass == 2 && notlike) {
        if (i_ptr->tval == TV_NOTHING) {
            notlike = FALSE;
            msg_print("You feel comfortable again after removing that weapon.");
        } else if (!(i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
		   || !((i_ptr->flags2 & TR_BLESS_BLADE) == 0)) {
            notlike = FALSE;
            msg_print("You feel comfortable with your weapon once more.");
        }
    }
}




