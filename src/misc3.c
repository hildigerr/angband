/*
 * misc2.c: misc code for maintaining the dungeon, printing player info 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

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
 *If too many objects on floor level, delete some of them
 */
static void compact_objects()
{
    register int        i, j;
    int                 ctr, cur_dis, chance;
    register cave_type *cave_ptr;


    /* Debugging message */
    msg_print("Compacting objects...");

    ctr = 0;
    cur_dis = 66;
    do {

	for (i = 0; i < cur_height; i++) {
	    for (j = 0; j < cur_width; j++) {

		cave_ptr = &cave[i][j];

		if ((cave_ptr->tptr != 0)
		    && (distance(i, j, char_row, char_col) > cur_dis)) {

		    switch (i_list[cave_ptr->tptr].tval) {
		    case TV_VIS_TRAP:
			chance = 15;
			break;
		    case TV_RUBBLE:
		    case TV_INVIS_TRAP:
		    case TV_OPEN_DOOR:
		    case TV_CLOSED_DOOR:
			chance = 5;
			break;
		      case TV_UP_STAIR:
		      case TV_DOWN_STAIR:
		      case TV_STORE_DOOR:
			chance = 0;
			break;
		    case TV_SECRET_DOOR:
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

		/* Apply the saving throw */
		if (randint(100) <= chance) {

		/* Delete it */
		delete_object(i, j);

		/* Count it */
			ctr++;
		    }
		}
	    }
}
	if (ctr == 0)
	    cur_dis -= 6;
    }
    while (ctr <= 0);
    if (cur_dis < 66)

    /* Redraw */
    prt_map();
}


/*
 * Gives pointer to next free space			-RAK-
 */
int i_pop(void)
{
    if (tcptr == MAX_TALLOC) compact_objects();
    return (tcptr++);
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

    /* standard deviation twice as wide at bottom of Angband as top */
    stand_dev = (OBJ_STD_ADJ * (1 + level / 100)) + OBJ_STD_MIN;

    /* check for level > max_std to check for overflow... */
    if (stand_dev > 40) stand_dev = 40;

    /* abs may be a macro, don't call it with randnor as a parameter */
    tmp = randnor(0, stand_dev);

    x = (tmp * diff / 150) + (level * limit / 200) + base;

    if (x < base) return (base);

    else return (x);
}




/* Places a particular trap at location y, x		-RAK-	 */
void 
place_trap(y, x, sval)
int y, x, sval;
{
    register int cur_pos;

    if (!in_bounds(y, x))
	return;	       /* abort! -CFT */
    if (cave[y][x].cptr >= MIN_M_IDX)
	return;	       /* don't put rubble under monsters, it's annoying -CFT */

    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_TRAP_LIST + sval);
}


/* Places rubble at location y, x			-RAK-	 */
void 
place_rubble(y, x)
int y, x;
{
    register int        cur_pos;
    register cave_type *cave_ptr;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    cur_pos = i_pop();
    cave_ptr = &cave[y][x];
    cave_ptr->tptr = cur_pos;
    cave_ptr->fval = BLOCKED_FLOOR;
    invcopy(&i_list[cur_pos], OBJ_RUBBLE);
}

/* if killed a 'Creeping _xxx_ coins'... -CWS */
void
get_coin_type(c_ptr)
monster_race *c_ptr;
{
    if (!stricmp(c_ptr->name, "Creeping copper coins")) {
	coin_type = 2;
    }

    if (!stricmp(c_ptr->name, "Creeping silver coins")) {
	coin_type = 5;
    }

    if (!stricmp(c_ptr->name, "Creeping gold coins")) {
	coin_type = 10;
    }

    if (!stricmp(c_ptr->name, "Creeping mithril coins")) {
	coin_type = 16;
    }

    if (!stricmp(c_ptr->name, "Creeping adamantite coins")) {
	coin_type = 17;
    }
}

