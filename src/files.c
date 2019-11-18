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
 * This is rather inelegant code; no checks are made for malloc() or
 * realloc() failures.  If you malloc/realloc are broken, you're in
 * trouble, and your compiler vendor needs a good swift kick in the
 * forehead. [cjh]
 * And it is no longer optional, everyone must use it.  If you
 * wish to reinstate "constant paths", please do so by giving
 * the variables below constant "initial values".
 */

char *ANGBAND_TST = NULL;		/* was LIBDIR(test)			*/
char *ANGBAND_DIR_BONES = NULL;		/* was LIBDIR(bones)			*/
char *ANGBAND_DIR_SAVE = NULL;		/* was LIBDIR(save)			*/

char *ANGBAND_NEWS = NULL;		/* was LIBDIR(files/news.hlp)		*/
char *ANGBAND_TOP = NULL;		/* was LIBDIR(files/newscores)		*/
char *ANGBAND_WELCOME = NULL;		/* was LIBDIR(files/welcome.hlp)	*/
char *ANGBAND_VERSION = NULL;		/* was LIBDIR(files/version.hlp)	*/

char *ANGBAND_WIZ = NULL;		/* was LIBDIR(files/wizards.txt)		*/
char *ANGBAND_HOURS = NULL;		/* was LIBDIR(files/hours)		*/
char *ANGBAND_LOAD = NULL;		/* was LIBDIR(files/loadcheck)		*/
char *ANGBAND_LOG = NULL;		/* was LIBDIR(files/ANGBAND.log)	*/

char *ANGBAND_R_HELP = NULL;		/* was LIBDIR(files/cmds_r.hlp)	*/
char *ANGBAND_O_HELP = NULL;	/* was LIBDIR(files/cmds_o.hlp)	*/
char *ANGBAND_W_HELP = NULL;		/* was LIBDIR(files/cmds_w.hlp)	*/
char *ANGBAND_OWIZ_HELP = NULL;	/* was LIBDIR(files/owizcmds.hlp)	*/



/* Find the path to our files...  if the ANGBAND_PATH environment var
 * isn't set, use DEFAULT_PATH to look for the files.  If your system
 * can't do environment variables, you'll have to kludge this.  [cjh]
 */
