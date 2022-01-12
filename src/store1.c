/*
 * store1.c: store code, updating store inventory, pricing objects 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static void special_offer(inven_type *);
static void insert_store(int, int, s32b, struct inven_type *);
static void store_create(int);





/* Check to see if he will be carrying too many objects	-RAK-	 */
int store_check_num(inven_type *t_ptr, int store_num)
{
    register int        store_check, i;
    register store_type *s_ptr;
    register inven_type *i_ptr;

    store_check = FALSE;
    s_ptr = &store[store_num];
    if (s_ptr->store_ctr < STORE_INVEN_MAX)
	store_check = TRUE;
    else if (t_ptr->sval >= ITEM_SINGLE_STACK_MIN)
	for (i = 0; i < s_ptr->store_ctr; i++) {
	    i_ptr = &s_ptr->store_inven[i].sitem;

	/* note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match 
	 */
	    if (i_ptr->tval == t_ptr->tval && i_ptr->sval == t_ptr->sval
		&& ((int)i_ptr->number + (int)t_ptr->number < 256)
		&& (t_ptr->sval < ITEM_GROUP_MIN
		    || (i_ptr->p1 == t_ptr->p1)))
		store_check = TRUE;
	}

/* But, wait.  If at home, don't let player drop 25th item, or he will lose it. -CFT */
    if (is_home && (t_ptr->sval >= ITEM_SINGLE_STACK_MIN))
	for (i = 0; i < s_ptr->store_ctr; i++) {
	    i_ptr = &s_ptr->store_inven[i].sitem;
	/*
	 * note: items with sval of gte ITEM_SINGLE_STACK_MAX only stack if
	 * their svals match 
	 */
	    if (i_ptr->tval == t_ptr->tval && i_ptr->sval == t_ptr->sval
		&& ((int)i_ptr->number + (int)t_ptr->number > 24)
		&& (t_ptr->sval < ITEM_GROUP_MIN
		    || (i_ptr->p1 == t_ptr->p1)))
		store_check = FALSE;
	}
    return (store_check);
}


/* Insert INVEN_MAX at given location	 */
static void insert_store(int store_num, int pos, s32b icost, inven_type *i_ptr)
{
    register int        i;
    register store_type *s_ptr;

    s_ptr = &store[store_num];
    for (i = s_ptr->store_ctr - 1; i >= pos; i--)
	s_ptr->store_inven[i + 1] = s_ptr->store_inven[i];
    s_ptr->store_inven[pos].sitem = *i_ptr;
    s_ptr->store_inven[pos].scost = (-icost);
    s_ptr->store_ctr++;
}


/* Add the item in INVEN_MAX to stores inventory.	-RAK-	 */
void store_carry(int store_num, int *ipos, inven_type *t_ptr)
{
    int                 item_num, item_val, flag;
    register int        typ, subt;
    s32b               icost, dummy;
    register inven_type *i_ptr;
    register store_type *s_ptr;
    int stacked = FALSE; /* from inven_carry() -CFT */

    *ipos = -1;
    if (sell_price(store_num, &icost, &dummy, t_ptr) > 0 || is_home)
    {
	s_ptr = &store[store_num];
	item_val = 0;
	item_num = t_ptr->number;
	flag = FALSE;
	typ  = t_ptr->tval;
	subt = t_ptr->sval;
	if (subt >= ITEM_SINGLE_STACK_MIN) { /* try to stack in store's inven */
	    do {
		i_ptr = &s_ptr->store_inven[item_val].sitem;
		if (typ == i_ptr->tval)
		{
		    if (subt == i_ptr->sval && /* Adds to other item        */
			subt >= ITEM_SINGLE_STACK_MIN
			&& (subt < ITEM_GROUP_MIN || i_ptr->p1 == t_ptr->p1))
		    {
			stacked = TRUE; /* remember that we did stack it... -CFT */
			*ipos = item_val;
			i_ptr->number += item_num;
			/* must set new scost for group items, do this only for items
			   strictly greater than group_min, not for torches, this
			   must be recalculated for entire group */
			if (subt > ITEM_GROUP_MIN)
			{
			    (void) sell_price (store_num, &icost, &dummy, i_ptr);
			    s_ptr->store_inven[item_val].scost = -icost;
			}
			/* must let group objects (except torches) stack over 24
			   since there may be more than 24 in the group */
			else if (i_ptr->number > 24)
			    i_ptr->number = 24;
			flag = TRUE;
		    }
		}
		item_val ++;
	    } while (!stacked && (item_val < s_ptr->store_ctr));
	} /* if might stack... -CFT */
	if (!stacked) {		/* either never stacks, or didn't find a place to stack */
	    item_val = 0;
	    do {
		i_ptr = &s_ptr->store_inven[item_val].sitem;
		if ((typ > i_ptr->tval) || /* sort by desc tval, */
		    ((typ == i_ptr->tval) &&
		     ((t_ptr->level < i_ptr->level) || /* then by inc level, */
		      ((t_ptr->level == i_ptr->level) &&
		       (subt < i_ptr->sval))))) /* and finally by inc sval -CFT */
		{		/* Insert into list             */
		    insert_store(store_num, item_val, icost, t_ptr);
		    flag = TRUE;
		    *ipos = item_val;
		}
		item_val++;
	    } while ((item_val < s_ptr->store_ctr) && (!flag));
	} /* if didn't already stack it... */
	if (!flag)		/* Becomes last item in list    */
	{
	    insert_store(store_num, (int)s_ptr->store_ctr, icost, t_ptr);
	    *ipos = s_ptr->store_ctr - 1;
	}
    }
}


