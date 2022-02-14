/* File: store.c */

/* Purpose: store code, updating store inventory, pricing objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




#ifndef MAC
/* MPW doesn't seem to handle this very well, so replace store_buy array
   with a function call on mac */
/* functions defined in sets.c */
extern int general_store(), armory(), weaponsmith(), temple(),
  alchemist(), magic_shop();

int blackmarket();
int home();

/* Each store will buy only certain items, based on TVAL */
int (*store_buy[MAX_STORES])() = {
       general_store, armory, weaponsmith, temple, alchemist, magic_shop,
       blackmarket, home};
#endif

static cptr comment1[14] = {
    "Done! ", "Accepted! ", "Fine. ", "Agreed! ", "Ok. ", "Taken! ",
    "You drive a hard bargain, but taken. ",
    "You'll force me bankrupt, but it's a deal. ", "Sigh.  I'll take it. ",
    "My poor sick children may starve, but done! ", "Finally!  I accept. ",
    "Robbed again. ", "A pleasure to do business with you! ",
    "My spouse will skin me, but accepted. "
};

static cptr comment2a[3] = {
    "%A2 is my final offer; take it or leave it.",
    "I'll give you no more than %A2.",
    "My patience grows thin.  %A2 is final."
};

static cptr comment2b[16] = {
    "%A1 for such a fine item?  HA!  No less than %A2.",
    "%A1 is an insult!  Try %A2 gold pieces.",
    "%A1?!?  Go try Londis instead!",
    "Why, I'll take no less than %A2 gold pieces.",
    "Ha!  No less than %A2 gold pieces.",
    "Thou knave!  No less than %A2 gold pieces.",
    "%A1 is far too little, how about %A2?",
    "I paid more than %A1 for it myself, try %A2.",
    "%A1 my arse!  How about %A2 gold pieces?",
    "As scrap this would bring %A1.  Try %A2 in gold.",
    "May the fleas of 1000 orcs molest you.  I want %A2.",
    "My mother you can get for %A1, this costs %A2.",
    "May your most favourite parts go mouldy!  I want %A2 in gold!",
    "Sell this for such a pittance?  Give me %A2 gold.",
    "May Morgoth find you tasty!  %A2 gold pieces?",
    "Your mother was a Troll/Orc/Elf!  %A2 or I'll tell."
};

static cptr comment3a[3] = {
    "I'll pay no more than %A1; take it or leave it.",
    "You'll get no more than %A1 from me.",
    "%A1 and that's final."
};

static cptr comment3b[15] = {
    "%A2 for that piece of junk?  No more than %A1.",
    "For %A2 I could own ten of those.  Try %A1.",
    "%A2?  NEVER!  %A1 is more like it.",
    "Let's be reasonable...NOT! How about %A1 gold pieces?",
    "%A1 gold for that! That's you, that is!.",
    "%A1 gold pieces and be thankful for it!",
    "%A1 gold pieces and not a copper more.",
    "%A2 gold?  HA!  %A1 is more like it.", "Try about %A1 gold.",
    "I wouldn't pay %A2 for your bottom, try %A1.",
    "*CHOKE* For that!?  Let's say %A1.", "How about %A1?",
    "That looks war surplus!  Say %A1 gold.",
    "I'll buy it as scrap for %A1.",
    "%A2 is too much, let us say %A1 gold."
};

static cptr comment4a[5] = {
    "ENOUGH!  You have abused me once too often!",
    "THAT DOES IT!  You shall waste my time no more!",
    "This is getting nowhere.  I'm going to Londis!",
    "BAHAHAHAHAHA!  No more shall you insult me!",
    "Begone!  I have had enough abuse for one day."
};

static cptr comment4b[5] = {
    "Out of my place!", "out... Out... OUT!!!", "Come back tomorrow.",
    "Leave my place.  Begone!", "Come back when thou art richer."
};

static cptr comment5[10] = {
    "You will have to do better than that!", "That's an insult!",
    "Do you wish to do business or not?", "Hah!  Try again.",
    "Ridiculous!", "You've got to be kidding!", "You'd better be kidding!",
    "You try my patience.", "I don't hear you.",
    "Hmmm, nice weather we're having."
};

static cptr comment6[5] = {
    "I must have heard you wrong.", "What was that?",
    "I'm sorry, say that again.", "What did you say?",
    "Sorry, what was that again?"
};



/*
 * Given a buffer, replace the first occurance of the string "target"
 * with the textual form of the long integer "number"
 */
static bool insert_lnum(char *buf, cptr target, s32b number)
{
    char	   insert[32];

    /* Prepare a string to insert */
    sprintf(insert, "%ld", (long)number);

    /* Insert it */
    return (insert_str(buf, target, insert));
}


/*
 * Successful haggle.
 */
static void prt_comment1(void)
{
    msg_print(comment1[randint(14) - 1]);
}


/*
 * Continue haggling (player is buying)
 */
static void prt_comment2(s32b offer, s32b asking, int final)
{
    vtype comment;

    if (final > 0) {
	(void)strcpy(comment, comment2a[randint(3) - 1]);
    }
    else {
	(void)strcpy(comment, comment2b[randint(16) - 1]);
    }

    insert_lnum(comment, "%A1", offer);
    insert_lnum(comment, "%A2", asking);

    msg_print(comment);
}


/*
 * Continue haggling (player is selling)
 */
static void prt_comment3(s32b offer, s32b asking, int final)
{
    vtype comment;

    if (final > 0) {
	(void)strcpy(comment, comment3a[rand_int(3)]);
    }
    else {
	(void)strcpy(comment, comment3b[rand_int(15)]);
    }

    insert_lnum(comment, "%A1", offer);
    insert_lnum(comment, "%A2", asking);

    msg_print(comment);
}


/*
 * Kick 'da bum out.					-RAK-	 
 */
static void prt_comment4(void)
{
    msg_print(comment4a[rand_int(5)]);
    msg_print(comment4b[rand_int(5)]);
}


/*
 * You are insulting me
 */
static void prt_comment5(void)
{
    msg_print(comment5[rand_int(10)]);
}


/*
 * That makes no sense.
 */
static void prt_comment6(void)
{
    msg_print(comment6[rand_int(5)]);
}




#define MDO MAX_DUNGEON_OBJ

/*
 * Hack -- Kind index objects that may appear in the stores
 *
 * XXX This whole function is really just a giant hack... XXX
 */

