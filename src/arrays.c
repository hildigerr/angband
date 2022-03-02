/* File: arrays.c */ 

/* Purpose: initialize various arrays */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"




/*
 * Prepare the ANGBAND_xxx filepath "constants".
 *
 * First, we'll look for the ANGBAND_PATH environment variable,
 * and then look for the files in there.  If that doesn't work,
 * we'll try the DEFAULT_PATH constant.  So be sure that one of
 * these two things works...
 *
 * The code is now a lot cleaner, with no realloc calls. -BEN-
 * And it is no longer optional, everyone must use it.  If you
 * wish to reinstate "constant paths", please do so by giving
 * the variables below constant "initial values".
 */

cptr ANGBAND_DIR_FILES = NULL;		/* Dir: ascii files  */
cptr ANGBAND_DIR_BONES = NULL;		/* Dir: ascii bones files */
cptr ANGBAND_DIR_SAVE = NULL;		/* Dir: binary save files */

cptr ANGBAND_NEWS = NULL;		/* News file */
cptr ANGBAND_TOP = NULL;		/* was LIBDIR(files/newscores) */
cptr ANGBAND_WELCOME = NULL;		/* Player generation help */
cptr ANGBAND_VERSION = NULL;		/* Version information */

cptr ANGBAND_WIZ = NULL;		/* Acceptable wizard uid's */
cptr ANGBAND_HOURS = NULL;		/* Hours of operation */
cptr ANGBAND_LOAD = NULL;		/* Load information */
cptr ANGBAND_LOG = NULL;		/* Log file of some form */

cptr ANGBAND_R_HELP = NULL;		/* Roguelike command help */
cptr ANGBAND_O_HELP = NULL;		/* Original command help */
cptr ANGBAND_W_HELP = NULL;		/* Wizard command help */
cptr ANGBAND_OWIZ_HELP = NULL;	/* was LIBDIR(files/owizcmds.hlp) */

cptr ANGBAND_R_LIST = NULL;		/* Ascii monster race file */
cptr ANGBAND_K_LIST = NULL;		/* Ascii item kind file */


/*
 * Find the paths to all of our important files and directories...
 * Use the ANGBAND_PATH environment var if possible, else use DEFAULT_PATH,
 * and then branch off appropriately from there (see below).
 *
 * If your system can't do "getenv()", you'll have to kludge this.  [cjh]
 */
void get_file_paths()
{
    /* The current path (and "workspace") */
    char path[1024];

    /* Pointer to the "end" of the workspace */
    char *tail;

    /* Grab the base "path" */
    char *angband_path = NULL;

    /* Get the environment variable */
    angband_path = getenv("ANGBAND_PATH");

    /* Use the angband_path, or a default */
    strcpy(path, angband_path ? angband_path : DEFAULT_PATH);

    /* Be sure not to duplicate any "Path separator" */
    if (!suffix(path,PATH_SEP)) strcat(path, PATH_SEP);

    /* Prepare to append to the Base Path */
    tail = path + strlen(path);

    /* Find some directory names */
    strcpy(tail, "save");
    ANGBAND_DIR_SAVE = string_make(path);
    strcpy(tail, "bones");
    ANGBAND_DIR_BONES = string_make(path);
    strcpy(tail, "files");
    ANGBAND_DIR_FILES = string_make(path);

    /* Add a path separator */
    strcat(tail, PATH_SEP);

    /* Use the "files" directory (from above) */
    tail = tail + strlen(tail);

    /* The basic info files */
    strcpy(tail, "news.hlp");
    ANGBAND_NEWS = string_make(path);
    strcpy(tail, "newscores");
    ANGBAND_TOP = string_make(path);
    strcpy(tail, "welcome.hlp");
    ANGBAND_WELCOME = string_make(path);
    strcpy(tail, "version.hlp");
    ANGBAND_VERSION = string_make(path);

    /* The command help files */
    strcpy(tail, "cmds_r.hlp");
    ANGBAND_R_HELP = string_make(path);
    strcpy(tail, "cmds_o.hlp");
    ANGBAND_O_HELP = string_make(path);
    strcpy(tail, "cmds_w.hlp");
    ANGBAND_W_HELP = string_make(path);
    strcpy(tail, "owizcmds.hlp");
    ANGBAND_OWIZ_HELP = string_make(path);

    /* Some parsable text files */
    strcpy(tail, "wizards.txt");
    ANGBAND_WIZ = string_make(path);
    strcpy(tail, "ANGBAND.log");
    ANGBAND_LOG = string_make(path);
    strcpy(tail, "hours");
    ANGBAND_HOURS = string_make(path);
    strcpy(tail, "loadcheck" );
    ANGBAND_LOAD = string_make(path);

    /* Parsable Item/Monster template files */
    strcpy(tail, "r_list.txt");
    ANGBAND_R_LIST = string_make(path);
    strcpy(tail, "k_list.txt");
    ANGBAND_K_LIST = string_make(path);
}


