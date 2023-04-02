/* File: save-old.c */

/* Purpose: support for loading pre-2.7.0 savefiles */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/*
 * This save package was brought to by		-JWT- and -RAK-
 * and has been completely rewritten for UNIX by	-JEW-  
 *
 * and has been completely rewritten again by	 -CJS-
 * and completely rewritten again! for portability by -JEW-
 *
 * This file is only used to parse pre-2.7.0 savefiles.
 * See the file "save.c" for more recent methods.
 */


/*
 * these are used for the save file, to avoid having to pass them to every
 * procedure 
 */

static FILE	*fff;		/* Current save "file" */

static byte	xor_byte;	/* Simple encryption */

static byte	version_maj;	/* Major version */
static byte	version_min;	/* Minor version */
static byte	patch_level;	/* Patch level */

static bool say;		/* Debugging */


/*
 * This function determines if the version of the savefile
 * currently being read is older than version "x.y.z".
 */
static bool older_than(int x, int y, int z)
{
    /* Much older, or much more recent */
    if (version_maj < x) return (TRUE);
    if (version_maj > x) return (FALSE);

    /* Distinctly older, or distinctly more recent */
    if (version_min < y) return (TRUE);
    if (version_min > y) return (FALSE);

    /* Barely older, or barely more recent */
    if (patch_level < z) return (TRUE);
    if (patch_level > z) return (FALSE);

    /* Identical versions */
    return (FALSE);
}


/*
 * Show information on the screen, one line at a time.
 * If "where" is negative, advance "-where" lines from last location.
 */
static void prt_note(int where, cptr msg)
{
    static int y = 0;

    /* Accept line number, Remember the line */
    y = (where < 0) ? (y - where) : where;

    /* Attempt to "wrap" if forced to */
    if (y >= 24) y = 0;

    /* Draw the message */
    prt(msg, y, 0);

    /* Flush it */
    put_qio();
}




/*
 * The basic Input function for old savefiles
 * All information is read one byte at a time
 * Note that this was stolen from "save.c"
 */

static byte sf_get(void)
{
    register byte c, v;

    /* Get a character, decode the value */
    c = getc(fff) & 0xFF;
    v = c ^ xor_byte;
    xor_byte = c;

    /* Return the value */    
    return (v);
}




/*
 * Write/Read various "byte sized" objects
 */

static void rd_byte(byte *ip)
{
    *ip = sf_get();
}


static void rd_char(char *ip)
{
    rd_byte((byte*)ip);
}


/*
 * Write/Read various "short" objects
 */

static void rd_u16b(u16b *ip)
{
    (*ip) = sf_get();
    (*ip) |= ((u16b)(sf_get()) << 8);
}


static void rd_s16b(s16b *ip)
{
    rd_u16b((u16b*)ip);
}



/*
 * Write/Read various "long" objects
 */

static void rd_u32b(u32b *ip)
{
    (*ip) = sf_get();
    (*ip) |= ((u32b)(sf_get()) << 8);
    (*ip) |= ((u32b)(sf_get()) << 16);
    (*ip) |= ((u32b)(sf_get()) << 24);
}


static void rd_s32b(s32b *ip)
{
    rd_u32b((u32b*)ip);
}




/*
 * Strings
 */

static void rd_string(char *str)
{
    while (1) {
	byte tmp;
	rd_byte(&tmp);
	*str = tmp;
	if (!*str) break;
	str++;
    }
}



/*
 * Mega-Hack -- convert the old "name2" fields into the new
 * name1/name2 fields.
 */

