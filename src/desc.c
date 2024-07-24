/* File: desc.c */

/* Purpose: handle object descriptions, mostly string handling code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/*
 * There is a new metaphor for "store made objects". 
 *
 * If an item is in a store, it is guaranteed to be "known".  So perhaps it
 * should no longer be possible to see an item as "a red potion of death",
 * since once the potion type is known, all red potions are known.
 *
 * Also note that the "aware" predicate returns TRUE if the object is known.
 */


/*
 * Color adjectives and colors, for potions.
 * Hack -- The first three are hard-coded for slime mold juice,
 * apple juice, and water, so do not scramble them.
 */

static cptr potion_adj[MAX_COLORS] = {
    "Icky Green", "Light Brown", "Clear","Azure","Blue",
    "Blue Speckled","Black","Brown","Brown Speckled","Bubbling",
    "Chartreuse","Cloudy","Copper Speckled","Crimson","Cyan",
    "Dark Blue","Dark Green","Dark Red","Gold Speckled","Green",
    "Green Speckled","Grey","Grey Speckled","Hazy","Indigo",
    "Light Blue","Light Green","Magenta","Metallic Blue","Metallic Red",
    "Metallic Green","Metallic Purple","Misty","Orange","Orange Speckled",
    "Pink","Pink Speckled","Puce","Purple","Purple Speckled",
    "Red","Red Speckled","Silver Speckled","Smoky","Tangerine",
    "Violet","Vermilion","White","Yellow", "Purple Speckled",
    "Pungent","Clotted Red","Viscous Pink","Oily Yellow","Gloopy Green",
    "Shimmering","Coagulated Crimson"
};


/*
 * Color adjectives and colors, for mushrooms and molds
 */

static cptr food_adj[MAX_SHROOM] = {
    "Blue","Black","Black Spotted","Brown","Dark Blue",
    "Dark Green","Dark Red","Ecru","Furry","Green",
    "Grey","Light Blue","Light Green","Plaid","Red",
    "Slimy","Tan","White","White Spotted","Wooden",
    "Wrinkled",/*"Yellow","Shaggy","Red Spotted","Pale Blue","Dark Orange"*/
};


/*
 * Wood adjectives and colors, for staffs
 */

static cptr staff_adj[MAX_WOODS] = {
    "Aspen","Balsa","Banyan","Birch","Cedar",
    "Cottonwood","Cypress","Dogwood","Elm","Eucalyptus",
    "Hemlock","Hickory","Ironwood","Locust","Mahogany",
    "Maple","Mulberry","Oak","Pine","Redwood",
    "Rosewood","Spruce","Sycamore","Teak","Walnut",
    "Mistletoe","Hawthorn","Bamboo","Silver","Runed",
    "Golden","Ashen"/*,"Gnarled","Ivory","Decorative","Willow"*/
};


/*
 * Metal adjectives and colors, for wands
 */

static cptr wand_adj[MAX_METALS] = {
    "Aluminum","Cast Iron","Chromium","Copper","Gold",
    "Iron","Magnesium","Molybdenum","Nickel","Rusty",
    "Silver","Steel","Tin","Titanium","Tungsten",
    "Zirconium","Zinc","Aluminum-Plated","Copper-Plated","Gold-Plated",
    "Nickel-Plated","Silver-Plated","Steel-Plated","Tin-Plated","Zinc-Plated",
    "Mithril-Plated","Mithril","Runed","Bronze","Brass",
    "Platinum","Lead"/*,"Lead-Plated","Ivory","Pewter"*/
};


/*
 * Rock adjectives and colors, for rings
 */

static cptr ring_adj[MAX_ROCKS] = {
    "Alexandrite","Amethyst","Aquamarine","Azurite","Beryl",
    "Bloodstone","Calcite","Carnelian","Corundum","Diamond",
    "Emerald","Fluorite","Garnet","Granite","Jade",
    "Jasper","Lapis Lazuli","Malachite","Marble","Moonstone",
    "Onyx","Opal","Pearl","Quartz","Quartzite",
    "Rhodonite","Ruby","Sapphire","Tiger Eye","Topaz",
    "Turquoise","Zircon","Platinum","Bronze","Gold",
    "Obsidian","Silver","Tortoise Shell","Mithril","Jet",
    "Engagement","Adamantite"
};


/*
 * Amulet adjectives and colors, for amulets
 */

static cptr amulet_adj[MAX_AMULETS] = {
    "Amber","Driftwood","Coral","Agate","Ivory",
    "Obsidian","Bone","Brass","Bronze","Pewter",
    "Tortoise Shell","Golden","Azure","Crystal","Silver",
    "Copper"
};


/*
 * Syllables for scrolls
 */

