/* File: defines.h */

/* Purpose: global constants used by Angband */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * Note to the Wizard:
 *
 *       Tweaking these constants can *GREATLY* change the game.
 *       Two years of constant tuning have generated these      
 *       values.  Minor adjustments are encouraged, but you must
 *       be very careful not to unbalance the game.  Moria was  
 *       meant to be challenging, not a give away.  Many        
 *       adjustments can cause the game to act strangely, or even
 *       cause errors.
 */

/*
 * Addendum:
 * 
 * I have greatly expanded the number of defined constants.  However, if
 * you change anything below, without understanding EXACTLY how the game
 * uses the number, the program may stop working correctly.  Modify the
 * constants at your own risk.  Also note that not all constants in the
 * code are written using the proper symbolic constant.  Several hard
 * coded values are lurking around.
 */


/*
 * Current version number of Angband: 2.6.1
 *
 * Note that 5.2 must never be used, as it was used in Angband 2.4-2.5.
 * Actually, by that time, nobody will want to use 2.5 savefiles!
 */

#define CUR_VERSION_MAJ 2
#define CUR_VERSION_MIN 6
#define PATCH_LEVEL 1



/* 
 * Some constants
 */



/*
 * These values are used to keep things "small"
 */
#define MAX_UCHAR       255
#define MAX_SHORT       32767
#define MAX_LONG        0xFFFFFFFFL


/*
 * Used for "Check Load"
 */
#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif


/*
 * This value cannot be changed (yet)
 */
#define MSG_LINE		0

/* number of messages to save in a buffer */
#define MAX_SAVE_MSG   22               /* How many messages to save -CJS- */

/*
 * This is the maximum number of high scores to save
 */
#define MAX_SAVE_HISCORES	500


/* Dungeon size parameters */
#define MAX_HEIGHT		66	/* Multiple of 11; >= 22 */
#define MAX_WIDTH		198	/* Multiple of 33; >= 66 */
#define SCREEN_HEIGHT		22	/* Hard Coded */
#define SCREEN_WIDTH		66	/* Hard Coded */
#define QUART_HEIGHT (SCREEN_HEIGHT / 4)
#define QUART_WIDTH  (SCREEN_WIDTH / 4)


/*
 * Dungeon generation values
 *
 * Note: The entire design of dungeon can be changed by only
 * slight adjustments here.
 */

#define DUN_TUN_RND       9     /* 1/Chance of Random direction          */
#define DUN_TUN_CHG      70     /* Chance of changing direction (99 max) */
#define DUN_TUN_CON      15     /* Chance of extra tunneling             */
#define DUN_ROO_MEA      45     /* Mean of # of rooms, standard dev2     */
#define DUN_TUN_PEN      25     /* Chance of room doors                  */
#define DUN_TUN_JCT       8     /* Chance of doors at tunnel junctions   */
#define DUN_STR_DEN       5     /* Density of streamers                  */
#define DUN_STR_RNG       2     /* Width of streamers                    */
#define DUN_STR_MAG       3     /* Number of magma streamers             */
#define DUN_STR_MC       90     /* 1/chance of treasure per magma        */
#define DUN_STR_QUA       2     /* Number of quartz streamers            */
#define DUN_STR_QC       40     /* 1/chance of treasure per quartz       */
#define DUN_UNUSUAL      200    /* Level/chance of unusual room          */
#define DUN_DEST         15     /* 1/chance of having a destroyed level */
#define SPEC_DEST        2

/* Store constants */
#define MAX_OWNERS       24     /* Number of owners to choose from       */
#define MAX_STORES        8     /* Number of different stores            */
#define STORE_INVEN_MAX  24     /* Max number of discrete objs in inven  */
#define STORE_CHOICES    30     /* NUMBER of items to choose stock from  */
#define STORE_TURNOVER    9     /* Normal shop turnover, per day */
#define STORE_MIN_KEEP    6     /* Min slots to "always" keep full */
#define STORE_MAX_KEEP   18     /* Max slots to "always" keep full */

#define MAX_QUESTS        4     /* only 1 defined anyway --CFT */
#define DEFINED_QUESTS    1 
#define SAURON_QUEST      0 
#define Q_PLANE          -1


/*
 * Hack -- this is the chance (1/GREAT_OBJ) that when an item is
 * created by "get_obj_num()", that the "level" will be "enhanced"
 * by quite a large amount.  This will allow, for example, a ring
 * of speed to be found at 50 feet.  
 */
#define GREAT_OBJ        11     /* 1/Chance of item being a Great Item  */


/*
 * Note that the following constants are all related, if you change one,
 * you must also change all succeeding ones.  Also, player_init[] and
 * store_choice[] may also have to be changed.  And most importantly,
 * these constants are fixed by their index in the "k_list" file.
 */

#define OBJ_NOTHING             0

#define OBJ_FOOD_RATION		21
#define OBJ_MUSH                28

#define OBJ_BROAD_SWORD		34
#define OBJ_DAGGER		43
#define OBJ_SMALL_SWORD		46
#define OBJ_MACE		53
#define OBJ_LEAD_FILLED_MACE	56

#define OBJ_LONG_BOW		74
#define OBJ_ARROW		78
#define OBJ_BOLT		80
#define OBJ_SHOT		83

#define OBJ_IRON_CROWN		98

#define OBJ_SOFT_LEATHER	103
#define OBJ_CHAIN_MAIL		109

#define OBJ_ANTI_EVIL		209
#define OBJ_SCROLL_RECALL	220

#define OBJ_POTION_HEALING	242
#define OBJ_POTION_BERSERK	258

#define OBJ_SPELL_BOOK		330
#define OBJ_PRAYER_BOOK		334

#define OBJ_RUINED_CHEST        344

#define OBJ_TORCH		346

#define OBJ_RUBBLE              445
#define OBJ_OPEN_DOOR           446
#define OBJ_CLOSED_DOOR         447
#define OBJ_SECRET_DOOR         448
#define OBJ_UP_STAIR            449
#define OBJ_DOWN_STAIR          450

#define OBJ_SCARE_MON           459

#define OBJ_STORE_DOOR          451
#define OBJ_TRAP_LIST           460
#define OBJ_GOLD_LIST           480

/* Start of the "Special Objects" */

#define OBJ_SPECIAL         	500

/* The actual special objects */

#define OBJ_GALADRIEL		(OBJ_SPECIAL + 0)
#define OBJ_ELENDIL		(OBJ_SPECIAL + 1)
#define OBJ_THRAIN		(OBJ_SPECIAL + 2)

#define OBJ_CARLAMMAS		(OBJ_SPECIAL + 3)
#define OBJ_INGWE		(OBJ_SPECIAL + 4)
#define OBJ_DWARVES		(OBJ_SPECIAL + 5)

#define OBJ_BARAHIR		(OBJ_SPECIAL + 6)
#define OBJ_TULKAS		(OBJ_SPECIAL + 7)
#define OBJ_NARYA		(OBJ_SPECIAL + 8)
#define OBJ_NENYA		(OBJ_SPECIAL + 9)
#define OBJ_VILYA		(OBJ_SPECIAL + 10)
#define OBJ_POWER		(OBJ_SPECIAL + 11)


/* Total Number of object kinds */
#define MAX_K_IDX		512



#define MAX_GOLD       18       /* Number of different types of gold     */


#define TREAS_ROOM_ALLOC  9     /* Amount of objects for rooms         */
#define TREAS_ANY_ALLOC   3     /* Amount of objects for corridors     */
#define TREAS_GOLD_ALLOC  3     /* Amount of gold (and gems)           */


/*
 * Magic Treasure Generation constants                  
 * Note: Number of special objects, and degree of enchantments
 *       can be adjusted here.
 */