static int convert_name2[] = {

    0				/* 0 = SN_NULL */,
    2000+EGO_R			/* 1 = SN_R */,
    2000+EGO_RESIST_A		/* 2 = SN_RA */,
    2000+EGO_RESIST_F		/* 3 = SN_RF */,
    2000+EGO_RESIST_C		/* 4 = SN_RC */,
    2000+EGO_RESIST_E		/* 5 = SN_RL */,
    2000+EGO_HA			/* 6 = SN_HA */,
    2000+EGO_DF			/* 7 = SN_DF */,
    2000+EGO_SLAY_A		/* 8 = SN_SA */,
    2000+EGO_SLAY_D		/* 9 = SN_SD */,
    2000+EGO_SLAY_E		/* 10 = SN_SE */,
    2000+EGO_SLAY_U		/* 11 = SN_SU */,
    2000+EGO_FT			/* 12 = SN_FT */,
    2000+EGO_FB			/* 13 = SN_FB */,
    2000+EGO_FREE_ACTION	/* 14 = SN_FREE_ACTION */,
    2000+EGO_SLAYING		/* 15 = SN_SLAYING */,
    2000+EGO_CLUMSINESS		/* 16 = SN_CLUMSINESS */,
    2000+EGO_WEAKNESS		/* 17 = SN_WEAKNESS */,
    2000+EGO_SLOW_DESCENT	/* 18 = SN_SLOW_DESCENT */,
    2000+EGO_SPEED		/* 19 = SN_SPEED */,
    2000+EGO_STEALTH		/* 20 = SN_STEALTH */,
    2000+EGO_SLOWNESS		/* 21 = SN_SLOWNESS */,
    2000+EGO_NOISE		/* 22 = SN_NOISE */,
    2000+EGO_GREAT_MASS		/* 23 = SN_GREAT_MASS */,
    2000+EGO_INTELLIGENCE	/* 24 = SN_INTELLIGENCE */,
    2000+EGO_WISDOM		/* 25 = SN_WISDOM */,
    2000+EGO_INFRAVISION	/* 26 = SN_INFRAVISION */,
    2000+EGO_MIGHT		/* 27 = SN_MIGHT */,
    2000+EGO_LORDLINESS		/* 28 = SN_LORDLINESS */,
    2000+EGO_MAGI		/* 29 = SN_MAGI */,
    2000+EGO_BEAUTY		/* 30 = SN_BEAUTY */,
    2000+EGO_SEEING		/* 31 = SN_SEEING */,
    2000+EGO_REGENERATION	/* 32 = SN_REGENERATION */,
    2000+EGO_STUPIDITY		/* 33 = SN_STUPIDITY */,
    2000+EGO_DULLNESS		/* 34 = SN_DULLNESS */,
    2000+EGO_BLINDNESS				/* 35 = SN_BLINDNESS */,
    2000+EGO_TIMIDNESS				/* 36 = SN_TIMIDNESS */,
    2000+EGO_TELEPORTATION				/* 37 = SN_TELEPORTATION */,
    2000+EGO_UGLINESS		/* 38 = SN_UGLINESS */,
    2000+EGO_PROTECTION		/* 39 = SN_PROTECTION */,
    2000+EGO_IRRITATION		/* 40 = SN_IRRITATION */,
    2000+EGO_VULNERABILITY	/* 41 = SN_VULNERABILITY */,
    2000+EGO_ENVELOPING		/* 42 = SN_ENVELOPING */,
    2000+EGO_FIRE		/* 43 = SN_FIRE */,
    2000+EGO_SLAY_EVIL		/* 44 = SN_SLAY_EVIL */,
    2000+EGO_DRAGON_SLAYING	/* 45 = SN_DRAGON_SLAYING */,
    9000+EGO_EMPTY		/* 46 = SN_EMPTY */,
    9000+EGO_LOCKED		/* 47 = SN_LOCKED */,
    9000+EGO_POISON_NEEDLE	/* 48 = SN_POISON_NEEDLE */,
    9000+EGO_GAS_TRAP		/* 49 = SN_GAS_TRAP */,
    9000+EGO_EXPLOSION_DEVICE	/* 50 = SN_EXPLOSION_DEVICE */,
    9000+EGO_SUMMONING_RUNES	/* 51 = SN_SUMMONING_RUNES */,
    9000+EGO_MULTIPLE_TRAPS	/* 52 = SN_MULTIPLE_TRAPS */,
    9000+EGO_DISARMED		/* 53 = SN_DISARMED */,
    9000+EGO_UNLOCKED		/* 54 = SN_UNLOCKED */,
    2000+EGO_SLAY_ANIMAL	/* 55 = SN_SLAY_ANIMAL */,
    1000+ART_GROND		/* 56 = SN_GROND */,
    1000+ART_RINGIL		/* 57 = SN_RINGIL */,
    1000+ART_AEGLOS		/* 58 = SN_AEGLOS */,
    1000+ART_ARUNRUTH		/* 59 = SN_ARUNRUTH */,
    1000+ART_MORMEGIL		/* 60 = SN_MORMEGIL */,
    2000+EGO_MORGUL		/* 61 = SN_MORGUL */,
    1000+ART_ANGRIST		/* 62 = SN_ANGRIST */,
    1000+ART_GURTHANG		/* 63 = SN_GURTHANG */,
    1000+ART_CALRIS		/* 64 = SN_CALRIS */,
    2000+EGO_ACCURACY		/* 65 = SN_ACCURACY */,
    1000+ART_ANDURIL		/* 66 = SN_ANDURIL */,
    2000+EGO_SLAY_O		/* 67 = SN_SO */,
    2000+EGO_POWER		/* 68 = SN_POWER */,
    1000+ART_DURIN		/* 69 = SN_DURIN */,
    1000+ART_AULE		/* 70 = SN_AULE */,
    2000+EGO_WEST		/* 71 = SN_WEST */,
    2000+EGO_BLESS_BLADE	/* 72 = SN_BLESS_BLADE */,
    2000+EGO_SLAY_DEMON		/* 73 = SN_SDEM */,
    2000+EGO_SLAY_T		/* 74 = SN_ST */,
    1000+ART_BLOODSPIKE		/* 75 = SN_BLOODSPIKE */,
    1000+ART_THUNDERFIST	/* 76 = SN_THUNDERFIST */,
    2000+EGO_WOUNDING		/* 77 = SN_WOUNDING */,
    1000+ART_ORCRIST		/* 78 = SN_ORCRIST */,
    1000+ART_GLAMDRING		/* 79 = SN_GLAMDRING */,
    1000+ART_STING		/* 80 = SN_STING */,
    2000+EGO_LIGHT		/* 81 = SN_LITE */,
    2000+EGO_AGILITY		/* 82 = SN_AGILITY */,
    2000+EGO_BACKBITING		/* 83 = SN_BACKBITING */,
    1000+ART_DOOMCALLER		/* 84 = SN_DOOMCALLER */,
    2000+EGO_SLAY_G		/* 85 = SN_SG */,
    2000+EGO_TELEPATHY		/* 86 = SN_TELEPATHY */,
    2000+EGO_DRAGONKIND				/* 87 = SN_DRAGONKIND */,
    1000+ART_NENYA				/* 88 = SN_NENYA */,
    1000+ART_NARYA				/* 89 = SN_NARYA */,
    1000+ART_VILYA				/* 90 = SN_VILYA */,
    2000+EGO_AMAN		/* 91 = SN_AMAN */,
    1000+ART_BELEGENNON		/* 92 = SN_BELEGENNON */,
    1000+ART_FEANOR		/* 93 = SN_FEANOR */,
    1000+ART_ANARION		/* 94 = SN_ANARION */,
    1000+ART_ISILDUR		/* 95 = SN_ISILDUR */,
    1000+ART_FINGOLFIN		/* 96 = SN_FINGOLFIN */,
    2000+EGO_ELVENKIND		/* 97 = SN_ELVENKIND */,
    1000+ART_SOULKEEPER		/* 98 = SN_SOULKEEPER */,
    1000+ART_DOR_LOMIN		/* 99 = SN_DOR_LOMIN */,
    1000+ART_MORGOTH		/* 100 = SN_MORGOTH */,
    1000+EGO_BELEG	/* 101 = SN_BELTHRONDING */,
    1000+ART_DAL		/* 102 = SN_DAL */,
    1000+ART_PAURHACH		/* 103 = SN_PAURHACH */,
    1000+ART_PAURNIMMEN		/* 104 = SN_PAURNIMMEN */,
    1000+ART_PAURAEGEN		/* 105 = SN_PAURAEGEN */,
    1000+ART_CAMMITHRIM		/* 106 = SN_CAMMITHRIM */,
    1000+ART_CAMBELEG		/* 107 = SN_CAMBELEG */,
    1000+ART_HOLHENNETH		/* 108 = SN_HOLHENNETH */,
    1000+ART_PAURNEN		/* 109 = SN_PAURNEN */,
    1000+ART_AEGLIN		/* 110 = SN_AEGLIN */,
    1000+ART_CAMLOST		/* 111 = SN_CAMLOST */,
    1000+ART_NIMLOTH		/* 112 = SN_NIMLOTH */,
    1000+ART_NAR		/* 113 = SN_NAR */,
    1000+ART_BERUTHIEL		/* 114 = SN_BERUTHIEL */,
    1000+ART_GORLIM		/* 115 = SN_GORLIM */,
    1000+ART_NARTHANC		/* 116 = SN_NARTHANC */,
    1000+ART_NIMTHANC		/* 117 = SN_NIMTHANC */,
    1000+ART_DETHANC		/* 118 = SN_DETHANC */,
    1000+ART_GILETTAR		/* 119 = SN_GILETTAR */,
    1000+ART_RILIA		/* 120 = SN_RILIA */,
    1000+ART_BELANGIL		/* 121 = SN_BELANGIL */,
    1000+ART_BALLI		/* 122 = SN_BALLI */,
    1000+ART_LOTHARANG		/* 123 = SN_LOTHARANG */,
    1000+ART_FIRESTAR		/* 124 = SN_FIRESTAR */,
    1000+ART_ERIRIL		/* 125 = SN_ERIRIL */,
    1000+ART_CUBRAGOL		/* 126 = SN_CUBRAGOL */,
    1000+ART_BARD		/* 127 = SN_BARD */,
    1000+ART_COLLUIN		/* 128 = SN_COLLUIN */,
    1000+ART_HOLCOLLETH		/* 129 = SN_HOLCOLLETH */,
    1000+ART_TOTILA		/* 130 = SN_TOTILA */,
    1000+ART_PAIN		/* 131 = SN_PAIN */,
    1000+ART_ELVAGIL		/* 132 = SN_ELVAGIL */,
    1000+ART_AGLARANG		/* 133 = SN_AGLARANG */,
    1000+EGO_ROHAN		/* 134 = SN_ROHIRRIM */,
    1000+ART_EORLINGAS		/* 135 = SN_EORLINGAS */,
    1000+ART_BARUKKHELED	/* 136 = SN_BARUKKHELED */,
    1000+ART_WRATH		/* 137 = SN_WRATH */,
    1000+ART_HARADEKKET		/* 138 = SN_HARADEKKET */,
    1000+ART_MUNDWINE		/* 139 = SN_MUNDWINE */,
    1000+ART_GONDRICAM		/* 140 = SN_GONDRICAM */,
    1000+ART_ZARCUTHRA		/* 141 = SN_ZARCUTHRA */,
    1000+ART_CARETH		/* 142 = SN_CARETH */,
    1000+ART_FORASGIL		/* 143 = SN_FORASGIL */,
    1000+ART_CRISDURIAN		/* 144 = SN_CRISDURIAN */,
    1000+ART_COLANNON		/* 145 = SN_COLANNON */,
    1000+ART_HITHLOMIR		/* 146 = SN_HITHLOMIR */,
    1000+ART_THALKETTOTH	/* 147 = SN_THALKETTOTH */,
    1000+ART_ARVEDUI		/* 148 = SN_ARVEDUI */,
    1000+ART_THRANDUIL		/* 149 = SN_THRANDUIL */,
    1000+ART_THENGEL		/* 150 = SN_THENGEL */,
    1000+ART_HAMMERHAND		/* 151 = SN_HAMMERHAND */,
    1000+ART_CELEGORM		/* 152 = SN_CELEGORM */,
    1000+ART_THROR		/* 153 = SN_THROR */,
    1000+ART_MAEDHROS		/* 154 = SN_MAEDHROS */,
    1000+ART_OLORIN		/* 155 = SN_OLORIN */,
    1000+ART_ANGUIREL		/* 156 = SN_ANGUIREL */,
    1000+ART_THORIN		/* 157 = SN_THORIN */,
    1000+ART_CELEBORN		/* 158 = SN_CELEBORN */,
    1000+ART_OROME		/* 159 = SN_OROME */,
    1000+ART_EONWE		/* 160 = SN_EONWE */,
    1000+ART_GONDOR		/* 161 = SN_GONDOR */,
    1000+ART_THEODEN		/* 162 = SN_THEODEN */,
    1000+ART_THINGOL		/* 163 = SN_THINGOL */,
    1000+ART_THORONGIL		/* 164 = SN_THORONGIL */,
    1000+ART_LUTHIEN		/* 165 = SN_LUTHIEN */,
    1000+ART_TUOR		/* 166 = SN_TUOR */,
    1000+ART_ULMO		/* 167 = SN_ULMO */,
    1000+ART_OSONDIR		/* 168 = SN_OSONDIR */,
    1000+ART_TURMIL		/* 169 = SN_TURMIL */,
    1000+ART_CASPANION		/* 170 = SN_CASPANION */,
    1000+ART_TIL		/* 171 = SN_TIL */,
    1000+ART_DEATHWREAKER	/* 172 = SN_DEATHWREAKER */,
    1000+ART_AVAVIR		/* 173 = SN_AVAVIR */,
    1000+ART_TARATOL		/* 174 = SN_TARATOL */,
    1000+ART_RAZORBACK		/* 175 = SN_RAZORBACK */,
    1000+ART_BLADETURNER	/* 176 = SN_BLADETURNER */,
    2000+EGO_SHATTERED		/* 177 = SN_SHATTERED */,
    2000+EGO_BLASTED		/* 178 = SN_BLASTED */,
    2000+EGO_ATTACKS		/* 179 = SN_ATTACKS */,
};



