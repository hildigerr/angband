/* externs.h: declarations for global variables and initialized data

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/*****************************************************************************/


/*
 * Here's some functions that've been macroized rather than being called
 * from everywhere.  They're short enough so that inlining them will probably
 * result in a smaller executable, and speed things up, to boot. -CWS
 */


/*
 * Simple integer math functions
 * Should just use the real ones.
 */
#define MY_MAX	MAX
#define MY_MIN	MIN
#define MY_ABS	ABS
#define MY_POM	POM


/*
 * Determines if a map location is fully inside the outer walls
 */
#define in_bounds(y, x) \
   ((((y) > 0) && ((x) > 0) && ((y) < cur_height-1) && ((x) < cur_width-1)) ? \
    (TRUE) : (FALSE))

/*
 * Determines if a map location is on or inside the outer walls
 */
#define in_bounds2(y, x) \
   ((((y) >= 0) && ((x) >= 0) && ((y) < cur_height) && ((x) < cur_width)) ? \
    (TRUE) : (FALSE))


/*
 * Determines if a map location is currently "on screen" -RAK-
 * Note that "panel_contains(y,x)" always implies "in_bounds2(y,x)".
 */
#define panel_contains(y, x) \
  ((((y) >= panel_row_min) && ((y) <= panel_row_max) && \
    ((x) >= panel_col_min) && ((x) <= panel_col_max)) ? (TRUE) : (FALSE))


/*
 * Generates a random integer X where 1<=X<=MAXVAL	-RAK-
 */
#define randint(maxval) (((maxval) < 1) ? (1) : ((random() % (maxval)) + 1))




/*****************************************************************************/


#define get_Yn get_check


/* many systems don't define these anywhere */
#if defined(USG) || defined(DGUX) || defined(atarist)
#if !(defined(HPUX) || defined(linux))
extern int sprintf();
#endif
#else
#if !(defined(MSDOS) || defined(NeXT) || defined(ultrix) || defined(linux) \
|| defined(__386BSD__) || defined(SOLARIS))
extern char *sprintf();
#endif
#endif

#ifndef MSDOS
extern int errno;
#endif

extern char *copyright[5];

extern int player_uid;

/* horrible hack: needed because compact_monster() can be called from deep
   within creatures() via place_monster() and summon_monster() */
extern int hack_monptr;

extern vtype died_from;
extern vtype savefile;			/* The save file. -CJS- */

/* These are options, set with do_cmd_options command -CJS- */
extern int rogue_like_commands;
extern int find_cut;			/* Cut corners on a run */
extern int find_examine;		/* Check corners on a run */
extern int find_prself;			/* Print yourself on a run (slower) */
extern int find_bound;			/* Stop run when the map shifts */
extern int prompt_carry_flag;	/* auto-pickupobjects */
extern int show_inven_weight;	/* Display weights in inventory */
extern int show_equip_weight;	/* Display weights in equip list -CWS */
extern int notice_seams;		/* Highlight magma and quartz */
extern int find_ignore_doors;	/* Run through open doors */
extern int delay_spd;			/* 1-10 for delays */
extern int hitpoint_warn;		/* Low hitpoint warning */
extern int peek;				/* should we display additional msgs */
extern int carry_query_flag;	/* ask whether to pick something up */
extern int is_home;				/* are we in our home? */
extern int in_store_flag;		/* Don't redisplay light in stores -DGK */
extern int plain_descriptions;	/* don't add color to any obj -CWS */
extern int no_haggle_flag;		/* does the player have to haggle? -CWS */
extern int quick_messages;		/* do quick messages -CWS */
extern int equippy_chars;		/* do equipment characters -CWS */
extern int coin_type;			/* remember Creeping _xxx_ coin type -CWS */
extern int opening_chest;		/* do not generate another chest -CWS */