static cptr syllables[MAX_SYLLABLES] = {
  "a","ab","ag","aks","ala","an","ankh","app",
  "arg","arze","ash","aus","ban","bar","bat","bek",
  "bie","bin","bit","bjor","blu","bot","bu",
  "byt","comp","con","cos","cre","dalf","dan",
  "den","der","doe","dok","eep","el","eng","er","ere","erk",
  "esh","evs","fa","fid","flit","for","fri","fu","gan",
  "gar","glen","gop","gre","ha","he","hyd","i",
  "ing","ion","ip","ish","it","ite","iv","jo",
  "kho","kli","klis","la","lech","man","mar",
  "me","mi","mic","mik","mon","mung","mur","nag","nej",
  "nelg","nep","ner","nes","nis","nih","nin","o",
  "od","ood","org","orn","ox","oxy","pay","pet",
  "ple","plu","po","pot","prok","re","rea","rhov",
  "ri","ro","rog","rok","rol","sa","san","sat",
  "see","sef","seh","shu","ski","sna","sne","snik",
  "sno","so","sol","sri","sta","sun","ta","tab",
  "tem","ther","ti","tox","trol","tue","turs","u",
  "ulk","um","un","uni","ur","val","viv","vly",
  "vom","wah","wed","werg","wex","whon","wun","x",
  "yerg","yp","zun","tri","blaa"
};


/*
 * Hold the titles of scrolls, ten characters each
 */

static char scroll_adj[MAX_TITLES][10];



/*
 * Initialize all Potions, wands, staves, scrolls, etc.	
 */
void flavor_init(void)
{
    register int        h, i, j, k;
    register cptr		tmp;
    vtype               string;


    /* Hack -- Play games with the R.N.G. */
    set_seed(randes_seed);

    /* The first 3 entries for potions are fixed */
    /* That is, slime mold juice, apple juice, water */
    for (i = 3; i < MAX_COLORS; i++) {
	j = rand_int(MAX_COLORS - 3) + 3;
	tmp = potion_adj[i];
	potion_adj[i] = potion_adj[j];
	potion_adj[j] = tmp;
    }

    /* Woods are used for staffs */
    for (i = 0; i < MAX_WOODS; i++) {
	j = rand_int(MAX_WOODS);
	tmp = staff_adj[i];
	staff_adj[i] = staff_adj[j];
	staff_adj[j] = tmp;
    }

    /* Wands are made of metal */
    for (i = 0; i < MAX_METALS; i++) {
	j = rand_int(MAX_METALS);
	tmp = wand_adj[i];
	wand_adj[i] = wand_adj[j];
	wand_adj[j] = tmp;
    }

    /* Rocks are used for rings */
    for (i = 0; i < MAX_ROCKS; i++) {
	j = rand_int(MAX_ROCKS);
	tmp = ring_adj[i];
	ring_adj[i] = ring_adj[j];
	ring_adj[j] = tmp;
    }

    /* Rocks are used for amulets */
    for (i = 0; i < MAX_AMULETS; i++) {
	j = rand_int(MAX_AMULETS);
	tmp = amulet_adj[i];
	amulet_adj[i] = amulet_adj[j];
	amulet_adj[j] = tmp;
    }

    /* Hack -- Molds and Mushrooms (not normal foods) have colors */
    for (i = 0; i < MAX_SHROOM; i++) {
	j = rand_int(MAX_SHROOM);
	tmp = food_adj[i];
	food_adj[i] = food_adj[j];
	food_adj[j] = tmp;
    }

    /* Hack -- Scrolls have titles, and are always white */
    for (h = 0; h < MAX_TITLES; h++) {
	string[0] = '\0';

	/* Construct a two or three word title */
	k = rand_range(2,3);
	for (i = 0; i < k; i++) {

	    /* Add a one or two syllable word */
	    for (j = rand_range(1,2); j > 0; j--) {
		(void)strcat(string, syllables[rand_int(MAX_SYLLABLES)]);
	    }

	    /* Add a space */
	    if (i < k - 1) {
		(void)strcat(string, " ");
	    }
	}

	/* Hack -- chop off part of the title */
	if (string[8] == ' ') {
	    string[8] = '\0';
	}
	else {
	    string[9] = '\0';
	}

	/* Save the title */
	(void)strcpy(scroll_adj[h], string);

    }

    /* Hack -- undo the hack above */
    reset_seed();
}






/*
 * Return "TRUE" is the given item has a "flavor"
 */
bool flavor_p(inven_type *i_ptr)
{
    switch (i_ptr->tval) {

      /* The standard "flavored" items */
      case TV_AMULET:
      case TV_RING:
      case TV_STAFF:
      case TV_WAND:
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_POTION1:
      case TV_POTION2:
      case TV_ROD:
	return (TRUE);

      /* Hack -- food SOMETIMES has a flavor */
      case TV_FOOD:
	if (i_ptr->sval < SV_FOOD_MIN_FOOD) return (TRUE);
    }

    /* No flavor */
    return (FALSE);
}