/*
 * Convert the old savefile "flags" into the new ones
 * This just converts the flags that are the same, see
 * "rd_item_old()" for total conversion method.
 */
static void repair_item_flags_old(inven_type *i_ptr)
{
    int i = 0;
    u32b f1 = i_ptr->flags1;
    u32b f2 = i_ptr->flags2;


    /* Wipe the flags */
    i_ptr->flags1 = i_ptr->flags2 = i_ptr->flags3 = 0L;


    /*** Old Flag Set #1 ***/

    if (f1 & 0x00000001L) i_ptr->flags1 |= TR1_STR;
    if (f1 & 0x00000002L) i_ptr->flags1 |= TR1_INT;
    if (f1 & 0x00000004L) i_ptr->flags1 |= TR1_WIS;
    if (f1 & 0x00000008L) i_ptr->flags1 |= TR1_DEX;
    if (f1 & 0x00000010L) i_ptr->flags1 |= TR1_CON;
    if (f1 & 0x00000020L) i_ptr->flags1 |= TR1_CHR;
    if (f1 & 0x00000040L) i_ptr->flags1 |= TR1_SEARCH;
    if (f1 & 0x00000080L) i_ptr->flags3 |= TR3_SLOW_DIGEST;
    if (f1 & 0x00000100L) i_ptr->flags1 |= TR1_STEALTH;
    if (f1 & 0x00000200L) i_ptr->flags3 |= TR3_AGGRAVATE;
    if (f1 & 0x00000400L) i_ptr->flags3 |= TR3_TELEPORT;
    if (f1 & 0x00000800L) i_ptr->flags3 |= TR3_REGEN;
    if (f1 & 0x00001000L) i_ptr->flags1 |= TR1_SPEED;
    if (f1 & 0x00002000L) i_ptr->flags1 |= TR1_SLAY_DRAGON;
    if (f1 & 0x00004000L) i_ptr->flags1 |= TR1_SLAY_ANIMAL;
    if (f1 & 0x00008000L) i_ptr->flags1 |= TR1_SLAY_EVIL;
    if (f1 & 0x00010000L) i_ptr->flags1 |= TR1_SLAY_UNDEAD;
    if (f1 & 0x00020000L) i_ptr->flags1 |= TR1_BRAND_COLD;
    if (f1 & 0x00040000L) i_ptr->flags1 |= TR1_BRAND_FIRE;
    if (f1 & 0x00080000L) i_ptr->flags2 |= TR2_RES_FIRE;
    if (f1 & 0x00100000L) i_ptr->flags2 |= TR2_RES_ACID;
    if (f1 & 0x00200000L) i_ptr->flags2 |= TR2_RES_COLD;
    if (f1 & 0x00400000L) i = i; /* SUST_STAT extracted already */
    if (f1 & 0x00800000L) i_ptr->flags2 |= TR2_FREE_ACT;
    if (f1 & 0x01000000L) i_ptr->flags3 |= TR3_SEE_INVIS;
    if (f1 & 0x02000000L) i_ptr->flags2 |= TR2_RES_ELEC;
    if (f1 & 0x04000000L) i_ptr->flags3 |= TR3_FEATHER;
    if (f1 & 0x08000000L) i_ptr->flags1 |= TR1_KILL_DRAGON;
    if (f1 & 0x10000000L) i_ptr->flags2 |= TR2_RES_POIS;
    if (f1 & 0x20000000L) i_ptr->flags1 |= TR1_TUNNEL;
    if (f1 & 0x40000000L) i_ptr->flags1 |= TR1_INFRA;
    if (f1 & 0x80000000L) i_ptr->flags3 |= TR3_CURSED;


    /*** Old Flag Set #2 ***/

    if (f2 & 0x00000001L) i_ptr->flags1 |= TR1_SLAY_DEMON;
    if (f2 & 0x00000002L) i_ptr->flags1 |= TR1_SLAY_TROLL;
    if (f2 & 0x00000004L) i_ptr->flags1 |= TR1_SLAY_GIANT;
    if (f2 & 0x00000008L) i_ptr->flags2 |= TR2_HOLD_LIFE;
    if (f2 & 0x00000010L) i_ptr->flags1 |= TR1_SLAY_ORC;
    if (f2 & 0x00000020L) i_ptr->flags3 |= TR3_TELEPATHY;
    if (f2 & 0x00000040L) i_ptr->flags2 |= TR2_IM_FIRE;
    if (f2 & 0x00000080L) i_ptr->flags2 |= TR2_IM_COLD;
    if (f2 & 0x00000100L) i_ptr->flags2 |= TR2_IM_ACID;
    if (f2 & 0x00000200L) i_ptr->flags2 |= TR2_IM_ELEC;
    if (f2 & 0x00000400L) i_ptr->flags3 |= TR3_LITE;
    if (f2 & 0x00000800L) i_ptr->flags3 |= TR3_ACTIVATE;
    if (f2 & 0x00001000L) i_ptr->flags1 |= TR1_BRAND_ELEC;
    if (f2 & 0x00002000L) i_ptr->flags1 |= TR1_IMPACT;
    if (f2 & 0x00004000L) i_ptr->flags2 |= TR2_IM_POIS;
    if (f2 & 0x00008000L) i_ptr->flags2 |= TR2_RES_CONF;
    if (f2 & 0x00010000L) i_ptr->flags2 |= TR2_RES_SOUND;
    if (f2 & 0x00020000L) i_ptr->flags2 |= TR2_RES_LITE;
    if (f2 & 0x00040000L) i_ptr->flags2 |= TR2_RES_DARK;
    if (f2 & 0x00080000L) i_ptr->flags2 |= TR2_RES_CHAOS;
    if (f2 & 0x00100000L) i_ptr->flags2 |= TR2_RES_DISEN;
    if (f2 & 0x00200000L) i_ptr->flags2 |= TR2_RES_SHARDS;
    if (f2 & 0x00400000L) i_ptr->flags2 |= TR2_RES_NEXUS;
    if (f2 & 0x00800000L) i_ptr->flags2 |= TR2_RES_BLIND;
    if (f2 & 0x01000000L) i_ptr->flags2 |= TR2_RES_NETHER;
    if (f2 & 0x02000000L) i = i; /* Artifact Flag extracted into "name1" */
    if (f2 & 0x04000000L) i_ptr->flags3 |= TR3_BLESSED;
    if (f2 & 0x08000000L) i_ptr->flags1 |= TR1_ATTACK_SPD;
    if (f2 & 0x10000000L) i_ptr->flags2 |= TR2_RES_FEAR;
}


/*
 * Read an old-version "item" structure
 */
