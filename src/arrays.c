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

    strcpy(tail, "k_list.txt");
    ANGBAND_K_LIST = string_make(path);
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

    /* Allocate and Wipe the array of object "kind info" */
    C_MAKE(k_list, MAX_OBJECTS, inven_kind);

    /* Initialize k_list from a file */
    init_k_list();
}



