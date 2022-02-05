/* File: moria1.c */

/* Purpose: player inventory (and related commands) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static void inven_screen(int);
static void sub1_move_light(int, int, int, int);
static void sub3_move_light(int, int, int, int);


/*
 * Check a char for "vowel-hood"
 */
int is_a_vowel(int ch)
{
    switch (ch & 127) {
      case 'a':
      case 'e':
      case 'i':
      case 'o':
      case 'u':
      case 'A':
      case 'E':
      case 'I':
      case 'O':
      case 'U':
	return (TRUE);
      default:
	return (FALSE);
    }
}


/*
 * Return a string describing how a given item is carried. -CJS- 
 */
cptr describe_use(int i)
{
    register cptr p;

    switch (i) {
      case INVEN_WIELD: p = "wielding"; break;
      case INVEN_AUX:   p = "holding ready by your side"; break;
      case INVEN_LEFT:  p = "wearing on your left hand"; break;
      case INVEN_RIGHT: p = "wearing on your right hand"; break;
      case INVEN_NECK:  p = "wearing around your neck"; break;
      case INVEN_LIGHT: p = "using to light the way"; break;
      case INVEN_BODY:  p = "wearing on your body"; break;
      case INVEN_OUTER: p = "wearing about your body"; break;
      case INVEN_ARM:   p = "wearing on your arm"; break;
      case INVEN_HEAD:  p = "wearing on your head"; break;
      case INVEN_HANDS: p = "wearing on your hands"; break;
      case INVEN_FEET:  p = "wearing on your feet"; break;
      default:          p = "carrying in your pack"; break;
    }

    /* Return the result */
    return p;
}



/*
 * Displays inventory items from r1 to r2	-RAK-
 *
 * Designed to keep the display as far to the right as possible.  -CJS-
 *
 * The parameter col gives a column at which to start, but if the display does
 * not fit, it may be moved left.  The return value is the left edge used. 
 */
int show_inven(int r1, int r2, int weight, int col, int (*test) ())
{
    register int i, j, k;
    int          len, l, lim;
    bigvtype     tmp_val;
    vtype        out_val[23];

    /* Default "max-length" */
    len = 79 - col;

    /* Maximum space allowed for descriptions */
    if (weight) lim = 68; else lim = 76;

    for (i = 0; i < 23; i++) out_val[i][0] = '\0';

    for (k = 0, i = r1; i <= r2; i++) {

	if (test) {
	    if ((*test) (k_list[inventory[i].index].tval)) {

	/* Describe the object, enforce max length */
	objdes(tmp_val, &inventory[i], TRUE);
	tmp_val[lim] = 0;  /* Truncate if too long. */

	(void)sprintf(out_val[i], "  %c) %s", 'a' + i, tmp_val);

	/* Find the predicted "line length" */
	l = strlen(out_val[i]);

	/* Be sure to account for the weight */
	if (weight) l += 9;

	/* Maintain the maximum length */
	if (l > len) len = l;

	/* Advance to next "line" */
	k++;
    }

	} else {

	    objdes(tmp_val, &inventory[i], TRUE);
	    tmp_val[lim] = 0;	   /* Truncate if too long. */
	    (void)sprintf(out_val[i], "  %c) %s", 'a' + i, tmp_val);
	    l = strlen(out_val[i]);
	    if (weight) l += 9;
	    if (l > len) len = l;
	    k++;
	}
    }

    /* Find the column to start in */
    col = 79 - len;
    if (col < 0) col = 0;

    for (j = 0, i = r1; (i <= r2) && k; i++) {

	if (out_val[i][0]) {
	    k--;
	/* don't need first two spaces if in first column */
	    if (col == 0) prt(&out_val[i][2], 1 + j, col);
	    else prt(out_val[i], 1 + j, col);

	/* Display the weight if needed */
	if (weight) {
	    int wgt = inventory[i].weight * inventory[i].number;
	    (void)sprintf(tmp_val, "%3d.%d lb", wgt / 10, wgt % 10);
	    prt(tmp_val, j + 1, 71);
	}
	j++;
	}
    }

    /* Erase the final line */
    erase_line(j + 1,col);

    return col;
}



/*
 * Displays (all) equipment items    -RAK-
 * Keep display as far right as possible. -CJS-
 */
int show_equip(int weight, int col)
{
    register int         i, line = 0;
    register inven_type *i_ptr;
    int                  l, len, lim;
    register const char *prt1;
    bigvtype             prt2;

    vtype                out_val[INVEN_ARRAY_SIZE - INVEN_WIELD];

    len = 79 - col;
    if (weight) lim = 52; else lim = 60;

    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {

	i_ptr = &inventory[i];

	if (i_ptr->tval != TV_NOTHING) {

	    switch (i) {

	      case INVEN_WIELD:
		if (p_ptr->use_stat[A_STR] * 15 < i_ptr->weight) prt1 = "Just lifting";
		else prt1 = "Wielding";
		break;

	      case INVEN_HEAD: prt1 = "On head"; break;
	      case INVEN_NECK: prt1 = "Around neck"; break;
	      case INVEN_BODY: prt1 = "On body"; break;
	      case INVEN_ARM: prt1 = "On arm"; break;
	      case INVEN_HANDS: prt1 = "On hands"; break;
	      case INVEN_RIGHT: prt1 = "On right hand"; break;
	      case INVEN_LEFT: prt1 = "On left hand"; break;
	      case INVEN_FEET: prt1 = "On feet"; break;
	      case INVEN_OUTER: prt1 = "About body"; break;
	      case INVEN_LIGHT: prt1 = "Light source"; break;
	      case INVEN_AUX: prt1 = "Spare weapon"; break;
	      default: prt1 = "Unknown value"; break;
	    }

	/* Build a truncated object description */
	objdes(prt2, &inventory[i], TRUE);
	prt2[lim] = 0;	   /* Truncate if necessary */

	(void)sprintf(out_val[line], "  %c) %-14s: %s", line + 'a', prt1, prt2);

	l = strlen(out_val[line]);
	if (weight) l += 9;

	/* Maintain the max-length */
	if (l > len) len = l;

	/* Advance the entry */
	line++;
	}
    }

    /* Find a column to start in */
    col = 79 - len; if (col < 0) col = 0;

    line = 0;
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {

	/* Get the item */
	i_ptr = &inventory[i];

	if (i_ptr->tval != TV_NOTHING) {

	/* don't need first two spaces when using whole screen */
	    if (col == 0) prt(&out_val[line][2], line + 1, col);
	    else prt(out_val[line], line + 1, col);

	/* Display the weight if needed */
	if (weight) {
	    int wgt = i_ptr->weight * i_ptr->number;
	    (void)sprintf(prt2, "%3d.%d lb", wgt / 10, wgt % 10);
	    prt(prt2, line + 1, 71);
	}
	line++;
	}
    }
    
    /* Make a shadow below the list (if possible) */
    erase_line(line+1, col);

    return col;
}






/*
 * Get the ID of an item and return the CTR value of it	-RAK-	 
 */