#define OBJ_STD_ADJ     125     /* Adjust STD per level * 100        */
#define OBJ_STD_MIN       7     /* Minimum STD                       */
#define OBJ_TOWN_LEVEL    5     /* Town object generation level      */
#define OBJ_BASE_MAGIC   15     /* Base magic_chance                 */
#define OBJ_BASE_MAX     70     /* Maximum magic_chance              */
#define OBJ_DIV_SPECIAL   4     /* magic_chance/# special magic      */
#define OBJ_DIV_CURSED   13     /* 10*magic_chance/# cursed items    */

/* Constants describing limits of certain objects */
#define FUEL_LAMP	15000   /* Maximum amount that lamp can be filled  */
#define OBJ_BOLT_RANGE     18   /* Maximum range of bolts and balls        */
#define OBJ_BREAK_GLYPH   550   /* Rune of protection resistance           */

/* Monster race array limit */
#define MAX_R_IDX       549

/* Monster attack type limit */
#define MAX_A_IDX       285

/*
 * with MAX_M_IDX 101, it is possible to get compacting monsters messages
 * while breeding/cloning monsters
 *
 * with MAX_I_IDX 150, it is possible to get compacting objects during
 * level generation, although it is extremely rare
 */

#define MIN_I_IDX           1   /* Minimum i_list index used                */
#define MAX_I_IDX         400   /* Max objects per level                    */
#define MAX_K_LEV         125   /* Maximum level of items in dungeon        */

#define MAX_M_IDX         600   /* Max monsters per level                   */
#define MAX_M_ALLOC_CHANCE 160   /* 1/x chance of new monster each round     */
#define MAX_R_LEV         99   /* Maximum level of creatures               */

#define MAX_SIGHT          20   /* Maximum dis a creature can be seen       */
#define MAX_SPELL_DIS      18   /* Maximum dis creat. spell can be cast     */
#define MAX_MON_MULT       75   /* Maximum reproductions on a level         */
#define MON_MULT_ADJ        8   /* High value slows multiplication          */
#define MON_NASTY          50   /* 1/chance of high level creat             */
#define MIN_M_ALLOC_LEVEL  14   /* Minimum number of monsters/level         */
#define MIN_M_ALLOC_TD      4   /* Number of people on town level (day)     */
#define MIN_M_ALLOC_TN      8   /* Number of people on town level (night)   */
#define WIN_MON_TOT         2   /* Total number of "win" creatures          */
#define WIN_MON_APPEAR    100   /* Level where winning creatures begin      */
#define MON_SUMMON_ADJ      2   /* Adjust level of summoned creatures       */
#define MON_DRAIN_LIFE      2   /* Percent of player exp drained per hit    */
#define MIN_M_IDX           2   /* Minimum index in m_list (1=py, 0=no mon) */

/* Mega Hack -- Trap constants */
#define MAX_TRAP           18   /* Number of defined traps      */

/* Descriptive constants                                        */
#define MAX_COLORS     57       /* Used with potions      */
#define MAX_SHROOM     21       /* Used with mushrooms    */
#define MAX_WOODS      32       /* Used with staffs       */
#define MAX_METALS     32       /* Used with wands & rods */
#define MAX_ROCKS      42       /* Used with rings        */
#define MAX_AMULETS    16       /* Used with amulets      */
#define MAX_TITLES     45       /* Used with scrolls      */
#define MAX_SYLLABLES 158       /* Used with scrolls      */

/* Player constants                                             */
#define MAX_EXP      99999999L  /* Maximum amount of experience -CJS-      */
#define MAX_PLAYER_LEVEL    50  /* Maximum possible character level        */
#define MAX_RACES           10  /* Number of defined races                 */
#define MAX_CLASS            6  /* Number of defined classes               */
#define USE_DEVICE           3  /* x> Harder devices x< Easier devices     */
#define MAX_BACKGROUND     128  /* Number of types of histories for univ   */
#define PLAYER_FOOD_FULL 10000  /* Getting full                            */
#define PLAYER_FOOD_MAX  15000  /* Maximum food value, beyond is wasted    */
#define PLAYER_FOOD_FAINT  300  /* Character begins fainting               */
#define PLAYER_FOOD_WEAK  1000  /* Warn player that he is getting very low */
#define PLAYER_FOOD_ALERT 2000  /* Warn player that he is getting low      */
#define PLAYER_REGEN_FAINT  33  /* Regen factor*2^16 when fainting         */
#define PLAYER_REGEN_WEAK   98  /* Regen factor*2^16 when weak             */
#define PLAYER_REGEN_NORMAL 197 /* Regen factor*2^16 when full             */
#define PLAYER_REGEN_HPBASE 1442 /* Min amount hp regen*2^16               */
#define PLAYER_REGEN_MNBASE 524 /* Min amount mana regen*2^16              */
#define PLAYER_WEIGHT_CAP   130 /* Ex: 13 pounds per strength point	   */
#define PLAYER_EXIT_PAUSE    1  /* Pause time before player can re-roll    */

/* class level adjustment constants */
#define CLA_BTH         0
#define CLA_BTHB        1
#define CLA_DEVICE      2
#define CLA_DISARM      3
#define CLA_SAVE        4

/*
 * this depends on the fact that CLA_SAVE values are all the same, if not,
 * then should add a separate column for this
 */
#define CLA_MISC_HIT    4
#define MAX_LEV_ADJ     5

/* Base to hit constants                                        */
#define BTH_PLUS_ADJ    3       /* Adjust BTH per plus-to-hit   */


/*
 * Inventory slot values (do not change these)
 */
#define INVEN_WIELD	22
#define INVEN_HEAD      23
#define INVEN_NECK      24
#define INVEN_BODY      25
#define INVEN_ARM       26
#define INVEN_HANDS     27
#define INVEN_RIGHT     28
#define INVEN_LEFT      29
#define INVEN_FEET      30
#define INVEN_OUTER     31
#define INVEN_LITE      32
#define INVEN_AUX       33

/*
 * Total number of inventory slots (22 pack, 12 equip)
 */
#define INVEN_TOTAL	34



/* Hardcoded */
#define A_STR	0
#define A_INT	1
#define A_WIS	2
#define A_DEX	3
#define A_CON	4
#define A_CHR	5

/* some systems have a non-ANSI definition of this, so undef it first */
#undef CTRL
#define CTRL(c) ((c)&037)
#define DELETE          0x7f
#define ESCAPE        '\033'    /* ESCAPE character -CJS- */



/*
 * Fval definitions: various types of dungeon floors and walls
 * Note that numbers above "15" will cause all kinds of problems.
 * The "savefiles" in particular, will be totally trashed...
 */

#define NULL_WALL	0	/* Temp value for "generate.c" */

#define DARK_FLOOR      1
#define LIGHT_FLOOR     2
#define NT_DARK_FLOOR   3
#define NT_LIGHT_FLOOR  4
#define MAX_CAVE_ROOM   4
#define CORR_FLOOR      5	/* Floor, in a corridor */
#define BLOCKED_FLOOR   6       /* a corridor space with cl/st/se door or rubble */

#define MIN_WALL	8	/* Hack -- minimum "wall" fval */

#define TMP1_WALL	8
#define TMP2_WALL	9

#define GRANITE_WALL	12	/* Granite */
#define MAGMA_WALL	13	/* Magma */
#define QUARTZ_WALL	14	/* Quartz */
#define BOUNDARY_WALL	15	/* Permanent */

/* Class spell types */
#define NONE            0
#define MAGE            1
#define PRIEST          2

/* offsets to spell names in spell_names[] array */
#define SPELL_OFFSET    0
#define PRAYER_OFFSET   63

/* definitions for the psuedo-normal distribution generation */
#define NORMAL_TABLE_SIZE       256
#define NORMAL_TABLE_SD          64  /* the standard deviation for the table */


/* Masks for the player's status field */
#define PY_STATS        0x3F000000L

