/* File: project.c */

/* Purpose: generic bolt/ball/beam code	(-BEN-) */

#include "angband.h"




/*
 * Apply disenchantment to the player's stuff
 *
 * The "mode" is currently unused.
 *
 * Return "TRUE" if the player notices anything
 */
bool apply_disenchant(int mode)
{
    bool disenchant = FALSE:
    int                t = 0;
    inven_type         *i_ptr;
    vtype               t1, t2;


    /* Pick a random slot */
    switch (randint(7)) {
	 case 1: t = INVEN_WIELD; break;
	 case 2: t = INVEN_BODY; break;
	 case 3: t = INVEN_ARM; break;
	 case 4: t = INVEN_OUTER; break;
	 case 5: t = INVEN_HANDS; break;
	 case 6: t = INVEN_HEAD; break;
	 case 7: t = INVEN_FEET; break;
    }

    /* Get the item */                                
    i_ptr = &inventory[t];

    /* No item, nothing happens */
    if (i_ptr->tval == TV_NOTHING) return (FALSE);


    /* Nothing to disenchant */
    if ((i_ptr->tohit <= 0) && (i_ptr->todam <= 0) && (i_ptr->toac <= 0)) {

	/* Nothing to notice */
	return (FALSE);
    }


    /* Describe the object */
    objdes(t1, i_ptr, FALSE);


    /* Artifacts have 2/3 chance to resist */
    if (artifact_p(i_ptr) && (randint(3) != 1)) {

	/* Message */
	sprintf(t2, "Your %s (%c) %s disenchantment!",
		t1, index_to_label(t),
		(i_ptr->number != 1) ? "resist" : "resists");
	msg_print(t2);

	/* Notice */
	return (TRUE);
    }


    /* Disenchant tohit */
    if (i_ptr->tohit > 0) {
    i_ptr->tohit -= randint(2);
    if (i_ptr->tohit < 0) i_ptr->tohit = 0;
    disenchant = TRUE;
    }

    /* Disenchant todam */
    if (i_ptr->todam > 0) {
    i_ptr->todam -= randint(2);
    if (i_ptr->todam < 0) i_ptr->todam = 0;
    disenchant = TRUE;
    }

    /* Disenchant toac */
    if (i_ptr->toac > 0) {
    i_ptr->toac -= randint(2);
    if (i_ptr->toac < 0) i_ptr->toac = 0;
    disenchant = TRUE;
    }


    if( disenchant ) {
    sprintf(t2, "Your %s (%c) %s disenchanted!",
	    t1, index_to_label(t),
	    (i_ptr->number != 1) ? "were" : "was");
    msg_print(t2);

    /* Recalculate bonuses */
    calc_bonuses();
    }

    /* Notice */
    return (TRUE);
}


/*
 * Apply Nexus
 */