int get_item(int *com_val, cptr pmt, int i, int j, int (*test) ())
{
    vtype        out_val;
    char         which;
    register int test_flag, item;
    int          full, i_scr, redraw;

    int on_floor, ih;
    cave_type *c_ptr;
 
    /* check we're a) identifying and b) on the floor is an object
     * and c) it is a object wich could be picked up
     */

    c_ptr = &cave[char_row][char_col];
    ih = i_list[c_ptr->i_idx].tval;
    on_floor = ( (strcmp("Item you wish identified?",pmt) == 0) &&
		 !( (c_ptr->i_idx == 0) || ih == TV_NOTHING
		    || ih > TV_MAX_PICK_UP) );

    item = FALSE;
    redraw = FALSE;
    *com_val = 0;
    i_scr = 1;
    if (j > INVEN_WIELD) {
	full = TRUE;
	if (inven_ctr == 0) {
	    i_scr = 0;
	    j = equip_ctr - 1;
	} else
	    j = inven_ctr - 1;
    } else
	full = FALSE;

    if (inven_ctr > 0 || (full && equip_ctr > 0)) {
	do {
	    if (redraw) {
		if (i_scr > 0)
		    (void)show_inven(i, j, FALSE, 80, test);
		else
		    (void)show_equip(FALSE, 80);
	    }
	    if (full)
		(void)sprintf(out_val,
			      "(%s: %c-%c,%s%s / for %s, or ESC) %s",
			     (i_scr > 0 ? "Inven" : "Equip"), i + 'a', j + 'a',
			      (on_floor ? " - floor," : ""),
			      (redraw ? "" : " * to see,"),
			      (i_scr > 0 ? "Equip" : "Inven"), pmt);
	    else
		(void)sprintf(out_val,
			"(Items %c-%c,%s ESC to exit) %s", i + 'a', j + 'a',
			      (redraw ? "" : " * for inventory list,"), pmt);
	    test_flag = FALSE;
	    prt(out_val, 0, 0);
	    do {
		which = inkey();
		switch (which) {
		  case ESCAPE:
		    test_flag = TRUE;
		    free_turn_flag = TRUE;
		    i_scr = (-1);
		    break;
		  case '/':
		    if (full) {
			if (i_scr > 0) {
			    if (equip_ctr == 0) {
				prt("But you're not using anything -more-", 0, 0);
				(void)inkey();
			    } else {
				i_scr = 0;
				test_flag = TRUE;
				if (redraw) {
				    j = equip_ctr;
				    while (j < inven_ctr) {
					j++;
					erase_line(j, 0);
				    }
				}
				j = equip_ctr - 1;
			    }
			    prt(out_val, 0, 0);
			} else {
			    if (inven_ctr == 0) {
				prt("But you're not carrying anything -more-", 0, 0);
				(void)inkey();
			    } else {
				i_scr = 1;
				test_flag = TRUE;
				if (redraw) {
				    j = inven_ctr;
				    while (j < equip_ctr) {
					j++;
					erase_line(j, 0);
				    }
				}
				j = inven_ctr - 1;
			    }
			}

		    }
		    break;
		  case '*':
		    if (!redraw) {
			test_flag = TRUE;
			save_screen();
			redraw = TRUE;
		    }
		    break;
		case '-':
		/* not identified from INVEN or EQU but not aborted */
		    if (on_floor) {
			item = FUZZY;
			test_flag = TRUE;
			i_scr = -1;
			break;
		    }
		  default:
		    if (isupper((int)which))
			*com_val = which - 'A';
		    else
			*com_val = which - 'a';
		    if ((*com_val >= i) && (*com_val <= j)) {
			if (i_scr == 0) {
			    i = 21;
			    j = *com_val;
			    do {
				while (inventory[++i].tval == TV_NOTHING);
				j--;
			    }
			    while (j >= 0);
			    *com_val = i;
			}
			if (isupper((int)which) && !verify("Try", *com_val)) {
			    test_flag = TRUE;
			    free_turn_flag = TRUE;
			    i_scr = (-1);
			    break;
			}
			test_flag = TRUE;
			item = TRUE;
			i_scr = (-1);
		    } else
			bell();
		    break;
		}
	    }
	    while (!test_flag);
	}
	while (i_scr >= 0);
	if (redraw)
	    restore_screen();
	erase_line(MSG_LINE, 0);
    } else
	prt("You are not carrying anything.", 0, 0);
    return (item);
}



/* Player bonuses					-RAK-	 */
/* When an item is worn or taken off, this re-adjusts the player
 * bonuses.  Factor=1 : wear; Factor=-1 : removed  
 *****
 * Only calculates properties with cumulative effect.  Properties that depend
 * on everything being worn are recalculated by calc_bonuses() -CJS - 
 */
void py_bonuses(inven_type *t_ptr, int factor)
{
    register int i, amount;

    amount = t_ptr->pval * factor;
    if (t_ptr->flags1 & TR_STATS) {
	for (i = 0; i < 6; i++)
	    if ((1 << i) & t_ptr->flags1) {
		p_ptr->mod_stat[i] += amount;
		set_use_stat(i);
		p_ptr->status |= (PY_STR << i);
	    }
    }
    if (TR1_SEARCH & t_ptr->flags1) {
	p_ptr->srh += amount;
	p_ptr->fos -= amount;
    }
    if (TR1_STEALTH & t_ptr->flags1)
	p_ptr->stl += amount;
    if (TR1_SPEED & t_ptr->flags1) {
	if ((t_ptr->tval == TV_RING) &&
	    !stricmp("Speed",
		     k_list[t_ptr->index].name) &&
	    (t_ptr->pval > 0))
	    if ((inventory[INVEN_RIGHT].tval == TV_RING) &&
		!stricmp("Speed",
			 k_list[inventory[INVEN_RIGHT].index].name) &&
		(inventory[INVEN_RIGHT].pval > 0) &&
		(inventory[INVEN_LEFT].tval == TV_RING) &&
		!stricmp("Speed",
			 k_list[inventory[INVEN_LEFT].index].name) &&
		(inventory[INVEN_RIGHT].pval > 0))
		return;
	p_ptr->speed -= amount;
	p_ptr->status |= PY_SPEED;
    }
    if (TR1_INFRA & t_ptr->flags1)
	p_ptr->see_infra += amount;
}

/*
 * Recalculate the effect of all the stuff we use.		  -CJS-
 * Also initialise race intrinsics    SM
 */