/*
 * Is a given item "fully identified"?
 */
bool known2_p(inven_type *i_ptr)
{
    /* Some items get "tagged" as known */
    if (i_ptr->ident & ID_KNOWN) return (TRUE);

    /* Assume not known */
    return (FALSE);    
}



/*
 * Known2 is true when the "attributes" of an object are "known".
 * These include tohit, todam, toac, cost, and pval (charges).
 *
 * This routine also removes inscriptions generated by "innate feelings".
 */
void known2(inven_type *i_ptr)
{
    /* Remove "inscriptions" created when ID_FELT was set */
    if (i_ptr->ident & ID_FELT) {

	/* Hack -- Remove any inscription we may have made */
	if ((streq(i_ptr->inscrip, "cursed")) ||

	    (streq(i_ptr->inscrip, "terrible")) ||
	    (streq(i_ptr->inscrip, "worthless")) ||

	    (streq(i_ptr->inscrip, "blessed")) ||

	    (streq(i_ptr->inscrip, "special")) ||
	    (streq(i_ptr->inscrip, "excellent")) ||
	    (streq(i_ptr->inscrip, "good")) ||

	    (streq(i_ptr->inscrip, "average"))) {

	    /* Forget the inscription */
	    inscribe(i_ptr, "");
	}
    }

    /* Hack -- notice cursed items */
    else if (cursed_p(i_ptr)) {

	/* Put an initial inscription */
	inscribe(i_ptr, "cursed");
    }

    i_ptr->ident &= ~ID_MAGIK;

    /* Clear the "Felt" info */
    i_ptr->ident &= ~ID_FELT;

    /* Clear the "Empty" info */
    i_ptr->ident &= ~ID_EMPTY;

    /* Now we know all about it */
    i_ptr->ident |= ID_KNOWN;
}




/*
 * Is the player "aware" of the "flavor" of the given object?
 * The player is always "aware" of objects with no "flavor".
 * The player is "aware" of any object which he fully "knows".
 *
 * Thus, the only things the player can be "unaware of" are Potions, Scrolls,
 * Food, Amulets, Rings, Staffs, Wands, and Rods, which have unknown effects.
 */
bool inven_aware_p(inven_type *i_ptr)
{
    /* Hack -- player always knows "bland" objects */
    if (!flavor_p(i_ptr)) return (TRUE);

    /* Hack -- "known" induces "aware" */
    if (known2_p(i_ptr)) return (TRUE);

    /* Check the "x_list" */
    return (x_list[i_ptr->k_idx].aware);
}


/*
 * The player is now aware of the effects of the given object.
 */
void inven_aware(inven_type *i_ptr)
{
    /* Fully aware of the effects */
    x_list[i_ptr->k_idx].aware = TRUE;
}


/*
 * Has the player "tried" a given object?
 */
bool inven_tried_p(inven_type *i_ptr)
{
    /* Hack -- "aware" cancels "tried" */
    if (inven_aware_p(i_ptr)) return (FALSE);

    /* Check the "x_list" */
    return (x_list[i_ptr->k_idx].tried);
}


/*
 * Something has been "sampled"
 */
void inven_tried(inven_type *i_ptr)
{
    /* Mark it as tried (even if "aware") */
    x_list[i_ptr->k_idx].tried = TRUE;
}














/*
 * Helper function.  Compare the "ident" field of two objects.
 */
static bool similar_ident(inven_type *i_ptr, inven_type *j_ptr)
{
    /* XXX XXX Hack -- no longer possible (???) */
    if (inven_aware_p(i_ptr) != inven_aware_p(j_ptr)) return (0);

    /* XXX Hack -- force identical "ident" flag sets */
    if (i_ptr->ident != j_ptr->ident) return (0);

    /* Food, Potions, Scrolls are "simple" objects */
    if (i_ptr->tval == TV_FOOD) return (1);
    if (i_ptr->tval == TV_POTION) return (1);
    if (i_ptr->tval == TV_SCROLL) return (1);

    /* XXX Mega-Hack -- missiles do not have to be identified */
    if (i_ptr->tval == TV_SHOT) return (1);
    if (i_ptr->tval == TV_BOLT) return (1);
    if (i_ptr->tval == TV_ARROW) return (1);

    /* Normally, both items must be fully "known" to stack */
    if (!known2_p(i_ptr) || !known2_p(j_ptr)) return (0);

    /* Allow match */
    return (1);
}




