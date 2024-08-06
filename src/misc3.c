/* File: misc3.c */ 

/* Purpose: misc code for objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




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
 * Help pick and create a "special" object
 *
 * XXX XXX This function is a total hack.
 * Note that the function below calls us 20 times per "call".
 */
static int make_artifact_special_aux(inven_type *i_ptr)
{
    int lev = object_level;


    /* Analyze it */
    switch (randint(12)) {

      case 1:
	if (PHIAL) return (0);
	if (k_list[OBJ_GALADRIEL].level > lev + 40) return (0);
	if ((k_list[OBJ_GALADRIEL].level > lev) && (randint(30) > 1)) return (0);
	invcopy(i_ptr, OBJ_GALADRIEL);
	PHIAL = TRUE;
	return (ART_GALADRIEL);
	break;

      case 2:
	if (randint(8) > 1) return (0);
	if (ELENDIL) return (0);
	if (k_list[OBJ_ELENDIL].level > lev + 40) return (0);
	if ((k_list[OBJ_ELENDIL].level > lev) && (randint(30) > 1)) return (0);
	invcopy(i_ptr, OBJ_ELENDIL);
	ELENDIL = TRUE;
	return (ART_ELENDIL);

      case 3:
	if (randint(18) > 1) return (0);
	if (THRAIN) return (0);
	if (k_list[OBJ_THRAIN].level > lev + 40) return (0);
	if ((k_list[OBJ_THRAIN].level > lev) && (randint(60) > 1)) return (0);
	invcopy(i_ptr, OBJ_THRAIN);
	THRAIN = TRUE;
	return (ART_THRAIN);

      case 4:
	if (randint(6) > 1) return (0);
	if (CARLAMMAS) return (0);
	if (k_list[OBJ_CARLAMMAS].level > lev + 40) return (0);
	if ((k_list[OBJ_CARLAMMAS].level > lev) && (randint(35) > 1)) return (0);
	invcopy(i_ptr, OBJ_CARLAMMAS);
	CARLAMMAS = TRUE;
	return (ART_CARLAMMAS);

      case 5:
	if (randint(10) > 1) return (0);
	if (INGWE) return (0);
	if (k_list[OBJ_INGWE].level > lev + 40) return (0);
	if ((k_list[OBJ_INGWE].level > lev) && (randint(50) > 1)) return (0);
	invcopy(i_ptr, OBJ_INGWE);
	INGWE = TRUE;
	return (ART_INGWE);

      case 6:
	if (randint(25) > 1) return (0);
	if (NECKLACE) return (0);
	if (k_list[OBJ_DWARVES].level > lev + 40) return (0);
	if ((k_list[OBJ_DWARVES].level > lev) && (randint(60) > 1)) return (0);
	invcopy(i_ptr, OBJ_DWARVES);
	NECKLACE = TRUE;
	return (ART_DWARVES);

      case 7:
	if (randint(20) > 1) return (0);
	if (BARAHIR) return (0);
	if (k_list[OBJ_BARAHIR].level > lev + 40) return (0);
	if ((k_list[OBJ_BARAHIR].level > lev) && (randint(50) > 1)) return (0);
	invcopy(i_ptr, OBJ_BARAHIR);
	BARAHIR = TRUE;
	return (ART_BARAHIR);

      case 8:
	if (randint(25) > 1) return (0);
	if (TULKAS) return (0);
	if (k_list[OBJ_TULKAS].level > lev + 40) return (0);
	if ((k_list[OBJ_TULKAS].level > lev) && (randint(65) > 1)) return (0);
	invcopy(i_ptr, OBJ_TULKAS);
	TULKAS = TRUE;
	return (ART_TULKAS);

      case 9:
	if (randint(30) > 1) return (0);
	if (NARYA) return (0);
	if (k_list[OBJ_NARYA].level > lev + 40) return (0);
	if ((k_list[OBJ_NARYA].level > lev) && (randint(50) > 1)) return (0);
	invcopy(i_ptr, OBJ_NARYA);
	NARYA = TRUE;
	return (ART_NARYA);

      case 10:
	if (randint(35) > 1) return (0);
	if (NENYA) return (0);
	if (k_list[OBJ_NENYA].level > lev + 40) return (0);
	if ((k_list[OBJ_NENYA].level > lev) && (randint(60) > 1)) return (0);
	invcopy(i_ptr, OBJ_NENYA);
	NENYA = TRUE;
	return (ART_NENYA);

      case 11:
	if (randint(40) > 1) return (0);
	if (VILYA) return (0);
	if (k_list[OBJ_VILYA].level > lev + 40) return (0);
	if ((k_list[OBJ_VILYA].level > lev) && (randint(70) > 1)) return (0);
	invcopy(i_ptr, OBJ_VILYA);
	VILYA = TRUE;
	return (ART_VILYA);

      case 12:
	if (randint(60) > 1) return (0);
	if (POWER) return (0);
	if (k_list[OBJ_POWER].level > lev + 40) return (0);
	if ((k_list[OBJ_POWER].level > lev) && (randint(100) > 1)) return (0);
	invcopy(i_ptr, OBJ_POWER);
	POWER = TRUE;
	return (ART_POWER);
    }

    return (0);
}



/*
 * Attempt to create one of the "Special Objects"
 *
 * XXX Mega-Hack -- completely ignore the properties
 * of the given "object" -- just dump over them.
 */
static bool make_artifact_special(inven_type *i_ptr)
{
    int			what;
    int			done;


    /* Try to allocate a special object */
    for (what = 0, done = 0; !what; ++done) {

	/* Abort after a while */
	if (done > 21) return FALSE;

	/* Pick a special object */
	what = make_artifact_special_aux(i_ptr);
    }

    /* Save the Artifact "Name" */  
    i_ptr->name1 = what;

    /* Set the good item flag */
    good_item_flag = TRUE;

    /* Hack -- Describe */
    if (wizard || peek) {
	char buf[256];
	objdes_store(buf, i_ptr, TRUE);
	msg_print(buf);
    }

    /* Success */
    return (TRUE);    
}


/*
 * Attempt to change an object into an artifact
 */