static errr rd_item_old(inven_type *i_ptr)
{
    byte tmp8u;
    u16b tmp16u;

    rd_u16b(&i_ptr->k_idx);
    rd_byte(&i_ptr->name2);
    rd_string(i_ptr->inscrip);
    rd_u32b(&i_ptr->flags1);
    rd_byte(&i_ptr->tval);
    rd_byte(&i_ptr->tchar);
    rd_s16b(&i_ptr->pval);
    rd_s32b(&i_ptr->cost);
    rd_byte(&i_ptr->sval);
    rd_byte(&i_ptr->number);

    rd_u16b(&tmp16u);
    i_ptr->weight = tmp16u;

    rd_s16b(&i_ptr->tohit);
    rd_s16b(&i_ptr->todam);
    rd_s16b(&i_ptr->ac);
    rd_s16b(&i_ptr->toac);
    rd_byte(&i_ptr->damage[0]);
    rd_byte(&i_ptr->damage[1]);

    /* Forget old "level" */
    rd_byte(&tmp8u);

    rd_byte(&i_ptr->ident);
    rd_u32b(&i_ptr->flags2);

    /* Read and forget the old timeout */    
    rd_u16b(&tmp16u);

    /* Clear the timeout */
    i_ptr->timeout = 0;

    /* Several objects now have new "locations" */
    switch (i_ptr->k_idx) {

	/* Items map to their "base" */
	case 0: i_ptr->k_idx = 15; break;
	case 13: i_ptr->k_idx = 12; break;
	case 14: i_ptr->k_idx = 12; break;
	case 22: i_ptr->k_idx = 21; break;
	case 23: i_ptr->k_idx = 21; break;
	case 26: i_ptr->k_idx = 25; break;
	case 27: i_ptr->k_idx = 25; break;

	/* Cleaning of Edged Blades */
	case 28: i_ptr->k_idx = 38; break;
	case 29: i_ptr->k_idx = 43; break;

	/* Items map to their "base" */
	case 35: i_ptr->k_idx = 34; break;
	case 38: i_ptr->k_idx = 37; break;
	case 43: i_ptr->k_idx = 42; break;

	/* Separate Hafted from Polearm */
	case 58: i_ptr->k_idx = 62; break;
	case 62: i_ptr->k_idx = 58; break;

	/* Spikes, Lantern, Torches */
	case 84: i_ptr->k_idx = 345; break;
	case 85: i_ptr->k_idx = 347; break;
	case 86: i_ptr->k_idx = 346; break;

	/* Dungeon Shovels and Picks */
	case 87: i_ptr->k_idx = 88; break;
	case 88: i_ptr->k_idx = 89; break;
	case 89: i_ptr->k_idx = 85; break;
	case 90: i_ptr->k_idx = 86; break;

	/* More items mapping to their "base" */        
	case 102: i_ptr->k_idx = 101; break;
	case 144: i_ptr->k_idx = 78; break;
	case 170: i_ptr->k_idx = 80; break;
	case 177: i_ptr->k_idx = 176; break;
	case 178: i_ptr->k_idx = 176; break;
	case 179: i_ptr->k_idx = 176; break;
	case 182: i_ptr->k_idx = 181; break;
	case 183: i_ptr->k_idx = 181; break;
	case 191: i_ptr->k_idx = 190; break;
	case 195: i_ptr->k_idx = 194; break;

	/* Random Rod */
	case 196: i_ptr->k_idx = 352; break;

	/* More items linking to "base" */
	case 198: i_ptr->k_idx = 197; break;
	case 199: i_ptr->k_idx = 197; break;
	case 205: i_ptr->k_idx = 214; break;
	case 238: i_ptr->k_idx = 237; break;
	case 239: i_ptr->k_idx = 237; break;
	case 256: i_ptr->k_idx = 249; break;
	case 284: i_ptr->k_idx = 283; break;

	/* Flask of oil */
	case 268: i_ptr->k_idx = 348; break;

	/* Random Staff */
	case 293: i_ptr->k_idx = 306; break;

	/* Random Staff */
	case 299: i_ptr->k_idx = 316; break;

	/* More items linking to "base" */
	case 306: i_ptr->k_idx = 305; break;
	case 316: i_ptr->k_idx = 307; break;
	case 321: i_ptr->k_idx = 322; break;

	/* Miscellaneous Junk */
	case 345: i_ptr->k_idx = 389; break;
	case 346: i_ptr->k_idx = 390; break;

	/* Skeletons */
	case 353: i_ptr->k_idx = 391; break;
	case 354: i_ptr->k_idx = 392; break;
	case 347: i_ptr->k_idx = 393; break;
	case 348: i_ptr->k_idx = 394; break;
	case 349: i_ptr->k_idx = 395; break;
	case 350: i_ptr->k_idx = 396; break;
	case 351: i_ptr->k_idx = 397; break;
	case 352: i_ptr->k_idx = 398; break;

	/* Empty bottle */
	case 355: i_ptr->k_idx = 349; break;

	/* Random Scroll */
	case 378: i_ptr->k_idx = 191; break;

	/* Dragon Scale Mail */
	case 389: i_ptr->k_idx = 401; break;
	case 390: i_ptr->k_idx = 402; break;
	case 391: i_ptr->k_idx = 400; break;
	case 392: i_ptr->k_idx = 404; break;
	case 393: i_ptr->k_idx = 403; break;
	case 394: i_ptr->k_idx = 405; break;

	/* Random Daggers */
	case 395: i_ptr->k_idx = 43; break;
	case 396: i_ptr->k_idx = 43; break;
	case 397: i_ptr->k_idx = 43; break;

	/* Random Short Sword */
	case 398: i_ptr->k_idx = 35; break;

	/* Random leftover potions */
	case 399: i_ptr->k_idx = 422; break;
	case 400: i_ptr->k_idx = 417; break;
	case 401: i_ptr->k_idx = 415; break;
	case 402: i_ptr->k_idx = 420; break;
	case 403: i_ptr->k_idx = 418; break;
	case 404: i_ptr->k_idx = 416; break;

	/* Random Rods */  
	case 405: i_ptr->k_idx = 355; break;
	case 406: i_ptr->k_idx = 353; break;

	/* Random Staff */
	case 407: i_ptr->k_idx = 321; break;

	/* Dragon Scale Mail */
	case 408: i_ptr->k_idx = 408; break;
	case 409: i_ptr->k_idx = 409; break;

	/* Random Rod */
	case 410: i_ptr->k_idx = 354; break;

	/* Random Cloak */
	case 411: i_ptr->k_idx = 123; break;

	/* Random Scrolls */
	case 412: i_ptr->k_idx = 198; break;
	case 413: i_ptr->k_idx = 199; break;

	/* Random Ring */
	case 414: i_ptr->k_idx = 144; break;

	/* Dragon Scale Mail */
	case 415: i_ptr->k_idx = 410; break;
	case 416: i_ptr->k_idx = 407; break;
	case 417: i_ptr->k_idx = 411; break;
	case 418: i_ptr->k_idx = 406; break;
	case 419: i_ptr->k_idx = 412; break;

	/* More random potions */
	case 420: i_ptr->k_idx = 256; break;
	case 421: i_ptr->k_idx = 421; break;
	case 422: i_ptr->k_idx = 419; break;

	/* Store bought food */
	case 423: i_ptr->k_idx = 21; break;
	case 424: i_ptr->k_idx = 22; break;
	case 425: i_ptr->k_idx = 23; break;
	case 426: i_ptr->k_idx = 26; break;
	case 427: i_ptr->k_idx = 27; break;

	/* Storebought Shovels/Picks */
	case 428: i_ptr->k_idx = 87; break;
	case 429: i_ptr->k_idx = 84; break;

	/* Many old shop items mapped to real items */
	case 430: i_ptr->k_idx = 176; break;
	case 431: i_ptr->k_idx = 181; break;
	case 432: i_ptr->k_idx = 185; break;
	case 433: i_ptr->k_idx = 189; break;
	case 434: i_ptr->k_idx = 192; break;
	case 435: i_ptr->k_idx = 193; break;
	case 436: i_ptr->k_idx = 201; break;
	case 437: i_ptr->k_idx = 217; break;
	case 438: i_ptr->k_idx = 220; break;
	case 439: i_ptr->k_idx = 237; break;
	case 440: i_ptr->k_idx = 257; break;
	case 441: i_ptr->k_idx = 259; break;
	case 442: i_ptr->k_idx = 264; break;

	/* Old shop items -- lantern, torch, oil */
	case 443: i_ptr->k_idx = 347; break;
	case 444: i_ptr->k_idx = 346; break;
	case 445: i_ptr->k_idx = 348; break;

	/* Clean up the traps, the mush, and the rubble */
	case 459: i_ptr->k_idx = 460; break;
	case 460: i_ptr->k_idx = 477; break;
	case 477: i_ptr->k_idx = 445; break;
	case 478: i_ptr->k_idx = 21; break;
	case 479: i_ptr->k_idx = 459; break;

	/* The old "nothing" object */
	case 498: i_ptr->k_idx = 0; break;

	/* Ruined Chest */
	case 499: i_ptr->k_idx = 344; break;

	/* Special objects */
	case 501: i_ptr->k_idx = 508; break;
	case 502: i_ptr->k_idx = 509; break;
	case 503: i_ptr->k_idx = 510; break;
	case 504: i_ptr->k_idx = 511; break;
	case 505: i_ptr->k_idx = 500; break;
	case 506: i_ptr->k_idx = 504; break;
	case 507: i_ptr->k_idx = 503; break;
	case 508: i_ptr->k_idx = 501; break;
	case 509: i_ptr->k_idx = 502; break;
	case 510: i_ptr->k_idx = 507; break;
	case 511: i_ptr->k_idx = 505; break;
	case 512: i_ptr->k_idx = 506; break;
    }

    /* Artifact Names Dominate Ego-Item Names */
    if (i_ptr->name1) i_ptr->name2 = 0;


    /* The Old Special Names induce Artifact Names */
    if (i_ptr->name2) {

	int hack;

	/* Analyze the old "special name" */
	hack = convert_name2[i_ptr->name2];

	/* "Chest" names */
	if (hack > 9000) {
	}

	/* It is an ego-item */        
	else if (hack > 2000) {
	}

	/* It is an artifact */        
	else if (hack > 1000) {

	    /* Move it into the artifact table */
	    i_ptr->name1 = (hack - 1000);

	    /* Forget the old name */
	    i_ptr->name2 = 0;
	}

	/* Oops.  That name no longer exists... */
	else {
	    message("Ignoring illegal 'name2' field", 0);
	    i_ptr->name2 = 0;
	}
    }


    /* Access the (possibly new) item template */
    k_ptr = &k_list[i_ptr->k_idx];

    /* Take note if the object was an invisible trap */
    if (i_ptr->tval == TV_INVIS_TRAP) invis_trap = TRUE;

    /* Hack -- repair "tval" */
    i_ptr->tval = k_ptr->tval;

    /* Hack -- repair "sval" */
    i_ptr->sval = k_ptr->sval;

    /* XXX Hack -- un-repair invisible traps */
    if (invis_trap) i_ptr->tval = TV_INVIS_TRAP;



    /* Repair the "flags" in wearable objects */
    if (wearable_p(i_ptr)) {


	/* Save the "Sustain Stat flags" */
	u32b sustain2 = 0L;


	/* Extract old "TR_SUST_STAT" flag, save it for later */
	if (i_ptr->flags1 & 0x00400000L) {

	    /* Hack -- multi-sustain2 */
	    if (i_ptr->pval == 10) {

		/* Sustain everything */
		sustain2 |= (TR2_SUST_STR | TR2_SUST_DEX | TR2_SUST_CON |
			     TR2_SUST_INT | TR2_SUST_WIS | TR2_SUST_CHR);

		/* Forget the bogus pval */
		i_ptr->pval = 0;
	    }

	    /* Give a normal sustain2, keep the pval */
	    switch (i_ptr->pval) {
		case 1: sustain2 |= (TR2_SUST_STR); break;
		case 2: sustain2 |= (TR2_SUST_INT); break;
		case 3: sustain2 |= (TR2_SUST_WIS); break;
		case 4: sustain2 |= (TR2_SUST_DEX); break;
		case 5: sustain2 |= (TR2_SUST_CON); break;
		case 6: sustain2 |= (TR2_SUST_CHR); break;
	    }

	    /* Hack -- If the "pval" was "hidden", forget it */
	    if (i_ptr->ident & 0x40) i_ptr->pval = 0;
	}

	/* Completely repair old flags */
	repair_item_flags_old(i_ptr);

	/* Drop in the new "Sustain Stat" flags */
	i_ptr->flags2 |= sustain2;
    }


    /* Success */
    return (0);
}