void calc_bonuses()
{
    u32b		item_flags1, item_flags2;

    int			old_dis_ac;

    inven_type		*i_ptr;

    int			i;

    if (p_ptr->slow_digest) p_ptr->food_digested++;
    if (p_ptr->regenerate) p_ptr->food_digested -= 3;
    if (p_ptr->prace == 9) p_ptr->see_inv = TRUE;
    else p_ptr->see_inv = FALSE;
    p_ptr->teleport = FALSE;
    if (p_ptr->prace == 4) p_ptr->free_act = TRUE;
    else p_ptr->free_act = FALSE;
    p_ptr->slow_digest = FALSE;
    p_ptr->aggravate = FALSE;
    if (p_ptr->prace == 7) p_ptr->sustain_str = TRUE;
    else p_ptr->sustain_str = FALSE;
    p_ptr->sustain_int = FALSE;
    p_ptr->sustain_wis = FALSE;
    if (p_ptr->prace == 8) p_ptr->sustain_con = TRUE;
    else p_ptr->sustain_con = FALSE;
    if (p_ptr->prace == 3) p_ptr->sustain_dex = TRUE;
    else p_ptr->sustain_dex = FALSE;
    p_ptr->sustain_chr = FALSE;
    p_ptr->resist_fire = FALSE;
    p_ptr->resist_acid = FALSE;
    p_ptr->resist_cold = FALSE;
    p_ptr->regenerate = FALSE;
    p_ptr->resist_elec = FALSE;
    if (p_ptr->prace == 9) p_ptr->ffall = TRUE;
    else p_ptr->ffall = FALSE;
    p_ptr->resist_pois = FALSE;
    p_ptr->hold_life = FALSE;
    p_ptr->telepathy = FALSE;
    p_ptr->immune_fire = FALSE;
    p_ptr->immune_acid = FALSE;
    p_ptr->immune_pois = FALSE;
    p_ptr->immune_cold = FALSE;
    p_ptr->immune_elec = FALSE;
    p_ptr->light = FALSE;
    p_ptr->resist_conf = FALSE;
    p_ptr->resist_sound = FALSE;
    if (p_ptr->prace == 2) p_ptr->resist_lite = TRUE;
    else p_ptr->resist_lite = FALSE;
    if (p_ptr->prace == 6) p_ptr->resist_dark = TRUE;
    else p_ptr->resist_dark = FALSE;
    p_ptr->resist_chaos = FALSE;
    p_ptr->resist_disen = FALSE;
    p_ptr->resist_shards = FALSE;
    p_ptr->resist_nexus = FALSE;
    if (p_ptr->prace == 5) p_ptr->resist_blind = TRUE;
    else p_ptr->resist_blind = FALSE;
    p_ptr->resist_nether = FALSE;
    p_ptr->resist_fear = FALSE;

    old_dis_ac = p_ptr->dis_ac;
    p_ptr->ptohit = tohit_adj();   /* Real To Hit   */
    p_ptr->ptodam = todam_adj();   /* Real To Dam   */
    p_ptr->ptoac = toac_adj();	   /* Real To AC    */
    p_ptr->pac = 0;		   /* Real AC	     */
    p_ptr->dis_th = p_ptr->ptohit; /* Display To Hit	    */
    p_ptr->dis_td = p_ptr->ptodam; /* Display To Dam	    */
    p_ptr->dis_ac = 0;		   /* Display AC		 */
    p_ptr->dis_tac = p_ptr->ptoac; /* Display To AC	    */
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	i_ptr = &inventory[i];
	if (i_ptr->tval != TV_NOTHING) {
	    if ((TR3_CURSED & i_ptr->flags1) == 0) {
		p_ptr->pac += i_ptr->ac;
		p_ptr->dis_ac += i_ptr->ac;
	    }
	    p_ptr->ptohit += i_ptr->tohit;
	    if (i_ptr->tval != TV_BOW)            	/* Bows can't damage. -CJS- */
		p_ptr->ptodam += i_ptr->todam;
	    p_ptr->ptoac += i_ptr->toac;
	    if (known2_p(i_ptr)) {
		p_ptr->dis_th += i_ptr->tohit;
		if (i_ptr->tval != TV_BOW)
		    p_ptr->dis_td += i_ptr->todam;	/* Bows can't damage. -CJS- */
		p_ptr->dis_tac += i_ptr->toac;
	    }
	}
    }

    if (p_ptr->pclass == 2) {
	i_ptr = &inventory[INVEN_WIELD];
	if (!(i_ptr->flags2 & TR_BLESS_BLADE) && /* blessed blade == no penalty -CWS */
	    (i_ptr->tval == TV_SWORD || i_ptr->tval == TV_POLEARM)) {
	    p_ptr->ptohit -= 2;
	    p_ptr->ptodam -= 2;
	    p_ptr->dis_th -= 2;
	    p_ptr->dis_td -= 2;
	}
    }
    if (weapon_heavy)
	p_ptr->dis_th += (p_ptr->use_stat[A_STR] * 15 -
			  inventory[INVEN_WIELD].weight);

/* don't forget stun adj, or we'll get incorrect values... -CFT */
    if (p_ptr->stun > 50) {
	p_ptr->ptohit -= 20;
	p_ptr->dis_th -= 20;
	p_ptr->ptodam -= 20;
	p_ptr->dis_td -= 20;
    } else if (p_ptr->stun > 0) {
	p_ptr->ptohit -= 5;
	p_ptr->dis_th -= 5;
	p_ptr->ptodam -= 5;
	p_ptr->dis_td -= 5;
    }
/* Add in temporary spell increases	 */
/*
 * these changed from pac to ptoac, since mana now affected by high pac (to
 * sim. encumberence), and these really should be magical bonuses -CFT 
 */
    if (p_ptr->status & PY_INVULN) {
	p_ptr->ptoac += 100;
	p_ptr->dis_tac += 100;
    }
    if (p_ptr->status & PY_BLESSED) {	/* changed to agree w/ code in
					 * dungeon()... -CFT */
	p_ptr->ptoac += 5;
	p_ptr->dis_tac += 5;
	p_ptr->ptohit += 10;
	p_ptr->dis_th += 10;
    }
    if (p_ptr->shield > 0) {
	p_ptr->ptoac += 50;
	p_ptr->dis_tac += 50;
    }
    if (p_ptr->detect_inv > 0)
	p_ptr->see_inv = TRUE;
    if (p_ptr->status & PY_HERO) { /* now agrees w/ code in dungeon() -CFT */
	p_ptr->ptohit += 12;
	p_ptr->dis_th += 12;
    }
    if (p_ptr->status & PY_SHERO) {/* now agrees w/ code in dungeon() -CFT */
	p_ptr->ptohit += 24;
	p_ptr->dis_th += 24;
	p_ptr->ptoac -= 10;	   /* berserk, so not being careful... -CFT */
	p_ptr->dis_tac -= 10;
    }
    p_ptr->dis_ac += p_ptr->dis_tac;	/* this moved from above, so it will
					 * show ac adjustments from spells...
					 * -CFT */

/* can't print AC here because might be in a store */
    p_ptr->status |= PY_ARMOR;	   /* This was in an if, but I want to be
				    * sure ac is shown properly... -CFT */

    item_flags1 = 0L;
    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	item_flags1 |= i_ptr->flags1;
	i_ptr++;
    }
    item_flags2 = 0L;
    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i <= INVEN_LIGHT; i++) {
	item_flags2 |= i_ptr->flags2;
	i_ptr++;
    }

    if (TR3_SLOW_DIGEST & item_flags1) p_ptr->slow_digest = TRUE;
    if (TR3_AGGRAVATE & item_flags1) p_ptr->aggravate = TRUE;
    if (TR3_TELEPORT & item_flags1) p_ptr->teleport = TRUE;
    if (TR3_REGEN & item_flags1) p_ptr->regenerate = TRUE;
    if (TR2_RES_FIRE & item_flags1) p_ptr->resist_fire = TRUE;
    if (TR2_RES_ACID & item_flags1) p_ptr->resist_acid = TRUE;
    if (TR2_RES_COLD & item_flags1) p_ptr->resist_cold = TRUE;
    if (TR2_RES_POIS & item_flags1) p_ptr->resist_pois = TRUE;
    if (TR2_HOLD_LIFE & item_flags2) p_ptr->hold_life = TRUE;
    if (TR3_TELEPATHY & item_flags2) p_ptr->telepathy = TRUE;
    if (TR2_IM_FIRE & item_flags2) p_ptr->immune_fire = TRUE;
    if (TR2_IM_ACID & item_flags2) p_ptr->immune_acid = TRUE;
    if (TR2_IM_COLD & item_flags2) p_ptr->immune_cold = TRUE;
    if (TR2_IM_ELEC & item_flags2) p_ptr->immune_elec = TRUE;
    if (TR2_IM_POIS & item_flags2) p_ptr->immune_pois = TRUE;
    if (TR3_LITE & item_flags2) p_ptr->light = TRUE;
    if (TR2_FREE_ACT & item_flags1) p_ptr->free_act = TRUE;
    if (TR3_SEE_INVIS & item_flags1) p_ptr->see_inv = TRUE;
    if (TR2_RES_ELEC & item_flags1) p_ptr->resist_elec = TRUE;
    if (TR3_FEATHER & item_flags1) p_ptr->ffall = TRUE;
    if (TR2_RES_CONF & item_flags2) p_ptr->resist_conf = TRUE;
    if (TR2_RES_SOUND & item_flags2) p_ptr->resist_sound = TRUE;
    if (TR2_RES_LITE & item_flags2) p_ptr->resist_lite = TRUE;
    if (TR2_RES_DARK & item_flags2) p_ptr->resist_dark = TRUE;
    if (TR2_RES_CHAOS & item_flags2) p_ptr->resist_chaos = TRUE;
    if (TR2_RES_DISEN & item_flags2) p_ptr->resist_disen = TRUE;
    if (TR2_RES_SHARDS & item_flags2) p_ptr->resist_shards = TRUE;
    if (TR2_RES_NEXUS & item_flags2) p_ptr->resist_nexus = TRUE;
    if (TR2_RES_BLIND & item_flags2) p_ptr->resist_blind = TRUE;
    if (TR2_RES_NETHER & item_flags2) p_ptr->resist_nether = TRUE;
    if (TR2_RES_FEAR & item_flags2) p_ptr->resist_fear = TRUE;

    i_ptr = &inventory[INVEN_WIELD];
    for (i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
	if (TR_SUST_STAT & i_ptr->flags1)
	    switch (i_ptr->pval) {
	      case 1:
		p_ptr->sustain_str = TRUE;
		break;
	      case 2:
		p_ptr->sustain_int = TRUE;
		break;
	      case 3:
		p_ptr->sustain_wis = TRUE;
		break;
	      case 4:
		p_ptr->sustain_con = TRUE;
		break;
	      case 5:
		p_ptr->sustain_dex = TRUE;
		break;
	      case 6:
		p_ptr->sustain_chr = TRUE;
		break;
	      case 10:		   /* :-) ~Ludwig the Hacker!!! */
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;
	      default:
		break;
	    }
	i_ptr++;
    }

    if (p_ptr->slow_digest)
	p_ptr->food_digested--;
    if (p_ptr->regenerate)
	p_ptr->food_digested += 3;

    if (class[p_ptr->pclass].spell == MAGE) {
	calc_mana(A_INT);
    } else if (class[p_ptr->pclass].spell == PRIEST) {
	calc_mana(A_WIS);
    }
}