bool make_artifact(inven_type *i_ptr)
{
    return unique_artifact(i_ptr);
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



/*
 * Imbue an object with "magical" properties.
 *
 * The base "chance" of being "good" increases with the "level" parameter,
 * which is usually derived from the dungeon level.  Note that the "good"
 * and "great" flags over-ride this parameter somewhat.
 *			
 * If "okay" is true, this routine will have a small chance of turning
 * the object into an artifact.
 *
 * Note that the "k_list" has been rebuilt to remove the old problems
 * with multiple "similar" objects.
 */
void apply_magic(inven_type *i_ptr, int level, bool okay, bool good, bool great)
{
    register u32b      chance, special, cursed, i;
    u32b               tmp;

    /* Extract the "chance" of "goodness" */
    chance = OBJ_BASE_MAGIC + level;
    if (chance > OBJ_BASE_MAX) chance = OBJ_BASE_MAX;

    special = chance / OBJ_DIV_SPECIAL;

    /* Extract the "chance" of ickiness (approx range 11-54 percent) */
    cursed = (10 * chance) / OBJ_DIV_CURSED;


    /* Apply magic (good or bad) according to type */
    switch (i_ptr->tval) {


      case TV_DRAG_ARMOR:

	/* all DSM are enchanted, I guess -CFT */

	    /* Enchant */
	    i_ptr->toac += randint(3) + m_bonus(0, 5, level);

	    /* Perhaps an artifact */
	    if (great || (magik(chance) && magik(special))) {

		/* Even better */
		i_ptr->toac += randint(5);

		if ((great || randint(3) == 1) && okay
		    && make_artifact(i_ptr))	/* ...but is it an artifact? */
		    break;
	    }

	    /* Hack -- adjust cost for "toac" */
	    i_ptr->cost += i_ptr->toac * 500L;

	rating += 30;
	if (wizard || peek) msg_print("Dragon Scale Mail");

	break;


      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:
      case TV_SHIELD:

	/* Good */
	if (good || magik(chance)) {

	    /* Enchant */
	    i_ptr->toac += randint(3) + m_bonus(0, 5, level);

	    /* Try for artifacts */
	    if (great || magik(special)) {

		if ((i_ptr->tval == TV_SOFT_ARMOR) &&
		    (i_ptr->sval == SV_ROBE) &&
		    (magik(special)) &&
		    (randint(30) == 1)) {

		    i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD | 
				      TR2_RES_ACID | TR2_RES_FIRE |
				      TR2_HOLD_LIFE |
				      TR2_SUST_STR | TR2_SUST_DEX |
				      TR2_SUST_CON | TR2_SUST_INT |
				      TR2_SUST_WIS | TR2_SUST_CHR);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE | TR3_IGNORE_COLD |
				      TR3_IGNORE_ELEC | TR3_IGNORE_ACID);
		    i_ptr->ident |= ID_NOSHOW_P1;
		    i_ptr->pval = 10;
		    i_ptr->toac += 10 + randint(5);
		    i_ptr->cost = 10000L + (i_ptr->toac * 100);
		    give_1_hi_resist(i_ptr);
		    i_ptr->name2 = EGO_MAGI;

		    rating += 30;
		    if (wizard || peek) msg_print("Robe of the Magi");

		    break;
		}

		/* Make it "Excellent" */
		switch (randint(9)) {

		  case 1:
		    if ((great || randint(3) == 1) && okay &&
			make_artifact(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD |
				      TR2_RES_ACID | TR2_RES_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_ELEC | TR3_IGNORE_COLD |
				      TR3_IGNORE_ACID | TR3_IGNORE_FIRE);
		    if (randint(3) == 1) {
			i_ptr->flags1 |= TR1_STEALTH;
			i_ptr->pval = randint(3);
			i_ptr->toac += 15;
			i_ptr->cost += 15000L;
			give_1_hi_resist(i_ptr);
			i_ptr->name2 = EGO_ELVENKIND;
			rating += 25;
			if (peek) msg_print("Elvenkind");
		    }
		    else {
			i_ptr->toac += 8;
			i_ptr->cost += 12500L;
			i_ptr->name2 = EGO_R;
			rating += 20;
			if (peek) msg_print("Resist");
		    }
		    break;

		  case 2:
		    if ((great || randint(3) == 1) && okay &&
			make_artifact(i_ptr))
			break;
		    if (!strncmp(k_list[i_ptr->k_idx].name,
				 "Mithril", 7) ||
			!strncmp(k_list[i_ptr->k_idx].name,
				 "Adamantite", 10))
			break;
		    i_ptr->flags2 |= (TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
		    i_ptr->cost += 1000L;
		    i_ptr->name2 = EGO_RESIST_A;
		    rating += 15;
		    if (peek) msg_print("Resist Acid");
		    break;

		  case 3: case 4:
		    if ((great || randint(3) == 1) && okay &&
			make_artifact(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE);
		    i_ptr->cost += 600L;
		    i_ptr->name2 = EGO_RESIST_F;
		    rating += 17;
		    if (peek) msg_print("Resist Fire");
		    break;

		  case 5: case 6:
		    if ((great || randint(3) == 1) && okay &&
			make_artifact(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_COLD);
		    i_ptr->flags3 |= (TR3_IGNORE_COLD);
		    i_ptr->cost += 600L;
		    i_ptr->name2 = EGO_RESIST_C;
		    rating += 16;
		    if (peek) msg_print("Resist Cold");
		    break;

		  case 7: case 8: case 9:
		    if ((great || randint(3) == 1) && okay &&
			make_artifact(i_ptr))
			break;
		    i_ptr->flags2 |= (TR2_RES_ELEC);
		    i_ptr->flags3 |= (TR3_IGNORE_ELEC);
		    i_ptr->cost += 500L;
		    i_ptr->name2 = EGO_RESIST_E;
		    rating += 15;
		    if (peek) msg_print("Resist Lightning");
		    break;
		}
	    }
	}

	/* Cursed armor */
	else if (magik(cursed)) {
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac = -randint(3) - m_bonus(0, 10, level);
	    i_ptr->cost = 0L;
	}

	break;


      case TV_GLOVES:

	/* Good */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);

		/* Roll for artifact */
	    if ((((randint(2) == 1) && magik(5 * special / 2)) || great) &&
		!stricmp(k_list[i_ptr->k_idx].name,
			 "& Set of Leather Gloves") &&
		okay && make_artifact(i_ptr)) break;
	    if ((((randint(4) == 1) && magik(special)) || great)
		     && !stricmp(k_list[i_ptr->k_idx].name,
				 "& Set of Gauntlets") &&
		     okay && make_artifact(i_ptr)) break;
	    if ((((randint(5) == 1) && magik(special)) || great)
		     && !stricmp(k_list[i_ptr->k_idx].name,
				 "& Set of Cesti") &&
		     okay && make_artifact(i_ptr)) break;

	    /* Apply more magic */
	    if (great || magik(special)) {
		
		/* Make it excellent */
		switch (randint(10)) {

		  case 1: case 2: case 3:
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->cost += 1000L;
		    i_ptr->name2 = EGO_FREE_ACTION;
		    rating += 11;
		    if (peek) msg_print("Free action");
		    break;

		  case 4: case 5: case 6:
		    i_ptr->ident |= ID_SHOW_HITDAM;
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 250;
		    i_ptr->name2 = EGO_SLAYING;
		    rating += 17;
		    if (peek) msg_print("Slaying");
		    break;

		  case 7: case 8: case 9:
		    i_ptr->flags1 |= (TR1_DEX);
		    i_ptr->pval = 2 + randint(2);	/* +N DEX */
		    i_ptr->cost += (i_ptr->pval) * 400;
		    i_ptr->name2 = EGO_AGILITY;
		    rating += 14;
		    if (peek) msg_print("Agility");
		    break;

		  case 10:
		    if (((great || randint(3) == 1)) && okay &&
			make_artifact(i_ptr))
			break;
		    i_ptr->flags1 |= (TR1_STR);
		    i_ptr->ident |= ID_SHOW_HITDAM;
		    i_ptr->ident |= ID_NOSHOW_TYPE;
		    i_ptr->pval = 1 + randint(4);	/* +N STR */
		    i_ptr->tohit += 1 + randint(4);
		    i_ptr->todam += 1 + randint(4);
		    i_ptr->cost += (i_ptr->tohit + i_ptr->todam +
				      i_ptr->pval) * 300;
		    i_ptr->name2 = EGO_POWER;
		    rating += 22;
		    if (peek) msg_print("Power");
		    break;
		}
	    }
	}

	/* Cursed gloves */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac = 0 - (m_bonus(1, 20, level));

		/* Permanently damaged */
	    i_ptr->cost = 0L;

	    /* Permanently damaged */
	    if (magik(special)) {

		/* Strength or Dexterity */
		if (randint(2) == 1) {
		    i_ptr->flags1 |= TR1_DEX;
		    i_ptr->pval = (randint(3) - m_bonus(0, 10, level));
		    i_ptr->name2 = EGO_CLUMSINESS;
		}
		else {
		    i_ptr->flags1 |= TR1_STR;
		    i_ptr->pval = (randint(3) - m_bonus(0, 10, level));
		    i_ptr->name2 = EGO_WEAKNESS;
		}
	    }
	}

	break;


      case TV_BOOTS:

	/* Good */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(1, 10, level);

	    /* Apply more magic */
	    if (great || magik(special)) {

		tmp = randint(12);

		  if (tmp == 1) {
		    if (!((randint(2) == 1) && okay
			  && make_artifact(i_ptr))) {
		    i_ptr->flags1 |= TR1_SPEED;
		    i_ptr->name2 = EGO_SPEED;
		    i_ptr->pval = 1;
		    i_ptr->cost += 300000L;
		    
		    /* Increase the rating */
		    rating += 30;
		    if (wizard || peek) msg_print("Boots of Speed");
		    }
		} else if (stricmp("& Pair of Metal Shod Boots",
				   k_list[i_ptr->k_idx].name))	/* not metal */

		  if (tmp > 6) {
		    i_ptr->flags3 |= TR3_FEATHER;
		    i_ptr->cost += 250;
		    i_ptr->name2 = EGO_SLOW_DESCENT;
		    rating += 7;
		  } else if (tmp < 5) {
		    i_ptr->flags1 |= TR1_STEALTH;
		    i_ptr->pval = randint(3);	/* +N Stealth */
		    i_ptr->cost += 500;
		    i_ptr->name2 = EGO_STEALTH;
		    rating += 16;
		  } else {	   /* 5,6 */
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->cost += 500;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_FREE_ACTION;
		    rating += 15;
		    }
		else
		 /* is metal boots, different odds since no stealth */
		  if (tmp < 5) {
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->cost += 500;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_FREE_ACTION;
		    rating += 15;
		  } else {	   /* tmp > 4 */
		    i_ptr->flags3 |= TR3_FEATHER;
		    i_ptr->cost += 250;
		    i_ptr->name2 = EGO_SLOW_DESCENT;
		    rating += 7;
		    }
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac = 0 - m_bonus(2, 20, level);

		/* Permanent damage */
		i_ptr->cost = 0L;

		/* Pick some damage */
		switch (randint(3)) {
		    case 1:
			i_ptr->flags1 |= TR1_SPEED;
			i_ptr->pval = -1;
			i_ptr->name2 = EGO_SLOWNESS;
			break;
		    case 2:
			i_ptr->flags3 |= TR3_AGGRAVATE;
			i_ptr->name2 = EGO_NOISE;
			break;
		    case 3:
			i_ptr->weight = i_ptr->weight * 5;
			i_ptr->name2 = EGO_GREAT_MASS;
			break;
	    }
	}

	break;
	

      case TV_HELM:

	/* Hack -- crowns are "more magical" */
	if ((i_ptr->sval == SV_IRON_CROWN) ||
	    (i_ptr->sval == SV_GOLDEN_CROWN) ||
	    (i_ptr->sval == SV_JEWELED_CROWN)) {

	    /* Hack -- extra "goodness" based on cost */
	    chance += i_ptr->cost / 100L;

	    /* Hack -- improve the chance for "greatness" */
	    special += special;
	}

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac = randint(3) + m_bonus(0, 10, level);

	    /* Apply more magic */
	    if (great || magik(special)) {

		/* Process "helms" */
		if (i_ptr->sval < 6) {

		    /* Make it "excellent" */
		    switch (randint(14)) {

		      case 1: case 2:
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags1 |= TR1_INT;
			i_ptr->pval = randint(2);	/* +N INT */
			i_ptr->cost += i_ptr->pval * 500;
			i_ptr->name2 = EGO_INTELLIGENCE;
			rating += 13;
			if (peek) msg_print("Intelligence");
			}
			break;

		      case 3: case 4: case 5:
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags1 |= TR1_WIS;
			i_ptr->pval = randint(2);	/* +N Wis */
			i_ptr->cost += i_ptr->pval * 500;
			i_ptr->name2 = EGO_WISDOM;
			rating += 13;
			if (peek) msg_print("Wisdom");
			}
			break;

		      case 6: case 7: case 8: case 9:
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags1 |= TR1_INFRA;
			i_ptr->pval = 1 + randint(4);	/* +N Infra */
			i_ptr->cost += i_ptr->pval * 250;
			i_ptr->name2 = EGO_INFRAVISION;
			rating += 11;
			}
			break;

		      case 10: case 11:
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags2 |= (TR2_RES_LITE);
			i_ptr->flags3 |= (TR3_LITE);
			i_ptr->cost += 500;
			i_ptr->name2 = EGO_LITE;
			rating += 6;
			if (peek) msg_print("Light");
			}
			break;

		      case 12: case 13:
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags2 |= TR2_RES_BLIND;
			i_ptr->flags3 |= TR3_SEE_INVIS;
			i_ptr->cost += 1000;
			i_ptr->name2 = EGO_SEEING;
			rating += 8;
			if (peek) msg_print("Helm of Seeing");
			}
			break;

		     default: /* case 14: */
			if (!((randint(2) == 1) && okay &&
			      make_artifact(i_ptr))) {
			i_ptr->flags3 |= TR3_TELEPATHY;
			i_ptr->cost += 50000L;
			i_ptr->name2 = EGO_TELEPATHY;
			rating += 20;
			if (peek) msg_print("Telepathy");
			}
			break;
		    }
		}

		/* Process "crowns" */
		else {

		    /* Make it "excellent" */
		    switch (randint(6)) {

		      case 1:
			if (!((great || (randint(2) == 1)) &&
			      okay && make_artifact(i_ptr))) {
			i_ptr->flags1 |= (TR1_STR | TR1_DEX | TR1_CON);
			i_ptr->flags2 |= (TR2_FREE_ACT);
			i_ptr->pval = randint(3);	/* +N STR/DEX/CON */
			i_ptr->cost += 1000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_MIGHT;
			rating += 19;
			if (peek) msg_print("Crown of Might");
			}
			break;

		      case 2:
			i_ptr->flags1 |= (TR1_CHR | TR1_WIS);
			i_ptr->pval = randint(3);	/* +N WIS/CHR */
			i_ptr->cost += 1000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_LORDLINESS;
			rating += 17;
			if (peek) msg_print("Lordliness");
			break;

		      case 3:
			i_ptr->flags1 |= (TR1_INT);
			i_ptr->flags2 |= (TR2_RES_ELEC | TR2_RES_COLD |
					  TR2_RES_ACID | TR2_RES_FIRE);
			i_ptr->flags3 |= (TR3_IGNORE_ELEC | TR3_IGNORE_COLD |
					  TR3_IGNORE_ACID | TR3_IGNORE_FIRE);
			i_ptr->pval = randint(3);	/* +N INT */
			i_ptr->cost += 3000 + i_ptr->pval * 500;
			i_ptr->name2 = EGO_MAGI;
			rating += 15;
			if (peek) msg_print("Crown of the Magi");
			break;

		      case 4:
			i_ptr->flags1 |= TR1_CHR;
			i_ptr->pval = randint(4);	/* +N CHR */
			i_ptr->cost += 750;
			i_ptr->name2 = EGO_BEAUTY;
			rating += 8;
			if (peek) msg_print("Beauty");
			break;

		      case 5:
			i_ptr->flags1 |= (TR1_SEARCH);
			i_ptr->flags3 |= (TR3_SEE_INVIS);
			i_ptr->pval = 5 * (1 + randint(4));	/* +N Search */
			i_ptr->cost += 1000 + i_ptr->pval * 100;
			i_ptr->name2 = EGO_SEEING;
			rating += 8;
			if (peek) msg_print("Seeing");
			break;

		      case 6:
			i_ptr->flags3 |= TR3_REGEN;
			i_ptr->cost += 1500;
			i_ptr->name2 = EGO_REGENERATION;
			rating += 10;
			if (peek) msg_print("Regeneration");
			break;
		    }
		}
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->toac -= m_bonus(1, 20, level);
	    i_ptr->cost = 0L;

	    /* Permanent damage */
	    if (magik(special)) {

		/* Choose some damage */
		switch (randint(7)) {
		  case 1:
		    i_ptr->flags1 |= TR1_INT;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_STUPIDITY;
		    break;
		  case 2:
		  case 3:
		    i_ptr->flags1 |= TR1_WIS;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_DULLNESS;
		    break;
		  case 4:
		  case 5:
		    i_ptr->flags1 |= TR1_STR;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_WEAKNESS;
		    break;
		  case 6:
		    i_ptr->flags3 |= TR3_TELEPORT;
		    i_ptr->name2 = EGO_TELEPORTATION;
		    break;
		  case 7:
		    i_ptr->flags1 |= TR1_CHR;
		    i_ptr->pval = -randint(5);
		    i_ptr->name2 = EGO_UGLINESS;
		    break;
		}
	    }
	}
	break;


      case TV_CLOAK:

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->toac += 1 + m_bonus(0, 20, level);

	    /* Apply more magic */
	    if (great || magik(special)) {

		/* Roll for artifact */
		if (okay && ((randint(10) == 1)||(randint(20) == 1)) && make_artifact(i_ptr)) return;

		/* Make it "excellent" */
		if (randint(2) == 1) {
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
		    i_ptr->toac += m_bonus(0, 10, level) + (5 + randint(3));
		    i_ptr->cost += 250L;
		    i_ptr->name2 = EGO_PROTECTION;
		    rating += 8;
		}
		else if (randint(10) == 1) {
		    i_ptr->toac += 10 + randint(10);
		    i_ptr->pval = randint(3);
		    i_ptr->flags1 |= (TR1_STEALTH);
		    i_ptr->flags2 |= (TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_IGNORE_ACID);
		    i_ptr->name2 = EGO_AMAN;
		    i_ptr->cost += 4000 + (100 * i_ptr->toac);
		    rating += 16;
		}
		else {
		    i_ptr->toac += m_bonus(3, 10, level);
		    i_ptr->pval = randint(3);
		    i_ptr->flags1 |= TR1_STEALTH;
		    i_ptr->name2 = EGO_STEALTH;
		    i_ptr->cost += 500 + (50 * i_ptr->pval);
		    rating += 9;
		}
	    }
	}

	/* Cursed */
	else if (magik(cursed)) {

	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;

		/* Permanent damage */
		i_ptr->cost = 0L;

		/* Choose some damage */
		switch (randint(3)) {
		    case 1:
			i_ptr->name2 = EGO_IRRITATION;
			i_ptr->ident |= ID_SHOW_HITDAM;
			i_ptr->flags3 |= TR3_AGGRAVATE;
			i_ptr->toac -= m_bonus(1, 10, level);
			i_ptr->tohit -= m_bonus(1, 10, level);
			i_ptr->todam -= m_bonus(1, 10, level);
			break;
		    case 2:
			i_ptr->name2 = EGO_VULNERABILITY;
			i_ptr->toac -= m_bonus(10, 20, level + 50);
			break;
		    case 3:
			i_ptr->name2 = EGO_ENVELOPING;
			i_ptr->ident |= ID_SHOW_HITDAM;
			i_ptr->toac -= m_bonus(1, 10, level);
			i_ptr->tohit -= m_bonus(2, 15, level + 10);
			i_ptr->todam -= m_bonus(2, 15, level + 10);
			break;
	    }
	}
	break;


      case TV_DIGGING:

    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;

	/* Apply some magic */
	if (great || magik(chance)) {

	    tmp = randint(3);
	    if (tmp == 1) {
		i_ptr->pval += m_bonus(0, 5, level);
	    }
	    if (tmp == 2)	/* do not give additional plusses -CWS */
		;

	    else {
	    /* a cursed digging tool */
	    i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = (-m_bonus(1, 15, level));
		i_ptr->cost = 0L;
	    }
	}
	
	break;


      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:

    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->tohit += randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam += randint(3) + m_bonus(0, 10, level);
	/*
	 * the 3*special/2 is needed because weapons are not as common as
	 * before change to treasure distribution, this helps keep same
	 * number of ego weapons same as before, see also missiles 
	 */

	    /* Hack -- improve the "special" chance */
	    special = special * 3 / 2;
	    
	    /* Make it "excellent" */
	    if (great || magik(special)) {

		/* Hack -- Roll for whips of fire */
		if ((i_ptr->tval == TV_HAFTED) &&
		    (i_ptr->sval == SV_WHIP) &&
		    (randint(2) == 1)) {

		    i_ptr->flags1 |= (TR1_BRAND_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE);

		    /* Better stats */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;

		    /* this should allow some WICKED whips -CFT */
		    while (randint(5 * (int)i_ptr->damage[0]) == 1) {
			i_ptr->damage[0]++;
			i_ptr->cost += 2500;
			i_ptr->cost *= 2;
		    }

		    i_ptr->name2 = EGO_FIRE;

		    rating += 20;
		    if (peek) msg_print("Whip of Fire");

		    break;
		}

		/* Make it "excellent" */
		switch (randint(30)) {

		  case 1:
			if (((randint(2) == 1) || (great))
			    && okay &&
			    make_artifact(i_ptr))
			    break;
		    i_ptr->flags1 |= (TR1_SLAY_DEMON | TR1_WIS |
				      TR1_SLAY_UNDEAD | TR1_SLAY_EVIL);
		    i_ptr->flags3 |= (TR3_BLESSED | TR3_SEE_INVIS);
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;
		    i_ptr->toac += randint(4);

		    /* Obsolete Hack -- Pick "Sustain" based on "Pval" */
		    switch (i_ptr->pval) {
			case 1: i_ptr->flags2 |= (TR2_SUST_STR); break;
			case 2: i_ptr->flags2 |= (TR2_SUST_INT); break;
			case 3: i_ptr->flags2 |= (TR2_SUST_WIS); break;
			case 4: i_ptr->flags2 |= (TR2_SUST_DEX); break;
		    }

		    i_ptr->cost += i_ptr->pval * 500;
		    i_ptr->cost += 10000L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_HA;
		    rating += 30;
		    if (peek) msg_print("Holy Avenger");
		    break;

		  case 2:
			if (((randint(2) == 1) || (great)) && okay &&
			    make_artifact(i_ptr))
			    break;
		    i_ptr->flags1 |= (TR1_STEALTH);
		    i_ptr->flags2 |= (TR2_FREE_ACT |
				      TR2_RES_FIRE | TR2_RES_COLD |
				      TR2_RES_ELEC | TR2_RES_ACID);
		    i_ptr->flags3 |= (TR3_FEATHER | TR3_REGEN | TR3_SEE_INVIS |
				      TR3_IGNORE_FIRE | TR3_IGNORE_COLD |
				      TR3_IGNORE_ELEC | TR3_IGNORE_ACID);
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->toac += 5 + randint(5);
		    i_ptr->pval = randint(3);	/* +X Stealth */
		    i_ptr->cost += i_ptr->pval * 500;
		    i_ptr->cost += 7500L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_DF;
		    rating += 23;
		    if (peek) msg_print("Defender");
		    break;

		  case 3: case 4:
		    if (((randint(2) == 1) || (great)) && okay &&
			    make_artifact(i_ptr))
			    break;
		    i_ptr->flags1 |= (TR1_BRAND_FIRE);
		    i_ptr->flags2 |= (TR2_RES_FIRE);
		    i_ptr->flags2 |= (TR3_IGNORE_FIRE);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 3;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_FT;
		    rating += 20;
		    if (peek) msg_print("Flame");
		    break;

		  case 5: case 6:
			if (((randint(2) == 1) || (great)) && okay &&
			    make_artifact(i_ptr))
			    break;
		    i_ptr->flags1 |= (TR1_BRAND_COLD);
		    i_ptr->flags2 |= (TR2_RES_COLD);
		    i_ptr->flags3 |= (TR3_IGNORE_COLD);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 2200L;
		    i_ptr->name2 = EGO_FB;
		    rating += 20;
		    if (peek) msg_print("Frost");
		    break;

		  case 7: case 8:
		    i_ptr->flags1 |= TR1_SLAY_ANIMAL;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 2000L;
		    i_ptr->name2 = EGO_SLAY_A;
		    rating += 15;
		    if (peek) msg_print("Slay Animal");
		    break;

		  case 9: case 10:
		    i_ptr->flags1 |= TR1_SLAY_DRAGON;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 4000L;
		    i_ptr->name2 = EGO_SLAY_D;
		    rating += 18;
		    if (peek) msg_print("Slay Dragon");
		    break;

		  case 11: case 12:
		    i_ptr->flags1 |= TR1_SLAY_EVIL;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->cost += 4000L;
		    i_ptr->name2 = EGO_SLAY_E;

		    /* One in three is also a blessed wisdom booster */
		    if (randint(3) == 1) {
			i_ptr->flags1 |= (TR1_WIS);
			i_ptr->flags3 |= (TR3_BLESSED);
			i_ptr->pval = m_bonus(0, 3, level);
			i_ptr->cost += (200L * i_ptr->pval);
		    }

		    rating += 18;
		    if (peek) msg_print("Slay Evil");

		    break;

		  case 13: case 14:
		    i_ptr->flags1 |= (TR1_SLAY_UNDEAD);
		    i_ptr->flags3 |= (TR3_SEE_INVIS);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_SLAY_U;

		    /* One in three is also a Life Holder */
		    if (randint(3) == 1) {
			i_ptr->flags2 |= (TR2_HOLD_LIFE);
			i_ptr->cost += 1000L;
		    }

		    rating += 18;
		    if (peek) msg_print("Slay Undead");

		    break;

		  case 15: case 16: case 17:
		    i_ptr->flags1 |= TR1_SLAY_ORC;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_O;
		    rating += 13;
		    if (peek) msg_print("Slay Orc");
		    break;

		  case 18: case 19: case 20:
		    i_ptr->flags1 |= TR1_SLAY_TROLL;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_T;
		    rating += 13;
		    if (peek) msg_print("Slay Troll");
		    break;

		  case 21: case 22: case 23:
		    i_ptr->flags1 |= TR1_SLAY_GIANT;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_G;
		    rating += 14;
		    if (peek) msg_print("Slay Giant");
		    break;

		  case 24: case 25: case 26:
		    i_ptr->flags1 |= TR1_SLAY_DEMON;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->cost += 1200L;
		    i_ptr->name2 = EGO_SLAY_DEMON;
		    rating += 16;
		    if (peek) msg_print("Slay Demon");
		    break;

		  case 27:
			if (((randint(2) == 1) || (great)) && okay &&
			    make_artifact(i_ptr))
			    break;
		    i_ptr->flags1 |= (TR1_SLAY_ORC |
				      TR1_DEX | TR1_CON | TR1_STR);
		    i_ptr->flags2 |= (TR2_FREE_ACT);
		    i_ptr->flags3 |= (TR3_SEE_INVIS);
		    i_ptr->tohit += 3 + randint(5);
		    i_ptr->todam += 3 + randint(5);
		    i_ptr->pval = 1;
		    i_ptr->cost += 10000L;
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_WEST;
		    rating += 20;
		    if (peek) msg_print("Westernesse");
		    break;

		  /* Anything can be blessed */
		  case 28: case 29:
			if ((i_ptr->tval != TV_SWORD) &&
			    (i_ptr->tval != TV_POLEARM))
			    break;
		    i_ptr->flags3 |= TR3_BLESSED;
		    i_ptr->flags1 |= TR1_WIS;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->pval = randint(3);
		    i_ptr->cost += i_ptr->pval * 1000;
		    i_ptr->cost += 3000L;
		    i_ptr->name2 = EGO_BLESS_BLADE;
		    rating += 20;
		    if (peek) msg_print("Blessed");
		    break;

		  /* Extra Attacks */
		  case 30:
			if (((randint(2) == 1) || (great))
			    && okay && make_artifact(i_ptr))
			    break;
		    i_ptr->tohit += randint(5);
		    i_ptr->todam += randint(3);
		    i_ptr->flags1 |= (TR1_ATTACK_SPD);
		    if (i_ptr->weight <= 80) {
			i_ptr->pval = randint(3);
		    }
		    else if (i_ptr->weight <= 130) {
			i_ptr->pval = randint(2);
		    }
		    else {
			i_ptr->pval = 1;
		    }
		    i_ptr->cost += (i_ptr->pval * 2000);
		    i_ptr->cost *= 2;
		    i_ptr->name2 = EGO_ATTACKS;
		    rating += 20;
		    if (wizard || peek) msg_print("Weapon of Extra Attacks");
		    break;
		}
	    }
	}

	/* Cursed Weapons */
	else if (magik(cursed)) {
	
	    /* Cursed */
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->tohit = 0 - randint(3) - m_bonus(1, 20, level);
	    i_ptr->todam = 0 - randint(3) - m_bonus(1, 20, level);

	    /* Permanently cursed Weapon of Morgul */
	    if (level > (20 + randint(15)) && randint(10) == 1) {
		i_ptr->flags3 |= (TR3_AGGRAVATE | TR3_SEE_INVIS);
		i_ptr->tohit -= 15;
		i_ptr->todam -= 15;
		i_ptr->toac = -10;
		i_ptr->weight += 100;
		i_ptr->name2 = EGO_MORGUL;
	    }
	    i_ptr->cost = 0L;
	}
	break;


      case TV_BOW:

    /* always show tohit/todam values if identified */
	i_ptr->ident |= ID_SHOW_HITDAM;

	/* Apply some magic */
	if (good || magik(chance)) {

	    /* Make it better */
	    i_ptr->tohit = randint(3) + m_bonus(0, 10, level);
	    i_ptr->todam = randint(3) + m_bonus(0, 10, level);

	    switch (randint(15)) {

		  case 1: case 2: case 3:
		if (((randint(3)==1)||(great)) && okay && make_artifact(i_ptr)) break;
		if (((randint(5) == 1) || (great)) && okay && make_artifact(i_ptr)) break;

		    i_ptr->sval++; /* make it do an extra multiple of damage */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 10;
		    i_ptr->name2 = EGO_MIGHT;
		    rating += 15;
		    if (peek) msg_print("Bow of Might");
		    break;

		  case 4: case 5: case 6: case 7: case 8:
		    i_ptr->tohit += 5;
		    i_ptr->todam += 12;
		    i_ptr->name2 = EGO_MIGHT;
		    rating += 11;
		    if (peek) msg_print("Bow of Might");
		    break;

		  case 9: case 10: case 11: case 12:
		  case 13: case 14: case 15:
		    i_ptr->tohit += 12;
		    i_ptr->todam += 5;
		    i_ptr->name2 = EGO_ACCURACY;
		    rating += 11;
		    if (peek) msg_print("Accuracy");
		    break;
	    }
	}

	else if (magik(cursed)) {
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->tohit = 0 - m_bonus(5, 30, level);
	    i_ptr->todam = 0 - m_bonus(5, 20, level);
	    i_ptr->cost = 0L;
	}
	
	break;


      case TV_BOLT:
      case TV_ARROW:
      case TV_SHOT:

	/* this fn makes ammo for player's missile weapon more common -CFT */
{
    inven_type *w_ptr = NULL;

    /* if wielding a bow as main/aux weapon, then ammo will be "right" ammo
     * more often than not of the time -CFT */
    if (inventory[INVEN_WIELD].tval == TV_BOW) w_ptr=&inventory[INVEN_WIELD];
    else if (inventory[INVEN_AUX].tval == TV_BOW) w_ptr=&inventory[INVEN_AUX];

    if (w_ptr && (randint(2)==1)){
	if ((i_ptr->tval == TV_SHOT) &&
	    (w_ptr->sval >= 20) && (w_ptr->sval <= 21));
	/* right type, do nothing */
	else if ((i_ptr->tval == TV_ARROW) &&
		 (w_ptr->sval >= 1) && (w_ptr->sval <= 4));
	/* right type, do nothing */
	else if ((i_ptr->tval == TV_BOLT) &&
		 (w_ptr->sval >= 10) && (w_ptr->sval <= 12));
	/* right type, do nothing */
	else if ((w_ptr->sval >= 20) && (w_ptr->sval <= 21))
	    invcopy(i_ptr, OBJ_SHOT);
	else if ((w_ptr->sval >= 1) && (w_ptr->sval <= 4))
	    invcopy(i_ptr, OBJ_ARROW);
	else			/* xbow */
	    invcopy(i_ptr, OBJ_BOLT);
    }

    i_ptr->number = 0;
    for (i = 0; i < 7; i++)
	i_ptr->number += randint(6);
    if (missile_ctr == MAX_SHORT)
	missile_ctr = -MAX_SHORT - 1;
    else
	missile_ctr++;
    i_ptr->pval = missile_ctr;

    /* always show tohit/todam values if identified */
    i_ptr->ident |= ID_SHOW_HITDAM;

	if (magik(chance)||good) {

	    i_ptr->tohit = randint(5) + m_bonus(1, 15, level);
	    i_ptr->todam = randint(5) + m_bonus(1, 15, level);

	    /* see comment for weapons */
	    if (magik(5*special/2)||(great))

		switch (randint(11)) {

		  case 1: case 2: case 3:
		    i_ptr->name2 = EGO_WOUNDING; /* swapped with slaying -CFT */
		    i_ptr->tohit += 5;
		    i_ptr->todam += 5;
		    i_ptr->damage[0] ++; /* added -CFT */
		    i_ptr->cost += 30;
		    rating += 5;
		    break;

		  case 4: case 5:
		    i_ptr->flags1 |= (TR1_BRAND_FIRE);
		    i_ptr->flags3 |= (TR3_IGNORE_FIRE);
		    i_ptr->tohit += 2;
		    i_ptr->todam += 4;
		    i_ptr->name2 = EGO_FIRE;
		    i_ptr->cost += 25;
		    rating += 6;
		    break;

		  case 6: case 7:
		    i_ptr->flags1 |= TR1_SLAY_EVIL;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->name2 = EGO_SLAY_EVIL;
		    i_ptr->cost += 25;
		    rating += 7;
		    break;

		  case 8: case 9:
		    i_ptr->flags1 |= TR1_SLAY_ANIMAL;
		    i_ptr->tohit += 2;
		    i_ptr->todam += 2;
		    i_ptr->name2 = EGO_SLAY_ANIMAL;
		    i_ptr->cost += 30;
		    rating += 5;
		    break;

		  case 10:
		    i_ptr->flags1 |= TR1_SLAY_DRAGON;
		    i_ptr->tohit += 3;
		    i_ptr->todam += 3;
		    i_ptr->name2 = EGO_DRAGON_SLAYING;
		    i_ptr->cost += 35;
		    rating += 9;
		    break;

		  case 11:
		    i_ptr->tohit += 10; /* reduced because of dice bonus -CFT */
		    i_ptr->todam += 10;
		    i_ptr->name2 = EGO_SLAYING; /* swapped w/ wounding -CFT */
		    i_ptr->damage[0] += 2; /* added -CFT */
		    i_ptr->cost += 45;
		    rating += 10;
		    break;
	    }

	    while (magik(special)) { /* added -CFT */
		i_ptr->damage[0]++;
		i_ptr->cost += i_ptr->damage[0]*5;
	    }
	}

	else if (magik(cursed)) {

	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->tohit = (-randint(10)) - m_bonus(5, 25, level);
	    i_ptr->todam = (-randint(10)) - m_bonus(5, 25, level);
	    i_ptr->cost = 0L;

	    if (randint(5)==1) {
		i_ptr->name2 = EGO_BACKBITING;
		i_ptr->tohit -= 20;
		i_ptr->todam -= 20;
	    }
	}
}

	break;


      case TV_RING:

	if (!((randint(10) == 1) && okay && make_artifact(i_ptr))) {

	switch (i_ptr->sval) {

	  /* Strength, Constitution, Dexterity, Intelligence */
	  case SV_RING_STR:
	  case SV_RING_CON:
	  case SV_RING_DEX:
	  case SV_RING_INT:
	    i_ptr->pval = m_bonus(1, 6, level);
	    if (magik(cursed)) {
		i_ptr->pval = -m_bonus(1, 10, level);
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->cost = -i_ptr->cost;
	    } else {
		i_ptr->cost += i_ptr->pval * 100;
	    }
	    break;

	  /* Ring of Speed! */
	  case SV_RING_SPEED:

	    /* Cursed Ring */
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -randint(3);
		i_ptr->cost = -i_ptr->cost;
	    } else {

	    if (randint(888) == 1) i_ptr->pval = 2;
	    else i_ptr->pval = 1;

	    /* Rating boost */
	    rating += 35;
	    if (peek) msg_print("Ring of Speed");
	    }
	    break;

	  /* Searching */
	  case SV_RING_SEARCHING:
	    i_ptr->pval = 5 * m_bonus(1, 10, level);
	    i_ptr->cost += i_ptr->pval * 30;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -i_ptr->pval;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Flames, Acid, Ice */
	  case SV_RING_FLAMES:
	  case SV_RING_ACID:
	  case SV_RING_ICE:
	    i_ptr->toac = m_bonus(1, 10, level);
	    i_ptr->toac += 5 + randint(7);
	    i_ptr->cost += i_ptr->toac * 100;
	    break;

	  /* WOE, Stupidity */
	  case SV_RING_WOE:
	  case SV_RING_STUPIDITY:
	    i_ptr->toac = (-5) - m_bonus(1,10,level);
	    i_ptr->pval = (-randint(4));
	    break;

	  /* Increase damage */
	  case 19:
	    i_ptr->todam = m_bonus(1, 10, level);
	    i_ptr->todam += 3 + randint(10);
	    i_ptr->cost += i_ptr->todam * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->todam = -i_ptr->todam;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Increase To-Hit */
	  case 20:
	    i_ptr->tohit = m_bonus(1, 10, level);
	    i_ptr->tohit += 3 + randint(10);
	    i_ptr->cost += i_ptr->tohit * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->tohit = -i_ptr->tohit;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  /* Protection */
	  case 21:
	    i_ptr->toac = m_bonus(0, 10, level);
	    i_ptr->toac += 4 + randint(5);
	    i_ptr->cost += i_ptr->toac * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->toac = -i_ptr->toac;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;

	  case 24:
	  case 25:
	  case 26:
	  case 27:
	  case 28:
	  case 29:
	    i_ptr->ident |= ID_NOSHOW_P1;
	    break;

	  /* Slaying */
	  case 30:
	    i_ptr->ident |= ID_SHOW_HITDAM;
	    i_ptr->todam = m_bonus(1, 10, level);
	    i_ptr->todam += 2 + randint(3);
	    i_ptr->tohit = m_bonus(1, 10, level);
	    i_ptr->tohit += 2 + randint(3);
	    i_ptr->cost += (i_ptr->tohit + i_ptr->todam) * 100;
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->tohit = 0 - i_ptr->tohit;
		i_ptr->todam = 0 - i_ptr->todam;
		i_ptr->cost = -i_ptr->cost;
	    }
	    break;
	}
	}
	break;

      case TV_AMULET:

	if ((i_ptr->sval == SV_AMULET_WISDOM) ||
	    (i_ptr->sval == SV_AMULET_CHARISMA)) {
	    i_ptr->pval = m_bonus(1, 5, level);
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -m_bonus(1, 5, level);
		i_ptr->cost = -i_ptr->cost;
	    } else {
		i_ptr->cost += i_ptr->pval * 100;
	    }
	}
	else if (i_ptr->sval == SV_AMULET_SEARCHING) {
	    i_ptr->pval = 5 * (randint(3) + m_bonus(0, 8, level));
	    if (magik(cursed)) {
		i_ptr->flags3 |= TR3_CURSED;
		i_ptr->pval = -i_ptr->pval;
		i_ptr->cost = -i_ptr->cost;
	    } else
		i_ptr->cost += 20 * i_ptr->pval;
	}
	else if (i_ptr->sval == SV_AMULET_THE_MAGI) {
	    rating += 25;
	    i_ptr->pval = 5 * (randint(2) + m_bonus(0, 10, level));
	    i_ptr->toac = randint(4) + m_bonus(0, 8, level) - 2;
	    i_ptr->cost += 20 * i_ptr->pval + 50 * i_ptr->toac;
	    if (i_ptr->toac < 0) /* sort-of cursed...just to be annoying -CWS */
		i_ptr->flags3 |= TR3_CURSED;
	}
	else if (i_ptr->sval == SV_AMULET_DOOM) {
	    i_ptr->flags3 |= TR3_CURSED;
	    i_ptr->pval = 0 - randint(5) - m_bonus(2, 10, level);
	    i_ptr->toac = 0 - randint(3) - m_bonus(0, 6, level);
	}
	break;


      case TV_WAND:
	charge_wand(i_ptr);
	break;


      case TV_STAFF:
	charge_staff(i_ptr);
	break;


      case TV_LITE:

	/* Torches -- random fuel */
	if (i_ptr->sval == SV_LITE_TORCH) {
	    i_ptr->pval = randint(i_ptr->pval);
	}

	/* Lanterns -- random fuel */            
	if (i_ptr->sval == SV_LITE_LANTERN) {
	    i_ptr->pval = randint(i_ptr->pval);
	}

	break;


      case TV_CHEST:

	switch (randint(level + 4)) {
	  case 1:
	    i_ptr->flags1 = 0L;
	    i_ptr->flags2 = 0L;
	    break;
	  case 2:
	    i_ptr->flags2 |= CH2_LOCKED;
	    break;
	  case 3:
	  case 4:
	    i_ptr->flags2 |= (CH2_LOSE_STR | CH2_LOCKED);
	    break;
	  case 5:
	  case 6:
	    i_ptr->flags2 |= (CH2_POISON | CH2_LOCKED);
	    break;
	  case 7:
	  case 8:
	  case 9:
	    i_ptr->flags2 |= (CH2_PARALYSED | CH2_LOCKED);
	    break;
	  case 10:
	  case 11:
	    i_ptr->flags2 |= (CH2_EXPLODE | CH2_LOCKED);
	    break;
	  case 12:
	  case 13:
	  case 14:
	    i_ptr->flags2 |= (CH2_SUMMON | CH2_LOCKED);
	    break;
	  case 15:
	  case 16:
	  case 17:
	    i_ptr->flags2 |= (CH2_PARALYSED | CH2_POISON | CH2_LOSE_STR | CH2_LOCKED);
	    break;
	  default:
	    i_ptr->flags2 |= (CH2_SUMMON | CH2_EXPLODE | CH2_LOCKED);
	    break;
	}
	if (!okay)		/* if bought from store - dbd */
	    i_ptr->pval = randint(i_ptr->level);
	else			/* store the level chest's found on - dbd */
	    i_ptr->pval = dun_level;
	break;

      case TV_SPIKE:
	i_ptr->number = 0;
	for (i = 0; i < 7; i++)
	    i_ptr->number += randint(6);
	if (missile_ctr == MAX_SHORT)
	    missile_ctr = -MAX_SHORT - 1;
	else
	    missile_ctr++;
	i_ptr->pval = missile_ctr;
	break;

      case TV_FOOD:
    /* make sure all food rations have the same level */
	if (i_ptr->sval == 90)
	    i_ptr->level = 0;
    /* give all elvish waybread the same level */
	else if (i_ptr->sval == 92)
	    i_ptr->level = 6;
	break;

      case TV_SCROLL:
    /* give all identify scrolls the same level */
	if (i_ptr->sval == 67)
	    i_ptr->level = 1;
    /* scroll of light */
	else if (i_ptr->sval == 69)
	    i_ptr->level = 0;
    /* scroll of trap detection */
	else if (i_ptr->sval == 80)
	    i_ptr->level = 5;
    /* scroll of door/stair location */
	else if (i_ptr->sval == 81)
	    i_ptr->level = 5;
	break;

      case TV_POTION:		   /* potions */
    /* cure light */
	if (i_ptr->sval == 76)
	    i_ptr->level = 0;
	break;

      default:
	break;
    }
}