/*
 * Grab one flag in a monster_race from a textual string
 */
static int grab_one_flag(monster_race *r_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L;

    if (!what) what = what;

    else if (streq(what,"WINNER"))	flags1 |= MF1_WINNER;
    else if (streq(what,"QUESTOR"))	flags2 |= MF2_QUESTOR;
    else if (streq(what,"UNIQUE"))	flags2 |= MF2_UNIQUE;
    else if (streq(what,"MAX_HP"))	flags2 |= MF2_MAX_HP;
    else if (streq(what,"INTELLIGENT"))	flags2 |= MF2_INTELLIGENT;

    else if (streq(what,"SPECIAL"))	flags2 |= MF2_SPECIAL;
    else if (streq(what,"GOOD"))	flags2 |= MF2_GOOD;
    else if (streq(what,"CARRY_GOLD"))	flags1 |= MF1_CARRY_GOLD;
    else if (streq(what,"CARRY_OBJ"))	flags1 |= MF1_CARRY_OBJ;
    else if (streq(what,"PICK_UP"))	flags1 |= MF1_PICK_UP;

    else if (streq(what,"MV_ONLY_ATT"))	flags1 |= MF1_MV_ONLY_ATT;
    else if (streq(what,"MV_ATT_NORM"))	flags1 |= MF1_MV_ATT_NORM;
    else if (streq(what,"MV_20"))	flags1 |= MF1_MV_20;
    else if (streq(what,"MV_40"))	flags1 |= MF1_MV_40;
    else if (streq(what,"MV_75"))	flags1 |= MF1_MV_75;
    else if (streq(what,"MV_INVIS"))	flags1 |= MF1_MV_INVIS;
    else if (streq(what,"NO_INFRA"))	flags2 |= MF2_NO_INFRA;
    else if (streq(what,"MINDLESS"))	flags2 |= MF2_MINDLESS;
    else if (streq(what,"THRO_DR"))	flags1 |= MF1_THRO_DR;
    else if (streq(what,"THRO_WALL"))	flags1 |= MF1_THRO_WALL;
    else if (streq(what,"THRO_CREAT"))	flags1 |= MF1_THRO_CREAT;
    else if (streq(what,"MULTIPLY"))	flags1 |= MF1_MULTIPLY;
    else if (streq(what,"GROUP"))	flags2 |= MF2_GROUP;
    else if (streq(what,"HAS_60"))	flags1 |= MF1_HAS_60;
    else if (streq(what,"HAS_90"))	flags1 |= MF1_HAS_90;
    else if (streq(what,"HAS_1D2"))	flags1 |= MF1_HAS_1D2;
    else if (streq(what,"HAS_2D2"))	flags1 |= MF1_HAS_2D2;
    else if (streq(what,"HAS_4D2"))	flags1 |= MF1_HAS_4D2;

    else if (streq(what,"ANIMAL"))	flags2 |= MF2_ANIMAL;
    else if (streq(what,"EVIL"))	flags2 |= MF2_EVIL;
    else if (streq(what,"ORC"))	flags2 |= MF2_ORC;
    else if (streq(what,"TROLL"))	flags2 |= MF2_TROLL;
    else if (streq(what,"GIANT"))	flags2 |= MF2_GIANT;
    else if (streq(what,"DRAGON"))	flags2 |= MF2_DRAGON;
    else if (streq(what,"DEMON"))	flags2 |= MF2_DEMON;
    else if (streq(what,"UNDEAD"))	flags2 |= MF2_UNDEAD;

    else if (streq(what,"IM_ACID"))	flags2 |= MF2_IM_ACID;
    else if (streq(what,"IM_FIRE"))	flags2 |= MF2_IM_FIRE;
    else if (streq(what,"IM_COLD"))	flags2 |= MF2_IM_COLD;
    else if (streq(what,"IM_ELEC"))	flags2 |= MF2_IM_ELEC;
    else if (streq(what,"IM_POIS"))	flags2 |= MF2_IM_POIS;

    else if (streq(what,"HURT_LITE"))	flags2 |= MF2_HURT_LITE;
    else if (streq(what,"HURT_ROCK"))	flags2 |= MF2_HURT_ROCK;
    else if (streq(what,"CHARM_SLEEP"))	flags2 |= MF2_CHARM_SLEEP;
    else if (streq(what,"BREAK_WALL"))	flags2 |= MF2_BREAK_WALL;
    else if (streq(what,"DESTRUCT"))	flags2 |= MF2_DESTRUCT;

    else if (streq(what,"MALE"))	r_ptr->gender = 'm';
    else if (streq(what,"FEMALE"))	r_ptr->gender = 'f';
    else if (streq(what,"PLURAL"))	r_ptr->gender = 'p';

    if (!flags1 && !flags2) return (0);

    if (flags1) r_ptr->cflags1 |= flags1;
    if (flags2) r_ptr->cflags2 |= flags2;

    return (1);
}


