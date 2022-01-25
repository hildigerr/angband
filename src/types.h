/* File: types.h */

/* Purpose: global type declarations */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * This file should ONLY be included by "angband.h"
 */


/*
 * some machines will not accept 'signed char' as a type, and some accept it
 * but still treat it like an unsigned character, let's just avoid it,
 * any variable which can ever hold a negative value must be 16 or 32 bits 
 */


/*
 * Some simple buffers (overkill)
 */

#define VTYPESIZ    160
#define BIGVTYPESIZ 300

typedef char vtype[VTYPESIZ];
typedef char bigvtype[BIGVTYPESIZ];



/*
 * Note that fixed length character fields allow pure "structure copy",
 * (which is technically bad anyway), but take up a LOT of space.  The
 * only one left is "inscrip", and it would be nice to remove it.
 *
 * all fields are given the smallest possible type, and all fields are
 * aligned within the structure to their natural size boundary, so that
 * the structures contain no padding and are minimum size.  In theory...
 *
 * Note that bit fields are extremely inefficient, and are thus used
 * only for "cave_type", where they provide a major savings in space.
 * Actually, this is not really true, perhaps we should eliminate them.
 */


/*
 * Monster attack and damage types
 * Field order fixed by use in tables.c
 */
typedef struct _monster_attack monster_attack;

struct _monster_attack {
    byte attack_type;		    
    byte attack_desc;
    byte attack_dice;
    byte attack_sides;
};


/*
 * Monster "variety" or "race"
 *
 * Should attempt to "move" the monster attack info into the race.
 * This would allow the attacks to be much more "varied"...
 */

typedef struct _monster_race monster_race;

struct _monster_race {
  cptr name;			/* Descrip of creature    */

  u16b level;			/* Level of creature		*/
  byte rarity;			/* Rarity of creature		*/
  char gender;			/* one of 'm','f','n','p' to genderize monsters -CWS */
  byte r_char;			/* Racial "symbol"		*/

  byte hd[2];			/* Creatures hit die		*/
  u16b ac;			/* Armour Class			*/

  u16b sleep;			/* Inactive counter/10		*/
  byte aaf;			/* Area affect radius		*/
  byte speed;			/* Speed (+10)		*/

  u32b mexp;			/* Exp value for kill		*/

  u16b damage[4];		/* Type attack and damage	*/

  u32b cflags1;		/* Flags 1 (movement)		*/
  u32b cflags2;		/* Flags 2 (defense)		*/

  u32b spells1;		/* Spell flags 1		*/
  u32b spells2;		/* Spell flags 2		*/
  u32b spells3;		/* Spell flags 3		*/
};



/*
 * Monster memories (or "lore").
 *
 * Note that there is really no reason for "monster_lore" not to
 * be part of "monster_race" except that it separates the things
 * that get written to the save-file from those that do not.
 */

typedef struct _monster_lore monster_lore;

struct _monster_lore {

    u32b r_cflags1;
    u32b r_cflags2;
    u32b r_spells1;
    u32b r_spells2;
    u32b r_spells3;

    u16b r_kills;		/* Count player killing monster */
    u16b r_deaths;		/* Count monster killing player */

    byte r_attacks[MAX_MON_NATTACK];

    byte r_wake;		/* Number of times woken up (?) */
    byte r_ignore;		/* Number of times ignored (?) */

};


struct unique_mon {
  s32b exist;
  s32b dead;
};




/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 */

typedef struct _monster_type monster_type;

struct _monster_type {

  u16b r_idx;			/* Monster race index		*/

  byte fy;			/* Y location on map		*/
  byte fx;			/* X location on map		*/

  s16b hp;			/* Current Hit points		*/
  s16b maxhp;			/* Max Hit points		*/

  s16b csleep;			/* Inactive counter		*/

  s16b mspeed;			/* Monster "speed"		*/
  
  byte stunned;		/* Monster is stunned		*/
  byte confused;		/* Monster is confused		*/
  byte monfear;		/* Monster is afraid		*/

  byte cdis;			/* Current dis from player	*/

  byte ml;			/* Monster is "visible"		*/
};



/*
 * The "kind" of an object (or treasure).
 *
 * Note that "name" is dynamically allocated.
 * Note that "name" must be at the end.
 */

typedef struct _inven_kind inven_kind;

struct _inven_kind {

  cptr name;			/* Name				*/

  byte tval;			/* Object type			*/
  byte sval;			/* Object sub type		*/
  s16b pval;			/* Object extra info		*/