/*
 * Remove item from equipment list		-RAK-	
 */
void inven_takeoff(int item_val, int posn)
{
    bigvtype             out_val, prt2;
    register inven_type *t_ptr;

    cptr p;

    equip_ctr--;
    t_ptr = &inventory[item_val];
    inven_weight -= t_ptr->weight * t_ptr->number;
    p_ptr->status |= PY_STR_WGT;

    if (item_val == INVEN_WIELD || item_val == INVEN_AUX)
	p = "Was wielding ";
    else if (item_val == INVEN_LIGHT)
	p = "Light source was ";
    else
	p = "Was wearing ";

    objdes(prt2, t_ptr, TRUE);
    if (posn >= 0)
	(void)sprintf(out_val, "%s%s. (%c)", p, prt2, 'a' + posn);
    else if (posn == -1)
	(void)sprintf(out_val, "%s%s.", p, prt2);
    msg_print(out_val);
    if (item_val != INVEN_AUX)	   /* For secondary weapon  */
	py_bonuses(t_ptr, -1);
    invcopy(t_ptr, OBJ_NOTHING);
}


/*
 * Used to verify if this really is the item we wish to wear or read.
 */
int verify(cptr prompt, int item)
{
    bigvtype out_str, object;

    objdes(object, &inventory[item], TRUE);
    (void)sprintf(out_str, "%s %s? ", prompt, object);
    return get_check(out_str);
}



/*
 * Drops (some of) an item from inventory to "near" the current location
 */
static void inven_drop(int item_val, int drop_all)
{
    int                  i;
    inven_type		*i_ptr;
    vtype                prt2;
    bigvtype             prt1;

    i_ptr = &inventory[item_val];

    if (cave[char_row][char_col].i_idx != 0)
	(void)delete_object(char_row, char_col);
    i = i_pop();
    i_list[i] = *i_ptr;
    cave[char_row][char_col].i_idx = i;

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
    p_ptr->status |= PY_STR_WGT;
}


/*
 * All inventory commands (wear, exchange, take off, drop, inventory and
 * equipment) are handled in an alternative command input mode, which accepts
 * any of the inventory commands. 
 *
 * It is intended that this function be called several times in succession, as
 * some commands take up a turn, and the rest of moria must proceed in the
 * interim. A global variable is provided, doing_inven, which is normally
 * zero; however if on return from inven_command it is expected that
 * inven_command should be called *again*, (being still in inventory command
 * input mode), then doing_inven is set to the inventory command character
 * which should be used in the next call to inven_command. 
 *
 * On return, the screen is restored, but not flushed. Provided no flush of the
 * screen takes place before the next call to inven_command, the inventory
 * command screen is silently redisplayed, and no actual output takes place
 * at all. If the screen is flushed before a subsequent call, then the player
 * is prompted to see if we should continue. This allows the player to see
 * any changes that take place on the screen during inventory command input. 
 *
 * The global variable, screen_change, is cleared by inven_command, and set when
 * the screen is flushed. This is the means by which inven_command tell if
 * the screen has been flushed. 
 *
 * The display of inventory items is kept to the right of the screen to minimize
 * the work done to restore the screen afterwards.		-CJS- 
 */

/* Inventory command screen states. */
#define BLANK_SCR	0
#define EQUIP_SCR	1
#define INVEN_SCR	2
#define WEAR_SCR	3
#define HELP_SCR	4
#define WRONG_SCR	5

/* Keep track of the state of the inventory screen. */
static int scr_state, scr_left, scr_base;
static int wear_low, wear_high;

/* Draw the inventory screen. */
static void inven_screen(int new_scr)
{
    register int line = 0;

    if (new_scr != scr_state) {
	scr_state = new_scr;
	switch (new_scr) {
	  case BLANK_SCR:
	    line = 0;
	    break;
	  case HELP_SCR:
	    if (scr_left > 52)
		scr_left = 52;
	    prt("  e  : list used equipment", 1, scr_left);
	    prt("  i  : inventory of pack", 2, scr_left);
	    prt("  t  : take off item", 3, scr_left);
	    prt("  w  : wear or wield object", 4, scr_left);
	    prt("  x  : exchange weapons", 5, scr_left);
	    prt("  d  : drop object", 6, scr_left);
	    prt("  ESC: exit", 7, scr_left);
	    line = 7;
	    break;
	  case INVEN_SCR:
	    scr_left = show_inven(0, inven_ctr - 1, show_inven_weight, scr_left, 0);
	    line = inven_ctr;
	    break;
	  case WEAR_SCR:
	    scr_left = show_inven(wear_low, wear_high, show_inven_weight, scr_left, 0);
	    line = wear_high - wear_low + 1;
	    break;
	  case EQUIP_SCR:
	    scr_left = show_equip(show_equip_weight, scr_left);
	    line = equip_ctr;
	    break;
	}
	if (line >= scr_base) {
	    scr_base = line + 1;
	    erase_line(scr_base, scr_left);
	} else {
	    while (++line <= scr_base)
		erase_line(line, scr_left);
	}
    }
}