/* Unique artifact weapon flags */
extern s32b GROND, RINGIL, AEGLOS, ARUNRUTH, MORMEGIL, ANGRIST, GURTHANG,
  CALRIS, ANDURIL, STING, ORCRIST, GLAMDRING, DURIN, AULE, THUNDERFIST,
  BLOODSPIKE, DOOMCALLER, NARTHANC, NIMTHANC, DETHANC, GILETTAR, RILIA,
  BELANGIL, BALLI, LOTHARANG, FIRESTAR, ERIRIL, CUBRAGOL, BARD, COLLUIN,
  HOLCOLLETH, TOTILA, PAIN, ELVAGIL, AGLARANG, EORLINGAS, BARUKKHELED,
  WRATH, HARADEKKET, MUNDWINE, GONDRICAM, ZARCUTHRA, CARETH, FORASGIL,
  CRISDURIAN, COLANNON, HITHLOMIR, THALKETTOTH, ARVEDUI, THRANDUIL, THENGEL,
  HAMMERHAND, CELEGORM, THROR, MAEDHROS, OLORIN, ANGUIREL, OROME,
  EONWE, THEODEN, ULMO, OSONDIR, TURMIL, TIL, DEATHWREAKER, AVAVIR, TARATOL;

/* Unique artifact armour flags */
extern s32b DOR_LOMIN, NENYA, NARYA, VILYA, BELEGENNON, FEANOR, ISILDUR,
SOULKEEPER, FINGOLFIN, ANARION, POWER, PHIAL, BELEG, DAL, PAURHACH,
PAURNIMMEN, PAURAEGEN, PAURNEN, CAMMITHRIM, CAMBELEG, INGWE, CARLAMMAS,
HOLHENNETH, AEGLIN, CAMLOST, NIMLOTH, NAR, BERUTHIEL, GORLIM, ELENDIL,
THORIN, CELEBORN, THRAIN, GONDOR, THINGOL, THORONGIL, LUTHIEN, TUOR, ROHAN,
TULKAS, NECKLACE, BARAHIR, CASPANION, RAZORBACK, BLADETURNER;

/* Brand new extra effecient and kind way to add unique monsters... HOORAY!! */
extern struct unique_mon u_list[MAX_R_IDX];

extern int quests[MAX_QUESTS];

/* global flags */
extern int unfelt;
extern int in_store_flag;		/* flag so equippy chars work right -DGK */
extern int good_item_flag;		/* True if an artifact has been created... */
extern int LOAD;
extern int new_level_flag;		/* Next level when true  */
extern int teleport_flag;		/* Handle teleport traps  */
extern int eof_flag;			/* Used to handle eof/HANGUP */
extern int player_light;		/* Player carrying light */
extern int cur_lite,old_lite;           /* Light radius */
extern int find_flag;        	/* Used in MORIA	      */
extern int free_turn_flag;		/* Used in MORIA	      */
extern int weapon_heavy;		/* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;			/* Flag if the pack too heavy -CJS- */
extern char doing_inven;		/* Track inventory commands */
extern int screen_change;		/* Screen changes (used in inven_commands) */
extern int monster_is_afraid;	/* redo monster fear messages -CWS */

extern int character_generated;	/* don't save score until char gen finished */
extern int character_saved;		/* prevents save on kill after save_player() */
extern int feeling;				/* level feeling */
extern int command_count;		/* Repetition of commands. -CJS- */
extern int default_dir;			/* Use last direction in repeated commands */
extern s16b noscore;			/* Don't score this game. -CJS- */
extern u32b randes_seed;		/* For encoding colors */
extern u32b town_seed;		/* Seed for town generation */
extern char *old_state;			/* state array initialized by time -CWS */
extern char *dummy_state;		/* dummy state array so that town/colors look
                                 * the same -CWS */

extern s16b dun_level;         /* Cur dungeon level   */
extern s16b object_level;		/* used to generate objects -CWS */
extern s16b missile_ctr;		/* Counter for missiles */
extern int msg_flag;			/* Set with first msg  */
extern vtype old_msg[MAX_SAVE_MSG];	/* Last messages -CJS- */
extern s16b last_msg;			/* Where in the array is the last */
extern int death;				/* True if died	      */
extern s32b turn;				/* Cur turn of game    */
extern s32b old_turn;			/* last turn feeling was felt */
extern int wizard;				/* Wizard flag	      */
extern int to_be_wizard;
extern s16b panic_save;		/* this is true if playing from a panic save */

extern int wait_for_more;

extern char days[7][29];
extern int closing_flag;		/* Used for closing   */