/*
 * XXX Mega-Hack -- attempt to place one of the "Special Objects"
 *
 */
static int special_place_object(int y, int x)
{
    register int	cur_pos;
    cave_type		*c_ptr;
    inven_type		hack;


    /* Is this cave grid sacred? */
    if (!valid_grid(y,x)) return (FALSE);


    /* Get the cave */
    c_ptr = &cave[y][x];


    /* Hack -- clean up "hack" */
    invcopy(&hack, OBJ_NOTHING);

    /* Hack -- Try to allocate a special object */
    if (!make_artifact_special(&hack)) return (FALSE);


    /* Delete anything that is there */
    delete_object(y, x);

    /* Make the object, using the index from above */
    cur_pos = i_pop();

    /* Place the object there */
    i_list[cur_pos] = hack;

    c_ptr->i_idx = cur_pos;
    i_list[cur_pos].ident |= ID_NOSHOW_TYPE; /* don't show (+x of yyy) for these */
    if (k_list[tmp].level > object_level) {
	rating += 2 * (k_list[tmp].level - object_level);
    }

    /* Is it on the player? */
    if (c_ptr->m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }

    /* Success */
    return TRUE;
}


/*
 * Attempts to places a random object at the given location -RAK-
 */
void place_object(int y, int x)
{
    register int cur_pos, tmp;
    cave_type *c_ptr;

    /* Certain locations are not valid */
    if (!valid_grid(y,x)) return;


    /* Get the cave */
    c_ptr = &cave[y][x];

    /* Delete anything already there */
    delete_object(y, x);
    
    if (randint(MAX_K_IDX)>OBJ_SPECIAL && randint(10)==1)
	if (special_place_object(y, x)) return;

    /* Make it */
    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;

    do {	   /* don't generate another chest if opening_chest is true -CWS */
	tmp = get_obj_num(dun_level, FALSE);
    } while (opening_chest && (k_list[tmp].tval == TV_CHEST));
	
    invcopy(&i_list[cur_pos], tmp);
    apply_magic(&i_list[cur_pos], dun_level, FALSE, FALSE, 0);
    if (k_list[tmp].level > dun_level)
	rating += k_list[tmp].level - dun_level;
    if (peek) {
	if (k_list[tmp].level > dun_level) {
	    char buf[200];
	    objdes_store(buf, &i_list[cur_pos], TRUE);
	    msg_print(buf);
	}
    }

    /* Under the player */
    if (c_ptr->m_idx == 1) {
	msg_print ("You feel something roll beneath your feet.");
    }
}