/*
 * Determine if an item can "absorb" a second item
 *
 * No object can absorb itself.  This prevents object replication.
 *
 * When an object absorbs another, the second object loses all
 * of its attributes (except for "number", which gets added in),
 * so it is important to verify that all important attributes match.
 *
 * These fields are ignored: k_idx, ix, iy, scost.
 *
 * Note that all "problems" associated with "combining" differently
 * priced objects have been removed by never combining such objects.
 *
 * XXX Currently, we allow identical unidentified "stackables" to
 * combine.  This includes "arrows", which is a major hack.
 *
 * We do not allow chests to combine, it would be annoying.
 *
 * We do NOT allow artifacts or ego-items or dragon scale mail to
 * combine, since the "activation" code would become very messy.
 */
int item_similar(inven_type *i_ptr, inven_type *j_ptr)
{
    /* Hack -- Identical items cannot be stacked */
    if (i_ptr == j_ptr) return (0);

    /* Different objects cannot be stacked */
    if (i_ptr->k_idx != j_ptr->k_idx) return (0);


    /* Hack -- refuse weapons/armor */
    if (
	(wearable_p(i_ptr)) &&
        (i_ptr->tval != TV_LITE) &&
        (i_ptr->tval != TV_RING) &&
        (i_ptr->tval != TV_AMULET)) return (0);
        
    /* Hack -- refuse wands/staffs/rods */
    if (
        ((i_ptr->tval == TV_STAFF) ||
         (i_ptr->tval == TV_WAND) ||
         (i_ptr->tval == TV_ROD))) return (0);
        

    /* Different charges (etc) cannot be stacked */
    if (i_ptr->pval != j_ptr->pval) return (0);

    /* Require matching prices */
    if ((i_ptr->cost != j_ptr->cost)) return (0);


    /* Require many identical values */
    if ((i_ptr->tohit     != j_ptr->tohit)     ||
	(i_ptr->todam     != j_ptr->todam)     ||
	(i_ptr->toac      != j_ptr->toac)      ||
	(i_ptr->ac        != j_ptr->ac)        ||
	(i_ptr->damage[0]        != j_ptr->damage[0])        ||
	(i_ptr->damage[1]        != j_ptr->damage[1])) {
	return (0);
    }

    /* Require identical flags */
    if ((i_ptr->flags1 != j_ptr->flags1) ||
	(i_ptr->flags2 != j_ptr->flags2) ||
	(i_ptr->flags3 != j_ptr->flags3)) {
	return (0);
    }

    /* Both items must be "fully identified" (see above) */
    if (!similar_ident(i_ptr, j_ptr)) return (0);


    /* Require identical "artifact" names */
    if (i_ptr->name1 != j_ptr->name1) return (0);

    /* Require identical "ego-item" names */
    if (i_ptr->name2 != j_ptr->name2) return (0);


    /* XXX Hack -- never stack "activatable" items */
    if (wearable_p(i_ptr) && (i_ptr->flags3 & TR3_ACTIVATE)) return (0);
    if (wearable_p(j_ptr) && (j_ptr->flags3 & TR3_ACTIVATE)) return (0);


    /* Hack -- Never stack chests */
    if (i_ptr->tval == TV_CHEST) return (0);


    /* No stack can grow bigger than a certain size */
    if (i_ptr->number + j_ptr->number >= MAX_STACK_SIZE) return (0);


    /* Require matching "inscriptions" */
    if (strcmp(i_ptr->inscrip, j_ptr->inscrip)) return (0);


    /* Paranoia -- Different types cannot be stacked */
    if (i_ptr->tval != j_ptr->tval) return (0);

    /* Paranoia -- Different sub-types cannot be stacked */
    if (i_ptr->sval != j_ptr->sval) return (0);

    /* Paranoia -- Could possibly have objects with "broken" weights */
    if (i_ptr->weight != j_ptr->weight) return (0);

    /* Paranoia -- Timeout should always be zero (see "TR3_ACTIVATE" above) */
    if (i_ptr->timeout || j_ptr->timeout) return (0);


    /* They match, so they must be similar */
    return (TRUE);
}










/*
 * defines for pval_use, determine how the pval field is printed 
 */

#define IGNORED     0		/* ignore the pval field */
#define Z_PLUSSES   1		/* show pval as "(+x)" */
#define PLUSSES     2		/* show pval as "(+x)", unless zero */
#define FLAGS       3		/* show pval as "(+x to yyy)", or "(+x)" */
#define CHARGES     5		/* show pval as "(x charges)" */
#define USE_LITE    7		/* show pval as "with x turns of light" */