/* definitions for the player's status field */
#define PY_HUNGRY       0x00000001L
#define PY_WEAK         0x00000002L
#define PY_BLIND        0x00000004L
#define PY_CONFUSED     0x00000008L
#define PY_FEAR         0x00000010L
#define PY_POISONED     0x00000020L
#define PY_FAST         0x00000040L
#define PY_SLOW         0x00000080L
#define PY_SEARCH       0x00000100L
#define PY_REST         0x00000200L
#define PY_STUDY        0x00000400L

#define PY_INVULN       0x00001000L
#define PY_HERO         0x00002000L
#define PY_SHERO        0x00004000L
#define PY_BLESSED      0x00008000L
#define PY_DET_INV      0x00010000L
#define PY_TIM_INFRA    0x00020000L
#define PY_SPEED        0x00040000L
#define PY_STR_WGT      0x00080000L
#define PY_PARALYSED    0x00100000L
#define PY_REPEAT       0x00200000L
#define PY_ARMOR        0x00400000L

#define PY_STR          0x01000000L /* these 6 stat flags must be adjacent */
#define PY_INT          0x02000000L
#define PY_WIS          0x04000000L
#define PY_DEX          0x08000000L
#define PY_CON          0x10000000L
#define PY_CHR          0x20000000L

#define PY_HP           0x40000000L
#define PY_MANA         0x80000000L





/*
 * No stack can grow to "MAX_STACK_SIZE" items.
 * This should be a number between 48 and 256.
 */
#define MAX_STACK_SIZE			100


/*
 * The "sval" mask, used for assigning "flavors"
 */
#define ITEM_SUBVAL_MASK (64 - 1)


#define TR_EGO_WEAPON   0x0007E000L


/* creature spell definitions */
#define CS_SPELLS       0xFF07FFF0L
#define CS_TEL_SHORT    0x00000010L
#define CS_TEL_LONG     0x00000020L
#define CS_TEL_TO       0x00000040L
#define CS_LGHT_WND     0x00000080L
#define CS_SER_WND      0x00000100L
#define CS_HOLD_PER     0x00000200L
#define CS_BLIND        0x00000400L
#define CS_CONFUSE      0x00000800L
#define CS_FEAR         0x00001000L
#define CS_SUMMON_MON   0x00002000L
#define CS_SUMMON_UND   0x00004000L
#define CS_SLOW_PER     0x00008000L
#define CS_DRAIN_MANA   0x00010000L

#define CS_BR_LIGHT     0x00080000L
#define CS_BR_GAS       0x00100000L
#define CS_BR_ACID      0x00200000L
#define CS_BR_FROST     0x00400000L
#define CS_BR_FIRE      0x00800000L

/* creature defense flags */
#define CD_DRAGON       0x0001
#define CD_ANIMAL       0x0002
#define CD_EVIL         0x0004
#define CD_UNDEAD       0x0008
#define CD_WEAKNESS     0x03F0
#define CD_FROST        0x0010
#define CD_FIRE         0x0020
#define CD_POISON       0x0040
#define CD_ACID         0x0080
#define CD_LIGHT        0x0100
#define CD_STONE        0x0200

#define CD_NO_SLEEP     0x1000
#define CD_INFRA        0x2000
#define CD_MAX_HP       0x4000
#define CD_ORC          0x8000


/* Special "Item Description Flags" */

/* id's used for item description, stored in i_ptr->ident */
#define ID_FELT         0x01	/* Item has been "felt" */
#define ID_EMPTY        0x04	/* Item is now "empty" */
#define ID_KNOWN        0x08	/* Item is fully "known" */
#define ID_SHOW_HITDAM  0x20
#define ID_NOSHOW_P1    0x40    /* don't show (+x) even if pval != 0 -CWS   */
#define ID_NOSHOW_TYPE  0x80    /* don't show (+x of yyy), just (+x) -CWS */



/*
 * Ego-Item indexes
 *
 * All the "Bad" Ego-Items are at the end.
 * The holes were left by artifacts.
 */

#define EGO_R			1
#define EGO_RESIST_A		2
#define EGO_RESIST_F		3
#define EGO_RESIST_C		4
#define EGO_RESIST_E		5
#define EGO_HA			6
#define EGO_DF			7
#define EGO_SLAY_A		8
#define EGO_SLAY_D		9
#define EGO_SLAY_E		10
#define EGO_SLAY_U		11
#define EGO_FT			12
#define EGO_FB			13
#define EGO_FREE_ACTION		14
#define EGO_SLAYING		15

#define EGO_SLOW_DESCENT	18
#define EGO_SPEED		19
#define EGO_STEALTH		20

#define EGO_INTELLIGENCE	24
#define EGO_WISDOM		25
#define EGO_INFRAVISION		26
#define EGO_MIGHT		27
#define EGO_LORDLINESS		28
#define EGO_MAGI		29
#define EGO_BEAUTY		30
#define EGO_SEEING		31
#define EGO_REGENERATION	32

#define EGO_PROTECTION		39

#define EGO_FIRE		43
#define EGO_SLAY_EVIL		44
#define EGO_DRAGON_SLAYING	45

#define EGO_SLAY_ANIMAL		55


#define EGO_ACCURACY		65

#define EGO_SLAY_O		67
#define EGO_POWER		68

#define EGO_WEST		71
#define EGO_BLESS_BLADE		72
#define EGO_SLAY_DEMON		73
#define EGO_SLAY_T		74

#define EGO_WOUNDING		77

#define EGO_LITE		81
#define EGO_AGILITY		82

#define EGO_SLAY_G		85
#define EGO_TELEPATHY		86

#define EGO_DRAGONKIND           87
#define EGO_ELVENKIND            88	/* Was 97 */

#define EGO_ATTACKS		90	/* Was 179 */

#define EGO_AMAN		91

#define EGO_MIN_WORTHLESS	96	/* First "worthless" Ego-Item */

#define EGO_WEAKNESS		104
#define EGO_STUPIDITY		105
#define EGO_DULLNESS		106
#define EGO_CLUMSINESS		108
#define EGO_UGLINESS		109
#define EGO_TELEPORTATION	110

#define EGO_IRRITATION		112
#define EGO_VULNERABILITY	113
#define EGO_ENVELOPING		114

#define EGO_SLOWNESS		116
#define EGO_NOISE		117
#define EGO_GREAT_MASS		118

#define EGO_BACKBITING		120

#define EGO_BLINDNESS		121
#define EGO_TIMIDNESS		122

#define EGO_MORGUL		124

#define EGO_SHATTERED		126
#define EGO_BLASTED		127

#define EGO_MAX			128



/*
 * Artifact indexes
 */

	/* Lites */
#define ART_GALADRIEL		1
#define ART_ELENDIL		2
#define ART_THRAIN		3

	/* Amulets */
#define ART_CARLAMMAS		4
#define ART_INGWE		5
#define ART_DWARVES		6

	/* Rings */
#define ART_BARAHIR		8
#define ART_TULKAS		9
#define ART_NARYA		10
#define ART_NENYA		11
#define ART_VILYA		12
#define ART_POWER		13

	/* Dragon Scale */
#define ART_RAZORBACK		16
#define ART_BLADETURNER		17

	/* Hard Armour */
#define ART_SOULKEEPER		19
#define ART_ISILDUR		20
#define ART_ROHIRRIM		21
#define ART_BELEGENNON		22
#define ART_CELEBORN		23
#define ART_ARVEDUI		24
#define ART_CASPANION		25

	/* Soft Armour */
#define ART_HITHLOMIR		27
#define ART_THALKETTOTH		28

	/* Shields */
#define ART_THORIN		30
#define ART_CELEGORM		31
#define ART_ANARION		32

	/* Helms */