/*
 * Places a "GOOD" object at given row, column co-ordinate ~Ludwig 
 * If "great" is TRUE, place a "GREAT" object
 *
 * Really only called when MF2_GOOD monster dies, or scroll of
 * acquirement read, or "vault" is constructed.  Perhaps the
 * normal "place_object()" should occasionally call us...
 *
 * This routine uses "object_level" for the "generation level".
 */
void place_good(int y, int x, bool great)
{
    register int cur_pos, k_idx;
    int          tv, sv;
    cave_type *c_ptr;

    /* Do not hurt artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;


    /* Get the grid */
    c_ptr = &cave[y][x];


    /* Hack -- much higher chance of doing "Special Objects" */
    if (randint(10) == 1) {
	if (special_place_object(y, x)) return;
    }


    /* Pick a good "base object" */
    while (1) {

	/* Pick a random object, based on "object_level" */
	k_idx = get_obj_num((object_level + 10), TRUE);

	/* Examine the object */
	tv = k_list[k_idx].tval;
	sv = k_list[k_idx].sval;

	/* Rusty Chainmail is not good */
	if ((tv == TV_HARD_ARMOR) && (sv == SV_RUSTY_CHAIN_MAIL)) continue;

	/* Filthy Rags are not good */
	if ((tv == TV_SOFT_ARMOR) && (sv == SV_FILTHY_RAG)) continue;

	/* Broken daggers/swords are not good */
	if ((tv == TV_SWORD) && (sv == SV_BROKEN_DAGGER)) continue;
	if ((tv == TV_SWORD) && (sv == SV_BROKEN_SWORD)) continue;

	/* Normal weapons/armour are okay (except "shots" or "shovels") */
	if ((tv == TV_HELM) || (tv == TV_SHIELD) || (tv == TV_CLOAK) ||
	    (tv == TV_SWORD) || (tv == TV_HAFTED) || (tv == TV_POLEARM) ||
	    (tv == TV_BOW) || (tv == TV_BOLT) || (tv == TV_ARROW) ||
	    (tv == TV_HARD_ARMOR) || (tv == TV_SOFT_ARMOR) ||
	    (tv == TV_DRAG_ARMOR) || (tv == TV_BOOTS) || (tv == TV_GLOVES)) {
	    break;
	}

	/* XXX Hack -- High spell books are good.  Highest is great. */
	if (((tv == TV_MAGIC_BOOK) || (tv == TV_PRAYER_BOOK)) &&
	     (sv >= (great ? (SV_BOOK + 8) : (SV_BOOK + 4)))) {
	    break;
	}
    }


    /* Delete anything already there */
    delete_object(y, x);

    /* Make a new object, drop into dungeon */
    cur_pos = i_pop();
    invcopy(&i_list[cur_pos], k_idx);

    /* Drop it into the dungeon */
    c_ptr->i_idx = cur_pos;

    /* Apply some good magic to the item.  Make a great item if requested. */
    apply_magic(&i_list[cur_pos], object_level, TRUE, TRUE, great);

	/* Hack -- look at it */
    if (peek) {
	if (k_list[k_idx].level > object_level) {
	    char                buf[200];
	    objdes_store(buf, &i_list[cur_pos], TRUE);
	    msg_print(buf);
	}
    }

    if (c_ptr->m_idx == 1) {
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
			place_good(j, k, TRUE);
		} else {
		    place_good(j, k, TRUE);
		}
		i = 9;
	}
    }
}




