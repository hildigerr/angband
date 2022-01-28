/* File: dungeon.c */ 

/* Purpose: the main command interpreter, updating player status */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

static char original_commands();
static void do_command();
static int  valid_countcommand();

/* ANGBAND game module					-RAK-	 */
/* The code in this section has gone through many revisions, and */
/* some of it could stand some more hard work.	-RAK-	       */

/* It has had a bit more hard work.			-CJS- */







/*
 * Given an item, return a textual "feeling" about the item.
 */
static cptr value_check(inven_type *i_ptr)
{
    /* Paranoia -- No item */
    if (i_ptr->tval == TV_NOTHING) return (NULL);

    /* Known items need no feeling */
    if (known2_p(i_ptr)) return (NULL);

    if (store_bought_p(i_ptr)) return 0;

    if (i_ptr->ident & ID_MAGIK) return 0;

    if (i_ptr->ident & ID_DAMD) return 0;

    if (i_ptr->inscrip[0] != '\0') return 0;

    /* Cursed items (including artifacts/ego-weapons) */
    if (i_ptr->flags1 & TR3_CURSED ) {

    if (i_ptr->name2 == SN_NULL) return "worthless";

    if (i_ptr->name2 != SN_NULL) return "terrible";
    }


    /* Hack -- "good" digging tools -CFT */
    if ((i_ptr->tval == TV_DIGGING) && (i_ptr->flags1 & TR1_TUNNEL) &&
	(i_ptr->pval > k_list[i_ptr->index].pval)) {
	return "good";
    }

    if ((i_ptr->tohit<=0 && i_ptr->todam<=0 && i_ptr->toac<=0) &&
	i_ptr->name2==SN_NULL)  /* normal shovels will also reach here -CFT */
	return "average";

    if (i_ptr->name2 == SN_NULL)
	return "good";

    if ((i_ptr->name2 == EGO_R) || (i_ptr->name2 == EGO_RESIST_A) ||
	(i_ptr->name2 == EGO_RESIST_F) || (i_ptr->name2 == EGO_RESIST_C) ||
	(i_ptr->name2 == EGO_RESIST_E) || (i_ptr->name2 == EGO_SLAY_E) ||
	(i_ptr->name2 == EGO_HA) || (i_ptr->name2 == EGO_FT) ||
	(i_ptr->name2 == EGO_DF) || (i_ptr->name2 == EGO_FB) ||
	(i_ptr->name2 == EGO_SLAY_A) || (i_ptr->name2 == EGO_FREE_ACTION) ||
	(i_ptr->name2 == EGO_SLAY_D) || (i_ptr->name2 == EGO_SLAYING) ||
	(i_ptr->name2 == EGO_SLAY_U) || (i_ptr->name2 == EGO_SLOW_DESCENT) ||
	(i_ptr->name2 == EGO_SPEED) || (i_ptr->name2 == EGO_STEALTH) ||
	(i_ptr->name2 == EGO_INTELLIGENCE) || (i_ptr->name2 == EGO_WISDOM) ||
	(i_ptr->name2 == EGO_INFRAVISION) || (i_ptr->name2 == EGO_MIGHT) ||
	(i_ptr->name2 == EGO_LORDLINESS) || (i_ptr->name2 == EGO_MAGI) ||
	(i_ptr->name2 == EGO_BEAUTY) || (i_ptr->name2 == EGO_SEEING) ||
     (i_ptr->name2 == EGO_REGENERATION) || (i_ptr->name2 == EGO_PROTECTION) ||
	(i_ptr->name2 == EGO_FIRE) || (i_ptr->name2 == EGO_SLAY_EVIL) ||
	(i_ptr->name2 == EGO_DRAGON_SLAYING) || (i_ptr->name2 == EGO_SLAY_ANIMAL) ||
	(i_ptr->name2 == EGO_ACCURACY) || (i_ptr->name2 == EGO_SLAY_O) ||
	(i_ptr->name2 == ART_POWER) || (i_ptr->name2 == EGO_WEST) ||
	(i_ptr->name2 == EGO_SLAY_DEMON) || (i_ptr->name2 == EGO_SLAY_T) ||
	(i_ptr->name2 == EGO_LIGHT) || (i_ptr->name2 == EGO_AGILITY) ||
	(i_ptr->name2 == EGO_SLAY_G) || (i_ptr->name2 == EGO_TELEPATHY) ||
	(i_ptr->name2 == EGO_DRAGONKIND) || (i_ptr->name2 == EGO_AMAN) ||
	(i_ptr->name2 == EGO_ELVENKIND) || (i_ptr->name2 == EGO_WOUNDING) ||
	(i_ptr->name2 == EGO_BLESS_BLADE) || (i_ptr->name2 == EGO_ATTACKS))
	return "excellent";

    return "special";
}


/*
 * Regenerate hit points				-RAK-	 
 */
static void regenhp(int percent)
{
    register s32b        new_chp, new_chp_frac;
    int                   old_chp;

    old_chp = p_ptr->chp;
    new_chp = ((long)p_ptr->mhp) * percent + PLAYER_REGEN_HPBASE;
    p_ptr->chp += new_chp >> 16;   /* div 65536 */

    /* check for overflow */
    if (p_ptr->chp < 0 && old_chp > 0) p_ptr->chp = MAX_SHORT;
    new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;	/* mod 65536 */
    if (new_chp_frac >= 0x10000L) {
	p_ptr->chp_frac = new_chp_frac - 0x10000L;
	p_ptr->chp++;
    }
    else {
	p_ptr->chp_frac = new_chp_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->chp >= p_ptr->mhp) {
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;
    }

    /* Notice changes */
    if (old_chp != p_ptr->chp) prt_chp();
}


/* 
 * Regenerate mana points				-RAK-	 
 */
static void regenmana(int percent)
{
    register s32b        new_mana, new_mana_frac;
    int                   old_cmana;

    old_cmana = p_ptr->cmana;
    new_mana = ((long)p_ptr->mana) * percent + PLAYER_REGEN_MNBASE;
    p_ptr->cmana += new_mana >> 16;	/* div 65536 */
    /* check for overflow */
    if (p_ptr->cmana < 0 && old_cmana > 0) {
	p_ptr->cmana = MAX_SHORT;
    }
    new_mana_frac = (new_mana & 0xFFFF) + p_ptr->cmana_frac;	/* mod 65536 */
    if (new_mana_frac >= 0x10000L) {
	p_ptr->cmana_frac = new_mana_frac - 0x10000L;
	p_ptr->cmana++;
    }
    else {
	p_ptr->cmana_frac = new_mana_frac;
    }

    /* Must set frac to zero even if equal */
    if (p_ptr->cmana >= p_ptr->mana) {
	p_ptr->cmana = p_ptr->mana;
	p_ptr->cmana_frac = 0;
    }
    
    /* Redraw mana */
    if (old_cmana != p_ptr->cmana) prt_cmana();
}



static void regen_monsters(void)
{
    register int i;

    /* Regenerate everyone */
    for (i = 0; i < MAX_M_IDX; i++) {

	/* Paranoia -- Skip "dead" monsters */
	if (m_list[i].hp < 0) continue;

	    if (m_list[i].maxhp == 0) {	/* then we're just going to fix it!  -CFT */
		if ((r_list[m_list[i].r_idx].cflags2 & MF2_MAX_HP) )
		    m_list[i].maxhp = max_hp(r_list[m_list[i].r_idx].hd);
		else
		    m_list[i].maxhp = pdamroll(r_list[m_list[i].r_idx].hd);
	    }

	/* Allow regeneration */
	if (m_list[i].hp < m_list[i].maxhp) {
	    int frac = 2 * m_list[i].maxhp / 100;
	    if (!frac) frac = 1;
	    m_list[i].hp += frac;
	    if (m_list[i].hp > m_list[i].maxhp) m_list[i].hp = m_list[i].maxhp;
	}
    }
}



/*
 * This is the main function of this file -- it places the user on the
 * current level and processes user input until the level is completed,
 * the user dies, or the game is terminated.
 */ 