  byte level;			/* Object level			*/
  byte number;			/* Number of items	*/
  byte rare;			/* True if Rare */
  byte tchar;			/* Character representation		*/

  s16b tohit;			/* Plusses to hit		*/
  s16b todam;			/* Plusses to damage		*/
  s16b toac;			/* Plusses to AC		*/
  s16b ac;			/* Normal AC			*/
  byte damage[2];			/* Damage when hits		*/
  u16b weight;			/* Weight			*/

  s32b cost;			/* Object "base cost"		*/

  u32b flags1;		/* Flags, set 1			*/
  u32b flags2;		/* Flags, set 2			*/
};


/*
 * Structure for an object.
 *
 * only damage, ac, and tchar are constant; level could possibly be made
   constant by changing index instead; all are used rarely
 *
 * extra fields x and y for location in dungeon would simplify pusht()
 *
 * Making inscrip a pointer and mallocing space does not work, there are
 * too many places where inven_types are copied, which results in dangling
 * pointers, so we use a char array for them instead.  We could always
 * attempt to remove this dependency on copying structures... :-)
 */

#define INSCRIP_SIZE 13	 /* notice alignment, must be 4*x + 1 */

typedef struct _inven_type inven_type;

struct _inven_type {

  u16b index;			/* Kind index (in k_list)	*/

  byte tval;			/* Category number		*/
  byte sval;			/* Sub-category number		*/
  s16b pval;			/* Misc. use variable		*/

  u16b timeout;		/* Timeout counter		*/
  byte name2;			/* Special type, if any		*/
  byte ident;			/* Identification info		*/
  byte number;			/* Number of items		*/
  u16b weight;			/* Weight			*/

  s16b tohit;			/* Plusses to hit		*/
  s16b todam;			/* Plusses to damage		*/
  s16b toac;			/* Plusses to AC		*/
  s16b ac;			/* Normal AC			*/
  byte damage[2];		/* Damage when hits		*/

  s32b cost;			/* Cost of item			*/

  u32b flags1;		/* Flags, set 1			*/
  u32b flags2;		/* Flags, set 2			*/

  byte level;			/* Level item first found	*/
  byte tchar;			/* Character representation */

  char inscrip[INSCRIP_SIZE];	/* Object inscription		*/
};






/*
 * spell name is stored in spell_names[] array
 * at index i if "mage", Or at index i + 31 if "priest"
 *
 * We should probably change the whole "sexp" method, so that
 * the "spell experience" is taken from slevel, smana, and sfail.
 */

typedef struct _spell_type spell_type;

struct _spell_type {
  byte slevel;		/* Required level */
  byte smana;		/* Required mana */
  byte sfail;		/* Minimum chance of failure */
  u16b sexp;		/* 1/4 of exp gained for learning spell */
};



/*
 * A single "grid" in a Cave
 */

typedef struct _cave_type cave_type;

struct _cave_type {

  u16b m_idx;		/* Monster index (in m_list) */
  u16b i_idx;		/* Item index (in i_list) */

  byte fval;		/* Grid type (0-15) */

  unsigned int lr : 1;		    /* room should be lit with perm light, walls with
				       this set should be perm lit after tunneled out	 */

  unsigned int fm : 1;		    /* field mark, used for traps/doors/stairs, object is
				       hidden if fm is FALSE				 */

  unsigned int pl : 1;		    /* permanent light, used for walls and lighted rooms */
  unsigned int tl : 1;		    /* temporary light, used for player's lamp light,etc.*/

};


/*
 * A store owner
 */

typedef struct _owner_type owner_type;

struct _owner_type {
  cptr owner_name;
  s16b max_cost;
  byte max_inflate;
  byte min_inflate;
  byte haggle_per;
  byte owner_race;
  byte insult_max;
};



typedef struct inven_record
{
  s32b scost;
  inven_type sitem;
} inven_record;



/*
 * A store.  Now holds some items, which themselves hold their store cost.
 */

typedef struct _store_type store_type;

struct _store_type {

  s32b store_open;

  s16b insult_cur;
  byte owner;
  byte store_ctr;

  u16b good_buy;
  u16b bad_buy;

  inven_record store_inven[STORE_INVEN_MAX];
};



typedef struct _player_race player_race;

struct _player_race {

  cptr trace;			    /* Type of race	    */

  s16b str_adj;
  s16b int_adj;		    
  s16b wis_adj;
  s16b dex_adj;
  s16b con_adj;
  s16b chr_adj;

