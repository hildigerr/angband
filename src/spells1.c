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
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *
 * Balls must explode BEFORE hitting walls, or they would "pass through" walls.
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


    /* Speed -- ignore "non-explosions" */
    if (!grids) return (FALSE);


    /* Start with "dist" of zero */
    dist = 0;



    /* Return "something was noticed" */
    return (notice);
}


