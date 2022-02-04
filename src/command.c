/* File: command.c */ 

/* Purpose: process player commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



/*
 * Give the player some help (files may be out of date)
 */
static void do_cmd_help(cptr fname)
{	    
    /* Help is always free */
    free_turn_flag = TRUE;

    /* Default help files */
    if (!fname) fname = rogue_like_commands ? ANGBAND_R_HELP : ANGBAND_O_HELP;

    /* Dump the help file file */
    helpfile(fname);
}




/*
 * Examine a Book					-RAK-	
 */
static void do_cmd_browse(void)
{
    u32b               j1, j2, tmp;
    int                  i, k, item_val, flag;
    int                  spell_index[63];
    register inven_type *i_ptr;
    register spell_type *s_ptr;
    int                  first_spell;

    if (!find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k)) {
	msg_print("You are not carrying any books.");
	return;
    }

    if (p_ptr->blind > 0) {
	msg_print("You can't see to read your spell book!");
	return;
    }

    if (no_lite()) {
	msg_print("You have no light to read by.");
	return;
    }

    if (p_ptr->confused > 0) {
	msg_print("You are too confused.");
	return;
    }

    /* Get a book or stop checking */
    if (!get_item(&item_val, "Which Book?", i, k, 0)) return;

    flag = FALSE;

    i_ptr = &inventory[item_val];

    /* Check the language */
    if (class[p_ptr->pclass].spell == MAGE) {
	if (i_ptr->tval == TV_MAGIC_BOOK) flag = TRUE;
    }
    else if (class[p_ptr->pclass].spell == PRIEST) {
	if (i_ptr->tval == TV_PRAYER_BOOK) flag = TRUE;
    }

    if (!flag) {
	msg_print("You do not understand the language.");
	return;
    }

    i = 0;

    j1 = i_ptr->flags1;

    /* check which spell was first */
    tmp = j1;
    first_spell = bit_pos(&tmp);

    while (j1) {
	k = bit_pos(&j1);
	s_ptr = &magic_spell[p_ptr->pclass - 1][k];
	if (s_ptr->slevel < 99) {
	    spell_index[i] = k;
	    i++;
	}
    }

    j2 = i_ptr->flags2;

    /* if none from other set of flags */
    if (first_spell == -1) {
	tmp = j2;
	first_spell = 32 + bit_pos(&tmp);
    }

    while (j2) {
	k = bit_pos(&j2);
	s_ptr = &magic_spell[p_ptr->pclass - 1][k + 32];
	if (s_ptr->slevel < 99) {
	    spell_index[i] = (k + 32);
	    i++;
	}
    }

    /* Display the spells */
    save_screen();
    print_spells(spell_index, i, TRUE, first_spell);
    pause_line(0);
    restore_screen();
}




/*
 * Go up one level					-RAK-	
 */
static void do_cmd_go_up()
{
    cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = &cave[char_row][char_col];
    i_ptr = &i_list[c_ptr->i_idx];

    /* Verify stairs */
    if (i_ptr->tval != TV_UP_STAIR) {
	msg_print("I see no up staircase here.");
	free_turn_flag = TRUE;
	return;
    }

    if (dun_level == Q_PLANE) {
	dun_level = 0;
	new_level_flag = TRUE;
	msg_print("You enter an inter-dimensional staircase. ");
	return;
    }

    /* Success */    
    msg_print("You enter a maze of up staircases. ");

    /* Go up the stairs */
    dun_level--;
    new_level_flag = TRUE;

    /* Create a way back */
    if (dun_level > 0) create_down_stair = TRUE;
}


/*
 * Go down one level
 */
static void do_cmd_go_down()
{
    cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = &cave[char_row][char_col];
    i_ptr = &i_list[c_ptr->i_idx];

    if (i_ptr->tval != TV_DOWN_STAIR) {
	msg_print("I see no down staircase here.");
	free_turn_flag = TRUE;
	return;
    }

    if (dun_level == Q_PLANE) {
	dun_level = 0;
	new_level_flag = TRUE;
	msg_print("You enter an inter-dimensional staircase. ");
	return;
    }

    /* Success */
    msg_print("You enter a maze of down staircases. ");

    /* Go down */
    dun_level++;
    new_level_flag = TRUE;

    /* Create a way back */
    create_up_stair = TRUE;
}


/*
 * Hack -- commit suicide
 */
static void do_cmd_suicide(void)
{
    free_turn_flag = TRUE;

    flush();

    if (total_winner) {
	if (!get_check("Do you want to retire?")) return;
    }
    else {
	if (!get_check("Do you really want to quit?")) return;
    }

    new_level_flag = TRUE;
    death = TRUE;
    (void)strcpy(died_from, "Quitting");
}


/*
 * Hack -- redraw the screen
 */
