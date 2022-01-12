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
	    creatures(FALSE);	 /* draw monsters */
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