/*
 * Places a particular trap at location y, x		-RAK-	 
 */
void place_trap(int y, int x, int sval)
{
    cave_type *c_ptr;
    register int cur_pos;


    /* Do not hurt artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;


    /* Get the cave grid */
    c_ptr = &cave[y][x];


    /* Don't put traps under player/monsters, it's annoying -CFT */
    if (c_ptr->m_idx) return;


    /* Delete whatever is there */
    delete_object(y, x);

    /* Make a new object */
    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
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

    /* Hack -- nuke any walls */
    c_ptr->fval = CORR_FLOOR;

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
    cave_type *c_ptr;


    /* Do not hurt illegals, artifacts, stairs, store doors */
    if (!valid_grid(y, x)) return;


    /* Get the grid */
    c_ptr = &cave[y][x];

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

    c_ptr->i_idx = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_GOLD_LIST + i);
    i_ptr = &i_list[cur_pos];
    i_ptr->cost += (8L * (long)randint((int)i_ptr->cost)) + randint(8);

    /* average the values to make Creeping _xxx_ coins not give too great treasure drops */
    if (coin_type) {
	i_ptr->cost = ((8L * (long)randint((int)k_list[OBJ_GOLD_LIST + i].cost))
		       + (i_ptr->cost)) >> 1;
    }

    /* Under the player */
    if (c_ptr->m_idx == 1) {
	msg_print("You feel something roll beneath your feet.");
    }
}