static u16b store_choice[MAX_STORES][STORE_CHOICES] = {

	/* General Store */
    { MDO, MDO, MDO, MDO, MDO, MDO, MDO, MDO, MDO+21, MDO+21,
      MDO+21, MDO+21, MDO+22, MDO+22, MDO+22, MDO+1, MDO+2, MDO+3, MDO+4, MDO+22,
      MDO+20, MDO+21, MDO+5, MDO+6, 84, 84, 123, MDO+22, MDO+22, MDO+21 },

	/* Armoury */
    { 103, 104, 105, 106, 107, 108, 109, 91, 92, 125,
      126, 128, 129, 130, 91, 92, 94, 95, 96, 103,
      104, 105, 125, 128, 94, 95, 111, 112, 113, 121 },

	/* Weaponsmith */
    { 29, 29, 29, 31, 34, 35, 42, 46, 49, 58,
      60, 61, 63, 64, 68, 73, 74, 75, 77, 78,
      80, 82, 83, 83, 78, 80, 82, 35, 65, 66 },

	/* Temple */
    { 334, 335, 336, 337, 334, 335, 336, 337, 257, 237,
      261, 262, 233, 233, 240, 241, 260, 260, MDO+14, MDO+15,
      MDO+15, MDO+15, 53, 54, 55, 52, 335, 180, 237, 240 },

	/* Alchemy shop */
    { 227, 227, 230, 230, 236, 206, 252, 252, 253, 253,
      MDO+7, MDO+7, MDO+7, MDO+8, MDO+8, MDO+8, MDO+9, MDO+10, MDO+11, MDO+12,
      MDO+13, MDO+15, MDO+15, 173, 174, 175, 185, 185, 185, 206 },

	/* Magic-User store*/
    { 330, 331, 332, 333, 330, 331, 332, 333, 326, 293,
      293, 299, 303, 301, 302, 318, 326, 282, 277, 279,
      292, 164, 167, 168, 153, 137, 142, 326, 328, 299 }
};


store_type store[MAX_STORES];

/*
 * Store owners have different characteristics for pricing and haggling
 * Note: Store owners should be added in groups, one for each store    
 */

static owner_type owners[MAX_OWNERS] = {

{"Rincewind the Chicken  (Human)      General Store",
	  450,	175,  108,    4,  0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armoury"	    ,
	32000,	200,  112,    4,  5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith"  ,
	10000,	185,  110,    5,  1,  8},
{"Ludwig the Humble      (Human)      Temple"	    ,
	 5000,	175,  109,    6,  0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist"    ,
	12000,	220,  115,    4,  4,  9},
{"Luthien Starshine      (Elf)        Magic Shop"   ,
	32000,	175,  110,    5,  2, 11},
{"Durwin the Shifty      (Human)      Black Market" ,
	32000,	250,  155,   10,  0,  5},
{"Your home"   ,
	    1,    1,    1,    1,  1,  1},
{"Bilbo the Friendly     (Hobbit)     General Store",
	  300,	170,  108,    5,  3, 15},
{"Darg-Low the Grim      (Human)      Armoury"	    ,
	10000,	190,  111,    4,  0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith"  ,
	32000,	195,  112,    4,  5,  8},
{"Gunnar the Paladin     (Human)      Temple"	    ,
	12000,	185,  110,    5,  0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist"    ,
	10000,	190,  111,    5,  1,  8},
{"Buggerby the Great!    (Gnome)      Magic Shop"   ,
	20000,	215,  113,    6,  4, 10},
{"Histor the Goblin      (Orc)        Black Market"   ,
	32000,	250,  160,   10,  6,  5},
{"Your sweet abode"   ,
	    1,    1,    1,    1,  1,  1},
{"Lyar-el the Comely     (Elf)        General Store",
	  600,	165,  107,    6,  2, 18},
{"Decado the Handsome    (Human)      Armoury",
	25000,  200,  112,    4,  5, 10},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith"  ,
	 6000,	210,  115,    6,  7,  8},
{"Delilah the Pure       (Half-Elf)   Temple"	    ,
	25000,	180,  107,    6,  1, 20},
{"Wizzle the Chaotic     (Hobbit)     Alchemist"    ,
	10000,	190,  110,    6,  3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000,	200,  110,    7,  0, 10},
{"Drago the Fair?        (Elf)        Black Market" ,
	32000,	250,  150,   10,  2,  5},
{"Your house"   ,
	    1,    1,    1,    1,  1,  1}
};


/*
 * Buying and selling adjustments for character race VS store
 * owner race							
 */

static byte rgold_adj[MAX_RACES][MAX_RACES] = {

			/*Hum, HfE, Elf,  Hal, Gno, Dwa, HfO, HfT, Dun, HiE*/

/*Human		 */	 { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105},
/*Half-Elf	 */	 { 110, 100, 100, 105, 110, 120, 125, 130, 110, 100},
/*Elf		 */	 { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100},
/*Halfling	 */	 { 115, 110, 105,  95, 105, 110, 115, 130, 115, 105},
/*Gnome		 */	 { 115, 115, 110, 105,  95, 110, 115, 130, 115, 110},
/*Dwarf		 */	 { 115, 120, 120, 110, 110,  95, 125, 135, 115, 120},
/*Half-Orc	 */	 { 115, 120, 125, 115, 115, 130, 110, 115, 115, 125},
/*Half-Troll	 */	 { 110, 115, 115, 110, 110, 130, 110, 110, 110, 115},
/*Dunedain 	 */	 { 100, 105, 105, 110, 113, 115, 120, 125, 100, 105},
/*High_Elf	 */	 { 110, 105, 100, 105, 110, 120, 125, 130, 110, 100}

};



/*
 * We store the current "store number" here so everyone can access it
 */
static int store_num = 0;

/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current "owner type" here so everyone can access it
 */
static owner_type *ot_ptr = NULL;





/*
 * Returns the value for any given object -RAK-
 */
