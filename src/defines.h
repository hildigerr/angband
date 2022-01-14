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


#ifndef FUZZY
#define FUZZY 2
#endif

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
#define STORE_MAX_INVEN  18     /* Max diff objs in stock for auto buy   */
#define STORE_MIN_INVEN  10     /* Min diff objs in stock for auto sell  */
#define STORE_TURN_AROUND 9     /* Amount of buying and selling normally */

#define MAX_QUESTS        4     /* only 1 defined anyway --CFT */
#define DEFINED_QUESTS    1 
#define SAURON_QUEST      0 
#define Q_PLANE          -1

/* Treasure constants                                           */
#define INVEN_ARRAY_SIZE 34     /* Size of inventory array(Do not change) */
#define MAX_OBJ_LEVEL   255     /* Maximum level of magic in dungeon      */

/*
 * Hack -- this is the chance (1/GREAT_OBJ) that when an item is
 * created by "get_obj_num()", that the "level" will be "enhanced"
 * by quite a large amount.  This will allow, for example, a ring
 * of speed to be found at 50 feet.  
 */
#define GREAT_OBJ        11     /* 1/Chance of item being a Great Item  */

/* Number of dungeon objects */
#define MAX_DUNGEON_OBJ  423

/*
 * Note that the following constants are all related, if you change one,
 * you must also change all succeeding ones.  Also, player_init[] and
 * store_choice[] may also have to be changed.
 */

#define OBJ_OPEN_DOOR           (MAX_DUNGEON_OBJ+23)
#define OBJ_CLOSED_DOOR         (MAX_DUNGEON_OBJ+24)
#define OBJ_SECRET_DOOR         (MAX_DUNGEON_OBJ+25)
#define OBJ_UP_STAIR            (MAX_DUNGEON_OBJ+26)
#define OBJ_DOWN_STAIR          (MAX_DUNGEON_OBJ+27)
#define OBJ_STORE_DOOR          (MAX_DUNGEON_OBJ+28)
#define OBJ_TRAP_LIST           (MAX_DUNGEON_OBJ+36)
#define OBJ_RUBBLE              (MAX_DUNGEON_OBJ+54)
#define OBJ_MUSH                (MAX_DUNGEON_OBJ+55)
#define OBJ_SCARE_MON           (MAX_DUNGEON_OBJ+56)
#define OBJ_GOLD_LIST           (MAX_DUNGEON_OBJ+57)
#define OBJ_NOTHING             (MAX_DUNGEON_OBJ+75)
#define OBJ_RUINED_CHEST        (MAX_DUNGEON_OBJ+76)
#define OBJ_WIZARD              (MAX_DUNGEON_OBJ+77)
/*Special start for rings amulets etc... */
#define OBJ_SPECIAL         	(MAX_DUNGEON_OBJ+79)
  /* Number of objects for universe*/
#define MAX_OBJECTS				(MAX_DUNGEON_OBJ+90)


/* was 7*64, see flavor_p() in desc.c, could be MAX_OBJECTS o_o() rewritten
 * now 8*64 beacuse of Rods
 */

#define OBJECT_IDENT_SIZE 1024



#define MAX_GOLD       18       /* Number of different types of gold     */



#define MAX_TALLOC      400     /* Max objects per level               */
#define MIN_TRIX          1     /* Minimum i_list index used           */
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
 * with MAX_TALLOC 150, it is possible to get compacting objects during
 * level generation, although it is extremely rare
 */

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
#define MAX_MON_NATTACK     4   /* Max num attacks (used in mons memory)    */
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
#define INVEN_LIGHT     32
#define INVEN_AUX       33




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
 * The "savefiles" in particular, will be totally trashed
 * and MIN_WALL will have to be changed
 */

#define NULL_WALL	0	/* Temp value for "generate.c" */

#define DARK_FLOOR      1
#define LIGHT_FLOOR     2
#define NT_DARK_FLOOR   3
#define NT_LIGHT_FLOOR  4
#define MAX_CAVE_ROOM   4
#define CORR_FLOOR      5	/* Floor, in a corridor */
#define BLOCKED_FLOOR   6       /* a corridor space with cl/st/se door or rubble */
#define MAX_CAVE_FLOOR  6

