/* File: effects.c */

/* Purpose: effects of various "objects" */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * XXX XXX XXX XXX Someone needs to verify all of these effects.
 */



/*
 * Lose a strength point.				-RAK-	 
 */
static void lose_str()
{
    if (!p_ptr->sustain_str) {
	(void)dec_stat(A_STR);
	msg_print("You feel very weak.");
    }
    else {
	msg_print("You feel weak for a moment;  it passes.");
    }
}


/*
 * Lose an intelligence point.				-RAK-	 
 */
static void lose_int()
{
    if (!p_ptr->sustain_int) {
	(void)dec_stat(A_INT);
	msg_print("You become very dizzy.");
    }
    else {
	msg_print("You become dizzy for a moment;  it passes.");
    }
}


/*
 * Lose a wisdom point.					-RAK-	 
 */
static void lose_wis()
{
    if (!p_ptr->sustain_wis) {
	(void)dec_stat(A_WIS);
	msg_print("You feel very naive.");
    }
    else {
	msg_print("You feel naive for a moment;  it passes.");
    }
}


/*
 * Lose a dexterity point.				-RAK-	 
 */
static void lose_dex()
{
    if (!p_ptr->sustain_dex) {
	(void)dec_stat(A_DEX);
	msg_print("You feel very sore.");
    }
    else {
	msg_print("You feel sore for a moment;  it passes.");
    }
}


/*
 * Lose a constitution point.				-RAK-	 
 */
static void lose_con()
{
    if (!p_ptr->sustain_con) {
	(void)dec_stat(A_CON);
	msg_print("You feel very sick.");
    }
    else {
	msg_print("You feel sick for a moment;  it passes.");
    }
}


/*
 * Lose a charisma point.				-RAK-	 
 */
static void lose_chr()
{
    if (!p_ptr->sustain_chr) {
	(void)dec_stat(A_CHR);
	msg_print("Your skin starts to itch.");
    }
    else {
	msg_print("Your skin starts to itch, but feels better now.");
    }
}



/*
 * Eat some food. -RAK-
 * A single food object disappears.
 * Food uses "pval" for "calories".
 */
