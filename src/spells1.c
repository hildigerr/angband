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
 * Note that missiles are no longer necessarily all destroyed
 * Destruction taken from "creature.c" code for "stealing".
 * Returns TRUE if anything was damaged.
 */
static int inven_damage(inven_func typ, int perc)
{
    register int index, i, offset;
    register inven_type *i_ptr;
    int		j, k, amt;
    vtype	tmp_str, out_val;

    /* Count the casualties */
    k = 0;

    offset = randint(inven_ctr);
    for (index = 0; index < inven_ctr; index++) {
	i = (index + offset) % inven_ctr; /* be clever and not destroy the first item */

	/* Get the item in that slot */
	i_ptr = &inventory[i];

	/* Give this item slot a shot at death */
	if ((*typ)(i_ptr)) {

	    /* Count the casualties */
	    for (amt = j = 0; j < i_ptr->number; ++j) {
		if (randint(100) < perc) amt++;
	    }

	    /* Some casualities */
	    if (amt) {

		/* Get a description */
		objdes(tmp_str, i_ptr, FALSE);

		/* Message */
		sprintf(out_val, "%sour %s (%c) %s destroyed!",
			((i_ptr->number > 1) ? 
			((amt == i_ptr->number) ? "All of y" :
			 (amt > 1 ? "Some of y" : "One of y")) : "Y"),
			tmp_str, index_to_label(i),
			((amt > 1) ? "were" : "was"));
		msg_print(out_val);

		/* Destroy "amt" items */
		inven_item_increase(i,-amt);
		inven_item_optimize(i);

		/* Count the casualties */
		k += amt;
	    }
	}
    }

    /* Return the casualty count */
    return (k);
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
	objdes(tmp_str, i_ptr, FALSE);
	(void)sprintf(out_val, "Your %s resists damage!", tmp_str);
	msg_print(out_val);
	return (FALSE);
    }

    /* Describe the damage */
    objdes(tmp_str, i_ptr, FALSE);
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
 * We are called from "project()" to "damage" cave grids
 * and the inventory items which may be contained inside them
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * Perhaps we should only SOMETIMES damage things on the ground.
 *
 * The "dist" parameter is the "distance from ground zero". 
 *
 * We must return "TRUE" if the player saw anything "useful" happen.
 */