  byte b_age;			    /* Base age of character	 */
  byte m_age;			    /* Maximum age of character	 */
  byte m_b_ht;			    /* base height for males	 */
  byte m_m_ht;			    /* mod height for males	 */
  byte m_b_wt;			    /* base weight for males	 */
  byte m_m_wt;			    /* mod weight for males	 */
  byte f_b_ht;			    /* base height females	 */
  byte f_m_ht;			    /* mod height for females	 */
  byte f_b_wt;			    /* base weight for female	 */
  byte f_m_wt;			    /* mod weight for females	 */

  s16b b_dis;			    /* base chance to disarm	 */
  s16b srh;			    /* base chance for search	 */
  s16b stl;			    /* Stealth of character	 */
  s16b fos;			    /* frequency of auto search	 */
  s16b bth;			    /* adj base chance to hit	 */
  s16b bthb;			    /* adj base to hit with bows */
  s16b bsav;			    /* Race base for saving throw*/

  byte bhitdie;			/* Base hit points for race	 */
  byte infra;			/* See infra-red		 */
  byte b_exp;			/* Base experience factor	 */
  byte rtclass;			/* Bit field for class types */
};

typedef struct _player_class player_class;

struct _player_class {

  cptr title;			    /* type of class		       */

  byte adj_hd;			    /* Adjust hit points	       */
  byte mdis;			    /* mod disarming traps	       */
  byte msrh;			    /* modifier to searching	       */
  byte mstl;			    /* modifier to stealth	       */
  byte mfos;			    /* modifier to freq-of-search      */
  byte mbth;			    /* modifier to base to hit	       */
  byte mbthb;			    /* modifier to base to hit - bows  */
  byte msav;			    /* Class modifier to save	       */

  s16b madj_str;		    /* Class modifier for strength     */
  s16b madj_int;		    /* Class modifier for intelligence */
  s16b madj_wis;		    /* Class modifier for wisdom       */
  s16b madj_dex;		    /* Class modifier for dexterity    */
  s16b madj_con;		    /* Class modifier for constitution */
  s16b madj_chr;		    /* Class modifier for charisma     */

  byte spell;			/* class use mage spells		*/
  byte m_exp;			/* Class experience factor		*/
  byte first_spell_lev;		/* First level spells usable		*/
  byte age_adj;			/* age percentage (warrior = 100)	*/
};


/*
 * Player background information
 */

typedef struct _player_background player_background;

struct _player_background {

  cptr info;			    /* History information	    */

  byte roll;			    /* Die roll needed for history  */
  byte chart;			    /* Table number		    */
  byte next;			    /* Pointer to next table	    */
  byte bonus;			    /* Bonus to the Social Class+50 */
};




/*
 * Most of the "player" information goes here.
 *
 * Basically, this stucture gives us a large collection of global
 * variables, which can all be wiped to zero at creation time.
 *
 */

typedef struct _player_type player_type;

struct _player_type {

      char name[27];		    /* Character name	*/

  byte prace;			/* # of race	*/
  byte pclass;			/* # of class	*/
  byte male;			/* Sex of character */
  byte hitdie;			/* Char hit die	*/
  byte expfact;			/* Experience factor	*/

  u16b age;			/* Characters age	*/
  u16b ht;			/* Height		*/
  u16b wt;			/* Weight		*/

  s32b au;			/* Gold		*/

  s32b max_exp;			/* Max experience	*/
  s32b exp;			/* Cur experience	*/
  u16b exp_frac;		/* Cur exp fraction * 2^16	*/

  u16b lev;			/* Level		*/

  s16b mana;			/* Mana points	*/
  s16b cmana;			/* Cur mana pts		*/
  u16b cmana_frac;		/* Cur mana fraction * 2^16 */

  s16b mhp;			/* Max hit pts	*/
  s16b chp;			/* Cur hit pts		*/
  u16b chp_frac;		/* Cur hit fraction * 2^16	*/

  u16b max_dlv;			/* Max level explored	*/

  s16b sc;			/* Social Class	*/
  s16b stl;			/* Stealth factor	*/
  s16b srh;			/* Chance in search */
  s16b fos;			/* Frenq of search	*/
  s16b disarm;			/* % to Disarm	*/
  s16b save;			/* Saving throw	*/

  s16b bth;			/* Base to hit	*/
  s16b bthb;			/* BTH with bows	*/