extern s16b cur_height, cur_width;	/* Cur dungeon size    */
/*  Following are calculated from max dungeon sizes		*/
extern s16b max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

#ifdef TARGET
/* Targetting code, stolen from Morgul -CFT */
extern int target_mode;
extern u16b target_col;
extern u16b target_row;
extern u16b target_mon;
#endif

/*  Following are all floor definitions				*/
#ifdef MAC
extern cave_type (*cave)[MAX_WIDTH];
#else
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

/* Following are player variables				*/
extern player_type py;
#ifdef MACGAME
extern char *(*player_title)[MAX_PLAYER_LEVEL];
extern player_race *race;
extern player_background *background;
#else
extern char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern player_race race[MAX_RACES];
extern player_background background[MAX_BACKGROUND];
#endif
extern u32b player_exp[MAX_PLAYER_LEVEL];
extern u16b player_hp[MAX_PLAYER_LEVEL];
extern s16b char_row;
extern s16b char_col;

#if 0 /* not used? */
extern char *dsp_race[MAX_RACES]; /* Short strings for races. -CJS- */
#endif

extern byte rgold_adj[MAX_RACES][MAX_RACES];

extern player_class class[MAX_CLASS];
extern s16b class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* Warriors don't have spells, so there is no entry for them. */
#ifdef MACGAME
extern spell_type (*magic_spell)[63];
#else
extern spell_type magic_spell[MAX_CLASS-1][63];
#endif
extern const char *spell_names[127];
extern u32b spell_learned;	/* Bit field for spells learnt -CJS- */
extern u32b spell_learned2;	/* Bit field for spells learnt -CJS- */
extern u32b spell_worked;		/* Bit field for spells tried -CJS- */
extern u32b spell_worked2;	/* Bit field for spells tried -CJS- */
extern u32b spell_forgotten;	/* Bit field for spells forgotten -JEW- */
extern u32b spell_forgotten2;	/* Bit field for spells forgotten -JEW- */
extern byte spell_order[64];	/* remember order that spells are learned in */
extern u32b spellmasks[MAX_CLASS][2];
/* used to check if player knows all spells knowable to him -CFT */
extern u16b player_init[MAX_CLASS][5];
extern s16b total_winner;

/* Following are store definitions				*/
#ifdef MACGAME
extern owner_type *owners;
#else
extern owner_type owners[MAX_OWNERS];
#endif
#ifdef MAC
extern store_type *store;
#else
extern store_type store[MAX_STORES];
#endif
extern u16b store_choice[MAX_STORES][STORE_CHOICES];
#ifndef MAC
extern int (*store_buy[MAX_STORES])();
#endif

/* Following are treasure arrays	and variables			*/
#ifdef MACGAME
extern inven_kind *k_list;
#else
extern inven_kind k_list[MAX_OBJECTS];
#endif
extern byte object_ident[OBJECT_IDENT_SIZE];
extern s16b t_level[MAX_OBJ_LEVEL+1];
extern inven_type i_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern const char *special_names[SN_ARRAY_SIZE];
extern s16b sorted_objects[MAX_DUNGEON_OBJ];
extern s16b inven_ctr;			/* Total different obj's	*/
extern s16b inven_weight;		/* Cur carried weight	*/
extern s16b equip_ctr;			/* Cur equipment ctr	*/
extern s16b tcptr;				/* Cur treasure heap ptr	*/

/* Following are creature arrays and variables			*/
#ifdef MACGAME
extern monster_race *c_list;
#else
extern monster_race c_list[MAX_R_IDX];
#endif
extern describe_mon_type desc_list[MAX_R_IDX];
extern monster_type m_list[MAX_M_IDX];
extern s16b m_level[MAX_R_LEV+1];
extern monster_attack monster_attacks[MAX_A_IDX];
#ifdef MAC
extern monster_lore *c_recall;
#else
extern monster_lore c_recall[MAX_R_IDX];	/* Monster memories. -CJS- */
#endif
extern monster_type blank_monster; /* Blank monster values	*/
extern s16b mfptr;				   /* Cur free monster ptr	*/
extern s16b mon_tot_mult;		   /* # of repro's of creature	*/