s32b item_value(inven_type *i_ptr)
{
    s32b value;

    /* Start with the item's known base cost */
    value = i_ptr->cost;

    /* don't purchase known cursed items */
    if (i_ptr->ident & ID_DAMD) value = 0;

		/* Weapons and armor	 */
    else if (((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) ||
	     ((i_ptr->tval >= TV_BOOTS) && (i_ptr->tval <= TV_SOFT_ARMOR))) {
	if (!known2_p(i_ptr))
	    value = k_list[i_ptr->k_idx].cost;
	else if ((i_ptr->tval >= TV_BOW) && (i_ptr->tval <= TV_SWORD)) {
	    if (i_ptr->tohit < 0)
		value = 0;
	    else if (i_ptr->todam < 0)
		value = 0;
	    else if (i_ptr->toac < 0)
		value = 0;
	    else
		value = i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 100;
	} else {
	    if (i_ptr->toac < 0)
		value = 0;
	    else
		value = i_ptr->cost + i_ptr->toac * 100;
	}
    } else if (((i_ptr->tval >= TV_SHOT) && (i_ptr->tval <= TV_ARROW))
	       || (i_ptr->tval == TV_SPIKE)) {	/* Ammo			 */
	if (!known2_p(i_ptr))
	    value = k_list[i_ptr->k_idx].cost;
	else {
	    if (i_ptr->tohit < 0)
		value = 0;
	    else if (i_ptr->todam < 0)
		value = 0;
	    else if (i_ptr->toac < 0)
		value = 0;
	    else

	    /* use 5, because missiles generally appear in groups of 20, so
	     * 20 * 5 == 100, which is comparable to weapon bonus above 
	     */
		value = i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 5;
	}
				/* Potions, Scrolls, and Food */
    } else if ((i_ptr->tval == TV_SCROLL1) || (i_ptr->tval == TV_SCROLL2) ||
	       (i_ptr->tval == TV_POTION1) || (i_ptr->tval == TV_POTION2)) {
	if (!known1_p(i_ptr))
	    value = 20;
    } else if (i_ptr->tval == TV_FOOD) {
	if ((i_ptr->sval < (ITEM_SINGLE_STACK_MIN + MAX_SHROOM))
	    && !known1_p(i_ptr))
	    value = 1;
				/* Rings and amulets */
    } else if ((i_ptr->tval == TV_AMULET) || (i_ptr->tval == TV_RING)) {
	/* player does not know what type of ring/amulet this is */
	if (!known1_p(i_ptr))
	    value = 45;
	else if (!known2_p(i_ptr))
	/* player knows what type of ring, but does not know whether it is
	 * cursed or not, if refuse to buy cursed objects here, then player
	 * can use this to 'identify' cursed objects 
	 */
	    value = k_list[i_ptr->k_idx].cost;
				/* Wands and staffs */
    } else if ((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND)) {
	if (!known1_p(i_ptr)) {

	    if (i_ptr->tval == TV_WAND)
		value = 50;
	    else
		value = 70;
	} else if (known2_p(i_ptr))
	    value = i_ptr->cost + (i_ptr->cost / 20) * i_ptr->pval;
    }
				/* picks and shovels */
    else if (i_ptr->tval == TV_DIGGING) {
	if (!known2_p(i_ptr))
	    value = k_list[i_ptr->k_idx].cost;
	else {
	    if (i_ptr->pval < 0)
		value = 0;
	    else {

	    /* some digging tools start with non-zero pval values, so only
	     * multiply the plusses by 100, make sure result is positive 
	     * no longer; have adjusted costs in treasure.c -CWS
	     */
		value = i_ptr->cost + i_ptr->pval;
		if (value < 0)
		    value = 0;
	    }
	}
    }
/* multiply value by number of items if it is a group stack item */
    if (i_ptr->sval > ITEM_GROUP_MIN)	/* do not include torches here */
	value = value * i_ptr->number;
    return (value);
}


static void special_offer(inven_type *i_ptr)
{
    s32b orig_cost = i_ptr->cost;

    /* Possible discount */    
    if (randint(30) == 1) {
	i_ptr->cost = (i_ptr->cost * 3) / 4;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "25% discount");
    } else if (randint(150) == 1) {
	i_ptr->cost /= 2;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "50% discount");
    } else if (randint(300) == 1) {
	i_ptr->cost /= 4;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "75% discount");
    } else if (randint(500) == 1) {
	i_ptr->cost /= 10;
	if (i_ptr->cost < 1)
	    i_ptr->cost = 1;
	if (i_ptr->cost < orig_cost)
	    inscribe(i_ptr, "to clear");
    }
}






/*
 * Asking price for an item			-RAK-
 */
static s32b sell_price(s32b *max_sell, s32b *min_sell, inven_type *i_ptr)
{
    register s32b      i;

    i = item_value(i_ptr);

    /* check i_ptr->cost in case it is cursed, check i in case it is damaged */
    if ((i_ptr->cost > 0) && (i > 0)) {

    /* Get the "basic value" */
    i = i * rgold_adj[ot_ptr->owner_race][p_ptr->prace] / 100;

    /* Nothing becomes free */
    if (i < 1) i = 1;

    /* Extract min/max sell values */
    *max_sell = i * ot_ptr->max_inflate / 100;
    *min_sell = i * ot_ptr->min_inflate / 100;

    /* Black market is always over-priced */
    if (store_num == 6) {
	(*max_sell) *= 2;
	(*min_sell) *= 2;
    }

    /* Paranoia */
    if (*min_sell > *max_sell) *min_sell = *max_sell;

    /* Return the price */
    return (i);
    } else
    /* don't let the item get into the store inventory */
	return (0);
}



/*
 * Check to see if the shop will be carrying too many objects	-RAK-	 
 */
static int store_check_num(inven_type *i_ptr)
{
    register int        store_check, i;
    register inven_type *j_ptr;

    store_check = FALSE;

    if (st_ptr->store_ctr < STORE_INVEN_MAX) store_check = TRUE;

    else if (i_ptr->sval >= ITEM_SINGLE_STACK_MIN)
	for (i = 0; i < st_ptr->store_ctr; i++) {
	    j_ptr = &st_ptr->store_item[i];

	/* note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match 
	 */
	    if (j_ptr->tval == i_ptr->tval && j_ptr->sval == i_ptr->sval
		&& ((int)j_ptr->number + (int)i_ptr->number < 256)
		&& (i_ptr->sval < ITEM_GROUP_MIN
		    || (j_ptr->pval == i_ptr->pval)))
		store_check = TRUE;
	}

/* But, wait.  If at home, don't let player drop 25th item, or he will lose it. -CFT */
    if (store_num == 7 && (i_ptr->sval >= ITEM_SINGLE_STACK_MIN))
	for (i = 0; i < st_ptr->store_ctr; i++) {
	    j_ptr = &st_ptr->store_item[i];
	/*
	 * note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match 
	 */
	    if (j_ptr->tval == i_ptr->tval && j_ptr->sval == i_ptr->sval
		&& ((int)j_ptr->number + (int)i_ptr->number > 24)
		&& (i_ptr->sval < ITEM_GROUP_MIN
		    || (j_ptr->pval == i_ptr->pval)))
		store_check = FALSE;
	}
    return (store_check);
}



/*
 * Add the item in INVEN_MAX to stores inventory.	-RAK-	 
 */
static void store_carry(int *ipos, inven_type *i_ptr)
{
    int                 item_num, item_val, flag;
    register int        typ, subt;
    s32b               icost, dummy;
    register inven_type *j_ptr;
    int stacked = FALSE; /* from inven_carry() -CFT */

    *ipos = -1;
    if (sell_price(&icost, &dummy, i_ptr) > 0 || store_num == 7)
    {
	item_val = 0;
	item_num = i_ptr->number;
	flag = FALSE;
	typ  = i_ptr->tval;
	subt = i_ptr->sval;
	if (subt >= ITEM_SINGLE_STACK_MIN) { /* try to stack in store's inven */
	    do {
		j_ptr = &st_ptr->store_item[item_val];
		if (typ == j_ptr->tval)
		{
		    if (subt == j_ptr->sval && /* Adds to other item        */
			subt >= ITEM_SINGLE_STACK_MIN
			&& (subt < ITEM_GROUP_MIN || j_ptr->pval == i_ptr->pval))
		    {
			stacked = TRUE; /* remember that we did stack it... -CFT */
			*ipos = item_val;
			j_ptr->number += item_num;
			/* must set new scost for group items, do this only for items
			   strictly greater than group_min, not for torches, this
			   must be recalculated for entire group */
			if (subt > ITEM_GROUP_MIN)
			{
			    (void) sell_price (&icost, &dummy, j_ptr);
			    st_ptr->store_item[item_val].scost = -icost;
			}
			/* must let group objects (except torches) stack over 24
			   since there may be more than 24 in the group */
			else if (j_ptr->number > 24)
			    j_ptr->number = 24;
			flag = TRUE;
		    }
		}
		item_val ++;
	    } while (!stacked && (item_val < st_ptr->store_ctr));
	} /* if might stack... -CFT */
	if (!stacked) {		/* either never stacks, or didn't find a place to stack */
	    item_val = 0;
	    do {
		j_ptr = &st_ptr->store_item[item_val];
		if ((typ > j_ptr->tval) || /* sort by desc tval, */
		    ((typ == j_ptr->tval) &&
		     ((i_ptr->level < j_ptr->level) || /* then by inc level, */
		      ((i_ptr->level == j_ptr->level) &&
		       (subt < j_ptr->sval))))) /* and finally by inc sval -CFT */
		{		/* Insert into list             */
		    insert_store(item_val, icost, i_ptr);
		    flag = TRUE;
		    *ipos = item_val;
		}
		item_val++;
	    } while ((item_val < st_ptr->store_ctr) && (!flag));
	} /* if didn't already stack it... */
	if (!flag)		/* Becomes last item in list    */
	{
	    insert_store((int)st_ptr->store_ctr, icost, i_ptr);
	    *ipos = st_ptr->store_ctr - 1;
	}
    }
}