/*
 * Read and Write monsters
 */

static void rd_monster_old(monster_type *m_ptr)
{
    byte	tmp8u;
    u16b	tmp16u;
    
    /* Read the current hitpoints */
    rd_s16b(&m_ptr->hp);

    if (older_than(2,6,0)) {
	/* Hack -- see below as well */
	m_ptr->maxhp = m_ptr->hp;
    }
    else {
	/* Read the maximal hitpoints */
	rd_s16b(&m_ptr->maxhp);
    }

    rd_s16b(&m_ptr->csleep);

    rd_u16b(&tmp16u);	/* Old speed */

    rd_u16b(&m_ptr->r_idx);

    rd_byte(&m_ptr->fy);
    rd_byte(&m_ptr->fx);

    rd_byte(&tmp8u); /* ignore saved "m_ptr->cdis" */
    rd_byte(&tmp8u); /* ignore saved "m_ptr->ml" */

    rd_byte(&m_ptr->stunned);
    rd_byte(&m_ptr->confused);

    if (older_than(2,6,0)) {
	/* Clear the monster fear value */
	m_ptr->monfear = 0;
    }
    else {
	/* Read the monster fear */
	rd_byte(&m_ptr->monfear);
    }
}



/*
 * Read the old lore
 */
static void rd_lore_old(monster_lore *l_ptr)
{
	int i;

    rd_u32b(&l_ptr->r_cflags1);
    rd_u32b(&l_ptr->r_spells1);
    rd_u32b(&l_ptr->r_spells2);
    rd_u32b(&l_ptr->r_spells3);
    rd_u16b(&l_ptr->r_kills);
    rd_u16b(&l_ptr->r_deaths);
    rd_u32b(&l_ptr->r_cflags2);
    rd_byte(&l_ptr->r_wake);
    rd_byte(&l_ptr->r_ignore);
    rd_byte(&l_ptr->r_attacks[0]);
    rd_byte(&l_ptr->r_attacks[1]);
    rd_byte(&l_ptr->r_attacks[2]);
    rd_byte(&l_ptr->r_attacks[3]);

    /* XXX Remember to "extract" max_num later */
}




static errr rd_store_old(store_type *st_ptr)
{
    int j;

    rd_s32b(&st_ptr->store_open);
    rd_s16b(&st_ptr->insult_cur);
    rd_byte(&st_ptr->owner);
    rd_byte(&st_ptr->store_ctr);
    rd_u16b(&st_ptr->good_buy);
    rd_u16b(&st_ptr->bad_buy);

    /* Too many items */    
    if (st_ptr->store_ctr > STORE_INVEN_MAX) {
	prt_note(-2, "Too many items in store");
	return (10);
    }

    /* Read the items (and costs) */
    for (j = 0; j < st_ptr->store_ctr; j++) {
	s32b scost;
	rd_s32b(&scost);
	rd_item_old(&st_ptr->store_item[j]);
	st_ptr->store_item[j].scost = scost;
    }

    /* Success */
    return (0);
}



/*
 * Read the artifacts -- old version
 */

