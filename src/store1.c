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


