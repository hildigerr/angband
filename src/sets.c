/*
 * sets.c: code to emulate the original Pascal sets 
 *
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


int set_corrodes(inven_type *e)			   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_SWORD:
      case TV_HELM:
      case TV_SHIELD:
      case TV_HARD_ARMOR:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_ACID)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_ACID))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_WAND:
	return (TRUE);
    }
    return (FALSE);
}


int set_flammable(inven_type *e)		   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_FIRE)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_FIRE))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_STAFF:
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_FLASK:
	return (TRUE);
      case TV_LITE:
	if (e->sval >= 192)	   /* only torches... -CFT */
	    return (TRUE);
	else
	    return (FALSE);
    }
    return (FALSE);
}


int set_frost_destroy(inven_type *e)		   /* changed -CFT */
{
    int element = e->tval;

    if ((element == TV_POTION1) || (element == TV_POTION2)
	|| (element == TV_FLASK))
	return (TRUE);
    return (FALSE);
}


int set_meteor_destroy(inven_type *e)		   /* added -DGK */
{
    byte fi, fo;

    fi = set_fire_destroy(e);
    fo = set_frost_destroy(e);
    return (fi | fo);
}


int set_mana_destroy(inven_type *e)		   /* added -DGK */
{				   /* destroy everything but artifacts */
    int element = e->tval;

    if ((element >= TV_MIN_ENCHANT) && (element <= TV_MAX_WEAR) &&
	artifact_p(e))
	return (FALSE);
    return (TRUE);
}


int set_holy_destroy(inven_type *e)		   /* added -DGK */
{
    int element = e->tval;

    if ((element >= TV_MIN_ENCHANT) && (element <= TV_MAX_WEAR) &&
	(e->flags3 & TR3_CURSED) && (!artifact_p(e)))
	return (TRUE);
    return (FALSE);
}


int set_plasma_destroy(inven_type *e)		   /* added -DGK */
{
    byte fi, li;

    fi = set_fire_destroy(e);
    li = set_lightning_destroy(e);
    return (fi | li);
}


int set_acid_affect(inven_type *e)		   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_BOLT:
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_ACID)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_ACID))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_MISC:
      case TV_CHEST:
	return (TRUE);
    }
    return (FALSE);
}


int set_lightning_destroy(inven_type *e)	   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_RING:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_ELEC)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_ELEC))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_WAND:
	return (TRUE);
    }
    return (FALSE);
}


int set_null(inven_type *e)
{
    return (FALSE);
}


int set_acid_destroy(inven_type *e)		   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_HELM:
      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_ACID)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_ACID))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_FOOD:
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
      case TV_STAFF:
	return (TRUE);
    }
    return (FALSE);
}


int set_fire_destroy(inven_type *e)		   /* changed -CFT */
{
    int element = e->tval;

    switch (element) {
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_SOFT_ARMOR:
	if (artifact_p(e)	/* shouldn't kill artifacts -CFT */
	    ||(e->flags2 & TR2_RES_FIRE)	/* can't check outside, because flags1 */
	    ||(e->flags2 & TR2_IM_FIRE))	/* used differently in potions/etc */
	    return (FALSE);
	return (TRUE);
      case TV_STAFF:
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_POTION1:
      case TV_POTION2:
      case TV_FLASK:
      case TV_FOOD:
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
	return (TRUE);
      case TV_LITE:
	if (e->sval >= 192)	   /* only torches... -CFT */
	    return (TRUE);
	else
	    return (FALSE);
    }
    return (FALSE);
}


