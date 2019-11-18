/*
 * files.c: misc code to access files used by Moria 
 *
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

char *ANGBAND_DIR_FILES = NULL;		/* Dir: ascii files  */
char *ANGBAND_DIR_BONES = NULL;		/* Dir: ascii bones files */
char *ANGBAND_DIR_SAVE = NULL;		/* Dir: binary save files */

char *ANGBAND_NEWS = NULL;		/* News file */
char *ANGBAND_TOP = NULL;		/* was LIBDIR(files/newscores) */
char *ANGBAND_WELCOME = NULL;		/* Player generation help */
char *ANGBAND_VERSION = NULL;		/* Version information */

char *ANGBAND_WIZ = NULL;		/* Acceptable wizard uid's */
char *ANGBAND_HOURS = NULL;		/* Hours of operation */
char *ANGBAND_LOAD = NULL;		/* Load information */
char *ANGBAND_LOG = NULL;		/* Log file of some form */

char *ANGBAND_R_HELP = NULL;		/* Roguelike command help */
char *ANGBAND_O_HELP = NULL;		/* Original command help */
char *ANGBAND_W_HELP = NULL;		/* Wizard command help */
char *ANGBAND_OWIZ_HELP = NULL;	/* was LIBDIR(files/owizcmds.hlp) */



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

}





/* Prints a list of random objects to a file.  Note that -RAK-	 */
/* the objects produced is a sampling of objects which		 */
/* be expected to appear on that level.				 */
void 
print_objects()
{
    register int         i;
    int                  nobj, j, level;
    vtype                filename1;
    bigvtype             tmp_str;
    register FILE       *file1;
    register inven_type *i_ptr;

    prt("Produce objects on what level?: ", 0, 0);
    level = 0;
    if (!get_string(tmp_str, 0, 32, 10))
	return;
    level = atoi(tmp_str);
    prt("Produce how many objects?: ", 0, 0);
    nobj = 0;
    if (!get_string(tmp_str, 0, 27, 10))
	return;
    nobj = atoi(tmp_str);
    if ((nobj > 0) && (level > -1) && (level < 1201)) {
	if (nobj > 10000)
	    nobj = 10000;
	prt("File name: ", 0, 0);
	if (get_string(filename1, 0, 11, 64)) {
	    if (strlen(filename1) == 0)
		return;
	    if ((file1 = my_tfopen(filename1, "w")) != NULL) {
		(void)sprintf(tmp_str, "%d", nobj);
		prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
		put_qio();
		(void)fprintf(file1, "*** Random Object Sampling:\n");
		(void)fprintf(file1, "*** %d objects\n", nobj);
		(void)fprintf(file1, "*** For Level %d\n", level);
		(void)fprintf(file1, "\n");
		(void)fprintf(file1, "\n");
		j = i_pop();
		for (i = 0; i < nobj; i++) {
		    if (randint(MAX_DUNGEON_OBJ) == 1) {
			int                 tmp;

		again:
			tmp = randint(MAX_OBJECTS - (OBJ_SPECIAL - 1)) +
			    (OBJ_SPECIAL - 1) - 1;
			switch (tmp) {
			  case (OBJ_SPECIAL - 1):
			    if (k_list[tmp].level <= level)
				msg_print("*Remove Curse*");
			    break;
			  case (OBJ_SPECIAL):
			    if (k_list[tmp].level <= level)
				msg_print("[Resistance of Scarabtarices]");
			    break;
			  case (OBJ_SPECIAL + 1):
			    if (k_list[tmp].level <= level)
				msg_print("[Mordenkainen's Escapes]");
			    break;
			  case (OBJ_SPECIAL + 2):
			    if (k_list[tmp].level <= level)
				msg_print("[Kelek's Grimoire of Power]");
			    break;
			  case (OBJ_SPECIAL + 3):
			    if (randint(2) == 1)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("[Tenser's Transformations]");
			    break;
			  case (OBJ_SPECIAL + 4):
			    if (randint(2) == 1)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("[Raal's Tome of Destruction]");
			    break;
			  case (OBJ_SPECIAL + 5):
			    if (k_list[tmp].level <= level)
				msg_print("[Ethereal Openings]");
			    break;
			  case (OBJ_SPECIAL + 6):
			    if (k_list[tmp].level <= level)
				msg_print("[Godly Insights]");
			    break;
			  case (OBJ_SPECIAL + 7):
			    if (k_list[tmp].level <= level)
				msg_print("[Purifications and Healing]");
			    break;
			  case (OBJ_SPECIAL + 8):
			    if (k_list[tmp].level <= level)
				msg_print("[Wrath of God]");
			    break;
			  case (OBJ_SPECIAL + 9):
			    if (randint(2) > 1)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("[Holy Infusions]");
			    break; /* End of Books */
			  case (OBJ_SPECIAL + 10):	/* Narya */
			    if (randint(4) > 1)
				goto again;
			    if (NARYA)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Narya");
			    NARYA = TRUE;
			    break;
			  case (OBJ_SPECIAL + 11):	/* Nenya */
			    if (randint(5) > 1)
				goto again;
			    if (NENYA)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Nenya");
			    NENYA = TRUE;
			    break;
			  case (OBJ_SPECIAL + 12):	/* Vilya */
			    if (randint(6) > 1)
				goto again;
			    if (VILYA)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Vilya");
			    VILYA = TRUE;
			    break;
			  case (OBJ_SPECIAL + 13):	/* The Ring of Power */
			    if (randint(10) > 1)
				goto again;
			    if (POWER)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Power (The One Ring)");
			    POWER = TRUE;
			    break;
			  case (OBJ_SPECIAL + 14):	/* Dragon Scale mails */
			  case (OBJ_SPECIAL + 15):
			  case (OBJ_SPECIAL + 16):
			  case (OBJ_SPECIAL + 17):
			  case (OBJ_SPECIAL + 18):
			  case (OBJ_SPECIAL + 19):
			  case (OBJ_SPECIAL + 20):
			    if (randint(2) > 1)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Dragon Scale Mail");
			    break;
			  case (OBJ_SPECIAL + 23):	/* Multi-Hued Dragon
							 * Scale Mail */
			    if (randint(3) > 1)
				goto again;
			    if (k_list[tmp].level <= level)
				msg_print("Multi-Hued Dragon Scale Mail");
			    break;
			}
			if (k_list[tmp].level > level)
			    continue;
			invcopy(&i_list[j], tmp);
			if (k_list[i_list[j].index].tval == TV_HARD_ARMOR)
			    i_list[j].toac += 10 + randint(10);
			i_ptr = &i_list[j];
			store_bought(i_ptr);
			objdes(tmp_str, i_ptr, TRUE);
		    } else {
			invcopy(&i_list[j],
				sorted_objects[get_obj_num(level, FALSE)]);
			magic_treasure(j, level, 0, FALSE);
			i_ptr = &i_list[j];
			store_bought(i_ptr);
			if (i_ptr->flags & TR_CURSED)
			    add_inscribe(i_ptr, ID_DAMD);
			objdes(tmp_str, i_ptr, TRUE);
		    }
		    (void)fprintf(file1, "%d %s.\n", i_ptr->level, tmp_str);
		}
		pusht((s16b) j);
		(void)fclose(file1);
		prt("Completed.", 0, 0);
	    } else
		prt("File could not be opened.", 0, 0);
	}
    } else
	prt("Parameters no good.", 0, 0);
}