/* Places a treasure (Gold or Gems) at given row, column -RAK-	 */
void 
place_gold(y, x)
int y, x;
{
    register int        i, cur_pos;
    register inven_type *t_ptr;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);
    cur_pos = i_pop();
    i = ((randint(object_level + 2) + 2) / 2) - 1;
    if (randint(GREAT_OBJ) == 1)
	i += randint(object_level + 1);
    if (i >= MAX_GOLD)
	i = MAX_GOLD - 1;
    if (coin_type) {			/* if killed a Creeping _xxx_ coins... */
	if (coin_type > MAX_GOLD - 1)
	    coin_type = 0;		/* safety check -CWS */
	i = coin_type;
    }
    cave[y][x].tptr = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_GOLD_LIST + i);
    t_ptr = &i_list[cur_pos];
    t_ptr->cost += (8L * (long)randint((int)t_ptr->cost)) + randint(8);

/* average the values to make Creeping _xxx_ coins not give too great treasure drops */
    if (coin_type)
	t_ptr->cost = ((8L * (long)randint((int)k_list[OBJ_GOLD_LIST + i].cost))
		       + (t_ptr->cost)) >> 1;
					  
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
}


/* Returns the array number of a random object		-RAK-	 */
int 
get_obj_num(level, good)
int level, good;
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



int 
special_place_object(y, x)
int y, x;
{
    register int cur_pos, tmp;
    char         str[100];
    int          done = 0;

    if (!in_bounds(y, x))
	return 0;		   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return 0;		   /* don't replace stairs, stores, artifacts */
	else
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
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    invcopy(&i_list[cur_pos], tmp);
    i_list[cur_pos].timeout = 0;
    i_list[cur_pos].ident |= ID_NOSHOW_TYPE; /* don't show (+x of yyy) for these */
    if (k_list[tmp].level > object_level) {
	rating += 2 * (k_list[sorted_objects[tmp]].level - object_level);
    }
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
    return (-1);
}

/* Places an object at given row, column co-ordinate    -RAK-   */
void
place_object(y, x)
int y, x;
{
    register int cur_pos, tmp;

    if (!in_bounds(y,x)) return; /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return; /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y,x);
    
    if (randint(MAX_OBJECTS)>OBJ_SPECIAL && randint(10)==1)
	if (special_place_object(y,x)==(-1))
	    return;
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;

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
    if (cave[y][x].cptr == 1)
	msg_print ("You feel something roll beneath your feet.");
}

/* Places a GOOD-object at given row, column co-ordinate ~Ludwig */
void 
place_good(y, x, good)
int    y, x;
u32b good;
{
    register int cur_pos, tmp;
    int          tv, is_good = FALSE;

    if (!in_bounds(y, x))
	return;			   /* abort! -CFT */
    if (cave[y][x].tptr != 0)
	if ((i_list[cave[y][x].tptr].tval == TV_STORE_DOOR) ||
	    (i_list[cave[y][x].tptr].tval == TV_UP_STAIR) ||
	    (i_list[cave[y][x].tptr].tval == TV_DOWN_STAIR) ||
	    ((i_list[cave[y][x].tptr].tval >= TV_MIN_WEAR) &&
	     (i_list[cave[y][x].tptr].tval <= TV_MAX_WEAR) &&
	     (i_list[cave[y][x].tptr].flags2 & TR_ARTIFACT)))
	    return;		   /* don't replace stairs, stores, artifacts */
	else
	    delete_object(y, x);

    if (randint(10) == 1)
	if (special_place_object(y, x) == (-1))
	    return;
    cur_pos = i_pop();
    cave[y][x].tptr = cur_pos;
    do {
	tmp = get_obj_num((object_level + 10), TRUE);
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
	    (k_list[sorted_objects[tmp]].sval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
	if ((tv == TV_PRAYER_BOOK) &&	/* if book, good must be one of the
					 * deeper, special must be Wrath of
					 * God */
	    (k_list[sorted_objects[tmp]].sval > ((good & SPECIAL) ? 71 : 67)))
	    is_good = TRUE;
    } while (!is_good);
    invcopy(&i_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, object_level, (good & SPECIAL) ? 666 : 1, 0);
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
    if (cave[y][x].cptr == 1)
	msg_print("You feel something roll beneath your feet.");
}




/* Creates objects nearby the coordinates given		-RAK-	 */
void 
random_object(y, x, num)
int y, x, num;
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    do {
	i = 0;
	do {
	    do {
		j = y - 3 + randint(5);
		k = x - 4 + randint(7);
	    } while (!in_bounds(j, k));
	    cave_ptr = &cave[j][k];
	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		object_level = dun_level;
		if (randint(100) < 75)
		    place_object(j, k);
		else
		    place_gold(j, k);
		i = 9;
	    }
	    i++;
	}
	while (i <= 10);
	num--;
    }
    while (num != 0);
}