/* This does all the work. */
void inven_command(int command)
{
    register int         slot = 0, item;
    int                  tmp, tmp2, selecting, from, to, light_chg = FALSE;
    const char          *prompt, *swap, *disp, *string;
    char                 which, query;
    bigvtype             prt1, prt2;
    register inven_type *i_ptr;
    inven_type           tmp_obj;

    free_turn_flag = TRUE;
    save_screen();
/* Take up where we left off after a previous inventory command. -CJS- */
    if (doing_inven) {
    /* If the screen has been flushed, we need to redraw. If the command is a
     * simple ' ' to recover the screen, just quit. Otherwise, check and see
     * what the user wants. 
     */
	if (screen_change) {
	    if (command == ' ' || !get_check("Continuing with inventory command?")) {
		doing_inven = FALSE;
		return;
	    }
	    scr_left = 50;
	    scr_base = 0;
	}
	tmp = scr_state;
	scr_state = WRONG_SCR;
	inven_screen(tmp);
    } else {
	scr_left = 50;
	scr_base = 0;
    /* this forces exit of inven_command() if selecting is not set true */
	scr_state = BLANK_SCR;
    }
    do {
	if (isupper((int)command))
	    command = tolower((int)command);

    /* Simple command getting and screen selection. */
	selecting = FALSE;
	switch (command) {
	  case 'i':		   /* Inventory	    */
	    if (inven_ctr == 0)
		msg_print("You are not carrying anything.");
	    else
		inven_screen(INVEN_SCR);
	    break;
	  case 'e':		   /* Equipment	   */
	    if (equip_ctr == 0)
		msg_print("You are not using any equipment.");
	    else
		inven_screen(EQUIP_SCR);
	    break;
	  case 't':		   /* Take off	   */
	    if (equip_ctr == 0)
		msg_print("You are not using any equipment.");
	/* don't print message restarting inven command after taking off
	 * something, it is confusing 
	 */
	    else if (inven_ctr >= INVEN_WIELD && !doing_inven)
		msg_print("You will have to drop something first.");
	    else {
		if (scr_state != BLANK_SCR)
		    inven_screen(EQUIP_SCR);
		selecting = TRUE;
	    }
	    break;
	  case 'd':		   /* Drop */
	    if (inven_ctr == 0 && equip_ctr == 0)
		msg_print("But you're not carrying anything.");
	    else if (cave[char_row][char_col].i_idx != 0)
		msg_print("There's no room to drop anything here.");
	    else {
		selecting = TRUE;
		if ((scr_state == EQUIP_SCR && equip_ctr > 0) || inven_ctr == 0) {
		    if (scr_state != BLANK_SCR)
			inven_screen(EQUIP_SCR);
		    command = 'r'; /* Remove - or take off and drop. */
		} else if (scr_state != BLANK_SCR)
		    inven_screen(INVEN_SCR);
	    }
	    break;
	  case 'w':		   /* Wear/wield	   */
	    for (wear_low = 0;
	     wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR;
		 wear_low++);
	    for (wear_high = wear_low;
		 wear_high < inven_ctr && inventory[wear_high].tval >= TV_MIN_WEAR;
		 wear_high++);
	    wear_high--;
	    if (wear_low > wear_high)
		msg_print("You have nothing to wear or wield.");
	    else {
		if (scr_state != BLANK_SCR && scr_state != INVEN_SCR)
		    inven_screen(WEAR_SCR);
		selecting = TRUE;
	    }
	    break;
	  case 'x':
	    if (inventory[INVEN_WIELD].tval == TV_NOTHING &&
		inventory[INVEN_AUX].tval == TV_NOTHING)
		msg_print("But you are wielding no weapons.");
	    else if (TR3_CURSED & inventory[INVEN_WIELD].flags1) {
		objdes(prt1, &inventory[INVEN_WIELD], FALSE);
		(void)sprintf(prt2,
		     "The %s you are wielding appears to be cursed.", prt1);
		msg_print(prt2);
	    } else {
		free_turn_flag = FALSE;
		tmp_obj = inventory[INVEN_AUX];
		inventory[INVEN_AUX] = inventory[INVEN_WIELD];
		inventory[INVEN_WIELD] = tmp_obj;
		if (scr_state == EQUIP_SCR)
		    scr_left = show_equip(show_inven_weight, scr_left);
		py_bonuses(&inventory[INVEN_AUX], -1);	/* Subtract bonuses */
		py_bonuses(&inventory[INVEN_WIELD], 1);	/* Add bonuses    */

		if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
		    (void)strcpy(prt1, "Primary weapon   : ");
		    objdes(prt2, &inventory[INVEN_WIELD], TRUE);
		    msg_print(strcat(prt1, prt2));
		} else
		    msg_print("No primary weapon.");

	    /* this is a new weapon, so clear the heavy flag */
/* no, don't; the check_strength will clear it if it needs to be cleared
		weapon_heavy = FALSE; */
		check_strength();
	    }
	    break;
	  case ' ':		   /* Dummy command to return again to main
				    * prompt. */
	    break;
	  case '?':
	    inven_screen(HELP_SCR);
	    break;
	  default:
	/* Nonsense command					   */
	    bell();
	    break;
	}

    /* Clear the doing_inven flag here, instead of at beginning, so that can
     * use it to control when messages above appear. 
     */
	doing_inven = 0;

    /* Keep looking for objects to drop/wear/take off/throw off */
	which = 'z';
	while (selecting && free_turn_flag) {
	    swap = "";
	    if (command == 'w') {
		from = wear_low;
		to = wear_high;
		prompt = "Wear/Wield";
	    } else {
		from = 0;
		if (command == 'd') {
		    to = inven_ctr - 1;
		    prompt = "Drop";
		    if (equip_ctr > 0)
			swap = ", / for Equip";
		} else {
		    to = equip_ctr - 1;
		    if (command == 't')
			prompt = "Take off";
		    else {	   /* command == 'r' */
			prompt = "Throw off";
			if (inven_ctr > 0)
			    swap = ", / for Inven";
		    }
		}
	    }
	    if (from > to)
		selecting = FALSE;
	    else {
		if (scr_state == BLANK_SCR)
		    disp = ", * to list";
		else
		    disp = "";
		(void)sprintf(prt1,
		   "(%c-%c%s%s, space to break, ESC to exit) %s which one?",
			      from + 'a', to + 'a', disp, swap, prompt);

	    /* Abort everything. */
		if (!get_com(prt1, &which)) {
		    selecting = FALSE;
		    which = ESCAPE;
		}
	    /* Draw the screen and maybe exit to main prompt. */
		else if (which == ' ' || which == '*') {
		    if (command == 't' || command == 'r')
			inven_screen(EQUIP_SCR);
		    else if (command == 'w' && scr_state != INVEN_SCR)
			inven_screen(WEAR_SCR);
		    else
			inven_screen(INVEN_SCR);
		    if (which == ' ')
			selecting = FALSE;
		}
	    /* Swap screens (for drop) */
		else if (which == '/' && swap[0]) {
		    if (command == 'd')
			command = 'r';
		    else
			command = 'd';
		    if (scr_state == EQUIP_SCR)
			inven_screen(INVEN_SCR);
		    else if (scr_state == INVEN_SCR)
			inven_screen(EQUIP_SCR);
		} else if (((which < from + 'a') || (which > to + 'a'))
			   && ((which < from + 'A') || (which > to + 'A')))
		    bell();
		else {		   /* Found an item! */
		    if (isupper((int)which))
			item = which - 'A';
		    else
			item = which - 'a';
		    if (command == 'r' || command == 't') {
		    /* Get its place in the equipment list. */
			tmp = item;
			item = 21;
			do {
			    item++;
			    if (inventory[item].tval != TV_NOTHING)
				tmp--;
			}
			while (tmp >= 0);
			if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else if (TR3_CURSED & inventory[item].flags1) {
			    msg_print("Hmmm, it seems to be cursed.");
			    item = (-1);
			} else if (command == 't' &&
				   !inven_check_num(&inventory[item])) {
			    if (cave[char_row][char_col].i_idx != 0) {
				msg_print("You can't carry it.");
				item = (-1);
			    } else if (get_check("You can't carry it.  Drop it?"))
				command = 'r';
			    else
				item = (-1);
			}
			if (item >= 0)
			    if (item == INVEN_LIGHT)
				light_chg = TRUE;
			if (command == 'r') {
			    inven_drop(item, TRUE);

			    /* As a safety measure, set the player's inven weight to 0,
			     * when the last object is dropped */
			    if (inven_ctr == 0 && equip_ctr == 0)
				inven_weight = 0;
			} else if (item >= 0) {
			    slot = inven_carry(&inventory[item]);
			    inven_takeoff(item, slot);
			}
			check_strength();
			free_turn_flag = FALSE;
			if (command == 'r')
			    selecting = FALSE;
		    } else if (command == 'w') {

    /* Wearing. Go to a bit of trouble over replacing existing equipment. */
			if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else
			    switch (inventory[item].tval) { /* Slot for equipment */
			      case TV_SHOT:
			      case TV_BOLT:
			      case TV_ARROW:
			      case TV_BOW:
			      case TV_HAFTED:
			      case TV_POLEARM:
			      case TV_SWORD:
			      case TV_DIGGING:
				slot = INVEN_WIELD;
				break;
			      case TV_LITE:
				slot = INVEN_LIGHT;
				break;
			      case TV_BOOTS:
				slot = INVEN_FEET;
				break;
			      case TV_GLOVES:
				slot = INVEN_HANDS;
				break;
			      case TV_CLOAK:
				slot = INVEN_OUTER;
				break;
			      case TV_HELM:
				slot = INVEN_HEAD;
				break;
			      case TV_SHIELD:
				slot = INVEN_ARM;
				break;
			      case TV_HARD_ARMOR:
			      case TV_SOFT_ARMOR:
				slot = INVEN_BODY;
				break;
			      case TV_AMULET:
				slot = INVEN_NECK;
				break;
			      case TV_RING:
				if (inventory[INVEN_RIGHT].tval == TV_NOTHING)
				    slot = INVEN_RIGHT;
				else if (inventory[INVEN_LEFT].tval == TV_NOTHING)
				    slot = INVEN_LEFT;
				else {
				    slot = 0;
				/*
				 * Rings. Give some choice over where they
				 * go. 
				 */
				    do {
					if (!get_com(
				 "Put ring on which hand (l/r/L/R)?", &query)) {
					    item = (-1);
					    slot = (-1);
					} else if (query == 'l')
					    slot = INVEN_LEFT;
					else if (query == 'r')
					    slot = INVEN_RIGHT;
					else {
					    if (query == 'L')
						slot = INVEN_LEFT;
					    else if (query == 'R')
						slot = INVEN_RIGHT;
					    else
						bell();
					    if (slot && !verify("Replace", slot))
						slot = 0;
					}
				    }
				    while (slot == 0);
				}
				break;
			      default:
		msg_print("IMPOSSIBLE: I don't see how you can use that.");
				item = (-1);
				break;
			    }
			if (item >= 0 && inventory[slot].tval != TV_NOTHING) {
			    if (TR3_CURSED & inventory[slot].flags1) {
				objdes(prt1, &inventory[slot], FALSE);
				(void)sprintf(prt2, "The %s you are ", prt1);
				if (slot == INVEN_WIELD)	/* changed from
								 * INVEN_HEAD -CFT */
				    (void)strcat(prt2, "wielding ");
				else
				    (void)strcat(prt2, "wearing ");
				msg_print(strcat(prt2, "appears to be cursed."));
				item = (-1);
			    } else if (inventory[item].sval == ITEM_GROUP_MIN &&
				       inventory[item].number > 1 &&
				       !inven_check_num(&inventory[slot])) {
 /* this can happen if try to wield a torch, and have more than one in your inventory */
				msg_print("You will have to drop something first.");
				item = (-1);
			    }
			}
			if (item >= 0) {
			/* OK. Wear it. */
			    free_turn_flag = FALSE;

			    if (slot == INVEN_LIGHT)
				light_chg = TRUE;

			/* first remove new item from inventory */
			    tmp_obj = inventory[item];
			    i_ptr = &tmp_obj;

			    wear_high--;
			/* Fix for torches	   */
			    if (i_ptr->number > 1
				&& i_ptr->sval <= ITEM_SINGLE_STACK_MAX) {
				i_ptr->number = 1;
				wear_high++;
			    }
			    inven_weight += i_ptr->weight * i_ptr->number;
			    inven_destroy(item);	/* Subtracts weight */

/* second, add old item to inv and remove from equipment list, if necessary */
			    i_ptr = &inventory[slot];
			    if (i_ptr->tval != TV_NOTHING) {
				tmp2 = inven_ctr;
				tmp = inven_carry(i_ptr);
/* if item removed did not stack with anything in inventory, then increment wear_high */
				if (inven_ctr != tmp2)
				    wear_high++;
				inven_takeoff(slot, tmp);
			    }
			/* third, wear new item */
			    *i_ptr = tmp_obj;
			    equip_ctr++;
			    py_bonuses(i_ptr, 1);
			    if (slot == INVEN_WIELD)
				string = "You are wielding";
			    else if (slot == INVEN_LIGHT)
				string = "Your light source is";
			    else
				string = "You are wearing";
			    objdes(prt2, i_ptr, TRUE);
			/* Get the right equipment letter. */
			    tmp = INVEN_WIELD;
			    item = 0;
			    while (tmp != slot)
				if (inventory[tmp++].tval != TV_NOTHING)
				    item++;

			    (void)sprintf(prt1, "%s %s. (%c)", string, prt2,
					  'a' + item);
			    msg_print(prt1);
			/* check_str will clear the heavy flag if necessary */
			    check_strength();
			    if (i_ptr->flags1 & TR3_CURSED) {
				msg_print("Oops! It feels deathly cold!");
				add_inscribe(i_ptr, ID_DAMD);
			    /* To force a cost of 0, even if unidentified. */
			    /* i_ptr->cost = (-1); Not... */
			    }
			}
		    } else {	   /* command == 'd' */
			if (inventory[item].number > 1) {
			    objdes(prt1, &inventory[item], TRUE);
			    (void)sprintf(prt2, "Drop all %s?  [y/n]", prt1);
			    prt(prt2, 0, 0);
			    query = inkey();
			    if (query != 'y' && query != 'n') {
				if (query != ESCAPE)
				    bell();
				erase_line(MSG_LINE, 0);
				item = (-1);
			    }
			} else if (isupper((int)which) && !verify(prompt, item))
			    item = (-1);
			else
			    query = 'y';
			if (item >= 0) {
			    free_turn_flag = FALSE;	/* Player turn   */
			    inven_drop(item, query == 'y');
			    check_strength();
			}
			selecting = FALSE;
			/* As a safety measure, set the player's inven weight to 0,
			 * when the last object is dropped. */
			if (inven_ctr == 0 && equip_ctr == 0)
			    inven_weight = 0;
		    }
		    if (free_turn_flag == FALSE && scr_state == BLANK_SCR)
			selecting = FALSE;
		}
	    }
	}
	if (which == ESCAPE || scr_state == BLANK_SCR)
	    command = ESCAPE;
	else if (!free_turn_flag) {
	/* Save state for recovery if they want to call us again next turn. */
	    if (selecting)
		doing_inven = command;
	    else
		doing_inven = ' '; /* A dummy command to recover screen. */
	/* flush last message before clearing screen_change and exiting */
	    msg_print(NULL);
	    screen_change = FALSE; /* This lets us know if the world changes */
	    command = ESCAPE;
	} else {
	/* Put an appropriate header. */
	    if (scr_state == INVEN_SCR) {
		if (!show_inven_weight || inven_ctr == 0)
		    (void)sprintf(prt1,
			  "You are carrying %d.%d pounds. In your pack there is %s",
				  inven_weight / 10, inven_weight % 10,
				  (inven_ctr == 0 ? "nothing." : "-"));
		else
		    (void)sprintf(prt1,
		  "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. %s",
				  inven_weight / 10, inven_weight % 10,
				  weight_limit() / 10, weight_limit() % 10,
				  "In your pack is -");
		prt(prt1, 0, 0);
	    } else if (scr_state == WEAR_SCR) {
		if (wear_high < wear_low)
		    prt("You have nothing you could wield.", 0, 0);
		else
		    prt("You could wield -", 0, 0);
	    } else if (scr_state == EQUIP_SCR) {
		if (equip_ctr == 0)
		    prt("You are not using anything.", 0, 0);
		else
		    prt("You are using -", 0, 0);
	    } else
		prt("Allowed commands:", 0, 0);
	    erase_line(scr_base, scr_left);
	    put_str("e/i/t/w/x/d/?/ESC:", scr_base, 60);
	    command = inkey();
	    erase_line(scr_base, scr_left);
	}
    }
    while (command != ESCAPE);
    if (scr_state != BLANK_SCR)
	restore_screen();

