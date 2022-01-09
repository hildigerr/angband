/* File: project.c */


#include "angband.h"




/* This seems like a pretty standard "typedef" */
/* For some reason, it was not being used on Unix */
typedef int (*inven_func)(inven_type *);

/*
 * Destroys a type of item on a given percent chance	-RAK-	 
 */
static int inven_damage(inven_func typ, int perc)
{
    register int index, i, j, offset;
    vtype	tmp_str, out_val;

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



/*
 * AC gets worse					-RAK-
 * Note: This routine affects magical AC bonuses so that stores
 * can detect the damage.
static int minus_ac(u32b typ_dam)
{
    register int         i, j;
    int                  tmp[6], minus, do_damage;
    inven_type		*i_ptr;
    bigvtype		out_val, tmp_str;

    i = 0;
    if (inventory[INVEN_BODY].tval != TV_NOTHING) {
	tmp[i] = INVEN_BODY;
	i++;
    }
    if (inventory[INVEN_ARM].tval != TV_NOTHING) {
	tmp[i] = INVEN_ARM;
	i++;
    }
    if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
	tmp[i] = INVEN_OUTER;
	i++;
    }
    if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
	tmp[i] = INVEN_HANDS;
	i++;
    }
    if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
	tmp[i] = INVEN_HEAD;
	i++;
    }
/* also affect boots */
    if (inventory[INVEN_FEET].tval != TV_NOTHING) {
	tmp[i] = INVEN_FEET;
	i++;
    }
    minus = FALSE;
    if (i > 0) {
	j = tmp[randint(i) - 1];
	i_ptr = &inventory[j];
	switch (typ_dam) {
	  case TR2_RES_ACID:
	    if ((i_ptr->flags & TR2_RES_ACID) || (i_ptr->flags2 & TR2_IM_ACID) ||
		((i_ptr->flags2 & TR_ARTIFACT) && (randint(5)>2)))
		do_damage = FALSE;
	    else
		do_damage = TRUE;
	    break;
	  default:		   /* unknown damage type... */
	    do_damage = FALSE;
	}
	if (do_damage == FALSE) {
	    objdes(tmp_str, &inventory[j], FALSE);
	    (void)sprintf(out_val, "Your %s resists damage!", tmp_str);
	    msg_print(out_val);
	    minus = FALSE;
	} else if ((i_ptr->ac + i_ptr->toac) > 0) {
	    objdes(tmp_str, &inventory[j], FALSE);
	    (void)sprintf(out_val, "Your %s is damaged!", tmp_str);
	    msg_print(out_val);
	    i_ptr->toac--;
	    calc_bonuses();
	    minus = TRUE;
	}
    }
    return (minus);
}


/*
 * Corrode the unsuspecting person's armor		 -RAK-
 */
void corrode_gas(const cptr kb_str)
{
    if (!py.flags.immune_acid)
	if (!minus_ac((u32b) TR2_RES_ACID))
	    take_hit(randint(8), kb_str);
    inven_damage(set_corrodes, 5);
}

/*
 * Throw acid on the hapless victim			-RAK-
 */
void acid_dam(int dam, cptr kb_str)
{
    register int flag;

    if (py.flags.resist_acid > 0) dam = dam / 3;
    if (py.flags.oppose_acid > 0) dam = dam / 3;
    if (py.flags.immune_acid) dam = 1;
    flag = 0;
    if (!py.flags.oppose_acid)
	if (minus_ac((u32b) TR2_RES_ACID)) flag = 1;
    if (py.flags.resist_acid) flag += 2;
    inven_damage(set_acid_affect, 3);
}


/*
 * Lightning bolt the sucker away.			-RAK-
 */
void light_dam(int dam, cptr kb_str)
{
    if (py.flags.oppose_elec) dam = dam / 3;
    if (py.flags.resist_elec) dam = dam / 3;
    if (py.flags.immune_elec) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_lightning_destroy, 3);
}




/*
 *Burn the fool up.					-RAK-
 */
void fire_dam(int dam, cptr kb_str)
{
    if (py.flags.resist_fire) dam = dam / 3;
    if (py.flags.oppose_fire > 0) dam = dam / 3;
    if (py.flags.immune_fire) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_flammable, 3);
}


/*
 * Freeze him to death.				-RAK-
 */
void cold_dam(int dam, cptr kb_str)
{
    if (py.flags.resist_cold) dam = dam / 3;
    if (py.flags.oppose_cold > 0) dam = dam / 3;
    if (py.flags.immune_cold) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_frost_destroy, 5);
}



/*
 * Poison gas the idiot.				-RAK-
 */
void poison_gas(int dam, cptr kb_str)
{
    if (py.flags.oppose_pois > 0) dam = 2 * dam / 3;
    if (py.flags.resist_pois) dam = (dam * 3) / 5;
    if (py.flags.immune_pois) dam = 1;
    take_hit(dam, kb_str);
    if (!(py.flags.resist_pois || py.flags.oppose_pois
	  || py.flags.immune_pois))
	py.flags.poisoned += 12 + randint(dam);
}



