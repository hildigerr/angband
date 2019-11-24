/*
 * misc2.c: misc code for maintaining the dungeon, printing player info 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "monster.h"

/* include before angband.h because param.h defines NULL incorrectly */
#ifndef USG
#include <sys/param.h>
#endif

#include "angband.h"


#ifdef ATARIST_MWC
char               *index();
#endif


extern int rating;

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


/* Destroys a type of item on a given percent chance	-RAK-	 */
int 
inven_damage(typ, perc)
#ifdef MSDOS
int (*typ) (inven_type *);

#else
int (*typ) ();

#endif
register int perc;

{
    register int index, i, j, offset;
    vtype        tmp_str, out_val;

    j = 0;
    offset = randint(inven_ctr);
    for (index = 0; index < inven_ctr; index++) {
	i = (index + offset) % inven_ctr; /* be clever and not destroy the first item */
	if ((*typ) (&inventory[i]) && (randint(100) < perc)) {
	    objdes(tmp_str, &inventory[i], FALSE);
	    sprintf(out_val, "%sour %s (%c) %s destroyed!",
		    ((inventory[i].sval <= ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked single items */
		    ? "One of y" : "Y",
		    tmp_str, i + 'a',
		    ((inventory[i].sval > ITEM_SINGLE_STACK_MAX) &&
		     (inventory[i].number > 1))	/* stacked group items */
		    ? "were" : "was");
	    msg_print(out_val);
	    inven_destroy(i);
	    j++;
	}
    }
    return (j);
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




/* calculate number of spells player should have, and learn forget spells
 * until that number is met -JEW- 
 */
void 
calc_spells(stat)
int stat;
{
    register int    i;
    register u32b mask;
    u32b          spell_flag;
    int             j, offset;
    int             num_allowed, new_spells, num_known, levels;
    vtype           tmp_str;
    const char           *p;
    register struct misc *p_ptr;
    register spell_type  *msp_ptr;

    p_ptr = &py.misc;
    msp_ptr = &magic_spell[p_ptr->pclass - 1][0];
    if (stat == A_INT) {
	p = "spell";
	offset = SPELL_OFFSET;
    } else {
	p = "prayer";
	offset = PRAYER_OFFSET;
    }

/* check to see if know any spells greater than level, eliminate them */
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
	if (mask & spell_learned)
	    num_known++;
	if (mask & spell_learned2)
	    num_known++;
    }

    new_spells = num_allowed - num_known;

    if (new_spells > 0) {

    /* remember forgotten spells while forgotten spells exist of new_spells
     * positive, remember the spells in the order that they were learned 
     */
	for (i = 0; ((spell_forgotten | spell_forgotten2) && new_spells
		     && (i < num_allowed) && (i < 64)); i++) {
	/* j is (i+1)th spell learned */
	    j = spell_order[i];

	/* shifting by amounts greater than number of bits in long gives an
	 * undefined result, so don't shift for unknown spells 
	 */
	    if (j == 99)
		continue;	   /* don't process unknown spells... -CFT */

	    if (j < 32) {	   /* use spell_learned, spell_forgotten...
				    * -CFT */
		mask = 1L << j;	   /* bit in spell fields */
		if (mask & spell_forgotten) {
		    if (msp_ptr[j].slevel <= p_ptr->lev) {
			spell_forgotten &= ~mask;
			spell_learned |= mask;
			new_spells--;
			(void)sprintf(tmp_str, "You have remembered the %s of %s.", p,
				      spell_names[j + offset]);
			msg_print(tmp_str);
		    } else
			num_allowed++;	/* if was too high lv to remember */
		} /* if mask&spell_forgotten */
	    }
	     /* j < 32 */ 
	    else {		   /* j > 31, use spell_learned2,
				    * spell_forgotten2... -CFT */
		mask = 1L << (j - 32);	/* bit in spell fields */
		if (mask & spell_forgotten2) {
		    if (msp_ptr[j].slevel <= p_ptr->lev) {
			spell_forgotten2 &= ~mask;
			spell_learned2 |= mask;
			new_spells--;
			(void)sprintf(tmp_str, "You have remembered the %s of %s.", p,
				      spell_names[j + offset]);
			msg_print(tmp_str);
		    } else
			num_allowed++;	/* if was too high lv to remember */
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
	    for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++)
		if (spell_flag & mask) {
		    spell_flag &= ~mask;
		    if (msp_ptr[j].slevel <= p_ptr->lev)
			i++;
		}
	/* only bother with spells learnable by class -CFT */
	    spell_flag = spellmasks[py.misc.pclass][1] & ~spell_learned2;
	    mask = 0x1;
	    for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++)
		if (spell_flag & mask) {
		    spell_flag &= ~mask;
		    if (msp_ptr[j + 32].slevel <= p_ptr->lev)
			i++;
		}
	    if (new_spells > i)
		new_spells = i;
	}
    } else if (new_spells < 0) {

    /* forget spells until new_spells zero or no more spells know, spells are
     * forgotten in the opposite order that they were learned 
     */
	for (i = 63; new_spells && (spell_learned | spell_learned2); i--) {
	/* j is the (i+1)th spell learned */
	    j = spell_order[i];

	/* shifting by amounts greater than number of bits in long gives an
	 * undefined result, so don't shift for unknown spells 
	 */
	    if (j == 99)
		continue;	   /* don't process unknown spells... -CFT */

	    if (j < 32) {	   /* use spell_learned, spell_forgotten...
				    * -CFT */
		mask = 1L << j;	   /* bit in spell fields */
		if (mask & spell_learned) {
		    spell_learned &= ~mask;
		    spell_forgotten |= mask;
		    new_spells++;
		    (void)sprintf(tmp_str, "You have forgotten the %s of %s.", p,
				  spell_names[j + offset]);
		    msg_print(tmp_str);
		} /* if mask&spell_learned */
	    }
	     /* j < 32 */ 
	    else {		   /* j > 31, use spell_learned2,
				    * spell_forgotten2... -CFT */
		mask = 1L << (j - 32);	/* bit in spell fields */
		if (mask & spell_learned2) {
		    spell_learned2 &= ~mask;
		    spell_forgotten2 |= mask;
		    new_spells++;
		    (void)sprintf(tmp_str, "You have forgotten the %s of %s.", p,
				  spell_names[j + offset]);
		    msg_print(tmp_str);
		} /* if mask&spell_learned2 */
	    } /* j > 31 */
	} /* for loop... */
	new_spells = 0;		   /* we've forgotten, so we shouldn't be
				    * learning any... */
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


/* gain spells when player wants to		- jw */
void 
gain_spells()
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
    if (py.flags.blind > 0)
	msg_print("You can't see to read your spell book!");
    else if (no_lite())
	msg_print("You have no light to read by.");
    else if (py.flags.confused > 0)
	msg_print("You are too confused.");
    else
	i = 1;
    if (i == 0)
	return;

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

    for (last_known = 0; last_known < 64; last_known++)
	if (spell_order[last_known] == 99)
	    break;

    if (!new_spells) {
	(void)sprintf(tmp_str, "You can't learn any new %ss!",
		      (stat == A_INT ? "spell" : "prayer"));
	msg_print(tmp_str);
	free_turn_flag = TRUE;
    } else {
    /* determine which spells player can learn */
    /* mages need the book to learn a spell, priests do not need the book */
	spell_flag = 0;
	spell_flag2 = 0;
	for (i = 0; i < inven_ctr; i++)
	    if (((stat == A_INT) && (inventory[i].tval == TV_MAGIC_BOOK))
	    || ((stat == A_WIS) && (inventory[i].tval == TV_PRAYER_BOOK))) {
		spell_flag |= inventory[i].flags;
		spell_flag2 |= inventory[i].flags2;
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
	 * actually shown on the screen, so limit choice to those 
	 */
	    if (j >= 0 && j < i && j < 22) {
		new_spells--;
		if (spells[j] < 32)
		    spell_learned |= 1L << spells[j];
		else
		    spell_learned2 |= 1L << (spells[j] - 32);
		spell_order[last_known++] = spells[j];
		for (; j <= i - 1; j++)
		    spells[j] = spells[j + 1];
		i--;
		erase_line(j + 1, 31);
		print_spells(spells, i, FALSE, -1);
	    } else
		bell();
	}
	restore_screen();
    } else {
    /* pick a prayer at random */
	while (new_spells) {
	    j = randint(i) - 1;
	    if (spells[j] < 32)
		spell_learned |= 1L << spells[j];
	    else
		spell_learned2 |= 1L << (spells[j] - 32);
	    spell_order[last_known++] = spells[j];
	    (void)sprintf(tmp_str,
			  "You have learned the prayer of %s.",
			  spell_names[spells[j] + offset]);
	    msg_print(tmp_str);
	    for (; j <= i - 1; j++)
		spells[j] = spells[j + 1];
	    i--;
	    new_spells--;
	}
    }
    py.flags.new_spells = new_spells + diff_spells;
    if (py.flags.new_spells == 0)
	py.flags.status |= PY_STUDY;
/* set the mana for first level characters when they learn first spell */
    if (py.misc.mana == 0)
	calc_mana(stat);
}



/* Gain some mana if you know at least one spell	-RAK-	 */
void 
calc_mana(stat)
int stat;
{
    register int          new_mana, levels;
    register struct misc *p_ptr;
    register s32b        value;
    register int          i;
    register inven_type  *i_ptr;
    int                   amrwgt, maxwgt;

    p_ptr = &py.misc;
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
	if (new_mana > 0)
	    new_mana++;
	if ((inventory[INVEN_HANDS].tval != TV_NOTHING) &&
	    !((inventory[INVEN_HANDS].flags & TR_FREE_ACT) ||
	      ((inventory[INVEN_HANDS].flags & TR_DEX) &&
	       (inventory[INVEN_HANDS].p1 > 0)))
/* gauntlets of dex (or free action - DGK) can hardly interfere w/ spellcasting!
 * But cursed ones can! -CFT */

	    &&(py.misc.pclass == 1 || py.misc.pclass == 3 || py.misc.pclass == 4)) {
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
	if (amrwgt > maxwgt)
	    new_mana -= ((amrwgt - maxwgt) / 10);
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
		value = (((long)p_ptr->cmana << 16) + p_ptr->cmana_frac)
		    / p_ptr->mana * new_mana;
		p_ptr->cmana = value >> 16;
		p_ptr->cmana_frac = value & 0xFFFF;
	    } else {
		p_ptr->cmana = new_mana;
		p_ptr->cmana_frac = 0;
	    }
	    p_ptr->mana = new_mana;
	/* can't print mana here, may be in store or inventory mode */
	    py.flags.status |= PY_MANA;
	}
    } else if (p_ptr->mana != 0) {
	p_ptr->mana = 0;
	p_ptr->cmana = 0;
    /* can't print mana here, may be in store or inventory mode */
	py.flags.status |= PY_MANA;
    }
}




void 
insert_lnum(object_str, mtc_str, number, show_sign)
char                *object_str;
register const char *mtc_str;
s32b                number;
int                  show_sign;
{
    int            mlen;
    vtype          str1, str2;
    register char *string, *tmp_str;
    int            flag;

    flag = 1;
    mlen = strlen(mtc_str);
    tmp_str = object_str;
    do {
	string = (char *) index(tmp_str, mtc_str[0]);
	if (string == 0)
	    flag = 0;
	else {
	    flag = strncmp(string, mtc_str, mlen);
	    if (flag)
		tmp_str = string + 1;
	}
    }
    while (flag);
    if (string) {
	(void)strncpy(str1, object_str, (int)(string - object_str));
	str1[(int)(string - object_str)] = '\0';
	(void)strcpy(str2, string + mlen);
	if ((number >= 0) && (show_sign))
	    (void)sprintf(object_str, "%s+%ld%s", str1, (long)number, str2);
	else
	    (void)sprintf(object_str, "%s%ld%s", str1, (long)number, str2);
    }
}




/* Given direction "dir", returns new row, column location -RAK- */
/* targeting code stolen from Morgul -CFT */
/* 'dir=0' moves toward target				    CDW  */
int 
mmove(dir, y, x)
int                 dir;
register int       *y, *x;
{
    register int new_row = 0, new_col = 0;
    int          boolflag;

    switch (dir) {
#ifdef TARGET
      case 0:			/* targetting code stolen from Morgul -CFT */
	new_row = *y;
	new_col = *x;
	mmove2(&new_row, &new_col,
	       char_row, char_col,
	       target_row, target_col);
	break;
#endif /* TARGET */
      case 1:
	new_row = *y + 1;
	new_col = *x - 1;
	break;
      case 2:
	new_row = *y + 1;
	new_col = *x;
	break;
      case 3:
	new_row = *y + 1;
	new_col = *x + 1;
	break;
      case 4:
	new_row = *y;
	new_col = *x - 1;
	break;
      case 5:
	new_row = *y;
	new_col = *x;
	break;
      case 6:
	new_row = *y;
	new_col = *x + 1;
	break;
      case 7:
	new_row = *y - 1;
	new_col = *x - 1;
	break;
      case 8:
	new_row = *y - 1;
	new_col = *x;
	break;
      case 9:
	new_row = *y - 1;
	new_col = *x + 1;
	break;
    }
    boolflag = FALSE;
    if ((new_row >= 0) && (new_row < cur_height)
	&& (new_col >= 0) && (new_col < cur_width)) {
	*y = new_row;
	*x = new_col;
	boolflag = TRUE;
    }
    return (boolflag);
}




/* Add a comment to an object description.		-CJS- */
void 
scribe_object()
{
    int   item_val, j;
    vtype out_val, tmp_str;

    if (inven_ctr > 0 || equip_ctr > 0) {
	if (get_item(&item_val, "Which one? ", 0, INVEN_ARRAY_SIZE, 0)) {
	    objdes(tmp_str, &inventory[item_val], TRUE);
	    (void)sprintf(out_val, "Inscribing %s.", tmp_str);
	    msg_print(out_val);
	    if (inventory[item_val].inscrip[0] != '\0')
		(void)sprintf(out_val, "Replace \"%s\" with the inscription: ",
			      inventory[item_val].inscrip);
	    else
		(void)strcpy(out_val, "Inscription: ");
	    j = 78 - strlen(tmp_str);
	    if (j > 12)
		j = 12;
	    prt(out_val, 0, 0);
	    if (get_string(out_val, 0, strlen(out_val), j))
		inscribe(&inventory[item_val], out_val);
	}
    } else
	msg_print("You are not carrying anything to inscribe.");
}

/* Append an additional comment to an object description.	-CJS- */
void 
add_inscribe(i_ptr, type)
inven_type *i_ptr;
int         type;
{
    i_ptr->ident |= (byte) type;
}

/* Replace any existing comment in an object description with a new one. CJS */
void 
inscribe(i_ptr, str)
inven_type *i_ptr;
const char *str;
{
    (void)strcpy(i_ptr->inscrip, str);
}


