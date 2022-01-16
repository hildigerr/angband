/* File: recall.c */

/* Purpose: maintain and display monster memory */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"

static void roff(const char *);

static const char  *desc_atype[] = {
    "do something undefined",
    "attack",
    "weaken",
    "confuse",
    "terrify",
    "burn",
    "shoot acid",
    "freeze",
    "electrify",
    "corrode",
    "blind",
    "paralyse",
    "steal money",
    "steal things",
    "poison",
    "reduce dexterity",
    "reduce constitution",
    "drain intelligence",
    "drain wisdom",
    "lower experience",
    "call for help",
    "disenchant",
    "eat your food",
    "absorb light",
    "absorb charges",
    "reduce all stats"
};

static const char  *desc_amethod[] = {
    "make an undefined advance",
    "hit",
    "bite",
    "claw",
    "sting",
    "touch",
    "kick",
    "gaze",
    "breathe",
    "spit",
    "wail",
    "embrace",
    "crawl on you",
    "release spores",
    "beg",
    "slime you",
    "crush",
    "trample",
    "drool",
    "insult",
    "butt",
    "charge",
    "engulf",
    "moan"
};

static const char  *desc_howmuch[] = {
    " not at all",
    " a bit",
    "",
    " quite",
    " very",
    " most",
    " highly",
    " extremely"
};

static const char  *desc_move[] = {
   "move invisibly",
   "open doors",
   "pass through walls",
   "kill weaker creatures",
   "pick up objects",
   "breed explosively"
};

static const char  *desc_spell[] = {
    "1",
    "2",
    "4",
    "8",
    "teleport short distances",
    "teleport long distances",
    "teleport %s prey",
    "cause light wounds",
    "cause serious wounds",
    "paralyse %s prey",
    "induce blindness",
    "confuse",
    "terrify",
    "summon a monster",
    "summon the undead",
    "slow %s prey",
    "drain mana",
    "summon demonkind",
    "summon dragonkind",
    "lightning",	/* breaths 1 */
    "poison gases",
    "acid",
    "frost",
    "fire",	/* end */
    "cast fire bolts",
    "cast frost bolts",
    "cast acid bolts",
    "cast magic missiles",
    "cause critical wounds",
    "cast fire balls",
    "cast frost balls",
    "cast mana bolts",
    "chaos",	/* breaths2 */
    "shards",
    "sound",
    "confusion",
    "disenchantment",
    "nether",	/* end of part1 */
    "cast lightning bolts",
    "cast lightning balls",
    "cast acid balls",
    "create traps",
    "wound %s prey",
    "cast mind blasting",
    "teleport away %s prey",
    "heal",
    "haste",
    "fire missiles",
    "cast plasma bolts",
    "summon many creatures",
    "cast nether bolts",
    "cast ice bolts",
    "cast darkness",
    "cause amnesia",
    "sear your mind",
    "cast stinking clouds",
    "teleport %s preys level",
    "cast water bolts",
    "cast whirlpools",
    "cast nether balls",
    "summon an angel",
    "summon spiders",
    "summon hounds",
    "nexus",	/* part 2... */
    "elemental force",	/* breaths 3 */
    "inertia",
    "light",
    "time",
    "gravity",
    "darkness",
    "plasma",	/* end */
    "fire arrows",
    "summon Ringwraiths",
    "cast darkness storms",
    "cast mana storms",
    "summon reptiles",
    "summon ants",
    "summon unique creatures",
    "summon greater undead",
    "summon ancient dragons"
};

static const char  *desc_weakness[] = {
    "bright light",
    "rock remover"
};

static const char  *desc_immune[] = {
    "frost",
    "fire",
    "lightning",
    "poison",
    "acid"
};

static vtype        roffbuf;	   /* Line buffer. */
static char        *roffp;	   /* Pointer into line buffer. */
static int          roffpline;	   /* Place to print line now being loaded. */