/* Destroy an item in the stores inventory.  Note that if	 */
/* "one_of" is false, an entire slot is destroyed	-RAK-	 */
void store_destroy(int store_num, int item_val, int one_of)
{
    register int         j, number;
    register store_type *s_ptr;
    register inven_type *i_ptr;

    s_ptr = &store[store_num];
    i_ptr = &s_ptr->store_inven[item_val].sitem;

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
	for (j = item_val; j < s_ptr->store_ctr - 1; j++)
	    s_ptr->store_inven[j] = s_ptr->store_inven[j + 1];
	invcopy(&s_ptr->store_inven[s_ptr->store_ctr - 1].sitem, OBJ_NOTHING);
	s_ptr->store_inven[s_ptr->store_ctr - 1].scost = 0;
	s_ptr->store_ctr--;
    }
}


/* Initializes the stores with owners			-RAK-	 */
void store_init()
{
    register int         i, j, k;
    register store_type *s_ptr;

    i = MAX_OWNERS / MAX_STORES;
    for (j = 0; j < MAX_STORES; j++) {
	s_ptr = &store[j];
	s_ptr->owner = MAX_STORES * (randint(i) - 1) + j;
	s_ptr->insult_cur = 0;
	s_ptr->store_open = 0;
	s_ptr->store_ctr = 0;
	s_ptr->good_buy = 0;
	s_ptr->bad_buy = 0;
	for (k = 0; k < STORE_INVEN_MAX; k++) {
	    invcopy(&s_ptr->store_inven[k].sitem, OBJ_NOTHING);
	    s_ptr->store_inven[k].scost = 0;
	}
    }
}


/* Creates an item and inserts it into store's inven	-RAK-	 */
static void store_create(int store_num)
{
    register int         i, tries;
    int                  cur_pos, dummy;
    register store_type *s_ptr;
    register inven_type *t_ptr;

    tries = 0;
    cur_pos = i_pop();
    s_ptr = &store[store_num];
    object_level = OBJ_TOWN_LEVEL;
    do {
	if (store_num != 6) {
	    i = store_choice[store_num][randint(STORE_CHOICES) - 1];
	    invcopy(&i_list[cur_pos], i);
	    magic_treasure(cur_pos, OBJ_TOWN_LEVEL, FALSE, TRUE);
	    t_ptr = &i_list[cur_pos];
	    if (store_check_num(t_ptr, store_num)) {
		if ((t_ptr->cost > 0) &&	/* Item must be good	 */
		    (t_ptr->cost < owners[s_ptr->owner].max_cost)) {

/* equivalent to calling ident_spell(), except will not change the object_ident array */
		    store_bought(t_ptr);
		    special_offer(t_ptr);
		    store_carry(store_num, &dummy, t_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	} else {
	    i = get_obj_num(40, FALSE);
	    invcopy(&i_list[cur_pos], i);
	    magic_treasure(cur_pos, 40, FALSE, TRUE);
	    t_ptr = &i_list[cur_pos];
	    if (store_check_num(t_ptr, store_num)) {
		if (t_ptr->cost > 0) {	/* Item must be good	 */
		/*
		 * equivalent to calling ident_spell(), except will not
		 * change the object_ident array 
		 */
		    store_bought(t_ptr);
		    special_offer(t_ptr);
		    store_carry(store_num, &dummy, t_ptr);
		    tries = 10;
		}
	    }
	    tries++;
	}
    }
    while (tries <= 3);
    pusht(cur_pos);
}

static void special_offer(inven_type *i_ptr)
{
    s32b orig_cost = i_ptr->cost;

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

/* Initialize and up-keep the store's inventory.		-RAK-	 */
void store_maint()
{
    register int         i, j;
    register store_type *s_ptr;

    for (i = 0; i < (MAX_STORES - 1); i++) {
	s_ptr = &store[i];
	s_ptr->insult_cur = 0;
	if (s_ptr->store_ctr >= STORE_MIN_INVEN) {
	    j = randint(STORE_TURN_AROUND);
	    if (s_ptr->store_ctr >= STORE_MAX_INVEN)
		j += 1 + s_ptr->store_ctr - STORE_MAX_INVEN;
	    while (--j >= 0)
		store_destroy(i, randint((int)s_ptr->store_ctr) - 1, FALSE);
	}
	if (s_ptr->store_ctr <= STORE_MAX_INVEN) {
	    j = randint(STORE_TURN_AROUND);
	    if (s_ptr->store_ctr < STORE_MIN_INVEN)
		j += STORE_MIN_INVEN - s_ptr->store_ctr;
	    while (--j >= 0)
		store_create(i);
	}
    }
}

/* eliminate need to bargain if player has haggled well in the past   -DJB- */
int noneedtobargain(int store_num, s32b minprice)
{
    register int         flagnoneed;
    register store_type *s_ptr;

    if (no_haggle_flag)
	return (TRUE);

    s_ptr = &store[store_num];
    flagnoneed = ((s_ptr->good_buy == MAX_SHORT)
		  || ((s_ptr->good_buy - 3 * s_ptr->bad_buy) > (5 + (minprice/50))));
    return (flagnoneed);
}


/* update the bargin info					-DJB- */
void updatebargain(int store_num, s32b price, s32b minprice)
{
    register store_type *s_ptr;

    s_ptr = &store[store_num];
    if (minprice > 9)
	if (price == minprice) {
	    if (s_ptr->good_buy < MAX_SHORT)
		s_ptr->good_buy++;
	} else {
	    if (s_ptr->bad_buy < MAX_SHORT)
		s_ptr->bad_buy++;
	}
}

