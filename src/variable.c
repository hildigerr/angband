/* File: variable.c */

/* Purpose: Global variables */

#include "angband.h"


/* Link a copyright message into the executable */
cptr copyright[5] = {
    "Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
    "",
    "This software may be copied and distributed for educational, research,",
    "and not for profit purposes provided that this copyright and statement",
    "are included in all such copies."
};

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int hack_m_idx = (-1);		/* XXX */


int cur_lite = 0;		/* Current light radius (zero for none) */
int old_lite;

s16b total_winner = FALSE;	/* Semi-Hack -- Game has been won */

int character_generated = 0;	/* A character has been generated */
int character_saved = 0;	/* The character has been saved */


int LOAD = 0;


u32b randes_seed;		/* Hack -- consistent object colors */
u32b town_seed;		/* Hack -- consistent town layout */

char last_command = ' ';  	/* Memory of previous command. */
int command_rep;		/* Gives repetition of commands. -CJS- */
int default_dir = FALSE;	/* Use last direction for repeated command */

int death = FALSE;		/* True if player has died */
int free_turn_flag;		/* Command is "free", do not move creatures */
int find_flag;			/* Number of turns spent running */

int msg_flag;			/* Used in msg_print() for "buffering" */
vtype old_msg[MAX_SAVE_MSG];	/* Last message	      */
s16b last_msg = 0;		/* Where last is held */

s16b cur_height;		/* Cur dungeon height */
s16b cur_width;			/* Cur dungeon width */
s16b dun_level = 0;		/* Cur dungeon level */
int object_level = 0;		/* level for objects to be created -CWS  */

s32b turn = (-1);			/* Cur turn of game    */
s32b old_turn = (-1);		/* Last feeling message */

int wizard = FALSE;		/* Is the player currently in Wizard mode? */
int to_be_wizard = FALSE;	/* Is the player about to be a wizard? */
int can_be_wizard = FALSE;	/* Does the player have wizard permissions? */

s16b panic_save = FALSE;	/* this is true if playing from a panic save */
s16b noscore = FALSE;		/* Don't log the game. -CJS- */

int in_store_flag = FALSE;	/* Don't redisplay light in stores -DGK */

int coin_type;		/* remember Creeping _xxx_ coin type -CWS */
int opening_chest = 0;          /* Hack -- prevent chest generation */


s16b missile_ctr = 0;		/* Counter for missiles */
int weapon_heavy = FALSE;
int pack_heavy = FALSE;


int rogue_like_commands;	/* set in config.h/main.c */

/* OPTION: options set via the '=' command */
/* note that the values set here will be the default settings */

int quick_messages = FALSE;		/* Quick messages -CWS */
int prompt_carry_flag = FALSE;		/* Require "g" key to pick up */
int carry_query_flag = TRUE;		/* Prompt for pickup */

int equippy_chars = FALSE;	/* do equipment characters -CWS */
int notice_seams = FALSE;	/* Highlight mineral seams */

int find_cut = TRUE;		/* Cut corners */
int find_examine = TRUE;	/* Examine corners */
int find_prself = FALSE;		/* Print self */
int find_bound = FALSE;		/* Stop on borders */
int find_ignore_doors = FALSE;	/* Run through doors */

int no_haggle_flag = FALSE;	/* does the player not want to haggle? -CWS */

int show_inven_weight = FALSE;	/* Show weights in inven */
int show_equip_weight = FALSE;	/* Show weights in equip */
int plain_descriptions = FALSE;	/* Plain descriptions */





int hitpoint_warn = 1;		/* Hitpoint warning (0 to 9) */
int delay_spd = 5;

int peek = FALSE;		/* Let user "see" internal stuff */

int feeling = 0;		/* Most recent feeling */
int rating = 0;			/* Level's current rating */

char doing_inven = FALSE;	/* Hack -- track inventory commands */
int screen_change = FALSE;	/* Track screen updates for inven_commands. */

int new_level_flag;		/* Start a new level */
int teleport_flag;		/* Hack -- handle teleport traps */

int eof_flag = FALSE;		/* Used to signal EOF/HANGUP condition */
int light_flag = FALSE;		/* Track if temporary light about player.  */
int wait_for_more = FALSE;	/* used when ^C hit during -more- prompt */
int closing_flag = FALSE;	/* Dungeon is closing */

/*  Following are calculated from max dungeon sizes		*/
s16b max_panel_rows, max_panel_cols;
int panel_row, panel_col;
int panel_row_min, panel_row_max;
int panel_col_min, panel_col_max;
int panel_col_prt, panel_row_prt;

/* Player location in dungeon */
s16b char_row;
s16b char_col;

#ifdef TARGET
/* Targetting information, this code stolen from Morgul -CFT */
int target_mode = FALSE;
u16b target_col;
u16b target_row;
u16b target_mon;
#endif




/* Current player's user id */
int player_uid = 0;

/* What killed the player */
vtype died_from;

/* The savefile to use. */
vtype savefile;

#ifdef MAC
cave_type (*cave)[MAX_WIDTH];
#else
cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

/* The array of dungeon monsters [MAX_M_IDX] */
monster_type m_list[MAX_M_IDX];

#ifdef MAC
monster_lore *c_recall;
#else
monster_lore c_recall[MAX_R_IDX];	/* Monster memories */
#endif

struct unique_mon u_list[MAX_R_IDX]; /* Unique check list... -LVB- */ 



static player_type py;	/* Static player info record */

u32b spell_learned = 0;       /* bit mask of spells learned */
u32b spell_learned2 = 0;      /* bit mask of spells learned */
u32b spell_worked = 0;        /* bit mask of spells tried and worked */
u32b spell_worked2 = 0;       /* bit mask of spells tried and worked */
u32b spell_forgotten = 0;     /* bit mask of spells learned but forgotten */
u32b spell_forgotten2 = 0;    /* bit mask of spells learned but forgotten */
byte spell_order[64];          /* order spells learned/remembered/forgotten */

/*
 * Calculated base hp values for player at each level,
 * store them so that drain life + restore life does not
 * affect hit points.
 */
u16b player_hp[MAX_PLAYER_LEVEL];