#define MAX_OPEN_SPACE  5
#define MIN_CLOSED_SPACE 6

#define TMP1_WALL	8
#define TMP2_WALL	9

#define MIN_WALL	12	/* Hack -- minimum "wall" fval */

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

/* definitions for objects that can be worn */
#define TR_STATS        0x0000003FL /* the stats must be the low 6 bits */
#define TR1_STR          0x00000001L
#define TR1_INT          0x00000002L
#define TR1_WIS          0x00000004L
#define TR1_DEX          0x00000008L
#define TR1_CON          0x00000010L
#define TR1_CHR          0x00000020L
#define TR1_SEARCH       0x00000040L
#define TR3_SLOW_DIGEST  0x00000080L
#define TR1_STEALTH      0x00000100L
#define TR3_AGGRAVATE    0x00000200L
#define TR3_TELEPORT     0x00000400L
#define TR3_REGEN        0x00000800L
#define TR1_SPEED        0x00001000L

#define TR_EGO_WEAPON   0x0007E000L
#define TR1_SLAY_DRAGON  0x00002000L
#define TR1_SLAY_ANIMAL  0x00004000L
#define TR1_SLAY_EVIL    0x00008000L
#define TR1_SLAY_UNDEAD  0x00010000L
#define TR1_BRAND_COLD  0x00020000L
#define TR1_BRAND_FIRE 0x00040000L

#define TR2_RES_FIRE     0x00080000L
#define TR2_RES_ACID     0x00100000L
#define TR2_RES_COLD     0x00200000L
#define TR_SUST_STAT    0x00400000L
#define TR2_FREE_ACT     0x00800000L
#define TR3_SEE_INVIS    0x01000000L
#define TR2_RES_ELEC    0x02000000L
#define TR3_FEATHER        0x04000000L
#define TR1_SLAY_X_DRAGON        0x08000000L
#define TR2_RES_POIS       0x10000000L
#define TR1_TUNNEL       0x20000000L
#define TR1_INFRA        0x40000000L
#define TR3_CURSED       0x80000000L

/* flags for flags2 */
#define TR1_SLAY_DEMON   0x00000001L
#define TR1_SLAY_TROLL   0x00000002L
#define TR1_SLAY_GIANT   0x00000004L
#define TR2_HOLD_LIFE    0x00000008L
#define TR1_SLAY_ORC     0x00000010L
#define TR3_TELEPATHY    0x00000020L
#define TR2_IM_FIRE      0x00000040L
#define TR2_IM_COLD      0x00000080L
#define TR2_IM_ACID      0x00000100L
#define TR2_IM_ELEC     0x00000200L
#define TR3_LITE        0x00000400L
#define TR3_ACTIVATE     0x00000800L
#define TR3_LITENING    0x00001000L
#define TR1_IMPACT       0x00002000L
#define TR2_IM_POIS    0x00004000L
#define TR2_RES_CONF     0x00008000L
#define TR2_RES_SOUND    0x00010000L
#define TR2_RES_LITE       0x00020000L
#define TR2_RES_DARK     0x00040000L
#define TR2_RES_CHAOS    0x00080000L
#define TR2_RES_DISEN       0x00100000L
#define TR2_RES_SHARDS   0x00200000L
#define TR2_RES_NEXUS    0x00400000L
#define TR2_RES_BLIND    0x00800000L
#define TR2_RES_NETHER   0x01000000L
#define TR_ARTIFACT     0x02000000L /* means "is an artifact" -CFT */
#define TR_BLESS_BLADE  0x04000000L /* priests use w/o penalty -DGK*/
#define TR1_ATTACK_SPD   0x08000000L /* extra attacks/round -DGK */
#define TR2_RES_FEAR     0x10000000L