/* Following are arrays for descriptive pieces			*/
#ifdef MACGAME
extern const char **colors;
extern const char **mushrooms;
extern const char **woods;
extern const char **metals;
extern const char **rocks;
extern const char **amulets;
extern const char **syllables;
#else
extern const char *colors[MAX_COLORS];
extern const char *mushrooms[MAX_SHROOM];
extern const char *woods[MAX_WOODS];
extern const char *metals[MAX_METALS];
extern const char *rocks[MAX_ROCKS];
extern const char *amulets[MAX_AMULETS];
extern const char *syllables[MAX_SYLLABLES];
#endif

extern byte blows_table[11][12];

extern u16b normal_table[NORMAL_TABLE_SIZE];

/* Initialized data which had to be moved from some other file */
/* Since these get modified, macrsrc.c must be able to access them */
/* Otherwise, game cannot be made restartable */
/* dungeon.c */
extern char last_command;		/* Memory of previous command. */
/* moria1.c */
/* Track if temporary light about player.  */
extern int light_flag;

#ifdef MSDOS
extern byte	floorsym, wallsym;
extern int	ansi, saveprompt;
extern char	moriatop[], moriasav[];
#endif


/*
 * The FILEPATHS to various files, see "arrays.c"
 */

extern cptr ANGBAND_DIR_FILES;		/* Dir: ascii files  */
extern char *ANGBAND_DIR_BONES;		/* Dir: ascii bones files */
extern char *ANGBAND_DIR_SAVE;		/* Dir: binary save files */

extern char *ANGBAND_NEWS;		/* News file */
extern char *ANGBAND_TOP;		/* was LIBDIR(files/newscores) */
extern char *ANGBAND_WELCOME;		/* Player generation help */
extern char *ANGBAND_VERSION;		/* Version information */

extern char *ANGBAND_WIZ;		/* Acceptable wizard uid's */
extern char *ANGBAND_HOURS;		/* Hours of operation */
extern char *ANGBAND_LOAD;		/* Load information */
extern char *ANGBAND_LOG;		/* Log file of some form */

extern char *ANGBAND_R_HELP;		/* Roguelike command help */
extern char *ANGBAND_O_HELP;		/* Original command help */
extern char *ANGBAND_W_HELP;		/* Wizard command help */
extern char *ANGBAND_OWIZ_HELP;	/* was LIBDIR(files/owizcmds.hlp) */



/*
 * only extern functions declared here, static functions declared
 * inside the file that defines them.  Duh...
 */


#ifdef __STDC__

/* birth.c */
void player_birth(void);

/* creature.c */
void update_mon(int);
int movement_rate(int);
int multiply_monster(int, int, int, int);
void creatures(int);

/* death.c */
void init_scorefile(void);
void exit_game(void);
void display_scores(int, int);
void delete_entry(int);
long total_points(void);
int look_line(int);

/* desc.c */
int is_a_vowel(int);
void magic_init(void);
void known1(inven_type *);
int known1_p(inven_type *);
void known2(inven_type *);
int known2_p(inven_type *);
void clear_known2(inven_type *);
void clear_empty(inven_type *);
void store_bought(inven_type *);
int store_bought_p(inven_type *);
void sample(struct inven_type *);
void identify(int *);
void unmagic_name(inven_type *);
void objdes(char *, struct inven_type *, int);
void scribe_object(void);
void add_inscribe(inven_type *, int);
void inscribe(inven_type *, const char *);
void invcopy(inven_type *, int);
void desc_charges(int);
void inven_item_describe(int);
s16b flavor_p(inven_type *);

/* command.c */
void do_cmd_options(void);

/* dungeon.c */
void dungeon(void);
int special_check(inven_type *);
int is_quest(int);
void rerate(void);
int ruin_stat(int);

/* eat.c */
void eat(void);

/* files.c */
void read_times(void);
void helpfile(const char *);
int file_character(char *);

/* generate.c */
void generate_cave(void);

/* cave.c */

int los(int, int, int, int);
int test_lite(int, int);
int no_lite(void);

void move_cursor_relative(int, int);

void lite_spot(int, int);

void screen_map(void);

/* help.c */
void ident_char(void);