/*
 * Grab one spell in a monster_race from a textual string
 */
static int grab_one_spell(monster_race *r_ptr, cptr what)
{
    u32b flags1 = 0L, flags2 = 0L, flags3 = 0L;

    int chance;

    if (!what) what = what;

    /* Hack -- store the "frequency" in the spell flags */
    else if (1 == sscanf(what, "1_IN_%d", &chance)) {

	/* Hack -- frequency stored as "flags" */
	flags1 |= ((u32b)chance & CS1_FREQ);
    }

    else if (streq(what,"HEAL"))		flags2 |= MS2_HEAL;
    else if (streq(what,"HASTE"))		flags2 |= MS2_HASTE;
    else if (streq(what,"BLINK"))		flags1 |= MS1_BLINK;
    else if (streq(what,"TELEPORT"))		flags1 |= MS1_TELEPORT;

    else if (streq(what,"TELE_TO"))		flags1 |= MS1_TELE_TO;
    else if (streq(what,"TELE_AWAY"))		flags2 |= MS2_TELE_AWAY;
    else if (streq(what,"TELE_LEVEL"))	flags2 |= MS2_TELE_LEVEL;
    else if (streq(what,"BLIND"))		flags1 |= MS1_BLIND;
    else if (streq(what,"HOLD"))		flags1 |= MS1_HOLD;
    else if (streq(what,"SLOW"))		flags1 |= MS1_SLOW;
    else if (streq(what,"CONF"))		flags1 |= MS1_CONF;
    else if (streq(what,"FEAR"))		flags1 |= MS1_FEAR;

    else if (streq(what,"CAUSE_1"))		flags1 |= MS1_CAUSE_1;
    else if (streq(what,"CAUSE_2"))		flags1 |= MS1_CAUSE_2;
    else if (streq(what,"CAUSE_3"))		flags1 |= MS1_CAUSE_3;
    else if (streq(what,"ARROW_1"))		flags1 |= MS1_ARROW_1;
    else if (streq(what,"ARROW_2"))		flags2 |= MS2_ARROW_2;
    else if (streq(what,"ARROW_3"))		flags3 |= MS3_ARROW_3;
    else if (streq(what,"RAZOR"))		flags2 |= MS2_RAZOR;

    else if (streq(what,"MANA_DRAIN"))	flags1 |= MS1_MANA_DRAIN;
    else if (streq(what,"MIND_BLAST"))	flags2 |= MS2_MIND_BLAST;
    else if (streq(what,"BRAIN_SMASH"))	flags2 |= MS2_BRAIN_SMASH;
    else if (streq(what,"FORGET"))		flags2 |= MS2_FORGET;
    else if (streq(what,"TRAP_CREATE"))	flags2 |= MS2_TRAP_CREATE;
    else if (streq(what,"DARKNESS"))		flags2 |= MS2_DARKNESS;
    else if (streq(what,"DARK_STORM"))	flags3 |= MS3_DARK_STORM;
    else if (streq(what,"MANA_STORM"))	flags3 |= MS3_MANA_STORM;

    else if (streq(what,"BO_ACID"))		flags1 |= MS1_BO_ACID;
    else if (streq(what,"BO_FIRE"))		flags1 |= MS1_BO_FIRE;
    else if (streq(what,"BO_COLD"))		flags1 |= MS1_BO_COLD;
    else if (streq(what,"BO_ELEC"))		flags2 |= MS2_BO_ELEC;
    else if (streq(what,"BO_ICEE"))		flags2 |= MS2_BO_ICEE;
    else if (streq(what,"BO_WATE"))		flags2 |= MS2_BO_WATE;
    else if (streq(what,"BO_MANA"))		flags1 |= MS1_BO_MANA;
    else if (streq(what,"BO_PLAS"))		flags2 |= MS2_BO_PLAS;
    else if (streq(what,"BO_NETH"))		flags2 |= MS2_BO_NETH;

    else if (streq(what,"BA_ACID"))		flags2 |= MS2_BA_ACID;
    else if (streq(what,"BA_FIRE"))		flags1 |= MS1_BA_FIRE;
    else if (streq(what,"BA_COLD"))		flags1 |= MS1_BA_COLD;
    else if (streq(what,"BA_ELEC"))		flags2 |= MS2_BA_ELEC;
    else if (streq(what,"BA_WATE"))		flags2 |= MS2_BA_WATE;
    else if (streq(what,"BA_POIS"))		flags2 |= MS2_BA_POIS;
    else if (streq(what,"BA_NETH"))		flags2 |= MS2_BA_NETH;

    else if (streq(what,"BR_ACID"))		flags1 |= MS1_BR_ACID;
    else if (streq(what,"BR_FIRE"))		flags1 |= MS1_BR_FIRE;
    else if (streq(what,"BR_COLD"))		flags1 |= MS1_BR_COLD;
    else if (streq(what,"BR_ELEC"))		flags1 |= MS1_BR_ELEC;
    else if (streq(what,"BR_POIS"))		flags1 |= MS1_BR_POIS;
    else if (streq(what,"BR_LITE"))		flags3 |= MS3_BR_LITE;
    else if (streq(what,"BR_DARK"))		flags3 |= MS3_BR_DARK;
    else if (streq(what,"BR_SOUN"))		flags2 |= MS2_BR_SOUN;
    else if (streq(what,"BR_CONF"))		flags2 |= MS2_BR_CONF;
    else if (streq(what,"BR_CHAO"))		flags2 |= MS2_BR_CHAO;
    else if (streq(what,"BR_SHAR"))		flags2 |= MS2_BR_SHAR;
    else if (streq(what,"BR_LIFE"))		flags2 |= MS2_BR_LIFE;
    else if (streq(what,"BR_DISE"))		flags2 |= MS2_BR_DISE;
    else if (streq(what,"BR_WALL"))		flags3 |= MS3_BR_WALL;
    else if (streq(what,"BR_SLOW"))		flags3 |= MS3_BR_SLOW;
    else if (streq(what,"BR_TIME"))		flags3 |= MS3_BR_TIME;
    else if (streq(what,"BR_GRAV"))		flags3 |= MS3_BR_GRAV;
    else if (streq(what,"BR_PLAS"))		flags3 |= MS3_BR_PLAS;
    else if (streq(what,"BR_NETH"))		flags2 |= MS2_BR_NETH;

    else if (streq(what,"S_MONSTER"))		flags1 |= MS1_S_MONSTER;
    else if (streq(what,"S_SUMMON"))		flags2 |= MS2_S_SUMMON;
    else if (streq(what,"S_UNDEAD"))		flags1 |= MS1_S_UNDEAD;
    else if (streq(what,"S_DEMON"))		flags1 |= MS1_S_DEMON;
    else if (streq(what,"S_DRAGON"))		flags1 |= MS1_S_DRAGON;
    else if (streq(what,"S_ANGEL"))		flags2 |= MS2_S_ANGEL;
    else if (streq(what,"S_REPTILE"))		flags3 |= MS3_S_REPTILE;
    else if (streq(what,"S_SPIDER"))		flags2 |= MS2_S_SPIDER;
    else if (streq(what,"S_ANT"))		flags3 |= MS3_S_ANT;
    else if (streq(what,"S_HOUND"))		flags2 |= MS2_S_HOUND;
    else if (streq(what,"S_UNIQUE"))		flags3 |= MS3_S_UNIQUE;
    else if (streq(what,"S_WRAITH"))		flags3 |= MS3_S_WRAITH;
    else if (streq(what,"S_GUNDEAD"))		flags3 |= MS3_S_GUNDEAD;
    else if (streq(what,"S_ANCIENTD"))	flags3 |= MS3_S_ANCIENTD;

    if (!flags1 && !flags2 && !flags3) return (0);

    if (flags1) r_ptr->spells1 |= flags1;
    if (flags2) r_ptr->spells2 |= flags2;
    if (flags3) r_ptr->spells3 |= flags3;

    return (1);
}