/*
 * Insert INVEN_MAX at given location	
 */
static void insert_store(int pos, s32b icost, inven_type *i_ptr)
{
    register int        i;

    for (i = st_ptr->store_ctr - 1; i >= pos; i--)
	st_ptr->store_item[i + 1] = st_ptr->store_item[i];
    st_ptr->store_item[pos] = *i_ptr;
    st_ptr->store_item[pos].scost = (-icost);
    st_ptr->store_ctr++;
}


/*
 * Destroy an item in the stores inventory.  Note that if
 * "one_of" is false, an entire slot is destroyed	-RAK-	
 */
void store_destroy(int item_val, int one_of)
{
    register int         j, number;
    register inven_type *i_ptr;

    i_ptr = &st_ptr->store_item[item_val];

/* for single stackable objects, only destroy one half on average, this will
 * help ensure that general store and alchemist have reasonable selection of
 * objects 
 */
    if ((i_ptr->sval >= ITEM_SINGLE_STACK_MIN) &&
	(i_ptr->sval <= ITEM_SINGLE_STACK_MAX)) {
	if (one_of)
	    number = 1;
	else
	    number = randint((int)i_ptr->number);
    } else
	number = i_ptr->number;

    if (number != i_ptr->number)
	i_ptr->number -= number;
    else {
	for (j = item_val; j < st_ptr->store_ctr - 1; j++)
	    st_ptr->store_item[j] = st_ptr->store_item[j + 1];
	invcopy(&st_ptr->store_item[st_ptr->store_ctr - 1], OBJ_NOTHING);
	st_ptr->store_item[st_ptr->store_ctr - 1].scost = 0;
	st_ptr->store_ctr--;
    }
}


/*
 * Creates a random item and gives it to a store
 */