  s16b pac;			/* Total AC		*/
  s16b ptoac;			/* Magical AC	*/
  s16b ptohit;			/* Plusses to hit	*/
  s16b ptodam;			/* Plusses to dam	*/

  s16b dis_th;			/* Display +ToHit	*/
  s16b dis_td;			/* Display +ToDam	*/
  s16b dis_ac;			/* Display +ToAC	*/
  s16b dis_tac;		/* Display +ToTAC	*/

      char history[4][60];	    /* History record		*/

  u16b max_stat[6];		/* Current "maximal" stat values */
  byte cur_stat[6];		/* Current "natural" stat values */
  s16b mod_stat[6];		/* Current "stat modifiers" */
  u16b use_stat[6];		/* Current "resulting" stat values */

  u32b status;		/* Status of player	   */

  s16b speed;			/* Current speed adjust	   */
  
  s16b food;			/* Current nutrition	*/
  s16b food_digested;		/* Food per round	*/

  s16b rest;			/* Rest counter		*/
  s16b blind;			/* Blindness counter	*/
  s16b paralysis;		/* Paralysis counter	*/
  s16b confused;		/* Confusion counter	*/
  s16b protection;		/* Protection fr. evil	*/
  s16b fast;			/* Temp speed change	*/
  s16b slow;			/* Temp speed change	*/
  s16b afraid;			/* Fear			*/
  s16b cut;			/* Wounds		*/
  s16b stun;			/* Stunned player	*/
  s16b poisoned;		/* Poisoned		*/
  s16b image;			/* Hallucinate		*/

  s16b protevil;		/* Protect VS evil	   */
  s16b invuln;			/* Increases AC	   */
  s16b hero;			/* Heroism		   */
  s16b shero;			/* Super Heroism	   */
  s16b shield;			/* Shield Spell	   */
  s16b blessed;		/* Blessed		   */
  s16b detect_inv;		/* Timed see invisible */
  s16b word_recall;		/* Timed teleport level*/
  s16b see_infra;		/* See warm creatures  */
  s16b tim_infra;		/* Timed infra vision  */

  s16b oppose_acid;		/* Timed acid resist   */
  s16b oppose_elec;		/* Timed lightning resist  */
  s16b oppose_fire;		/* Timed heat resist   */
  s16b oppose_cold;		/* Timed cold resist   */
  s16b oppose_pois;		/* Timed poison resist */

  byte immune_acid;		/* Immune to acid	   */
  byte immune_elec;		/* Immune to lightning     */
  byte immune_fire;		/* Immune to fire	   */
  byte immune_cold;		/* Immune to cold	   */
  byte immune_pois;		/* Immune to poison	   */

  byte resist_acid;		/* Resistance to acid  */
  byte resist_elec;		/* Resistance to lightning */
  byte resist_fire;		/* Resistance to fire  */
  byte resist_cold;		/* Resistance to cold  */
  byte resist_pois;		/* Resistance to poison	   */

  byte resist_conf;		/* Resist confusion	*/
  byte resist_sound;		/* Resist sound		*/
  byte resist_lite;		/* Resist light		*/
  byte resist_dark;		/* Resist darkness	*/
  byte resist_chaos;		/* Resist chaos		*/
  byte resist_disen;		/* Resist disenchant	*/
  byte resist_shards;		/* Resist shards	*/
  byte resist_nexus;		/* Resist nexus		*/
  byte resist_blind;		/* Resist blindness	*/
  byte resist_nether;		/* Resist nether	*/
  byte resist_fear;		/* Resist fear		*/

  byte sustain_str;		/* Keep strength	*/
  byte sustain_int;		/* Keep intelligence	*/
  byte sustain_wis;		/* Keep wisdom		*/
  byte sustain_dex;		/* Keep dexterity	*/
  byte sustain_con;		/* Keep constitution	*/
  byte sustain_chr;		/* Keep charisma	*/

  byte aggravate;		/* Aggravate monsters	*/
  byte teleport;		/* Random teleporting	*/

  byte ffall;			/* No damage falling	*/
  byte light;			/* Permanent light	*/
  byte free_act;		/* Never paralyzed	*/
  byte see_inv;			/* Can see invisible	*/
  byte regenerate;		/* Regenerate hit pts	*/
  byte hold_life;		/* Immune to drain-life	*/
  byte telepathy;		/* Has telepathy	*/
  byte slow_digest;		/* Lower food needs	*/
  byte confuse_monster;		/* Glowing hands.	*/
  byte new_spells;		/* Number of spells can learn. */
};