/*
 * Hack -- indicate errors during parsing of "r_list.txt"
 */
static int error_r_idx = -1;

/*
 * Initialize the "r_list" array by parsing an ascii file
 */
static errr init_r_list_txt(void)
{
    register char *s, *t;

    /* No monster yet */
    int m = -1;

    /* No r_ptr yet */
    monster_race *r_ptr = NULL;

    /* The "monsters" file */
    FILE *fp;

    /* No line should be more than 80 chars */
    char buf[160];

    /* Current race description */
    char desc[24*80];

    /* Open the monster file */
    fp = fopen(ANGBAND_R_LIST, "r");

    /* Failure */
    if (!fp) return (-1);

    
    /* Load the monster descriptions from the file */
    while (1) {

	/* Read a line from the file, stop when done */
	if (!fgets(buf, 160, fp)) break;

	/* Skip comments */
	if (buf[0] == '#') continue;

	/* Strip the final newline */
	for (s = buf; isprint(*s); ++s); *s = '\0';

	/* Blank lines terminate monsters */
	if (!buf[0]) {

	    /* No current r_ptr */
	    if (!r_ptr) continue;

	    /* Save the decription */
	    if (desc[0]) r_ptr->desc = string_make(desc);

	    /* Now there is no current r_ptr */
	    r_ptr = NULL;

	    /* Next... */
	    continue;
	}

	/* The line better have a colon and such */
	if (buf[1] != ':') return (1);

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N') {

	    /* Not done the previous one */
	    if (r_ptr) return (11);

	    /* Find, verify, and nuke the colon before the name */
	    if (!(s = strchr(buf+2, ':'))) return (2);

	    /* Nuke the colon, advance to the name */
	    *s++ = '\0';

	    /* Require non-empty names */
	    if (!*s) return (3);

	    /* Get the index */
	    m = atoi(buf+2);

	    /* Save the index */
	    error_r_idx = m;

	    /* Verify */
	    if ((m < 0) || (m >= MAX_R_IDX)) return (7);

	    /* Start a new r_ptr */
	    r_ptr = &r_list[m];

	    /* Make sure we have not done him yet */
	    if (r_ptr->name) return (8);

	    /* Save the name */
	    r_ptr->name = string_make(s);

	    /* No desc yet */
	    desc[0] = '\0';

	    /* Next... */
	    continue;
	}

	/* There better be a current r_ptr */
	if (!r_ptr) return (10);

	/* Process 'I' for "Info" (one line only) */
	if (buf[0] == 'I') {

	    char chr;
	    int tmp, spd, hp1, hp2, aaf, ac, slp, rar, lev;
	    long exp;

	    /* Scan for the values */
	    if (11 != sscanf(buf, "I:%c:%d:%dd%d:%d:%d:%d:%d:%d:%ld",
		&chr, &spd, &hp1, &hp2,
		&aaf, &ac, &slp, &rar, &lev, &exp)) return (11);

	    /* Save the values */
	    r_ptr->r_char = chr;
	    r_ptr->speed = spd;
	    r_ptr->hd[0] = hp1;
	    r_ptr->hd[1] = hp2;
	    r_ptr->aaf = aaf;
	    r_ptr->ac = ac;
	    r_ptr->sleep = slp;
	    r_ptr->rarity = rar;
	    r_ptr->level = lev;
	    r_ptr->mexp = exp;

	    /* Next... */
	    continue;
	}

	/* Process 'A' for "Attacks" (one line only) */
	if (buf[0] == 'A') {

	    int i;

	    /* Simply read each number following a colon */
	    for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i) {

		/* Store the attack damage index */
		r_ptr->damage[i] = atoi(s+1);

		/* Find the next colon */
		s = strchr(s+1, ':');
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'F' for "Flags" (multiple lines) */
	if (buf[0] == 'F') {

	    /* Parse every entry */
	    for (s = buf + 2; *s; ) {

		/* Find the end of this entry */
		for (t = s; *t && *t != ' ' && *t != '|'; ++t);

		/* Nuke and skip any dividers */
		if (*t) {
		    *t++ = '\0';
		    while (*t == ' ' || *t == '|') t++;
		}

		/* Parse this entry */
		if (!grab_one_flag(r_ptr, s)) return (18);

		/* Start the next entry */
		s = t;
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'S' for "Spells" (multiple lines) */
	if (buf[0] == 'S') {

	    /* Parse every entry */
	    for (s = buf + 2; *s; ) {

		/* Find the end of this entry */
		for (t = s; *t && *t != ' ' && *t != '|'; ++t);

		/* Nuke and skip any dividers */
		if (*t) {
		    *t++ = '\0';
		    while (*t == ' ' || *t == '|') t++;
		}

		/* Parse this entry */
		if (!grab_one_spell(r_ptr, s)) return (19);

		/* Start the next entry */
		s = t;
	    }

	    /* Next... */
	    continue;
	}

	/* Process 'D' for "Description" */
	if (buf[0] == 'D') {

	    /* Collect the description, allocated later */
	    strcat(desc,buf+2);

	    /* Next... */
	    continue;
	}
    }

    /* Close the file */
    fclose(fp);


    /* Success */
    return (0);
}


/*
 * Initialize the "r_list" array by parsing various files.
 */
static void init_r_list()
{
    errr err;

    /* Try the text version */
    err = init_r_list_txt();

    /* Still no luck? Fail! */
    if (err) {
    
	/* Warning */
	msg_print(format("Fatal error #%d parsing 'r_list.txt', record %d",
			 err, error_r_idx));
	msg_print(NULL);

	/* Quit */
	quit("cannot load 'r_list.txt'");
    }

}


/*
 * Hack -- location saver for error messages
 */
static int error_k_idx = -1;


/*
 * Initialize the "k_list" array by parsing a file
 * Note that "k_list" starts out totally cleared
 */
static errr init_k_list_txt()
{
    register char *s, *t;

    /* No item kind yet */
    int m = -1;

    /* No k_ptr yet */
    inven_kind *k_ptr = NULL;

    /* The "objects" file */
    FILE *fp;

    /* No line should be more than 80 chars */
    char buf[160];

    /* Open the file */
    fp = fopen(ANGBAND_K_LIST, "r");

    /* Failure */
    if (!fp) return (-1);

    /* Parse the file to initialize "k_list" */
    while (1) {

	/* Read a line from the file, stop when done */
	if (!fgets(buf, 160, fp)) break;

	/* Skip comments */
	if (buf[0] == '#') continue;

	/* Strip the final newline */
	for (s = buf; isprint(*s); ++s); *s = '\0';

	/* Blank lines terminate monsters */
	if (!buf[0]) {

	    /* No current k_ptr */
	    if (!k_ptr) continue;

	    /* Now there is no current k_ptr */
	    k_ptr = NULL;

	    /* Next... */
	    continue;
	}

	/* The line better have a colon and such */
	if (buf[1] != ':') return (1);

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N') {

	    /* Not done the previous one */
	    if (k_ptr) return (2);

	    /* Find, verify, and nuke the colon before the name */
	    if (!(s = strchr(buf+2, ':'))) return (3);

	    /* Nuke the colon, advance to the name */
	    *s++ = '\0';

	    /* Do not allow empty names */
	    if (!*s) return (4);

	    /* Get the index */
	    m = atoi(buf+2);

	    /* For errors */
	    error_k_idx = m;

	    /* Verify */
	    if ((m < 0) || (m >= MAX_OBJECTS)) return (5);

	    /* Start a new k_ptr */
	    k_ptr = &k_list[m];

	    /* Make sure we have not done him yet */
	    if (k_ptr->name) return (6);

	    /* Save the name */
	    k_ptr->name = string_make(s);

	    /* Next... */
	    continue;
	}

	/* There better be a current k_ptr */
	if (!k_ptr) return (10);

	/* Process 'I' for "Info" (one line only) */
	if (buf[0] == 'I') {

	    char sym;
	    int tval, sval, pval, num, wgt, lev;
	    long cost;

	    /* Scan for the values */
	    if (8 != sscanf(buf+2, "%c:%c:%d:%d:%d:%d:%d:%ld",
		&sym, &tval, &sval, &pval,
		&num, &wgt, &lev, &cost)) return (11);

	    /* Save the values */
	    k_ptr->tchar = sym;
	    k_ptr->tval = tval;
	    k_ptr->sval = sval;
	    k_ptr->pval = pval;
	    k_ptr->number = num;
	    k_ptr->weight = wgt;
	    k_ptr->level = lev;
	    k_ptr->cost = cost;


	    /* Next... */
	    continue;
	}

	/* Process 'A' for "Allocation" (one line only) */
	if (buf[0] == 'A') {

	    int i;

	    /* Simply read each number following a colon */
	    for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i) {

		/* Find the slash */
		t = strchr(s+1, '/');

		/* Find the next colon */
		s = strchr(s+1, ':');

		/* If the slash is "nearby", use it */
		if (t && (!s || t < s)) {
		    int chance = atoi(t+1);
		    if (chance > 0) k_ptr->rare = chance;
		}
	    }

	    /* Next... */
	    continue;
	}

	/* Hack -- Process 'P' for "power" and such */
	if (buf[0] == 'P') {

	    int ac, hd1, hd2, th, td, ta;

	    /* Scan for the values */
	    if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
		&ac, &hd1, &hd2, &th, &td, &ta)) return (15);

	    k_ptr->ac = ac;
	    k_ptr->damage[0] = hd1;
	    k_ptr->damage[1] = hd2;
	    k_ptr->tohit = th;
	    k_ptr->todam = td;
	    k_ptr->toac =  ta;

	    /* Next... */
	    continue;
	}

	/* Hack -- Process 'F' for flags */
	if (buf[0] == 'F') {

	    huge flags1, flags2;

	    /* XXX XXX Hack -- Scan for "pure" values */
	    /* Note that "huge" may not equal "u32b" */
	    if (3 == sscanf(buf+2, "0x%lx:0x%lx",
		&flags1, &flags2)) {

		k_ptr->flags1 = flags1;
		k_ptr->flags2 = flags2;

		continue;
	    }

	    /* Next... */
	    continue;
	}
    }

    /* Close the file */
    fclose(fp);

    /* Success */
    return (0);
}