void 
special_random_object(y, x, num)
int y, x, num;
{
    register int        i, j, k;
    register cave_type *cave_ptr;

    object_level = dun_level;
    do {
	i = 0;
	do {
	    j = y - 3 + randint(5);
	    k = x - 4 + randint(7);
	    cave_ptr = &cave[j][k];
	    if ((cave_ptr->fval <= MAX_CAVE_FLOOR) && (cave_ptr->tptr == 0)) {
		if (randint(5) == 1) {
		    if (!special_place_object(j, k))
			place_good(j, k, SPECIAL);
		} else {
		    place_good(j, k, SPECIAL);
		}
		i = 9;
	    }
	    i++;
	}
	while (i <= 10);
	num--;
    }
    while (num != 0);
}




/* Destroy an item in the inventory			-RAK-	 */
void 
inven_destroy(item_val)
int item_val;
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
    py.flags.status |= PY_STR_WGT;
}


/*
 * Copies the object in the second argument over the first argument. However,
 * the second always gets a number of one except for ammo etc. 
 */
void 
take_one_item(s_ptr, i_ptr)
register inven_type *s_ptr, *i_ptr;
{
    *s_ptr = *i_ptr;
    if ((s_ptr->number > 1) && (s_ptr->sval >= ITEM_SINGLE_STACK_MIN)
	&& (s_ptr->sval <= ITEM_SINGLE_STACK_MAX))
	s_ptr->number = 1;
}


/* Drops an item from inventory to given location	-RAK-	 */
void 
inven_drop(item_val, drop_all)
register int item_val, drop_all;
{
    int                  i;
    register inven_type *i_ptr;
    vtype                prt2;
    bigvtype             prt1;

    i_ptr = &inventory[item_val];
    if (cave[char_row][char_col].tptr != 0)
	(void)delete_object(char_row, char_col);
    i = i_pop();
    i_list[i] = *i_ptr;
    cave[char_row][char_col].tptr = i;

    if (item_val >= INVEN_WIELD)
	inven_takeoff(item_val, -1);
    else {
	if (drop_all || i_ptr->number == 1) {
	    inven_weight -= i_ptr->weight * i_ptr->number;
	    inven_ctr--;
	    while (item_val < inven_ctr) {
		inventory[item_val] = inventory[item_val + 1];
		item_val++;
	    }
	    invcopy(&inventory[inven_ctr], OBJ_NOTHING);
	} else {
	    i_list[i].number = 1;
	    inven_weight -= i_ptr->weight;
	    i_ptr->number--;
	}
	objdes(prt1, &i_list[i], TRUE);
	(void)sprintf(prt2, "Dropped %s.", prt1);
	msg_print(prt2);
    }
    py.flags.status |= PY_STR_WGT;
}



/* return FALSE if picking up an object would change the players speed */
int 
inven_check_weight(i_ptr)
register inven_type *i_ptr;
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
void 
check_strength()
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
	py.flags.speed += i - pack_heavy;
	py.flags.status |= PY_SPEED;
	pack_heavy = i;
    }
    py.flags.status &= ~PY_STR_WGT;

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