/* Update screen for changed light radius. -DGK */
/* Fix redisplay of lighting when in stores. -DGK */
    if (light_chg && !in_store_flag) {
	register int        i, j;
	int                 min_i, max_i, min_j, max_j;

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
	min_i = MY_MAX(0, (char_row - cur_lite));
	max_i = MY_MIN(cur_height, (char_row + cur_lite));
	min_j = MY_MAX(0, (char_col - cur_lite));
	max_j = MY_MIN(cur_width, (char_col + cur_lite));

	for (i = min_i; i <= max_i; i++)
	    for (j = min_j; j <= max_j; j++)
		cave[i][j].tl = FALSE;

#ifdef TC_COLOR
	textcolor(LIGHTGRAY);
#endif
	tmp2 = cur_lite;
	print('@', char_row, char_col);
	if (inventory[INVEN_LIGHT].tval == TV_LITE)
	    tmp = inventory[INVEN_LIGHT].sval;
	else
	    tmp = 195;
	cur_lite = 1 + (tmp < 190) + (tmp == 4 || tmp == 6);
	if (tmp2 < cur_lite)
	    tmp2 = cur_lite;

	if (!p_ptr->blind) {
	    min_i = MY_MAX(0, (char_row - cur_lite));
	    max_i = MY_MIN(cur_height, (char_row + cur_lite));
	    min_j = MY_MAX(0, (char_col - cur_lite));
	    max_j = MY_MIN(cur_width, (char_col + cur_lite));

	    for (i = min_i; i <= max_i; i++)
		for (j = min_j; j <= max_j; j++)
		    if (los(char_row, char_col, i, j) &&
			distance(char_row, char_col, i, j) <= cur_lite)
			cave[i][j].tl = TRUE;
	}

	min_i = MY_MAX(0, (char_row - tmp2));
	max_i = MY_MIN(cur_height, (char_row + tmp2));
	min_j = MY_MAX(0, (char_col - tmp2));
	max_j = MY_MIN(cur_width, (char_col + tmp2));
	for (i = min_i; i <= max_i; i++)
	    for (j = min_j; j <= max_j; j++)
		lite_spot(i, j);
    }
    calc_bonuses();

    /* If we ain't in a store, do the equippy chars -DGK*/
    if (!in_store_flag) 
	prt_equippy_chars();
}