/* definitions for chests */
#define CH_LOCKED       0x00000001L
#define CH_TRAPPED      0x000001F0L
#define CH_LOSE_STR     0x00000010L
#define CH_POISON       0x00000020L
#define CH_PARALYSED    0x00000040L
#define CH_EXPLODE      0x00000080L
#define CH_SUMMON       0x00000100L

/* definitions for creatures, cmove field */
#define CM_ALL_MV_FLAGS 0x0000001FL
#define CM_ATTACK_ONLY  0x00000001L
#define CM_MOVE_NORMAL  0x00000002L

#define CM_RANDOM_MOVE  0x0000001CL
#define CM_20_RANDOM    0x00000004L
#define CM_40_RANDOM    0x00000008L
#define CM_75_RANDOM    0x00000010L

#define CM_SPECIAL      0x003F0000L
#define CM_INVISIBLE    0x00010000L
#define CM_OPEN_DOOR    0x00020000L
#define CM_PHASE        0x00040000L
#define CM_EATS_OTHER   0x00080000L
#define CM_PICKS_UP     0x00100000L
#define CM_MULTIPLY     0x00200000L

#define CM_CARRY_OBJ    0x01000000L
#define CM_CARRY_GOLD   0x02000000L
#define CM_TREASURE     0x7C000000L
#define CM_TR_SHIFT     26              /* used for recall of treasure */
#define CM_60_RANDOM    0x04000000L
#define CM_90_RANDOM    0x08000000L
#define CM_1D2_OBJ      0x10000000L
#define CM_2D2_OBJ      0x20000000L
#define CM_4D2_OBJ      0x40000000L
#define CM_WIN          0x80000000L

/* creature spell definitions */
#define CS_FREQ         0x0000000FL
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

#define CS_INT1         0x0006FC30L     /* was 0x80060020L -DGK */
#define CS_INT2         0x71027200L     /* was 0x51023400L -DGK */
#define CS_INT3         0x0000F900L     /* was 0x00000000L -DGK */
#define CS_BREATHE      0x00F80000L
#define CS_BREATHE2     0x8000003FL
#define CS_BREATHE3     0x0000007FL
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


/* inventory stacking svals
 * these never stack:
 */

#define ITEM_NEVER_STACK_MIN    0
#define ITEM_NEVER_STACK_MAX    63

/* these items always stack with others of same sval, always treated as
 * single objects, must be power of 2
 */

#define ITEM_SINGLE_STACK_MIN   64
#define ITEM_SINGLE_STACK_MAX   192     /* see NOTE below */


/* these items stack with others only if have same sval and same p1,
 * they are treated as a group for wielding, etc.
 */

#define ITEM_GROUP_MIN          192
#define ITEM_GROUP_MAX          255

/* NOTE: items with sval 192 are treated as single objects, but only stack
 * with others of same sval if have the same p1 value, only used for
 * torches
 */


/* Special "Item Description Flags" */

/* id's used for object description, stored in object_ident */
#define OD_TRIED        0x1
#define OD_KNOWN1       0x2

/* id's used for item description, stored in i_ptr->ident */
#define ID_MAGIK        0x1
#define ID_DAMD         0x2
#define ID_EMPTY        0x4	/* Item is now "empty" */
#define ID_KNOWN2       0x8	/* Item is fully "known" */
#define ID_STOREBOUGHT  0x10
#define ID_SHOW_HITDAM  0x20
#define ID_NOSHOW_P1    0x40    /* don't show (+x) even if p1 != 0 -CWS   */
#define ID_NOSHOW_TYPE  0x80    /* don't show (+x of yyy), just (+x) -CWS */

/* indexes into the special name table */
#define SN_NULL                 0
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

#define EGO_CLUMSINESS           16
#define EGO_WEAKNESS             17

#define EGO_SLOW_DESCENT	18
#define EGO_SPEED		19
#define EGO_STEALTH		20

#define EGO_SLOWNESS             21
#define EGO_NOISE                22
#define EGO_GREAT_MASS           23

#define EGO_INTELLIGENCE	24
#define EGO_WISDOM		25
#define EGO_INFRAVISION		26
#define EGO_MIGHT		27
#define EGO_LORDLINESS		28
#define EGO_MAGI		29
#define EGO_BEAUTY		30
#define EGO_SEEING		31
#define EGO_REGENERATION	32