/* Pluralizer: count, singular, plural) */
#define plural(c, ss, sp)	((c) == 1 ? (ss) : (sp))

/* Number of kills needed for information. */
/* the higher the level of the monster, the fewer the kills you need */
#define knowarmor(l,d)		((d) > 304 / (4 + (l)))

/* the higher the level of the monster, the fewer the attacks you need, */
/* the more damage an attack does, the more attacks you need */
#define knowdamage(l,a,d)	((4 + (l))*(a) > 80 * (d))

/* use slightly different tests for unique monsters to hasten learning -CFT */
#define knowuniqarmor(l,d)    ((d) > 304 / (38 + (5*(l))/4))
#define knowuniqdamage(l,a,d) ((4 +(long)(l))*(2*(long)(a)) > 80 * (d))






/*
 * Do we know anything special about this monster? 
 */
int bool_roff_recall(int r_idx)
{
    register monster_lore *l_ptr;
    register int          i;

    if (wizard) return TRUE;

    l_ptr = &l_list[r_idx];

    if (l_ptr->r_cmove || l_ptr->r_cdefense ||
	l_ptr->r_spells || l_ptr->r_spells2 || l_ptr->r_spells3 ||
	l_ptr->r_kills || l_ptr->r_deaths) {
	return TRUE;
    }

    for (i = 0; i < 4; i++) {
	if (l_ptr->r_attacks[i]) {
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Print out what we have discovered about this monster.
 */
int roff_recall(int r_idx)
{
    const char             *p, *q;
    attid                  *pu;
    vtype                   temp;

    register monster_lore  *l_ptr;
    register monster_race  *r_ptr;

    register u32b         i, j, k;

    int                 mspeed;
    u32b              rcmove, rspells, rspells2, rspells3;
    u32b              rcdefense; /* this was u16b, but l_list[] uses u32b -CFT */
    monster_lore         save_mem;
    int                 breath = FALSE, magic = FALSE;
    char			sex;

    /* Access the race and lore */
    r_ptr = &r_list[r_idx];
    l_ptr = &l_list[r_idx];

    sex = r_ptr->gender;

    /* Hack -- Wizards know everything */
    if (wizard) {

	/* Save the "old" memory */
	save_mem = *l_ptr;

	/* Make assumptions about kills, etc */
	l_ptr->r_kills = MAX_SHORT;
	l_ptr->r_wake = l_ptr->r_ignore = MAX_UCHAR;

	j = (((r_ptr->cmove & CM_4D2_OBJ) != 0) * 8) +
	    (((r_ptr->cmove & CM_2D2_OBJ) != 0) * 4) +
	    (((r_ptr->cmove & CM_1D2_OBJ) != 0) * 2) +
	    ((r_ptr->cmove & CM_90_RANDOM) != 0) +
	    ((r_ptr->cmove & CM_60_RANDOM) != 0);

	l_ptr->r_cmove = (r_ptr->cmove & ~CM_TREASURE) | (j << CM_TR_SHIFT);
	l_ptr->r_cdefense = r_ptr->cdefense;
	l_ptr->r_spells = r_ptr->spells | CS_FREQ;
	l_ptr->r_spells2 = r_ptr->spells2;
	l_ptr->r_spells3 = r_ptr->spells3;
	j = 0;
	pu = r_ptr->damage;
	while (*pu != 0 && j < 4) {
	    l_ptr->r_attacks[j] = MAX_UCHAR;
	    j++;
	    pu++;
	}
    }
    roffpline = 0;
    roffp = roffbuf;
    rspells = l_ptr->r_spells & r_ptr->spells & ~CS_FREQ;
    rspells2 = l_ptr->r_spells2 & r_ptr->spells2;
    rspells3 = l_ptr->r_spells3 & r_ptr->spells3;

/* the CM_WIN property is always known, set it if a win monster */
    rcmove = l_ptr->r_cmove | (CM_WIN & r_ptr->cmove);
    rcdefense = l_ptr->r_cdefense & r_ptr->cdefense;
    if ((r_ptr->cdefense & UNIQUE) || (sex == 'p'))
	(void)sprintf(temp, "%s:\n", r_ptr->name);
    else
	(void)sprintf(temp, "The %s:\n", r_ptr->name);
    roff(temp);
/* Conflict history. */
/* changed to act better for unique monsters -CFT */
    if (r_ptr->cdefense & UNIQUE) {   /* treat unique differently... -CFT */
	if (l_ptr->r_deaths) {	   /* We've been killed... */
	    (void)sprintf(temp, "%s slain %d of your ancestors",
			  (sex == 'm' ? "He has" : sex == 'f' ? "She has" :
			   sex == 'p' ? "They have" : "It has"),
			  l_ptr->r_deaths);
	    roff(temp);

	    /* but we've also killed it */
	    if (u_list[r_idx].dead) {
		sprintf(temp, ", but you have avenged %s!  ",
			plural(l_ptr->r_deaths, "him", "them"));
		roff(temp);
	    }
	    else {
		sprintf(temp, ", who %s unavenged.  ",
			plural(l_ptr->r_deaths, "remains", "remain"));
		roff(temp);
	    }
	}

	/* Dead unique who never hurt us */
	else if (u_list[r_idx].dead) {
	    roff("You have slain this foe.  ");
	}
    }

    /* Not unique, but killed us */
    else if (l_ptr->r_deaths) {

	(void)sprintf(temp,
		      "%d of your ancestors %s",
		      l_ptr->r_deaths, plural(l_ptr->r_deaths, "has", "have"));
	roff(temp);
	roff((sex == 'p' ? " been killed by these creatures, and " :
	      " been killed by this creature, and "));

	if (l_ptr->r_kills == 0) {
	    sprintf(temp, "%s not ever known to have been defeated.  ",
		    (sex == 'm' ? "he is" : sex == 'f' ? "she is"
		     : sex == 'p' ? "they are" : "it is"));
	    roff(temp);
	}
	else {
	    (void)sprintf(temp,
			"at least %d of the beasts %s been exterminated.  ",
			  l_ptr->r_kills, plural(l_ptr->r_kills, "has", "have"));
	    roff(temp);
	}
    }

    /* Not unique, and never killed us */
    else if (l_ptr->r_kills) {
	(void)sprintf(temp, "At least %d of these creatures %s",
		      l_ptr->r_kills, plural(l_ptr->r_kills, "has", "have"));
	roff(temp);
	roff(" been killed by you and your ancestors.  ");
    }

    /* Never killed (and not unique) */
    else {
	roff("No battles to the death are recalled.  ");
    }

#if 0
    for (k = 0; k < MAX_R_IDX; k++) {
	if (!stricmp(desc_list[k].name, r_ptr->name)) {
	    if (strlen(desc_list[k].desc) != 0)
		roff(desc_list[k].desc);
	    break;
	}
    }
#endif
    k = r_idx;
    if (k == MAX_R_IDX - 1)
	roff("You feel you know it, and it knows you.  This can only mean trouble.  ");
    else {
	roff(desc_list[k].desc);
	roff("  ");
    }

    /* Describe location */

    k = FALSE;
    if (r_ptr->level == 0) {
	sprintf(temp, "%s in the town",
		(sex == 'm' ? "He lives" : sex == 'f' ? "She lives" :
		 sex == 'p' ? "They live" : "It lives"));
	roff(temp);
	k = TRUE;
    }
    else if (l_ptr->r_kills) {
	(void)sprintf(temp, "%s normally found at depths of %d feet",
		      (sex == 'm' ? "He is" : sex == 'f' ? "She is" :
		       sex == 'p' ? "They are" : "It is"),
		      r_ptr->level * 50);
	roff(temp);
	k = TRUE;
    }


    /* Extract the "speed" */
    mspeed = r_ptr->speed - 10;

    /* Describe movement, if any observed */
    if ((rcmove & CM_ALL_MV_FLAGS) || (rcmove & CM_RANDOM_MOVE)) {
	if (k) {
	    roff(", and");
	}
	else {
	    roff((sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
	    k = TRUE;
	}
	roff((sex == 'p' ? " move" : " moves"));
	if (rcmove & CM_RANDOM_MOVE) {
	    roff(desc_howmuch[(rcmove & CM_RANDOM_MOVE) >> 3]);
	    roff(" erratically");
	}
	if (mspeed == 1) {
	    roff(" at normal speed");
	}
	else {
	    if (rcmove & CM_RANDOM_MOVE) {
		roff(", and");
	    }
	    if (mspeed <= 0) {
		if (mspeed == -1) roff(" very");
		else if (mspeed < -1) roff(" incredibly");
		roff(" slowly");
	    }
	    else {
		if (mspeed == 3) roff(" very");
		else if (mspeed > 3) roff(" unbelievably");
		roff(" quickly");
	    }
	}
    }

    /* The code above includes "attack speed" */
    if (rcmove & CM_ATTACK_ONLY) {
	if (k) {
	    roff(", but");
	}
	else {
	    roff((sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
	    k = TRUE;
	}
	roff(" does not deign to chase intruders");
    }

    /* End this sentence */
    if (k) {
	roff(".  ");
    }


    /* Kill it once to know experience, and quality */
    /* (natural, evil, undead) and variety (race) */
    if (l_ptr->r_kills) {

	if (r_ptr->cdefense & UNIQUE) {
	    roff("Killing this");
	}
	else {
	    roff((sex == 'p' ? "A kill of these" : "A kill of this"));
	}

	/* Describe the "quality" */
	if (r_ptr->cdefense & ANIMAL) roff(" natural");
	if (r_ptr->cdefense & EVIL) roff(" evil");
	if (r_ptr->cdefense & UNDEAD) roff(" undead");

	if (r_ptr->cdefense & GIANT) roff(" giant");
	else if (r_ptr->cdefense & ORC) roff(" orc");
	else if (r_ptr->cdefense & DRAGON) roff(" dragon");
	else if (r_ptr->cdefense & DEMON) roff(" demon");
	else if (r_ptr->cdefense & TROLL) roff(" troll");
	else roff((sex == 'p' ? " creatures" : " creature"));

	/* calculate the integer exp part */
	i = (long)r_ptr->mexp * r_ptr->level / py.misc.lev;

	/* calculate the fractional exp part scaled by 100, */
	/* must use long arithmetic to avoid overflow  */
	j = ((((long)r_ptr->mexp * r_ptr->level % py.misc.lev) * (long)1000 /
	     py.misc.lev + 5) / 10);

	/* Mention the experience */
	(void)sprintf(temp, " is worth %lu.%02lu point%s",
		      (huge)i, (huge)j,
		      (i == 1 && j == 0 ? "" : "s"));
	roff(temp);

	/* Take account of annoying English */
	if ((py.misc.lev / 10) == 1)
	p = "th";
	else {
	i = py.misc.lev % 10;
	if (i == 1) p = "st";
	else if (i == 2) p = "nd";
	else if (i == 3) p = "rd";
	else p = "th";
	}

	i = py.misc.lev;
	if ((i == 8) || (i == 11) || (i == 18)) q = "n";
	else q = "";

	/* Mention the dependance on the player's level */
	(void)sprintf(temp, " for a%s %lu%s level character.  ",
		      q, (long)i, p);
	roff(temp);
	if (r_ptr->cdefense & GROUP) {
	    sprintf(temp, "%s usually appears in groups.  ",
		    (sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
	    roff(temp);
	}
    }
/* Spells known, if have been used against us. */
    k = TRUE;
    i = 0;

    /* First, handle (and forget!) the "breath" */
    if ((rspells & CS_BREATHE) ||
	(rspells2 & CS_BREATHE2) ||
	(rspells3 & CS_BREATHE3)) {

	/* Note that breathing has occurred */
	breath = TRUE;

	/* Process the "breath" and remove it */
	j = rspells & CS_BREATHE;
	rspells &= ~CS_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		sprintf(temp, "%s can breathe ",
			(sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
		roff(temp);
		k = FALSE;
	    }
	    else if (j || (rspells2 & CS_BREATHE2) || (rspells3 & CS_BREATHE3)) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}

	/* Process the "breath" and remove it */
	j = rspells2 & CS_BREATHE2;
	rspells2 &= ~CS_BREATHE2;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		sprintf(temp, "%s can breathe ",
			(sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
		roff(temp);
		k = FALSE;
	    }
	    else if (j || (rspells3 & CS_BREATHE3)) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i + 32],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}

	/* Process the "breath" and remove it */
	j = rspells3 & CS_BREATHE3;
	rspells &= ~CS_BREATHE3;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		sprintf(temp, "%s can breathe ",
			(sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
		roff(temp);
		k = FALSE;
	    }
	    else if (j) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    sprintf(temp, desc_spell[i + 64],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}
    }

    /* Now dump the normal spells */
    k = TRUE;

    /* Dump the normal spells */
    if (rspells || rspells2 || rspells3) {

	/* Note that spells exist */
	magic = TRUE;

	/* Describe the spells */
	j = rspells & ~CS_BREATHE;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    roff((sex == 'p' ? ", and are also" : ", and is also"));
		}
		else {
		    roff((sex == 'm' ? "He is" : sex == 'f' ? "She is" : sex == 'p' ? "They are" : "It is"));
		}
		if (l_ptr->r_cdefense & INTELLIGENT) {
		    roff(" magical, casting spells intelligently which ");
		}
		else {
		    roff(" magical, casting spells which ");
		}
		k = FALSE;
	    }
	    else if ((j & CS_SPELLS) || rspells2 || rspells3) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}

	j = rspells2 & ~CS_BREATHE2;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    roff((sex == 'p' ? ", and are also" : ", and is also"));
		}
		else {
		    roff((sex == 'm' ? "He is" : sex == 'f' ? "She is" : sex == 'p' ? "They are" : "It is"));
		}
		roff(" magical, casting spells which ");
		k = FALSE;
	    }
	    else if (j || rspells3) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i + 32],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}

	j = rspells3 & ~CS_BREATHE3;
	while ((i = bit_pos(&j)) != -1) {
	    if (k) {
		if (breath) {
		    roff((sex == 'p' ? ", and are also" : ", and is also"));
		}
		else {
		    roff((sex == 'm' ? "He is" : sex == 'f' ? "She is" : sex == 'p' ? "They are" : "It is"));
		}
		roff(" magical, casting spells which ");
		k = FALSE;
	    }
	    else if (j) {
		roff(", ");
	    }
	    else {
		roff(" or ");
	    }
	    sprintf(temp, desc_spell[i + 64],
		    (sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	    roff(temp);
	}
    }


    /* The monster has SOME form of magic */
    if (breath || magic) {

	/* XXX Could offset by level (?) */

	/* Describe the spell frequency */
	if ((l_ptr->r_spells & CS_FREQ) > 5) {
	    (void)sprintf(temp, "; 1 time in %lu", r_ptr->spells & CS_FREQ);
	    roff(temp);
	}

	/* End this sentence */
	roff(".  ");
    }


    /* Do we know how hard they are to kill? Armor class, hit die. */
    /* hasten learning of uniques -CFT */
    if (knowarmor(r_ptr->level, l_ptr->r_kills) ||
	((r_ptr->cdefense & UNIQUE) &&
	 knowuniqarmor(r_ptr->level, l_ptr->r_kills))) {

	(void)sprintf(temp, "%s an armor rating of %d",
		      (sex == 'm' ? "He has" : sex == 'f' ? "She has" : sex == 'p' ? "They have" : "It has"), r_ptr->ac);
	roff(temp);
	(void)sprintf(temp, " and a%s life rating of %dd%d.  ",
		      ((r_ptr->cdefense & MAX_HP) ? " maximized" : ""),
		      r_ptr->hd[0], r_ptr->hd[1]);
	roff(temp);
    }

    /* Do we know how clever they are? Special abilities. */
    k = TRUE;
    j = rcmove;

    /* I wonder why this wasn't here before? -CFT */
    if (rcdefense & BREAK_WALL) {
	roff((sex == 'm' ? "He can bore through rock" :
	      sex == 'f' ? "She can bore through rock" :
	      sex == 'p' ? "They can bore through rock" :
	      "It can bore through rock"));
	k = FALSE;
    }

    for (i = 0; j & CM_SPECIAL; i++) {
	if (j & (CM_INVISIBLE << i)) {
	    j &= ~(CM_INVISIBLE << i);
	    if (k) {
		roff((sex == 'm' ? "He can " : sex == 'f' ? "She can " : sex == 'p' ? "They can " : "It can "));
		k = FALSE;
	    }
	    else if (j & CM_SPECIAL) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    roff(desc_move[i]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    /* Do we know its special weaknesses? Most cdefense flags. */
    k = TRUE;
    j = rcdefense;

    if (j & HURT_LIGHT) {
	if (k) {
	    roff((sex == 'm' ? "He is susceptible to " :
		  sex == 'f' ? "She is susceptible to " :
		  sex == 'p' ? "They are susceptible to " :
		  "It is susceptible to "));
	    roff(desc_weakness[0]);
	    k = FALSE;
	}
    }

    if (j & HURT_ROCK) {
	if (k) {
	    roff((sex == 'm' ? "He is susceptible to " :
		  sex == 'f' ? "She is susceptible to " :
		  sex == 'p' ? "They are susceptible to " :
		  "It is susceptible to "));
	    roff(desc_weakness[1]);
	    k = FALSE;
	}
	else {
	    roff(" and ");
	    roff(desc_weakness[1]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    /* Do we know its special weaknesses? Most cdefense flags. */
    k = TRUE;
    for (i = 0; j & (IM_FROST | IM_FIRE | IM_ACID | IM_POISON | IM_LIGHTNING); i++) {
	if (j & (IM_FROST << i)) {
	    j &= ~(IM_FROST << i);
	    if (k) {
		roff((sex == 'm' ? "He resists " :
		      sex == 'f' ? "She resists " :
		      sex == 'p' ? "They resist " :
		      "It resists "));
		k = FALSE;
	    }
	    else if (j & (IM_FROST | IM_FIRE | IM_ACID | IM_POISON | IM_LIGHTNING)) {
		roff(", ");
	    }
	    else {
		roff(" and ");
	    }
	    roff(desc_immune[i]);
	}
    }
    if (!k) {
	roff(".  ");
    }

    if (rcdefense & NO_INFRA) {
	roff((sex == 'm' ? "He is cold blooded" :
	      sex == 'f' ? "She is cold blooded" :
	      sex == 'p' ? "They are cold blooded" :
	      "It is cold blooded"));
    }
    if (rcdefense & CHARM_SLEEP) {
	if (rcdefense & NO_INFRA) {
	    roff(", and");
	}
	else {
	    roff((sex == 'm' ? "He" : sex == 'f' ? "She" : sex == 'p' ? "They" : "It"));
	}
	roff(" cannot be charmed or slept");
    }
    if (rcdefense & (CHARM_SLEEP | NO_INFRA)) {
	roff(".  ");
    }

    /* Do we know how aware it is? */
    if (((l_ptr->r_wake * l_ptr->r_wake) > r_ptr->sleep) ||
	(l_ptr->r_ignore == MAX_UCHAR) ||
	(r_ptr->sleep == 0 && l_ptr->r_kills >= 10)) {

	roff((sex == 'm' ? "He " : sex == 'f' ? "She " : sex == 'p' ? "They " : "It "));
	if (r_ptr->sleep > 200) {
	    roff("prefers to ignore");
	}
	else if (r_ptr->sleep > 95) {
	    roff("pays very little attention to");
	}
	else if (r_ptr->sleep > 75) {
	    roff("pays little attention to");
	}
	else if (r_ptr->sleep > 45) {
	    roff("tends to overlook");
	}
	else if (r_ptr->sleep > 25) {
	    roff("takes quite a while to see");
	}
	else if (r_ptr->sleep > 10) {
	    roff("takes a while to see");
	}
	else if (r_ptr->sleep > 5) {
	    roff((sex == 'p' ? "are fairly observant of" : "is fairly observant of"));
	}
	else if (r_ptr->sleep > 3) {
	    roff((sex == 'p' ? "are observant of" : "is observant of"));
	}
	else if (r_ptr->sleep > 1) {
	    roff((sex == 'p' ? "are very observant of" : "is very observant of"));
	}
	else if (r_ptr->sleep != 0) {
	    roff((sex == 'p' ? "are vigilant for" : "is vigilant for"));
	}
	else {
	    roff((sex == 'p' ? "are ever vigilant for" : "is ever vigilant for"));
	}
	(void)sprintf(temp, " intruders, which %s may notice from %d feet.  ",
		      (sex == 'm' ? "he" : sex == 'f' ? "she" : sex == 'p' ? "they" : "it"), 10 * r_ptr->aaf);
	roff(temp);
    }


    /* Comment on what it carries, if known */
    if (rcmove & (CM_CARRY_OBJ | CM_CARRY_GOLD)) {

	roff((sex == 'm' ? "He may" : sex == 'f' ? "She may" : sex == 'p' ? "They may" : "It may"));

	j = (rcmove & CM_TREASURE) >> CM_TR_SHIFT;

	/* Only one treasure observed */
	if (j == 1) {
	    if ((r_ptr->cmove & CM_TREASURE) == CM_60_RANDOM) {
		roff(" sometimes");
	    }
	    else {
		roff(" often");
	    }
	}

	/* Only two treasures observed */
	else if ((j == 2) &&
		 ((r_ptr->cmove & CM_TREASURE) ==
		  (CM_60_RANDOM | CM_90_RANDOM))) {
	    roff(" often");
	}

	/* They have to carry it before they drop it */
	roff(" carry");


	/* Describe the treasure quality */
	if (r_ptr->cdefense & SPECIAL) {
	    p = (j==1?"n exceptional object":" exceptional objects");
	}
	else if (r_ptr->cdefense & GOOD) {
	    p = (j==1?" good object":" good objects");
	}
	else {
	    p = (j==1?"n object":" objects");
	}

	if (j == 1) {
	    p = " an object";
	}
	else if (j == 2) {
	    roff(" one or two");
	}
	else {
	    (void)sprintf(temp, " up to %lu", (long)j);
	    roff(temp);
	}

	if (rcmove & CM_CARRY_OBJ) {
	    roff(p);
	    if (rcmove & CM_CARRY_GOLD) {
		roff(" or treasure");
		if (j > 1) roff("s");
	    }
	    roff(".  ");
	} else if (j != 1)
	    roff(" treasures.  ");
	else
	    roff(" treasure.  ");
    }


    /* Count the number of "known" attacks */
    for (k = j = 0; j < 4; j++) {
	if (l_ptr->r_attacks[j]) k++;
    }

    pu = r_ptr->damage;

    /* Count the number of attacks printed so far */
    j = 0;

    /* Examine the actual attacks */    
    for (i = 0; *pu != 0 && i < 4; pu++, i++) {

	int att_type, att_how, d1, d2;

	/* Skip "unknown" attacks */
	if (!l_ptr->r_attacks[i]) continue;

	/* Extract the attack info */
	att_type = a_list[*pu].attack_type;
	att_how = a_list[*pu].attack_desc;
	d1 = a_list[*pu].attack_dice;
	d2 = a_list[*pu].attack_sides;

	/* Count the attacks as printed */
	j++;

	/* Introduce the attack description */
	if (j == 1) {
	    roff((sex == 'm' ? "He can " : sex == 'f' ? "She can " : sex == 'p' ? "They can " : "It can "));
	}
	else if (j == k) {
	    roff(", and ");
	}
	else {
	    roff(", ");
	}

	/* Hack -- only describe attacks this routine "knows" about */
	if (att_how > 23) att_how = 0;
	roff(desc_amethod[att_how]);

	/* Non-standard attacks, or attacks doing damage */
	if (att_type != 1 || (d1 > 0 && d2 > 0)) {
	    roff(" to ");

	    /* Hack -- only describe certain attacks */
	    if (att_type > 25) att_type = 0;
	    roff(desc_atype[att_type]);

	    /* Does it do damage? */
	    if (d1 && d2) {

		/* Hack -- do we KNOW the damage? */
		if (knowdamage(r_ptr->level, l_ptr->r_attacks[i], (int)d1 * (int)d2) ||
		    ((r_ptr->cdefense & UNIQUE) &&
		     knowuniqdamage(r_ptr->level, l_ptr->r_attacks[i], (int)d1 * (int)d2))) {

		    /* Hack -- Loss of experience */
		    if (att_type == 19) {
			roff(" by");
		    }

		    /* Normal attacks */
		    else {
			roff(" with damage");
		    }

		    /* Display the damage */
		    (void)sprintf(temp, " %dd%d", d1, d2);
		    roff(temp);
		}
	    }
	}
    }

    /* Finish sentence above */
    if (j) {
	roff(".");
    }

    else if (k > 0 && l_ptr->r_attacks[0] >= 10) {
	sprintf(temp, " %s no physical attacks.",
		(sex == 'm' ? "He has" : sex == 'f' ? "She has" : sex == 'p' ? "They have" : "It has"));
	roff(temp);
    }

    /* Or describe the lack of knowledge */
    else {
	sprintf(temp, "Nothing is known about %s attack.",
		(sex == 'm' ? "his" : sex == 'f' ? "her" : sex == 'p' ? "their" : "its"));
	roff(temp);
    }



    /* XXX Hack -- Always know the win creature. */
    if (r_ptr->cmove & CM_WIN) {
	roff("  Killing him wins the game!");
    }

    roff("\n");

    /* Prompt for pause */
    prt("   --pause--", roffpline, 0);

    /* Hack -- Undo the "wizard memory" */
    if (wizard) *l_ptr = save_mem;

    return inkey();
}

/* Print out strings, filling up lines as we go. */
static void roff(register const char *p)
{
    register char *q, *r;
    register int   linesize;

    linesize = sizeof(roffbuf);
    if (linesize > 80)
	linesize = 80;

    while (*p) {
	*roffp = *p;
	if (*p == '\n' || roffp >= roffbuf + linesize) {
	    q = roffp;
	    if (*p != '\n') {
		if (*q == ' ')
		    q--;
		if (*q == ' ')
		    q--;
		while (*q != ' ')
		    q--;
	    }
	    *q = 0;
	    prt(roffbuf, roffpline, 0);
	    roffpline++;
	    r = roffbuf;
	    while (q < roffp)
		*r++ = *++q;
	    roffp = r;
	} else
	    roffp++;
	p++;
    }
}