/*
 * Creates a description of the item "i_ptr", and stores it in "out_val".
 *
 * If "pref" is TRUE, the description is verbose, and has an article (or number,
 * or "no more") prefixed to the description.
 *
 * Note that out_val must be large enough to hold more than 80 characters
 * (is this true?), but it seems that 160 chars will always be enough
 *
 * Originally used sprintf(buf, "%+d", val), but several machines don't
 * support it, so use sprintf(buf, "%c%d", MY_POM(val), MY_ABS(val))
 *
 *
 * Note that ALL ego-items (when known) append an "Artifact Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that ALL artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known".
 *
 * Special Lite's use the "k_list" base-name (Phial, Star, or Arkenstone).
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_list" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_list" name.
 * In all three cases above, the "artifact name" is appended if the object
 * is "known".
 *
 * There is extra processing for "The One Ring", though I disapprove...
 */
void objdes(char *out_val, inven_type *i_ptr, int pref)
{
    register cptr basenm, modstr;
    bool aware, append_name;
    int power, indexx, pval_use;
    vtype                tmp_str, damstr;
    bigvtype             tmp_val;


    /* Is the player "aware" of the object? */
    aware = (inven_aware_p(i_ptr));

    /* Hack -- Extract the sub-type "indexx" */
    indexx = i_ptr->sval & ITEM_SUBVAL_MASK;

    /* Extract the (default) "base name" */
    basenm = k_list[i_ptr->k_idx].name;

    /* Assume we will NOT append the "kind" name */
    append_name = FALSE;

    /* Assume no modifier string */
    modstr = NULL;

    /* Start with no damage string */
    damstr[0] = '\0';

    /* Assume no display of "pval" */
    pval_use = IGNORED;


    /* Analyze the object */
    switch (i_ptr->tval) {

      /* Some objects are easy to describe */
      case TV_MISC:
      case TV_SPIKE:
      case TV_FLASK:
	break;

      case TV_CHEST:
	break;

      /* Weapons have a damage string, and flags */
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:
      case TV_DIGGING:
	pval_use = (i_ptr->flags1 & TR1_TUNNEL) ? Z_PLUSSES : FLAGS;
	(void)sprintf(damstr, " (%dd%d)", i_ptr->damage[0], i_ptr->damage[1]);
	break;

      /* Bows get a special "damage string" */
      case TV_BOW:

	/* Hack -- Extract the "base power" */
	power = (i_ptr->sval % 10);

	/* Build the damage string */
	sprintf(damstr, " (x%d)", power);

	if (artifact_p(i_ptr)) {	/* only show pval for artifacts... */
	    pval_use = FLAGS;
	}

	/* All done */
	break;

      case TV_SHOT:
      case TV_BOLT:
      case TV_ARROW:
	(void)sprintf(damstr, " (%dd%d)", i_ptr->damage[0], i_ptr->damage[1]);
	break;


      /* Armour uses flags */
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_HELM:
      case TV_SHIELD:
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
      case TV_DRAG_ARMOR:
	pval_use = FLAGS;
	break;


      /* Lites (including a few "Specials") */
      case TV_LITE:

	pval_use = USE_LITE;

	/* Special Lites do NOT show "turns of light" */
	if (artifact_p(i_ptr)) {
	    pval_use = IGNORED;
	}

	break;


      /* Amulets (including a few "Specials") */
      case TV_AMULET:

	pval_use = FLAGS;

	if (!aware) {
	    basenm = "& %s Amulet~";
	    modstr = amulet_adj[indexx];
	}
	else if (artifact_p(i_ptr)) {
	    /* Use default values */
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Amulet~";
	    modstr = amulet_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Amulet~";
	    append_name = TRUE;
	}
	break;


      /* Rings (including a few "Specials") */
      case TV_RING:

	pval_use = PLUSSES;

	if (!aware) {
	    basenm = "& %s Ring~";
	    modstr = ring_adj[indexx];
	    if (i_ptr->sval == SV_RING_POWER) modstr = "Plain Gold";
	}
	else if (artifact_p(i_ptr)) {
	    /* Use default values */
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Ring~";
	    modstr = ring_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Ring~";
	    append_name = TRUE;
	}
	break;

      case TV_STAFF:
	pval_use = CHARGES;
	if (!aware) {
	    basenm = "& %s Staff~";
	    modstr = staff_adj[indexx];
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Staff~";
	    modstr = staff_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Staff~";
	    append_name = TRUE;
	}
	break;

      case TV_WAND:
	pval_use = CHARGES;
	if (!aware) {
	    basenm = "& %s Wand~";
	    modstr = wand_adj[indexx];
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Wand~";
	    modstr = wand_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Wand~";
	    append_name = TRUE;
	}
	break;

      case TV_ROD:
	if (!aware) {
	    basenm = "& %s Rod~";
	    modstr = wand_adj[indexx];
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Rod~";
	    modstr = wand_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Rod~";
	    append_name = TRUE;
	}
	break;

      case TV_SCROLL1:
      case TV_SCROLL2:
	if (!aware) {
	    basenm = "& Scroll~ titled \"%s\"";
	    modstr = scroll_adj[indexx];
	}
	else if (!plain_descriptions) {
	    basenm = "& Scroll~ titled \"%s\"";
	    modstr = scroll_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Scroll~";
	    append_name = TRUE;
	}
	break;

      case TV_POTION1:
      case TV_POTION2:
	if (!aware) {
	    basenm = "& %s Potion~";
	    modstr = potion_adj[indexx];
	}
	else if (!plain_descriptions) {
	    basenm = "& %s Potion~";
	    modstr = potion_adj[indexx];
	    append_name = TRUE;
	}
	else {
	    basenm = "& Potion~";
	    append_name = TRUE;
	}
	break;

      case TV_FOOD:

	/* Ordinary food is "boring" */
	if (i_ptr->sval >= SV_FOOD_MIN_FOOD) break;

	/* The Molds */
	if (i_ptr->sval >= SV_FOOD_MIN_MOLD) {
	    if (!aware) {
		basenm = "& Hairy %s Mold~";
		modstr = food_adj[indexx];
	    }
	    else if (!plain_descriptions) {
		basenm = "& Hairy %s Mold~";
		modstr = food_adj[indexx];
		append_name = TRUE;
	    }
	    else {
		basenm = "& Hairy Mold~";
		append_name = TRUE;
	    }
	}

	/* The Mushrooms */
	else {

	    if (!aware) {
		basenm = "& %s Mushroom~";
		modstr = food_adj[indexx];
	    }
	    else if (!plain_descriptions) {
		basenm = "& %s Mushroom~";
		modstr = food_adj[indexx];
		append_name = TRUE;
	    }
	    else {
		basenm = "& Mushroom~";
		append_name = TRUE;
	    }
	}
	break;


      /* Magic Books */
      case TV_MAGIC_BOOK:
	modstr = basenm;
	basenm = "& Book~ of Magic Spells %s";
	break;

      /* Prayer Books */
      case TV_PRAYER_BOOK:
	modstr = basenm;
	basenm = "& Holy Book~ of Prayers %s";
	break;


      /* Things in the dungeon */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
      case TV_SECRET_DOOR:
      case TV_RUBBLE:
	break;

      case TV_GOLD:
      case TV_INVIS_TRAP:
      case TV_VIS_TRAP:
      case TV_UP_STAIR:
      case TV_DOWN_STAIR:
	strcpy(out_val, basenm);
	return;

      case TV_STORE_DOOR:
	sprintf(out_val, "the entrance to the %s", basenm);
	return;

      /* Used in the "inventory" routine */
      default:
	strcpy(out_val, "(nothing)");
	return;
    }


    /* Insert the modifier */
    if (modstr) {
	sprintf(tmp_val, basenm, modstr);
    }
    else {
	strcpy(tmp_val, basenm);
    }


    /* Append the "kind name" to the "base name" */
    if (append_name) {
	(void)strcat(tmp_val, " of ");
	(void)strcat(tmp_val, k_list[i_ptr->k_idx].name);
    }


    /* Attempt to pluralize somewhat correctly */
    if (i_ptr->number != 1) {
	insert_str(tmp_val, "s~", "ses");
	insert_str(tmp_val, "x~", "xes");
	insert_str(tmp_val, "sh~", "shes");
	insert_str(tmp_val, "ch~", "ches");
	insert_str(tmp_val, "~", "s");
    }

    /* Delete the plural, if any */
    else {
	insert_str(tmp_val, "~", NULL);
    }


    /* Hack -- No prefixes requested */
    if (!pref) {

	cptr skip = tmp_val;

	/* Delete the count symbol */
	if (skip[0] == '&') skip += 2;

	/* Use the name (without the "&") */
	(void)strcpy(out_val, skip);

	/* Short answer... */
	return;
    }


    /* Hack -- Append "Artifact" or "Special" names */
    if (known2_p(i_ptr)) {

	/* Hack -- grab the artifact name */
	if (i_ptr->name1) {
	    (void)strcat(tmp_val, " ");
	    (void)strcat(tmp_val, art_names[i_ptr->name1]);
	}

	/* Otherwise, grab the "ego-item" name */
	else if (i_ptr->name2) {
	    (void)strcat(tmp_val, " ");
	    (void)strcat(tmp_val, ego_names[i_ptr->name2]);
	}
    }

    /* Append the "damage info", if any */
    if (damstr[0]) {
	(void)strcat(tmp_val, damstr);
    }


    /* We know it, describe it */	
    if (known2_p(i_ptr)) {

	char *tail = tmp_val + strlen(tmp_val);

	/* Show the tohit/todam on request */
	if (i_ptr->ident & ID_SHOW_HITDAM) {
	    (void)sprintf(tail, " (%c%d,%c%d)",
			  MY_POM(i_ptr->tohit), MY_ABS(i_ptr->tohit),
			  MY_POM(i_ptr->todam), MY_ABS(i_ptr->todam));
	}
	
	/* Show the tohit if needed */
	else if (i_ptr->tohit) {
	    (void)sprintf(tail, " (%c%d)",
			  MY_POM(i_ptr->tohit), MY_ABS(i_ptr->tohit));
	}

	/* Show the todam if needed */
	else if (i_ptr->todam) {
	    (void)sprintf(tail, " (%c%d)",
			  MY_POM(i_ptr->todam), MY_ABS(i_ptr->todam));
	}
    }


    /* Add in the "armor class info", base and magic */
    /* Hack -- show "zero" ac for crowns */
    if (i_ptr->ac || (i_ptr->tval == TV_HELM)) {
	char *tail;
	char b1 = '[', b2 = ']';
	tail = tmp_val + strlen(tmp_val);
	(void)sprintf(tail, " %c%d", b1, i_ptr->ac);
	if (known2_p(i_ptr)) {
	    tail = tail + strlen(tail);
	    (void)sprintf(tail, ",%c%d",
			  MY_POM(i_ptr->toac), MY_ABS(i_ptr->toac));
	}
	tail = tail + strlen(tail);
	(void)sprintf(tail, "%c", b2);
    }

    /* No base armor, but does increase armor */
    else if (i_ptr->toac && known2_p(i_ptr)) {
	char *tail = tmp_val + strlen(tmp_val);
	(void)sprintf(tail, " [%c%d]",
		      MY_POM(i_ptr->toac), MY_ABS(i_ptr->toac));
    }


    /* Unknown things cannot display the charge */
    if (!known2_p(i_ptr)) pval_use = IGNORED;

    /* Erase tmp_str */
    tmp_str[0] = '\0';

    /* override defaults, check for pval flags in the ident field */
	if (pval_use != IGNORED) {
	    if (pval_use == USE_LITE);
	    else if (i_ptr->ident & ID_NOSHOW_P1)
		pval_use = IGNORED;
	    else if (i_ptr->ident & ID_NOSHOW_TYPE)
		pval_use = PLUSSES;
	}

    /* Nothing to add */
    if (pval_use == IGNORED) {
	/* Nothing */
    }

    /* Hack -- Boring Shovels */
    else if (pval_use == Z_PLUSSES) {
	(void)sprintf(tmp_str, "%c%d",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Torches and Lanterns have predictable life */
    else if (pval_use == USE_LITE) {
	(void)sprintf(tmp_str, "with %d turns of light", i_ptr->pval);
    }

    /* Wands and Staffs have charges */
    else if (pval_use == CHARGES) {
	    (void)sprintf(tmp_str, "%d charge%s",
			  i_ptr->pval, (i_ptr->pval == 1 ? "" : "s"));
    }

    /* Nothing to declare */
    else if (i_ptr->pval == 0) {
	/* Nothing */
    }

    /* Boring objects */        
    else if (pval_use == PLUSSES) {
	(void)sprintf(tmp_str, "%c%d",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

		else if (i_ptr->ident & ID_NOSHOW_TYPE) {
		    (void)sprintf(tmp_str, " %c%d",
				  MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Hack -- Everything else is a flag */
    else if (pval_use != FLAGS) {
	/* Nothing */
    }

    /* Speed */
    else if ((i_ptr->flags1 & TR1_SPEED) &&
	     (i_ptr->name2 != EGO_SPEED)) {
	(void)sprintf(tmp_str, "%c%d to speed",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Search (Hack -- display redundant info?) */
    else if (i_ptr->flags1 & TR1_SEARCH) {
	/* && (i_ptr->name2 != EGO_SEARCH) */
	(void)sprintf(tmp_str, "%c%d to searching",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Stealth */
    else if ((i_ptr->flags1 & TR1_STEALTH) &&
	     (i_ptr->name2 != EGO_STEALTH)) {
	(void)sprintf(tmp_str, "%c%d to stealth",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Infravision */
    else if ((i_ptr->flags1 & TR1_INFRA) &&
	     (i_ptr->name2 != EGO_INFRAVISION)) {
	(void)sprintf(tmp_str, "%c%d to infravision",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }

    /* Attack speed */
    else if (i_ptr->flags1 & TR1_ATTACK_SPD) {
	(void)sprintf(tmp_str, "%c%d attack%s",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval),
		      ((MY_ABS(i_ptr->pval) == 1) ? "" : "s"));
    }

    /* Default to Boring Plusses */
    else {
	(void)sprintf(tmp_str, "%c%d",
		      MY_POM(i_ptr->pval), MY_ABS(i_ptr->pval));
    }


    /* Extract the extra info, if any */
    if (tmp_str[0]) {
	char *tail = tmp_val + strlen(tmp_val);
	(void)sprintf(tail, " (%s)", tmp_str);
    }


    /* The object "expects" a "number" */
    if (tmp_val[0] == '&') {

	/* Hack -- None left */
	if (i_ptr->number < 1) {
	    (void)sprintf(out_val, "%s%s", "no more", &tmp_val[1]);
	}

	/* Extract the number */
	else if (i_ptr->number > 1) {
	    (void)sprintf(out_val, "%d%s", (int)i_ptr->number, &tmp_val[1]);
	}

	/* Hack -- The only one of its kind */
	else if (known2_p(i_ptr) && artifact_p(i_ptr)) {
	    (void)sprintf(out_val, "The%s", &tmp_val[1]);
	}

	/* A single one, with a vowel */
	else if (is_a_vowel(tmp_val[2])) {
	    (void)sprintf(out_val, "an%s", &tmp_val[1]);
	}

	/* A single one, without a vowel */
	else {
	    (void)sprintf(out_val, "a%s", &tmp_val[1]);
	}
    }

    /* Hack -- objects that never take an article */
    else {

	/* Hack -- all gone */
	if (i_ptr->number < 1) {
	    (void)sprintf(out_val, "no more %s", tmp_val);
	}

	/* Prefix a number if required */
	else if (i_ptr->number > 1) {
	    (void)sprintf(out_val, "%d %s", (int)i_ptr->number, tmp_val);
	}

	/* Hack -- The only one of its kind */
	else if (known2_p(i_ptr) && artifact_p(i_ptr)) {
	    (void)sprintf(out_val, "The %s", tmp_val);
	}

	/* Hack -- single items get no prefix */
	else {
	    (void)strcpy(out_val, tmp_val);
	}
    }


    /* Start with the user's inscription */
    strcpy(tmp_str, i_ptr->inscrip);


    /* Hack -- create a "fake" inscription */
    if (!tmp_str[0]) {

	/* If the item is "known", only inscribe curses */
	if (known2_p(i_ptr)) {
	    if (cursed_p(i_ptr)) (void)strcat(tmp_str, "cursed");
	}

	/* Note "cursed" if any curse has been felt */
	else if ((i_ptr->ident & ID_FELT) && (cursed_p(i_ptr))) {
	    (void)strcat(tmp_str, "cursed");
	}

	/* Note "tried" if the object has been tested */
	else if (inven_tried_p(i_ptr)) {
	    (void)strcat(tmp_str, "tried");
	}
    }


    /* Hack -- note empty wands/staffs */
    if (!known2_p(i_ptr) && (i_ptr->ident & ID_EMPTY)) {
	if (tmp_str[0]) strcat(tmp_str, " ");
	(void)strcat(tmp_str, "empty");
    }


    /* If we created an inscription, append it */
    if (tmp_str[0]) {
	char *tail = out_val + strlen(out_val);
	(void)sprintf(tail, " {%s}", tmp_str);
    }
}




/*
 * Make "i_ptr" a "clean" copy of the given "kind" of object
 */
void invcopy(inven_type *i_ptr, int k_idx)
{
    register inven_kind *k_ptr;

    /* Get the object template */
    k_ptr = &k_list[k_idx];

    /* Save the kind index */
    i_ptr->k_idx = k_idx;

    /* Quick access to tval/sval */
    i_ptr->tval = k_ptr->tval;
    i_ptr->sval = k_ptr->sval;

    /* Save the default "pval" */
    i_ptr->pval = k_ptr->pval;

    /* Default number and weight */
    i_ptr->number = k_ptr->number;
    i_ptr->weight = k_ptr->weight;

    /* Default magic */
    i_ptr->tohit = k_ptr->tohit;
    i_ptr->todam = k_ptr->todam;
    i_ptr->toac = k_ptr->toac;
    i_ptr->ac = k_ptr->ac;
    i_ptr->damage[0] = k_ptr->damage[0];
    i_ptr->damage[1] = k_ptr->damage[1];

    /* Default cost and flags */
    i_ptr->cost = k_ptr->cost;
    i_ptr->flags1 = k_ptr->flags1;
    i_ptr->flags2 = k_ptr->flags2;

    /* Wipe the inscription */
    i_ptr->inscrip[0] = '\0';

    /* No artifact name */
    i_ptr->name1 = 0;

    /* No special name */
    i_ptr->name2 = 0;

    /* No ident info yet */
    i_ptr->ident = 0;

    i_ptr->tchar = k_ptr->tchar;
    i_ptr->level = k_ptr->level;
}