#define EGO_STUPIDITY            33
#define EGO_DULLNESS             34
#define EGO_BLINDNESS            35
#define EGO_TIMIDNESS            36
#define EGO_TELEPORTATION        37
#define EGO_UGLINESS             38

#define EGO_PROTECTION		39

#define EGO_IRRITATION           40
#define EGO_VULNERABILITY        41
#define EGO_ENVELOPING           42

#define EGO_FIRE		43
#define EGO_SLAY_EVIL		44
#define EGO_DRAGON_SLAYING	45

#define EGO_EMPTY                46
#define EGO_LOCKED               47
#define EGO_POISON_NEEDLE        48
#define EGO_GAS_TRAP             49
#define EGO_EXPLOSION_DEVICE     50
#define EGO_SUMMONING_RUNES      51
#define EGO_MULTIPLE_TRAPS       52
#define EGO_DISARMED             53
#define EGO_UNLOCKED             54

#define EGO_SLAY_ANIMAL		55

#define ART_GROND                56
#define ART_RINGIL               57
#define ART_AEGLOS               58
#define ART_ARUNRUTH             59
#define ART_MORMEGIL             60
#define EGO_MORGUL               61
#define ART_ANGRIST              62
#define ART_GURTHANG             63
#define ART_CALRIS               64

#define EGO_ACCURACY		65

#define ART_ANDURIL              66

#define EGO_SLAY_O		67
#define ART_POWER		68

#define ART_DURIN                69
#define ART_AULE                 70

#define EGO_WEST		71
#define EGO_BLESS_BLADE		72
#define EGO_SLAY_DEMON		73
#define EGO_SLAY_T		74

#define ART_BLOODSPIKE           75
#define ART_THUNDERFIST          76

#define EGO_WOUNDING		77

#define ART_ORCRIST              78
#define ART_GLAMDRING            79
#define ART_STING                80

#define EGO_LIGHT		81
#define EGO_AGILITY		82

#define EGO_BACKBITING           83
#define ART_DOOMCALLER           84

#define EGO_SLAY_G		85
#define EGO_TELEPATHY		86

#define EGO_DRAGONKIND           87

#define ART_NENYA                88
#define ART_NARYA                89
#define ART_VILYA                90
#define EGO_AMAN                 91
#define ART_BELEGENNON           92
#define ART_FEANOR               93
#define ART_ANARION              94
#define ART_ISILDUR              95
#define ART_FINGOLFIN            96
#define EGO_ELVENKIND            97
#define ART_SOULKEEPER           98
#define ART_DOR_LOMIN            99
#define ART_MORGOTH             100
#define EGO_BELEG               101
#define ART_DAL                 102
#define ART_PAURHACH            103
#define ART_PAURNIMMEN          104
#define ART_PAURAEGEN           105
#define ART_CAMMITHRIM          106
#define ART_CAMBELEG            107
#define ART_HOLHENNETH          108
#define ART_PAURNEN             109
#define ART_AEGLIN              110
#define ART_CAMLOST             111
#define ART_NIMLOTH             112
#define ART_NAR                 113
#define ART_BERUTHIEL           114
#define ART_GORLIM              115
#define ART_NARTHANC            116
#define ART_NIMTHANC            117
#define ART_DETHANC             118
#define ART_GILETTAR            119
#define ART_RILIA               120
#define ART_BELANGIL            121
#define ART_BALLI               122
#define ART_LOTHARANG           123
#define ART_FIRESTAR            124
#define ART_ERIRIL              125
#define ART_CUBRAGOL            126
#define ART_BARD                127
#define ART_COLLUIN             128
#define ART_HOLCOLLETH          129
#define ART_TOTILA              130
#define ART_PAIN                131
#define ART_ELVAGIL             132
#define ART_AGLARANG            133
#define EGO_ROHAN               134
#define ART_EORLINGAS           135
#define ART_BARUKKHELED         136
#define ART_WRATH               137
#define ART_HARADEKKET          138
#define ART_MUNDWINE            139
#define ART_GONDRICAM           140
#define ART_ZARCUTHRA           141
#define ART_CARETH              142
#define ART_FORASGIL            143
#define ART_CRISDURIAN          144
#define ART_COLANNON            145
#define ART_HITHLOMIR           146
#define ART_THALKETTOTH         147
#define ART_ARVEDUI             148
#define ART_THRANDUIL           149
#define ART_THENGEL             150
#define ART_HAMMERHAND          151
#define ART_CELEGORM            152
#define ART_THROR               153
#define ART_MAEDHROS            154
#define ART_OLORIN              155
#define ART_ANGUIREL            156
#define ART_THORIN              157
#define ART_CELEBORN            158
#define ART_OROME               159
#define ART_EONWE               160
#define ART_GONDOR              161
#define ART_THEODEN             162
#define ART_THINGOL             163
#define ART_THORONGIL           164
#define ART_LUTHIEN             165
#define ART_TUOR                166
#define ART_ULMO                167
#define ART_OSONDIR             168
#define ART_TURMIL              169
#define ART_CASPANION           170
#define ART_TIL                 171
#define ART_DEATHWREAKER        172
#define ART_AVAVIR              173
#define ART_TARATOL             174
#define ART_RAZORBACK           175
#define ART_BLADETURNER         176
#define EGO_SHATTERED           177
#define EGO_BLASTED             178
#define EGO_ATTACKS             179
#define SN_ARRAY_SIZE          180 /* must be at end of this list */


