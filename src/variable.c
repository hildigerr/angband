/* variable.c: Global variables */

#include "angband.h"
#include "config.h"
#include "types.h"

const char *copyright[5] = {
"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
"",
"This software may be copied and distributed for educational, research, and",
"not for profit purposes provided that this copyright and statement are",
"included in all such copies."};

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int hack_monptr = (-1);

int weapon_heavy = FALSE;
int pack_heavy = FALSE;
s16b log_index = (-1);		/* Index to log file. (<= 0 means no log) */
vtype died_from;

vtype savefile;			/* The savefile to use. */

int cur_lite = 0, old_lite;				/* light radius spoo */
s16b total_winner = FALSE;
int NO_SAVE=FALSE;
int character_generated = 0;	/* don't save score until char gen finished */
int character_saved = 0;	/* prevents save on kill after save_player() */
int highscore_fd;		/* File descriptor to high score file */
int LOAD=0;
u32b randes_seed;		/* for restarting randes_state */
u32b town_seed;		/* for restarting town_seed */
char *old_state = NULL;         /* state array initialized by time -CWS */
char *dummy_state = NULL;       /* dummy state array -CWS */

s16b cur_height,cur_width;	/* Cur dungeon size    */
s16b dun_level = 0;		/* Cur dungeon level   */
int object_level = 0;		/* level for objects to be created -CWS  */
s16b missile_ctr = 0;		/* Counter for missiles */
int msg_flag;			/* Set with first msg  */
vtype old_msg[MAX_SAVE_MSG];	/* Last message	      */
s16b last_msg = 0;		/* Where last is held */
int death = FALSE;		/* True if died	      */
int find_flag;			/* Used in ANGBAND for .(dir) */
int free_turn_flag;		/* Used in ANGBAND, do not move creatures  */
int command_count;		/* Gives repetition of commands. -CJS- */
int default_dir = FALSE;	/* Use last direction for repeated command */
s32b turn = (-1);		/* Cur turn of game    */
s32b old_turn = (-1);		/* Last feeling message */
int wizard = FALSE;		/* Wizard flag	      */
int to_be_wizard = FALSE;	/* used during startup, when -w option used */
s16b panic_save = FALSE;	/* this is true if playing from a panic save */
s16b noscore = FALSE;		/* Don't log the game. -CJS- */
int is_home = FALSE;		/* Are we in our home? */
int in_store_flag = FALSE;	/* Don't redisplay light in stores -DGK */
int monster_is_afraid = 0;      /* redo monster fear messages -CWS */
int coin_type;			/* remember Creeping _xxx_ coin type -CWS */
int opening_chest = 0;          /* don't generate another chest -CWS */

struct unique_mon u_list[MAX_R_IDX]; /* Unique check list... -LVB- */ 

int rogue_like_commands;	/* set in config.h/main.c */

/* options set via the '=' command */
/* note that the values set here will be the default settings */
int find_cut = TRUE;
int find_examine = TRUE;
int find_bound = FALSE;
int find_prself = FALSE;
int prompt_carry_flag = FALSE;
int show_inven_weight = FALSE;
int show_equip_weight = FALSE;
int notice_seams = FALSE;
int find_ignore_doors = FALSE;
int hitpoint_warn = 1;
int feeling = 0;
int carry_query_flag = TRUE;
int delay_spd = 5;
int plain_descriptions = FALSE;
int no_haggle_flag = FALSE;	/* does the player not want to haggle? -CWS */
int quick_messages = FALSE;     /* quick messages -CWS */
int equippy_chars = FALSE;      /* do equipment characters -CWS */
char doing_inven = FALSE;	/* Track inventory commands. -CJS- */
int screen_change = FALSE;	/* Track screen updates for inven_commands. */
char last_command = ' ';  	/* Memory of previous command. */

/* these used to be in dungeon.c */
int new_level_flag;		/* Next level when true	 */
int teleport_flag;		/* Handle teleport traps  */
int player_light;		/* Player carrying light */
int eof_flag = FALSE;		/* Used to signal EOF/HANGUP condition */
int light_flag = FALSE;		/* Track if temporary light about player.  */

int wait_for_more = FALSE;	/* used when ^C hit during -more- prompt */
int closing_flag = FALSE;	/* Used for closing   */

/*  Following are calculated from max dungeon sizes		*/
s16b max_panel_rows,max_panel_cols;
int panel_row,panel_col;
int panel_row_min,panel_row_max;
int panel_col_min,panel_col_max;
int panel_col_prt,panel_row_prt;

#ifdef TARGET
/* Targetting information, this code stolen from Morgul -CFT */
int target_mode = FALSE;
u16b target_col;
u16b target_row;
u16b target_mon;
#endif

#ifdef MAC
cave_type (*cave)[MAX_WIDTH];
#else
cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

#ifdef MAC
monster_lore *c_recall;
#else
monster_lore c_recall[MAX_R_IDX];	/* Monster memories */
#endif