static void rd_artifacts_old()
{
    u32b tmp32u;

    rd_u32b(&tmp32u); GROND = tmp32u;
    rd_u32b(&tmp32u); RINGIL = tmp32u;
    rd_u32b(&tmp32u); AEGLOS = tmp32u;
    rd_u32b(&tmp32u); ARUNRUTH = tmp32u;
    rd_u32b(&tmp32u); MORMEGIL = tmp32u;
    rd_u32b(&tmp32u); ANGRIST = tmp32u;
    rd_u32b(&tmp32u); GURTHANG = tmp32u;
    rd_u32b(&tmp32u); CALRIS = tmp32u;
    rd_u32b(&tmp32u); ANDURIL = tmp32u;
    rd_u32b(&tmp32u); STING = tmp32u;
    rd_u32b(&tmp32u); ORCRIST = tmp32u;
    rd_u32b(&tmp32u); GLAMDRING = tmp32u;
    rd_u32b(&tmp32u); DURIN = tmp32u;
    rd_u32b(&tmp32u); AULE = tmp32u;
    rd_u32b(&tmp32u); THUNDERFIST = tmp32u;
    rd_u32b(&tmp32u); BLOODSPIKE = tmp32u;
    rd_u32b(&tmp32u); DOOMCALLER = tmp32u;
    rd_u32b(&tmp32u); NARTHANC = tmp32u;
    rd_u32b(&tmp32u); NIMTHANC = tmp32u;
    rd_u32b(&tmp32u); DETHANC = tmp32u;
    rd_u32b(&tmp32u); GILETTAR = tmp32u;
    rd_u32b(&tmp32u); RILIA = tmp32u;
    rd_u32b(&tmp32u); BELANGIL = tmp32u;
    rd_u32b(&tmp32u); BALLI = tmp32u;
    rd_u32b(&tmp32u); LOTHARANG = tmp32u;
    rd_u32b(&tmp32u); FIRESTAR = tmp32u;
    rd_u32b(&tmp32u); ERIRIL = tmp32u;
    rd_u32b(&tmp32u); CUBRAGOL = tmp32u;
    rd_u32b(&tmp32u); BARD = tmp32u;
    rd_u32b(&tmp32u); COLLUIN = tmp32u;
    rd_u32b(&tmp32u); HOLCOLLETH = tmp32u;
    rd_u32b(&tmp32u); TOTILA = tmp32u;
    rd_u32b(&tmp32u); PAIN = tmp32u;
    rd_u32b(&tmp32u); ELVAGIL = tmp32u;
    rd_u32b(&tmp32u); AGLARANG = tmp32u;
    rd_u32b(&tmp32u); EORLINGAS = tmp32u;
    rd_u32b(&tmp32u); BARUKKHELED = tmp32u;
    rd_u32b(&tmp32u); WRATH = tmp32u;
    rd_u32b(&tmp32u); HARADEKKET = tmp32u;
    rd_u32b(&tmp32u); MUNDWINE = tmp32u;
    rd_u32b(&tmp32u); GONDRICAM = tmp32u;
    rd_u32b(&tmp32u); ZARCUTHRA = tmp32u;
    rd_u32b(&tmp32u); CARETH = tmp32u;
    rd_u32b(&tmp32u); FORASGIL = tmp32u;
    rd_u32b(&tmp32u); CRISDURIAN = tmp32u;
    rd_u32b(&tmp32u); COLANNON = tmp32u;
    rd_u32b(&tmp32u); HITHLOMIR = tmp32u;
    rd_u32b(&tmp32u); THALKETTOTH = tmp32u;
    rd_u32b(&tmp32u); ARVEDUI = tmp32u;
    rd_u32b(&tmp32u); THRANDUIL = tmp32u;
    rd_u32b(&tmp32u); THENGEL = tmp32u;
    rd_u32b(&tmp32u); HAMMERHAND = tmp32u;
    rd_u32b(&tmp32u); CELEGORM = tmp32u;
    rd_u32b(&tmp32u); THROR = tmp32u;
    rd_u32b(&tmp32u); MAEDHROS = tmp32u;
    rd_u32b(&tmp32u); OLORIN = tmp32u;
    rd_u32b(&tmp32u); ANGUIREL = tmp32u;
    rd_u32b(&tmp32u); OROME = tmp32u;
    rd_u32b(&tmp32u); EONWE = tmp32u;
    rd_u32b(&tmp32u); THEODEN = tmp32u;
    rd_u32b(&tmp32u); ULMO = tmp32u;
    rd_u32b(&tmp32u); OSONDIR = tmp32u;
    rd_u32b(&tmp32u); TURMIL = tmp32u;
    rd_u32b(&tmp32u); CASPANION = tmp32u;
    rd_u32b(&tmp32u); TIL = tmp32u;
    rd_u32b(&tmp32u); DEATHWREAKER = tmp32u;
    rd_u32b(&tmp32u); AVAVIR = tmp32u;
    rd_u32b(&tmp32u); TARATOL = tmp32u;
    rd_u32b(&tmp32u); DOR_LOMIN = tmp32u;
    rd_u32b(&tmp32u); NENYA = tmp32u;
    rd_u32b(&tmp32u); NARYA = tmp32u;
    rd_u32b(&tmp32u); VILYA = tmp32u;
    rd_u32b(&tmp32u); BELEGENNON = tmp32u;
    rd_u32b(&tmp32u); FEANOR = tmp32u;
    rd_u32b(&tmp32u); ISILDUR = tmp32u;
    rd_u32b(&tmp32u); SOULKEEPER = tmp32u;
    rd_u32b(&tmp32u); FINGOLFIN = tmp32u;
    rd_u32b(&tmp32u); ANARION = tmp32u;
    rd_u32b(&tmp32u); POWER = tmp32u;
    rd_u32b(&tmp32u); PHIAL = tmp32u;
    rd_u32b(&tmp32u); BELEG = tmp32u;
    rd_u32b(&tmp32u); DAL = tmp32u;
    rd_u32b(&tmp32u); PAURHACH = tmp32u;
    rd_u32b(&tmp32u); PAURNIMMEN = tmp32u;
    rd_u32b(&tmp32u); PAURAEGEN = tmp32u;
    rd_u32b(&tmp32u); PAURNEN = tmp32u;
    rd_u32b(&tmp32u); CAMMITHRIM = tmp32u;
    rd_u32b(&tmp32u); CAMBELEG = tmp32u;
    rd_u32b(&tmp32u); INGWE = tmp32u;
    rd_u32b(&tmp32u); CARLAMMAS = tmp32u;
    rd_u32b(&tmp32u); HOLHENNETH = tmp32u;
    rd_u32b(&tmp32u); AEGLIN = tmp32u;
    rd_u32b(&tmp32u); CAMLOST = tmp32u;
    rd_u32b(&tmp32u); NIMLOTH = tmp32u;
    rd_u32b(&tmp32u); NAR = tmp32u;
    rd_u32b(&tmp32u); BERUTHIEL = tmp32u;
    rd_u32b(&tmp32u); GORLIM = tmp32u;
    rd_u32b(&tmp32u); ELENDIL = tmp32u;
    rd_u32b(&tmp32u); THORIN = tmp32u;
    rd_u32b(&tmp32u); CELEBORN = tmp32u;
    rd_u32b(&tmp32u); THRAIN = tmp32u;
    rd_u32b(&tmp32u); GONDOR = tmp32u;
    rd_u32b(&tmp32u); THINGOL = tmp32u;
    rd_u32b(&tmp32u); THORONGIL = tmp32u;
    rd_u32b(&tmp32u); LUTHIEN = tmp32u;
    rd_u32b(&tmp32u); TUOR = tmp32u;
    rd_u32b(&tmp32u); ROHAN = tmp32u;
    rd_u32b(&tmp32u); TULKAS = tmp32u;
    rd_u32b(&tmp32u); NECKLACE = tmp32u;
    rd_u32b(&tmp32u); BARAHIR = tmp32u;
    rd_u32b(&tmp32u); RAZORBACK = tmp32u;
    rd_u32b(&tmp32u); BLADETURNER = tmp32u;
}





/*
 * Read/Write the "ghost" information
 *
 * Note -- old savefiles do this VERY badly...
 */

static void rd_ghost_old()
{
    monster_race *r_ptr = &r_list[MAX_R_IDX-1];

    int i;
    byte tmp8u;
    u16b tmp16u;
    u32b tmp32u;


    /* A buffer for the ghost name */
    char gname[128];

    /* Hack -- read the name as bytes */
    for (i = 0; i < 100; i++) rd_char(&gname[i]);
    strcpy(ghost_name, gname);

    /* Restore ghost names & stats etc... */

    rd_u32b(&tmp32u);
    r_ptr->cflags1 = tmp32u;

    rd_u32b(&tmp32u);
    r_ptr->spells1 = tmp32u;

    rd_u32b(&tmp32u);
    r_ptr->cflags2 = tmp32u;


/*
 * fix player ghost's exp bug.  The mexp field is really an u32b, but the
 * savefile was writing/ reading an u16b.  Since I don't want to change
 * the savefile format, this insures that the mexp field is loaded, and that
 * the "high bits" of mexp do not contain garbage values which could mean that
 * player ghost are worth millions of exp. -CFT
 */

    rd_u16b(&tmp16u);
    r_ptr->mexp = (u32b)(tmp16u);

/*
 * more stupid size bugs that would've never been needed if these variables
 * had been given enough space in the first place -CWS
 */

    if (older_than(2,6,0)) {
	rd_byte(&tmp8u);
	r_ptr->sleep = tmp8u;
    }
    else {
	rd_u16b(&tmp16u);
	r_ptr->sleep = tmp16u;
    }

    rd_byte(&tmp8u);
    r_ptr->aaf = tmp8u;

    if (older_than(2,6,0)) {
	rd_byte(&tmp8u);
	r_ptr->ac = tmp8u;
    }
    else {
	rd_u16b(&tmp16u);
	r_ptr->ac = tmp16u;
    }

    /* Read the speed */
    rd_byte(&tmp8u);
    r_ptr->speed = tmp8u;

    rd_byte(&tmp8u);
    r_ptr->r_char = tmp8u;

    rd_byte(&r_ptr->hd[0]);
    rd_byte(&r_ptr->hd[1]);

    /* Hack -- read the attacks */
    for (i = 0; i < 4; i++) {
	rd_u16b(&r_ptr->damage[i]);
    }

    rd_u16b(&tmp16u);
    r_ptr->level = tmp16u;
}



/*
 * Read the OLD extra information
 */