/* io.c */
#ifdef SIGTSTP
int suspend(void);
#endif
void init_curses(void);
void moriaterm(void);
void put_str(const char *, int, int);
void put_qio(void);
void restore_term(void);
void shell_out(void);
char inkey(void);
void flush(void);
void erase_line(int, int);
void clear_screen(void);
void clear_from(int);
void count_msg_print(const char *);
void prt(const char *, int, int);
void move_cursor(int, int);
void msg_print(const char *);
int get_check(const char *);
int get_com(const char *, char *);
int get_string(char *, int, int, int);
void pause_line(int);
void pause_exit(int, int);
void save_screen(void);
void restore_screen(void);
void bell(void);
void print(int, int, int);

/* magic.c */
void cast(void);

/* arrays.c */
void get_file_paths(void);

/* main.c */
int main(int, char **);

/* misc1.c */
void init_seeds();
void set_seed(u32b);
void reset_seed(void);
int check_time(void);
int randnor(int, int);
int bit_pos(u32b *);
void panel_bounds(void);
int get_panel(int, int, int);
int damroll(int, int);
int pdamroll(byte *);
unsigned char loc_symbol(int, int);
void prt_map(void);
void add_food(int);
int m_pop(void);
int max_hp(byte *);
int place_monster(int, int, int, int);
int place_win_monster(void);
void place_group(int, int, int, int);
int get_mons_num(int);
void alloc_monster(int, int, int);
int summon_monster(int * ,int *, int);
int summon_undead(int *, int *);
int summon_demon(int, int *, int *);
int summon_dragon(int *, int *);
int summon_wraith(int *, int *);
int summon_reptile(int *, int *);
int summon_spider(int *, int *);
int summon_angel(int *, int *);
int summon_ant(int *, int *);
int summon_unique(int *, int *);
int summon_jabberwock(int *, int *);
int summon_gundead(int *, int *);
int summon_ancientd(int *, int *);
int summon_hound(int *, int *);
int i_pop(void);
void pusht(int);
int magik(int);
int m_bonus(int, int, int);
void magic_treasure(int, int, int, int);
int compact_monsters(void);
int get_nmons_num(int);
int distance(int, int, int, int);

/* misc2.c */
void place_trap(int, int, int);
void place_rubble(int, int);
void place_gold(int, int);
int get_obj_num(int,int);
void place_object(int, int);
void random_object(int, int, int);
void cnv_stat(int, char *);
void prt_stat(int);
void prt_field(const char *, int, int);
int stat_adj(int);
int chr_adj(void);
int con_adj(void);
const char *title_string(void);
void prt_title(void);
void prt_level(void);
void prt_cmana(void);
void prt_mhp(void);
void prt_chp(void);
void prt_pac(void);
void prt_gold(void);
void prt_depth(void);
void prt_hunger(void);
void prt_blind(void);
void prt_confused(void);
void prt_afraid(void);
void prt_poisoned(void);
void prt_state(void);
void prt_speed(void);
void prt_study(void);
void prt_winner(void);
u16b modify_stat(int, int);
void set_use_stat(int);
int inc_stat(int);
int dec_stat(int);
int res_stat(int);
void bst_stat(int, int);
int tohit_adj(void);
int toac_adj(void);
int todis_adj(void);
int todam_adj(void);
void prt_stat_block(void);
void draw_cave(void);
void put_character(void);
void put_stats(void);
const char *likert(int, int);
void put_misc1(void);
void put_misc2(void);
void put_misc3(void);
void display_player(void);
void get_name(void);
void change_name(void);
void inven_destroy(int);
void take_one_item(struct inven_type *, struct inven_type *);
void inven_drop(int, int);
int inven_damage(int (*)(), int);
int weight_limit(void);
int inven_check_num(inven_type *);
int inven_check_weight(struct inven_type *);
void check_strength(void);
int inven_carry(struct inven_type *);
int spell_chance(int);
void print_spells(int *, int, int, int);
int get_spell(int *, int, int *, int *, const char *, int);
void calc_spells(int);
void gain_spells(void);
void calc_mana(int);
void prt_experience(void);
void calc_hitpoints(void);
void insert_str(char *, const char *, const char *);
void insert_lnum(char *, const char *, s32b, int);
int enter_wiz_mode(void);
int attack_blows(int, int *);
int tot_dam(struct inven_type *, int, int);
int critical_blow(int, int, int, int);
int mmove(int, int *, int *);
int player_saves(void);
int find_range(int, int, int *, int *);
void teleport(int);
void check_view(void);
void place_good(int, int, u32b);
int place_ghost(void);
void prt_cut(void);
void prt_stun(void);
void special_random_object(int, int, int);
void cut_player(int);
void stun_player(int);
void prt_equippy_chars(void);
void get_coin_type(monster_race *);

