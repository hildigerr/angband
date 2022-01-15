/* player.c: player specific variable definitions

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "angband.h"

/* Player record for most player related info */
player_type py;
/* player location in dungeon */
s16b char_row;
s16b char_col;
/* calculated base hp values for player at each level, store them so that
   drain life + restore life does not affect hit points */
u16b player_hp[MAX_PLAYER_LEVEL];


/* 5 char race for printing scores. */
#if 0 /* not used */
const char *dsp_race[MAX_RACES] = {
  "Human",
  "H-Elf",
  "Elf  ",
  "Hobbt",
  "Gnome",
  "Dwarf",
  "H-Orc",
  "H-Tro",
  "Duned",
  "HiElf"
};
#endif

u32b spell_learned = 0;	/* bit mask of spells learned */
u32b spell_learned2 = 0;	/* bit mask of spells learned */
u32b spell_worked = 0;	/* bit mask of spells tried and worked */
u32b spell_worked2 = 0;	/* bit mask of spells tried and worked */
u32b spell_forgotten = 0;	/* bit mask of spells learned but forgotten */
u32b spell_forgotten2 = 0;	/* bit mask of spells learned but forgotten */
byte spell_order[64];		/* order spells learned/remembered/forgotten */

#define MDO MAX_DUNGEON_OBJ
/* Each type of character starts out with a few provisions.	*/
/* Note that the entries refer to elements of the k_list[] array*/
/* 356 = Food Ration, 365 = Wooden Torch, 123 = Cloak, 30 = Stiletto,
   103 = Soft Leather Armor, 318 = Beginners-Magic, 322 = Beginners Handbook */

u16b player_init[MAX_CLASS][5] = {
		{ MDO, MDO+21,  34, 109, 258},	/* Warrior	 */
		{ MDO, MDO+21,  29, 330, 220},	/* Mage		 */
		{ MDO, MDO+21,  53, 334, 242},	/* Priest	 */
		{ MDO, MDO+21,  46, 103, 330},	/* Rogue	 */
		{ MDO, MDO+21,  34, 330,  74},	/* Ranger	 */
		{ MDO, MDO+21,  34, 334, 209}	/* Paladin	 */
        /* Last array object added for one extra useful object per class */
};