static void apply_nexus(monster_type *m_ptr)
{
    int k = dun_level;
    int max1, cur1, max2, cur2, ii, jj;

    switch (randint(7)) {

	case 1: case 2: case 3:

	    teleport(200);
	    break;

	case 4: case 5:

	    teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	    break;

	case 6:

	    if (player_saves()) {
		msg_print("You resist the effects.");
		break;
	    }            

	    /* Teleport Level */
	    if (dun_level == Q_PLANE) dun_level = 0;
	    else if (is_quest(dun_level)) dun_level -= 1;
	    else dun_level += (-3) + 2 * randint(2);
	    if (dun_level < 0) dun_level = 0;
	    if (k == Q_PLANE) msg_print("You warp through a cross-dimension gate.");
	    else if (k < dun_level) msg_print("You sink through the floor.");
	    else msg_print("You rise up through the ceiling.");
	    new_level_flag = TRUE;
	    break;

	case 7:

	    if (player_saves() && randint(2) == 1) {
		msg_print("You resist the effects.");
		break;
	    }

	    msg_print("Your body starts to scramble...");

	    /* Pick a pair of stats */
	    ii = rand_int(6);
	    for (jj = ii; jj == ii; jj = rand_int(6));

	    max1 = p_ptr->max_stat[ii];
	    cur1 = p_ptr->cur_stat[ii];
	    max2 = p_ptr->max_stat[jj];
	    cur2 = p_ptr->cur_stat[jj];

	    p_ptr->max_stat[ii] = max2;
	    p_ptr->cur_stat[ii] = cur2;
	    p_ptr->max_stat[jj] = max1;
	    p_ptr->cur_stat[jj] = cur1;

	    set_use_stat(ii);
	    set_use_stat(jj);
	    prt_stat(ii);
	    prt_stat(jj);

	    break;
    }
}




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
 * The "dist" parameter is the "distance from ground zero".  Some
 * projections (such as "GF_LITE") have special effects at "ground
 * zero" (like "lite_a_dark_room()").  Note that ground zero is always
 * first of all the grids to be affected, unless the weapon was a beam.
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

	    /* Destroy Traps (and Locks) */
	    case GF_KILL_TRAP:

		/* Destroy traps */
		if ((i_ptr->tval == TV_INVIS_TRAP) ||
		    (i_ptr->tval == TV_VIS_TRAP)) {

		    /* Destroy it */
		    do_kill = TRUE;
		}

		/* Chests are noticed only if trapped or locked */
		else if (i_ptr->tval == TV_CHEST) {
		    if (i_ptr->flags2) {
			i_ptr->flags2 = 0L;
			i_ptr->flags2 |= CH2_DISARMED;
			known2(i_ptr);
			if (seen) {
			    msg_print("Click!");
			    note++;
			}
		    }
		}

		/* Doors are unlocked (without being seen) */
		else if (i_ptr->tval == TV_CLOSED_DOOR) {
		    i_ptr->pval = 0;
		}

		/* Secret doors are found and unlocked, and seen if visible */
		else if (i_ptr->tval == TV_SECRET_DOOR) {

		    /* Hack -- make a closed door */
		    invcopy(i_ptr, OBJ_CLOSED_DOOR);

		    /* Place it in the dungeon */
		    i_ptr->iy = y;
		    i_ptr->ix = x;

		    /* Hack -- if seen, notice and memorize */
		    if (seen) note++;

		    /* Redraw */
		    lite_spot(y, x);
		}

		break;

	    /* Destroy Doors (and traps) */
	    case GF_KILL_DOOR:	    

		if (i_ptr->tval == TV_CHEST) {
		    i_ptr->flags2 = 0L;
		    i_ptr->flags2 |= CH2_DISARMED;
		    if (seen) {
			note++;
			msg_print("You have disarmed the chest.");
		    }
		    break;
		}

		/* Explode doors/traps */
		if ((i_ptr->tval == TV_INVIS_TRAP) ||
		    (i_ptr->tval == TV_VIS_TRAP) ||
		    (i_ptr->tval == TV_OPEN_DOOR) ||
		    (i_ptr->tval == TV_CLOSED_DOOR) ||
		    (i_ptr->tval == TV_SECRET_DOOR)) {

		    /* Destroy it */
		    do_kill = TRUE;

		    /* Hack -- special message */
		    if (seen) msg_print("There is a bright flash of light!");
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

	    /* Lite up the grid */
	    case GF_LITE_WEAK:

		/* If the grid is visible, notice it */
		if (seen) note++;

		/* Ground zero -- lite the room */
		if (!dist)
		if (c_ptr->fval == LIGHT_FLOOR) light_room(y, x);

		/* Turn on the light */
		c_ptr->pl = TRUE;

		/* Draw (and perhaps memorize) the grid */
		lite_spot(y, x);

		break;

	    /* Darken the grid */            
	    case GF_DARK_WEAK:

		/* Notice */
		if (seen) note++;

		/* Darken the room. */
		if (!dist) darken_room(y, x);

		/* Turn off the light. */
		c_ptr->pl = FALSE;
		c_ptr->tl = FALSE;

		/* All done */
		break;
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

    /* Teleport setting (max distance) */
    int do_dist = 0;

    /* Confusion setting (amount to confuse) */
    int do_conf = 0;

    /* Stunning setting (amount to stun) */
    int do_stun = 0;

    /* Sleep amount (amount to sleep) */
    int do_sleep = 0;


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
	do_conf = (5 + randint(11)) * mul / div;
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
	do_stun = (10 + randint(15)) * mul / div;
	if (r_ptr->spells2 & MS2_BR_SOUN) {
	    note = " resists.";
	    dam *= 2; dam /= (randint(6)+6);
	}
	break;

      /* Confusion */
      case GF_CONFUSION:
	do_conf = (5 + randint(11)) * mul / div;
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
	do_stun = randint(15) * mul / div;
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

      /* Lite, but only hurts susceptible creatures */
      case GF_LITE_WEAK:
	m_ptr->csleep = 0;
	if (r_ptr->cflags2 & MF2_HURT_LITE) {
	    if (seen) l_ptr->r_cflags2 |= MF2_HURT_LITE;
	    note = " cringes from the light!";
	    note_dies = " shrivels away in the light!";
	}
	else {
	    obvious = FALSE;
	    dam = 0;
	}
	break;

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
	do_dist = 5;
	if (r_ptr->spells3 & MS3_BR_GRAV) {
	    note = " resists.";
	    dam *= 3; dam /= (randint(6)+6);
	    do_dist = 0;
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
	do_stun = randint(15) * mul / div;
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    note = " resists.";
	    dam /= 9;
	    if (seen) l_ptr->r_cflags2 |= MF2_IM_COLD;
	}
	break;


      /* Drain Life */
      case GF_OLD_DRAIN:
	if ((r_ptr->cflags2 & MF2_UNDEAD) ||
	    (r_ptr->cflags2 & MF2_DEMON) ||
	    (strchr("Egv", r_ptr->r_char))) {

	    if (r_ptr->cflags2 & MF2_UNDEAD) l_ptr->r_cflags2 |= MF2_UNDEAD;
	    if (r_ptr->cflags2 & MF2_DEMON) l_ptr->r_cflags2 |= MF2_DEMON;
	    obvious = FALSE;
	    dam = 0;
	}
	else note_dies = " dies in a fit of agony.";
	break;

      /* Polymorph monster (Use "dam" as "power") */	
      case GF_OLD_POLY:

	/* Attempt to polymorph (see below) */
	do_poly = TRUE;

	/* Powerful monsters can resist */
	if ((r_ptr->cflags2 & MF2_UNIQUE) ||
	    (r_ptr->level > 10 + randint((dam - 10) < 1 ? 1 : (dam - 10)))) {
	    do_poly = FALSE;
	    note = " is unaffected.";
	}

	/* No "real" damage */
	dam = 0;	

	break;


      /* Sleep (Use "dam" as "power") */
      case GF_OLD_SLEEP:

	/* Attempt a saving throw */
	if ((r_ptr->cflags2 & MF2_UNIQUE) ||
	    (r_ptr->cflags2 & MF2_CHARM_SLEEP) ||
	    (r_ptr->level > 10 + randint((dam - 10) < 1 ? 1 : (dam - 10)) + 10)) {

	    /* Hack -- memorize a flag (does it DO anything?) */
	    if (seen && (r_ptr->cflags2 & MF2_CHARM_SLEEP)) {
		l_ptr->r_cflags2 |= MF2_CHARM_SLEEP;
	    }
	    note = " is unaffected.";
	}
	else {

	    /* Go to sleep (much) later */
	    note = " falls asleep!";
	    do_sleep = 500;
	}

	/* No "real" damage */
	dam = 0;
	break;


      /* Confusion (Use "dam" as "power") */
      case GF_OLD_CONF:

	/* Get confused later */
	do_conf = damroll(3, (dam / 2)) + 1;

	/* Attempt a saving throw */
	if ((r_ptr->cflags2 & MF2_UNIQUE) ||
	    (r_ptr->cflags2 & (MS2_BR_CONF | MS2_BR_CHAO) ||
	    (r_ptr->level > 10 + randint((dam - 10) < 1 ? 1 : (dam - 10)) + 10)) {

	    /* Hack -- memorize a flag (does it DO anything?) */
	    if (seen && (r_ptr->cflags2 & MF2_CHARM_SLEEP)) {
		l_ptr->r_cflags2 |= MF2_CHARM_SLEEP;
	    }

	    /* Resist */
	    do_conf = 0;

	    note = " is unaffected.";
	}

	/* Wake the monster up */
	else m_ptr->csleep = 0;

	/* No "real" damage */
	dam = 0;
	break;


      /* Confusion (Use "dam" as "power") */
      case GF_OLD_SCARE:

	/* Attempt a saving throw */
	if ((r_ptr->cflags2 & MF2_UNIQUE) ||
	    (r_ptr->level > 10 + randint((dam - 10) < 1 ? 1 : (dam - 10)) + 10)) {

	    /* Hack -- memorize a flag (does it DO anything?) */
	    if (seen && (r_ptr->cflags2 & MF2_CHARM_SLEEP)) {
		l_ptr->r_cflags2 |= MF2_CHARM_SLEEP;
	    }

	    note = " is unaffected.";
	}

	/* Get scared */
	else {

	    /* Don't overflow */
	    if (m_ptr->monfear < 175) {
		m_ptr->monfear += (byte)(damroll(3, (dam / 2)) + 1);
	    }

	    /* Message */
	    note = " flees in terror!";
	}

	/* No "real" damage */
	dam = 0;
	break;




      /* Teleport monster (Use "dam" as "power") */
      case GF_OLD_TPORT:

	/* Wake the monster up */
	m_ptr->csleep = 0;

	/* Prepare to teleport */      	
	do_dist = dam;

	/* No "real" damage */
	dam = 0;
	break;
    }




    /* "Unique" monsters cannot be polymorphed */
    if (r_ptr->cflags2 & MF2_UNIQUE) do_poly = FALSE;


    /* "Unique" monsters can only be "killed" by the player */
    if (r_ptr->cflags2 & MF2_UNIQUE) {

	/* Uniques may only be killed by the player */
	if ((who > 1) && (dam > m_ptr->hp)) dam = m_ptr->hp;
    }


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

    /* Handle "teleport" */
    else if (do_dist) {

	/* Teleport */
	teleport_away(c_ptr->m_idx, do_dist);

	/* Re-extract location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Re-extract cave */
	c_ptr = &cave[y][x];
    }

    /* Sound and Impact breathers never stun */
    else if (do_stun &&
	     !(r_ptr->spells2 & MS2_BR_SOUN) &&
	     !(r_ptr->spells3 & MS3_BR_WALL)) {
	if (m_ptr->confused > 0) { 
	    note = " is more dazed.";
	    if (m_ptr->confused < 220) {
		m_ptr->confused += (1 + randint(5) * 2 / div);
	    }
	}
	else {
	    note = " is dazed.";
	    m_ptr->confused = do_stun;
	}
    }

    /* Confusion and Chaos breathers (and sleepers) never confuse */
    else if (do_conf &&
	    !(r_ptr->cflags2 & MF2_CHARM_SLEEP) &&
	    !(r_ptr->spells2 & MS2_BR_CONF) &&
	    !(r_ptr->spells2 & MS2_BR_CHAO)) {

	/* Already partially confused */
	if (m_ptr->confused > 0) { 
	    note = " is more confused.";
	    if (m_ptr->confused < 240) {
		m_ptr->confused += (7 * mul / div);
	    }
	}

	/* Was not confused */
	else {
	    note = " appears confused."
	    m_ptr->confused = do_conf;
	}
    }



    /* Give detailed messages if visible or destroyed */
    if (note && (seen || (dam > m_ptr->hp))) {
	msg_print(m_name);
	msg_print(note);
    }	

    /* Hack -- Pain message */
    else if ((dam > 0) && (dam <= m_ptr->hp)) {
	char out_val[160];
	(void)sprintf(out_val, pain_message(c_ptr->m_idx, dam), m_name);
	message(out_val, 0x01);
    }


    /* Hack -- sleep is done INSTEAD of damage */
    if (do_sleep) {

	/* Just set the "sleep" field */
	m_ptr->csleep = do_sleep;
    }

    /* If another monster did the damage, hurt the monster by hand */
    else if (who > 1) {

	/* Paranoia -- No negative damage */
	if (dam < 0) dam = 0;

	/* Wake the monster up */
	m_ptr->csleep = 0;

	/* Hurt the monster */
	m_ptr->hp -= dam;

	/* Dead monster */
	if (m_ptr->hp < 0) {

 	    /* Generate treasure (Hack -- handle creeping coins) */
	    coin_type = 0;
	    get_coin_type(r_ptr);
	    monster_death(m_ptr);
	    coin_type = 0;

	    /* Delete the monster */
	    delete_monster_idx(c_ptr->m_idx);
	}
    }

    /* If the player did it, give him experience */
    else {

	/* Hurt the monster, display fear msg's */
	if (mon_take_hit(c_ptr->m_idx, dam, TRUE)) {

	    /* Give experience if killed */
	    prt_experience();
	}
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
 * If the source monster is "breathing" (as opposed to "casting a bolt"),
 * then PROJECT_XTRA will be set, and we can do a little "extra" damage.
 *
 * Although unused, we return "TRUE" if any "useful" effects were observed.
 *
 * While it is no longer true that "rad" must equal "zero", currently, it
 * always does.  We would need "confused monsters" or something to do otherwise.
 */
static bool project_p(int who, int rad, int y, int x, int dam, int typ, int flg)
{
    register int i, k = 0;

    /* Player blind-ness */
    int blind = FALSE;

    /* Player needs a "description" (he is blind) */
    int fuzzy = FALSE;

    /* Player should take "extra" effects from "breath" */
    int extra = (flg & PROJECT_XTRA) ? TRUE : FALSE;

    /* "Damage" factor.  Multiply by "mul/div" */
    int mul = 1, div = 1;

    /* Source monster */
    register monster_type *m_ptr;

    /* Monster name (for damage) */
    char killer[80];


    /* Hack -- decrease power over distance */
    if (rad) div = rad;

    /* Adjust damage */
    dam = dam * mul / div;

    /* Hack -- always do at least one point of damage */
    if (dam <= 0) dam = 1;

    /* Hack -- Never do excessive damage */
    if (dam > 1600) dam = 1600;


    /* Get "blind" */
    if (p_ptr->blind > 0) blind = TRUE;

    /* If the player is blind, be more descriptive */
    if (blind) fuzzy = TRUE;


    /* Get the source monster */
    m_ptr = &m_list[who];

    /* Get the monster's real name */
    monster_name(killer, m_ptr);


    /* Analyze the damage */
    switch (typ) {

	/* Standard damage -- hurts inventory too */
	case GF_ACID:
	    if (fuzzy) msg_print("You are hit by a jet of acidic fluid!");
	    acid_dam(dam, killer);
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_FIRE:
	    if (fuzzy) msg_print("You are hit by something hot!");
	    fire_dam(dam, killer);
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_COLD:
	    if (fuzzy) msg_print("You are hit by something cold!");
	    cold_dam(dam, killer);
	    break;

	/* Standard damage -- hurts inventory too */
	case GF_ELEC:
	    if (fuzzy) msg_print("You are hit by electricity!");
	    light_dam(dam, killer);
	    break;

	/* Standard damage */
	case GF_POIS:
	    if (fuzzy) msg_print("You are hit by a blast of noxious gases!");
	    poison_gas(dam, killer);
	    break;

	/* Standard damage */
	case GF_MISSILE:
	    if (fuzzy) msg_print("You are hit by something!");
	    take_hit(dam, killer);
	    break;

	/* Hold Orb -- Player only takes partial damage, cause he is "good" */
	case GF_HOLY_ORB:
	    if (fuzzy) msg_print("You are hit by something!");
	    dam /= 2;
	    take_hit(dam, killer);
	    break;

	/* Arrow -- XXX no dodging */
	case GF_ARROW:
	    if (fuzzy) msg_print("You are hit by something!");
	    take_hit(dam, killer);
	    break;

	/* Plasma -- XXX No resist */
	case GF_PLASMA:
	    if (fuzzy) msg_print("You are hit by something!");
	    take_hit(dam, killer);
	    if (extra && !p_ptr->resist_sound) {
		stun_player(randint((dam > 40) ? 35 : (dam * 3 / 4 + 5)));
	    }
	    break;

	case GF_NETHER:
	    if (fuzzy) msg_print("You are hit by an unholy blast!");
	    if (p_ptr->resist_nether) {
		dam *= 6; dam /= (randint(6) + 6);
	    }
	    else {
		if (!extra && p_ptr->hold_life && randint(5) > 1) {
		    msg_print("You keep hold of your life force!");
		}
		else if (extra && p_ptr->hold_life && randint(3) > 1) {
		    msg_print("You keep hold of your life force!");
		}
		else if (p_ptr->hold_life) {
		    msg_print("You feel your life slipping away!");
		    lose_exp(200 + (p_ptr->exp/1000) * MON_DRAIN_LIFE);
		}
		else {
		    msg_print("You feel your life draining away!");
		    lose_exp(200 + (p_ptr->exp/100) * MON_DRAIN_LIFE);
		}
	    }
	    take_hit(dam, killer);
	    break;

	/* Water -- stun/confuse */
	case GF_WATER:
	    if (fuzzy) msg_print("You are hit by a jet of water!");
	    if (!extra) {
		if (!p_ptr->resist_sound) stun_player(randint(15));
	    }
	    else {
		if (!p_ptr->resist_sound) stun_player(randint(55));
		if (!player_saves() &&
		    !p_ptr->resist_conf &&
		    !p_ptr->resist_chaos) {

		    if ((p_ptr->confused < 32000) &&
		    (p_ptr->confused > 0)) p_ptr->confused += 6;
		    else p_ptr->confused = randint(8) + 6;
		}
	    }
	    take_hit(dam, killer);
	    break;

	/* Chaos -- many effects */
	case GF_CHAOS:
	    if (fuzzy) msg_print("You are hit by wave of entropy!");
	    if (p_ptr->resist_chaos) {
		dam *= 6; dam /= (randint(6) + 6);
	    }
	    if ((!p_ptr->resist_conf) && (!p_ptr->resist_chaos)) {
		if (p_ptr->confused > 0) p_ptr->confused += 12;
		else p_ptr->confused = randint(20) + 10;
	    }
	    if (!p_ptr->resist_chaos) {
		p_ptr->image += randint(10);
	    }
	    if (extra && !p_ptr->resist_nether && !p_ptr->resist_chaos) {
		if (p_ptr->hold_life && randint(3) > 1) {
		    msg_print("You keep hold of your life force!");
		}
		else if (p_ptr->hold_life) {
		    msg_print("You feel your life slipping away!");
		    lose_exp(500 + (p_ptr->exp/1000) * MON_DRAIN_LIFE);
		}
		else {
		    msg_print("You feel your life draining away!");
		    lose_exp(5000 + (p_ptr->exp/100) * MON_DRAIN_LIFE);
		}
	    }
	    take_hit(dam, killer);
	    break;

	/* Shards -- mostly cutting */
	case GF_SHARDS:
	    if (fuzzy) msg_print("You are cut by sharp fragments!");
	    if (p_ptr->resist_shards) {
		dam *= 6; dam /= (randint(6) + 6);
	    }
	    else {
		cut_player(dam);
	    }
	    take_hit(dam, killer);
	    break;

	/* Sound -- mostly stunning */
	case GF_SOUND:
	    if (fuzzy) msg_print("You are deafened by a blast of noise!");
	    if (p_ptr->resist_sound) {
		dam *= 5; dam /= (randint(6) + 6);
	    }
	    else if (extra) {
		stun_player(randint((dam > 90) ? 35 : (dam / 3 + 5)));
	    }
	    else {
		stun_player(randint((dam > 60) ? 25 : (dam / 3 + 5)));
	    }
	    take_hit(dam, killer);
	    break;

	/* Pure confusion */
	case GF_CONFUSION:
	    if (fuzzy) msg_print("You are hit by a wave of dizziness!");
	    if (p_ptr->resist_conf) {
		dam *= 5; dam /= (randint(6) + 6);
	    }
	    if (extra && !p_ptr->resist_conf && !p_ptr->resist_chaos) {
		if (p_ptr->confused > 0) p_ptr->confused += 12;
		else p_ptr->confused = randint(20) + 10;
	    }
	    else if (!extra && !p_ptr->resist_conf && !p_ptr->resist_chaos) {
		if (p_ptr->confused > 0) p_ptr->confused += 8;
		else p_ptr->confused = randint(15) + 5;
	    }
	    take_hit(dam, killer);
	    break;

	/* Disenchantment -- see above */
	case GF_DISENCHANT:
	    if (fuzzy) msg_print("You are hit by something!");
	    if (p_ptr->resist_disen) {
		dam *= 6; dam /= (randint(6) + 6);
	    }
	    else {
		(void)apply_disenchant(0);
	    }
	    take_hit(dam, killer);
	    break;

	/* Nexus -- see above XXX No Bolt Effects? */
	case GF_NEXUS:
	    if (fuzzy) msg_print("You are hit by something strange!");
	    if (p_ptr->resist_nexus) {
		dam *= 6; dam /= (randint(6) + 6);
	    }
	    else if (extra) {
		apply_nexus(m_ptr);
	    }
	    take_hit(dam, killer);
	    break;

	/* Force -- mostly stun */
	case GF_FORCE:
	    if (fuzzy) msg_print("You are hit hard by a sudden force!");
	    if (extra) {
		if (!p_ptr->resist_sound) stun_player(randint(20));
	    }
	    else {
		if (!p_ptr->resist_sound) stun_player(randint(15) + 1);
	    }
	    take_hit(dam, killer);
	    break;

	/* Inertia -- slowness */
	case GF_INERTIA:
	    if (fuzzy) msg_print("You are hit by something!");
	    if (p_ptr->slow > 0) && (p_ptr->slow < 32000)) {
		p_ptr->slow += randint(5);
	    }
	    else {
		msg_print("You feel less able to move.");
		p_ptr->slow = randint(5) + 3;
	    }
	    take_hit(dam, killer);
	    break;

	/* Lite -- blinding */
	case GF_LITE:
	    if (fuzzy) msg_print("You are hit by something!");
	    if (p_ptr->resist_lite) {
		dam *= 4; dam /= (randint(6) + 6);
	    }
	    else if (!blind && !p_ptr->resist_blind) {
		msg_print("You are blinded by the flash!");
		p_ptr->blind += randint(5) + 2;
	    }
	    if( extra ) lite_area(char_row, char_col, 0, rad);
	    take_hit(dam, killer);
	    break;

	/* Dark -- blinding */
	case GF_DARK:
	    if (fuzzy) msg_print("You are hit by something!");
	    if (p_ptr->resist_dark) {
	       dam *= 4; dam /= (randint(6) + 6);
	    }
	    else if (!blind && !p_ptr->resist_blind) {
		msg_print("The darkness prevents you from seeing!");
		p_ptr->blind += randint(5) + 2;
	    }
	    if( extra ) unlite_area(char_row, char_col);
	    take_hit(dam, killer);
	    break;

	/* Time -- bolt fewer effects XXX */
	case GF_TIME:
	    if (fuzzy) msg_print("You are hit by something!");
	    i = randint(10);
	    if (!extra && (i == 10)) i = 9;
	    switch (i) {
		case 1: case 2: case 3: case 4: case 5:
		    msg_print("You feel life has clocked back.");
		    lose_exp(m_ptr->hp + (p_ptr->exp / 300) * MON_DRAIN_LIFE);
		    break;
		case 6: case 7: case 8: case 9:
		    switch (randint(6)) {
			case 1: k = A_STR; msg_print("You're not as strong as you used to be..."); break;
			case 2: k = A_INT; msg_print("You're not as bright as you used to be..."); break;
			case 3: k = A_WIS; msg_print("You're not as wise as you used to be..."); break;
			case 4: k = A_DEX; msg_print("You're not as agile as you used to be..."); break;
			case 5: k = A_CON; msg_print("You're not as hale as you used to be..."); break;
			case 6: k = A_CHR; msg_print("You're not as beautiful as you used to be..."); break;
		    }

		    p_ptr->cur_stat[k] = (p_ptr->cur_stat[k] * 3) / 4;
		    if (p_ptr->cur_stat[k] < 3) p_ptr->cur_stat[k] = 3;
		    set_use_stat(k);
		    prt_stat(k);
		    break;
		    
		case 10:
		    for (k = 0; k < 6; k++) {
			p_ptr->cur_stat[k] = (p_ptr->cur_stat[k] * 3) / 4;
			if (p_ptr->cur_stat[k] < 3) p_ptr->cur_stat[k] = 3;
			set_use_stat(k);
			prt_stat(k);
		    }
		    msg_print("You're not as strong as you used to be...");
		    msg_print("You're not as bright as you used to be...");
		    msg_print("You're not as wise as you used to be...");
		    msg_print("You're not as agile as you used to be...");
		    msg_print("You're not as hale as you used to be...");
		    msg_print("You're not as beautiful as you used to be...");
		    break;
	    }
	    take_hit(dam, killer);
	    break;

	/* Gravity -- stun or slowness, plus teleport */
	case GF_GRAVITY:
	    if (fuzzy) msg_print("You are hit by a surge of gravity!");
	    if (p_ptr->ffall) {
		dam *= 3; dam /= (randint(6) + 6);
	    }
	    else {
		if (!p_ptr->resist_sound) {
		    if (extra) stun_player(randint((dam > 90) ? 35 : (dam / 3 + 5)));
		    else stun_player(randint(15) + 1);
		}
		else {
		    if ((p_ptr->slow > 0) && (p_ptr->slow < 32000)) p_ptr->slow += randint(5);
		    else {
			msg_print("You feel less able to move.");
			p_ptr->slow = randint(5) + 3;
		    }
		}
	    }
	    if (extra) {
	    msg_print("Gravity warps around you.");
	    teleport(5);
	    }
	    take_hit(dam, killer);
	    break;

	/* Pure damage */
	case GF_MANA:
	    if (fuzzy) msg_print("You are hit by a beam of power!");
	    take_hit(dam, killer);
	    break;

	/* Pure damage */
	case GF_METEOR:
	    if (fuzzy) msg_print("You are hit by something!");
	    take_hit(dam, killer);
	    break;

	/* Ice -- cold plus stun plus cuts */
	case GF_ICE:
	    if (fuzzy) msg_print("You are hit by something cold and sharp!");
	    cold_dam(dam, killer);
	    if (!p_ptr->resist_shards) cut_player(damroll(8, 10));
	    if (extra) {
		if (!p_ptr->resist_sound) stun_player(randint(25));
	    }
	    else {
		if (!p_ptr->resist_sound) stun_player(randint(15) + 1);
	    }
	    break;

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
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
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
 * For example, this is used by the "GF_LITE" / "GF_DARK" ball weapons to do
 * "correct" room darkening.
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