#define ART_MORGOTH		34
#define ART_BERUTHIEL		35
#define ART_THRANDUIL		36
#define ART_THENGEL		37
#define ART_HAMMERHAND		38
#define ART_DOR			39
#define ART_HOLHENNETH		40
#define ART_GORLIM		41
#define ART_GONDOR		42

	/* Cloaks */
#define ART_COLLUIN		44
#define ART_HOLCOLLETH		45
#define ART_THINGOL		46
#define ART_THORONGIL		47
#define ART_COLANNON		48
#define ART_LUTHIEN		49
#define ART_TUOR		50

	/* Gloves */
#define ART_CAMBELEG		52
#define ART_CAMMITHRIM		53
#define ART_PAURHACH		54
#define ART_PAURNIMMEN		55
#define ART_PAURAEGEN		56
#define ART_PAURNEN		57
#define ART_CAMLOST		58
#define ART_FINGOLFIN		59

	/* Boots */
#define ART_FEANOR		60
#define ART_DAL			61
#define ART_THROR		62


	/* Swords */
#define ART_MAEDHROS		64
#define ART_ANGRIST		65
#define ART_NARTHANC		66
#define ART_NIMTHANC		67
#define ART_DETHANC		68
#define ART_RILIA		69
#define ART_BELANGIL		70
#define ART_CALRIS		71
#define ART_ARUNRUTH		72
#define ART_GLAMDRING		73
#define ART_AEGLIN		74
#define ART_ORCRIST		75
#define ART_GURTHANG		76
#define ART_ZARCUTHRA		77
#define ART_MORMEGIL		78
#define ART_GONDRICAM		79
#define ART_CRISDURIAN		80
#define ART_AGLARANG		81
#define ART_RINGIL		82
#define ART_ANDURIL		83
#define ART_ANGUIREL		84
#define ART_ELVAGIL		85
#define ART_FORASGIL		86
#define ART_CARETH		87
#define ART_STING		88
#define ART_HARADEKKET		89
#define ART_GILETTAR		90
#define ART_DOOMCALLER		91

	/* Polearms */
#define ART_THEODEN		93
#define ART_PAIN		94
#define ART_OSONDIR		95
#define ART_TIL			96
#define ART_AEGLOS		97
#define ART_OROME		98
#define ART_NIMLOTH		99
#define ART_EORLINGAS		100
#define ART_DURIN		101
#define ART_EONWE		102
#define ART_BALLI		103
#define ART_LOTHARANG		104
#define ART_MUNDWINE		105
#define ART_BARUKKHELED		106
#define ART_WRATH		107
#define ART_ULMO		108
#define ART_AVAVIR		109

	/* Hafted */
#define ART_GROND		111
#define ART_TOTILA		112
#define ART_THUNDERFIST		113
#define ART_BLOODSPIKE		114
#define ART_FIRESTAR		115
#define ART_TARATOL		116
#define ART_AULE		117
#define ART_NAR			118
#define ART_ERIRIL		119
#define ART_OLORIN		120
#define ART_DEATHWREAKER	121
#define ART_TURMIL		122

	/* Bows */
#define ART_BELTHRONDING	124
#define ART_BARD		125
#define ART_CUBRAGOL		126

	/* Number of entries */
#define ART_MAX			128


/*
 * The values for the "tval" field of various objects.
 * This value is the primary means by which items are sorted in the
 * player inventory.  It also groups things for MIN_WEAR/MAX_WEAR.
 *
 * Note that all items with tval from 10 to 50 are "wearable_p()",
 * which means that the special "TR#_*" flags apply to them.
 */

#define TV_NEVER        -1	/* used by find_range() for non-search */
#define TV_NOTHING       0	/* Nothing (used in OBJ_NOTHING) */
#define TV_SKELETON      1	/* Skeletons ('s') */
#define TV_BOTTLE	 2	/* Empty bottles ('!') */
#define TV_JUNK          3	/* Sticks, Pottery, etc ('~') */
#define TV_SPIKE         5	/* Spikes ('~') */
#define TV_CHEST         7	/* Chests ('~') */
#define TV_MIN_WEAR     10	/* Min tval for "wearable" items */
#define TV_LITE         10	/* Torches, Lanterns, Specials */
#define TV_SHOT		16	/* Ammo for slings */
#define TV_ARROW        17	/* Ammo for bows */
#define TV_BOLT         18	/* Ammo for x-bows */
#define TV_BOW          19	/* Slings/Bows/Xbows */
#define TV_DIGGING      20	/* Shovels/Picks */
#define TV_HAFTED       21	/* Priest Weapons */
#define TV_POLEARM      22	/* Axes and Pikes */
#define TV_SWORD        23	/* Edged Weapons */
#define TV_BOOTS        30	/* Boots */
#define TV_GLOVES       31	/* Gloves */
#define TV_HELM         33	/* Helms/Crowns */
#define TV_SHIELD       34	/* Shields */
#define TV_CLOAK        35	/* Cloaks */
#define TV_SOFT_ARMOR   36	/* Soft Armor */
#define TV_HARD_ARMOR   37	/* Hard Armor */
#define TV_DRAG_ARMOR	38	/* Dragon Scale Mail */
#define TV_AMULET       40
#define TV_RING         45
#define TV_MAX_WEAR     50   /* max tval for wearable items */
#define TV_STAFF        55
#define TV_WAND         65
#define TV_ROD          66
#define TV_SCROLL       70   /* Only one tval for scrolls now */
#define TV_POTION       75   /* Only one tval for potions now */
#define TV_FLASK        77
#define TV_FOOD         80
#define TV_MAGIC_BOOK   90
#define TV_PRAYER_BOOK  91
#define TV_MAX_OBJECT   99	/* This is the max TV monsters pick up */
#define TV_GOLD         100	/* Gold can only be picked up by players */
#define TV_MAX_PICK_UP  100     /* This is the max TV players pick up */
#define TV_INVIS_TRAP   101	/* Invisible traps -- see visible traps */
#define TV_MIN_VISIBLE  102	/* This is the first "visible landmark" */
#define TV_VIS_TRAP     102     /* Visible traps */
/* following objects are never deleted when trying to create another one
   during level generation */
#define TV_MIN_DOORS    104
#define TV_OPEN_DOOR    104	/* Open doorway */
#define TV_UP_STAIR     107	/* Staircase up */
#define TV_DOWN_STAIR   108	/* Staircase down */
#define TV_STORE_DOOR   110	/* Entrance to store */
#define TV_SECRET_DOOR  117	/* Secret door -- treated as a "wall" */
#define TV_CLOSED_DOOR  118	/* Closed door -- treated as a "wall" */
#define TV_RUBBLE       119	/* Rubble pile -- treated as a "wall" */
#define TV_MAX_VISIBLE  119


/* The sval codes for TV_LITE */
#define SV_LITE_TORCH		0
#define SV_LITE_LANTERN		1
#define SV_LITE_GALADRIEL	4
#define SV_LITE_ELENDIL		5
#define SV_LITE_THRAIN		6


/* The "sval" codes for TV_BOW */
#define SV_SLING		2	/* (x2) */
#define SV_SHORT_BOW		12	/* (x2) */
#define SV_LONG_BOW		13	/* (x3) */
#define SV_LIGHT_XBOW		23	/* (x3) */
#define SV_HEAVY_XBOW		24	/* (x4) */

/* The "sval" codes for TV_DIGGING */
#define SV_SHOVEL		1
#define SV_GNOMISH_SHOVEL	2
#define SV_DWARVEN_SHOVEL	3
#define SV_PICK			4
#define SV_ORCISH_PICK		5
#define SV_DWARVEN_PICK		6