/* The values for the treasure type (tval) field of various objects.
 */

#define TV_NEVER        -1	/* used by find_range() for non-search */
#define TV_NOTHING       0	/* Nothing (used in OBJ_NOTHING) */
#define TV_MISC          1
#define TV_CHEST         2	/* Chests ('~') */
#define TV_SPIKE         3	/* Spikes ('~') */

/* min tval for wearable items, all items between TV_MIN_WEAR and TV_MAX_WEAR
 * use the same flag bits, see the TR_* defines
 */

#define TV_MIN_WEAR     10	/* Min tval for "wearable" items */

/* items tested for enchantments, i.e. the MAGIK inscription, see the
 * enchanted() procedure
 */

#define TV_MIN_ENCHANT  10
#define TV_SHOT		10	/* Ammo for slings */
#define TV_BOLT         11	/* Ammo for x-bows */
#define TV_ARROW        12	/* Ammo for bows */
#define TV_LITE         15	/* Torches, Lanterns, Specials */
#define TV_BOW          20	/* Slings/Bows/Xbows */
#define TV_HAFTED       21	/* Priest Weapons */
#define TV_POLEARM      22	/* Axes and Pikes */
#define TV_SWORD        23	/* Edged Weapons */
#define TV_DIGGING      25	/* Shovels/Picks */
#define TV_BOOTS        30	/* Boots */
#define TV_GLOVES       31	/* Gloves */
#define TV_CLOAK        32	/* Cloaks */
#define TV_HELM         33	/* Helms/Crowns */
#define TV_SHIELD       34	/* Shields */
#define TV_HARD_ARMOR   35	/* Hard Armor */
#define TV_SOFT_ARMOR   36	/* Soft Armor */
/* max tval that uses the TR_* flags */
#define TV_MAX_ENCHANT  39
#define TV_AMULET       40
#define TV_RING         45
#define TV_MAX_WEAR     50   /* max tval for wearable items */
#define TV_STAFF        55
#define TV_WAND         65
#define TV_ROD          66
#define TV_SCROLL1      70
#define TV_SCROLL2      71
#define TV_POTION1      75
#define TV_POTION2      76
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
#define TV_RUBBLE       103	/* Rubble pile -- treated as a "wall" */
/* following objects are never deleted when trying to create another one
   during level generation */