static void flood_light(int y, int x)
{
    register cave_type *c_ptr;
    register int temp;
    c_ptr = &cave[y][x];
    if (c_ptr->lr) {
	temp=c_ptr->tl;
	c_ptr->tl = TRUE;
	if (c_ptr->fval<MIN_CLOSED_SPACE && temp==FALSE) {
	    flood_light(y+1,x);
	    flood_light(y-1,x);
	    flood_light(y,x+1);
	    flood_light(y,x-1);
	    flood_light(y+1,x+1);
	    flood_light(y-1,x-1);
	    flood_light(y-1,x+1);
	    flood_light(y+1,x-1);
	}
    }
}

static void flood_permanent(int y, int x)
{
    register cave_type *c_ptr;
    c_ptr = &cave[y][x];
    if (c_ptr->tl) {
	c_ptr->tl = FALSE;
	c_ptr->pl = TRUE;
	if (c_ptr->fval==NT_DARK_FLOOR)
	    c_ptr->fval=NT_LIGHT_FLOOR;
	else if (c_ptr->fval==DARK_FLOOR)
	    c_ptr->fval=LIGHT_FLOOR;
#ifdef MSDOS
	lite_spot(y,x); /* this does all that; plus color-safe -CFT */
#else
	if ((y-panel_row_prt)<23 && (y-panel_row_prt)>0 &&
	    (x-panel_col_prt)>12 && (x-panel_col_prt)<80)
	    print(loc_symbol(y, x), y, x);
#endif
	if (c_ptr->fval<MIN_CLOSED_SPACE) {
	    flood_permanent(y+1,x);
      flood_permanent(y-1,x);
	    flood_permanent(y,x+1);
	    flood_permanent(y,x-1);
	    flood_permanent(y+1,x+1);
	    flood_permanent(y-1,x-1);
	    flood_permanent(y-1,x+1);
	    flood_permanent(y+1,x-1);
	}
    }
}

static void flood_permanent_dark(int y, int x)
{
    register cave_type *c_ptr;
    c_ptr = &cave[y][x];
    if (c_ptr->tl) {
	c_ptr->tl = FALSE;
	if (c_ptr->fval==NT_LIGHT_FLOOR)
	    c_ptr->fval=NT_DARK_FLOOR;
	else if (c_ptr->fval==LIGHT_FLOOR)
	    c_ptr->fval=DARK_FLOOR;
#ifdef MSDOS
	if (panel_contains(y,x)) {
	    if (c_ptr->fval < MIN_CLOSED_SPACE){
		c_ptr->pl = FALSE;
		flood_permanent_dark(y+1,x);
		flood_permanent_dark(y-1,x);
		flood_permanent_dark(y,x+1);
		flood_permanent_dark(y,x-1);
		flood_permanent_dark(y+1,x+1);
		flood_permanent_dark(y-1,x-1);
		flood_permanent_dark(y-1,x+1);
		flood_permanent_dark(y+1,x-1);
	    }
	    lite_spot(y,x);
	}
#else
	if ((y-panel_row_prt)<23 && (y-panel_row_prt)>0 &&
	    (x-panel_col_prt)>12 && (x-panel_col_prt)<80)
	    if (c_ptr->fval<MIN_CLOSED_SPACE) {
		c_ptr->pl = FALSE;
		flood_permanent_dark(y+1,x);
		flood_permanent_dark(y-1,x);
		flood_permanent_dark(y,x+1);
		flood_permanent_dark(y,x-1);
		flood_permanent_dark(y+1,x+1);
		flood_permanent_dark(y-1,x-1);
		flood_permanent_dark(y-1,x+1);
		flood_permanent_dark(y+1,x-1);
	    }
	print(loc_symbol(y, x), y, x);
#endif
    }
}

