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
 * XXX XXX XXX XXX Someone needs to verify all of these effects.
 */



/*
 * Lose a strength point.				-RAK-	 
 */
static void lose_str()
{
    if (!py.flags.sustain_str) {
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
    if (!py.flags.sustain_int) {
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
    if (!py.flags.sustain_wis) {
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
    if (!py.flags.sustain_dex) {
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
    if (!py.flags.sustain_con) {
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
    if (!py.flags.sustain_chr) {
	(void)dec_stat(A_CHR);
	msg_print("Your skin starts to itch.");
    }
    else {
	msg_print("Your skin starts to itch, but feels better now.");
    }
}



/*
 * Eat some food. -RAK-
 */
void do_cmd_eat_food(void)
{
    u32b                 flg;
    int			   j, k, item_val, ident;
    register struct flags *f_ptr = &py.flags;
    register struct misc  *m_ptr = &py.misc;;
    register inven_type   *i_ptr;

    /* Assume the turn is free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_FOOD, TV_NEVER, &j, &k)) {
	msg_print("You are not carrying any food.");
	return;
    }

    /* Get a food */
    if (!get_item(&item_val, "Eat what?", j, k, 0)) return;

    /* Get the item */
    i_ptr = &inventory[item_val];

	free_turn_flag = FALSE;

    /* Identity not known yet */
    ident = FALSE;

    /* Apply all of the food flags */
    for (flg = i_ptr->flags; flg; ) {

	/* Extract the next "effect" bit */
	j = bit_pos(&flg) + 1;

	/* Analyze the effect */
	switch (j) {

	  case 1:
	    if (!f_ptr->resist_pois)
		f_ptr->poisoned += randint(10) + i_ptr->level;
		ident = TRUE;
	    break;

	  case 2:
	    if (!py.flags.resist_blind) {
		f_ptr->blind += randint(250) + 10 * i_ptr->level + 100;
		draw_cave();
		msg_print("A veil of darkness surrounds you.");
		ident = TRUE;
	    }
	    break;

	  case 3:
	    if (!py.flags.resist_fear) {
		f_ptr->afraid += randint(10) + i_ptr->level;
		msg_print("You feel terrified!");
		ident = TRUE;
	    }
	    break;

	  case 4:
	    if ((!py.flags.resist_conf) && (!py.flags.resist_chaos)) {
		f_ptr->confused += randint(10) + i_ptr->level;
		msg_print("You feel drugged.");
	    }
		ident = TRUE;
	    break;

	  case 5:
	    f_ptr->image += randint(200) + 25 * i_ptr->level + 200;
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
	    if (f_ptr->afraid > 1) {
		f_ptr->afraid = 1;
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

    if (ident) {
	    if (!known1_p(i_ptr)) {
	    /* use identified it, gain experience */

	    /* round half-way case up */
		m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
		prt_experience();
		identify(&item_val);
		i_ptr = &inventory[item_val];
	    }
	}

    else if (!known1_p(i_ptr))
	    sample(i_ptr);
	add_food(i_ptr->p1);
	py.flags.status &= ~(PY_WEAK | PY_HUNGRY);
	prt_hunger();
	inven_item_describe(item_val);
	inven_destroy(item_val);
}





/*
 * Quaff a potion
 */
void do_cmd_quaff_potion(void)
{
    u32b i, l;
    int    j, k, item_val;
    int    ident;
    register inven_type   *i_ptr;
    register struct misc  *m_ptr = &py.misc;
    register struct flags *f_ptr = &py.flags;

    /* Assume the turn will be free */
    free_turn_flag = TRUE;

    if (inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
	return;
    }

    if (!find_range(TV_POTION1, TV_POTION2, &j, &k)) {
	msg_print("You are not carrying any potions.");
	return;
    }

    /* Get a potion */
    if (!get_item(&item_val, "Quaff which potion?", j, k, 0)) return; {

    /* Get the item */
    i_ptr = &inventory[item_val];

	i = i_ptr->flags;
	free_turn_flag = FALSE;

    /* Not identified yet */
    ident = FALSE;

    /* Note potions with no effects */
    if (i_ptr->flags == 0) {
	msg_print("You feel less thirsty.");
	ident = TRUE;
    }

    else while (i != 0) {

		j = bit_pos(&i);
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
	    if (py.flags.cut > 0) {
		msg_print("Your wounds heal.");
		py.flags.cut -= 10;
		if (py.flags.cut < 0) py.flags.cut = 0;
		ident = TRUE;
	    }
	    break;

	  case 14:
	    if (hp_player(damroll(4, 7))) ident = TRUE;
	    if (py.flags.cut > 0) {
		msg_print("Your wounds heal.");
		py.flags.cut = (py.flags.cut / 2) - 50;
		if (py.flags.cut < 0) py.flags.cut = 0;
		ident = TRUE;
	    }
	    break;

	  case 15:
	    if (hp_player(damroll(6, 7))) ident = TRUE;
	    if (py.flags.cut > 0) {
		msg_print("Your wounds heal.");
		py.flags.cut = 0;
		ident = TRUE;
	    }
	    if (py.flags.stun > 0) {
		msg_print("Your head stops stinging.");
		py.flags.stun = 0;
		if (py.flags.stun > 50) {
		    py.misc.ptohit += 20;
		    py.misc.ptodam += 20;
		} else {
		    py.misc.ptohit += 5;
		    py.misc.ptodam += 5;
		}
		ident = TRUE;
	    }
	    break;

	  case 16:
	    if (hp_player(400)) ident = TRUE;
	    if (py.flags.stun > 0) {
		msg_print("Your head stops stinging.");
		py.flags.stun = 0;
		if (py.flags.stun > 50) {
		    py.misc.ptohit += 20;
		    py.misc.ptodam += 20;
		} else {
		    py.misc.ptohit += 5;
		    py.misc.ptodam += 5;
		}
		ident = TRUE;
	    }
	    if (py.flags.cut > 0) {
		msg_print("Your wounds heal.");
		py.flags.cut = 0;
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
	    if (m_ptr->exp < MAX_EXP) {
		l = (m_ptr->exp / 2) + 10;
		if (l > 100000L) l = 100000L;
		m_ptr->exp += l;
		msg_print("You feel more experienced.");
		prt_experience();
		ident = TRUE;
	    }
	    break;

	  case 19:;
	    if (!f_ptr->free_act) {
		/* paralysis must be zero, we are drinking */
		/* but what about multiple potion effects? */
		msg_print("You fall asleep.");
		f_ptr->paralysis += randint(4) + 4;
		ident = TRUE;
	    }
	    break;

	  case 20:
	    if (!py.flags.resist_blind) {
		if (f_ptr->blind == 0) {
		    msg_print("You are covered by a veil of darkness.");
		    ident = TRUE;
		}
		f_ptr->blind += randint(100) + 100;
	    }
	    break;

	  case 21:
	    if (!f_ptr->resist_conf) {
		if (f_ptr->confused == 0) {
		    msg_print("Hey!  This is good stuff!  * Hick! *");
		    ident = TRUE;
		}
		f_ptr->confused += randint(20) + 12;
	    }
	    break;

	  case 22:
	    if (!(f_ptr->immune_pois ||
		  f_ptr->resist_pois ||
		  f_ptr->oppose_pois)) {
		msg_print("You feel very sick.");
		f_ptr->poisoned += randint(15) + 10;
	    }
	    else {
		msg_print("The poison has no effect.");
	    }
	    if (!f_ptr->resist_pois)
	    ident = TRUE;
	    break;

	  case 23:
	    if (py.flags.fast == 0) ident = TRUE;
	    if (py.flags.fast <= 0) {
		py.flags.fast += randint(25) + 15;
	    } else
		py.flags.fast += randint(5);
	    break;

	  case 24:
	    if (py.flags.slow == 0) ident = TRUE;
	    py.flags.slow += randint(25) + 15;
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
	    if (!py.flags.hold_life && py.misc.exp > 0) {
		s32b               m, scale;

		msg_print("You feel your memories fade.");
		m = py.misc.exp / 5;
		if (py.misc.exp > MAX_SHORT) {
		    scale = MAX_LONG / py.misc.exp;
		    m += (randint((int)scale) * py.misc.exp) / (scale * 5);
		}
		else {
		    m += randint((int)py.misc.exp) / 5;
		}
		lose_exp(m);
	    }
	    else
		msg_print("You feel you memories fade for a moment, but quickly return.");
	    ident = TRUE;
	    break;

	  case 35:
	    (void)cure_poison();
	    if (f_ptr->food > 150) f_ptr->food = 150;
	    f_ptr->paralysis = 4;
	    msg_print("The potion makes you vomit! ");
	    ident = TRUE;
	    break;

	  case 37:
	    if (py.flags.hero == 0) ident = TRUE;
	    py.flags.hero += randint(25) + 25;
	    break;

	  case 38:
	    if (py.flags.shero == 0) ident = TRUE;
	    py.flags.shero += randint(25) + 25;
	    break;

	  case 39:
	    if (remove_fear()) ident = TRUE;
	    break;

	  case 40:
	    if (restore_level()) ident = TRUE;
	    break;

	  case 41:
	    if (!f_ptr->oppose_fire) ident = TRUE;
	    f_ptr->oppose_fire += randint(10) + 10;
	    break;

	  case 42:
	    if (!f_ptr->oppose_cold) ident = TRUE;
	    f_ptr->oppose_cold += randint(10) + 10;
	    break;

	  case 43:
	    if (!py.flags.detect_inv) ident = TRUE;
	    detect_inv2(randint(12) + 12);
	    break;

	  case 44:
	    if (slow_poison()) ident = TRUE;
	    break;

	  case 45:
	    if (cure_poison()) ident = TRUE;
	    break;

	  case 46:
	    if (m_ptr->cmana < m_ptr->mana) {
		m_ptr->cmana = m_ptr->mana;
		msg_print("Your feel your head clear.");
		prt_cmana();
		ident = TRUE;
	    }
	    break;

	  case 47:
	    if (f_ptr->tim_infra == 0) {
		msg_print("Your eyes begin to tingle.");
		ident = TRUE;
	    }
	    f_ptr->tim_infra += 100 + randint(100);
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
	    (py.flags.stun > 0) |
	    (py.flags.cut > 0) |
	    (py.flags.image > 0) |
	    remove_fear()) {
	    py.flags.cut = 0;
	    py.flags.image = 0;
	    if (py.flags.stun > 0) {
		    if (py.flags.stun > 50) {
			py.misc.ptohit += 20;
			py.misc.ptodam += 20;
		    } else {
			py.misc.ptohit += 5;
			py.misc.ptodam += 5;
		    }
		    py.flags.stun = 0;
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
	    if (py.flags.stun > 0) {
		py.flags.stun = 0;
		msg_print("Your head stops stinging.");
		if (py.flags.stun > 50) {
		    py.misc.ptohit += 20;
		    py.misc.ptodam += 20;
		} else {
		    py.misc.ptohit += 5;
		    py.misc.ptodam += 5;
		}
		ident = TRUE;
	    }
	    if (py.flags.cut > 0) {
		py.flags.cut = 0;
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
    if (ident) {
    if (!known1_p(i_ptr)) {
	int lev = i_ptr->level;
	m_ptr->exp += (lev + (m_ptr->lev >> 1)) / m_ptr->lev;
	prt_experience();
	identify(&item_val);
	i_ptr = &inventory[item_val];
	}
	}
	else if (!known1_p(i_ptr)) sample(i_ptr);

    /* Potions can feed the player */
	add_food(i_ptr->p1);

    /* Destroy the potion */
	inven_item_describe(item_val);
	inven_destroy(item_val);
    }
}