#define TV_MIN_DOORS    104
#define TV_OPEN_DOOR    104	/* Open doorway */
#define TV_CLOSED_DOOR  105	/* Closed door -- treated as a "wall" */
#define TV_UP_STAIR     107	/* Staircase up */
#define TV_DOWN_STAIR   108	/* Staircase down */
#define TV_SECRET_DOOR  109	/* Secret door -- treated as a "wall" */
#define TV_STORE_DOOR   110	/* Entrance to store */
#define TV_MAX_VISIBLE  110




#define SV_STAFF_LITE        1L
#define SV_STAFF_DOOR_STAIR_LOC        2L
#define SV_STAFF_TRAP_LOC       3L
#define SV_STAFF_TREASURE_LOC       4L
#define SV_STAFF_OBJECT_LOC       5L
#define SV_STAFF_TELEPORTATION         6L
#define SV_STAFF_EARTHQUAKES        7L
#define SV_STAFF_SUMMONING       8L
#define SV_STAFF_DESTRUCTION         9L
#define SV_STAFF_STARLITE         10L
#define SV_STAFF_HASTE_MONSTERS      11L
#define SV_STAFF_SLOW_MONSTERS      12L
#define SV_STAFF_SLEEP_MONSTERS      13L
#define SV_STAFF_CURE_LIGHT      14L
#define SV_STAFF_DETECT_INVIS      15L
#define SV_STAFF_SPEED        16L
#define SV_STAFF_SLOWNESS         17L
#define SV_STAFF_REMOVE_CURSE       18L
#define SV_STAFF_DETECT_EVIL      19L
#define SV_STAFF_CURING       20L
#define SV_STAFF_DISPEL_EVIL      21L
#define SV_STAFF_DARKNESS         22L
#define SV_STAFF_GENOCIDE     23L
#define SV_STAFF_POWER        24L
#define SV_STAFF_THE_MAGI         25L
#define SV_STAFF_IDENTIFY     27L
#define SV_STAFF_HOLINESS     26L
#define SV_STAFF_MAPPING     28L
#define SV_STAFF_HEALING      29L
#define SV_STAFF_PROBING        30L

#define SV_WAND_LITE           1L
#define SV_WAND_ELEC      2L
#define SV_WAND_COLD      3L
#define SV_WAND_FIRE      4L
#define SV_WAND_STONE_TO_MUD       5L
#define SV_WAND_POLYMORPH         6L
#define SV_WAND_HEAL_MONSTER      7L
#define SV_WAND_HASTE_MONSTER      8L
#define SV_WAND_SLOW_MONSTER      9L
#define SV_WAND_CONFUSE_MONSTER      10L
#define SV_WAND_SLEEP_MONSTER      11L
#define SV_WAND_DRAIN_LIFE        12L
#define SV_WAND_TRAP_DOOR_DEST      13L
#define SV_WAND_MAGIC_MISSILE      14L
#define SV_WAND_FEAR_MONSTER      15L
#define SV_WAND_CLONE_MONSTER        16L
#define SV_WAND_TELEPORT_AWAY         17L
#define SV_WAND_DISARMING       18L
#define SV_WAND_ELEC_BALL      19L
#define SV_WAND_COLD_BALL      20L
#define SV_WAND_FIRE_BALL      21L
#define SV_WAND_STINKING_CLOUD       22L
#define SV_WAND_ACID_BALL      23L
#define SV_WAND_WONDER       24L
#define SV_WAND_DRAGON_FIRE     25L
#define SV_WAND_DRAGON_COLD     26L
#define SV_WAND_DRAGON_BREATH     27L
#define SV_WAND_ANNIHILATION        29L
#define SV_WAND_ACID      28L

