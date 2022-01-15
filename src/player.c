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