static bool project_i(int who, int dist, int y, int x, int dam, int typ, int flg)
{

    register cave_type *c_ptr;
    register inven_type *i_ptr;

    int note = 0;

    bool	seen = FALSE;
    bool	plural = FALSE;
    bool	do_kill = FALSE;

    bool	old_floor = FALSE;


    /* XXX Determine if the player can "see" anything happen (set "seen") */
    /* XXX This should take into account: blindness, los, and illumination */

    /* Help determine if the grid is visible to the player */
    if (test_lite(y, x)) seen = TRUE;


    /* Get the grid */    
    c_ptr = &cave[y][x];

    /* Get the object */
    i_ptr = &i_list[c_ptr->i_idx];

    /* Check for "floor" before we function */
    old_floor = (floor_grid_bold(y, x));


    /* Get the "plural"-ness */
    if (i_ptr->number > 1) plural = TRUE;

    /* Affect the object */
    if (c_ptr->i_idx && (flg & PROJECT_ITEM)) {

	/* Analyze the type */        
	switch (typ) {

	    /* Acid -- Lots of things */
	    case GF_ACID:
		if (hates_acid(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Elec -- Rings and Wands */
	    case GF_ELEC:
		if (hates_elec(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Fire -- Flammable objects */
	    case GF_FIRE:
		if (hates_fire(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Cold -- potions and flasks */
	    case GF_COLD:
		if (hates_cold(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Fire + Elec */
	    case GF_PLASMA:
		if (hates_fire(i_ptr)) {
		    do_kill = TRUE;
		}
		if (hates_elec(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Fire + Cold */
	    case GF_METEOR:
		if (hates_fire(i_ptr)) {
		    do_kill = TRUE;
		}
		if (hates_cold(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Hack -- break potions and such */		
	    case GF_ICE:
	    case GF_SHARDS:
	    case GF_FORCE:
	    case GF_SOUND:
		if (hates_cold(i_ptr)) {
		    do_kill = TRUE;
		}
		break;

	    /* Mana -- destroys everything */
	    case GF_MANA:
		do_kill = TRUE;

	    /* Holy Orb -- destroys cursed non-artifacts */
	    case GF_HOLY_ORB:
		if (cursed_p(i_ptr)) {
		    do_kill = TRUE;
		}
		break;
	}


	/* Attempt to destroy the object */
	if (do_kill) {

	    /* Effect "observed" */
	    if (seen) note++;

	    /* Kill it */

		/* Delete the object */
		delete_object(y,x);

		/* Redraw */
		lite_spot(y,x);
	}
    }


    /* Then, affect the grid itself */
    if (flg & PROJECT_GRID) {

	switch (typ) {
	}
    }

    /* Return "Anything seen?" */
    return (note);
}







/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
static bool project_m(int who, int rad, int y, int x, int dam, int typ, int flg)
{
    register int i;

    /* Cave grid */
    register cave_type *c_ptr = &cave[y][x];

    /* Monster info */
    register monster_type *m_ptr = &m_list[c_ptr->m_idx];
    register monster_race *r_ptr = &r_list[m_ptr->r_idx];
    register monster_lore *l_ptr = &l_list[m_ptr->r_idx];


    /* Player blind-ness */
    bool blind = (p_ptr->blind);

    /* Monster visibility */
    bool seen = (!blind && m_ptr->ml);

    /* Were the "effects" obvious (if seen)? */
    bool obvious = TRUE;


    /* Polymorph setting (true or false) */
    int do_poly = 0;


    /* "Damage" factor.  Multiply by "mul/div" */
    int mul = 1, div = 1;

    /* Hold the monster name */
    char m_name[80];

    /* Assume no note */
    cptr note = NULL;

    /* Assume a default death */
    cptr note_dies = " dies.";


    /* Get the monster name (BEFORE polymorphing) */
    sprintf(m_name, (r_ptr->cflags2 & MF2_UNIQUE)? "%s":"The %s",r_ptr->name);



    /* Some monsters are not "living" */
    if ((r_ptr->cflags2 & MF2_DEMON) ||
	(r_ptr->cflags2 & MF2_UNDEAD) ||
	(r_ptr->cflags2 & MF2_MINDLESS) ||
	(strchr("EvgX", r_ptr->r_char))) {

	/* Special note at death */
	note_dies = " is destroyed.";
    }


    /* Hack -- decrease power over distance */
    if (rad) div = rad + 1;

    /* Adjust damage */
    dam = dam * mul / div;


    /* Analyze the damage type */
    switch (typ) {

      /* Magic Missile -- pure damage */
      case GF_MISSILE:
	break;

      /* Acid */
      case GF_ACID:
	if (r_ptr->cflags2 & MF2_IM_ACID) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_ACID;
	}
	break;

      /* Electricity */
      case GF_ELEC:
	if (r_ptr->cflags2 & MF2_IM_ELEC) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_ELEC;
	}
	break;

      /* Fire damage */
      case GF_FIRE:
	if (r_ptr->cflags2 & MF2_IM_FIRE) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_FIRE;
	}
	break;

      /* Cold */
      case GF_COLD:
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_COLD;
	}
	break;

      /* Poison */
      case GF_POIS:
	if (r_ptr->cflags2 & MF2_IM_POIS) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_POIS;
	}
	break;

      /* Holy Orb -- hurts Evil */
      case GF_HOLY_ORB:
	if (r_ptr->cflags2 & MF2_EVIL) {
	    dam *= 2;
	    note = " is hit hard."
	    if (seen) l_ptr->r_cflags2 |= MF2_EVIL;
	}
	break;

      /* Arrow -- XXX no defense */
      case GF_ARROW:
	break;

      /* Plasma -- XXX perhaps check ELEC or FIRE */
      case GF_PLASMA:
	if (!strncmp("Plasma", r_ptr->name, 6) ||
	    (r_ptr->spells3 & MS3_BR_PLAS)) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Nether -- see above */
      case GF_NETHER:
	if (r_ptr->cflags2 & MF2_UNDEAD) {
	    note = " is immune.";
	    dam = 0;
	    if (seen) l_ptr->r_cflags2 |= MF2_UNDEAD;
	}
	else if (r_ptr->spells2 & MS2_BR_LIFE) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	else if (r_ptr->cflags2 & MF2_EVIL) {
	    dam /= 2;
	    note = " resists somewhat."
	    if (seen) l_ptr->r_cflags2 |= MF2_EVIL;
	}
	break;

      /* Water (acid) damage -- Water spirits/elementals and "Waldern" are immune */
      case GF_WATER:
	if ((r_ptr->r_char == 'E') && (r_ptr->name[0] == 'W')) {
	    note = " is immune.";
	    dam = 0;
	}
	break;

      /* Chaos -- Chaos breathers resist */
      case GF_CHAOS:
	do_poly = TRUE;
	if (r_ptr->spells2 & MS2_BR_CHAO) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	    do_poly = FALSE;
	}
	break;

      /* Shards -- Shard breathers resist */
      case GF_SHARDS:
	if (r_ptr->spells2 & MS2_BR_SHAR) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Sound -- Sound breathers resist */
      case GF_SOUND:
	if (r_ptr->spells2 & MS2_BR_SOUN) {
	    note = " resists.";
	    dam *= 2; dam /= (randint(6)+6);
	}
	break;

      /* Confusion */
      case GF_CONFUSION:
	if (r_ptr->spells2 & MS2_BR_CONF) { 
	    note = " resists.";
	    dam *= 2; dam /= (randint(6)+6);
	}
	else if (r_ptr->cflags2 & MF2_CHARM_SLEEP) {
	    note = " resists somewhat.";
	    dam /= 2;
	}
	break;

      /* Disenchantment -- Breathers and Disenchanters resist */
      case GF_DISENCHANT:
	if ((r_ptr->spells2 & MS2_BR_DISE) ||
	    !strncmp("Disen", r_ptr->name, 5)) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Nexus -- Breathers and Existers resist */
      case GF_NEXUS:
	if ((r_ptr->spells2 & MS2_BR_NETH) ||
	    !strncmp("Nexus", r_ptr->name, 5)) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Force */
      case GF_FORCE:
	if (r_ptr->spells3 & MS3_BR_WALL) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Inertia -- breathers resist */
      case GF_INERTIA:
	if (r_ptr->spells3 & MS3_BR_SLOW) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}

      /* Lite -- opposite of Dark */
      case GF_LITE:
	if (r_ptr->spells3 & MS3_BR_LITE) {
	    note = " resists.";
	    dam *= 2; dam /= (randint(6)+6);
	}
	else if (r_ptr->cflags2 & MF2_HURT_LITE) {
	    note = " is hit hard.";
	    dam *= 2;
	}
	else if (r_ptr->spells3 & MS3_BR_DARK) {
	    note = " is hit hard.";
	    dam = dam * 3 / 2;
	}
	break;

      /* Dark -- opposite of Lite */
      case GF_DARK:
	if (r_ptr->spells2 & MS3_BR_DARK) {
	    note = " resists.";
	    dam *= 2; dam /= (randint(6)+6);
	}
	else if (r_ptr->cflags2 & MF2_HURT_LITE) {
	    note = " resists somewhat.";
	    dam /= 2;
	}
	else if (r_ptr->spells3 & MS3_BR_LITE) {
	    note = " is hit hard.";
	    dam = dam * 3 / 2;
	}
	break;

      /* Time -- breathers resist */
      case GF_TIME:
	if (r_ptr->spells3 & MS3_BR_TIME) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Gravity -- breathers resist */
      case GF_GRAVITY:
	if (r_ptr->spells3 & MS3_BR_GRAV) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	}
	break;

      /* Pure damage */
      case GF_MANA:
	break;

      /* Meteor -- powerful magic missile */
      case GF_METEOR:
	break;

      /* Ice -- Cold + Cuts + Stun */
      case GF_ICE:
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_COLD;
	}
	break;
    }




    /* "Unique" monsters cannot be polymorphed */
    if (r_ptr->cflags2 & MF2_UNIQUE) do_poly = FALSE;


    /* Check for death */
    if (dam > m_ptr->hp) {

	/* Extract method of death */
	note = note_dies;
    }

    /* Handle "polymorph" -- monsters get a saving throw */
    else if (do_poly && (randint(90) > r_ptr->level)) {

	/* Pick a "new" monster race */
	i = poly_r_idx(m_ptr->r_idx);

	/* Handle polymorh */
	if (i != m_ptr->r_idx) {

	    /* Monster polymorphs */
	    note = " changes!";

	    /* Turn off the damage */
	    dam = 0;

	    /* "Kill" the "old" monster */
	    delete_monster_idx(c_ptr->m_idx);

	    /* Place the new monster where the old one was */
	    place_monster(y, x, i, FALSE);

	    /* Get new monster */
	    m_ptr = &m_list[cave[y][x].m_idx];
	    r_ptr = &r_list[m_ptr->r_idx];
	    l_ptr = &l_list[m_ptr->r_idx];
	}
    }




    /* Give detailed messages if visible or destroyed */
    if (note && (seen || (dam > m_ptr->hp))) {
	msg_print(m_name);
	msg_print(note);
    }	

	/* Hurt the monster, display fear msg's */
	if (mon_take_hit(c_ptr->m_idx, dam, TRUE)) {

	    /* Give experience if killed */
	    prt_experience();
	}

    /* "Fix" the monster, and redraw him (or erase him) */
    update_mon(c_ptr->m_idx);


    /* Return TRUE if the player saw anything */
    return (seen && obvious);
}






/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_m()" above.
 *
 * Although unused, we return "TRUE" if any "useful" effects were observed.
 *
 */
static bool project_p(int who, int rad, int y, int x, int dam, int typ, int flg)
{

    /* Source monster */
    register monster_type *m_ptr;


    /* Get the source monster */
    m_ptr = &m_list[who];


    /* Analyze the damage */
    switch (typ) {

	/* Standard damage -- hurts inventory too */
	case GF_ACID:
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_FIRE:
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_COLD:
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_ELEC:
	    break;

	/* Hold Orb -- Player only takes partial damage, cause he is "good" */
	case GF_HOLY_ORB:
	    break;

	/* Plasma -- XXX No resist */
	case GF_PLASMA:
	    break;

	/* Shards -- mostly cutting */
	case GF_SHARDS:
	    break;

	/* Sound -- mostly stunning */
	case GF_SOUND:
	    break;

	/* Force -- mostly stun */
	case GF_FORCE:
	    break;

	/* Pure damage */
	case GF_MANA:
	    break;

	/* Pure damage */
	case GF_METEOR:
	    break;

	/* Ice -- cold plus stun plus cuts */
	case GF_ICE:

	default:
	    msg_print("Oops.  Undefined beam/bolt/ball hit player.");
    }


    /* Disturb */
    disturb(1, 0);


    /* Hack -- Assume something happened */
    return (TRUE);
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



/*
 * Generic "beam"/"bolt"/"ball" projection routine.  -BEN-
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind towards
 * a given location (optionally passing over the heads of interposing monsters),
 * and have it do a given amount of damage to the monsters (and optionally objects)
 * within the given radius of the final location.
 *
 * A "bolt" travels from the source to target and affects only the target grid.
 * A "beam" travels from the source to target, affecting all grids passed through.
 * A "ball" travels from the source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does not
 * pass over the heads of interposing monsters, much like a traditional missile,
 * and will "stop" abruptly at the "target" even if no monster is positioned there.
 * A "ball", on the other hand, traditionally passes over the heads of monsters
 * between the source and target, and affects everything except the source monster
 * which lies within the final radius.  Traditionally, a "beam" affects every
 * monster between the source and target, except for the casting monster (or player),
 * and only affects things on the ground in special cases (light, disarm, walls).
 *
 * Returns TRUE if the player saw anything "useful" happen.
 *
 * Input:
 *   who: Index of "source" monster (one for "player")
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *   typ: Type of damage to apply to monsters (and objects) 
 *   flg: Extra flags, see below
 *
 * The available "flags" are described where "PROJECT_xxxx" are defined
 *
 * Only 256 grids can be affected per projection, limiting the effective
 * "radius" of standard ball attacks to nine units (diameter nineteen).
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets (like those from mmove()) to the initial location (see "line_spell()").
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits somethings (useful for "stone to mud").
 *
 * When targetting an actual monster, be sure to verify visibility (and perhaps
 * reachability) of the monster by the player, even if PROJECT_THRU is off, or
 * the player will be able to "seek" for invisible/teleported monsters.
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *
 * Balls must explode BEFORE hitting walls, or they would "pass through" walls.
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 * The algorithm is not necessarily the most efficient that one could write.
 * Walls and doors are included in the blast area, so that they can be "burned".
 * Permanent rock is NEVER included in the blast area, nor are undefined locations.
 *
 * Objects in the blast area when the blast occurs are (potentially) destroyed,
 * even if they are "under" monsters.  But objects dropped by monsters
 * who are destroyed by the blast are "shielded" by the monsters corpse.
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *
 * The array "gy[],gx[]" with "current" size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *
 * Note the rather complex usage of the "gm[]" array.
 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (OBJ_BOLT_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *
 * It is rather important that the grids are processed from ground-zero outward.
 *
 * We attempt to return "true" if any "effects" of the projection were observed.
 */
bool project(int who, int rad, int y, int x, int dam, int typ, int flg)
{
    int			i, t;
    int                 y1, x1, y2, x2;
    int			y0, x0, y9, x9;
    int			dist;

    /* Affected location(s) */
    register cave_type *c_ptr;

    /* Assume the player sees nothing */
    bool notice = FALSE;

    /* Is the player blind? */    
    int blind = FALSE;

    /* Number of "blast grids" visible to the player */
    int drawn = 0;

    /* Number of grids in the "blast area" (including the "beam" path) */
    int grids = 0;

    /* Coordinates of the affected grids */
    byte gx[256], gy[256];

    /* Encoded "radius" info (see above) */
    byte gm[16];


    /* The source is a monster */
    if (who > 1) {
	x1 = m_list[who].fx;
	y1 = m_list[who].fy;
    }

    /* The source is a player */
    else {
	x1 = char_col;
	y1 = char_row;
    }


    /* Location of player */
    y0 = char_row;
    x0 = char_col;

    /* Check player blind-ness */
    if (p_ptr->blind) blind = TRUE;


    /* Hack -- Assume there will be no blast (max radius 16) */
    for (dist = 0; dist < 16; dist++) gm[dist] = 0;


    /* Default "destination" */
    y2 = y; x2 = x;

    /* XXX Apply "offset" mode */

    /* Apply "travel through" mode by "sliding" target location */
    if (flg & PROJECT_THRU) {
	y2 = y1 + 99 * (y2 - y1);
	x2 = x1 + 99 * (x2 - x1);
    }


    /* Start at the source */
    x = x9 = x1;
    y = y9 = y1;
    dist = 0;

    /* Project until done */
    while (1) {

	/* Gather beam grids */
	if (flg & PROJECT_BEAM) {
	    gy[grids] = y;
	    gx[grids] = x;
	    grids++;
	}

	/* XXX XXX Hack -- Display "beam" grids */
	if (!blind && !(flg & PROJECT_HIDE) &&
	    (dist > 0) && (flg & PROJECT_BEAM) &&
	    panel_contains(y, x) && los(char_row, char_col, y, x)) {

	    /* Hack -- Visual effect -- "explode" the grids */
#ifdef TC_COLOR
	    if (!no_color_flag) textcolor(bolt_color(typ));
#endif
	    print('*', y, x);
#ifdef TC_COLOR
	    if (!no_color_flag) textcolor(LIGHTGRAY);
#endif
	}

	/* Check the grid */
	c_ptr = &cave[y][x];

	/* Never pass BEYOND a wall or door */
	if (!floor_grid_bold(y, x)) break;

	/* Check for arrival at "final target" */
	if ((x == x2) && (y == y2)) break;

	/* If allowed, and we have moved at all, stop when we hit anybody */
	if ((c_ptr->m_idx > 0) && (dist > 0) && (flg & PROJECT_STOP)) break;


	/* Calculate the new location */
	y9 = y;
	x9 = x;
	(void)mmove2(&y9, &x9, y1, x1, y2, x2);

	/* Hack -- Balls explode BEFORE reaching walls or doors */
	if (!floor_grid_bold(y9, x9) && (rad > 0)) break;

	/* Keep track of the distance traveled */
	dist++;

	/* Nothing can travel furthur than the maximal distance */
	if (dist > OBJ_BOLT_RANGE) break;

	/* Hack -- Visual effects -- Display, Highlight, Flush, Pause, Erase */
	/* Note that we consider the "bolt" to be "self illuminating" */
	if (!blind && !(flg & PROJECT_HIDE) &&
	    panel_contains(y9, x9) && los(char_row, char_col, y9, x9)) {

#ifdef TC_COLOR
	    if (!no_color_flag) textcolor(bolt_color(typ));
#endif
	    print(bolt_char(y, x, y9, x9), y9, x9);
#ifdef TC_COLOR
	    if (!no_color_flag) textcolor(LIGHTGRAY);
#endif
	    move_cursor_relative(y9, x9);
	    put_qio();
#ifdef MSDOS
	    delay(8 * delay_spd);
#else
	    usleep(8000 * delay_spd);
#endif
	    lite_spot(y9, x9);
	}

	/* Save the new location */
	y = y9;
	x = x9;        
    }


    /* Save the "blast epicenter" */
    y2 = y;
    x2 = x;

    /* Start the "explosion" */
    gm[0] = 0;

    /* Hack -- make sure beams get to "explode" */
    gm[1] = grids;

    /* If we found a "target", explode there */
    if (dist <= OBJ_BOLT_RANGE) {

	/* Hack -- remove the final "beam" grid */
	if ((flg & PROJECT_BEAM) && (grids > 0)) grids--;

	/* Determine the blast area, work from the inside out */
	for (dist = 0; dist <= rad; dist++) {

	    /* Scan the maximal blast area of radius "dist" */
	    for (y = y2 - dist; y <= y2 + dist; y++) {
		for (x = x2 - dist; x <= x2 + dist; x++) {

		    /* Note that we DO add perma-rock to the blast */
		    if (!in_bounds2(y, x)) continue;

		    /* Enforce a "circular" explosion */
		    if (distance(y2, x2, y, x) != dist) continue;

		    /* Ball explosions are stopped by walls */
		    if (!los(y2, x2, y, x)) continue;

		    /* Save this grid */        
		    gy[grids] = y;
		    gx[grids] = x;
		    grids++;
		}
	    }

	    /* Encode some more "radius" info */
	    gm[dist+1] = grids;        
	}
    }


    /* Speed -- ignore "non-explosions" */
    if (!grids) return (FALSE);


    /* Scan the "blast area" for visible locations, and display them */
    if (!blind && !(flg & PROJECT_HIDE)) {

	/* Then do the "blast", from inside out */
	for (t = 0; t <= rad; t++) {

	    /* Dump everything with this radius */
	    for (i = gm[t]; i < gm[t+1]; i++) {        
		if (panel_contains(gy[i], gx[i])) {
		    drawn++;
#ifdef TC_COLOR
		    if (!no_color_flag) textcolor(bolt_color(typ));
#endif
		    print('*', gy[i], gx[i]);
#ifdef TC_COLOR
		    if (!no_color_flag) textcolor(LIGHTGRAY);
#endif
		}
	    }

	    /* Flush each "radius" seperately */
	    /* Note that the cursor may go offscreen */
	    if (gm[t+1] > gm[t]) {
		move_cursor_relative(y2, x2);
		put_qio();
#ifdef MSDOS
		delay(10 * delay_spd);
#else
		usleep(10000 * delay_spd);
#endif
	    }
	}

	/* Erase the explosion drawn above */
	for (i = 0; i < grids; i++) {
	    if (panel_contains(gy[i], gx[i])) {
		lite_spot(gy[i], gx[i]);
	    }
	}

	/* Flush the erasing */
	if (drawn) {
	    move_cursor_relative(y2, x2);
	    put_qio();
	}
    }


    /* Start with "dist" of zero */
    dist = 0;

    /* Now hurt the cave grids (and objects) from the inside out */
    for (i = 0; i < grids; i++) {

	/* Hack -- Notice new "dist" values */
	if (gm[dist+1] == i) dist++;

	/* Get the grid location */        
	y = gy[i];
	x = gx[i];

	/* Allow the beam/ball to "damage" the grid itself */
	if (project_i(who, dist, y, x, dam, typ, flg)) notice = TRUE;
    }


    /* Now hurt the monsters, from inside out */
    for (i = 0; i < grids; i++) {

	/* Hack -- Notice new "dist" values */
	if (gm[dist+1] == i) dist++;

	/* Get the grid location */
	y = gy[i];
	x = gx[i];

	/* Walls protect monsters */
	if (!floor_grid_bold(y,x)) continue;

	/* Get the cave grid */
	c_ptr = &cave[y][x];

	/* Affect real monsters (excluding the caster) */
	if ((c_ptr->m_idx > 1) && (c_ptr->m_idx != who)) {

	    /* XXX Perhaps hilite the monster in some way */
	    
	    /* Damage the monster */
	    if (project_m(who, dist, y, x, dam, typ, flg)) notice = TRUE;
	}
    }


    /* Start with "dist" of zero */
    dist = 0;

    /* Now see if the player gets hurt */
    for (i = 0; i < grids; i++) {

	/* Hack -- Notice new "dist" values */
	if (gm[dist+1] == i) dist++;

	/* Get the grid location */
	y = gy[i];
	x = gx[i];

	/* Hack -- Walls protect the player (never happens) */
	if (!floor_grid_bold(y,x)) continue;

	/* Get the cave grid */
	c_ptr = &cave[y][x];

	/* The player is here */
	if ((c_ptr->m_idx == 1) && (c_ptr->m_idx != who)) {

	    /* Damage the player */
	    if (project_p(who, dist, y, x, dam, typ, flg)) notice = TRUE;
	}
    }


    /* Return "something was noticed" */
    return (notice);
}