#define SV_ROD_LIGHT           1L
#define SV_ROD_ELEC      2L
#define SV_ROD_COLD      3L
#define SV_ROD_FIRE      4L
#define RD_ST_MUD       5L
#define SV_ROD_POLYMORPH         6L
#define SV_ROD_SLOW_MONSTER      7L
#define RD_CONF_MN      8L
#define SV_ROD_SLEEP_MONSTER      9L
#define SV_ROD_DRAIN_LIFE        10L
#define RD_TR_DEST      11L
#define RD_MAG_MIS      12L
#define SV_ROD_TELEPORT_AWAY         13L
#define SV_ROD_DISARMING       14L
#define SV_ROD_ELEC_BALL      15L
#define SV_ROD_COLD_BALL      16L
#define SV_ROD_FIRE_BALL      17L
#define RD_ST_CLD       18L
#define SV_ROD_ACID_BALL      19L
#define SV_ROD_ACID      20L
#define RD_ANHIL        21L
#define SV_ROD_MAPPING      22L
#define SV_ROD_IDENTIFY        23L
#define SV_ROD_CURING         24L
#define SV_ROD_HEALING         25L
#define SV_ROD_DETECTION       26L
#define SV_ROD_RESTORATION      27L
#define SV_ROD_SPEED        28L
#define SV_ROD_ILLUMINATION       29L
#define SV_ROD_PROBING        30L
#define SV_ROD_RECALL       31L
#define SV_ROD_TRAP_LOC     32L
#define SV_ROD_MK_WALL      33L



/*
 * Spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
 */
#define GF_MISSILE		0
#define GF_ELEC         1
#define GF_POIS         2
#define GF_ACID         3
#define GF_COLD         4
#define GF_FIRE         5
#define GF_HOLY_ORB     6
#define GF_ARROW        7
#define GF_PLASMA       8
#define GF_NETHER       9
#define GF_WATER        10
#define GF_CHAOS        11
#define GF_SHARDS       12
#define GF_SOUND        13
#define GF_CONFUSION    14
#define GF_DISENCHANT   15
#define GF_NEXUS        16
#define GF_FORCE        17
#define GF_INERTIA      18
#define GF_LITE        19
#define GF_DARK         20
#define GF_TIME         21
#define GF_GRAVITY      22
#define GF_MANA         23
#define GF_METEOR       24
#define GF_ICE          25




/* bit flags used in my revamped enchant() code -CFT */
#define ENCH_TOHIT   0x01
#define ENCH_TODAM   0x02
#define ENCH_TOAC    0x04



/*
 * Adaptation of the old monster.h file
 */



#define NONE8       	0x00000000L
#define MV_ONLY_ATT    	0x00000001L
#define MV_ATT_NORM 	0x00000002L
#define MV_20	        0x00000004L
#define MV_40		0x00000008L
#define MV_75		0x00000010L
#define MV_INVIS	0x00010000L
#define THRO_DR		0x00020000L
#define THRO_WALL	0x00040000L
#define THRO_CREAT	0x00080000L
#define PICK_UP		0x00100000L
#define MULTIPLY        0x00200000L
#define CARRY_OBJ   	0x01000000L
#define CARRY_GOLD	0x02000000L
#define HAS_60		0x04000000L
#define HAS_90		0x08000000L
#define HAS_1D2		0x10000000L
#define HAS_2D2		0x20000000L
#define HAS_4D2		0x40000000L
#define WINNER		0x80000000L

#define BLINK		0x000010L
#define TELE		0x000020L
#define TELE_TO	 	0x000040L
#define CAUSE_LIGHT	0x000080L
#define CAUSE_SERIOUS	0x000100L
#define HOLD_PERSON	0x000200L
#define BLINDNESS	0x000400L
#define CONFUSION	0x000800L
#define FEAR		0x001000L
#define MONSTER		0x002000L
#define S_UNDEAD	0x004000L
#define SLOW		0x008000L
#define MANA_DRAIN	0x010000L
#define S_DEMON		0x020000L
#define S_DRAGON	0x040000L
#define BREATH_L	0x080000L
#define BREATH_G	0x100000L
#define BREATH_A	0x200000L
#define BREATH_FR	0x400000L
#define BREATH_FI	0x800000L
#define FIRE_BOLT   	0x1000000L
#define FROST_BOLT	0x2000000L
#define ACID_BOLT     	0x04000000L
#define MAG_MISS      	0x08000000L
#define CAUSE_CRIT    	0x10000000L
#define FIRE_BALL     	0x20000000L
#define FROST_BALL    	0x40000000L
#define MANA_BOLT     	0x80000000L