void dungeon(void)
{
    int                    find_count, i;

    register cave_type		*c_ptr;
    register inven_type		*i_ptr;

    /* Regenerate hp and mana */
    int                    regen_amount;


    char                   command;      /* Last command           */

    i_ptr = &inventory[INVEN_LIGHT];

/* Check light status for setup	   */
    if (i_ptr->pval > 0 || p_ptr->light)
	player_light = TRUE;
    else
	player_light = FALSE;
    if (i_ptr->tval == TV_LITE)
	i = i_ptr->sval;
    else
	i = 195;
    cur_lite = 1 + (i < 190) + (i == 4 || i == 6);

/* Check for a maximum level		   */
/* Added check to avoid -50' being "deepest", since max_dlv unsigned -CFT */
    if ((dun_level >= 0) && ((unsigned) dun_level > p_ptr->max_dlv))
	p_ptr->max_dlv = dun_level;

    /* Reset flags and initialize variables (most of it is overkill) */
    new_level_flag	= FALSE;
    teleport_flag	= FALSE;
    find_flag		= FALSE;

    command_rep		= 0;

    eof_flag       = FALSE;
    find_count     = 0;
    mon_tot_mult   = 0;
    old_lite        = (-1);
    coin_type      = 0;
    opening_chest  = FALSE;

#ifdef TARGET
    /* target code taken from Morgul -CFT */
    target_mode = FALSE;
#endif

    cave[char_row][char_col].m_idx = 1;

    if (create_up_stair && (dun_level == 0))	/* just in case... */
	create_up_stair = FALSE;

    /* Make a stairway. */
    if (create_up_stair || create_down_stair) {

	register int        cur_pos;

	c_ptr = &cave[char_row][char_col];
	i_ptr = &i_list[c_ptr->i_idx];

	if (valid_grid(char_row, char_col)) {
	    if (c_ptr->i_idx != 0)
	    delete_object(char_row, char_col);
	    cur_pos = i_pop();
	    i_ptr = &i_list[cur_pos];
	    c_ptr->i_idx = cur_pos;
	    if (create_up_stair) {
		invcopy(i_ptr, OBJ_UP_STAIR);
	    }
	    else if (create_down_stair && !is_quest(dun_level)) {
		invcopy(i_ptr, OBJ_DOWN_STAIR);
	    }
	} else
	    msg_print("The object resists your attempt to transform it into a stairway.");

	/* Cancel the stair request */
	create_down_stair = FALSE;
	create_up_stair = FALSE;
    }

/* Ensure we display the panel. Used to do this with a global var. -CJS- */
    panel_row = panel_col = (-1);

    /* Check the view */
    check_view();

/* must do this after panel_row/col set to -1, because search_off() will call
 * check_view(), and so the panel_* variables must be valid before
 * search_off() is called 
 */
	search_off();

    /* Update the monsters */
    update_monsters();

    /* Print the depth */
    prt_depth();

/* FIXME: figure this out */
    if (((turn - old_turn) > randint(50) + 50) && dun_level) {
	unfelt = FALSE;
	do_cmd_feeling();
    }
    old_turn = turn;


    /*** Process this dungeon level ***/


    /* Loop until the character, level, or game, dies */

    do {

	/*** Handle real turns for the player ***/
	
	/* Advance the turn counter */
	turn++;

	/*** Check the Load ***/

#ifdef CHECK_HOURS
#ifndef MAC
    /* The Mac ignores the game hours file		 */
	/* Check for game hours			       */
	if (((turn % 100) == 1) && !check_time()) {
	    if (closing_flag > 2) {
		msg_print("The gates to ANGBAND are now closed.");
		(void)strcpy(died_from, "(closing gate: saved)");
		if (!save_player()) {
		(void)strcpy(died_from, "a slammed gate");
		death = TRUE;
		}
		exit_game();
	    }
	    else {
		disturb(0, 0);
		closing_flag++;
		msg_print("The gates to ANGBAND are closing due to high load.");
		msg_print("Please finish up or save your game.");
	    }
	}
#endif
#endif

	/*** Update the Stores ***/
	
    /* turn over the store contents every, say, 1000 turns */
	/* Update the stores once a day */
	if ((dun_level) && ((turn % 1000) == 0)) {

	/* if (peek) msg_print("Store update: "); */

	    store_maint();

	/* if (peek) msg_print("Complete "); */
	}


	/*** Make, and Heal, the Monsters ***/

	/* Check for creature generation */
	if (randint(MAX_M_ALLOC_CHANCE) == 1) {
	    alloc_monster(1, MAX_SIGHT, FALSE);
	}

	/* Check for creature regeneration */
	if (!(turn % 20)) regen_monsters();


	/*** Handle the Lights ***/

	/* Check for light being wielded */
	i_ptr = &inventory[INVEN_LIGHT];

	if (player_light)
	    if (i_ptr->pval > 0) {
		if (!(i_ptr->flags2 & TR3_LITE))

		/* Decrease life-span */
		i_ptr->pval--;   /* don't dec if perm light -CFT */

		if (i_ptr->pval == 0) {
		    player_light = FALSE;
		    disturb(0, 1);
		/* unlight creatures */
		    update_monsters();
		    msg_print("Your light has gone out!");
		}

		/* The light is getting dim */
		else if ((i_ptr->pval < 40) && (randint(5) == 1) &&
			   (p_ptr->blind < 1) &&
			   !(i_ptr->flags2 & TR3_LITE)) { /* perm light doesn't dim -CFT */
		    disturb(0, 0);
		    msg_print("Your light is growing faint.");
		}
	    } else {
		if (!p_ptr->light) {
		    player_light = FALSE;
		    disturb(0, 1);
		/* unlight creatures */
		    update_monsters();
		}
	    }
	else if (i_ptr->pval > 0 || p_ptr->light) {
	    if (!(i_ptr->flags2 & TR3_LITE))
		i_ptr->pval--;	   /* don't dec if perm light -CFT */
	    player_light = TRUE;
	    disturb(0, 1);
	/* light creatures */
	    update_monsters();
	}


	/*** Check the Food, and Regenerate ***/

	/* Default regeneration */
	regen_amount = PLAYER_REGEN_NORMAL;

	/* Check food status	       */
	if (p_ptr->food < PLAYER_FOOD_ALERT) {
	    if (p_ptr->food < PLAYER_FOOD_WEAK) {

		if (p_ptr->food < 0) {
		    regen_amount = 0;
		}
		else if (p_ptr->food < PLAYER_FOOD_FAINT) {
		    regen_amount = PLAYER_REGEN_FAINT;
		}
		else if (p_ptr->food < PLAYER_FOOD_WEAK) {
		    regen_amount = PLAYER_REGEN_WEAK;
		}

		if ((PY_WEAK & p_ptr->status) == 0) {
		    p_ptr->status |= PY_WEAK;
		    msg_print("You are getting weak from hunger.");
		    disturb(0, 0);
		    prt_hunger();
		}
		if ((p_ptr->food < PLAYER_FOOD_FAINT) && (randint(8) == 1)) {
		    p_ptr->paralysis += randint(5);
		    msg_print("You faint from the lack of food.");
		    disturb(1, 0);
		}
	    }
	    else if ((PY_HUNGRY & p_ptr->status) == 0) {
		p_ptr->status |= PY_HUNGRY;
		msg_print("You are getting hungry.");
		disturb(0, 0);
		prt_hunger();
	    }
	}

	/* Food consumption */
	/* Note: Speeded up characters really burn up the food!  */
	/* now summation, not square, since spd less powerful -CFT */
	
	/* Fast players consume slightly more food */
	if (p_ptr->speed < 0) {
	    p_ptr->food -=  (p_ptr->speed * p_ptr->speed - p_ptr->speed) / 2;
	}

	/* Digest some food */
	p_ptr->food -= p_ptr->food_digested;

	/* Starve to death */
	if (p_ptr->food < 0) {
	    take_hit(-p_ptr->food / 16, "starvation");	/* -CJS- */
	    disturb(1, 0);
	}

	/* Regeneration ability */
	if (p_ptr->regenerate) {
	    regen_amount = regen_amount * 3 / 2;
	}

	/* Searching or Resting */
	if ((p_ptr->status & PY_SEARCH) || p_ptr->rest) {
	    regen_amount = regen_amount * 2;
	}

	/* Regenerate the mana (even if poisoned or cut (?)) */
	if (p_ptr->cmana < p_ptr->mana) {
	    regenmana(regen_amount);
	}

	if ((p_ptr->poisoned < 1) && (p_ptr->cut < 1) &&
	    (p_ptr->chp < p_ptr->mhp)) {
	    regenhp(regen_amount);
	}


	/*** Assorted Maladies ***/

	/* Paralysis -- player cannot see monster movement */
	if (p_ptr->paralysis > 0) {
	    p_ptr->paralysis--;
	    disturb(1, 0);
	}

	/* Blindness */
	if (p_ptr->blind > 0) {
	    if (!(PY_BLIND & p_ptr->status)) {
		p_ptr->status |= PY_BLIND;
		prt_map();
		prt_blind();
		disturb(0, 1);
	    /* unlight creatures */
		update_monsters();
	    }
	    p_ptr->blind--;
	    if (!p_ptr->blind) {
		p_ptr->status &= ~PY_BLIND;
		msg_print("The veil of darkness lifts.");
		prt_blind();
		prt_map();
	    /* light creatures */
		disturb(0, 1);
		update_monsters();
	    }
	}

	/* Hallucinating? */
	if (p_ptr->image > 0) {
	    end_find();
	    p_ptr->image--;
	    if (p_ptr->image == 0)
		prt_map();	   /* Used to draw entire screen! -CJS- */
	}

	/* Confusion */
	if (p_ptr->confused > 0) {
	    if ((PY_CONFUSED & p_ptr->status) == 0) {
		p_ptr->status |= PY_CONFUSED;
		prt_confused();
	    }
	    p_ptr->confused--;
	    if (!p_ptr->confused) {
		p_ptr->status &= ~PY_CONFUSED;
		msg_print("You feel less confused now.");
		prt_confused();

		if (p_ptr->rest > 0 || p_ptr->rest == -1)
		    rest_off();
	    }
	}

	/* Stun */
	if (p_ptr->stun > 0) {
	    int oldstun = p_ptr->stun;

	    p_ptr->stun -= (con_adj() <= 0 ? 1 : (con_adj() / 2 + 1));
				/* fixes endless stun if bad con. -CFT */
	    if ((oldstun > 50) && (p_ptr->stun <= 50)) { 
				/* if crossed 50 mark... */
		p_ptr->ptohit += 15;
		p_ptr->ptodam += 15;
		p_ptr->dis_th += 15;
		p_ptr->dis_td += 15;
	    }
	    if (p_ptr->stun <= 0) {
		p_ptr->stun = 0;
		msg_print("Your head stops stinging.");
		p_ptr->ptohit += 5;
		p_ptr->ptodam += 5;
		p_ptr->dis_th += 5;
		p_ptr->dis_td += 5;
	    }
	}

	/* Hack -- Always redraw the stun */
	prt_stun();

	/* Cut */
	if (p_ptr->cut > 0) {
	    if (p_ptr->cut > 1000) {
		take_hit(3 , "a fatal wound");
		disturb(1,0);
	    }
	    else if (p_ptr->cut > 200) {
		take_hit(3, "a fatal wound");
		p_ptr->cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else if (p_ptr->cut > 100) {
		take_hit(2, "a fatal wound");
		p_ptr->cut-=(con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    else {
		take_hit(1, "a fatal wound");
		p_ptr->cut -= (con_adj()<0?1:con_adj())+1;
		disturb(1,0);
	    }
	    prt_cut();
	    if (p_ptr->cut <= 0) {
		p_ptr->cut = 0;
		if (p_ptr->chp >= 0) msg_print("Your wound heals.");
	    }
	}

	/* Always redraw "cuts" */
	prt_cut();

	/* Poisoned */
	if (p_ptr->poisoned > 0) {
	    if ((PY_POISONED & p_ptr->status) == 0) {
		p_ptr->status |= PY_POISONED;
		prt_poisoned();
	    }
	    p_ptr->poisoned--;
	    if (p_ptr->poisoned == 0 || p_ptr->immune_pois ||
		p_ptr->resist_pois ||
		p_ptr->oppose_pois) {
		p_ptr->poisoned = 0;
		p_ptr->status &= ~PY_POISONED;
		prt_poisoned();
		msg_print("You feel better.");
		disturb(0, 0);
	    }
	    else {
		switch (con_adj()) {
		  case -4: i = 4; break;
		  case -3:
		  case -2: i = 3; break;
		  case -1: i = 2; break;
		  case 0: i = 1; break;
		  case 1:
		  case 2:
		  case 3: i = ((turn % 2) == 0); break;
		  case 4:
		  case 5: i = ((turn % 3) == 0); break;
		  case 6: i = ((turn % 4) == 0); break;
		  case 7: i = ((turn % 5) == 0); break;
		  default: i = ((turn % 6) == 0); break;
		}
		take_hit(i, "poison");
		disturb(1, 0);
	    }
	}

	/* Afraid */
	if (p_ptr->afraid > 0) {
	    if ((PY_FEAR & p_ptr->status) == 0) {
		if ((p_ptr->shero + p_ptr->hero + p_ptr->resist_fear) > 0)
		    p_ptr->afraid = 0;
		else {
		    p_ptr->status |= PY_FEAR;
		    prt_afraid();
		}
	    } else if ((p_ptr->shero + p_ptr->hero + p_ptr->resist_fear) > 0)
		p_ptr->afraid = 1;
	    p_ptr->afraid--;
	    if (!p_ptr->afraid) {
		p_ptr->status &= ~PY_FEAR;
		prt_afraid();
		msg_print("You feel bolder now.");
		disturb(0, 0);
	    }
	}


	/*** Check the Speed ***/

	/* Fast */
	if (p_ptr->fast > 0) {
	    if (!(PY_FAST & p_ptr->status)) {
		p_ptr->status |= PY_FAST;
		msg_print("You feel yourself moving faster.");
		p_ptr->speed -= 1;
		p_ptr->status |= PY_SPEED;
		disturb(0, 0);
	    }
	    p_ptr->fast--;
	    if (!p_ptr->fast) {
		p_ptr->status &= ~PY_FAST;
		msg_print("You feel yourself slow down.");
		p_ptr->speed += 1;
		p_ptr->status |= PY_SPEED;
		disturb(0, 0);
	    }
	}

	/* Slow */
	if (p_ptr->slow > 0) {
	    if (!(PY_SLOW & p_ptr->status)) {
		p_ptr->status |= PY_SLOW;
		msg_print("You feel yourself moving slower.");
		p_ptr->speed += 1;
		p_ptr->status |= PY_SPEED;
		disturb(0, 0);
	    }
	    p_ptr->slow--;
	    if (!p_ptr->slow) {
		p_ptr->status &= ~PY_SLOW;
		msg_print("You feel yourself speed up.");
		p_ptr->speed -= 1;
		p_ptr->status |= PY_SPEED;
		disturb(0, 0);
	    }
	}

    /* Check for interrupts to find or rest. */
	if ((command_rep > 0 || find_flag || p_ptr->rest > 0 || p_ptr->rest == -1
	     || p_ptr->rest == -2)
#if defined(MSDOS) || defined(VMS) /* stolen from Um55 src -CFT */
	    && kbhit()
#else
	    && (check_input(find_flag ? 0 : 10000))
#endif
	    ) {
#ifdef MSDOS
	    (void)msdos_getch();
#endif
#ifdef VMS
	/* Get and ignore the key used to interrupt resting/running.  */
	    (void)vms_getch();
#endif
	    disturb(0, 0);
	}


	/*** All good things must come to an end... ***/

	/* Protection from evil counter */
	if (p_ptr->protevil > 0) {
	    p_ptr->protevil--;
	    if (!p_ptr->protevil) {
		msg_print("You no longer feel safe from evil.");
	    }
	}

	/* Invulnerability */
	if (p_ptr->invuln > 0) {
	    if ((PY_INVULN & p_ptr->status) == 0) {
		p_ptr->status |= PY_INVULN;
		msg_print("Your skin turns to steel!");
		disturb(0, 0);
		p_ptr->ptoac += 100;	/* changed to ptoac -CFT */
		p_ptr->dis_tac += 100;
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->invuln--;
	    if (p_ptr->invuln == 0) {
		p_ptr->status &= ~PY_INVULN;
		msg_print("Your skin returns to normal.");
		disturb(0, 0);
		p_ptr->ptoac -= 100;	/* changed to ptoac -CFT */
		p_ptr->dis_tac -= 100;
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Heroism */
	if (p_ptr->hero > 0) {
	    if (!(PY_HERO & p_ptr->status)) {
		p_ptr->status |= PY_HERO;
		msg_print("You feel like a HERO!");
		disturb(0, 0);
		p_ptr->mhp += 10;
		p_ptr->chp += 10;
		p_ptr->ptohit += 12;
		p_ptr->dis_th += 12;
		prt_mhp();
		prt_chp();
	    }
	    p_ptr->hero--;
	    if (!p_ptr->hero) {
		p_ptr->status &= ~PY_HERO;
		msg_print("The heroism wears off.");
		disturb(0, 0);
		p_ptr->mhp -= 10;
		if (p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		    p_ptr->chp_frac = 0;
		    prt_chp();
		}
		p_ptr->ptohit -= 12;
		p_ptr->dis_th -= 12;
		prt_mhp();
	    }
	}

	/* Super Heroism */
	if (p_ptr->shero > 0) {
	    if (!(PY_SHERO & p_ptr->status)) {
		p_ptr->status |= PY_SHERO;
		msg_print("You feel like a killing machine!");
		disturb(0, 0);
		p_ptr->mhp += 30;
		p_ptr->chp += 30;
		p_ptr->ptohit += 24;
		p_ptr->dis_th += 24;
		p_ptr->ptoac -= 10;
		p_ptr->dis_tac -= 10;
		prt_mhp();
		prt_chp();
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->shero--;
	    if (!p_ptr->shero) {
		p_ptr->status &= ~PY_SHERO;
		msg_print("You feel less Berserk.");
		disturb(0, 0);
		p_ptr->mhp -= 30;
		p_ptr->ptoac += 10;
		p_ptr->dis_tac += 10;
		if (p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		    p_ptr->chp_frac = 0;
		    prt_chp();
		}
		p_ptr->ptohit -= 24;
		p_ptr->dis_th -= 24;
		prt_mhp();
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Blessed */
	if (p_ptr->blessed > 0) {
	    if (!(PY_BLESSED & p_ptr->status)) {
		p_ptr->status |= PY_BLESSED;
		msg_print("You feel righteous!");
		disturb(0, 0);
		p_ptr->ptohit += 10;
		p_ptr->dis_th += 10;
		p_ptr->ptoac += 5; /* changed to ptoac -CFT */
		p_ptr->dis_tac += 5;
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	    p_ptr->blessed--;
	    if (!p_ptr->blessed) {
		p_ptr->status &= ~PY_BLESSED;
		msg_print("The prayer has expired.");
		disturb(0, 0);
		p_ptr->ptohit -= 10;
		p_ptr->dis_th -= 10;
		p_ptr->ptoac -= 5; /* changed to ptoac -CFT */
		p_ptr->dis_tac -= 5;
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Shield */
	if (p_ptr->shield > 0) {
	    p_ptr->shield--;
	    if (!p_ptr->shield) {
		msg_print("Your mystic shield crumbles away.");
		disturb(0, 0);
		p_ptr->ptoac -= 50;	/* changed to ptoac -CFT */
		p_ptr->dis_tac -= 50;
		p_ptr->status |= PY_ARMOR;	/* have to update ac display */
	    }
	}

	/* Detect Invisible */
	if (p_ptr->detect_inv > 0) {
	    if (!(PY_DET_INV & p_ptr->status)) {
		p_ptr->status |= PY_DET_INV;
		p_ptr->see_inv = TRUE;
		update_monsters();
	    }
	    p_ptr->detect_inv--;
	    if (!p_ptr->detect_inv) {
		p_ptr->status &= ~PY_DET_INV;
	    /* may still be able to see_inv if wearing magic item */
		if (p_ptr->prace == 9)
		    p_ptr->see_inv = TRUE;
		else {
		    p_ptr->see_inv = FALSE;	/* unless item grants it */
		    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++)
			if (TR3_SEE_INVIS & inventory[i].flags1)
			    p_ptr->see_inv = TRUE;
		}
		update_monsters();
	    }
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra > 0) {
	    if (!(PY_TIM_INFRA & p_ptr->status)) {
		p_ptr->status |= PY_TIM_INFRA;
		p_ptr->see_infra++;
		update_monsters();
	    }
	    p_ptr->tim_infra--;
	    if (!p_ptr->tim_infra) {
		p_ptr->status &= ~PY_TIM_INFRA;
		p_ptr->see_infra--;
		update_monsters();
	    }
	}


	/*** Timed resistance must end eventually ***/


	if (p_ptr->oppose_fire > 0) {
	    p_ptr->oppose_fire--;
	    if (!p_ptr->oppose_fire) {
		msg_print("You no longer feel safe from flame.");
	    }
	}

	if (p_ptr->oppose_cold > 0) {
	    p_ptr->oppose_cold--;
	    if (!p_ptr->oppose_cold) {
		msg_print("You no longer feel safe from cold.");
	    }
	}

	if (p_ptr->oppose_acid > 0) {
	    p_ptr->oppose_acid--;
	    if (!p_ptr->oppose_acid) {
		msg_print("You no longer feel safe from acid.");
	    }
	}

	if (p_ptr->oppose_elec > 0) {
	    p_ptr->oppose_elec--;
	    if (!p_ptr->oppose_elec) {
		msg_print("You no longer feel safe from lightning.");
	    }
	}

	if (p_ptr->oppose_pois > 0) {
	    p_ptr->oppose_pois--;
	    if (!p_ptr->oppose_pois) {
		msg_print("You no longer feel safe from poison.");
	    }
	}


	/*** Involuntary Movement ***/

	/* Word-of-Recall -- Note: Word-of-Recall is a delayed action */
	if (p_ptr->word_recall > 0) {
	    if (p_ptr->word_recall == 1) {
		new_level_flag = TRUE;
		p_ptr->paralysis++;
		p_ptr->word_recall = 0;
		if (dun_level > 0) {
		    dun_level = 0;
		    msg_print("You feel yourself yanked upwards! ");
		}
		else if (p_ptr->max_dlv) {
		    dun_level = p_ptr->max_dlv;
		    msg_print("You feel yourself yanked downwards! ");
		}
	    }
	    else {
		p_ptr->word_recall--;
	    }
	}

    /* Random teleportation */
	if ((p_ptr->teleport) && (randint(100) == 1)) {
	    disturb(0, 0);
	    teleport(40);
	}



	/*** Handle Resting ***/

	/* Check "Resting" status */
	if (p_ptr->rest != 0) {

	    /* +n -> rest for n turns */
	    if (p_ptr->rest > 0) {
		p_ptr->rest--;
		if (p_ptr->rest == 0) {
		    rest_off();
		}
	    }

	    /* -1 -> rest until HP/mana restored */
	    else if (p_ptr->rest == -1) {
		if ((p_ptr->chp == p_ptr->mhp) &&
		    (p_ptr->cmana == p_ptr->mana)) {

		    p_ptr->rest = 0;
		    rest_off();
		}
	    }

	    /* -2 -> like -1, plus blind/conf/fear/stun/halluc/recall/slow */
	    /* Note: stop (via "disturb") as soon as blind or recall is done */
	    else if (p_ptr->rest == -2) {
		if ((p_ptr->blind < 1) && (p_ptr->confused < 1) &&
		    (p_ptr->afraid < 1) && (p_ptr->stun < 1) &&
		    (p_ptr->image < 1) && (p_ptr->word_recall < 1) &&
		    (p_ptr->slow < 1) &&
		    (p_ptr->chp == p_ptr->mhp) &&
		    (p_ptr->cmana == p_ptr->mana)) {

		    p_ptr->rest = 0;
		    rest_off();
		}
	    }
	}


    /* See if we are too weak to handle the weapon or pack.  -CJS- */
	if (p_ptr->status & PY_STR_WGT) {
	    check_strength();
	}

	
	/*** Display some things ***/

	if (p_ptr->status & PY_STUDY) {
	    prt_study();
	}

	if (p_ptr->status & PY_SPEED) {
	    p_ptr->status &= ~PY_SPEED;
	    prt_speed();
	}

	if ((p_ptr->status & PY_PARALYSED) && (p_ptr->paralysis < 1)) {
	    p_ptr->status &= ~PY_PARALYSED;
	    prt_state();
	}
	else if (p_ptr->paralysis > 0) {
	    p_ptr->status |= PY_PARALYSED;
	    prt_state();
	}
	else if (p_ptr->rest != 0) {
	    prt_state();
	}

	if (p_ptr->status & PY_STATS) {
	    if (p_ptr->status & PY_STR) prt_stat(A_STR);
	    if (p_ptr->status & PY_INT) prt_stat(A_INT);
	    if (p_ptr->status & PY_WIS) prt_stat(A_WIS);
	    if (p_ptr->status & PY_DEX) prt_stat(A_DEX);
	    if (p_ptr->status & PY_CON) prt_stat(A_CON);
	    if (p_ptr->status & PY_CHR) prt_stat(A_CHR);
	    p_ptr->status &= ~PY_STATS;
	}

	if (p_ptr->status & PY_ARMOR) {
	    p_ptr->dis_ac = p_ptr->pac + p_ptr->dis_tac;	/* use updated ac */
	    p_ptr->status &= ~PY_ARMOR;
	    prt_pac();
	}

	if (p_ptr->status & PY_HP) {
	    p_ptr->status &= ~PY_HP;
	    prt_mhp();
	    prt_chp();
	}

	if (p_ptr->status & PY_MANA) {
	    p_ptr->status &= ~PY_MANA;
	    prt_cmana();
	}


	/*** Process Inventory ***/

	/* Timeout Artifacts */
	for (i = 22; i < (INVEN_ARRAY_SIZE - 1); i++) {

	    /* Get the object */
	    i_ptr = &inventory[i];

	    /* Skip fake objects */
	    if (i_ptr->tval == TV_NOTHING) continue;

	    if (i_ptr->flags2 & TR3_ACTIVATE) {

	    /* Let activatable objects recharge */
	    if (i_ptr->timeout > 0) i_ptr->timeout--;

	    if ((i_ptr->tval == TV_RING) && (!stricmp(k_list[i_ptr->index].name, "Power"))) {
		if ((randint(20) == 1) && (p_ptr->exp > 0)) {
		    p_ptr->exp--;
		    p_ptr->max_exp--;
		    prt_experience();
		}
	    }
	}

	/* Timeout rods */
	for (i = 0; i < 22; i++) {

	    i_ptr = &inventory[i];

	    if (i_ptr->tval == TV_ROD && (i_ptr->flags2 & TR3_ACTIVATE)) {
		if (i_ptr->timeout > 0) i_ptr->timeout--;
	    }
	}


	/*** Auto-Detect-Enchantment ***/

    /* Allow for a slim chance of detect enchantment -CJS- */
    /*
     * for 1st level char, check once every 2160 turns for 40th level char,
     * check once every 416 turns 
     */
	if (p_ptr->pclass == 2 ?
	    ((p_ptr->confused == 0) && (p_ptr->pclass != 0) &&
	(randint((int)(10000 / (p_ptr->lev * p_ptr->lev + 40)) + 1) == 1))
	    :
	    (((turn & 0xF) == 0) && (p_ptr->confused == 0)
	     && (randint((int)(10 + 750 / (5 + p_ptr->lev))) == 1))
	    ) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_ARRAY_SIZE; i++) {

		if (i == inven_ctr) i = 22;

	    /* Get the object */
	    i_ptr = &inventory[i];

	    /*
	     * if in inventory, succeed 1 out of 50 times, if in equipment
	     * list, success 1 out of 10 times, unless you're a priest or
	     * rogue... 
	     */
		if (((i_ptr->tval >= TV_MIN_WEAR) && (i_ptr->tval <= TV_MAX_WEAR)) &&
		    special_check(i_ptr) &&
		    ((p_ptr->pclass == 2 || p_ptr->pclass == 3) ?
		     (randint(i < 22 ? 5 : 1) == 1) :
		     (randint(i < 22 ? 50 : 10) == 1))) {

		    if (p_ptr->pclass == 0 || p_ptr->pclass == 3 ||
			p_ptr->pclass == 5)
			if ((i_ptr->tval == TV_SWORD) ||
			    (i_ptr->tval == TV_HAFTED) ||
			    (i_ptr->tval == TV_POLEARM) ||
			    (i_ptr->tval == TV_BOW) ||
			    (i_ptr->tval == TV_BOLT) ||
			    (i_ptr->tval == TV_ARROW) ||
			    (i_ptr->tval == TV_DIGGING) ||
			    (i_ptr->tval == TV_SHOT) ||
			    (i_ptr->tval == TV_SOFT_ARMOR) ||
			    (i_ptr->tval == TV_HARD_ARMOR) ||
			    (i_ptr->tval == TV_HELM) ||
			    (i_ptr->tval == TV_BOOTS) ||
			    (i_ptr->tval == TV_CLOAK) ||
			    (i_ptr->tval == TV_GLOVES) ||
			    (i_ptr->tval == TV_SHIELD))
			    continue;
		    (void)sprintf(tmp_str,
			    "There's something %s about what you are %s...",
				  special_check(i_ptr) > 0 ? "good" : "bad",
				  describe_use(i));
		    disturb(0, 0);
		    msg_print(tmp_str);
		    add_inscribe(i_ptr, (special_check(i_ptr) > 0) ?
				 ID_MAGIK : ID_DAMD);
		}
	    }
	}

    /* Warriors, Rogues and paladins inbuilt ident */
	if (((p_ptr->pclass == 0) && (p_ptr->confused == 0) &&
	 (randint((int)(9000 / (p_ptr->lev * p_ptr->lev + 40)) + 1) == 1))
	    ||
	    ((p_ptr->pclass == 3) && (p_ptr->confused == 0) &&
	(randint((int)(20000 / (p_ptr->lev * p_ptr->lev + 40)) + 1) == 1))
	    ||
	    ((p_ptr->pclass == 5) && (p_ptr->confused == 0) &&
	     (randint((int)(80000L / (p_ptr->lev * p_ptr->lev + 40)) + 1) == 1))) {
	    vtype               tmp_str;

	    for (i = 0; i < INVEN_ARRAY_SIZE; i++) {

		if (i == inven_ctr) i = 22;

	    /* Get the object */
	    i_ptr = &inventory[i];

	    /*
	     * if in inventory, succeed 1 out of 5 times, if in equipment
	     * list, always succeed! 
	     */
		if (((i_ptr->tval == TV_SOFT_ARMOR) ||
		     (i_ptr->tval == TV_HARD_ARMOR) ||
		     (i_ptr->tval == TV_SWORD) ||
		     (i_ptr->tval == TV_HAFTED) ||
		     (i_ptr->tval == TV_POLEARM) ||
		     (i_ptr->tval == TV_SHIELD) ||
		     (i_ptr->tval == TV_HELM) ||
		     (i_ptr->tval == TV_BOOTS) ||
		     (i_ptr->tval == TV_GLOVES) ||
		     (i_ptr->tval == TV_DIGGING) ||
		     (i_ptr->tval == TV_SHOT) ||
		     (i_ptr->tval == TV_BOLT) ||
		     (i_ptr->tval == TV_ARROW) ||
		     (i_ptr->tval == TV_BOW) ||
		     (i_ptr->tval == TV_CLOAK))
		    && value_check(i_ptr) &&
		    (randint(i < 22 ? 5 : 1) == 1)) {
		    char                out_val[100], tmp[100], *ptr;
		    int                 sp;

		    (void)strcpy(tmp, k_list[i_ptr->index].name);

		    ptr = tmp;
		    sp = 0;
		    while (tmp[sp] == ' ' || tmp[sp] == '&')
			ptr = &tmp[++sp];

		    (void)strcpy(out_val, ptr);

		    ptr = out_val;

		    while (*ptr) {
			if (*ptr == '~')
			    *ptr = 's';
			ptr++;
		    }

		    (void)sprintf(tmp_str,
				  "You feel the %s (%c) you are %s %s %s...",
				  out_val,
		    ((i < INVEN_WIELD) ? i + 'a' : (i + 'a' - INVEN_WIELD)),
				  describe_use(i),
				  ((i_ptr->tval == TV_BOLT) ||
				   (i_ptr->tval == TV_ARROW) ||
				   (i_ptr->tval == TV_SHOT) ||
				   (i_ptr->tval == TV_BOOTS) ||
				 (i_ptr->tval == TV_GLOVES)) ? "are" : "is",
				  value_check(i_ptr));
		    disturb(0, 0);
		    msg_print(tmp_str);
		    if (!stricmp(value_check(i_ptr), "terrible"))
			add_inscribe(i_ptr, ID_DAMD);
		    else if (!stricmp(value_check(i_ptr), "worthless"))
			add_inscribe(i_ptr, ID_DAMD);
		    else
			inscribe(i_ptr, value_check(i_ptr));
		}
	    }
	}

    /*
     * Check the state of the monster list, and delete some monsters if the
     * monster list is nearly full.  This helps to avoid problems in
     * creature.c when monsters try to multiply.  Compact_monsters() is much
     * more likely to succeed if called from here, than if called from within
     * process_monsters().  
     */
	if (MAX_M_IDX - m_max < 10)
	    (void)compact_monsters();

	/* Resting -- Voluntary trade of moves for regeneration */
	/* Mega-Stunned -- Unable to do anything but stagger */
	/* Paralyzed -- Not allowed to do anything */
	/* Dead -- Not really able to do anything */

	if ((p_ptr->rest) ||
	    (p_ptr->stun >= 100) ||
	    (p_ptr->paralysis > 0) ||
	    (death) ) {

	    /* Hilite the player */
	    move_cursor_relative(char_row, char_col);

	    /* Flush output */
	    put_qio();
	}


	/* Get (and execute) one or more user commands */
	else {

	    do {

		/* Commands are assumed to take time */
		free_turn_flag = FALSE;

		/* Update the "repeat" count */
		if (p_ptr->status & PY_REPEAT) prt_state();

		default_dir = FALSE;

		if ((old_lite >= 0) && (!find_flag)) {
		    cur_lite = old_lite;
		    old_lite = (-1);
		}

		/* Hack -- If running, run some more */
		if (find_flag) {

		    /* Take a step */
		    find_step();
		    find_count--;
		    if (find_count == 0)
			end_find();
		    put_qio();
		} else if (doing_inven)
		    inven_command(doing_inven);
		else {
		/* move the cursor to the players character */
		    move_cursor_relative(char_row, char_col);
		    if (command_rep > 0) {
			command_rep--;
			msg_flag = FALSE;
			default_dir = TRUE;
		    } else {
#ifdef TARGET
/* This bit of targetting code taken from Morgul -CFT */
/* If we are in targetting mode, with a creature target, make the targetted */
/* row and column match the creature's.  This optimizes a lot of code.  CDW */
			if ((target_mode)&&(target_mon<(unsigned) m_max)) {
			    target_row = m_list[target_mon].fy;
			    target_col = m_list[target_mon].fx;
			}
#endif
			msg_flag = FALSE;
			command = inkey();
			i = 0;
		    /* Get a count for a command. */
			if ((rogue_like_commands
			     && command >= '0' && command <= '9')
			    || (!rogue_like_commands && command == '#')) {
			    char                tmp[8];

			    prt("Repeat count:", 0, 0);
			    if (command == '#')
				command = '0';
			    i = 0;
			    while (TRUE) {
				if (command == DELETE || command == CTRL('H')) {
				    i = i / 10;
				    (void)sprintf(tmp, "%d", i);
				    prt(tmp, 0, 14);
				} else if (command >= '0' && command <= '9') {
				    if (i > 99)
					bell();
				    else {
					i = i * 10 + command - '0';
					(void)sprintf(tmp, "%d", i);
					prt(tmp, 0, 14);
				    }
				} else
				    break;
				command = inkey();
			    }
			    if (i == 0) {
				i = 99;
				(void)sprintf(tmp, "%d", i);
				prt(tmp, 0, 14);
			    }
			/* a special hack to allow numbers as commands */
			    if (command == ' ') {
				prt("Command:", 0, 20);
				command = inkey();
			    }
			}

		    /* Another way of typing control codes -CJS- */
			if (command == '^') {
			    if (command_rep > 0)
				prt_state();
			    if (get_com("Control-", &command)) {
				if (command >= 'A' && command <= 'Z')
				    command -= 'A' - 1;
				else if (command >= 'a' && command <= 'z')
				    command -= 'a' - 1;
				else {
				    msg_print("Type ^ <letter> for a control char");
				    command = ' ';
				}
			    } else
				command = ' ';
			}

		    /* move cursor to player char again, in case it moved */
			move_cursor_relative(char_row, char_col);
		    /* Commands are always converted to rogue form. -CJS- */
			if (rogue_like_commands == FALSE)
			    command = original_commands(command);
			if (i > 0) {
			    if (!valid_countcommand(command)) {
				free_turn_flag = TRUE;
				msg_print("Invalid command with a count.");
				command = ' ';
			    } else {
				command_rep = i;
				prt_state();
				command_rep--;	/* count this pass as
							 * one */
			    }
			}
		    }
		/* Flash the message line. */
		    erase_line(MSG_LINE, 0);
		    move_cursor_relative(char_row, char_col);
		    put_qio();

		    do_command(command);
		/* Find is counted differently, as the command changes. */
		    if (find_flag) {
			find_count = command_rep;
			command_rep = 0;
		    } else if (old_lite >= 0) {
			cur_lite = old_lite;
			old_lite = (-1);
		    }
		    if (free_turn_flag)
			command_rep = 0;
		}
	    /* End of commands				     */
	    }
	    while (free_turn_flag && !new_level_flag && !eof_flag);
	}


	/* Mega-Hack -- process teleport traps */
	if (teleport_flag) teleport(100);

    /* Move the creatures	       */
	if (!new_level_flag)
	    process_monsters();
    /* Exit when new_level_flag is set   */
    }
    while (!new_level_flag && !eof_flag);
}


static char original_commands(char com_val)
{
    int dir_val;

    switch (com_val) {
      case CTRL('K'):		/* ^K = exit    */
	com_val = 'Q';
	break;
      case CTRL('J'):		/* not used */
      case CTRL('M'):		/* not used */
        com_val = ' ';
	break;
      case CTRL('F'):		/* ^F = repeat feeling */
      case CTRL('R'):		/* ^R = redraw screen  */
      case CTRL('P'):		/* ^P = repeat  */
      case CTRL('W'):		/* ^W = enter wizard mode */
      case CTRL('X'):		/* ^X = save    */
      case ' ':
	break;
      case '.': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT */
	  int temp = target_mode;
	  target_mode = FALSE;
#endif
	  if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	      case 1:
		com_val = 'B';
		break;
	      case 2:
		com_val = 'J';
		break;
	      case 3:
		com_val = 'N';
		break;
	      case 4:
		com_val = 'H';
		break;
	      case 6:
		com_val = 'L';
		break;
	      case 7:
		com_val = 'Y';
		break;
	      case 8:
		com_val = 'K';
		break;
	      case 9:
		com_val = 'U';
		break;
	      default:
		com_val = ' ';
		break;
	    }
	else
	    com_val = ' ';
#ifdef TARGET
	  target_mode = temp; /* restore old target code ... -CFT */
#endif
        }
	break;
      case '/':
      case '<':
      case '>':
      case '-':
      case '=':
      case '{':
      case '?':
      case 'A':
	break;
      case '1':
	com_val = 'b';
	break;
      case '2':
	com_val = 'j';
	break;
      case '3':
	com_val = 'n';
	break;
      case '4':
	com_val = 'h';
	break;
      case '5':			/* Rest one turn */
	com_val = '.';
	break;
      case '6':
	com_val = 'l';
	break;
      case '7':
	com_val = 'y';
	break;
      case '8':
	com_val = 'k';
	break;
      case '9':
	com_val = 'u';
	break;
      case 'B':
	com_val = 'f';
	break;
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'g':
	break;
      case 'L':
	com_val = 'W';
	break;
      case 'M':
      case 'R':
	break;
      case 'S':
	com_val = '#';
	break;
      case 'T': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	int temp = target_mode;
	target_mode = FALSE;
#endif
	if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	      case 1:
		com_val = CTRL('B');
		break;
	      case 2:
		com_val = CTRL('J');
		break;
	      case 3:
		com_val = CTRL('N');
		break;
	      case 4:
		com_val = CTRL('H');
		break;
	      case 6:
		com_val = CTRL('L');
		break;
	      case 7:
		com_val = CTRL('Y');
		break;
	      case 8:
		com_val = CTRL('K');
		break;
	      case 9:
		com_val = CTRL('U');
		break;
	      default:
		com_val = ' ';
		break;
	    }
	else
	    com_val = ' ';
#ifdef TARGET
	  target_mode = temp;
#endif
        }
	break;
      case 'a':
	com_val = 'z';
	break;
      case 'b':
	com_val = 'P';
	break;
      case 'c':
      case 'd':
      case 'e':
	break;
      case 'f':
	com_val = 't';
	break;
      case 'h':
	com_val = '?';
	break;
      case 'i':
	break;
      case 'j':
	com_val = 'S';
	break;
      case 'l':
	com_val = 'x';
	break;
      case 'm':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
	break;
      case 't':
	com_val = 'T';
	break;
      case 'u':
	com_val = 'Z';
	break;
      case 'z':
	com_val = 'a';
	break;
      case 'v':
      case 'V':
      case 'w':
	break;
      case 'x':
	com_val = 'X';
	break;

    /* wizard mode commands follow */
      case '\\':		/* \ = wizard help */
      case CTRL('A'):		/* ^A = cure all */
      case CTRL('D'):		/* ^D = up/down */
      case CTRL('E'):		/* ^E = wizchar */
      case CTRL('G'):		/* ^G = treasure */
      case CTRL('I'):		/* ^I = identify */
      case CTRL('O'):		/* ^O = generate objects */
      case CTRL('T'):		/* ^T = teleport */
      case CTRL('V'):		/* ^V = treasure */
      case CTRL('Z'):		/* ^Z = genocide */
      case ':':			/* map area */
      case '~':			/* artifact list to file */
      case '!':			/* rerate hitpoints */
      case '@':			/* create object */
      case '$':			/* wiz. light */
      case '%':			/* '%' == self knowledge */
      case '&':			/* & = summon  */
      case '*':			/* Indentify up to level */
      case '+':			/* add experience */
      case '|':			/* check uniques - cba */
	break;
      default:
	com_val = '(';		/* Anything illegal. */
	break;
    }
    return com_val;
}

/* Is an item an enchanted weapon or armor and we don't know?  -CJS- */
/* returns positive if it is a good enchantment */
/* returns negative if a bad enchantment... */
int special_check(inven_type *t_ptr)
{
    if (t_ptr->tval == TV_NOTHING)
	return 0;
    if (known2_p(t_ptr))
	return 0;
    if (store_bought_p(t_ptr))
	return 0;
    if (t_ptr->ident & ID_MAGIK)
	return 0;
    if (t_ptr->ident & ID_DAMD)
	return 0;
    if (t_ptr->flags1 & TR3_CURSED)
	return -1;
    if (t_ptr->tval != TV_HARD_ARMOR && t_ptr->tval != TV_SWORD &&
	t_ptr->tval != TV_SOFT_ARMOR && t_ptr->tval != TV_SHIELD &&
	t_ptr->tval != TV_CLOAK && t_ptr->tval != TV_GLOVES &&
	t_ptr->tval != TV_BOOTS && t_ptr->tval != TV_HELM &&
	t_ptr->tval != TV_DIGGING && t_ptr->tval != TV_SPIKE &&
	t_ptr->tval != TV_SHOT && t_ptr->tval != TV_BOLT &&
	t_ptr->tval != TV_ARROW && t_ptr->tval != TV_BOW &&
	t_ptr->tval != TV_POLEARM && t_ptr->tval != TV_HAFTED)
	return 0;
    if (t_ptr->tohit > 0 || t_ptr->todam > 0 || t_ptr->toac > 0)
	return 1;
    if ((t_ptr->tval == TV_DIGGING) && /* digging tools will pseudo ID, either
					  as {good} or {average} -CFT */
	(t_ptr->flags1 & TR1_TUNNEL))
	return 1;

    return 0;
}


static void do_command(char com_val)
{
    int                    dir_val, do_pickup;
    int                    y, x, i, j = 0;
    vtype                  out_val, tmp_str;
    char                   prt1[80];

/* hack for move without pickup.  Map '-' to a movement command. */
    if (com_val == '-') {
	do_pickup = FALSE;
	i = command_rep;
#ifdef TARGET
	{
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	int temp = target_mode;
	target_mode = FALSE;
#endif
	if (get_dir(NULL, &dir_val)) {
	    command_rep = i;
	    switch (dir_val) {
	      case 1:
		com_val = 'b';
		break;
	      case 2:
		com_val = 'j';
		break;
	      case 3:
		com_val = 'n';
		break;
	      case 4:
		com_val = 'h';
		break;
	      case 6:
		com_val = 'l';
		break;
	      case 7:
		com_val = 'y';
		break;
	      case 8:
		com_val = 'k';
		break;
	      case 9:
		com_val = 'u';
		break;
	      default:
		com_val = '(';
		break;
	    }
	} else
	    com_val = ' ';
#ifdef TARGET
	target_mode = temp;
	}
#endif
    } else
	do_pickup = TRUE;

    switch (com_val) {
      case 'Q':			/* (Q)uit		(^K)ill */
	flush();
	if ((!total_winner) ? get_check("Do you really want to quit?")
	    : get_check("Do you want to retire?")) {
	    new_level_flag = TRUE;
	    death = TRUE;
	    (void)strcpy(died_from, "Quitting");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('P'):		/* (^P)revious message. */
	if (command_rep > 0) {
	    i = command_rep;
	    if (i > MAX_SAVE_MSG)
		i = MAX_SAVE_MSG;
	    command_rep = 0;
	} else if (last_command != 16)
	    i = 1;
	else
	    i = MAX_SAVE_MSG;
	j = last_msg;
	if (i > 1) {
	    save_screen();
	    x = i;
	    while (i > 0) {
		i--;
		prt(old_msg[j], i, 0);
		if (j == 0)
		    j = MAX_SAVE_MSG - 1;
		else
		    j--;
	    }
	    erase_line(x, 0);
	    pause_line(x);
	    restore_screen();
	} else {
	/* Distinguish real and recovered messages with a '>'. -CJS- */
	    put_str(">", 0, 0);
	    prt(old_msg[j], 0, 1);
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('F'):		/* Repeat (^F)eeling */
	free_turn_flag = TRUE;
	do_cmd_feeling();
	break;
      case CTRL('W'):		/* (^W)izard mode */
	if (wizard) {
	    wizard = FALSE;
	    msg_print("Wizard mode off.");
	} else if (enter_wiz_mode())
	    msg_print("Wizard mode on.");
	prt_winner();
	free_turn_flag = TRUE;
	break;
      case CTRL('X'):		/* e(^X)it and save */
	if (total_winner) {
	    msg_print("You are a Total Winner,  your character must be retired.");
	    if (rogue_like_commands)
		msg_print("Use 'Q' to when you are ready to retire.");
	    else
		msg_print("Use <Control>-K when you are ready to retire.");
	} else {
	    (void)strcpy(died_from, "(saved)");
	    msg_print("Saving game...");
	    if (save_player())
		exit_game();
	    msg_print("Save failed...");
	    (void)strcpy(died_from, "(alive and well)");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('R'):
	if (p_ptr->image > 0)
	    msg_print("You cannot be sure what is real and what is not!");
	else {
	    draw_cave();
	    update_monsters();	  /* draw monsters */
	    prt_equippy_chars();  /* redraw equippy chars */
	}
	free_turn_flag = TRUE;
	break;
#ifdef TARGET
/* select a target (sorry, no intuitive letter keys were left: a/A for aim,
 * t/T for target, f/F for focus, s/S for select, c/C for choose and p/P for pick
 *  were all already taken.  Wiz light command moved to '$', which was unused. -CFT
 */
      case '*':
    if (p_ptr->blind > 0) {
	msg_print("You can't see anything to target!");
    }
    else if (!target_set()) {
	msg_print("Aborting Target.");
    }
    else {
	msg_print("Target selected.");
    }
	free_turn_flag = TRUE;
	break;    			
#endif
      case '=':			/* (=) set options */
	save_screen();
	do_cmd_options();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case '{':			/* ({) inscribe an object    */
	scribe_object();
	free_turn_flag = TRUE;
	break;
      case '!':			/* (!) escape to the shell */
	if (!wizard)
#ifdef MSDOS			/* Let's be a little more accurate... */
	    msg_print("Sorry, Angband doesn't leave enough free memory for a subshell.");
#else
	    msg_print("Sorry, inferior shells are not allowed from ANGBAND.");
#endif
	else
	    rerate();
	free_turn_flag = TRUE;
	break;
      case ESCAPE:		/* (ESC)   do nothing. */
      case ' ':			/* (space) do nothing. */
	free_turn_flag = TRUE;
	break;
      case 'b':			/* (b) down, left	(1) */
	move_player(1, do_pickup);
	break;
      case 'j':			/* (j) down		(2) */
	move_player(2, do_pickup);
	break;
      case 'n':			/* (n) down, right	(3) */
	move_player(3, do_pickup);
	break;
      case 'h':			/* (h) left		(4) */
	move_player(4, do_pickup);
	break;
      case 'l':			/* (l) right		(6) */
	move_player(6, do_pickup);
	break;
      case 'y':			/* (y) up, left		(7) */
	move_player(7, do_pickup);
	break;
      case 'k':			/* (k) up		(8) */
	move_player(8, do_pickup);
	break;
      case 'u':			/* (u) up, right	(9) */
	move_player(9, do_pickup);
	break;
      case 'B':			/* (B) run down, left	(. 1) */
	find_init(1);
	break;
      case 'J':			/* (J) run down		(. 2) */
	find_init(2);
	break;
      case 'N':			/* (N) run down, right	(. 3) */
	find_init(3);
	break;
      case 'H':			/* (H) run left		(. 4) */
	find_init(4);
	break;
      case 'L':			/* (L) run right	(. 6) */
	find_init(6);
	break;
      case 'Y':			/* (Y) run up, left	(. 7) */
	find_init(7);
	break;
      case 'K':			/* (K) run up		(. 8) */
	find_init(8);
	break;
      case 'U':			/* (U) run up, right	(. 9) */
	find_init(9);
	break;
      case '/':			/* (/) identify a symbol */
	ident_char();
	free_turn_flag = TRUE;
	break;
      case '.':			/* (.) stay in one place (5) */
	move_player(5, do_pickup);
	if (command_rep > 1) {
	    command_rep--;
	    rest();
	}
	break;
      case '<':			/* (<) go down a staircase */
	do_cmd_go_up();
	break;
      case '>':			/* (>) go up a staircase */
	do_cmd_go_down();
	break;
      case '?':			/* (?) help with commands */
	if (rogue_like_commands)
	    helpfile(ANGBAND_R_HELP);
	else
	    helpfile(ANGBAND_O_HELP);
	free_turn_flag = TRUE;
	break;
#ifdef ALLOW_SCORE
      case 'v':   /* score patch originally by Mike Welsh mikewe@acacia.cs.pdx.edu */
	sprintf(prt1,"Your current score is: %ld", total_points());
	msg_print(prt1);
	break;
#endif
      case 'f':			/* (f)orce		(B)ash */
	bash();
	break;
      case 'A':			/* (A)ctivate		(A)ctivate */
	do_cmd_activate();
	break;
      case 'C':			/* (C)haracter description */
	save_screen();
	change_name();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case 'D':			/* (D)isarm trap */
	do_cmd_disarm();
	break;
      case 'E':			/* (E)at food */
	do_cmd_eat_food();
	break;
      case 'F':			/* (F)ill lamp */
	do_cmd_refill_lamp();
	break;
      case 'G':			/* (G)ain magic spells */
	gain_spells();
	break;
      case 'g':			/* (g)et an object... */
	if (prompt_carry_flag) {
	    if (cave[char_row][char_col].i_idx != 0)	/* minor change -CFT */
		carry(char_row, char_col, TRUE);
	} else
	    free_turn_flag = TRUE;
	break;
      case 'W':			/* (W)here are we on the map	(L)ocate on map */
	if ((p_ptr->blind > 0) || no_lite())
	    msg_print("You can't see your map.");
	else {
	    int                 cy, cx, p_y, p_x;
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	    int temp = target_mode;
	    target_mode = FALSE;
#endif

	    y = char_row;
	    x = char_col;
	    if (get_panel(y, x, TRUE))
		prt_map();
	    cy = panel_row;
	    cx = panel_col;
	    for (;;) {
		p_y = panel_row;
		p_x = panel_col;
		if (p_y == cy && p_x == cx)
		    tmp_str[0] = '\0';
		else
		    (void)sprintf(tmp_str, "%s%s of",
			     p_y < cy ? " North" : p_y > cy ? " South" : "",
			      p_x < cx ? " West" : p_x > cx ? " East" : "");
		(void)sprintf(out_val,
      "Map sector [%d,%d], which is%s your sector. Look which direction?",
			      p_y, p_x, tmp_str);
		if (!get_dir(out_val, &dir_val))
		    break;

/* -CJS- Should really use the move function, but what the hell. This is nicer,
 * as it moves exactly to the same place in another section. The direction
 * calculation is not intuitive. Sorry.
 */
		for (;;) {
		    x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
		    y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
		    if (x < 0 || y < 0 || x >= cur_width || y >= cur_width) {
			msg_print("You've gone past the end of your map.");
			x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
			y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
			break;
		    }
		    if (get_panel(y, x, TRUE)) {
			prt_map();
			break;
		    }
		}
	    }
	/* Move to a new panel - but only if really necessary. */
	    if (get_panel(char_row, char_col, FALSE))
		prt_map();
#ifdef TARGET
	    target_mode = temp; /* restore target mode... */
#endif
	}
	free_turn_flag = TRUE;
	break;
      case 'R':			/* (R)est a while */
	rest();
	break;
      case '#':			/* (#) search toggle	(S)earch toggle */
	    search_off();
	else
	    search_on();
	free_turn_flag = TRUE;
	break;
      case CTRL('B'):		/* (^B) tunnel down left	(T 1) */
	tunnel(1);
	break;
      case CTRL('M'):		/* cr must be treated same as lf. */
      case CTRL('J'):		/* (^J) tunnel down		(T 2) */
	tunnel(2);
	break;
      case CTRL('N'):		/* (^N) tunnel down right	(T 3) */
	tunnel(3);
	break;
      case CTRL('H'):		/* (^H) tunnel left		(T 4) */
	tunnel(4);
	break;
      case CTRL('L'):		/* (^L) tunnel right		(T 6) */
	tunnel(6);
	break;
      case CTRL('Y'):		/* (^Y) tunnel up left		(T 7) */
	tunnel(7);
	break;
      case CTRL('K'):		/* (^K) tunnel up		(T 8) */
	tunnel(8);
	break;
      case CTRL('U'):		/* (^U) tunnel up right		(T 9) */
	tunnel(9);
	break;
      case 'z':			/* (z)ap a wand		(a)im a wand */
	do_cmd_aim_wand();
	break;
      case 'a':			/* (a)ctivate a rod	(z)ap a rod */
	do_cmd_zap_rod();
	break;
      case 'M':
	screen_map();
	free_turn_flag = TRUE;
	break;
      case 'P':			/* (P)eruse a book	(B)rowse in a book */
	do_cmd_browse();
	free_turn_flag = TRUE;
	break;
      case 'c':			/* (c)lose an object */
	do_cmd_close();
	break;
      case 'd':			/* (d)rop something */
	inven_command('d');
	break;
      case 'e':			/* (e)quipment list */
	inven_command('e');
	break;
      case 't':			/* (t)hrow something	(f)ire something */
	do_cmd_fire();
	break;
      case 'i':			/* (i)nventory list */
	inven_command('i');
	break;
      case 'S':			/* (S)pike a door	(j)am a door */
	do_cmd_spike();
	break;
      case 'x':			/* e(x)amine surrounds	(l)ook about */
	do_cmd_look();
	free_turn_flag = TRUE;
	break;
      case 'm':			/* (m)agic spells */
	cast();
	break;
      case 'o':			/* (o)pen something */
	do_cmd_open();
	break;
      case 'p':			/* (p)ray */
	pray();
	break;
      case 'q':			/* (q)uaff */
	do_cmd_quaff_potion();
	break;
      case 'r':			/* (r)ead */
	do_cmd_read_scroll();
	break;
      case 's':			/* (s)earch for a turn */
	search(char_row, char_col, p_ptr->srh);
	break;
      case 'T':			/* (T)ake off something	(t)ake off */
	inven_command('t');
	break;
      case 'Z':			/* (Z)ap a staff	(u)se a staff */
	do_cmd_use_staff();
	break;
      case 'V':			/* (V)ersion of game */
	helpfile(ANGBAND_VERSION);
	free_turn_flag = TRUE;
	break;
      case 'w':			/* (w)ear or wield */
	inven_command('w');
	break;
      case 'X':			/* e(X)change weapons	e(x)change */
	inven_command('x');
	break;
#ifdef ALLOW_ARTIFACT_CHECK /* -CWS */
      case '~':
	if ((!wizard) && (dun_level != 0)) {
	    msg_print("You need to be on the town level to check artifacts!");
	    msg_print(NULL);		/* make sure can see the message -CWS */
	} else
	    artifact_check_no_file();
	break;
#endif
#ifdef ALLOW_CHECK_UNIQUES /* -CWS */
      case '|':
	do_cmd_check_uniques();
	break;
#endif
      default:
	if (wizard) {
	    free_turn_flag = TRUE; /* Wizard commands are free moves */
	    switch (com_val) {
	      case '\\':	   /* \ wizard help */
		helpfile(ANGBAND_W_HELP);
	      case CTRL('A'):	   /* ^A = Cure all */
		(void)remove_all_curse();
		(void)cure_blindness();
		(void)cure_confusion();
		(void)cure_poison();
		(void)remove_fear();
		(void)res_stat(A_STR);
		(void)res_stat(A_INT);
		(void)res_stat(A_WIS);
		(void)res_stat(A_CON);
		(void)res_stat(A_DEX);
		(void)res_stat(A_CHR);
		(void)restore_level();
		(void)hp_player(2000);
		p_ptr->food = PLAYER_FOOD_MAX;
		if (p_ptr->slow > 1)
		    p_ptr->slow = 1;
		if (p_ptr->image > 1)
		    p_ptr->image = 1;
		if (p_ptr->cut > 1)
		    p_ptr->cut = 1;
		if (p_ptr->stun > 1)
		    p_ptr->stun = 1;
		break;
	      case CTRL('D'):	/* ^D = up/down */
		if (command_rep > 0) {
		    if (command_rep > 99)
			i = 0;
		    else
			i = command_rep;
		    command_rep = 0;
		} else {
		    prt("Go to which level (0-10000) ? ", 0, 0);
		    i = (-1);
		    if (get_string(tmp_str, 0, 27, 10))
			i = atoi(tmp_str);
		    if (i > 10000)
			i = 10000;
		}
		if (i > -1) {
		    dun_level = i;
		    if (dun_level > 10000)
			dun_level = 10000;
		    new_level_flag = TRUE;
		} else
		    erase_line(MSG_LINE, 0);
		break;
	      case CTRL('E'):	/* ^E = wizchar */
		change_character();
		erase_line(MSG_LINE, 0); /* from um55 -CFT */
		break;
	      case CTRL('G'):	/* ^G = treasure */
		if (command_rep > 0) {
		    i = command_rep;
		    command_rep = 0;
		} else
		    i = 1;
		random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('I'):	/* ^I = identify */
		(void)ident_spell();
		break;
	      case CTRL('T'):	/* ^T = teleport */
		teleport(100);
		break;
	      case CTRL('V'):	/* ^V special treasure */
		if (command_rep > 0) {
		    i = command_rep;
		    command_rep = 0;
		} else
		    i = 1;
		special_random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('Z'):	/* ^Z = genocide */
		(void)mass_genocide(FALSE);
		break;
	      case ':':
		map_area();
		break;
	      case '~':
		artifact_check_no_file();
		break;
	      case '|':
		do_cmd_check_uniques();
		break;
	      case '@':
		wizard_create();
		break;
	      case '$':	   /* $ = wiz light */
		wiz_lite(TRUE);
		break;
	      case '%':	   /* self-knowledge */
		self_knowledge();
		break;
	      case '&':	   /* & = summon  */
		y = char_row;
		x = char_col;
		(void)summon_monster(&y, &x, TRUE);
		update_monsters();
		break;
	      case '*':		/* '*' = identify all up to a level */
		prt("Identify objects upto which level (0-200) ? ", 0, 0);
		i = (-1);
		if (get_string(tmp_str, 0, 47, 10))
		    i = atoi(tmp_str);
		if (i > 200)
		    i = 200;
		if (i > -1) {
		    int                 temp;
		    inven_type          inv;

		    for (temp = 0; temp < MAX_DUNGEON_OBJ; temp++) {
			if (k_list[temp].level <= i) {
			    invcopy(&inv, temp);
			    known1(&inv);
			}
		    }
		}
		erase_line(MSG_LINE, 0);
		break;
	      case '+':
		if (command_rep > 0) {
		    p_ptr->exp = command_rep;
		    command_rep = 0;
		} else if (p_ptr->exp == 0)
		    p_ptr->exp = 1;
		else
		    p_ptr->exp = p_ptr->exp * 2;
		prt_experience();
		break;
	      default:
		prt("Type '?' or '\\' for help.", 0, 0);
	    }
	} else {
	    prt("Type '?' for help.", 0, 0);
	    free_turn_flag = TRUE;
	}
    }
    last_command = com_val;
}

/* Check whether this command will accept a count.     -CJS-  */
static int valid_countcommand(char c)
{
    switch (c) {
      case 'Q':
      case CTRL('W'):
      case CTRL('X'):
      case '=':
      case '{':
      case '/':
      case '<':
      case '>':
      case '?':
      case 'A':
      case 'C':
      case 'E':
      case 'F':
      case 'G':
      case '#':
      case 'z':
      case 'P':
      case 'c':
      case 'd':
      case 'e':
      case 't':
      case 'i':
      case 'x':
      case 'm':
      case 'p':
      case 'q':
      case 'r':
      case 'T':
      case 'Z':
      case 'V':
      case 'w':
      case 'W':
      case 'X':
      case CTRL('A'):
      case '\\':
      case CTRL('I'):
      case CTRL('^'):
      case '$':
      case '*':
      case ':':
      case CTRL('T'):
      case CTRL('E'):
      case CTRL('F'):
      case CTRL('Z'):
      case CTRL('S'):
      case CTRL('Q'):
      case CTRL('R'):
	return FALSE;
      case CTRL('P'):
      case ESCAPE:
      case ' ':
      case '-':
      case 'b':
      case 'f':
      case 'j':
      case 'n':
      case 'h':
      case 'l':
      case 'y':
      case 'k':
      case 'u':
      case '.':
      case 'B':
      case 'J':
      case 'N':
      case 'H':
      case 'L':
      case 'Y':
      case 'K':
      case 'U':
      case 'D':
      case 'R':
      case CTRL('Y'):
      case CTRL('K'):
      case CTRL('U'):
      case CTRL('L'):
      case CTRL('N'):
      case CTRL('J'):
      case CTRL('B'):
      case CTRL('H'):
      case 'S':
      case 'o':
      case 's':
      case CTRL('D'):
      case CTRL('G'):
      case '+':
	return TRUE;
      default:
	return FALSE;
    }
}


int ruin_stat(int stat)
{
    register int tmp_stat;

    tmp_stat = p_ptr->cur_stat[stat];
    if (tmp_stat > 3) {
	if (tmp_stat > 6) {
	    if (tmp_stat < 19) {
		tmp_stat -= 3;
	    } else {
		tmp_stat /= 2;
		if (tmp_stat < 18)
		    tmp_stat = 18;
	    }
	} else
	    tmp_stat--;

	p_ptr->cur_stat[stat] = tmp_stat;
	p_ptr->max_stat[stat] = tmp_stat;
	set_use_stat(stat);
	prt_stat(stat);
	return TRUE;
    } else
	return FALSE;
}