void light_room(int y, int x)
{
    register cave_type *c_ptr;
    register monster_type  *m_ptr;

    c_ptr = &cave[y][x];
    if (!c_ptr->pl && c_ptr->lr) {
	c_ptr->pl = TRUE;
	m_ptr = &m_list[c_ptr->m_idx];

/* Monsters that are intelligent wake up all the time; non-MINDLESS monsters wake
 * up 1/3 the time, and MINDLESS monsters wake up 1/10 the time -CWS
 */
	if ((r_list[m_ptr->r_idx].cflags2 & MF2_INTELLIGENT) ||
	    (!(r_list[m_ptr->r_idx].cflags2 & MF2_MINDLESS) && (randint(3) == 1)) ||
	    (randint(10) == 1))
	    m_ptr->csleep = 0;

	if (c_ptr->fval == NT_DARK_FLOOR)
	    c_ptr->fval = NT_LIGHT_FLOOR;
	else if (c_ptr->fval == DARK_FLOOR)
	    c_ptr->fval = LIGHT_FLOOR;
#ifdef MSDOS
	lite_spot(y, x);	   /* this does all that; plus color-safe -CFT */
#else
	if ((y - panel_row_prt) < 23 && (y - panel_row_prt) > 0 &&
	    (x - panel_col_prt) > 12 && (x - panel_col_prt) < 80)
	    print(loc_symbol(y, x), y, x);
#endif
    
	if (c_ptr->fval < MIN_CLOSED_SPACE) {
	    c_ptr = &cave[y + 1][x];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x);
	    
	    c_ptr = &cave[y - 1][x];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x);

	    c_ptr = &cave[y][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y, x + 1);
	    
	    c_ptr = &cave[y][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y, x - 1);
	    
	    c_ptr = &cave[y + 1][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x + 1);
	    
	    c_ptr = &cave[y - 1][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x - 1);
	    
	    c_ptr = &cave[y - 1][x + 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y - 1, x + 1);
	    
	    c_ptr = &cave[y + 1][x - 1];
	    if ((!c_ptr->pl) && (c_ptr->lr))
		light_room(y + 1, x - 1);
	}
    }     
}

void darken_room(int y, int x)
{
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];
    if (c_ptr->pl && c_ptr->lr) {
	c_ptr->tl = FALSE;
	if (c_ptr->fval == NT_LIGHT_FLOOR)
	    c_ptr->fval = NT_DARK_FLOOR;
	else if (c_ptr->fval == LIGHT_FLOOR)
	    c_ptr->fval = DARK_FLOOR;
#ifdef MSDOS
	if (panel_contains(y, x))
	    if (c_ptr->fval < MIN_CLOSED_SPACE) {
#else
	if ((y - panel_row_prt) < 23 && (y - panel_row_prt) > 0 &&
	    (x - panel_col_prt) > 12 && (x - panel_col_prt) < 80)
	    if (c_ptr->fval < MIN_CLOSED_SPACE) {
#endif
		c_ptr->pl = FALSE;
		darken_room(y + 1, x);
		darken_room(y - 1, x);
		darken_room(y, x + 1);
		darken_room(y, x - 1);
		darken_room(y + 1, x + 1);
		darken_room(y - 1, x - 1);
		darken_room(y - 1, x + 1);
		darken_room(y + 1, x - 1);
	    }
	print(loc_symbol(y, x), y, x);
    }
}


/* Normal movement					 */
/* When FIND_FLAG,  light only permanent features	 */
static void sub1_move_light(int y1, int x1, int y2, int x2)
{
    register int        i, j;
    register cave_type *c_ptr;
    int                 tval, top, left, bottom, right;
    int          min_i, max_i, min_j, max_j;

    if (light_flag) {
	darken_player(y1, x1);
	if (find_flag && !find_prself)
	    light_flag = FALSE;
    } else if (!find_flag || find_prself)
	light_flag = TRUE;

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
    min_i = MY_MAX(0, (y2 - cur_lite));
    max_i = MY_MIN(cur_height, (y2 + cur_lite));
    min_j = MY_MAX(0, (x2 - cur_lite));
    max_j = MY_MIN(cur_width, (x2 + cur_lite));
    for (i = min_i; i <= max_i; i++)
	for (j = min_j; j <= max_j; j++)
	    if (los(y2, x2, i, j) && distance(i, j, y2, x2) <= cur_lite) {
		c_ptr = &cave[i][j];
	    /* only light up if normal movement */
		if (light_flag)
		    c_ptr->tl = TRUE;
		if (c_ptr->fval >= MIN_WALL)
		    c_ptr->pl = TRUE;
		else if (!c_ptr->fm && c_ptr->i_idx != 0) {
		    tval = i_list[c_ptr->i_idx].tval;
		    if ((tval >= TV_MIN_VISIBLE) && (tval <= TV_MAX_VISIBLE))
			c_ptr->fm = TRUE;
		}
	    }
/* From uppermost to bottom most lines player was on.	 */
    if (y1 < y2) {
	top = y1 - cur_lite;
	bottom = y2 + cur_lite;
    } else {
	top = y2 - cur_lite;
	bottom = y1 + cur_lite;
    }
    if (x1 < x2) {
	left = x1 - cur_lite;
	right = x2 + cur_lite;
    } else {
	left = x2 - cur_lite;
	right = x1 + cur_lite;
    }
    for (i = top; i <= bottom; i++)
	for (j = left; j <= right; j++)	/* Leftmost to rightmost do */
	    lite_spot(i, j);	   /* this does that, plus panel check +
				    * color safe */
}


/* When blinded,  move only the player symbol.		 */
/* With no light,  movement becomes involved.		 */
static void sub3_move_light(int y1, int x1, int y2, int x2)
{
    if (light_flag) {
	darken_player(y1, x1);
	light_flag = FALSE;
    } else if (!find_flag || find_prself)	/* um55 change -CFT */
	lite_spot(y1, x1);

#ifdef TC_COLOR
    if (!no_color_flag)
	textcolor(LIGHTGRAY);
#endif

    if (!find_flag || find_prself)
	print('@', y2, x2);
}

void darken_player(int y1, int x1)
{
    int min_i, max_i, min_j, max_j, rad, i, j;

    rad = MY_MAX(cur_lite, old_lite);

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
    min_i = MY_MAX(0, (y1 - rad));
    max_i = MY_MIN(cur_height, (y1 + rad));
    min_j = MY_MAX(0, (x1 - rad));
    max_j = MY_MIN(cur_width, (x1 + rad));

    for (i = min_i; i <= max_i; i++)
        for (j = min_j; j <= max_j; j++) {
	    cave[i][j].tl = FALSE;  /* Turn off light */
	    lite_spot(i, j);
        }
}

/* Package for moving the character's light about the screen	 */
/* Four cases : Normal, Finding, Blind, and Nolight	 -RAK-	 */
void move_light(int y1, int x1, int y2, int x2)
{
    if (p_ptr->blind > 0 || !player_light)
	sub3_move_light(y1, x1, y2, x2);
    else
	sub1_move_light(y1, x1, y2, x2);
}


/* Attacker's level and plusses,  defender's AC		-RAK-	 */
int test_hit(int bth, int level, int pth, int ac, int attack_type)
{
    register int i, die;

    disturb(1, 0);
    i = bth + pth * BTH_PLUS_ADJ
	+ (level * class_level_adj[p_ptr->pclass][attack_type]);
/* pth could be less than 0 if player wielding weapon too heavy for him */
/* always miss 1 out of 20, always hit 1 out of 20 */
    die = randint(20);
    if ((die != 1) && ((die == 20)
		       || ((i > 0) && (randint(i) > ((3 * ac) / 4)))))
	return TRUE;
    else
	return FALSE;
}


/* Decreases players hit points and sets death flag if necessary */
/* -RAK-	 */
void take_hit(int damage, const char *hit_from)
{
    if (p_ptr->invuln > 0 && damage < 9000)
	damage = 0;
    p_ptr->chp -= damage;
    if (p_ptr->chp < 0) {
	if ((wizard) && !(get_check("Die?"))) {
	    p_ptr->chp=p_ptr->mhp;
	    death=FALSE;
	    prt_chp();
	    msg_print("OK, so you don't die.");
	} else {	    
	    if (!death) {
		death = TRUE;
		(void)strcpy(died_from, hit_from);
		total_winner = FALSE;
	    }
	    new_level_flag = TRUE;
	}
    } else
	prt_chp();
    if (p_ptr->chp <= p_ptr->mhp * hitpoint_warn / 10) {
	msg_print("*** LOW HITPOINT WARNING! ***");
	msg_print(NULL);	/* make sure they see it -CWS */
    }
}