static void store_create(void)
{
    int			i, tries;
    int                  cur_pos, dummy;
    inven_type		*i_ptr;

    tries = 0;
    cur_pos = i_pop();

    object_level = OBJ_TOWN_LEVEL;
    do {
	if (store_num != 6) {
	    i = store_choice[store_num][randint(STORE_CHOICES) - 1];
	    invcopy(&i_list[cur_pos], i);
	    apply_magic(&i_list[cur_pos], OBJ_TOWN_LEVEL, FALSE, TRUE);
	    i_ptr = &i_list[cur_pos];
	    if (store_check_num(i_ptr)) {
		if ((i_ptr->cost > 0) &&	/* Item must be good	 */
		    (i_ptr->cost < owners[st_ptr->owner].max_cost)) {

/* equivalent to calling ident_spell(), except will not change the object_ident array */
		    store_bought(i_ptr);
		    special_offer(i_ptr);
		    store_carry(&dummy, i_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	} else {
	    i = get_obj_num(40, FALSE);
	    invcopy(&i_list[cur_pos], i);
	    apply_magic(&i_list[cur_pos], 40, FALSE, TRUE);
	    i_ptr = &i_list[cur_pos];
	    if (store_check_num(i_ptr)) {
		if (i_ptr->cost > 0) {	/* Item must be good	 */
		/*
		 * equivalent to calling ident_spell(), except will not
		 * change the object_ident array 
		 */
		    store_bought(i_ptr);
		    special_offer(i_ptr);
		    store_carry(&dummy, i_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	}
    }
    while (tries <= 3);
    pusht(cur_pos);
}



/*
 * eliminate need to bargain if player has haggled well in the past
 */
static int noneedtobargain(s32b minprice)
{
    register int         flagnoneed;

    /* Allow haggling to be turned off */
    if (no_haggle_flag) return (TRUE);

    flagnoneed = ((st_ptr->good_buy == MAX_SHORT)
		  || ((st_ptr->good_buy - 3 * st_ptr->bad_buy) > (5 + (minprice/50))));
    return (flagnoneed);
}


/*
 * update the bargain info					-DJB- 
 */
static void updatebargain(s32b price, s32b minprice)
{

    /* Ignore cheap items */
    if (minprice < 10) return;

    /* Count the successful haggles */
    if (price == minprice) {
	if (st_ptr->good_buy < MAX_SHORT) {
	    st_ptr->good_buy++;
	}
    }

    /* Count the failed haggles */
    else {
	if (st_ptr->bad_buy < MAX_SHORT) {
	    st_ptr->bad_buy++;
	}
    }
}



/*
 * Displays the set of commands
 */
static void display_commands(void)
{
    /* Display the legal commands */
    prt("You may:", 20, 0);
    if (store_num == 7) {
	prt(" g) Get an item.               b) Browse through your home.", 21, 0);
	prt(" d) Drop an item.              i/e/t/w/x) Inventory/Equipment Lists.",
	    22, 0);
	prt("ESC) Exit from your home.      ^R) Redraw the screen.", 23, 0);
    } else {
	prt(" p) Purchase an item.          b) Browse store's inventory.", 21, 0);
	prt(" s) Sell an item.              i/e/t/w/x) Inventory/Equipment Lists.",
	    22, 0);
	prt("ESC) Exit from Building.       ^R) Redraw the screen.", 23, 0);
    }
}


/*
 * Displays the set of commands				-RAK-	 
 */
static void haggle_commands(int typ)
{
    if (typ == -1) {
	prt("Specify an asking-price in gold pieces.", 21, 0);
    }
    else {
	prt("Specify an offer in gold pieces.", 21, 0);
    }

    prt("ESC) Quit Haggling.", 22, 0);

    erase_line(23, 0);		   /* clear last line */
}


/*
 * Displays a store's inventory			-RAK-	
 */
static void display_inventory(int start)
{
    register inven_type *i_ptr;
    register int         i, j, stop;
    bigvtype             out_val1, out_val2;
    s32b                x;

    i = (start % 12);
    stop = ((start / 12) + 1) * 12;
    if (stop > st_ptr->store_ctr)
	stop = st_ptr->store_ctr;
    while (start < stop) {
	i_ptr = &st_ptr->store_item[start];
	x = i_ptr->number;
	if (store_num != 7) {
	    if ((i_ptr->sval >= ITEM_SINGLE_STACK_MIN)
		&& (i_ptr->sval <= ITEM_SINGLE_STACK_MAX))
		i_ptr->number = 1;
	}
	objdes(out_val1, i_ptr, TRUE);
	i_ptr->number = x;
	(void)sprintf(out_val2, "%c) %s", 'a' + i, out_val1);
	prt(out_val2, i + 5, 0);
	if (store_num != 7) {
	    x = st_ptr->store_item[start].scost;
	    if (x < 0) {
		s32b               value = (s32b)(-x);
		
		value = value * chr_adj() / 100;
		if (value <= 0)
		    value = 1;
		(void)sprintf(out_val2, "%9ld", (long) value);
	    } else {
		(void)sprintf(out_val2, "%9ld [Fixed]", (long) x);
	    }
	    prt(out_val2, i + 5, 59);
	}
	i++;
	start++;
    }
    if (i < 12)
	for (j = 0; j < (11 - i + 1); j++)
	    erase_line(j + i + 5, 0);	/* clear remaining lines */
    if (st_ptr->store_ctr > 12)
	put_str("- cont. -", 17, 60);
    else
	erase_line(17, 60);
}


/* Re-displays only a single cost			-RAK-	 */
static void display_cost(int pos)
{
    register int         i;
    register s32b       j;
    vtype                out_val;

    i = (pos % 12);
    if (st_ptr->store_item[pos].scost < 0) {
	j = (- (st_ptr->store_item[pos]).scost);
	j = j * chr_adj() / 100;
	(void)sprintf(out_val, "%ld", (long) j);
    } else
	(void)sprintf(out_val, "%9ld [Fixed]",
		      (long) (st_ptr->store_item[pos]).scost);
    prt(out_val, i + 5, 59);
}


/*
 * Displays players gold					-RAK-	 
 */
static void store_prt_gold(void)
{
    vtype out_val;

    (void)sprintf(out_val, "Gold Remaining : %ld", (long)p_ptr->au);
    prt(out_val, 18, 17);
}


/*
 * Displays store (after clearing screen)		-RAK-	 
 */
static void display_store(int cur_top)
{
    /* Erase the screen */
    clear_screen();

	/* Put the owner name */
	put_str(owners[st_ptr->owner].owner_name, 3, 9);
    put_str("Item", 4, 3);
    if (store_num != 7) {
	put_str("Asking Price", 4, 60);
	store_prt_gold();
    }
    display_commands();
    display_inventory(cur_top);
}



/*
 * Get the ID of a store item and return it's value	-RAK-	 
 */
static int get_store_item(int *com_val, cptr pmt, int i, int j)
{
    char         command;
    vtype        out_val;

    *com_val = (-1);

    /* Build the prompt */
    (void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s",
		  i + 'a', j + 'a', pmt);

    /* Ask until done */
    while (TRUE) {

	/* Escape */
	if (!get_com(out_val, &command)) break;

	/* Legal responses */
	if (command >= i+'a' && command <= j+'a') {
	    *com_val = command - 'a';
	    break;
	}

	/* Oops */
	bell();
    }

    erase_line(MSG_LINE, 0);

    return (command != ESCAPE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-	 
 */
static int increase_insults(void)
{
    st_ptr->insult_cur++;

    if (st_ptr->insult_cur > owners[st_ptr->owner].insult_max) {
	prt_comment4();
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;
	st_ptr->store_open = turn + 2500 + randint(2500);
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Decrease insults					-RAK-	 
 */
static void decrease_insults(void)
{
    if (st_ptr->insult_cur != 0) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling				-RAK-	 
 */
static int haggle_insults(void)
{
    /* Increase insults */
    if (increase_insults()) return (TRUE);

    /* Display and flush insult */
    prt_comment5();
    msg_print(NULL);

    /* Still okay */
    return (FALSE);
}


/*
 * Get a haggle
 */
static int get_haggle(cptr comment, s32b *new_offer,
		      int num_offer, s32b price, int final)
{
    register s32b      i;
    vtype               out_val;
    char                buf[100];
    register int        flag, clen;
    register char      *p;
    static s32b        last_inc = 0L;
    int                 inc = FALSE;

    flag = TRUE;
    if (last_inc && !final) {
	(void)sprintf(buf, "%s [%c%ld] ", comment,
			(last_inc < 0) ? '-' : '+', (last_inc < 0) ? (long)-last_inc : (long)last_inc);
    }
    else {
	(void)sprintf(buf, "%s [accept] ", comment);
    }

    clen = strlen(buf);
    i = 0;
    do {
	prt(buf, 0, 0);
	if (!get_string(out_val, 0, clen, 40))
	    flag = FALSE, last_inc = FALSE;
	i = atol(out_val);
    /*
     * don't allow incremental haggling, if player has not made an offer yet 
     */
	for (p = out_val; *p == ' '; p++);
	if (flag && num_offer == 0 && (*p == '+' || *p == '-')) {
	    msg_print("You haven't even made your first offer yet!");
	    i = 0;
	}
	if (last_inc && !i && !final)
	    i = last_inc, inc = TRUE;
	else if (!i)
	    i = price;
    } while (flag && !i);
    if (flag) {
	for (p = out_val; *p == ' '; p++);
	if (*p == '+' || *p == '-')
	    *new_offer += i, last_inc = i;
	else if (inc)
	    *new_offer += i, last_inc = i;
	else
	    *new_offer = i, last_inc = 0;
    } else
	erase_line(0, 0);
    return (flag);
}


/*
 * Receive an offer (from the player)
 */
static int receive_offer(cptr comment, s32b *new_offer,
			 s32b last_offer, int num_offer, int factor,
			 s32b price, int final)
{
    register int flag, receive;

    receive = 0;

    for (flag = FALSE; !flag; ) {
	if (get_haggle(comment, new_offer, num_offer, price, final)) {
	    if (*new_offer * factor >= last_offer * factor) {
		flag = TRUE;
	    }
	    else if (haggle_insults()) {
		receive = 2;
		flag = TRUE;
	    }
	    else {
		/* new_offer rejected, reset so that */
		/* incremental haggling works correctly */
		(*new_offer) = last_offer;
	    }
	}
	else {
	    receive = 1;
	    flag = TRUE;
	}
    }

    return (receive);
}


/*
 * Haggling routine					-RAK-	 
 */
static int purchase_haggle(s32b *price, inven_type *i_ptr)
{
    s32b               max_sell, min_sell, max_buy;
    s32b               cost, cur_ask, final_ask, min_offer;
    s32b               last_offer, new_offer;
    s32b               x1, x2, x3;
    s32b               min_per, max_per;
    register int        flag, loop_flag;
    const char         *comment;
    vtype               out_val;
    int                 purchase, num_offer, final_flag, final = FALSE;

    flag = FALSE;
    purchase = 0;
    *price = 0;
    final_flag = 0;

    /* Determine the cost of the group of items */
    cost = sell_price(&max_sell, &min_sell, i_ptr);

    max_sell = max_sell * chr_adj() / 100;
    if (max_sell <= 0) max_sell = 1;

    min_sell = min_sell * chr_adj() / 100;
    if (min_sell <= 0) min_sell = 1;

    /* XXX This appears to be a hack.  See sell_price(). */
    /* cast max_inflate to signed so that subtraction works correctly */
    max_buy = cost * (200 - (int)ot_ptr->max_inflate) / 100;
    if (max_buy <= 0) max_buy = 1;

    min_per = ot_ptr->haggle_per;
    max_per = min_per * 3;

    haggle_commands(1);

    cur_ask = max_sell;
    final_ask = min_sell;
    min_offer = max_buy;
    last_offer = min_offer;
    new_offer = 0;

    /* this prevents incremental haggling on first try */
    num_offer = 0;
    comment = "Asking";

    /* Go right to final price if player has bargained well */
    if (noneedtobargain(final_ask)) {

	msg_print("After a long bargaining session, you agree upon the price.");
	msg_print(NULL);

	/* not haggling penalty -CWS */
	if (no_haggle_flag && min_sell > 10) {
	    min_sell *= 1.10;
	}

	cur_ask = min_sell;

    }
    do {
	do {
	    loop_flag = TRUE;
	    (void)sprintf(out_val, "%s :  %ld", comment, (long)cur_ask);
	    put_str(out_val, 1, 0);
	    purchase = receive_offer("What do you offer? ",
				     &new_offer, last_offer, num_offer,
				     1, cur_ask, final);
	    if (purchase != 0) {
		flag = TRUE;
	    }
	    else {
		if (new_offer > cur_ask) {
		    prt_comment6();
		    /* rejected, reset new_offer for incremental haggling */
		    new_offer = last_offer;
		}
		else if (new_offer == cur_ask) {
		    flag = TRUE;
		    *price = new_offer;
		}
		else {
		    loop_flag = FALSE;
		}
	    }
	}
	while (!flag && loop_flag);

	if (!flag) {
	    x1 = (new_offer - last_offer) * 100 / (cur_ask - last_offer);
	    if (x1 < min_per) {
		flag = haggle_insults();
		if (flag) purchase = 2;
	    }
	    else if (x1 > max_per) {
		x1 = x1 * 75 / 100;
		if (x1 < max_per) x1 = max_per;
	    }
	    x2 = x1 + randint(5) - 3;
	    x3 = ((cur_ask - new_offer) * x2 / 100) + 1;
	    /* don't let the price go up */
	    if (x3 < 0) x3 = 0;
	    cur_ask -= x3;

	    if (cur_ask < final_ask) {
		final = TRUE;
		cur_ask = final_ask;
		comment = "Final Offer";
		final_flag++;
		if (final_flag > 3) {
		    if (increase_insults()) purchase = 2;
		    else purchase = 1;
		    flag = TRUE;
		}
	    }
	    else if (new_offer >= cur_ask) {
		flag = TRUE;
		*price = new_offer;
	    }
	    if (!flag) {
		last_offer = new_offer;
		num_offer++;	   /* enable incremental haggling */
		erase_line(1, 0);
		(void)sprintf(out_val, "Your last offer : %ld",
			      (long)last_offer);
		put_str(out_val, 1, 39);
		prt_comment2(last_offer, cur_ask, final_flag);
	    }
	}
    }
    while (!flag);

    /* update bargaining info */
    if (purchase == 0) updatebargain(*price, final_ask);

    return (purchase);
}


/*
 * Haggling routine					-RAK-	 
 */
static int sell_haggle(s32b *price, inven_type *i_ptr)
{
    s32b               max_sell = 0, max_buy = 0, min_buy = 0;
    s32b               cost = 0, cur_ask = 0, final_ask = 0, min_offer = 0;
    s32b               last_offer = 0, new_offer = 0;
    s32b               max_gold = 0;
    s32b               x1, x2, x3;
    s32b               min_per, max_per;
    register int        flag, loop_flag;
    const char          *comment;
    vtype               out_val;
    int                 sell, num_offer, final_flag, final = FALSE;

    flag = FALSE;
    sell = 0;
    *price = 0;
    final_flag = 0;

    cost = item_value(i_ptr);
    if (cost < 1) {
	sell = 3;
	flag = TRUE;
    } else {

    cost = cost * (200 - chr_adj()) / 100;
    cost = cost * (200 - rgold_adj[ot_ptr->owner_race][p_ptr->prace]) / 100;

    if (cost < 1) cost = 1;
    max_sell = cost * ot_ptr->max_inflate / 100;

    /* cast max_inflate to signed so that subtraction works correctly */
    max_buy = cost * (200 - (int)ot_ptr->max_inflate) / 100;
    min_buy = cost * (200 - (int)ot_ptr->min_inflate) / 100;
    if (min_buy < 1) min_buy = 1;
    if (max_buy < 1)  max_buy = 1;

    if (min_buy < max_buy) min_buy = max_buy;
    min_per = ot_ptr->haggle_per;
    max_per = min_per * 3;
    max_gold = ot_ptr->max_cost;
    }
    if (!flag) {
    haggle_commands(-1);

    if (max_buy > max_gold) {
	final_flag = 1;
	final = TRUE;
	comment = "Final Offer";
	cur_ask = max_gold;
	final_ask = max_gold;
	msg_print("I am sorry, but I have not the money to afford such a fine item.");
    }
    else {
	cur_ask = max_buy;
	final_ask = min_buy;
	if (final_ask > max_gold) final_ask = max_gold;
	comment = "Offer";

	/* go right to final price if player has bargained well */
	if (noneedtobargain(final_ask)) {

	    msg_print("After a long bargaining session, you agree upon the price.");
	    msg_print(NULL);

	    /* not haggling penalty -CWS */
	    if (no_haggle_flag && final_ask > 10) {
		final_ask *= .90;
	    }

	    cur_ask = final_ask;

	}
    }


    min_offer = max_sell;
    last_offer = min_offer;
    new_offer = 0;

    /* this prevents incremental haggling on first try */
    num_offer = 0;

    if (cur_ask < 1) cur_ask = 1;

	do {
	    do {
	    loop_flag = TRUE;
	    (void)sprintf(out_val, "%s :  %ld", comment, (long)cur_ask);
	    put_str(out_val, 1, 0);
	    sell = receive_offer("What price do you ask? ",
				 &new_offer, last_offer, num_offer,
				 -1, cur_ask, final);
	    if (sell != 0) {
		flag = TRUE;
	    }
	    else {
		if (new_offer < cur_ask) {
		    prt_comment6();
		    /* rejected, reset new_offer for incremental haggling */
		    new_offer = last_offer;
		}
		else if (new_offer == cur_ask) {
		    flag = TRUE;
		    *price = new_offer;
		}
		else {
		    loop_flag = FALSE;
		}
	    }
	}
	while (!flag && loop_flag);

	if (!flag) {
	    x1 = (last_offer - new_offer) * 100 / (last_offer - cur_ask);
	    if (x1 < min_per) {
		flag = haggle_insults();
		if (flag) sell = 2;
	    }
	    else if (x1 > max_per) {
		x1 = x1 * 75 / 100;
		if (x1 < max_per) x1 = max_per;
	    }
	    x2 = x1 + randint(5) - 3;
	    x3 = ((new_offer - cur_ask) * x2 / 100) + 1;
	    /* don't let the price go down */
	    if (x3 < 0) x3 = 0;
	    cur_ask += x3;
	    if (cur_ask > final_ask) {
		cur_ask = final_ask;
		final = TRUE;
		comment = "Final Offer";
		final_flag++;
		if (final_flag > 3) {
		if (increase_insults()) sell = 2;
			else sell = 1;
			flag = TRUE;
		}
	    }
	    else if (new_offer <= cur_ask) {
		flag = TRUE;
		*price = new_offer;
	    }
	    if (!flag) {
		last_offer = new_offer;
		num_offer++;   /* enable incremental haggling */
		erase_line(1, 0);
		(void)sprintf(out_val,
			     "Your last bid %ld", (long)last_offer);
		put_str(out_val, 1, 39);
		prt_comment3(cur_ask, last_offer, final_flag);
	    }
	}
    }
	while (!flag);
    }

    /* update bargaining info */
    if (sell == 0) updatebargain(*price, final_ask);

    return (sell);
}





/*
 * Buy an item from a store				-RAK-	 
 */
static int store_purchase(int *cur_top)
{
    s32b               price;
    register int        i, choice;
    bigvtype            out_val, tmp_str;
    inven_type          sell_obj;
    register inven_type *r_ptr;
    int                 item_val, item_new, purchase;

    purchase = FALSE;

/* i == number of objects shown on screen	 */
    if (*cur_top == 12)
	i = st_ptr->store_ctr - 1 - 12;
    else if (st_ptr->store_ctr > 11)
	i = 11;
    else
	i = st_ptr->store_ctr - 1;
    if (st_ptr->store_ctr < 1) {
	if (store_num == 7)
	    msg_print("Your home is empty.");
	else
	    msg_print("I am currently out of stock.");
    }
/* Get the item number to be bought		 */
    else if (get_store_item(&item_val,
			    store_num == 7 ? "Which item do you want to take? " :
			    "Which item are you interested in? ", 0, i)) {
	item_val = item_val + *cur_top;	/* TRUE item_val	 */
	take_one_item(&sell_obj, &st_ptr->store_item[item_val]);
	if (inven_check_num(&sell_obj)) {
	    if (store_num != 7) {
		if (st_ptr->store_item[item_val].scost > 0) {
		    price = st_ptr->store_item[item_val].scost;
		    choice = 0;
		} else
		    choice = purchase_haggle(&price, &sell_obj);
		if (choice == 0) {
		    if (p_ptr->au >= price) {
			prt_comment1();
			decrease_insults();
			p_ptr->au -= price;
			item_new = inven_carry(&sell_obj);
			i = st_ptr->store_ctr;
			store_destroy(item_val, TRUE);
			inventory[item_new].inscrip[0] = 0;
			objdes(tmp_str, &inventory[item_new], TRUE);
			(void)sprintf(out_val, "You have %s (%c)",
				      tmp_str, item_new + 'a');
			prt(out_val, 0, 0);
			check_strength();
			if (*cur_top >= st_ptr->store_ctr) {
			    *cur_top = 0;
			    display_inventory(*cur_top);
			} else {
			    r_ptr = &st_ptr->store_item[item_val];
			    if (i == st_ptr->store_ctr) {
				if (r_ptr->scost < 0) {
				    r_ptr->scost = price;
				    display_cost(item_val);
				}
			    } else
				display_inventory(item_val);
			    store_prt_gold();
			}
		    } else {
			if (increase_insults())
			    purchase = TRUE;
			else {
			    prt_comment1();
			    msg_print("Liar!  You have not the gold!");
			}
		    }
		} else if (choice == 2)
		    purchase = TRUE;
	    } else {		   /* is_home... */
		item_new = inven_carry(&sell_obj);
		i = st_ptr->store_ctr;
		store_destroy(item_val, TRUE);
		objdes(tmp_str, &inventory[item_new], TRUE);
		(void)sprintf(out_val, "You have %s (%c)",
			      tmp_str, item_new + 'a');
		prt(out_val, 0, 0);
		check_strength();
		if (*cur_top >= st_ptr->store_ctr) {
		    *cur_top = 0;
		    display_inventory(*cur_top);
		} else {
		    r_ptr = &st_ptr->store_item[item_val];

#if 0
			if (i == st_ptr->store_ctr) {
				if (r_ptr->scost < 0) {
					r_ptr->scost = price;
					display_cost(item_val);
				}
			} else 
#endif
		    display_inventory(item_val);
		}
	    }
	/* Less intuitive, but looks better here than in purchase_haggle. */
	    display_commands();
	    erase_line(1, 0);
	} else
	    prt("You cannot carry that many different items.", 0, 0);
    }
    return (purchase);
}


/*
 * Sell an item to the store	-RAK-	 
 */
static int store_sell(int *cur_top)
{
    int                 item_val, item_pos;
    s32b               price;
    bigvtype            out_val, tmp_str;
    inven_type          sold_obj;
    int			sell, choice, test;

    sell = FALSE;
    for (item_val = 0, test = FALSE; (!test && (item_val < inven_ctr)); item_val++)
	test = (*store_buy[store_num]) (inventory[item_val].tval);

    /* Check for stuff */
    if (inven_ctr < 1) {
	msg_print("You aren't carrying anything.");
    }

    /* Check for stuff */
    else if (!test) {
	msg_print("You have nothing that I want.");
    }

    else if (get_item(&item_val, "Which one? ", 0,
		      inven_ctr - 1, (store_buy[store_num]))) {
	take_one_item(&sold_obj, &inventory[item_val]);
	objdes(tmp_str, &sold_obj, TRUE);
	if (store_num != 7) {
	    (void)sprintf(out_val, "Selling %s (%c)", tmp_str, item_val + 'a');
	    msg_print(out_val);
	}
	if ((*store_buy[store_num]) (sold_obj.tval))
	    if (store_check_num(&sold_obj)) {
		if (store_num != 7) {
		    choice = sell_haggle(&price, &sold_obj);
		    if (choice == 0) {
			s32b               cost;
			s32b               dummy;

			prt_comment1();
			decrease_insults();
			p_ptr->au += price;

			cost = item_value(&sold_obj);

		    /* identify object in inventory to set object_ident */
			identify(&item_val);
		    /* retake sold_obj so that it will be identified */
			take_one_item(&sold_obj, &inventory[item_val]);
		    /* call known2 for store item, so charges/pluses are known */
			known2(&sold_obj);

			dummy = item_value(&sold_obj);

			inven_destroy(item_val);
			objdes(tmp_str, &sold_obj, TRUE);
			(void)sprintf(out_val, "You've sold %s. ", tmp_str);
			msg_print(out_val);

			if (dummy == 0) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear a shriek!");
				break;
			      case 2:
				msg_print("You bastard!");
				break;
			      case 3:
				msg_print("You hear sobs coming from the back of the store...");
				break;
			      case 4:
				msg_print("Arrgghh!!!!");
				break;
			    }
			} else if (dummy < cost) {
			    switch (randint(3)) {
			      case 1:
				msg_print("You hear someone swearing...");
				break;
			      case 2:
				msg_print("You hear mumbled curses...");
				break;
			      case 3:
				msg_print("The shopkeeper glares at you.");
				break;
			    }
			} else if (dummy > (4 * cost)) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear someone jumping for joy!");
				break;
			      case 2:
				msg_print("Yipee!");
				break;
			      case 3:
				msg_print("I think I'll retire!");
				break;
			      case 4:
				msg_print("The shopkeeper smiles gleefully!");
				break;
			    }
			} else if (dummy > cost) {
			    switch (randint(4)) {
			      case 1:
				msg_print("You hear someone giggling");
				break;
			      case 2:
				msg_print("You've made my day!");
				break;
			      case 3:
				msg_print("What a fool!");
				break;
			      case 4:
				msg_print("The shopkeeper laughs loudly!");
				break;
			    }
			}
			store_carry(&item_pos, &sold_obj);
			check_strength();
			if (item_pos >= 0) {
			    if (item_pos < 12)
				if (*cur_top < 12)
				    display_inventory(item_pos);
				else {
				    *cur_top = 0;
				    display_inventory(*cur_top);
				}
			    else if (*cur_top > 11)
				display_inventory(item_pos);
			    else {
				*cur_top = 12;
				display_inventory(*cur_top);
			    }
			}
			store_prt_gold();
		    } else if (choice == 2)
			sell = TRUE;
		    else if (choice == 3) {
			msg_print("How dare you!");
			msg_print("I will not buy that!");
			sell = increase_insults();
		    }
		/* Less intuitive, but looks better here than in sell_haggle. */
		    erase_line(1, 0);
		    display_commands();
		} else {	   /* is_home... */
		/* retake sold_obj so that it will be identified */
		    take_one_item(&sold_obj, &inventory[item_val]);
		    inven_destroy(item_val);
		    objdes(tmp_str, &sold_obj, TRUE);
		    (void)sprintf(out_val, "You drop %s", tmp_str);
		    msg_print(out_val);
		    store_carry(&item_pos, &sold_obj);
		    check_strength();
		    if (item_pos >= 0) {
			if (item_pos < 12)
			    if (*cur_top < 12)
				display_inventory(item_pos);
			    else {
				*cur_top = 0;
				display_inventory(*cur_top);
			    }
			else if (*cur_top > 11)
			    display_inventory(item_pos);
			else {
			    *cur_top = 12;
			    display_inventory(*cur_top);
			}
		    }
		/* Less intuitive, but looks better here than in sell_haggle. */
		    erase_line(1, 0);
		    display_commands();
		}
	    } else {
		if (store_num == 7)
		    msg_print("Your home is full.");
		else
		    msg_print("I have not the room in my store to keep it.");
	    }
	else {
	    if (store_num == 7)
		msg_print("Whoops! Cock up.");
	    else
		msg_print("I do not buy such items.");
	}
    }

    /* Return result */
    return (sell);
}


/*
 * Enter a store, and interact with it.
 */
void enter_store(int which)
{
    int                  cur_top, tmp_chr;
    char                 command;
    register int         exit_flag;


    /* Check the "locked doors" */
    if (store[which].store_open >= turn) {
	msg_print("The doors are locked.");
	return;
    }


    /* Hack -- note we are in a store */
    in_store_flag = TRUE;


    /* Save the store number */
    store_num = which;

    /* Save the store and owner pointers */
    st_ptr = &store[store_num];
    ot_ptr = &owners[st_ptr->owner];


    /* Start at the beginning */
    cur_top = 0;

    /* Display the store */
    display_store(cur_top);

    /* Interact with player */
    for (exit_flag = FALSE; !exit_flag; ) {

	    move_cursor(20, 9);

	/* Assume player has read his messages */
	msg_flag = FALSE;

	    if (get_com(NULL, &command)) {
	
	/* Check the charisma */
	tmp_chr = p_ptr->use_stat[A_CHR];
	
	/* Process the command */
	switch (command) {

	    case 'b':
		if (st_ptr->store_ctr <= 12) {
		    msg_print("Entire inventory is shown.");
		}
		else {
		    cur_top += 12;
		    if (cur_top >= st_ptr->store_ctr) cur_top = 0;
		    display_inventory(cur_top);
		}
		break;

	    case 'E': case 'e':	   /* Equipment List	 */
	    case 'I': case 'i':	   /* Inventory		 */
	    case 'T': case 't':	   /* Take off		 */
	    case 'W': case 'w':	   /* Wear			 */
	    case 'X': case 'x':	   /* Switch weapon		 */
		inven_command(command);
		break;

	    case 'g':
		if (store_num != 7) bell();
		else exit_flag = store_purchase(&cur_top);
		break;

	    case 'p':
		if (store_num == 7) bell();
		else exit_flag = store_purchase(&cur_top);
		break;

	    case 's':
		if (store_num == 7)bell();
		else exit_flag = store_sell(&cur_top);
		break;

	    case 'd':
		if (store_num != 7) bell();
		else exit_flag = store_sell(&cur_top);
		break;

	    default:
		bell();
		break;
	}

	/* Redisplay store prices if charisma changes */
	if (tmp_chr != p_ptr->use_stat[A_CHR]) {
	    display_inventory(cur_top);
	}

	    } else
		exit_flag = TRUE;
	}


    /* Forget the store number, etc */
    store_num = 0;
    st_ptr = NULL;
    ot_ptr = NULL;    


    /* Hack -- turn off the flag */
    in_store_flag = FALSE;

    /* Hack -- No free moves here. -CJS- */
    free_turn_flag = FALSE;
    

    /* Hack -- Redraw the player stats and the map */
    draw_cave();
}


/* 
 * Maintain the inventory at the stores.
 * Initialize and up-keep the store's inventory.		-RAK-	
 */
void store_maint(void)
{
    register int         i, j;


    /* Maintain every store (except the home) */
    for (i = 0; i < (MAX_STORES - 1); i++) {

	/* Save the store index */
	store_num = i;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Activate the new owner */
	ot_ptr = &owners[st_ptr->owner];


	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;


	if (st_ptr->store_ctr >= STORE_MIN_INVEN) {
	    j = randint(STORE_TURN_AROUND);
	    if (st_ptr->store_ctr >= STORE_MAX_INVEN)
		j += 1 + st_ptr->store_ctr - STORE_MAX_INVEN;
	    while (--j >= 0)
		store_destroy(randint((int)st_ptr->store_ctr) - 1, FALSE);
	}
	if (st_ptr->store_ctr <= STORE_MAX_INVEN) {
	    j = randint(STORE_TURN_AROUND);
	    if (st_ptr->store_ctr < STORE_MIN_INVEN)
		j += STORE_MIN_INVEN - st_ptr->store_ctr;
	    while (--j >= 0)
		store_create();
	}
    }


    /* Turn it all off */
    store_num = 0;
    st_ptr = NULL;
    ot_ptr = NULL;
}


/*
 * Initialize the stores
 */
void store_init(void)
{
    register int         j, k;

    /* Build each store */
    for (j = 0; j < MAX_STORES; j++) {

	/* Save the store index */
	store_num = j;

	/* Activate that store */
	st_ptr = &store[store_num];


	/* Pick an owner */
	st_ptr->owner = MAX_STORES * rand_int(MAX_OWNERS / MAX_STORES) + j;

	/* Activate the new owner */
	ot_ptr = &owners[st_ptr->owner];


	/* Initialize the store */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->store_ctr = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* No items yet */
	for (k = 0; k < STORE_INVEN_MAX; k++) {
	    invcopy(&st_ptr->store_item[k], OBJ_NOTHING);
	    st_ptr->store_item[k].scost = 0;
	}
    }


    /* Turn it all off */
    store_num = 0;
    st_ptr = NULL;
    ot_ptr = NULL;
}