/* The "sval" values for TV_HAFTED */
#define SV_WHIP			2	/* 1d6 */
#define SV_QUARTERSTAFF		3	/* 1d9 */
#define SV_MACE			5	/* 2d4 */
#define SV_BALL_AND_CHAIN	6	/* 2d4 */
#define SV_WAR_HAMMER		8	/* 3d3 */
#define SV_LUCERN_HAMMER	10	/* 2d5 */
#define SV_MORNING_STAR		12	/* 2d6 */
#define SV_FLAIL		13	/* 2d6 */
#define SV_LEAD_FILLED_MACE	15	/* 3d4 */
#define SV_TWO_HANDED_FLAIL	18	/* 3d6 */
#define SV_MACE_OF_DISRUPTION	20	/* 5d8 */
#define SV_GROND		50	/* 3d4 */

/* The "sval" values for TV_POLEARM */
#define SV_SPEAR		2	/* 1d6 */
#define SV_AWL_PIKE		4	/* 1d8 */
#define SV_TRIDENT		5	/* 1d9 */
#define SV_PIKE			8	/* 2d5 */
#define SV_BEAKED_AXE		10	/* 2d6 */
#define SV_BROAD_AXE		11	/* 2d6 */
#define SV_GLAIVE		13	/* 2d6 */
#define SV_HALBERD		15	/* 3d4 */
#define SV_SCYTHE		17	/* 5d3 */
#define SV_LANCE		20	/* 2d8 */
#define SV_BATTLE_AXE		22	/* 2d8 */
#define SV_GREAT_AXE		25	/* 4d4 */
#define SV_LOCHABER_AXE		28	/* 3d8 */
#define SV_SCYTHE_OF_SLICING	30	/* 8d4 */

/* The "sval" codes for TV_SWORD */
#define SV_BROKEN_DAGGER	1	/* 1d1 */
#define SV_BROKEN_SWORD		2	/* 1d2 */
#define SV_DAGGER		4	/* 1d4 */
#define SV_MAIN_GAUCHE		5	/* 1d5 */
#define SV_RAPIER		7	/* 1d6 */
#define SV_SMALL_SWORD		8	/* 1d6 */
#define SV_SHORT_SWORD		10	/* 1d7 */
#define SV_SABRE		11	/* 1d7 */
#define SV_CUTLASS		12	/* 1d7 */
#define SV_TULWAR		15	/* 2d4 */
#define SV_BROAD_SWORD		16	/* 2d5 */
#define SV_LONG_SWORD		17	/* 2d5 */
#define SV_SCIMITAR		18	/* 2d5 */
#define SV_KATANA		20	/* 3d4 */
#define SV_BASTARD_SWORD	21	/* 3d4 */
#define SV_TWO_HANDED_SWORD	25	/* 3d6 */
#define SV_EXECUTIONERS_SWORD	28	/* 4d5 */
#define SV_BLADE_OF_CHAOS	30	/* 6d5 */

/* The "sval" codes for TV_SHIELD */
#define SV_SMALL_LEATHER_SHIELD		2
#define SV_SMALL_METAL_SHIELD		3
#define SV_LARGE_LEATHER_SHIELD		4
#define SV_LARGE_METAL_SHIELD		5
#define SV_SHIELD_OF_DEFLECTION		10

/* The "sval" codes for TV_HELM */
#define SV_HARD_LEATHER_CAP		2
#define SV_METAL_CAP			3
#define SV_IRON_HELM			5
#define SV_STEEL_HELM			6
#define SV_IRON_CROWN			10
#define SV_GOLDEN_CROWN			11
#define SV_JEWELED_CROWN		12
#define SV_MORGOTH			50

/* The "sval" codes for TV_BOOTS */
#define SV_PAIR_OF_SOFT_LEATHER_BOOTS	2
#define SV_PAIR_OF_HARD_LEATHER_BOOTS	3
#define SV_PAIR_OF_METAL_SHOD_BOOTS	6

/* The "sval" codes for TV_CLOAK */
#define SV_CLOAK			1
#define SV_SHADOW_CLOAK			6

/* The "sval" codes for TV_GLOVES */
#define SV_SET_OF_LEATHER_GLOVES	1
#define SV_SET_OF_GAUNTLETS		2
#define SV_SET_OF_CESTI			5

/* The "sval" codes for TV_SOFT_ARMOR */
#define SV_FILTHY_RAG			1
#define SV_ROBE				2
#define SV_SOFT_LEATHER_ARMOR		4
#define SV_SOFT_STUDDED_LEATHER		5
#define SV_HARD_LEATHER_ARMOR		6
#define SV_HARD_STUDDED_LEATHER		7
#define SV_LEATHER_SCALE_MAIL		11

/* The "sval" codes for TV_HARD_ARMOR */
#define SV_RUSTY_CHAIN_MAIL		1	/* 14- */
#define SV_METAL_SCALE_MAIL		3	/* 13 */
#define SV_CHAIN_MAIL			4	/* 14 */
#define SV_AUGMENTED_CHAIN_MAIL		6	/* 16 */
#define SV_DOUBLE_CHAIN_MAIL		7	/* 16 */
#define SV_BAR_CHAIN_MAIL		8	/* 18 */
#define SV_METAL_BRIGANDINE_ARMOUR	9	/* 19 */
#define SV_PARTIAL_PLATE_ARMOUR		12	/* 22 */
#define SV_METAL_LAMELLAR_ARMOUR	13	/* 23 */
#define SV_FULL_PLATE_ARMOUR		15	/* 25 */
#define SV_RIBBED_PLATE_ARMOUR		18	/* 28 */
#define SV_MITHRIL_CHAIN_MAIL		20	/* 28+ */
#define SV_MITHRIL_PLATE_MAIL		25	/* 35+ */
#define SV_ADAMANTITE_PLATE_MAIL	30	/* 40+ */

/* The "sval" codes for TV_DRAG_ARMOR */
#define SV_DRAGON_BLACK			1
#define SV_DRAGON_BLUE			2
#define SV_DRAGON_WHITE			3
#define SV_DRAGON_RED			4
#define SV_DRAGON_GREEN			5
#define SV_DRAGON_MULTIHUED		6
#define SV_DRAGON_SHINING		10
#define SV_DRAGON_LAW			12
#define SV_DRAGON_BRONZE		14
#define SV_DRAGON_GOLD			16
#define SV_DRAGON_CHAOS			18
#define SV_DRAGON_BALANCE		20
#define SV_DRAGON_POWER			30

/* The "sval" codes for TV_AMULET */
#define SV_AMULET_WISDOM	0
#define SV_AMULET_CHARISMA	1
#define SV_AMULET_SEARCHING	2

#define SV_AMULET_THE_MAGI	8
#define SV_AMULET_DOOM		9
#define SV_AMULET_CARLAMMAS	10
#define SV_AMULET_INGWE		11
#define SV_AMULET_DWARVES	12

/* The sval codes for TV_RING */
#define SV_RING_STR		0
#define SV_RING_DEX		1
#define SV_RING_CON		2
#define SV_RING_INT		3
#define SV_RING_SPEED		4
#define SV_RING_SEARCHING	5
#define SV_RING_TELEPORTATION	6
#define SV_RING_SLOW_DIGESTION	7
#define SV_RING_RESIST_FIRE	8
#define SV_RING_RESIST_COLD	9
#define SV_RING_FEATHER_FALL	10
#define SV_RING_RESIST_POIS	11
#define SV_RING_FREE_ACTION	12
#define SV_RING_WEAKNESS	13
#define SV_RING_FLAMES		14
#define SV_RING_ACID		15
#define SV_RING_ICE		16
#define SV_RING_WOE		17
#define SV_RING_STUPIDITY	18
#define SV_RING_DAMAGE		19
#define SV_RING_ACCURACY	20
#define SV_RING_PROTECTION	21
#define SV_RING_AGGRAVATION	22
#define SV_RING_SEE_INVIS	23
#define SV_RING_SUSTAIN_STR	24
#define SV_RING_SUSTAIN_INT	25
#define SV_RING_SUSTAIN_WIS	26
#define SV_RING_SUSTAIN_DEX	27
#define SV_RING_SUSTAIN_CON	28
#define SV_RING_SUSTAIN_CHR	29
#define SV_RING_SLAYING		30
#define SV_RING_BARAHIR		32
#define SV_RING_TULKAS		33
#define SV_RING_NARYA		34
#define SV_RING_NENYA		35
#define SV_RING_VILYA		36
#define SV_RING_POWER		37