static void rd_extra_old()
{
    int i;
    byte tmp8u;

    rd_string(p_ptr->name);
    rd_byte(&p_ptr->male);
    rd_s32b(&p_ptr->au);
    rd_s32b(&p_ptr->max_exp);
    rd_s32b(&p_ptr->exp);
    rd_u16b(&p_ptr->exp_frac);
    rd_u16b(&p_ptr->age);
    rd_u16b(&p_ptr->ht);
    rd_u16b(&p_ptr->wt);
    rd_u16b(&p_ptr->lev);
    rd_u16b(&p_ptr->max_dlv);
    rd_s16b(&p_ptr->srh);
    rd_s16b(&p_ptr->fos);
    rd_s16b(&p_ptr->bth);
    rd_s16b(&p_ptr->bthb);
    rd_s16b(&p_ptr->mana);
    rd_s16b(&p_ptr->mhp);
    rd_s16b(&p_ptr->ptohit);
    rd_s16b(&p_ptr->ptodam);
    rd_s16b(&p_ptr->pac);
    rd_s16b(&p_ptr->ptoac);
    rd_s16b(&p_ptr->dis_th);
    rd_s16b(&p_ptr->dis_td);
    rd_s16b(&p_ptr->dis_ac);
    rd_s16b(&p_ptr->dis_tac);
    rd_s16b(&p_ptr->disarm);
    rd_s16b(&p_ptr->save);
    rd_s16b(&p_ptr->sc);
    rd_s16b(&p_ptr->stl);
    rd_byte(&p_ptr->pclass);
    rd_byte(&p_ptr->prace);
    rd_byte(&p_ptr->hitdie);
    rd_byte(&p_ptr->expfact);
    rd_s16b(&p_ptr->cmana);
    rd_u16b(&p_ptr->cmana_frac);
    rd_s16b(&p_ptr->chp);
    rd_u16b(&p_ptr->chp_frac);

    for (i = 0; i < 4; i++) {
	rd_string(p_ptr->history[i]);
    }

    /* Read the stats */    
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->max_stat[i]);
    if (older_than(2,5,7)) {
	for (i = 0; i < 6; i++) rd_s16b(&p_ptr->cur_stat[i]);
    }
    else {
	for (i = 0; i < 6; i++) {
	    rd_byte(&tmp8u);
	    p_ptr->cur_stat[i] = tmp8u;
	}
    }
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->mod_stat[i]);
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->use_stat[i]);

    /* Read the flags */
    rd_u32b(&p_ptr->status);
    rd_s16b(&p_ptr->rest);
    rd_s16b(&p_ptr->blind);
    rd_s16b(&p_ptr->paralysis);
    rd_s16b(&p_ptr->confused);
    rd_s16b(&p_ptr->food);
    rd_s16b(&p_ptr->food_digested);
    rd_s16b(&p_ptr->protection);
    rd_s16b(&p_ptr->speed);
    rd_s16b(&p_ptr->fast);
    rd_s16b(&p_ptr->slow);
    rd_s16b(&p_ptr->afraid);
    rd_s16b(&p_ptr->cut);
    rd_s16b(&p_ptr->stun);
    rd_s16b(&p_ptr->poisoned);
    rd_s16b(&p_ptr->image);
    rd_s16b(&p_ptr->protevil);
    rd_s16b(&p_ptr->invuln);
    rd_s16b(&p_ptr->hero);
    rd_s16b(&p_ptr->shero);
    rd_s16b(&p_ptr->shield);
    rd_s16b(&p_ptr->blessed);
    rd_s16b(&p_ptr->oppose_fire);
    rd_s16b(&p_ptr->oppose_cold);
    rd_s16b(&p_ptr->oppose_acid);
    rd_s16b(&p_ptr->oppose_elec);
    rd_s16b(&p_ptr->oppose_pois);
    rd_s16b(&p_ptr->detect_inv);
    rd_s16b(&p_ptr->word_recall);
    rd_s16b(&p_ptr->see_infra);
    rd_s16b(&p_ptr->tim_infra);
    rd_byte(&p_ptr->see_inv);
    rd_byte(&p_ptr->teleport);
    rd_byte(&p_ptr->free_act);
    rd_byte(&p_ptr->slow_digest);
    rd_byte(&p_ptr->aggravate);
    rd_byte(&p_ptr->resist_fire);
    rd_byte(&p_ptr->resist_cold);
    rd_byte(&p_ptr->resist_acid);
    rd_byte(&p_ptr->regenerate);
    rd_byte(&p_ptr->resist_elec);
    rd_byte(&p_ptr->ffall);
    rd_byte(&p_ptr->sustain_str);
    rd_byte(&p_ptr->sustain_int);
    rd_byte(&p_ptr->sustain_wis);
    rd_byte(&p_ptr->sustain_con);
    rd_byte(&p_ptr->sustain_dex);
    rd_byte(&p_ptr->sustain_chr);
    rd_byte(&p_ptr->confusing);
    rd_byte(&p_ptr->new_spells);
    rd_byte(&p_ptr->resist_pois);
    rd_byte(&p_ptr->hold_life);
    rd_byte(&p_ptr->telepathy);
    rd_byte(&p_ptr->immune_fire);
    rd_byte(&p_ptr->immune_acid);
    rd_byte(&p_ptr->immune_pois);
    rd_byte(&p_ptr->immune_cold);
    rd_byte(&p_ptr->immune_elec);
    rd_byte(&p_ptr->lite);
    rd_byte(&p_ptr->resist_conf);
    rd_byte(&p_ptr->resist_sound);
    rd_byte(&p_ptr->resist_lite);
    rd_byte(&p_ptr->resist_dark);
    rd_byte(&p_ptr->resist_chaos);
    rd_byte(&p_ptr->resist_disen);
    rd_byte(&p_ptr->resist_shards);
    rd_byte(&p_ptr->resist_nexus);
    rd_byte(&p_ptr->resist_blind);
    rd_byte(&p_ptr->resist_nether);

    if (older_than(2,6,0)) {
	p_ptr->resist_fear = 0;	/* sigh */
    }
    else {
	rd_byte(&p_ptr->resist_fear);
    }

    rd_u16b(&missile_ctr);

    /* Current turn */
    rd_u32b(&turn);

    /* Last turn */
    if (older_than(2,6,0)) {
	old_turn = turn;
    }
    else {
	rd_u32b(&old_turn);
    }
}





/*
 * Old method
 */
static errr rd_dungeon_old()
{
    int i;
    byte count;
    byte ychar, xchar;
    byte tmp8u;
    u16b tmp16u;
    int total_count;
    cave_type *c_ptr;


    /* Header info */            
    rd_u16b(&dun_level);
    rd_u16b(&char_row);
    rd_u16b(&char_col);
    rd_u16b(&mon_tot_mult);
    rd_u16b(&cur_height);
    rd_u16b(&cur_width);
    rd_u16b(&max_panel_rows);
    rd_u16b(&max_panel_cols);


    /* read in the creature ptr info */
    while (1) {

	rd_byte(&tmp8u);
	if (tmp8u == 0xFF) break;

	ychar = tmp8u;
	rd_byte(&xchar);

	/* Invalid cave location */
	if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) return (71);

	/* let's correctly fix the invisible monster bug  -CWS */
	if (older_than(2,6,0)) {
	    rd_byte(&tmp8u);
	    cave[ychar][xchar].m_idx = tmp8u;
	}
	else {
	    rd_u16b(&tmp16u);
	    cave[ychar][xchar].m_idx = tmp16u;
	}
    }

    /* read in the treasure ptr info */
    while (1) { 
	rd_byte(&tmp8u);
	if (tmp8u == 0xFF) break;
	ychar = tmp8u;
	rd_byte(&xchar);
	rd_u16b(&tmp16u);
	if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) return (72);
	cave[ychar][xchar].i_idx = tmp16u;
    }


    /* Read in the actual "cave" data */
	c_ptr = &cave[0][0];
    total_count = 0;

    /* Read until done */
    while (total_count != MAX_HEIGHT * MAX_WIDTH) {

	/* Extract some RLE info */
	rd_byte(&count);
	rd_byte(&tmp8u);

	/* Apply the RLE info */
	for (i = count; i > 0; i--) {

#ifndef ATARIST_MWC
	    /* Prevent over-run */
	    if (c_ptr >= &cave[MAX_HEIGHT][0]) {
		prt_note(-2, "Dungeon too big!");
		return (81);
	    }
#endif

	    /* Extract the "wall data" */
	    c_ptr->fval = tmp8u & 0xF;

	    /* Extract the "info" */
	    c_ptr->lr = (tmp8u >> 4) & 0x1;
	    c_ptr->fm = (tmp8u >> 5) & 0x1;
	    c_ptr->pl = (tmp8u >> 6) & 0x1;
	    c_ptr->tl = (tmp8u >> 7) & 0x1;

	    /* Advance the cave pointers */
	    c_ptr++;
	}

	total_count += count;
    }


    /* Read the item count */
    rd_u16b(&i_max);
    if (i_max > MAX_I_IDX) {
	prt_note(-2, "Too many objects");
	return (92);
    }

    /* Read the dungeon items */
    for (i = MIN_I_IDX; i < i_max; i++) {
	inven_type *i_ptr = &i_list[i];

	/* Read the item */
	rd_item_old(i_ptr);
    }


    /* Read the monster count */        
    rd_u16b(&m_max);
    if (m_max > MAX_M_IDX) {
	prt_note(-2, "Too many monsters");
	return (93);
    }

    /* Read the monsters */
    for (i = MIN_M_IDX; i < m_max; i++) {
	monster_type *m_ptr = &m_list[i];
	rd_monster_old(m_ptr);
    }