/*
 * An entry for the object allocator below
 */
typedef struct _kind_entry {
    u16b k_idx;		/* Object kind index */
    byte locale;		/* Base dungeon level */
    byte chance;		/* Rarity of occurance */
} kind_entry;


/*
 * Returns the array number of a random object
 * Uses the locale/chance info for distribution.
 */
int get_obj_num(int level, int good)
{
    register int i, j;

    /* Number of entries in the "k_sort" table */
    static u16b size = 0;

    /* The actual table of entries */
    static kind_entry *table = NULL;

    /* Number of entries at each locale */
    static u16b t_lev[256];

    /* Initialize the table */
    if (!size) {

	inven_kind *k_ptr;

	u16b aux[256];

	/* Clear the level counter and the aux array */
	for (i = 0; i < 256; i++) t_lev[i] = aux[i] = 0;

	/* Scan all of the objects */
	for (i = 0; i < MAX_K_IDX; i++) {

	    /* Get the i'th object */
	    k_ptr = &k_list[i];

	    /* Scan all of the locale/chance pairs */
	    for (j = 0; j < 4; j++) {

		/* Count valid pairs */
		if (k_ptr->chance[j]) {

		    /* Count the total entries */
		    size++;

		    /* Count the entries at each level */
		    t_lev[k_ptr->locale[j]]++;
		}
	    }
	}

	/* Combine the "t_lev" entries */
	for (i = 1; i < 256; i++) t_lev[i] += t_lev[i-1];

	/* Allocate the table */
	C_MAKE(table, size, kind_entry);

	/* Initialize the table */
	for (i = 0; i < MAX_K_IDX; i++) {

	    /* Get the i'th object */
	    k_ptr = &k_list[i];

	    /* Scan all of the locale/chance pairs */
	    for (j = 0; j < 4; j++) {

		/* Count valid pairs */
		if (k_ptr->chance[j]) {

		    int r, x, y, z;

		    /* Extract the chance/locale */                    
		    r = k_ptr->chance[j];
		    x = k_ptr->locale[j];

		    /* Skip entries preceding our locale */
		    y = (x > 0) ? t_lev[x-1] : 0;

		    /* Skip previous entries at this locale */
		    z = y + aux[x];

		    /* Load the table entry */
		    table[z].k_idx = i;
		    table[z].locale = x;
		    table[z].chance = r;

		    /* Another entry complete for this locale */
		    aux[x]++;
		}
	    }
	}
    }


    /* Pick an object */
    while (1) {

	/* Town level is easy */
	if (level == 0) {

	    /* Pick a level 0 entry */
	    i = rand_int(t_lev[0]);
	}

	/* Other levels sometimes have great stuff */
	else {

	    /* Never exceed a given level */
	    if (level > MAX_K_LEV) level = MAX_K_LEV;

	    /* Occasionally, get a "better" object */
	    if (randint(GREAT_OBJ) == 1) {

		/* What a bizarre calculation */
		level = 1 + (level * MAX_K_LEV / randint(MAX_K_LEV));
		if (level > MAX_K_LEV) level = MAX_K_LEV;
	    }


	/*
	 * This code has been added to make it slightly more likely to get
	 * the higher level objects.	Originally a uniform distribution
	 * over all objects less than or equal to the dungeon level.  This
	 * distribution makes a level n objects occur approx 2/n% of the time
	 * on level n, and 1/2n are 0th level. 
	 */

	    /* Pick any object at or below the given level */
	    i = rand_int(t_lev[level]);

	    /* Sometimes, try for a "better" item */
	    if (randint(3) != 1) {

		/* Pick another object at or below the given level */
		j = rand_int(t_lev[level]);

		/* Keep it if it is "better" */
		if (table[i].locale < table[j].locale) i = j;
	    }

	    /* Sometimes, try for a "better" item */
	    if (randint(3) != 1) {

		/* Pick another object at or below the given level */
		j = rand_int(t_lev[level]);

		/* Keep it if it is "better" */
		if (table[i].locale < table[j].locale) i = j;
	    }
	}

	/* Access the "k_idx" of the chosen item */
	j = table[i].k_idx;

	/* The "good" parameter overwhelms "chance" requirements */
	if (good) break;

	/* Play the "chance game" */
	if (randint(table[i].chance) == 1) break;
    }

    /* Accept that object */
    return (j);
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
	&& (p_ptr->use_stat[A_STR] * 15 < i_ptr->weight)) {
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
	p_ptr->speed += i - pack_heavy;
	p_ptr->status |= PY_SPEED;
	pack_heavy = i;
    }
    p_ptr->status &= ~PY_STR_WGT;

    if (p_ptr->pclass == 2 && !notlike) {
        if ((i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
            && ((i_ptr->flags3 & TR3_BLESSED) == 0)) {
            notlike = TRUE;
            msg_print("You do not feel comfortable with your weapon.");
        }
    } else if (p_ptr->pclass == 2 && notlike) {
        if (i_ptr->tval == TV_NOTHING) {
            notlike = FALSE;
            msg_print("You feel comfortable again after removing that weapon.");
        } else if (!(i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)
		   || !((i_ptr->flags3 & TR3_BLESSED) == 0)) {
            notlike = FALSE;
            msg_print("You feel comfortable with your weapon once more.");
        }
    }
}