/* The "sval" codes for TV_STAFF */
#define SV_STAFF_LITE		0
#define SV_STAFF_DOOR_STAIR_LOC	1
#define SV_STAFF_TRAP_LOC	2
#define SV_STAFF_TREASURE_LOC	3
#define SV_STAFF_OBJECT_LOC	4
#define SV_STAFF_TELEPORTATION	5
#define SV_STAFF_EARTHQUAKES	6
#define SV_STAFF_SUMMONING	7
#define SV_STAFF_DESTRUCTION	8
#define SV_STAFF_STARLITE	9
#define SV_STAFF_HASTE_MONSTERS	10
#define SV_STAFF_SLOW_MONSTERS	11
#define SV_STAFF_SLEEP_MONSTERS	12
#define SV_STAFF_CURE_LIGHT	13
#define SV_STAFF_DETECT_INVIS	14
#define SV_STAFF_SPEED		15
#define SV_STAFF_SLOWNESS	16
#define SV_STAFF_REMOVE_CURSE	18
#define SV_STAFF_DETECT_EVIL	19
#define SV_STAFF_CURING		20
#define SV_STAFF_DISPEL_EVIL	21
#define SV_STAFF_DARKNESS	22
#define SV_STAFF_GENOCIDE	23
#define SV_STAFF_POWER		24
#define SV_STAFF_THE_MAGI	25
#define SV_STAFF_IDENTIFY	26
#define SV_STAFF_HOLINESS	27
#define SV_STAFF_MAPPING	28
#define SV_STAFF_HEALING	29
#define SV_STAFF_PROBING	30

/* The "sval" codes for TV_WAND */
#define SV_WAND_LITE		0
#define SV_WAND_ELEC		1
#define SV_WAND_COLD		2
#define SV_WAND_FIRE		3
#define SV_WAND_STONE_TO_MUD	4
#define SV_WAND_POLYMORPH	5
#define SV_WAND_HEAL_MONSTER	6
#define SV_WAND_HASTE_MONSTER	7
#define SV_WAND_SLOW_MONSTER	8
#define SV_WAND_CONFUSE_MONSTER	9
#define SV_WAND_SLEEP_MONSTER	10
#define SV_WAND_DRAIN_LIFE	11
#define SV_WAND_TRAP_DOOR_DEST	12
#define SV_WAND_MAGIC_MISSILE	13
#define SV_WAND_FEAR_MONSTER	14
#define SV_WAND_CLONE_MONSTER	15
#define SV_WAND_TELEPORT_AWAY	16
#define SV_WAND_DISARMING	17
#define SV_WAND_ELEC_BALL	18
#define SV_WAND_COLD_BALL	19
#define SV_WAND_FIRE_BALL	20
#define SV_WAND_STINKING_CLOUD	21
#define SV_WAND_ACID_BALL	22
#define SV_WAND_WONDER		23
#define SV_WAND_DRAGON_FIRE	24
#define SV_WAND_DRAGON_COLD	25
#define SV_WAND_DRAGON_BREATH	26
#define SV_WAND_ANNIHILATION	27
#define SV_WAND_ACID		28

/* The "sval" codes for TV_ROD */
#define SV_ROD_LIGHT		0
#define SV_ROD_ELEC		1
#define SV_ROD_COLD		2
#define SV_ROD_FIRE		3
#define SV_ROD_POLYMORPH	4
#define SV_ROD_SLOW_MONSTER	5
#define SV_ROD_SLEEP_MONSTER	6
#define SV_ROD_DRAIN_LIFE	7
#define SV_ROD_TELEPORT_AWAY	8
#define SV_ROD_DISARMING	9
#define SV_ROD_ELEC_BALL	10
#define SV_ROD_COLD_BALL	11
#define SV_ROD_FIRE_BALL	12
#define SV_ROD_ACID_BALL	13
#define SV_ROD_ACID		14
#define SV_ROD_MAPPING		15
#define SV_ROD_IDENTIFY		16
#define SV_ROD_CURING		17
#define SV_ROD_HEALING		18
#define SV_ROD_DETECTION	19
#define SV_ROD_RESTORATION	20
#define SV_ROD_SPEED		21
#define SV_ROD_ILLUMINATION	22
#define SV_ROD_PROBING		23
#define SV_ROD_RECALL		24
#define SV_ROD_TRAP_LOC		25
#define SV_ROD_MK_WALL		26


/* Some extra "sval" codes */
#define SV_BOOK			64	/* First Book */
#define SV_FOOD_MIN_MUSH	64+0	/* First Flavored Mushroom */
#define SV_FOOD_MIN_MOLD	64+16	/* First Flavored Mold */
#define SV_FOOD_MIN_FOOD	64+32	/* First Normal Food */


/* The "sval" codes for traps */
#define SV_TRAP_PIT		1
#define SV_TRAP_ARROW		2
#define SV_TRAP_COVERED_PIT	3
#define SV_TRAP_TRAP_DOOR	4
#define SV_TRAP_GAS_SLEEP	5
#define SV_TRAP_LOOSE_ROCK	6
#define SV_TRAP_DART_STR	7
#define SV_TRAP_TELEPORT	8
#define SV_TRAP_FALLING_ROCK	9
#define SV_TRAP_GAS_ACID	10
#define SV_TRAP_SUMMON		11
#define SV_TRAP_FIRE		12
#define SV_TRAP_ACID		13
#define SV_TRAP_GAS_POISON	14
#define SV_TRAP_GAS_BLIND	15
#define SV_TRAP_GAS_CONFUSE	16
#define SV_TRAP_DART_SLOW	17
#define SV_TRAP_DART_CON	18
#define SV_TRAP_GLYPH           63


/*
 * The "TR_xxx" values apply ONLY to the items with tval's between
 * TV_MIN_WEAR and TV_MAX_WEAR, that is, items which can be wielded
 * or worn.  Use the macro "wearable_p()" to check this condition.
 * The macros "artifact_p()" and "cursed_p()" call "wearable_p()".
 *
 * Note that chests no longer use the "TR_xxx" flags, see "CH2_xxx" below.
 *
 * Note that "flags1" contains all flags dependant on "pval", plus all "extra attack damage"
 * flags (SLAY_XXX and BRAND_XXX).
 *
 * Note that "flags2" contains all "resistances" Note that "Hold Life" is really an
 * "immunity" to ExpLoss, and "Free Action" is "immunity to paralysis".
 *
 * Note that "flags3" contains everything else.  Also "FEATHER" floating.
 */
#define TR_STATS        0x0000003FL /* the stats must be the low 6 bits */
#define TR1_STR			0x00000001L	/* Uses "pval" */
#define TR1_INT			0x00000002L	/* Uses "pval" */
#define TR1_WIS			0x00000004L	/* Uses "pval" */
#define TR1_DEX			0x00000008L	/* Uses "pval" */
#define TR1_CON			0x00000010L	/* Uses "pval" */
#define TR1_CHR			0x00000020L	/* Uses "pval" */
#define TR1_STEALTH		0x00000100L	/* Uses "pval" */
#define TR1_SEARCH		0x00000200L	/* Uses "pval" */
#define TR1_INFRA		0x00000400L	/* Uses "pval" */
#define TR1_TUNNEL		0x00000800L	/* Uses "pval" */
#define TR1_SPEED		0x00001000L	/* Uses "pval" */
#define TR1_ATTACK_SPD		0x00002000L	/* Uses "pval" */
#define TR1_SLAY_ANIMAL		0x00010000L
#define TR1_SLAY_EVIL		0x00020000L
#define TR1_SLAY_UNDEAD		0x00040000L
#define TR1_SLAY_DEMON		0x00080000L
#define TR1_SLAY_ORC		0x00100000L
#define TR1_SLAY_TROLL		0x00200000L
#define TR1_SLAY_GIANT		0x00400000L
#define TR1_SLAY_DRAGON		0x00800000L
#define TR1_KILL_DRAGON		0x01000000L	/* Execute Dragon */
#define TR1_IMPACT		0x04000000L	/* Start Earthquakes */
#define TR1_BRAND_ELEC		0x20000000L
#define TR1_BRAND_FIRE		0x40000000L
#define TR1_BRAND_COLD		0x80000000L