/*
 * Note that "k_list" starts out totally cleared
 */
static void init_k_list()
{
    errr err;

    err = init_k_list_txt();

    /* Still no luck? Fail! */
    if (err) {
    
	/* Warning */
	msg_print(format("Fatal error #%d parsing 'k_list.txt', record %d",
			 err, error_k_idx));
	msg_print(NULL);

	/* Quit */
	quit("cannot load 'k_list.txt'");
    }

}



static char original_commands(char command)
{
    int dir_val;

    /* Process the command */
    switch (command) {

	case CTRL('K'):		/* ^K = exit    */
	    command = 'Q';
	break;
	case CTRL('J'):		/* not used */
	case CTRL('M'):		/* not used */
	    command = ' ';
	    break;
	case CTRL('F'):		/* ^F = repeat feeling */
	case CTRL('R'):		/* ^R = redraw screen  */
	case CTRL('P'):		/* ^P = repeat  */
	case CTRL('W'):		/* ^W = enter wizard mode */
	case CTRL('X'):		/* ^X = save    */
	case ' ':
	    break;
	case '.': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT */
	    int temp = target_mode;
	    target_mode = FALSE;
#endif
	    if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	    case 1:
	    command = 'B';
	    break;
	    case 2:
	    command = 'J';
	    break;
	    case 3:
	    command = 'N';
	    break;
	    case 4:
	    command = 'H';
	    break;
	    case 6:
	    command = 'L';
	    break;
	    case 7:
	    command = 'Y';
	    break;
	    case 8:
	    command = 'K';
	    break;
	    case 9:
	    command = 'U';
	    break;
	    default:
	    command = ' ';
	    break;
	    }
	    else
	    command = ' ';
#ifdef TARGET
	    target_mode = temp; /* restore old target code ... -CFT */
#endif
	    }
	    break;
	case '/':
	case '<':
	case '>':
	case '-':
	case '=':
	case '{':
	case '?':
	case 'A':
	    break;
	case '1':
	    command = 'b';
	    break;
	case '2':
	    command = 'j';
	    break;
	case '3':
	    command = 'n';
	    break;
	case '4':
	    command = 'h';
	    break;
	case '5':			/* Rest one turn */
	    command = '.';
	    break;
	case '6':
	    command = 'l';
	    break;
	case '7':
	    command = 'y';
	    break;
	case '8':
	    command = 'k';
	    break;
	case '9':
	    command = 'u';
	    break;
	case 'B':
	    command = 'f';
	    break;
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'g':
	    break;
	case 'L':
	    command = 'W';
	    break;
	case 'M':
	case 'R':
	    break;
	case 'S':
	    command = '#';
	    break;
	case 'T': {
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	    int temp = target_mode;
	    target_mode = FALSE;
#endif
	    if (get_dir(NULL, &dir_val))
	    switch (dir_val) {
	    case 1:
	    command = CTRL('B');
	    break;
	    case 2:
	    command = CTRL('J');
	    break;
	    case 3:
	    command = CTRL('N');
	    break;
	    case 4:
	    command = CTRL('H');
	    break;
	    case 6:
	    command = CTRL('L');
	    break;
	    case 7:
	    command = CTRL('Y');
	    break;
	    case 8:
	    command = CTRL('K');
	    break;
	    case 9:
	    command = CTRL('U');
	    break;
	    default:
	    command = ' ';
	    break;
	    }
	    else
	    command = ' ';
#ifdef TARGET
	    target_mode = temp;
#endif
	    }
	    break;
	case 'a':
	    command = 'z';
	    break;
	case 'b':
	    command = 'P';
	    break;
	case 'c':
	case 'd':
	case 'e':
	    break;
	case 'f':
	    command = 't';
	    break;
	case 'h':
	    command = '?';
	    break;
	case 'i':
	    break;
	case 'j':
	    command = 'S';
	    break;
	case 'l':
	    command = 'x';
	break;
	case 'm':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	    break;
	case 't':
	    command = 'T';
	    break;
	case 'u':
	    command = 'Z';
	    break;
	case 'z':
	    command = 'a';
	    break;
	case 'v':
	case 'V':
	case 'w':
	    break;
	case 'x':
	    command = 'X';
	break;

    /* wizard mode commands follow */
	case '\\':		/* \ = wizard help */
	case CTRL('A'):		/* ^A = cure all */
	case CTRL('D'):		/* ^D = up/down */
	case CTRL('E'):		/* ^E = wizchar */
	case CTRL('G'):		/* ^G = treasure */
	case CTRL('I'):		/* ^I = identify */
	case CTRL('O'):		/* ^O = generate objects */
	case CTRL('T'):		/* ^T = teleport */
	case CTRL('V'):		/* ^V = treasure */
	case CTRL('Z'):		/* ^Z = genocide */
	case ':':			/* map area */
	case '~':			/* artifact list to file */
	case '!':			/* rerate hitpoints */
	case '@':			/* create object */
	case '$':			/* wiz. light */
	case '%':			/* '%' == self knowledge */
	case '&':			/* & = summon  */
	case '*':			/* Indentify up to level */
	case '+':			/* add experience */
	case '|':			/* check uniques - cba */
	    break;
	default:
	    command = '(';		/* Anything illegal. */
	    break;
    }
    return com_val;
}


/*
 * Note that the "C_MAKE()" macro allocates "clean" memory.
 */
void init_some_arrays()
{
    /* Allocate and Wipe the array of monster "race info" */
    C_MAKE(r_list, MAX_R_IDX, monster_race);

    /* Allocate and Wipe the array of object "kind info" */
    C_MAKE(k_list, MAX_OBJECTS, inven_kind);

    /* Initialize r_list from a file of some kind */
    init_r_list();

    /* Initialize k_list from a file */
    init_k_list();
}