void do_cmd_eat_food(void)
{
    u32b                 flg;
    int			   j, ident, lev;
    int                    item_val, i1, i2;
    register inven_type   *i_ptr;

    /* Assume the turn is free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_FOOD, TV_NEVER, &i1, &i2)) {
	msg_print("You are not carrying any food.");
	return;
    }

    /* Get a food */
    if (!get_item(&item_val, "Eat what?", i1, i2)) return;

    /* Get the item */
    i_ptr = &inventory[item_val];

    /* Get the item level */
    lev = i_ptr->level;

	free_turn_flag = FALSE;

    /* Identity not known yet */
    ident = FALSE;

    /* Apply all of the food flags */
    for (flg = i_ptr->flags1; flg; ) {

	/* Extract the next "effect" bit */
	j = bit_pos(&flg);

	/* Analyze the effect */
	switch (j + 1) {

	  case 1:
	    if (!p_ptr->resist_pois)
		p_ptr->poisoned += randint(10) + lev;
		ident = TRUE;
	    break;

	  case 2:
	    if (!p_ptr->resist_blind) {
		p_ptr->blind += randint(250) + 10 * lev + 100;
		draw_cave();
		msg_print("A veil of darkness surrounds you.");
		ident = TRUE;
	    }
	    break;

	  case 3:
	    if (!p_ptr->resist_fear) {
		p_ptr->afraid += randint(10) + lev;
		msg_print("You feel terrified!");
		ident = TRUE;
	    }
	    break;

	  case 4:
	    if ((!p_ptr->resist_conf) && (!p_ptr->resist_chaos)) {
		p_ptr->confused += randint(10) + lev;
		msg_print("You feel drugged.");
	    }
		ident = TRUE;
	    break;

	  case 5:
	    p_ptr->image += randint(200) + 25 * lev + 200;
	    msg_print("You feel drugged.");
	    ident = TRUE;
	    break;

	  case 6:
	    ident = cure_poison();
	    break;

	  case 7:
	    ident = cure_blindness();
	    break;

	  case 8:
	    if (p_ptr->afraid > 1) {
		p_ptr->afraid = 1;
		ident = TRUE;
	    }
	    break;

	  case 9:
	    ident = cure_confusion();
	    break;

	  case 10:
	    lose_str();
	    ident = TRUE;
	    break;

	  case 11:
	    lose_con();
	    ident = TRUE;
	    break;

#if 0				   /* 12 through 15 are not used */
	  case 12:
	    lose_int();
	    ident = TRUE;
	    break;

	  case 13:
	    lose_wis();
	    ident = TRUE;
	    break;

	  case 14:
	    lose_dex();
	    ident = TRUE;
	    break;

	  case 15:
	    lose_chr();
	    ident = TRUE;
	    break;
#endif

	  case 16:
	    if (res_stat(A_STR)) {
		msg_print("You feel your strength returning.");
		ident = TRUE;
	    }
	    break;

	  case 17:
	    if (res_stat(A_CON)) {
		msg_print("You feel your health returning.");
		ident = TRUE;
	    }
	    break;

	  case 18:
	    if (res_stat(A_INT)) {
		msg_print("Your head spins a moment.");
		ident = TRUE;
	    }
	    break;

	  case 19:
	    if (res_stat(A_WIS)) {
		msg_print("You feel your wisdom returning.");
		ident = TRUE;
	    }
	    break;

	  case 20:
	    if (res_stat(A_DEX)) {
		msg_print("You feel more dextrous.");
		ident = TRUE;
	    }
	    break;

	  case 21:
	    if (res_stat(A_CHR)) {
		msg_print("Your skin stops itching.");
		ident = TRUE;
	    }
	    break;

	  case 22:
	    ident = hp_player(randint(6));
	    break;

	  case 23:
	    ident = hp_player(randint(12));
	    break;

	  case 24:
	    ident = hp_player(randint(18));
	    break;

#if 0				   /* 25 is not used */
	  case 25:
	    ident = hp_player(damroll(3, 6));
	    break;
#endif

	  case 26:
	    ident = hp_player(damroll(3, 12));
	    break;

	  case 27:
	    take_hit(randint(18), "poisonous food.");
	    ident = TRUE;
	    break;

#if 0				   /* 28 through 30 are not used */
	  case 28:
	    take_hit(randint(8), "poisonous food.");
	    ident = TRUE;
	    break;

	  case 29:
	    take_hit(damroll(2, 8), "poisonous food.");
	    ident = TRUE;
	    break;

	  case 30:
	    take_hit(damroll(3, 8), "poisonous food.");
	    ident = TRUE;
	    break;
#endif

	  default:
	    msg_print("Internal error in do_cmd_eat_food()");
	    break;
	}
    }

    /* The player is now aware of the object */
    if (ident) { if (!known1_p(i_ptr)) {
	p_ptr->exp += (i_ptr->level + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
	}
    } else if (!known1_p(i_ptr)) sample(i_ptr);

    /* Consume the food */
    add_food(i_ptr->pval);

    /* Hack -- note loss of hunger */
    p_ptr->status &= ~(PY_WEAK | PY_HUNGRY);
    prt_hunger();

    /* Destroy the food */
    inven_item_describe(item_val);
    inven_destroy(item_val);
}





/*
 * Quaff a potion
 * A single potion object disappears.
 * Potions use "pval" for "calories"
 */
void do_cmd_quaff_potion(void)
{
    u32b flg, l;
    int    j, ident;
    int    item_val, i1, i2;
    register inven_type   *i_ptr;

    /* Assume the turn will be free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_POTION1, TV_POTION2, &i1, &i2)) {
	msg_print("You are not carrying any potions.");
	return;
    }

    /* Get a potion */
    if (!get_item(&item_val, "Quaff which potion?", i1, i2)) return;

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Not identified yet */
    ident = FALSE;

    /* Note potions with no effects */
    if (i_ptr->flags1 == 0) {
	msg_print("You feel less thirsty.");
	ident = TRUE;
    }

    /* Analyze the first set of effects */    
    for (flg = i_ptr->flags1; flg; ) {

	/* Extract the next effect bit */
	j = bit_pos(&flg);
	if (i_ptr->tval == TV_POTION2) j += 32;

	/* Analyze the effect */
	switch (j + 1) {

	  case 1:
	    if (inc_stat(A_STR)) {
		msg_print("Wow!  What bulging muscles! ");
		ident = TRUE;
	    }
	    break;

	  case 2:
	    ident = TRUE;
	    lose_str();
	    break;

	  case 3:
	    if (res_stat(A_STR)) {
		msg_print("You feel warm all over.");
		ident = TRUE;
	    }
	    break;

	  case 4:
	    if (inc_stat(A_INT)) {
		msg_print("Aren't you brilliant! ");
		ident = TRUE;
	    }
	    break;

	  case 5:
	    ident = TRUE;
	    lose_int();
	    break;

	  case 6:
	    if (res_stat(A_INT)) {
		msg_print("You have a warm feeling.");
		ident = TRUE;
	    }
	    break;

	  case 7:
	    if (inc_stat(A_WIS)) {
		msg_print("You suddenly have a profound thought! ");
		ident = TRUE;
	    }
	    break;

	  case 8:
	    ident = TRUE;
	    lose_wis();
	    break;

	  case 9:
	    if (res_stat(A_WIS)) {
		msg_print("You feel your wisdom returning.");
		ident = TRUE;
	    }
	    break;

	  case 10:
	    if (inc_stat(A_CHR)) {
		msg_print("Gee, ain't you cute! ");
		ident = TRUE;
	    }
	    break;

	  case 11:
	    ident = TRUE;
	    lose_chr();
	    break;

	  case 12:
	    if (res_stat(A_CHR)) {
		msg_print("You feel your looks returning.");
		ident = TRUE;
	    }
	    break;

	  case 13:
	    if (hp_player(damroll(2, 7))) ident = TRUE;
	    if (p_ptr->cut > 0) {
		msg_print("Your wounds heal.");
		p_ptr->cut -= 10;
		if (p_ptr->cut < 0) p_ptr->cut = 0;
		ident = TRUE;
	    }
	    break;

	  case 14:
	    if (hp_player(damroll(4, 7))) ident = TRUE;
	    if (p_ptr->cut > 0) {
		msg_print("Your wounds heal.");
		p_ptr->cut = (p_ptr->cut / 2) - 50;
		if (p_ptr->cut < 0) p_ptr->cut = 0;
		ident = TRUE;
	    }
	    break;

	  case 15:
	    if (hp_player(damroll(6, 7))) ident = TRUE;
	    if (p_ptr->cut > 0) {
		msg_print("Your wounds heal.");
		p_ptr->cut = 0;
		ident = TRUE;
	    }
	    if (p_ptr->stun > 0) {
		msg_print("Your head stops stinging.");
		p_ptr->stun = 0;
		if (p_ptr->stun > 50) {
		    p_ptr->ptohit += 20;
		    p_ptr->ptodam += 20;
		} else {
		    p_ptr->ptohit += 5;
		    p_ptr->ptodam += 5;
		}
		ident = TRUE;
	    }
	    break;

	  case 16:
	    if (hp_player(400)) ident = TRUE;
	    if (p_ptr->stun > 0) {
		msg_print("Your head stops stinging.");
		p_ptr->stun = 0;
		if (p_ptr->stun > 50) {
		    p_ptr->ptohit += 20;
		    p_ptr->ptodam += 20;
		} else {
		    p_ptr->ptohit += 5;
		    p_ptr->ptodam += 5;
		}
		ident = TRUE;
	    }
	    if (p_ptr->cut > 0) {
		msg_print("Your wounds heal.");
		p_ptr->cut = 0;
		ident = TRUE;
	    }
	    break;

	  case 17:
	    if (inc_stat(A_CON)) {
		msg_print("You feel tingly for a moment.");
		ident = TRUE;
	    }
	    break;

	  case 18:
	    if (p_ptr->exp < MAX_EXP) {
		l = (p_ptr->exp / 2) + 10;
		if (l > 100000L) l = 100000L;
		p_ptr->exp += l;
		msg_print("You feel more experienced.");
		prt_experience();
		ident = TRUE;
	    }
	    break;

	  case 19:
	    if (!p_ptr->free_act) {
		/* paralysis must be zero, we are drinking */
		/* but what about multiple potion effects? */
		msg_print("You fall asleep.");
		p_ptr->paralysis += randint(4) + 4;
		ident = TRUE;
	    }
	    break;

	  case 20:
	    if (!p_ptr->resist_blind) {
		if (p_ptr->blind == 0) {
		    msg_print("You are covered by a veil of darkness.");
		    ident = TRUE;
		}
		p_ptr->blind += randint(100) + 100;
	    }
	    break;

	  case 21:
	    if (!p_ptr->resist_conf) {
		if (p_ptr->confused == 0) {
		    msg_print("Hey!  This is good stuff!  * Hick! *");
		    ident = TRUE;
		}
		p_ptr->confused += randint(20) + 12;
	    }
	    break;

	  case 22:
	    if (!(p_ptr->immune_pois ||
		  p_ptr->resist_pois ||
		  p_ptr->oppose_pois)) {
		msg_print("You feel very sick.");
		p_ptr->poisoned += randint(15) + 10;
	    }
	    else {
		msg_print("The poison has no effect.");
	    }
	    if (!p_ptr->resist_pois)
	    ident = TRUE;
	    break;

	  case 23:
	    if (p_ptr->fast == 0) ident = TRUE;
	    if (p_ptr->fast <= 0) {
		p_ptr->fast += randint(25) + 15;
	    } else
		p_ptr->fast += randint(5);
	    break;

	  case 24:
	    if (p_ptr->slow == 0) ident = TRUE;
	    p_ptr->slow += randint(25) + 15;
	    break;

	  case 26:
	    if (inc_stat(A_DEX)) {
		msg_print("You feel more limber! ");
		ident = TRUE;
	    }
	    break;

	  case 27:
	    if (res_stat(A_DEX)) {
		msg_print("You feel less clumsy.");
		ident = TRUE;
	    }
	    break;

	  case 28:
	    if (res_stat(A_CON)) {
		msg_print("You feel your health returning! ");
		ident = TRUE;
	    }
	    break;

	  case 29:
	    if (cure_blindness()) ident = TRUE;
	    break;

	  case 30:
	    if (cure_confusion()) ident = TRUE;
	    break;

	  case 31:
	    if (cure_poison()) ident = TRUE;
	    break;

	  case 32:
	    break;		/* Unused */

	  case 33:
	    break;	/* Unused */

	  case 34:
	    if (!p_ptr->hold_life && p_ptr->exp > 0) {
		s32b               m, scale;

		msg_print("You feel your memories fade.");
		m = p_ptr->exp / 5;
		if (p_ptr->exp > MAX_SHORT) {
		    scale = MAX_LONG / p_ptr->exp;
		    m += (randint((int)scale) * p_ptr->exp) / (scale * 5);
		}
		else {
		    m += randint((int)p_ptr->exp) / 5;
		}
		lose_exp(m);
	    }
	    else {
		msg_print("You feel you memories fade for a moment, but quickly return.");
	    }
	    ident = TRUE;
	    break;

	  case 35:
	    (void)cure_poison();
	    if (p_ptr->food > 150) p_ptr->food = 150;
	    p_ptr->paralysis = 4;
	    msg_print("The potion makes you vomit! ");
	    ident = TRUE;
	    break;

	  case 37:
	    if (p_ptr->hero == 0) ident = TRUE;
	    p_ptr->hero += randint(25) + 25;
	    break;

	  case 38:
	    if (p_ptr->shero == 0) ident = TRUE;
	    p_ptr->shero += randint(25) + 25;
	    break;

	  case 39:
	    if (remove_fear()) ident = TRUE;
	    break;

	  case 40:
	    if (restore_level()) ident = TRUE;
	    break;

	  case 41:
	    if (!p_ptr->oppose_fire) ident = TRUE;
	    p_ptr->oppose_fire += randint(10) + 10;
	    break;

	  case 42:
	    if (!p_ptr->oppose_cold) ident = TRUE;
	    p_ptr->oppose_cold += randint(10) + 10;
	    break;

	  case 43:
	    if (!p_ptr->detect_inv) ident = TRUE;
	    detect_inv2(randint(12) + 12);
	    break;

	  case 44:
	    if (slow_poison()) ident = TRUE;
	    break;

	  case 45:
	    if (cure_poison()) ident = TRUE;
	    break;

	  case 46:
	    if (p_ptr->cmana < p_ptr->mana) {
		p_ptr->cmana = p_ptr->mana;
		msg_print("Your feel your head clear.");
		prt_cmana();
		ident = TRUE;
	    }
	    break;

	  case 47:
	    if (p_ptr->tim_infra == 0) {
		msg_print("Your eyes begin to tingle.");
		ident = TRUE;
	    }
	    p_ptr->tim_infra += 100 + randint(100);
	    break;

	  case 48:
	    wiz_lite(TRUE);
	    if (!res_stat(A_WIS)) inc_stat(A_WIS);
	    if (!res_stat(A_INT)) inc_stat(A_INT);
	    msg_print("You feel more enlightened! ");
	    msg_print(NULL);
	    /* after all, what is the key to enlightenment? -CFT */
	    self_knowledge();
	    identify_pack();
	    (void)detect_treasure();
	    (void)detect_object();
	    (void)detect_sdoor();
	    (void)detect_trap();
	    ident = TRUE;
	    break;

	  case 49:
	    msg_print("Massive explosions rupture your body! ");
	    take_hit(damroll(50, 20), "a potion of Detonation");
	    cut_player(5000);
	    stun_player(75);
	    ident = TRUE;
	    break;

	  case 50:
	    msg_print("A feeling of Death flows through your body.");
	    take_hit(5000, "a potion of Death");
	    ident = TRUE;
	    break;

	  case 51:
	    if (restore_level() |
	    res_stat(A_STR) |
	    res_stat(A_CON) |
	    res_stat(A_DEX) |
	    res_stat(A_WIS) |
	    res_stat(A_INT) |
	    res_stat(A_CHR) |
	    hp_player(5000) |
	    cure_poison() |
	    cure_blindness() |
	    cure_confusion() |
	    (p_ptr->stun > 0) |
	    (p_ptr->cut > 0) |
	    (p_ptr->image > 0) |
	    remove_fear()) {
	    p_ptr->cut = 0;
	    p_ptr->image = 0;
	    if (p_ptr->stun > 0) {
		    if (p_ptr->stun > 50) {
			p_ptr->ptohit += 20;
			p_ptr->ptodam += 20;
		    } else {
			p_ptr->ptohit += 5;
			p_ptr->ptodam += 5;
		    }
		    p_ptr->stun = 0;
	    }
	    ident = TRUE;
	    }
	    break;

	  case 52:	   /* Augm */
	    if (inc_stat(A_DEX)) ident = TRUE;
	    if (inc_stat(A_WIS)) ident = TRUE;
	    if (inc_stat(A_INT)) ident = TRUE;
	    if (inc_stat(A_STR)) ident = TRUE;
	    if (inc_stat(A_CHR)) ident = TRUE;
	    if (inc_stat(A_CON)) ident = TRUE;
	    if (ident) {
		msg_print("You feel power flow through your body! ");
	    }
	    break;

	  case 53:	   /* Ruination */
	    take_hit(damroll(10, 10), "a potion of Ruination");
	    ruin_stat(A_DEX);
	    ruin_stat(A_WIS);
	    ruin_stat(A_CON);
	    ruin_stat(A_STR);
	    ruin_stat(A_CHR);
	    ruin_stat(A_INT);
	    ident = TRUE;
	    msg_print("Your nerves and muscles feel weak and lifeless! ");
	    break;

	  case 54:
	    msg_print("An image of your surroundings forms in your mind...");
	    wiz_lite(TRUE);
	    ident = TRUE;
	    break;

	  case 55:
	    msg_print("You feel you know yourself a little better...");
	    msg_print(NULL);
	    self_knowledge();
	    ident = TRUE;
	    break;

	  case 56:
	    if (hp_player(1200)) ident = TRUE;
	    if (p_ptr->stun > 0) {
		p_ptr->stun = 0;
		msg_print("Your head stops stinging.");
		if (p_ptr->stun > 50) {
		    p_ptr->ptohit += 20;
		    p_ptr->ptodam += 20;
		} else {
		    p_ptr->ptohit += 5;
		    p_ptr->ptodam += 5;
		}
		ident = TRUE;
	    }
	    if (p_ptr->cut > 0) {
		p_ptr->cut = 0;
		msg_print("Your wounds heal.");
		ident = TRUE;
	    }
	    if (cure_blindness()) ident = TRUE;
	    if (cure_confusion()) ident = TRUE;
	    if (cure_poison()) ident = TRUE;
	    break;

	  default:
	    if (1) {
		    char                tmp_str[100];

		    msg_print("Internal error in potion()");
		    sprintf(tmp_str, "Number %d...", j);
		    msg_print(tmp_str);
	    }
	    break;
	}
    }

    /* An identification was made */
    if (ident) { if (!known1_p(i_ptr)) {
	int lev = i_ptr->level;
	p_ptr->exp += (lev + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
	i_ptr = &inventory[item_val];
	}
    } else if (!known1_p(i_ptr)) sample(i_ptr);

    /* Potions can feed the player */
    add_food(i_ptr->pval);

    /* Destroy the potion */
    inven_item_describe(item_val);
    inven_destroy(item_val);
}




/*
 * Read a scroll (destroy one scroll).
 */
void do_cmd_read_scroll(void)
{
    u32b                flg;
    int                   item_val, i1, i2, x, y;
    int			  j, k;
    int                   used_up, ident, l;
    int                   tmp[6];
    register inven_type  *i_ptr;
    bigvtype              out_val, tmp_str;

    free_turn_flag = TRUE;

    if (p_ptr->blind > 0) {
	msg_print("You can't see to read the scroll.");
	return;
    }

    if (no_lite()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->confused > 0) {
	msg_print("You are too confused to read a scroll.");
	return;
    }

    if (inven_ctr == 0) {
	msg_print("You are not carrying anything!");
	return;
    }

    if (!find_range(TV_SCROLL1, TV_SCROLL2, &i1, &i2)) {
	msg_print("You are not carrying any scrolls!");
	return;
    }

    /* Get a scroll */
    if (!get_item(&item_val, "Read which scroll?", i1, i2)) return; {

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Assume the scroll will get used up */
    used_up = TRUE;

    /* Not identified yet */
    ident = FALSE;

    /* Apply the first set of scroll effects */
    for (flg = i_ptr->flags1; flg; ) {

	/* Extract the next effect bit-flag */
	j = bit_pos(&i);
	if (i_ptr->tval == TV_SCROLL2) j += 32;

	/* Scrolls. */
	switch (j+1) {

	  case 1:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];

	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, 1, ENCH_TOHIT)) {
		    msg_print("The enchantment fails. ");
		}
		ident = TRUE;
	    }
	    break;

	  case 2:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];

	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, 1, ENCH_TODAM)) {
		    msg_print("The enchantment fails. ");
		}
		ident = TRUE;
	    }
	    break;		

	  case 3:

	    /* Hack -- make a "list" of "armor" indexes, size is "k" */
	    k = 0;
	    l = 0;
	    if (inventory[INVEN_BODY].tval != TV_NOTHING)  tmp[k++] = INVEN_BODY;
	    if (inventory[INVEN_ARM].tval != TV_NOTHING)   tmp[k++] = INVEN_ARM;
	    if (inventory[INVEN_OUTER].tval != TV_NOTHING) tmp[k++] = INVEN_OUTER;
	    if (inventory[INVEN_HANDS].tval != TV_NOTHING) tmp[k++] = INVEN_HANDS;
	    if (inventory[INVEN_HEAD].tval != TV_NOTHING)  tmp[k++] = INVEN_HEAD;
	    if (inventory[INVEN_FEET].tval != TV_NOTHING)  tmp[k++] = INVEN_FEET;

	    if (k > 0) l = tmp[randint(k) - 1];
	    if (TR3_CURSED & inventory[INVEN_BODY].flags1) l = INVEN_BODY;
	    else if (TR3_CURSED & inventory[INVEN_ARM].flags1) l = INVEN_ARM;
	    else if (TR3_CURSED & inventory[INVEN_OUTER].flags1) l = INVEN_OUTER;
	    else if (TR3_CURSED & inventory[INVEN_HEAD].flags1) l = INVEN_HEAD;
	    else if (TR3_CURSED & inventory[INVEN_HANDS].flags1) l = INVEN_HANDS;
	    else if (TR3_CURSED & inventory[INVEN_FEET].flags1) l = INVEN_FEET;

	    if (l > 0) {
	    i_ptr = &inventory[l];

	    /* Visual effect (known) */
	    objdes(tmp_str, i_ptr, FALSE);
	    (void) sprintf(out_val, "Your %s glows faintly! ", tmp_str);
	    msg_print(out_val);
	    ident = TRUE;

	    /* Attempt to enchant */
	    if (!enchant(i_ptr, 1, ENCH_TOAC)) {
		msg_print("The enchantment fails. ");
	    }
		}

	    break;

	  case 4:

	    /* Can be identified by "label" on scroll */
	    msg_print("This is an identify scroll.");
	    ident = TRUE;
	    used_up = ident_spell();

	    /* The identify may merge objects, causing the identify scroll to
	     * move to a different place.  Check for that here.  It can
	     * move arbitrarily far if an identify scroll was used on another
	     * identify scroll, but it always moves down. 
	     */
		while (i_ptr->tval != TV_SCROLL1 || i_ptr->flags1 != 0x00000008) {
		    item_val--;
		    i_ptr = &inventory[item_val];
		}
	    
	    break;

	  case 5:
	    if (remove_curse()) {
		msg_print("You feel as if someone is watching over you.");
		ident = TRUE;
	    }
	    break;

	  case 6:
	    ident = lite_area(char_row, char_col, damroll(2, 12), 2);
	    break;

	  case 7:
	    for (k = 0; k < randint(3); k++) {
		y = char_row;
		x = char_col;
		ident |= summon_monster(&y, &x, FALSE);
	    }
	    break;

	  case 8:
	    teleport(10);
	    ident = TRUE;
	    break;

	  case 9:
	    teleport(100);
	    ident = TRUE;
	    break;

	  case 10:
	    (void)tele_level();
	    ident = TRUE;
	    break;

	  case 11:
	    if (p_ptr->confusing == 0) {
		msg_print("Your hands begin to glow.");
		p_ptr->confusing = TRUE;
		ident = TRUE;
	    }
	    break;

	  case 12:
	    map_area();
	    ident = TRUE;
	    break;

	  case 13:
	    ident = sleep_monsters1(char_row, char_col);
	    break;

	  case 14:
	    warding_glyph();
	    ident = TRUE;
	    break;

	  case 15:
	    ident = detect_treasure();
	    break;

	  case 16:
	    ident = detect_object();
	    break;

	  case 17:
	    ident = detect_trap();
	    break;

	  case 18:
	    ident = detect_sdoor();
	    break;

	  case 19:
	    msg_print("This is a mass genocide scroll.");
	    mass_genocide(TRUE);
	    ident = TRUE;
	    break;

	  case 20:
	    ident = detect_invisible();
	    break;

	  case 21:
	    if (aggravate_monster(20)) {
		msg_print("There is a high pitched humming noise.");
		ident = TRUE;
	    }
	    break;

	  case 22:
	    ident = trap_creation();
	    break;

	  case 23:
	    ident = td_destroy();
	    break;

	  /* Not Used, used to be door creation */
	  case 24:
	    break;

	  case 25:
	    msg_print("This is a Recharge-Item scroll.");
	    /* Hack -- Like identify, recharge can be cancelled */
	    used_up = recharge(60);
	    ident = TRUE;
	    break;

	  case 26:
	    msg_print("This is a genocide scroll.");
	    genocide(TRUE);
	    ident = TRUE;
	    break;

	  case 27:
	    ident = unlite_area(char_row, char_col);
	    if (!p_ptr->resist_blind) {
		p_ptr->blind += 3 + randint(5);
	    }
	    break;

	  case 28:
	    ident = protect_evil();
	    break;

	  case 29:
	    create_food();
	    ident = TRUE;
	    break;

	  case 30:
	    ident = dispel_creature(MF2_UNDEAD, 60);
	    break;

	  case 31:
	    remove_all_curse();
	    ident = TRUE;
	    break;

	  case 33:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];
	    
	    /* Enchant */
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		(void) sprintf(out_val, "Your %s glows brightly!", tmp_str);
		msg_print(out_val);
		if (!enchant(i_ptr, randint(3), ENCH_TOHIT|ENCH_TODAM)) {
		    msg_print("The enchantment fails.");
		}
		ident = TRUE;
	    }
	    break;

	  case 34:

	    /* Pick a weapon */
	    i_ptr = &inventory[INVEN_WIELD];
	    
	    if (i_ptr->tval != TV_NOTHING) {
		objdes(tmp_str, i_ptr, FALSE);
		if (artifact_p(i_ptr) && (randint(7) < 4)) {
		    msg_print("A terrible black aura tries to surround your weapon,");
		    sprintf(out_val, "but your %s resists the effects!", tmp_str);
		    msg_print(out_val);
		}

		/* not artifact or failed save... */
		else {

		    /* Oops */
		    sprintf(out_val, "A terrible black aura blasts your %s!",
				   tmp_str);
		    msg_print(out_val);

		    py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
		    i_ptr->name2 = EGO_SHATTERED;
		    i_ptr->tohit = (-randint(5) - randint(5));
		    i_ptr->todam = (-randint(5) - randint(5));
		    i_ptr->flags1 = TR3_CURSED;
		    i_ptr->flags2 = 0;
		    i_ptr->damage[0] = i_ptr->damage[1] = 1;
		    i_ptr->toac = 0;	/* in case defender... */
		    i_ptr->cost = (-1);
		    
		    /* Recalculate bonuses */
		    py_bonuses(i_ptr, 1);
		    calc_bonuses();
		}

		ident = TRUE;
	    }
	    break;

	  case 35:

	    /* Hack -- make a "list" of "armor" indexes, size is "k" */
	    k = 0;
	    l = 0;
	    if (inventory[INVEN_BODY].tval != TV_NOTHING)  tmp[k++] = INVEN_BODY;
	    if (inventory[INVEN_ARM].tval != TV_NOTHING)   tmp[k++] = INVEN_ARM;
	    if (inventory[INVEN_OUTER].tval != TV_NOTHING) tmp[k++] = INVEN_OUTER;
	    if (inventory[INVEN_HANDS].tval != TV_NOTHING) tmp[k++] = INVEN_HANDS;
	    if (inventory[INVEN_HEAD].tval != TV_NOTHING)  tmp[k++] = INVEN_HEAD;
	    if (inventory[INVEN_FEET].tval != TV_NOTHING)  tmp[k++] = INVEN_FEET;

	    if (k > 0) l = tmp[randint(k) - 1];
	    if (TR3_CURSED & inventory[INVEN_BODY].flags1) l = INVEN_BODY;
	    else if (TR3_CURSED & inventory[INVEN_ARM].flags1) l = INVEN_ARM;
	    else if (TR3_CURSED & inventory[INVEN_OUTER].flags1) l = INVEN_OUTER;
	    else if (TR3_CURSED & inventory[INVEN_HEAD].flags1) l = INVEN_HEAD;
	    else if (TR3_CURSED & inventory[INVEN_HANDS].flags1) l = INVEN_HANDS;
	    else if (TR3_CURSED & inventory[INVEN_FEET].flags1) l = INVEN_FEET;

	    if (l > 0) {
	    i_ptr = &inventory[l];

	    /* Message (and knowledge) */
	    objdes(tmp_str, i_ptr, FALSE);
	    (void) sprintf(out_val,"Your %s glows brightly!", tmp_str);
	    msg_print(out_val);
	    ident = TRUE;
	    if (!enchant(i_ptr, randint(3)+1, ENCH_TOAC)) {
		msg_print("The enchantment fails.");
	    }
	    }

	    break;

	  case 36:

	    if ((inventory[INVEN_BODY].tval != TV_NOTHING) && (randint(4) == 1)) k = INVEN_BODY;
	    else if ((inventory[INVEN_ARM].tval != TV_NOTHING) && (randint(3) == 1)) k = INVEN_ARM;
	    else if ((inventory[INVEN_OUTER].tval != TV_NOTHING) && (randint(3) == 1)) k = INVEN_OUTER;
	    else if ((inventory[INVEN_HEAD].tval != TV_NOTHING) && (randint(3) == 1)) k = INVEN_HEAD;
	    else if ((inventory[INVEN_HANDS].tval != TV_NOTHING) && (randint(3) == 1)) k = INVEN_HANDS;
	    else if ((inventory[INVEN_FEET].tval != TV_NOTHING) && (randint(3) == 1)) k = INVEN_FEET;
	    else if (inventory[INVEN_BODY].tval != TV_NOTHING) k = INVEN_BODY;
	    else if (inventory[INVEN_ARM].tval != TV_NOTHING) k = INVEN_ARM;
	    else if (inventory[INVEN_OUTER].tval != TV_NOTHING) k = INVEN_OUTER;
	    else if (inventory[INVEN_HEAD].tval != TV_NOTHING) k = INVEN_HEAD;
	    else if (inventory[INVEN_HANDS].tval != TV_NOTHING) k = INVEN_HANDS;
	    else if (inventory[INVEN_FEET].tval != TV_NOTHING) k = INVEN_FEET;
	    else k = 0;

	    if (k > 0) {

		i_ptr = &inventory[k];

	    /* Describe */
	    objdes(tmp_str, i_ptr, FALSE);

	    /* Attempt a saving throw for artifacts */
	    if (artifact_p(i_pt) && (randint(7) < 4)) {
		msg_print("A terrible black aura tries to surround your");
		sprintf(out_val, "%s, but it resists the effects!", tmp_str);
		msg_print(out_val);
	    }

	    /* not artifact or failed save... */
	    else {

		/* Oops */
		sprintf(out_val, "A terrible black aura blasts your %s!",
			       tmp_str);
		msg_print(out_val);

		/* Blast the armor */
		py_bonuses(i_ptr, -1);	/* take off current bonuses -CFT */
		i_ptr->name2 = EGO_BLASTED;
		i_ptr->flags1 = TR3_CURSED;
		i_ptr->flags2 = 0;
		i_ptr->toac = (-randint(5) - randint(5));
		i_ptr->tohit = i_ptr->todam = 0;
		i_ptr->ac = (i_ptr->ac > 9) ? 1 : 0;
		i_ptr->cost = (-1);
		py_bonuses(i_ptr, 1);

		/* Recalculate bonuses */
		calc_bonuses();
	    }

		/* Well, you know all about it */
		ident = TRUE;
	    }
	    break;

	  case 37:
	    ident = FALSE;
	    for (k = 0; k < randint(3); k++) {
		y = char_row;
		x = char_col;
		ident |= summon_undead(&y, &x);
	    }
	    break;

	  case 38:
	    bless(randint(12) + 6);
	    ident = TRUE;
	    break;

	  case 39:
	    bless(randint(24) + 12);
	    ident = TRUE;
	    break;

	  case 40:
	    bless(randint(48) + 24);
	    ident = TRUE;
	    break;

	  case 41:
	    if (p_ptr->word_recall == 0) {
		p_ptr->word_recall = 15 + randint(20);
		msg_print("The air about you becomes charged...");
	    }
	    else {
		p_ptr->word_recall = 0;
		msg_print("A tension leaves the air around you...");
	    }
	    ident = TRUE;
	    break;

	  case 42:
	    destroy_area(char_row, char_col);
	    ident = TRUE;
	    break;

	  case 43:
	    place_good(char_row, char_col, MF2_SPECIAL);
	    prt_map();
	    ident = TRUE;
	    break;

	  case 44:
	    special_random_object(char_row, char_col, 1);
	    prt_map();
	    ident = TRUE;
	    break;

	  default:
	    msg_print("Internal error in scroll()");
	    break;
	}
    }

	i_ptr = &inventory[item_val];

    /* An identification was made */
    if (ident) { if (!known1_p(i_ptr)) {
	int lev = i_ptr->level;
	/* round half-way case up */
	p_ptr->exp += (lev + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
    }
    } else if (!known1_p(i_ptr)) sample(i_ptr);


    /* Hack -- allow certain scrolls to be "preserved" */
    if (!used_up) return;

    /* Destroy the scroll */
    inven_item_describe(item_val);
    inven_destroy(item_val);
    }
}