#define TR2_SUST_STR		0x00000001L
#define TR2_SUST_INT		0x00000002L
#define TR2_SUST_WIS		0x00000004L
#define TR2_SUST_DEX		0x00000008L
#define TR2_SUST_CON		0x00000010L
#define TR2_SUST_CHR		0x00000020L
#define TR2_IM_ACID		0x00000100L
#define TR2_IM_ELEC		0x00000200L
#define TR2_IM_FIRE		0x00000400L
#define TR2_IM_COLD		0x00000800L
#define TR2_IM_POIS		0x00001000L
#define TR2_FREE_ACT		0x00004000L	/* Free Action */
#define TR2_HOLD_LIFE	 	0x00008000L	/* Hold Life */

#define TR2_RES_ACID		0x00010000L
#define TR2_RES_ELEC		0x00020000L
#define TR2_RES_FIRE		0x00040000L
#define TR2_RES_COLD		0x00080000L
#define TR2_RES_POIS		0x00100000L
#define TR2_RES_FEAR		0x00200000L
#define TR2_RES_LITE		0x00400000L	/* Oops */
#define TR2_RES_DARK		0x00800000L	/* Oops */

#define TR2_RES_BLIND		0x01000000L	/* Oops */
#define TR2_RES_CONF		0x02000000L	/* Oops */
#define TR2_RES_SOUND		0x04000000L	/* Oops */
#define TR2_RES_SHARDS		0x08000000L	/* Oops */

#define TR2_RES_NETHER		0x10000000L	/* Oops */
#define TR2_RES_NEXUS		0x20000000L	/* Oops */
#define TR2_RES_CHAOS		0x40000000L	/* Oops */
#define TR2_RES_DISEN		0x80000000L	/* Oops */


#define TR3_FEATHER	 	0x00001000L	/* Feather Falling */
#define TR3_LITE		0x00002000L	/* Permanent Light */
#define TR3_SEE_INVIS		0x00004000L	/* See Invisible */
#define TR3_TELEPATHY		0x00008000L	/* Telepathy */
#define TR3_SLOW_DIGEST		0x00010000L	/* Item slows down digestion */
#define TR3_REGEN		0x00020000L	/* Item induces regeneration */
#define TR3_ACTIVATE		0x01000000L	/* Item can be activated */
#define TR3_TELEPORT		0x04000000L	/* Item teleports player */
#define TR3_AGGRAVATE		0x08000000L	/* Item aggravates monsters */
#define TR3_BLESSED		0x10000000L	/* Item is Blessed by the Gods */
#define TR3_CURSED		0x20000000L	/* Item is Cursed */

/* Mask for chest traps (flags2) */
#define CH2_TRAP_MASK      0x000001F0L

/* Definitions for chests (flags2) */
#define CH2_LOCKED		0x00000001L	/* Chest is locked */
#define CH2_DISARMED		0x00000002L	/* Chest has been disarmed */

/* Chest Trap Flags (flags2) */
#define CH2_LOSE_STR		0x00000010L
#define CH2_POISON		0x00000020L
#define CH2_PARALYSED		0x00000040L
#define CH2_EXPLODE		0x00000080L
#define CH2_SUMMON		0x00000100L

/* Chests -- Hack from pre-2.7.0 */
#define CH1_CARRY_OBJ   	0x01000000L
#define CH1_CARRY_GOLD		0x02000000L
#define CH1_HAS_60		0x04000000L
#define CH1_HAS_90		0x08000000L
#define CH1_HAS_1D2		0x10000000L
#define CH1_HAS_2D2		0x20000000L
#define CH1_HAS_4D2		0x40000000L



/*
 * Spell types used by project(), and related functions.
 */
#define GF_ELEC         1
#define GF_POIS         2
#define GF_ACID         3
#define GF_COLD         4
#define GF_FIRE         5
#define GF_MISSILE      10
#define GF_ARROW        11
#define GF_PLASMA       12
#define GF_HOLY_ORB     13
#define GF_WATER        14
#define GF_LITE         15
#define GF_DARK         16
#define GF_SHARDS       20
#define GF_SOUND        21
#define GF_CONFUSION    22
#define GF_FORCE        23
#define GF_INERTIA      24
#define GF_MANA         26
#define GF_METEOR       27
#define GF_ICE          28
#define GF_CHAOS        30
#define GF_NETHER       31
#define GF_DISENCHANT   32
#define GF_NEXUS        33
#define GF_TIME         34
#define GF_GRAVITY      35

#define GF_KILL_DOOR	41
#define GF_KILL_TRAP	42

#define GF_OLD_TPORT	51
#define GF_OLD_POLY	52
#define GF_OLD_CONF	56
#define GF_OLD_SCARE	57
#define GF_OLD_SLEEP	58
#define GF_OLD_DRAIN	59


/*
 * Bit flags for the "project()" function
 *
 *   BEAM: Work as a beam weapon -- affect every grid passed through
 *   HIDE: Do not "show" our progress (no visual feedback to player)
 *   STOP: Stop as soon as we hit a monster (used for "bolts")
 *   THRU: Continue "through" the targer (used for "bolts"/"beams")
 *   GRID: Affect each grid in the "blast area" in some way
 *   ITEM: Affect each item in the "blast area" in some way
 *   ONLY: Only affect the grid/item if there is no monster there
 *   XTRA: Do "extra" damage to player if he is in the "blast area"
 *
 * Notes:
 *   THRU, without STOP, means "go until we hit a wall or something"
 *   
 */
#define PROJECT_BEAM	0x01
#define PROJECT_HIDE	0x02
#define PROJECT_STOP	0x04
#define PROJECT_THRU	0x08
#define PROJECT_GRID	0x10
#define PROJECT_ITEM	0x20
#define PROJECT_ONLY	0x40
#define PROJECT_XTRA	0x80




/* bit flags used in my revamped enchant() code -CFT */
#define ENCH_TOHIT   0x01
#define ENCH_TODAM   0x02
#define ENCH_TOAC    0x04



/*
 * Adaptation of the old monster.h file
 *
 * Definitions of various monster flags
 *
 * Note that MF1_WINNER is used to see if the monster will drop
 * distinguished objects, while MF2_QUESTOR is used to see if
 * killing the monster satisfies any special quest, including
 * the one that wins the game.  That is, you win the game when
 * you manage to kill the last Quest Monster.
 *
 * Note that the code has been simplified quite a lot by first requiring
 * that the "WINNER" Monster be treated as a "Quest", and thus be set as
 * "QUESTOR".  Now the "WINNER" flag means only that he drops special stuff.
 * While the "QUESTOR" flag, among other things, means never out of depth.
 */