static void do_cmd_redraw(void)
{
    free_turn_flag = TRUE;

    if (p_ptr->image > 0) {
	msg_print("You cannot be sure what is real and what is not!");
    }

    else {
    /* Redraw the screen */
    draw_cave();
    update_monsters();	  /* draw monsters */
    prt_equippy_chars();  /* redraw equippy chars */
    }
}


/*
 * Hack -- change name
 */
static void do_cmd_change_name(void)
{
    free_turn_flag = TRUE;

    save_screen();
    change_name();
    restore_screen();
}


/*
 * Hack -- toggle search mode
 */
static void do_cmd_toggle_search(void)
{
    free_turn_flag = TRUE;

    if (p_ptr->status & PY_SEARCH) {
	search_off();
    }
    else {
	search_on();
    }
}


/*
 * Hack -- pick up objects
 */
static void do_cmd_pick_up(void)
{
    if (prompt_carry_flag) {
        if (cave[char_row][char_col].i_idx != 0)
	    carry(char_row, char_col, TRUE);
    } else
        free_turn_flag = TRUE;
}


/*
 * Refill the players lamp	-RAK-
 */
static void do_cmd_refill_lamp()
{
    int                  i, j;
    register int         k;
    register inven_type *i_ptr;

    free_turn_flag = TRUE;
    k = inventory[INVEN_LIGHT].sval;
    if (k != 0)
	msg_print("But you are not using a lamp.");
    else if (!find_range(TV_FLASK, TV_NEVER, &i, &j)) {
	msg_print("You have no oil.");
    }

    else {

	free_turn_flag = FALSE;
	i_ptr = &inventory[INVEN_LIGHT];

	i_ptr->pval += inventory[i].pval;

	if (i_ptr->pval > FUEL_LAMP) {
	    i_ptr->pval = FUEL_LAMP;
	    msg_print("Your lamp overflows, spilling oil on the ground.");
	    msg_print("Your lamp is full.");
	}
	else if (i_ptr->pval > FUEL_LAMP / 2) {
	    msg_print("Your lamp is more than half full.");
	}
	else if (i_ptr->pval == FUEL_LAMP / 2) {
	    msg_print("Your lamp is half full.");
	}
	else {
	    msg_print("Your lamp is less than half full.");
	}

	inven_item_describe(i);
	inven_destroy(i);
    }
}



/*
 * Support code for the "CTRL('P')" recall command
 */

static void do_cmd_messages(void)
{
    int i, j;

    /* Free move */
    free_turn_flag = TRUE;

	if (command_rep > 0) {
	    i = command_rep;
	    if (i > MAX_SAVE_MSG)
		i = MAX_SAVE_MSG;
	    command_rep = 0;
	} else if (last_command != 16)
	    i = 1;
	else
	    i = MAX_SAVE_MSG;

	j = last_msg;
	if (i > 1) {

    /* Save the screen */
	    save_screen();

	    x = i;

	    while (i > 0) {

		i--;
		prt(old_msg[j], i, 0);
		if (j == 0)
		    j = MAX_SAVE_MSG - 1;
		else
		    j--;
	    }

	    erase_line(x, 0);
	    pause_line(x);
	    restore_screen();

	} else {
	/* Distinguish real and recovered messages with a '>'. -CJS- */
	    put_str(">", 0, 0);
	    prt(old_msg[j], 0, 1);
	}
}


/*
 * Target command
 */
static void do_cmd_target()
{
    /* Free move */
    free_turn_flag = TRUE;

#ifdef TARGET

    /* Be sure we can see */
    if (p_ptr->blind > 0) {
	msg_print("You can't see anything to target!");
    }
    else if (!target_set()) {
	msg_print("Aborting Target.");
    }
    else {
	msg_print("Target selected.");
    }

#endif

}





/*
 * A simple structure to hold some options
 */
typedef struct _opt_desc {
    cptr	o_prompt;
    int         *o_var;
} opt_desc;


static opt_desc options[] = {

    { "Running: cut known corners",	 	&find_cut },
    { "Running: examine potential corners",	&find_examine },
    { "Running: print self during run", 	&find_prself },
    { "Running: stop when map sector changes",  &find_bound},
    { "Running: run through open doors", 	&find_ignore_doors},
    { "(g)et-key to pickup objects", 		&prompt_carry_flag},
    { "Prompt before pickup", 			&carry_query_flag},
    { "Rogue like commands", 			&rogue_like_commands},
    { "Show weights in inventory", 		&show_inven_weight},
    { "Show weights in equipment list",		&show_equip_weight},
    { "Highlight and notice mineral seams", 	&notice_seams},
    { "Disable haggling in stores",		&no_haggle_flag},
    { "Plain object descriptions",		&plain_descriptions},
    { "Quick messages",		                &quick_messages},
    { "Equippy chars",		                &equippy_chars},
    { "Low hitpoint warning",			&hitpoint_warn},
    { "Delay speed", 				&delay_spd},
    { (char *)0, 				(int *)0}
};