/*
 * Aim a wand (use a single charge).
 */
void do_cmd_aim_wand(void)
{
    u32b                i;
    int			ident, chance, dir;
    int			item_val, i1, i2, x, y, lev, done_effect;
    inven_type		*i_ptr;

    free_turn_flag = TRUE;

    if (!inven_ctr) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_WAND, TV_NEVER, &i1, &i2)) {
	msg_print("You are not carrying any wands.");
	return;
    }
    
    /* Get a wand */
    if (!get_item(&item_val, "Aim which wand?", i1, i2)) return;
    
    /* Get the item */
    i_ptr = &inventory[item_val];

    /* Get the level */
    lev =i_ptr->level;

    /* The turn is not free */
    free_turn_flag = FALSE;

    if (!get_dir_c(NULL, &dir)) return;

    ident = FALSE;

    /* Chance of success */
    chance = (p_ptr->save + stat_adj(A_INT) - (int)(lev > 42 ? 42 : lev) +
	      (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3));

    if (p_ptr->confused > 0) chance = chance / 2;

    /* Give everyone a slight chance */
    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;
    }

    if (chance <= 0) chance = 1;

    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the wand properly.");
	return;
    }

    /* The wand is already empty! */
    if (i_ptr->pval <= 0) {
	msg_print("The wand has no charges left.");
	if (!known2_p(i_ptr)) {
	    add_inscribe(i_ptr, ID_EMPTY);
	}
	return;
    }

	i = i_ptr->flags1;
	done_effect = 0;
	(i_ptr->pval)--;
	while (!done_effect) {

    /* Start at the player */
    y = char_row;
    x = char_col;

    /* Various effects */
    switch (i) {

	case SV_WAND_LITE:
	    msg_print("A line of blue shimmering light appears.");
	    lite_line(dir, char_row, char_col);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_ACID:	/* Acid , New */
	    if (randint(5)==1) line_spell(GF_ACID,dir,y,x,damroll(5,8));
	    else fire_bolt(GF_ACID,dir,y,x,damroll(5,8));
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_ELEC:	/* Lightning */
	    if (randint(6)==1) line_spell(GF_ELEC,dir,y,x,damroll(3,8));
	    else fire_bolt(GF_ELEC, dir, y, x, damroll(3, 8));
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_COLD:	/* Frost */
	    if (randint(6)==1) line_spell(GF_ELEC,dir,y,x,damroll(3,8));
	else fire_bolt(GF_ELEC, dir, y, x, damroll(3, 8));
	    ident = TRUE;
	    done_effect = 1;
	    break;
	case SV_WAND_DRAGON_COLD:
	    fire_ball(GF_COLD, dir, y, x, 80, 3);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_FIRE:	/* Fire */
	    if (randint(4)==1) line_spell(GF_FIRE,dir,y,x,damroll(6,8));
	else fire_bolt(GF_FIRE, dir, y, x, damroll(6, 8));
	    ident = TRUE;
	    done_effect = 1;
	    break;
	case SV_WAND_DRAGON_FIRE:
	    fire_ball(GF_FIRE, dir, y, x, 100, 3);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_STONE_TO_MUD:
	    ident = wall_to_mud(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_POLYMORPH:
	    ident = poly_monster(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_HEAL_MONSTER:
	    ident = hp_monster(dir,y,x, -damroll(4, 6));
	    done_effect = 1;
	    break;

	case SV_WAND_HASTE_MONSTER:
	    ident = speed_monster(dir,y,x,1);
	    done_effect = 1;
	    break;

	case SV_WAND_SLOW_MONSTER:
	    ident = speed_monster(dir,y,x,-1);
	    done_effect = 1;
	    break;

	case SV_WAND_CONFUSE_MONSTER:
	    ident = confuse_monster(dir,y,x,10);
	    done_effect = 1;
	    break;

	case SV_WAND_SLEEP_MONSTER:
	    ident = sleep_monster(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_DRAIN_LIFE:
	    ident = drain_life(dir,y,x,75);
	    done_effect = 1;
	    break;

	case SV_WAND_TRAP_DOOR_DEST:
	    ident = td_destroy2(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_MAGIC_MISSILE:
	    if (randint(6)==1) line_spell(GF_MISSILE,dir,y,x,damroll(2,6));
	    else fire_bolt(GF_MISSILE, dir,y,x, damroll(2,6));
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_FEAR_MONSTER:
	    ident = fear_monster(dir,y,x,10);
	    done_effect = 1;
	    break;

	case SV_WAND_CLONE_MONSTER:
	    ident = clone_monster(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_TELEPORT_AWAY:
	    ident = teleport_monster(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_DISARMING:
	    ident = disarm_all(dir,y,x);
	    done_effect = 1;
	    break;

	case SV_WAND_ELEC_BALL:
	    fire_ball(GF_ELEC, dir,y,x,32,2);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_COLD_BALL:
	    fire_ball(GF_COLD,dir,y,x,48,2);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_FIRE_BALL:
	    fire_ball(GF_FIRE,dir,y,x,72,2);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_STINKING_CLOUD:
	    fire_ball(GF_POIS,dir,y,x,12,2);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_ACID_BALL:
	    fire_ball(GF_ACID,dir,y,x,60,2);
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_WONDER:
	    i = randint(23);
	    break;

	case SV_WAND_DRAGON_BREATH:
	    switch (randint(5)) {
	      case 1:
		fire_ball(GF_FIRE, dir, y, x, 100, 3);
		break;
	      case 2:
		fire_ball(GF_COLD, dir, y, x, 80, 3);
		break;
	      case 3:
		fire_ball(GF_ACID, dir, y, x, 90, 3);
		break;
	      case 4:
		fire_ball(GF_ELEC, dir, y, x, 70, 3);
		break;
	      default:
		fire_ball(GF_POIS, dir, y, x, 70, 3);
		break;
	    }
	    ident = TRUE;
	    done_effect = 1;
	    break;

	case SV_WAND_ANNIHILATION:
	    ident = drain_life(dir,y,x,125);
	    done_effect = 1;
	    break;

	default:
	    msg_print("Internal error in wands() ");
	    done_effect = 1;
	    break;
    }
	}

    /* Apply identification */
    if (ident) { if (!known1_p(i_ptr)) {
	/* round half-way case up */
	p_ptr->exp += (lev + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
    }
	} else if (!known1_p(i_ptr)) sample(i_ptr);

	inven_item_charges(item_val);
    }
}




/*
 * Use a staff.			-RAK-	
 *
 * One charge of one staff disappears.
 */
void do_cmd_use_staff(void)
{
    u32b                i;
    int			  ident, chance, k, lev;
    int                   item_val, i1, i2, x, y;
    register inven_type  *i_ptr;

    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_STAFF, TV_NEVER, &i1, &i2)) {
	msg_print("You are not carrying any staffs.");
	return;
    }
    
    /* Get a staff */
    if (!get_item(&item_val, "Use which staff?", i1, i2)) return;
    
    /* Get the item */
    i_ptr = &inventory[item_val];

    /* Extract the item level */
    lev = i_ptr->level;

	free_turn_flag = FALSE;

    /* Chance of success */
    chance = (p_ptr->save + stat_adj(A_INT) - (int)(lev > 50 ? 50 : lev) +
	      (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3));

    if (p_ptr->confused > 0) chance = chance / 2;

    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;   /* Give everyone a slight chance */
    }

    if (chance <= 0) chance = 1;

    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the staff properly.");
	return;
    }

    if (i_ptr->pval <= 0) {
	msg_print("The staff has no charges left.");
	if (!known2_p(i_ptr)) add_inscribe(i_ptr, ID_EMPTY);
	return;
    }

    ident = FALSE;

    (i_ptr->pval)--;

    switch (i_ptr->flags1) {

      case SV_STAFF_HEALING:
	ident = hp_player(300);
	if (p_ptr->stun > 0) {
	    p_ptr->stun = 0;
	    msg_print("Your head stops stinging.");
	    if (p_ptr->stun > 50) {
		p_ptr->ptohit += 20;
		p_ptr->ptodam += 20;
	    } else {
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
	    }
	    ident = TRUE;
	}
	if (p_ptr->cut > 0) {
	    p_ptr->cut = 0;
	    msg_print("You feel better.");
	    ident = TRUE;
	}
	break;

      case SV_STAFF_GENOCIDE:
	genocide(FALSE);
	ident = TRUE;
	break;

      case SV_STAFF_PROBING:
	probing();
	ident = TRUE;
	break;

      case SV_STAFF_IDENTIFY:
	ident_spell();
	ident = TRUE;
	break;

      case SV_STAFF_HOLINESS:
	dispel_creature(MF2_EVIL, 120);
	protect_evil();
	cure_poison();
	remove_fear();
	hp_player(50);
	if (p_ptr->stun > 0) {
	    p_ptr->stun = 0;
	    msg_print("Your head stops stinging.");
	    if (p_ptr->stun > 50) {
		p_ptr->ptohit += 20;
		p_ptr->ptodam += 20;
	    } else {
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
	    }
	}
	if (p_ptr->cut > 0) {
	    p_ptr->cut = 0;
	    msg_print("You feel better.");
	}
	ident = TRUE;
	break;

      case SV_STAFF_THE_MAGI:
	if (res_stat(A_INT)) {
	    msg_print("You have a warm feeling.");
	    ident = TRUE;
	}
	if (p_ptr->cmana < p_ptr->mana) {
	    p_ptr->cmana = p_ptr->mana;
	    ident = TRUE;
	    msg_print("Your feel your head clear.");
	    prt_cmana();
	}
	break;

      case SV_STAFF_POWER:
	dispel_creature(0xFFFFFFFFL, 120);
	break;

      case SV_STAFF_MAPPING:
	map_area();
	ident = TRUE;
	break;

      case SV_STAFF_LITE:
	ident = lite_area(char_row, char_col, damroll(2, 10), 2);
	break;

      case SV_STAFF_DOOR_STAIR_LOC:
	ident = detect_sdoor();
	break;

      case SV_STAFF_TRAP_LOC:
	ident = detect_trap();
	break;

      case SV_STAFF_TREASURE_LOC:
	ident = detect_treasure();
	break;

      case SV_STAFF_OBJECT_LOC:
	ident = detect_object();
	break;

      case SV_STAFF_TELEPORTATION:
	teleport(100);
	ident = TRUE;
	break;

      case SV_STAFF_EARTHQUAKES:
	earthquake();
	ident = TRUE;
	break;

      case SV_STAFF_SUMMONING:
	ident = FALSE;
	for (k = 0; k < randint(4); k++) {
	    y = char_row;
	    x = char_col;
	    ident |= summon_monster(&y, &x, FALSE);
	}
	break;

      case SV_STAFF_DESTRUCTION:
	destroy_area(char_row, char_col);
	ident = TRUE;
	break;

      case SV_STAFF_STARLITE:
	starlite(char_row, char_col);
	ident = TRUE;
	break;

      case SV_STAFF_HASTE_MONSTERS:
	ident = speed_monsters(1);
	break;

      case SV_STAFF_SLOW_MONSTERS:
	ident = speed_monsters(-1);
	break;

      case SV_STAFF_SLEEP_MONSTERS:
	ident = sleep_monsters2();
	break;

      case SV_STAFF_CURE_LIGHT:
	ident = hp_player(randint(8));
	break;

      case SV_STAFF_DETECT_INVIS:
	ident = detect_invisible();
	break;

      case SV_STAFF_SPEED:
	if (p_ptr->fast == 0) ident = TRUE;
	if (p_ptr->fast <= 0) p_ptr->fast += randint(30) + 15;
	else p_ptr->fast += randint(5);
	break;

      case SV_STAFF_SLOWNESS:
	if (p_ptr->slow == 0) ident = TRUE;
	p_ptr->slow += randint(30) + 15;
	break;

      case SV_STAFF_REMOVE_CURSE:
	if (remove_curse()) {
	    if (p_ptr->blind < 1) {
		msg_print("The staff glows blue for a moment..");
	    }
	    ident = TRUE;
	}
	break;

      case SV_STAFF_DETECT_EVIL:
	ident = detect_evil();
	break;

      case SV_STAFF_CURING:
	if (cure_blindness()) ident = TRUE;
	if (cure_poison()) ident = TRUE;
	if (cure_confusion()) ident = TRUE;
	if (p_ptr->stun > 0) {
	    msg_print("Your head stops stinging.");
	    p_ptr->stun = 0;
	    if (p_ptr->stun > 50) {
		p_ptr->ptohit += 20;
		p_ptr->ptodam += 20;
	    } else {
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
	    }
	    ident = TRUE;
	}
	else if (p_ptr->cut > 0) {
	    msg_print("You feel better.");
	    p_ptr->cut = 0;
	    ident = TRUE;
	}
	break;

      case SV_STAFF_DISPEL_EVIL:
	ident = dispel_creature(MF2_EVIL, 60);
	break;

      case SV_STAFF_DARKNESS:
	ident = unlite_area(char_row, char_col);
	break;

      default:
	msg_print("Internal error in staffs()");
	break;
    }

    /* An identification was made */
    if (ident) { if (!known1_p(i_ptr)) {
	p_ptr->exp += (lev + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
	}
    } else if (!known1_p(i_ptr)) sample(i_ptr);

    inven_item_charges(item_val);
}





/*
 * Activate (zap) a Rod
 */
void do_cmd_zap_rod(void)
{
    int                 ident, chance, dir, lev;
    int                 item_val, i1, i2, x, y;
    inven_type		*i_ptr;

    /* Assume free turn */
    free_turn_flag = TRUE;

    if (!inven_ctr) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_ROD, TV_NEVER, &i1, &i2)) {
	msg_print("You are not carrying any rods.");
	return;
    }

    /* Get a rod */
    if (!get_item(&item_val, "Activate which rod?", i1, i2)) return;
    
    /* Get the item */
    i_ptr = &inventory[item_val];

    /* Extract the item level */
    lev = i_ptr->level;

	free_turn_flag = FALSE;
	ident = FALSE;

    /* Calculate the chance */
    chance = (p_ptr->save + (stat_adj(A_INT) * 2) - (int)((lev > 70) ? 70 : lev) +
	      (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3));

    if (p_ptr->confused > 0) chance = chance / 2;

    /* Give everyone a slight chance */
    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
	chance = USE_DEVICE;
    }

    /* Prevent errors in "randint" */
    if (chance <= 0) chance = 1;

    /* Fail to use */
    if (randint(chance) < USE_DEVICE) {
	msg_print("You failed to use the rod properly.");
	return;
    }

    /* Still charging */
    if (i_ptr->timeout) {
	msg_print("The rod is currently exhausted.");
	return;
    }

    /* Starting location */
    y = char_row;
    x = char_col;

    /* Activate it */
    switch (i_ptr->flags1) {

      case SV_ROD_LIGHT:
	if (!get_dir_c(NULL, &dir)) return;
	msg_print("A line of blue shimmering light appears.");
	lite_line(dir, char_row, char_col);
	ident = TRUE;
	i_ptr->timeout = 9;
	break;

      case SV_ROD_ILLUMINATION:
	lite_area(y, x, damroll(2, 8), 2);
	ident = TRUE;
	i_ptr->timeout = 30;
	break;

      case SV_ROD_ACID:
	if (!get_dir_c(NULL, &dir)) return;
	if (randint(10)==1) line_spell(GF_ACID, dir, y, x, damroll(6,8));
	else fire_bolt(GF_ACID, dir, y, x, damroll(6,8));
	ident = TRUE;
	i_ptr->timeout = 12;
	break;

      case SV_ROD_ELEC:
	if (!get_dir_c(NULL, &dir)) return;
	if (randint(12)==1) line_spell(GF_ELEC, dir, y, x, damroll(3,8));
	else fire_bolt(GF_ELEC, dir, y, x, damroll(3,8));
	ident = TRUE;
	i_ptr->timeout = 11;
	break;

      case SV_ROD_COLD:
	if (!get_dir_c(NULL, &dir)) return;
	if (randint(10)==1) line_spell(GF_COLD, dir, y, x, damroll(5,8));
	else fire_bolt(GF_COLD, dir, y, x, damroll(5,8));
	ident = TRUE;
	i_ptr->timeout = 13;
	break;

      case SV_ROD_FIRE:
	if (!get_dir_c(NULL, &dir)) return;
	if (randint(8)==1) line_spell(GF_FIRE, dir, y, x, damroll(8,8));
	else fire_bolt(GF_FIRE, dir, y, x, damroll(8,8));
	ident = TRUE;
	i_ptr->timeout = 15;
	break;

      case SV_ROD_POLYMORPH:
	if (!get_dir_c(NULL, &dir)) return;
	ident = poly_monster(dir, y, x);
	i_ptr->timeout = 25;
	break;

      case SV_ROD_SLOW_MONSTER:
	if (!get_dir_c(NULL, &dir)) return;
	ident = speed_monster(dir, y, x, -1);
	i_ptr->timeout = 20;
	break;

      case SV_ROD_SLEEP_MONSTER:
	if (!get_dir_c(NULL, &dir)) return;
	ident = sleep_monster(dir, y, x);
	i_ptr->timeout = 18;
	break;

      case SV_ROD_DRAIN_LIFE:
	if (!get_dir_c(NULL, &dir)) return;
	ident = drain_life(dir, y, x, 75);
	i_ptr->timeout = 23;
	break;

      case SV_ROD_TELEPORT_AWAY:
	if (!get_dir_c(NULL, &dir)) return;
	ident = teleport_monster(dir, y, x);
	i_ptr->timeout = 25;
	break;

      case SV_ROD_DISARMING:
	if (!get_dir_c(NULL, &dir)) return;
	ident = disarm_all(dir, y, x);
	i_ptr->timeout = 30;
	break;

      case SV_ROD_ELEC_BALL:
	if (!get_dir_c(NULL, &dir)) return;
	fire_ball(GF_ELEC, dir, y, x, 32, 2);
	ident = TRUE;
	i_ptr->timeout = 23;
	break;

      case SV_ROD_COLD_BALL:
	if (!get_dir_c(NULL, &dir)) return;
	fire_ball(GF_COLD, dir, y, x, 48, 2);
	ident = TRUE;
	i_ptr->timeout = 25;
	break;

      case SV_ROD_FIRE_BALL:
	if (!get_dir_c(NULL, &dir)) return;
	fire_ball(GF_FIRE, dir, y, x, 72, 2);
	ident = TRUE;
	i_ptr->timeout = 30;
	break;

      case SV_ROD_ACID_BALL:
	if (!get_dir_c(NULL, &dir)) return;
	fire_ball(GF_ACID, dir, y, x, 60, 2);
	ident = TRUE;
	i_ptr->timeout = 27;
	break;

      case SV_ROD_MAPPING:
	map_area();
	ident = TRUE;
	i_ptr->timeout = 99;
	break;

      case SV_ROD_IDENTIFY:
	ident_spell();
	/* We know what it is now */
	ident = TRUE;
	i_ptr->timeout = 10;
	break;

      case SV_ROD_CURING:
	if (cure_blindness()) ident = TRUE;
	if (cure_poison()) ident = TRUE;
	if (cure_confusion()) ident = TRUE;
	if (p_ptr->stun > 0) {
	    msg_print("Your head stops stinging.");
	    p_ptr->stun = 0;
	    if (p_ptr->stun > 50) {
		p_ptr->ptohit += 20;
		p_ptr->ptodam += 20;
	    } else {
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
	    }
	    ident = TRUE;
	}
	else if (p_ptr->cut > 0) {
	    msg_print("You feel better.");
	    p_ptr->cut = 0;
	    ident = TRUE;
	}
	i_ptr->timeout = 888;
	break;

      case SV_ROD_HEALING:
	ident = hp_player(500);
	if (p_ptr->stun > 0) {
	    msg_print("Your head stops stinging.");
	    p_ptr->stun = 0;
	    if (p_ptr->stun > 50) {
		p_ptr->ptohit += 20;
		p_ptr->ptodam += 20;
	    } else {
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
	    }
	    ident = TRUE;
	}
	if (p_ptr->cut > 0) {
	    msg_print("You feel better.");
	    p_ptr->cut = 0;
	    ident = TRUE;
	}
	i_ptr->timeout = 888;
	break;

      case SV_ROD_RECALL:
	if (p_ptr->word_recall == 0) {
	    msg_print("The air about you becomes charged...");
	    p_ptr->word_recall = 15 + randint(20);
	}
	else {
	    msg_print("A tension leaves the air around you...");
	    p_ptr->word_recall = 0;
	}
	ident = TRUE;
	i_ptr->timeout = 60;
	break;

      case SV_ROD_PROBING:
	probing();
	ident = TRUE;
	i_ptr->timeout = 50;
	break;

      case SV_ROD_DETECTION:
	detection();
	ident = TRUE;
	i_ptr->timeout = 99;
	break;

      case SV_ROD_RESTORATION:
	if (restore_level()) ident = TRUE;
	if (res_stat(A_STR)) ident = TRUE;
	if (res_stat(A_INT)) ident = TRUE;
	if (res_stat(A_WIS)) ident = TRUE;
	if (res_stat(A_DEX)) ident = TRUE;
	if (res_stat(A_CON)) ident = TRUE;
	if (res_stat(A_CHR)) ident = TRUE;
	i_ptr->timeout = 999;
	break;

      case SV_ROD_SPEED:
	if (p_ptr->fast == 0) ident = TRUE;
	p_ptr->fast += randint(30) + 15;
	i_ptr->timeout = 99;
	break;

      case SV_ROD_TRAP_LOC:
	if (detect_trap()) ident = TRUE;
	i_ptr->timeout = 99;
	break;

#if 0
      case SV_ROD_MK_WALL:	   /* JLS */
	if (!get_dir_c(NULL, &dir)) return;
	ident = build_wall(dir, y, x);
	/* don't want people to abuse this -JLS */
	i_ptr->timeout = 999;
	break;
#endif

      default:
	msg_print("Internal error in rods() ");
	break;
    }


    /* Successfully determined the object function */
    if (ident) { if (!known1_p(i_ptr)) {
	p_ptr->exp += (lev + (p_ptr->lev >> 1)) / p_ptr->lev;
	prt_experience();
	identify(&item_val);
	}
    } else if (!known1_p(i_ptr)) sample(i_ptr);

}