/* spells 2 :-> */
#define BREATH_CH   	0x00000001L /* Chaos */
#define BREATH_SH    	0x00000002L /* Shards */
#define BREATH_SD    	0x00000004L /* Sound */
#define BREATH_CO   	0x00000008L /* Confusion */
#define BREATH_DI 	0x00000010L /* Disenchantment */
#define BREATH_LD  	0x00000020L /* Life Draining */
#define LIGHT_BOLT 	0x00000040L
#define LIGHT_BALL   	0x00000080L
#define ACID_BALL    	0x00000100L
#define TRAP_CREATE  	0x00000200L
#define RAZOR        	0x00000400L
#define MIND_BLAST   	0x00000800L
#define TELE_AWAY    	0x00001000L
#define HEAL         	0x00002000L
#define HASTE        	0x00004000L
#define MISSILE      	0x00008000L
#define PLASMA_BOLT  	0x00010000L
#define SUMMON       	0x00020000L
#define NETHER_BOLT  	0x00040000L
#define ICE_BOLT     	0x00080000L
#define DARKNESS     	0x00100000L
#define FORGET       	0x00200000L
#define BRAIN_SMASH  	0x00400000L
#define ST_CLOUD     	0x00800000L
#define TELE_LEV     	0x01000000L
#define WATER_BOLT   	0x02000000L
#define WATER_BALL   	0x04000000L
#define NETHER_BALL  	0x08000000L
#define S_ANGEL      	0x10000000L
#define S_SPIDER     	0x20000000L
#define S_HOUND      	0x40000000L
#define BREATH_NE    	0x80000000L /* Nexus */

/* spells 3:-> */
#define BREATH_WA    	0x00000001L /* Wall Building */
#define BREATH_SL    	0x00000002L /* Slowness */
#define BREATH_LT    	0x00000004L /* Light */
#define BREATH_TI    	0x00000008L /* Time */
#define BREATH_GR    	0x00000010L /* Gravity */
#define BREATH_DA    	0x00000020L /* Darkness */
#define BREATH_PL    	0x00000040L /* Plasma */
#define ARROW        	0x00000080L /* fires an arrow */
#define S_WRAITH     	0x00000100L /* Summon ringwraiths */
#define DARK_STORM   	0x00000200L /* Big darkness breath */
#define MANA_STORM   	0x00000400L /* Mana storm */
#define S_REPTILE    	0x00000800L /* Summon reptiles */
#define S_ANT	     	0x00001000L /* Summon ants/ant lions */
#define S_UNIQUE     	0x00002000L /* Summon uniques */
#define S_GUNDEAD    	0x00004000L /* Summon greater undead */
#define S_ANCIENTD   	0x00008000L /* Summon ancient dragon */

#define ANIMAL		0x00000001L
#define EVIL		0x00000002L
#define ORC		0x00000004L
#define UNDEAD		0x00000008L
#define DRAGON		0x00000010L
#define DEMON		0x00000020L
#define HURT_LIGHT	0x00000040L
#define	HURT_ROCK	0x00000080L
#define CHARM_SLEEP	0x00000100L
#define NO_INFRA	0x00000200L
#define MAX_HP		0x00000400L
#define IM_FROST	0x00000800L
#define IM_FIRE		0x00001000L
#define IM_LIGHTNING	0x00002000L
#define IM_POISON	0x00004000L
#define IM_ACID		0x00008000L
#define TROLL		0x00010000L
#define GIANT		0x00020000L
#define SPECIAL         0x00040000L
#define GROUP           0x00080000L
#define GOOD            0x00100000L
#define BREAK_WALL      0x00200000L
#define DESTRUCT        0x00400000L
#define QUESTOR         0x00800000L /* killing it completes a quest */
#define INTELLIGENT     0x01000000L /* casts spells intelligently   */
#define MINDLESS        0x02000000L /* does not have a mind -CWS    */
#define UNIQUE          0x80000000L /* unique monster */