/* monsters.c */

/* moria1.c */
void py_bonuses(struct inven_type *, int);
void calc_bonuses(void);
int show_inven(int, int, int, int, int ());
const char *describe_use(int);
int show_equip(int, int);
void inven_takeoff(int, int);
int verify(const char *, int);
void inven_command(int);
int get_item(int *, const char *, int, int, int ());
int get_dir(const char *, int *);
int get_alldir(const char *, int *);
void move_rec(int, int, int, int);
void light_room(int, int);
void move_light(int, int, int, int);
void disturb(int, int);
void search_on(void);
void search_off(void);
void rest(void);
void rest_off(void);
int test_hit(int, int, int, int, int);
void take_hit(int, const char *);
void change_trap(int, int);
void search(int, int, int);
void find_init(int);
void find_run(void);
void end_find(void);
void area_affect(int, int, int);
int minus_ac(u32b);
void corrode_gas(const char *);
void poison_gas(int, const char *);
void fire_dam(int, const char *);
void cold_dam(int, const char *);
void light_dam(int, const char *);
void acid_dam(int, const char *);
void darken_player(int, int);

/* moria2.c */
int cast_spell(const char * ,int, int *, int *);
void delete_monster(int);
void fix1_delete_monster(int);
void fix2_delete_monster(int);
int delete_object(int, int);
u32b monster_death(int, int, u32b, u32b, u32b);
int mon_take_hit(int, int, int);
void move_player(int, int);
void do_cmd_open(void);
void do_cmd_close(void);
int twall(int, int, int, int);
void tunnel(int);
void do_cmd_disarm(void);
void look(void);
void do_cmd_fire(void);
void bash(void);
void delete_unique(void);
void carry(int, int, int);
void check_unique(monster_type *);
void target(void); /* target fns stolen from Morgul -CFT */
int at_target(int, int); /* target fns stolen from Morgul -CFT */
void mmove2(int *, int *, int, int, int, int);

#ifdef MSDOS
/* ms_misc.c */
char *getlogin(void);
#ifdef __TURBOC__
void sleep(int);
#else
unsigned int sleep(int );
#endif
void error(char *, ...);
void warn(char *, ...);
void msdos_init(void);
void msdos_raw(void);
void msdos_noraw(void);
int bios_getch(void);
int msdos_getch(void);
void bios_clear(void);
void msdos_intro(void);
void bios_clear(void);
#endif

/* potions.c */
void quaff(void);

/* prayer.c */
void pray(void);

/* recall.c */
int bool_roff_recall(int);
int roff_recall(int);

/* rnd.c is unused now -CWS */
/* random.c */
#if !(defined(linux) || defined(__osf__))
long random(void);
#if !(defined(__MINT__) || defined(__386BSD__))
void srandom(int);
#endif
char *initstate(unsigned int, char *, int);
char *setstate(char *);
#endif

/* rods.c */
void activate_rod(void);

/* save.c */
#ifdef MAC
int save_player(int);
#else
int save_player(void);
#endif
int _save_player(char *);
int load_player(int *);

/* scrolls.c */
void read_scroll(void);

/* sets.c */
int set_corrodes(inven_type *);
int set_flammable(inven_type *);
int set_frost_destroy(inven_type *);
int set_acid_affect(inven_type *);
int set_lightning_destroy(inven_type *);
int set_null(inven_type *);
int set_acid_destroy(inven_type *);
int set_fire_destroy(inven_type *);
int set_plasma_destroy(inven_type *);
int set_meteor_destroy(inven_type *);
int set_holy_destroy(inven_type *);
int set_mana_destroy(inven_type *);
int general_store(int);
int armory(int);
int weaponsmith(int);
int temple(int);
int alchemist(int);
int magic_shop(int);