#ifdef MSDOS
    /* change walls and floors to graphic symbols */
	t_ptr = &i_list[i_max - 1];
	for (i = i_max - 1; i >= MIN_I_IDX; i--) {
	    if (t_ptr->tchar == '#')
		t_ptr->tchar = wallsym;
	    t_ptr--;
	}
#endif

    /* Read the ghost info */
    rd_ghost_old();


    /* Success */
    return (0);
}







/*
 * Read options
 */

static void rd_options_old()
{
    u32b l;

    /* Standard options */
    rd_u32b(&l);

	if (!older_than(2,6,0)) {
	  rd_u32b(&l);
	  rd_u32b(&l);
	  rd_u32b(&l);
	}

    /* Hack -- Extract death */
    death = (l & 0x80000000) ? TRUE : FALSE;

    rogue_like_commands =  (l & 32) ? TRUE : FALSE;
    prompt_carry_flag =    (l & 16) ? TRUE : FALSE;
    carry_query_flag =     (l & 0x400L) ? TRUE : FALSE;
    quick_messages =       (l & 0x40000000L) ? TRUE : FALSE;

    notice_seams =         (l & 128) ? TRUE : FALSE;
    equippy_chars = (l & 0x20000000L) ? TRUE : FALSE;

    find_cut =             (l & 1) ? TRUE : FALSE;
    find_examine =         (l & 2) ? TRUE : FALSE;
    find_prself =          (l & 4) ? TRUE : FALSE;
    find_bound =           (l & 8) ? TRUE : FALSE;
    find_ignore_doors =    (l & 0x100L) ? TRUE : FALSE;

    no_haggle_flag =       (l & 0x200L) ? TRUE : FALSE;

    show_inven_weight =    (l & 64) ? TRUE : FALSE;
    show_equip_weight =		(l & 0x00800000L) ? TRUE : FALSE;
    plain_descriptions =	(l & 0x00400000L) ? TRUE : FALSE;

    unfelt = (l & 0x1000L) ? TRUE : FALSE;

    /* Read "delay_spd" */
    delay_spd = ((l >> 13) & 0xf);
    if (delay_spd > 10) delay_spd = 10;
    if (delay_spd < 0) delay_spd = 0;

    /* Read "hitpoint_warn" */
    hitpoint_warn = ((l >> 17) & 0xf);
}


static errr rd_inventory_old()
{
    int i;

    /* Reset the counters */
    inven_ctr = 0;
    equip_ctr = 0;
    inven_weight = 0;

    /* Count the items */
    rd_u16b(&inven_ctr);

    /* Verify */
    if (inven_ctr > INVEN_WIELD) {
	prt_note(-2, "Unable to read inventory");
	return (15);
    }

    /* Normal pack items */
    for (i = 0; i < inven_ctr; i++) {

	/* Read the item */
	rd_item_old(&inventory[i]);
    }

    /* Old "normal" equipment */
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {

	/* Read the item */
	rd_item_old(&inventory[i]);
    }

	rd_u16b(&inven_weight);

	rd_u16b(&equip_ctr);
}


static errr rd_savefile_old()
{
    int i;

    /* XXX Fake the system info */


    /* Read the artifacts */
    rd_artifacts_old();
    if (say) prt_note(-1,"Loaded Artifacts");


    /* Load the Quests */
    for (i = 0; i < MAX_QUESTS; i++) {
	rd_u32b(&quests[i]);
    }
    if (say) prt_note(-1,"Loaded Quests");


    /* Load the old "Uniques" flags */
    for (i = 0; i < MAX_R_IDX; i++) {

	/* Ignore the "exist" info, extracted later */
	rd_u32b(&tmp32u);

	/* Already true, but do it again anyway */
	l_list[i].cur_num = 0;

	/* XXX Hack -- That unique has been killed */
	rd_u32b(&tmp32u);
	l_list[i].max_num = 100;
	if (r_list[i].cflags2 & MF2_UNIQUE) l_list[i].max_num = 1;
	if (tmp32u) l_list[i].max_num = 0;
    }
    if (say) prt_note(-1,"Loaded Unique Beasts");


    /* Monster Memory */
    while (1) {

	/* Read some info, check for sentinal */
	rd_u16b(&u16b_tmp);
	if (u16b_tmp == 0xFFFF) break;

	/* Incompatible save files */
	if (u16b_tmp >= MAX_R_IDX) {
	    prt_note(-2,"Too many monsters!");
	    return (21);
	}

	/* Extract the monster lore */
	rd_lore_old(&l_list[u16b_tmp]);
    }
    if (say) prt_note(-1,"Loaded Monster Memory");

    /* Read the options */
    rd_options_old();
    if (say) prt_note(-1, "Loaded options");

    /* Read the extra stuff */
    rd_extra_old();
    if (say) prt_note(-1, "Loaded extra information");


    /* Read the inventory */
    rd_inventory_old();


    /* Read spell info */
    rd_u32b(&spell_learned);
    rd_u32b(&spell_worked);
    rd_u32b(&spell_forgotten);
    rd_u32b(&spell_learned2);
    rd_u32b(&spell_worked2);
    rd_u32b(&spell_forgotten2);

    for (i = 0; i < 64; i++) {
	rd_byte(&spell_order[i]);
    }

    if (say) prt_note(-1, "Read spell information");


    /* Hack -- analyze the "object_ident" array. */
    for (i = 0; i < OBJECT_IDENT_SIZE; i++)  {            
    rd_byte(&object_ident[i]);
	}

    /* Old seeds */
    rd_u32b(&randes_seed);
    rd_u32b(&town_seed);

    /* Old messages */
    rd_u16b(&last_msg);
    for (i = 0; i < MAX_SAVE_MSG; i++)
	rd_string(old_msg[i]);

    /* Some leftover info */
    rd_u16b(&panic_save);
    rd_u16b(&total_winner);
    rd_u16b(&noscore);

    /* Read the player_hp array */
    for (i = 0; i < MAX_PLAYER_LEVEL; i++) rd_u16b(&player_hp[i]);

    /* Hack -- Version 2.6.2 did silly things */
    if (!older_than(2,6,2)) {
	u16b icky;
	for (i = 50; i < 99; i++) rd_u16b(&icky);
    }

    if (say) prt_note(-1, "Read some more information.");


    /* Read the stores */
    for (i = 0; i < MAX_STORES; i++) {
	if (rd_store_old(&store[i])) {
	    prt_note(-2,"ERROR reading store");
	    return (32);
	}
    }


    /* Time at which file was saved */
    rd_u32b(&sf_when);

    /* Read the cause of death, if any */
    rd_string(died_from);

    if (say) prt_note(-1, "All player info restored");


    /* I'm not dead yet... */
    if (!death) {

	/* Dead players have no dungeon */
	prt_note(-1,"Restoring Dungeon...");
	if (rd_dungeon_old()) {
	    prt_note(-2, "Error reading dungeon data");
	    return (25);
	}

	/* Really old version -- read stores again */
	if (older_than(2,1,3)) {

	    /* Read the stores (again) */
	    for (i = 0; i < MAX_STORES; i++) {
		if (rd_store_old(&store[i])) {
		    prt_note(-2,"ERROR in STORE_INVEN_MAX");
		    return (33);
		}
	    }
	}


	/* Time goes here, too */
	rd_u32b(&sf_when);
    }



    /* Assume success */
    return (0);
}


/*
 * Hack -- forward declare
 */
extern errr rd_old_sf(FILE *fff1, int vmaj, int vmin, int vpat, int say1);

/*
 * Read an old savefile
 */
errr rd_old_sf(FILE *fff1, int vmaj, int vmin, int vpat, int say1)
{
    fff = fff1;
    
    version_maj = vmaj;
    version_min = vmin;
    patch_level = vpat;

    say = say1;
        
    /* Read the old savefile */
    return (rd_savefile_old());
}




