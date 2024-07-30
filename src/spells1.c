/* File: project.c */

/* Purpose: generic bolt/ball/beam code	(-BEN-) */

#include "angband.h"



/*
 * Does a given class of objects (usually) hate acid?
 * Note that acid can either melt or corrode something.
 */
static bool hates_acid(inven_type *i_ptr)
{
    /* Analyze the type */
    switch (i_ptr->tval) {

      /* Wearable items */
      case TV_ARROW:
      case TV_BOLT:
      case TV_BOW:
      case TV_SWORD:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_HELM:
      case TV_SHIELD:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
	return (TRUE);

      /* Staffs/Scrolls are wood/paper */
      case TV_STAFF:
      case TV_SCROLL:
	return (TRUE);

      /* Doors are wood */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
	return (TRUE);

      /* Ouch */
      case TV_CHEST:
	return (TRUE);

      /* Junk is useless */
      case TV_SKELETON:
      case TV_BOTTLE:
      case TV_JUNK:
      case TV_FOOD:
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate electricity?
 */
static bool hates_elec(inven_type *i_ptr)
{
    switch (i_ptr->tval) {

      case TV_RING:
	return (TRUE);
	
      case TV_WAND:
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate fire?
 * Hafted/Polearm weapons have wooden shafts.
 * Arrows/Bows are mostly wooden.
 */
static bool hates_fire(inven_type *i_ptr)
{
    /* Analyze the type */    
    switch (i_ptr->tval) {

      /* Wearable items */
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
	return (TRUE);

      /* Staffs/Scrolls burn */
      case TV_STAFF:
      case TV_SCROLL:
	return (TRUE);

      case TV_POTION:
      case TV_FLASK:
      case TV_FOOD:

      /* Doors are made of wood */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
	return (TRUE);

      case TV_LITE:
	if (e->sval >= 192)	   /* only torches... -CFT */
	    return (TRUE);
    }

    return (FALSE);
}


/*
 * Does a given object (usually) hate cold?
 */
static bool hates_cold(inven_type *i_ptr)
{
    switch (i_ptr->tval) {
      case TV_POTION:
      case TV_FLASK:
	return (TRUE);
    }

    return (FALSE);
}









/*
 * Melt something
 */
int set_acid_destroy(inven_type *i_ptr)
{
    if (!hates_acid(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && ((i_ptr->flags2 & TR2_RES_ACID) || (e->flags2 & TR2_IM_ACID)) return (FALSE);
    return (TRUE);
}


/*
 * Electrical damage
 */
int set_elec_destroy(inven_type *i_ptr)
{
    if (!hates_elec(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && ((i_ptr->flags2 & TR2_RES_ELEC)||(i_ptr->flags2 & TR2_IM_ELEC))) return (FALSE);
    return (TRUE);
}


/*
 * Burn something
 */
int set_fire_destroy(inven_type *i_ptr)
{
    if (!hates_fire(i_ptr)) return (FALSE);
    if (artifact_p(i_ptr)) return (FALSE);
    if (wearable_p(i_ptr) && ((e->flags2 & TR2_RES_FIRE)||(e->flags2 & TR2_IM_FIRE))) return (FALSE);
    return (TRUE);
}


/*
 * Freeze things
 */
int set_cold_destroy(inven_type *i_ptr)
{
    if (!hates_cold(i_ptr)) return (FALSE);
    return (TRUE);
}




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
		    ((inventory[i].number > 1) ? 
		    "One of y" : "Y"),
		    tmp_str, index_to_label(i),
		    ((inventory[i].number > 1) ? "were" : "was"));
	    msg_print(out_val);

		inven_item_increase(i,-1);
		inven_item_optimize(i);

	    j++;
	}
    }
    return (j);
}




/*
 * Acid has hit the player, attempt to affect some armor.
 *
 * Note that the "base armor" of an object never changes.
 */
static int minus_ac(void)
{
    register int         i, j;
    int                  tmp[6];
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
    if (inventory[INVEN_FEET].tval != TV_NOTHING) {
	tmp[i] = INVEN_FEET;
	i++;
    }

    /* Nothing to damage */
    if (i == 0)  return (FALSE);

	j = tmp[randint(i) - 1];
	i_ptr = &inventory[j];

    /* No damage left to be done */
    if (i_ptr->ac + i_ptr->toac <= 0) return (FALSE);


    /* Object resists? */
    if ((i_ptr->flags2 & TR2_RES_ACID) || (i_ptr->flags2 & TR2_IM_ACID) ||
		(artifact_p(i_ptr) && (randint(5)>2))) {
	objdes(tmp_str, &inventory[j], FALSE);
	(void)sprintf(out_val, "Your %s resists damage!", tmp_str);
	msg_print(out_val);
	return (FALSE);
    }

    /* Describe the damage */
    objdes(tmp_str, &inventory[j], FALSE);
    (void)sprintf(out_val, "Your %s is damaged!", tmp_str);
    msg_print(out_val);

    /* Damage the item */
    i_ptr->toac--;
    calc_bonuses();

    /* Item was damaged */
    return (TRUE);
}


/*
 * Corrode the unsuspecting person's armor		 -RAK-
 */
void corrode_gas(const cptr kb_str)
{
    if (!p_ptr->immune_acid)
	if (!minus_ac())
	    take_hit(randint(8), kb_str);
    inven_damage(set_acid_destroy, 5);
}


/*
 * Hurt the player with Acid
 */
void acid_dam(int dam, cptr kb_str)
{
    register int flag;

    if (p_ptr->resist_acid > 0) dam = dam / 3;
    if (p_ptr->oppose_acid > 0) dam = dam / 3;
    if (p_ptr->immune_acid) dam = 1;
    flag = 0;
    if (!p_ptr->oppose_acid)
	if (minus_ac()) flag = 1;
    if (p_ptr->resist_acid) flag += 2;
    inven_damage(set_acid_destroy, 3);
}


/*
 * Hurt the player with electricity
 */
void light_dam(int dam, cptr kb_str)
{
    if (p_ptr->oppose_elec) dam = dam / 3;
    if (p_ptr->resist_elec) dam = dam / 3;
    if (p_ptr->immune_elec) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_elec_destroy, 3);
}




/*
 * Hurt the player with Fire
 */
void fire_dam(int dam, cptr kb_str)
{
    if (p_ptr->resist_fire) dam = dam / 3;
    if (p_ptr->oppose_fire > 0) dam = dam / 3;
    if (p_ptr->immune_fire) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_fire_destroy, 3);
}


/*
 * Hurt the player with Cold
 */
void cold_dam(int dam, cptr kb_str)
{
    if (p_ptr->resist_cold) dam = dam / 3;
    if (p_ptr->oppose_cold > 0) dam = dam / 3;
    if (p_ptr->immune_cold) dam = 1;
    take_hit(dam, kb_str);
    inven_damage(set_cold_destroy, 5);
}



/*
 * Hurt the player with Poison Gas
 */
void poison_gas(int dam, cptr kb_str)
{
    if (p_ptr->oppose_pois > 0) dam = 2 * dam / 3;
    if (p_ptr->resist_pois) dam = (dam * 3) / 5;
    if (p_ptr->immune_pois) dam = 1;
    take_hit(dam, kb_str);
    if (!(p_ptr->resist_pois || p_ptr->oppose_pois
	  || p_ptr->immune_pois))
	p_ptr->poisoned += 12 + randint(dam);
}



/*
 * this assumes only 1 move apart -CFT
 */
static char bolt_char(int y, int x, int ny, int nx)
{
    if (ny == y) return '-';
    if (nx == x) return '|';
    if ((ny-y) == (nx-x)) return '\\';
    return '/';
}