#ifdef MAC
int store_buy(int, int);
#endif

/* signals.c */
void signals_ignore_tstp(void);
void signals(void);
void signals_init(void);
void ignore_signals(void);
void default_signals(void);
void restore_signals(void);

/* spells.c */
void monster_name(char *, struct monster_type *, struct monster_race *);
void lower_monster_name(char *, struct monster_type *, struct monster_race *);
int sleep_monsters1(int, int);
int detect_treasure(void);
int detect_object(void);
int detect_trap(void);
int detect_sdoor(void);
int detect_invisible(void);
int lite_area(int, int, int, int);
int unlite_area(int, int);
void map_area(void);
int ident_spell(void);
int aggravate_monster(int);
int trap_creation(void);
int door_creation(void);
int td_destroy(void);
int detect_monsters(void);
void mon_light_dam(int, int, int);
void lite_line(int, int, int);
void frost_line(int, int, int, int);
void starlite(int, int);
int disarm_all(int, int, int);
void get_flags(int, u32b *, u32b *, int (**)());
void fire_bolt(int, int, int, int, int);
void fire_ball(int, int, int, int, int, int);
void breath(int, int, int, int, char *, int);
int recharge(int);
int hp_monster(int, int, int, int);
int drain_life(int, int, int, int);
int speed_monster(int, int, int, int);
int confuse_monster(int, int, int, int);
int sleep_monster(int, int, int);
int wall_to_mud(int, int, int);
int td_destroy2(int, int, int);
int poly_monster(int, int, int);
int build_wall(int, int, int);
int clone_monster(int, int, int);
void teleport_away(int, int);
void teleport_to(int, int);
int teleport_monster(int, int, int);
int mass_genocide(int);
int genocide(int);
int speed_monsters(int);
int sleep_monsters2(void);
int mass_poly(void);
int detect_evil(void);
int hp_player(int);
int cure_confusion(void);
int cure_blindness(void);
int cure_poison(void);
int remove_fear(void);
void earthquake(void);
int protect_evil(void);
void create_food(void);
int dispel_creature(int, int);
int turn_undead(void);
void warding_glyph(void);
void lose_str(void);
void lose_int(void);
void lose_wis(void);
void lose_dex(void);
void lose_con(void);
void lose_chr(void);
void lose_exp(s32b);
int slow_poison(void);
void bless(int);
void detect_inv2(int);
void destroy_area(int, int);
int enchant(inven_type *, int, byte);
void elemental_brand(void);
int remove_curse(void);
int restore_level(void);
void self_knowledge(void);
int probing(void);
int detection(void);
void starball(int,int);
void bolt(int, int, int, int, char *, monster_type *, int);
int lose_all_info(void);
void tele_level(void);
void identify_pack(void);
int fear_monster(int, int, int, int);
int banish_creature(u32b, int);
int remove_all_curse(void);
void darken_room(int, int);
const char *pain_message(int, int);
void line_spell(int, int, int, int, int);

/* staffs.c */
void use(void);

/* store1.c */
s32b item_value(struct inven_type *);
s32b sell_price(int, s32b *, s32b *, struct inven_type *);
int store_check_num(inven_type *, int);
void store_carry(int, int *, struct inven_type *);
void store_destroy(int, int, int);
void store_init(void);
void store_maint(void);
int noneedtobargain(int, s32b);
void updatebargain(int, s32b, s32b);

/* store2.c */
void enter_store(int);

/* treasur1.c */

/* treasur2.c */

#ifdef unix
/* unix.c */
int check_input(int);
#if 0
int system_cmd(char *);
#endif
#endif

/* util.c */
#ifndef HAS_USLEEP
int usleep(unsigned long);
#endif
void user_name(char *buf, int id);
int my_topen(const char *, int, int);
FILE *my_tfopen(const char *, const char *);

/* variable.c */

/* wands.c */
void aim(void);

/* wizard.c */
void wiz_lite(int);
void change_character(void);
void wizard_create(void);
void artifact_check(void);
void artifact_check_no_file(void);
void do_cmd_check_uniques(void);
int is_wizard(int);

#endif      /* __STDC__ */