#define MF1_MV_ONLY_ATT		0x00000001L /* The monster does not move */
#define MF1_MV_ATT_NORM 	0x00000002L /* The monster moves normally */
#define MF1_MV_20	        0x00000004L /* 20% random */
#define MF1_MV_40		0x00000008L /* 40% random */
#define MF1_MV_75		0x00000010L /* 75% random */
#define MF1_MV_INVIS		0x00010000L
#define MF1_THRO_DR		0x00020000L
#define MF1_THRO_WALL		0x00040000L
#define MF1_THRO_CREAT		0x00080000L
#define MF1_PICK_UP		0x00100000L
#define MF1_MULTIPLY		0x00200000L
/* 2 bits missing */
#define MF1_CARRY_OBJ   	0x01000000L
#define MF1_CARRY_GOLD		0x02000000L
#define MF1_HAS_60		0x04000000L
#define MF1_HAS_90		0x08000000L
#define MF1_HAS_1D2		0x10000000L
#define MF1_HAS_2D2		0x20000000L
#define MF1_HAS_4D2		0x40000000L
#define MF1_WINNER		0x80000000L

#define MF2_ANIMAL		0x00000001L
#define MF2_EVIL		0x00000002L
#define MF2_ORC			0x00000004L
#define MF2_UNDEAD		0x00000008L
#define MF2_DRAGON		0x00000010L
#define MF2_DEMON		0x00000020L
#define MF2_HURT_LITE		0x00000040L
#define	MF2_HURT_ROCK		0x00000080L
#define MF2_CHARM_SLEEP		0x00000100L
#define MF2_NO_INFRA		0x00000200L
#define MF2_MAX_HP		0x00000400L
#define MF2_IM_COLD		0x00000800L
#define MF2_IM_FIRE		0x00001000L
#define MF2_IM_ELEC		0x00002000L
#define MF2_IM_POIS		0x00004000L
#define MF2_IM_ACID		0x00008000L
#define MF2_TROLL		0x00010000L
#define MF2_GIANT		0x00020000L
#define MF2_SPECIAL       	0x00040000L
#define MF2_GROUP         	0x00080000L
#define MF2_GOOD           	0x00100000L
#define MF2_BREAK_WALL		0x00200000L
#define MF2_DESTRUCT		0x00400000L
#define MF2_QUESTOR		0x00800000L /* killing it completes a quest */
#define MF2_INTELLIGENT		0x01000000L /* casts spells intelligently   */
#define MF2_MINDLESS		0x02000000L /* does not have a mind -CWS    */
/* 5 bits missing */
#define MF2_UNIQUE		0x80000000L /* unique monster */

/* r_ptr->spells1 */
#define MS1_BLINK		0x00000010L
#define MS1_TELEPORT		0x00000020L
#define MS1_TELE_TO	 	0x00000040L
#define MS1_CAUSE_1		0x00000080L
#define MS1_CAUSE_2		0x00000100L
#define MS1_HOLD		0x00000200L
#define MS1_BLIND		0x00000400L
#define MS1_CONF		0x00000800L
#define MS1_FEAR		0x00001000L
#define MS1_S_MONSTER		0x00002000L
#define MS1_S_UNDEAD		0x00004000L
#define MS1_SLOW		0x00008000L
#define MS1_MANA_DRAIN		0x00010000L
#define MS1_S_DEMON		0x00020000L
#define MS1_S_DRAGON		0x00040000L
#define MS1_BR_ELEC		0x00080000L
#define MS1_BR_POIS		0x00100000L
#define MS1_BR_ACID		0x00200000L
#define MS1_BR_COLD		0x00400000L
#define MS1_BR_FIRE		0x00800000L
#define MS1_BO_FIRE   		0x01000000L
#define MS1_BO_COLD		0x02000000L
#define MS1_BO_ACID     	0x04000000L
#define MS1_ARROW_1      	0x08000000L
#define MS1_CAUSE_3    		0x10000000L
#define MS1_BA_FIRE     	0x20000000L
#define MS1_BA_COLD    		0x40000000L
#define MS1_BO_MANA     	0x80000000L

/* r_ptr->spells2 */
#define MS2_BR_CHAO   		0x00000001L /* Chaos */
#define MS2_BR_SHAR    		0x00000002L /* Shards */
#define MS2_BR_SOUN    		0x00000004L /* Sound */
#define MS2_BR_CONF   		0x00000008L /* Confusion */
#define MS2_BR_DISE 		0x00000010L /* Disenchantment */
#define MS2_BR_LIFE  		0x00000020L /* Life Draining */
#define MS2_BO_ELEC 		0x00000040L
#define MS2_BA_ELEC   		0x00000080L
#define MS2_BA_ACID    		0x00000100L
#define MS2_TRAP_CREATE  	0x00000200L
#define MS2_RAZOR        	0x00000400L
#define MS2_MIND_BLAST   	0x00000800L
#define MS2_TELE_AWAY    	0x00001000L
#define MS2_HEAL         	0x00002000L
#define MS2_HASTE        	0x00004000L
#define MS2_ARROW_2      	0x00008000L
#define MS2_BO_PLAS  		0x00010000L
#define MS2_S_SUMMON       	0x00020000L
#define MS2_BO_NETH  		0x00040000L
#define MS2_BO_ICEE     	0x00080000L
#define MS2_DARKNESS     	0x00100000L
#define MS2_FORGET       	0x00200000L
#define MS2_BRAIN_SMASH  	0x00400000L
#define MS2_BA_POIS     	0x00800000L
#define MS2_TELE_LEVEL     	0x01000000L
#define MS2_BO_WATE   		0x02000000L
#define MS2_BA_WATE   		0x04000000L
#define MS2_BA_NETH  		0x08000000L
#define MS2_S_ANGEL      	0x10000000L
#define MS2_S_SPIDER     	0x20000000L
#define MS2_S_HOUND      	0x40000000L
#define MS2_BR_NETH    		0x80000000L /* Nexus */

/* m_ptr->spells3 */
#define MS3_BR_WALL   		0x00000001L /* Wall Building */
#define MS3_BR_SLOW   	 	0x00000002L /* Slowness */
#define MS3_BR_LITE    		0x00000004L /* Light */
#define MS3_BR_TIME    		0x00000008L /* Time */
#define MS3_BR_GRAV    		0x00000010L /* Gravity */
#define MS3_BR_DARK    		0x00000020L /* Darkness */
#define MS3_BR_PLAS    		0x00000040L /* Plasma */
#define MS3_ARROW_3        	0x00000080L /* fires an arrow */
#define MS3_S_WRAITH     	0x00000100L /* Summon ringwraiths */
#define MS3_DARK_STORM   	0x00000200L /* Big darkness breath */
#define MS3_MANA_STORM   	0x00000400L /* Mana storm */
#define MS3_S_REPTILE    	0x00000800L /* Summon reptiles */
#define MS3_S_ANT	     	0x00001000L /* Summon ants/ant lions */
#define MS3_S_UNIQUE     	0x00002000L /* Summon uniques */
#define MS3_S_GUNDEAD    	0x00004000L /* Summon greater undead */
#define MS3_S_ANCIENTD   	0x00008000L /* Summon ancient dragon */

/*
 * Eight bits of zeros
 */
#define NONE8			0x00000000L


/*
 * The "recall" of monster memory is a MESS
 */

/* Hack -- scan for "movement" */
#define CM1_ALL_MV_FLAGS 0x0000001FL
#define CM1_RANDOM_MOVE  0x0000001CL

/* Hack -- scan for "special movement" */
#define CM1_SPECIAL      0x003F0000L

/* Hack -- used to "count" treasures */
#define CM1_TREASURE     0x7C000000L
#define CM1_TR_SHIFT     26

/* Hack -- used to "count" spell attacks */
#define CS1_FREQ        0x0000000FL

/* Hack -- separate out the "breath" spells */
#define CS1_BREATHE     0x00F80000L
#define CS2_BREATHE     0x8000003FL
#define CS3_BREATHE     0x0000007FL

/* Hack -- take note of "intelligent" spells */
#define CS1_INT         0x0006FC30L     /* was 0x80060020L -DGK */
#define CS2_INT         0x71027200L     /* was 0x51023400L -DGK */
#define CS3_INT         0x0000F900L     /* was 0x00000000L -DGK */