void do_cmd_activate(void)
{
    int         i, flag, first, num, j, redraw, test = FALSE;
    int         a, dir;
    inven_type  *i_ptr;
    char        out_str[200], tmp[200], tmp2[200], choice;
    
    flag = FALSE;
    redraw = FALSE;
    num = 0;
    first = 0;

    for (i = 22; i < (INVEN_ARRAY_SIZE - 1); i++) {

	if ((inventory[i].flags2 & TR3_ACTIVATE) && (known2_p(&(inventory[i])))) {
	    num++;
	    if (!flag)
		first = i;
	    flag = TRUE;
	}
    }

    /* Nothing found */
    if (!flag) {
	msg_print("You are not wearing/wielding anything that can be activated.");
	free_turn_flag = TRUE;
	return;
    }

    sprintf(out_str, "Activate which item? (%c-%c, * to list, ESC to exit) ?", 'a', 'a' + (num - 1));

    flag = FALSE;
    while (!flag){
	if (!get_com(out_str, &choice))  /* on escape, get_com returns false: */
	    choice = '\033';             /* so it's set here to ESC.  Wierd huh? -CFT */

	if ((choice=='*') && !redraw) {  /* don't save screen again if it's already listed, OW it doesn't clear -CFT */
	    save_screen();
	    j=0;
	    if (!redraw) {
		for (i = first; i < (INVEN_ARRAY_SIZE - 1); i++) {
		    if ((inventory[i].flags2 & TR3_ACTIVATE) &&
			known2_p(&(inventory[i]))) {
			objdes(tmp2, &inventory[i], TRUE);
			sprintf(tmp, "%c) %-61s", 'a' + j, tmp2);
			erase_line(1 + j, 13);
/* we display at 15, but erase from 13 to give a couple of spaces,
 * so it looks tidy.  -CFT */
			sprintf(tmp, "%c) %-40s", 'a' + j, tmp2);
			prt(tmp, 1 + j, 15);
/* No need to check for bottom of screen, since only have 11 items in equip,
 * so will never reach bottom... -CFT */
			j++;
		    }
		}
		redraw = TRUE;
		continue;
	    }
	}
	else {
	    if (choice >= 'A' && choice <= ('A' + (num - 1))) {
		choice -= 'A';
		test = TRUE; /* test to see if he means it */
	    }
	    else if (choice >= 'a' && choice <= ('a' + (num - 1)))
		choice -= 'a';
	    else if (choice == '\033') {
		if (redraw) {
		    restore_screen();
		    redraw = FALSE;
		}
		free_turn_flag = TRUE;
		break;
	    } else {
		bell();
		continue;	   /* try another letter */
	    }

	    if (redraw) {
		restore_screen();
		redraw = FALSE;
	    }
	    if (choice > num) continue;
	    flag = TRUE;
	    j = 0;
	    for (i = first; i < (INVEN_ARRAY_SIZE - 1); i++) {
		if ((inventory[i].flags2 & TR3_ACTIVATE) && known2_p(&(inventory[i]))) {
		    if (j == choice)
			break;
		    j++;
		}
	    }

	    if ( (test && verify("Activate", i)) || !test)
		flag = TRUE;
	    else {
		flag = TRUE;           /* exit loop, he didn't want to try it... */
		free_turn_flag = TRUE; /* but he didn't do anything either */
		continue;
	    }

    /* Check the recharge */
	    if (inventory[i].timeout > 0) {
		msg_print("It whines, glows and fades...");
		break;
	    }

    /* Are we smart enough? */
	    if (p_ptr->use_stat[A_INT] < randint(18) &&
	     randint(k_list[inventory[i].k_idx].level) > p_ptr->lev) {
		msg_print("You fail to activate it properly.");
		break;
	    }

    /* Wonder Twin Powers... Activate! */
    msg_print("You activate it...");

	    switch (inventory[i].k_idx) {

	    case (29):
	    case (395):
	    case (396):	   /* The dreaded daggers:-> */
	    case (397):
		if (inventory[i].name2 == ART_NARTHANC) {
		    msg_print("Your dagger is covered in fire...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_FIRE, dir, char_row, char_col, damroll(9, 8));
			inventory[i].timeout = 5 + randint(10);
		    }
		}

		else if (inventory[i].name2 == ART_NIMTHANC) {
		    msg_print("Your dagger is covered in frost...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(6, 8));
			inventory[i].timeout = 4 + randint(8);
		    }
		}

		else if (inventory[i].name2 == ART_DETHANC) {
		    msg_print("Your dagger is covered in sparks...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_ELEC, dir, char_row, char_col, damroll(4, 8));
			inventory[i].timeout = 3 + randint(7);
		    }
		}

		else if (inventory[i].name2 == ART_RILIA) {
		    msg_print("Your dagger throbs deep green...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_POIS, dir, char_row, char_col, 12, 3);
			inventory[i].timeout = 3 + randint(3);
		    }
		}

		else if (inventory[i].name2 == ART_BELANGIL) {
		    msg_print("Your dagger is covered in frost...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_COLD, dir, char_row, char_col, 48, 2);
			inventory[i].timeout = 3 + randint(7);
		    }
		}
		break;

	    case (91):
		if (inventory[i].name2 == ART_DAL) {
		    msg_print("You feel energy flow through your feet...");
		    remove_fear();
		    cure_poison();
		    inventory[i].timeout = 5;
		}
		break;

	    case (42):
	    case (43):
		if (inventory[i].name2 == ART_RINGIL) {
		    msg_print("Your sword glows an intense blue...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_COLD, dir, char_row, char_col, 100, 2);
			inventory[i].timeout = 300;
		    }
		}

		else if (inventory[i].name2 == ART_ANDURIL) {
		    msg_print("Your sword glows an intense red...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_FIRE, dir, char_row, char_col, 72, 2);
			inventory[i].timeout = 400;
		    }
		}
		break;

	    case (52):
		if (inventory[i].name2 == ART_FIRESTAR) {
		    msg_print("Your morningstar rages in fire...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_FIRE, dir, char_row, char_col, 72, 3);
			inventory[i].timeout = 100;
		    }
		}
		break;

	    case (92):
		if (inventory[i].name2 == ART_FEANOR) {
		    p_ptr->fast += randint(25) + 15;
		    inventory[i].timeout = 200;
		}
		break;

	    case (59):
		if (inventory[i].name2 == ART_THEODEN) {
		    msg_print("The blade of your axe glows black...");
		    get_dir_c(NULL, &dir);
			drain_life(dir, char_row, char_col, 120);
			inventory[i].timeout = 400;
		    }
		}
		break;

	    case (62):
		if (inventory[i].name2 == ART_TURMIL) {
		    msg_print("The head of your hammer glows white...");
		    get_dir_c(NULL, &dir);
			drain_life(dir, char_row, char_col, 90);
			inventory[i].timeout = 70;
		    }
		}
		break;

	    case (111):
		if (inventory[i].name2 == ART_CASPANION) {
		    msg_print("Your mail magically disarms traps...");
		    td_destroy();
		    inventory[i].timeout = 10;
		}
		break;

	    case (71):
		if (inventory[i].name2 == ART_AVAVIR) {
		    if (p_ptr->word_recall == 0) {
			p_ptr->word_recall = 15 + randint(20);
			msg_print("The air about you becomes charged...");
		    } else {
			p_ptr->word_recall = 0;
			msg_print("A tension leaves the air around you...");
		    }
		    inventory[i].timeout = 200;
		}
		break;

	    case (53):
		if (inventory[i].name2 == ART_TARATOL) {
		    if (p_ptr->fast == 0)
			p_ptr->fast += randint(30) + 15;
		    inventory[i].timeout = 166;
		}
		break;

	    case (54):
		if (inventory[i].name2 == ART_ERIRIL) {
		    ident_spell();
		    inventory[i].timeout = 10;
		} else if (inventory[i].name2 == ART_OLORIN) {
		    probing();
		    inventory[i].timeout = 20;
		}
		break;

	    case (67):
		if (inventory[i].name2 == ART_EONWE) {
		    msg_print("Your axe lets out a long, shrill note...");
		    mass_genocide(TRUE);
		    inventory[i].timeout = 1000;
		}
		break;

	    case (68):
		if (inventory[i].name2 == ART_LOTHARANG) {
		    msg_print("Your battle axe radiates deep purple...");
		    hp_player(damroll(4, 7));
		    if (p_ptr->cut > 0) {
			p_ptr->cut = (p_ptr->cut / 2) - 50;
			if (p_ptr->cut < 0)
			    p_ptr->cut = 0;
			msg_print("You wounds heal.");
		    }
		    inventory[i].timeout = 2 + randint(2);
		}
		break;

	    case (75):
		if (inventory[i].name2 == ART_CUBRAGOL) {
		    for (a = 0; a < INVEN_WIELD; a++)
/* search for bolts that are not cursed and are not already named -CWS */
			if ((inventory[a].tval == TV_BOLT) &&
			    !(inventory[a].flags1 & TR3_CURSED) &&
			    (inventory[a].name2 == SN_NULL))
			    break;
		    if (a < INVEN_WIELD) {
			i_ptr = &inventory[a];
			msg_print("Your bolts are covered in a fiery aura!");
			i_ptr->name2 = EGO_FIRE;
			i_ptr->flags1 |= (TR1_BRAND_FIRE|TR2_RES_FIRE);
			i_ptr->cost += 25;
			enchant(i_ptr, 3+randint(3), ENCH_TOHIT|ENCH_TODAM);
			calc_bonuses();
		    } else {
			msg_print("The fiery enchantment fails.");
		    }
		    inventory[i].timeout = 999;
		}
		break;

	    case (34):
	    case (35):
		if (inventory[i].name2 == ART_ARUNRUTH) {
		    msg_print("Your sword glows a pale blue...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(12, 8));
			inventory[i].timeout = 500;
		    }
		}
		break;

	    case (64):
		if (inventory[i].name2 == ART_AEGLOS) {
		    msg_print("Your spear glows a bright white...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_COLD, dir, char_row, char_col, 100, 2);
			inventory[i].timeout = 500;
		    }
		}

		else if (inventory[i].name2 == ART_OROME) {
		    msg_print("Your spear pulsates...");
		    get_dir_c(NULL, &dir);
			wall_to_mud(dir, char_row, char_col);
			inventory[i].timeout = 5;
		    }
		}
		break;

	    case (118):
		if (inventory[i].name2 == ART_SOULKEEPER) {
		    msg_print("Your armour glows a bright white...");
		    msg_print("You feel much better...");
		    hp_player(1000);
		    inventory[i].timeout = 888;
		}
		break;

	    case (120):
		if (inventory[i].name2 == ART_BELEGENNON) {
		    teleport(10);
		    inventory[i].timeout = 2;
		}
		break;

	    case (119):
		if (inventory[i].name2 == ART_CELEBORN) {
		    genocide(TRUE);
		    inventory[i].timeout = 500;
		}
		break;

	    case (124):
		if (inventory[i].name2 == ART_LUTHIEN) {
		    restore_level();
		    inventory[i].timeout = 450;
		}
		break;

	    case (65):
		if (inventory[i].name2 == ART_ULMO) {
		    msg_print("Your trident glows deep red...");
		    get_dir_c(NULL, &dir);
			teleport_monster(dir, char_row, char_col);
			inventory[i].timeout = 150;
		    }
		}
		break;

	    case (123):	   /* Cloak */
	    case (411):
		if (inventory[i].name2 == ART_COLLUIN) {
		    msg_print("Your cloak glows many colours...");
		    msg_print("You feel you can resist anything.");
		    p_ptr->oppose_fire += randint(20) + 20;
		    p_ptr->oppose_cold += randint(20) + 20;
		    p_ptr->oppose_elec += randint(20) + 20;
		    p_ptr->oppose_pois += randint(20) + 20;
		    p_ptr->oppose_acid += randint(20) + 20;
		    inventory[i].timeout = 111;
		}

		else if (inventory[i].name2 == ART_HOLCOLLETH) {
		    msg_print("You momentarily disappear...");
		    sleep_monsters1(char_row, char_col);
		    inventory[i].timeout = 55;
		}

		else if (inventory[i].name2 == ART_THINGOL) {
		    msg_print("You hear a low humming noise...");
		    recharge(60);
		    inventory[i].timeout = 70;
		}

		else if (inventory[i].name2 == ART_COLANNON) {
		    teleport(100);
		    inventory[i].timeout = 45;
		}
		break;

	    case (50):	   /* Flail */
		if (inventory[i].name2 == ART_TOTILA) {
		    msg_print("Your flail glows in scintillating colours...");
		    get_dir_c(NULL, &dir);
			confuse_monster(dir, char_row, char_col, 20);
			inventory[i].timeout = 15;
		    }
		}
		break;

	    case (125):	   /* Gloves */
		if (inventory[i].name2 == ART_CAMMITHRIM) {
		    msg_print("Your gloves glow extremely brightly...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_MISSILE, dir, char_row, char_col,
				  damroll(2, 6));
			inventory[i].timeout = 2;
		    }
		}
		break;

	    case (126):	   /* Gauntlets */
		if (inventory[i].name2 == ART_PAURHACH) {
		    msg_print("Your gauntlets are covered in fire...");
		    get_dir_c(NULL, &dir);
			if (randint(4)==1)
			    line_spell(GF_FIRE, dir, char_row, char_col, damroll(9,8));
			else
			    fire_bolt(GF_FIRE, dir, char_row, char_col, damroll(9,8));
			inventory[i].timeout = 5 + randint(10);
		    }
		}

		else if (inventory[i].name2 == ART_PAURNIMMEN) {
		    msg_print("Your gauntlets are covered in frost...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_COLD, dir, char_row, char_col, damroll(6, 8));
			inventory[i].timeout = 4 + randint(8);
		    }
		}

		else if (inventory[i].name2 == ART_PAURAEGEN) {
		    msg_print("Your gauntlets are covered in sparks...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_ELEC, dir, char_row, char_col, damroll(4, 8));
			inventory[i].timeout = 3 + randint(7);
		    }
		}

		else if (inventory[i].name2 == ART_PAURNEN) {
		    msg_print("Your gauntlets look very acidic...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_ACID, dir, char_row, char_col, damroll(5, 8));
			inventory[i].timeout = 4 + randint(7);
		    }
		}
		break;

	    case (127):
		if (inventory[i].name2 == ART_FINGOLFIN) {
		    msg_print("Magical spikes appear on your cesti...");
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_ARROW, dir, char_row, char_col, 150);
			inventory[i].timeout = 88 + randint(88);
		    }
		}
		break;

	    case (96):
		if (inventory[i].name2 == ART_HOLHENNETH) {
		    msg_print("You close your eyes and an image forms in your mind...");
		    detection();
		    inventory[i].timeout = 55 + randint(55);
		}
		break;

	    case (99):
		if (inventory[i].name2 == ART_GONDOR) {
		    msg_print("You feel a warm tingling inside...");
		    hp_player(500);
		    inventory[i].timeout = 500;
		}
		break;

	    case (OBJ_SPECIAL - 1):	/* Narya */
		msg_print("The ring glows deep red...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_FIRE, dir, char_row, char_col, 120, 3);
		    inventory[i].timeout = 222 + randint(222);
		}
		break;

	    case (OBJ_SPECIAL): /* Nenya */
		msg_print("The ring glows bright white...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_COLD, dir, char_row, char_col, 200, 3);
		    inventory[i].timeout = 222 + randint(333);
		}
		break;

	    case (OBJ_SPECIAL + 1):	/* Vilya */
		msg_print("The ring glows deep blue...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_ELEC, dir, char_row, char_col, 250, 3);
		    inventory[i].timeout = 222 + randint(444);
		}
		break;

	    case (OBJ_SPECIAL + 2):	/* Power */
		msg_print("The ring glows intensely black...");
		switch (randint(17) + (8 - p_ptr->lev / 10)) {
		  case 5:
		    dispel_creature(0xFFFFFFFL, 1000);
		    break;
		  case 6:
		  case 7:
		    msg_print("You are surrounded by a malignant aura");
		    p_ptr->lev--;
		    /* XXX Convert to "rand_range()" */
		    p_ptr->exp = (player_exp[p_ptr->lev - 2] *
				  p_ptr->expfact / 100) +
				 randint((player_exp[p_ptr->lev - 1] *
					  p_ptr->expfact / 100) -
					 (player_exp[p_ptr->lev - 2] *
					  p_ptr->expfact / 100));
		    p_ptr->max_exp = p_ptr->exp;
		    prt_experience();
		    ruin_stat(A_STR);
		    ruin_stat(A_INT);
		    ruin_stat(A_WIS);
		    ruin_stat(A_DEX);
		    ruin_stat(A_CON);
		    ruin_stat(A_CHR);
		    calc_hitpoints();
		    if (class[p_ptr->pclass].spell == MAGE) {
			calc_spells(A_INT);
			calc_mana(A_INT);
		    }
		    else if (class[p_ptr->pclass].spell == PRIEST) {
			calc_spells(A_WIS);
			calc_mana(A_WIS);
		    }
		    prt_level();
		    prt_title();
		    take_hit((p_ptr->chp > 2) ? p_ptr->chp / 2 : 0,
			     "malignant aura");
		    break;
		  case 8:
		  case 9:
		  case 10:
		    get_dir_c(NULL, &dir);
			fire_ball(GF_MANA, dir, char_row, char_col, 300, 3);
		    }
		    break;
		  default:
		    get_dir_c(NULL, &dir);
			fire_bolt(GF_MANA, dir, char_row, char_col, 250);
		    }
		}
		inventory[i].timeout = 444 + randint(444);
		break;

	    case (389):	   /* Blue */
		msg_print("You breathe lightning...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_ELEC, dir, char_row, char_col, 100, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (390):	   /* White */
		msg_print("You breathe frost...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_COLD, dir, char_row, char_col, 110, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (391):	   /* Black */
		msg_print("You breathe acid...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_ACID, dir, char_row, char_col, 130, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (392):	   /* Gas */
		msg_print("You breathe poison gas...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_POIS, dir, char_row, char_col, 150, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (393):	   /* Fire */
		msg_print("You breathe fire...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_FIRE, dir, char_row, char_col, 200, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (394):	   /* Multi-hued */
		if (inventory[i].name2 == ART_RAZORBACK) {
		    msg_print("A storm of lightning spikes fires in all directions...");
		    starball(char_row, char_col);
		    inventory[i].timeout = 1000;
		}

		else {
		    get_dir_c(NULL, &dir);
			choice = randint(5);
			sprintf(tmp2, "You breathe %s...",
				((choice == 1) ? "lightning" :
				 ((choice == 2) ? "frost" :
				  ((choice == 3) ? "acid" :
				((choice == 4) ? "poison gas" : "fire")))));
			msg_print(tmp2);
			fire_ball(((choice == 1) ? GF_ELEC :
				   ((choice == 2) ? GF_COLD :
				    ((choice == 3) ? GF_ACID :
			       ((choice == 4) ? GF_POIS : GF_FIRE)))),
				  dir, char_row, char_col, 250, 2);
			inventory[i].timeout = 222 + randint(222);
		    }
		}
		break;

	    case (408):	   /* Bronze */
		msg_print("You breathe confusion...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_CONFUSION, dir, char_row, char_col, 120, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (409):	   /* Gold */
		msg_print("You breathe sound...");
		if (get_dir_c(NULL, &dir)) {
		    fire_ball(GF_SOUND, dir, char_row, char_col, 130, 2);
		    inventory[i].timeout = 444 + randint(444);
		}
		break;

	    case (415):	   /* Chaos */
		if (get_dir_c(NULL, &dir)) {
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "chaos" : "disenchantment")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_CHAOS : GF_DISENCHANT), dir,
			      char_row, char_col, 220, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;

	    case (416):	   /* Law */
		if (get_dir_c(NULL, &dir)) {
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "sound" : "shards")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_SOUND : GF_SHARDS), dir,
			      char_row, char_col, 230, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;

	    case (417):	   /* Balance */
		if (get_dir_c(NULL, &dir)) {
		    choice = randint(4);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1) ? "chaos" :
			     ((choice == 2) ? "disenchantment" :
			      ((choice == 3) ? "sound" : "shards"))));
		    msg_print(tmp2);
		    fire_ball(((choice == 1) ? GF_CHAOS :
			       ((choice == 2) ? GF_DISENCHANT :
				((choice == 3) ? GF_SOUND : GF_SHARDS))),
			      dir, char_row, char_col, 250, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;

	    case (418):	   /* Shining */
		if (get_dir_c(NULL, &dir)) {
		    choice = randint(2);
		    sprintf(tmp2, "You breathe %s...",
			    ((choice == 1 ? "light" : "darkness")));
		    msg_print(tmp2);
		    fire_ball((choice == 1 ? GF_LITE : GF_DARK), dir,
			      char_row, char_col, 200, 2);
		    inventory[i].timeout = 300 + randint(300);
		}
		break;

	    case (419):	   /* Power Dragon Scale Mail */
		if (inventory[i].name2 == ART_BLADETURNER) {
		    msg_print("Your armour glows many colours...");
		    msg_print("You enter a berserk rage...");
		    p_ptr->hero += randint(50) + 50;
		    p_ptr->shero += randint(50) + 50;
		    bless(randint(50) + 50);
		    p_ptr->oppose_fire += randint(50) + 50;
		    p_ptr->oppose_cold += randint(50) + 50;
		    p_ptr->oppose_elec += randint(50) + 50;
		    p_ptr->oppose_acid += randint(50) + 50;
		    inventory[i].timeout = 400;
		}

		else {
		    msg_print("You breathe the elements...");
		    get_dir_c(NULL, &dir);
			fire_ball(GF_MISSILE, dir, char_row, char_col, 300, 2);
			inventory[i].timeout = 300 + randint(300);
		    }
		}
		break;

	    case (OBJ_SPECIAL + 3):
		msg_print("The phial wells with clear light...");
		lite_area(char_row, char_col, damroll(2, 15), 3);
		inventory[i].timeout = 10 + randint(10);
		break;

	    case (OBJ_SPECIAL + 4):
		msg_print("An aura of good floods the area...");
		dispel_creature(MF2_EVIL, (int)(5 * p_ptr->lev));
		inventory[i].timeout = 444 + randint(222);
		break;

	    case (OBJ_SPECIAL + 5):
		msg_print("The amulet lets out a shrill wail...");
		msg_print("You feel somewhat safer...");
		protect_evil();
		inventory[i].timeout = 222 + randint(222);
		break;

	    case (OBJ_SPECIAL + 6):
		msg_print("The star shines brightly...");
		msg_print("And you sense your surroundings...");
		map_area();
		inventory[i].timeout = 50 + randint(50);
		break;

	    case (OBJ_SPECIAL + 7):
		msg_print("The stone glows a deep green");
		wiz_lite(TRUE);
		(void)detect_sdoor();
		(void)detect_trap();
		inventory[i].timeout = 100 + randint(100);
		break;

	    case (OBJ_SPECIAL + 8):
		msg_print("The ring glows brightly...");
		p_ptr->fast += randint(100) + 50;
		inventory[i].timeout = 200;
		break;

	    default:
		(void)sprintf(tmp2, "Inventory num %d, index %d", i,
			      inventory[i].k_idx);
		msg_print(tmp2);
	    }
	}
    }

    if (redraw) {
	restore_screen();
	redraw = FALSE;
    }
    if (!flag)			   /* if flag still false, then user aborted. 
				    * So we don't charge him a turn. -CFT */
	free_turn_flag = TRUE;
}