void get_file_paths()
{
#ifndef MAXPATHLEN
#define BOZO_MAXPATHLEN
#define MAXPATHLEN 512
#endif

    char *angband_path = NULL;

    angband_path = getenv( "ANGBAND_PATH" );
    if (angband_path == NULL) {

	angband_path = (char *)malloc( strlen( DEFAULT_PATH ) + 1 );
	strcpy( angband_path, DEFAULT_PATH );
    }

    ANGBAND_TST = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_TST, angband_path );
    strcat( ANGBAND_TST, PATH_SEP );
    strcat( ANGBAND_TST, "test" );
    ANGBAND_TST = (char *)realloc( ANGBAND_TST, strlen( ANGBAND_TST ) + 1 );

    ANGBAND_DIR_SAVE = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_DIR_SAVE, angband_path );
    strcat( ANGBAND_DIR_SAVE, PATH_SEP );
    strcat( ANGBAND_DIR_SAVE, "save" );
    ANGBAND_DIR_SAVE = (char *)realloc( ANGBAND_DIR_SAVE, strlen( ANGBAND_DIR_SAVE ) + 1 );

    ANGBAND_DIR_BONES = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_DIR_BONES, angband_path );
    strcat( ANGBAND_DIR_BONES, PATH_SEP );
    strcat( ANGBAND_DIR_BONES, "bones" );
    ANGBAND_DIR_BONES = (char *)realloc( ANGBAND_DIR_BONES, strlen( ANGBAND_DIR_BONES ) + 1 );

    ANGBAND_NEWS = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_NEWS, angband_path );
    strcat( ANGBAND_NEWS, PATH_SEP );
    strcat( ANGBAND_NEWS, "files" );
    strcat( ANGBAND_NEWS, PATH_SEP );
    strcat( ANGBAND_NEWS, "news.hlp" );
    ANGBAND_NEWS = (char *)realloc( ANGBAND_NEWS, strlen( ANGBAND_NEWS ) + 1 );

    ANGBAND_TOP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_TOP, angband_path );
    strcat( ANGBAND_TOP, PATH_SEP );
    strcat( ANGBAND_TOP, "files" );
    strcat( ANGBAND_TOP, PATH_SEP );
    strcat( ANGBAND_TOP, "newscores" );
    ANGBAND_TOP = (char *)realloc( ANGBAND_TOP, strlen( ANGBAND_TOP ) + 1 );

    ANGBAND_WELCOME = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_WELCOME, angband_path );
    strcat( ANGBAND_WELCOME, PATH_SEP );
    strcat( ANGBAND_WELCOME, "files" );
    strcat( ANGBAND_WELCOME, PATH_SEP );
    strcat( ANGBAND_WELCOME, "welcome.hlp" );
    ANGBAND_WELCOME = (char *)realloc( ANGBAND_WELCOME, strlen( ANGBAND_WELCOME ) + 1 );

    ANGBAND_VERSION = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_VERSION, angband_path );
    strcat( ANGBAND_VERSION, PATH_SEP );
    strcat( ANGBAND_VERSION, "files" );
    strcat( ANGBAND_VERSION, PATH_SEP );
    strcat( ANGBAND_VERSION, "version.hlp" );
    ANGBAND_VERSION = (char *)realloc( ANGBAND_VER, strlen( ANGBAND_VER ) + 1 );

    ANGBAND_R_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_R_HELP, angband_path );
    strcat( ANGBAND_R_HELP, PATH_SEP );
    strcat( ANGBAND_R_HELP, "files" );
    strcat( ANGBAND_R_HELP, PATH_SEP );
    strcat( ANGBAND_R_HELP, "cmds_r.hlp" );
    ANGBAND_R_HELP = (char *)realloc( ANGBAND_R_HELP, strlen( ANGBAND_R_HELP ) + 1 );

    ANGBAND_O_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_O_HELP, angband_path );
    strcat( ANGBAND_O_HELP, PATH_SEP );
    strcat( ANGBAND_O_HELP, "files" );
    strcat( ANGBAND_O_HELP, PATH_SEP );
    strcat( ANGBAND_O_HELP, "cmds_o.hlp" );
    ANGBAND_O_HELP = (char *)realloc(ANGBAND_O_HELP,
					strlen( ANGBAND_O_HELP ) + 1 );

    ANGBAND_W_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_W_HELP, angband_path );
    strcat( ANGBAND_W_HELP, PATH_SEP );
    strcat( ANGBAND_W_HELP, "files" );
    strcat( ANGBAND_W_HELP, PATH_SEP );
    strcat( ANGBAND_W_HELP, "cmds_w.hlp" );
    ANGBAND_W_HELP = (char *)realloc(ANGBAND_W_HELP,
				       strlen( ANGBAND_W_HELP ) + 1 );

    ANGBAND_OWIZ_HELP = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_OWIZ_HELP, angband_path );
    strcat( ANGBAND_OWIZ_HELP, PATH_SEP );
    strcat( ANGBAND_OWIZ_HELP, "files" );
    strcat( ANGBAND_OWIZ_HELP, PATH_SEP );
    strcat( ANGBAND_OWIZ_HELP, "owizcmds.hlp" );
    ANGBAND_OWIZ_HELP = (char *)realloc(ANGBAND_OWIZ_HELP,
					strlen( ANGBAND_OWIZ_HELP ) + 1 );

    ANGBAND_WIZ = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_WIZ, angband_path );
    strcat( ANGBAND_WIZ, PATH_SEP );
    strcat( ANGBAND_WIZ, "files" );
    strcat( ANGBAND_WIZ, PATH_SEP );
    strcat( ANGBAND_WIZ, "wizards.txt" );
    ANGBAND_WIZ = (char *)realloc( ANGBAND_WIZ, strlen( ANGBAND_WIZ ) + 1 );

    ANGBAND_LOG = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_LOG, angband_path );
    strcat( ANGBAND_LOG, PATH_SEP );
    strcat( ANGBAND_LOG, "files" );
    strcat( ANGBAND_LOG, PATH_SEP );
    strcat( ANGBAND_LOG, "ANGBAND.log" );
    ANGBAND_LOG = (char *)realloc( ANGBAND_LOG, strlen( ANGBAND_LOG ) + 1 );

    ANGBAND_HOURS = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_HOURS, angband_path );
    strcat( ANGBAND_HOURS, PATH_SEP );
    strcat( ANGBAND_HOURS, "files" );
    strcat( ANGBAND_HOURS, PATH_SEP );
    strcat( ANGBAND_HOURS, "hours" );
    ANGBAND_HOURS = (char *)realloc( ANGBAND_HOURS, strlen( ANGBAND_HOURS ) + 1 );

    ANGBAND_LOAD = (char *)malloc( MAXPATHLEN );
    strcpy( ANGBAND_LOAD, angband_path );
    strcat( ANGBAND_LOAD, PATH_SEP );
    strcat( ANGBAND_LOAD, "files" );
    strcat( ANGBAND_LOAD, PATH_SEP );
    strcat( ANGBAND_LOAD, "loadcheck" );
    ANGBAND_LOAD = (char *)realloc( ANGBAND_LOAD, strlen( ANGBAND_LOAD ) + 1 );

    return;
}

#ifdef BOZO_MAXPATHLEN
#undef BOZO_MAXPATHLEN
#undef MAXPATHLEN
#endif /* BOZO_MAXPATHLEN */




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