/*
 * Set or unset various boolean options.
 */
void do_cmd_options()
{
    register int i, max, ch;
    vtype        string;

    prt("  ESC when finished, y/n or 0-9 to set options, <return> or - to move cursor",
    0, 0);

    for (max = 0; options[max].o_prompt != 0; max++) {
	if (options[max].o_var == &hitpoint_warn)
	    (void)sprintf(string, "%-38s: %d0%% ", options[max].o_prompt,
			  *options[max].o_var);
	else if (options[max].o_var == &delay_spd)
	    (void)sprintf(string, "%-38s: %d ", options[max].o_prompt,
			  *options[max].o_var);
	else
	    (void)sprintf(string, "%-38s: %s ", options[max].o_prompt,
			  (*options[max].o_var ? "yes" : "no "));
	prt(string, max + 1, 0);
    }
    erase_line(max + 1, 0);

    /* Start at the first option */
    i = 0;

    /* Interact with the player */
    for (;;) {
	move_cursor(i + 1, 40);
	ch = inkey();
	switch (ch) {
	  case ESCAPE:	  
	    draw_cave();
	    update_monsters();	 /* draw monsters */
	    prt_equippy_chars(); /* redraw equippy chars */
	    return;
	  case '-':
	    if (i > 0) i--;
	    else i = max - 1;
	    break;
	  case ' ':
	  case '\n':
	  case '\r':
	    if (i + 1 < max) i++;
	    else i = 0;
	    break;
	  case 'y':
	  case 'Y':
	    if ((options[i].o_var == &hitpoint_warn) ||
		(options[i].o_var == &delay_spd))
		bell();
	    else {
		put_str("yes ", i + 1, 40);
	    *options[i].o_var = TRUE;
	    if (i + 1 < max) i++;
		else i = 0;
	    }
	    break;
	  case 'n':
	  case 'N':
	    if (options[i].o_var == &delay_spd)
		bell();
	    else if (options[i].o_var == &hitpoint_warn) {
		put_str("00%", i + 1, 40);
		*options[i].o_var = 0;
	    } else {
		put_str("no  ", i + 1, 40);
	    *options[i].o_var = FALSE;
	    if (i + 1 < max) i++;
		else i = 0;
	    }
	    break;
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
	  case '0':
	    if ((options[i].o_var != &delay_spd) &&
		(options[i].o_var != &hitpoint_warn))
		bell();
	    else {
		ch = ch - '0';
		*options[i].o_var = ch;
		if (options[i].o_var == &hitpoint_warn)
		    sprintf(string, "%d0%%  ", ch);
		else
		    sprintf(string, "%d   ", ch);
		put_str(string, i + 1, 40);
	    if (i + 1 < max) i++;
		else i = 0;
	    }
	    break;
	  default:
	    bell();
	    break;
	}
    }
}


/*
 * Check whether this command will accept a count.     -CJS- 
 */
static int valid_countcommand(char c)
{
    /* Examine the command */
    switch (c) {

	case 'Q':
	case CTRL('W'):
	case CTRL('X'):
	case '=':
	case '{':
	case '/':
	case '<':
	case '>':
	case '?':
	case 'A':
	case 'C':
	case 'E':
	case 'F':
	case 'G':
	case '#':
	case 'z':
	case 'P':
	case 'c':
	case 'd':
	case 'e':
	case 't':
	case 'i':
	case 'x':
	case 'm':
	case 'p':
	case 'q':
	case 'r':
	case 'T':
	case 'Z':
	case 'V':
	case 'w':
	case 'W':
	case 'X':
	case CTRL('A'):
	case '\\':
	case CTRL('I'):
	case CTRL('^'):
	case '$':
	case '*':
	case ':':
	case CTRL('T'):
	case CTRL('E'):
	case CTRL('F'):
	case CTRL('Z'):
	case CTRL('S'):
	case CTRL('Q'):
	case CTRL('R'):
	    return FALSE;
	case CTRL('P'):
	case ESCAPE:
	case ' ':
	case '-':
	case 'b':
	case 'f':
	case 'j':
	case 'n':
	case 'h':
	case 'l':
	case 'y':
	case 'k':
	case 'u':
	case '.':
	case 'B':
	case 'J':
	case 'N':
	case 'H':
	case 'L':
	case 'Y':
	case 'K':
	case 'U':
	case 'D':
	case 'R':
	case CTRL('Y'):
	case CTRL('K'):
	case CTRL('U'):
	case CTRL('L'):
	case CTRL('N'):
	case CTRL('J'):
	case CTRL('B'):
	case CTRL('H'):
	case 'S':
	case 'o':
	case 's':
	case CTRL('D'):
	case CTRL('G'):
	case '+':
	    return TRUE;
	default:
	return FALSE;
    }
}


