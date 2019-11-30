/*
 * rods.c : rod code 
 *
 * Copyright (c) 1989 Andrew Astrand 1991 
 *
 * Do what you like with it 
 *
 * I will sucker! ~Ludwig 
 */

#include "angband.h"




/* Rods for the slaughtering    			 */
void 
activate_rod()
{
    u32b              i;
    register int        l, ident;
    int                 item_val, j, k, chance, dir;
    register inven_type *i_ptr;
    register struct misc *m_ptr;

    free_turn_flag = TRUE;
    if (inven_ctr == 0)
	msg_print("But you are not carrying anything.");
    else if (!find_range(TV_ROD, TV_NEVER, &j, &k))
	msg_print("You are not carrying any rods.");
    else if (get_item(&item_val, "Activate which rod?", j, k, 0)) {
	i_ptr = &inventory[item_val];
	free_turn_flag = FALSE;
	ident = FALSE;
	m_ptr = &py.misc;
	chance = m_ptr->save + (stat_adj(A_INT) * 2) -
	    (int)((i_ptr->level > 70) ? 70 : i_ptr->level)
	    + (class_level_adj[m_ptr->pclass][CLA_DEVICE] * m_ptr->lev / 3);
	if (py.flags.confused > 0)
	    chance = chance / 2;
	if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1))
	    chance = USE_DEVICE;   /* Give everyone a slight chance */
	if (chance <= 0)
	    chance = 1;
	if (randint(chance) < USE_DEVICE)
	    msg_print("You failed to use the rod properly.");
	else if (i_ptr->timeout <= 0) {
	    i = i_ptr->flags;
	    k = char_row;
	    l = char_col;
	    switch (i) {
	      case RD_LT:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		msg_print("A line of blue shimmering light appears.");
		lite_line(dir, char_row, char_col);
		ident = TRUE;
		i_ptr->timeout = 9;
		break;
	      case RD_ILLUME:
		lite_area(k, l, damroll(2, 8), 2);
		ident = TRUE;
		i_ptr->timeout = 30;
		break;
	      case RD_AC_BLTS:	   /* Acid , New */
		if (!get_dir_c(NULL,&dir))
		    goto no_charge;
		if (randint(10)==1)
		    line_spell(GF_ACID,dir,k,l,damroll(6,8));
		else
		    fire_bolt(GF_ACID,dir,k,l,damroll(6,8));
		ident = TRUE;
		i_ptr->timeout = 12;
		break;
	      case RD_LT_BLTS:	   /* Lightning */
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		if (randint(12)==1)
		    line_spell(GF_ELEC, dir, k, l, damroll(3, 8));
		else
		    fire_bolt(GF_ELEC, dir, k, l, damroll(3, 8));
		ident = TRUE;
		i_ptr->timeout = 11;
		break;
	      case RD_FT_BLTS:	   /* Frost */
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		if (randint(10)==1)
		    line_spell(GF_COLD, dir, k, l, damroll(5, 8));
		else
		    fire_bolt(GF_COLD, dir, k, l, damroll(5, 8));
		ident = TRUE;
		i_ptr->timeout = 13;
		break;
	      case RD_FR_BLTS:	   /* Fire */
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		if (randint(8)==1)
		    line_spell(GF_FIRE, dir, k, l, damroll(8, 8));
		else
		    fire_bolt(GF_FIRE, dir, k, l, damroll(8, 8));
		ident = TRUE;
		i_ptr->timeout = 15;
		break;
	      case RD_POLY:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = poly_monster(dir, k, l);
		i_ptr->timeout = 25;
		break;
	      case RD_SLOW_MN:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = speed_monster(dir, k, l, -1);
		i_ptr->timeout = 20;
		break;
	      case RD_SLEE_MN:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = sleep_monster(dir, k, l);
		i_ptr->timeout = 18;
		break;
	      case RD_DRAIN:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = drain_life(dir, k, l, 75);
		i_ptr->timeout = 23;
		break;
	      case RD_TELE:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = teleport_monster(dir, k, l);
		i_ptr->timeout = 25;
		break;
	      case RD_DISARM:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = disarm_all(dir, k, l);
		i_ptr->timeout = 30;
		break;
	      case RD_LT_BALL:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		fire_ball(GF_ELEC, dir, k, l, 32, 2);
		ident = TRUE;
		i_ptr->timeout = 23;
		break;
	      case RD_CD_BALL:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		fire_ball(GF_COLD, dir, k, l, 48, 2);
		ident = TRUE;
		i_ptr->timeout = 25;
		break;
	      case RD_FR_BALL:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		fire_ball(GF_FIRE, dir, k, l, 72, 2);
		ident = TRUE;
		i_ptr->timeout = 30;
		break;
	      case RD_AC_BALL:
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		fire_ball(GF_ACID, dir, k, l, 60, 2);
		ident = TRUE;
		i_ptr->timeout = 27;
		break;
	      case RD_MAPPING:
		map_area();
		ident = TRUE;
		i_ptr->timeout = 99;
		break;
	      case RD_IDENT:
		ident_spell();
		ident = TRUE;
		i_ptr->timeout = 10;
		break;
	      case RD_CURE:
		if ((cure_blindness()) || (cure_poison()) ||
		    (cure_confusion()) || (py.flags.stun > 0) || (py.flags.cut > 0))
		    ident = TRUE;
		if (py.flags.stun > 0) {
		    if (py.flags.stun > 50) {
			py.misc.ptohit += 20;
			py.misc.ptodam += 20;
		    } else {
			py.misc.ptohit += 5;
			py.misc.ptodam += 5;
		    }
		    py.flags.stun = 0;
		    ident = TRUE;
		    msg_print("Your head stops stinging.");
		} else if (py.flags.cut > 0) {
		    py.flags.cut = 0;
		    ident = TRUE;
		    msg_print("You feel better.");
		}
		i_ptr->timeout = 888;
		break;
	      case RD_HEAL:
		ident = hp_player(500);
		if (py.flags.stun > 0) {
		    if (py.flags.stun > 50) {
			py.misc.ptohit += 20;
			py.misc.ptodam += 20;
		    } else {
			py.misc.ptohit += 5;
			py.misc.ptodam += 5;
		    }
		    py.flags.stun = 0;
		    ident = TRUE;
		    msg_print("Your head stops stinging.");
		}
		if (py.flags.cut > 0) {
		    py.flags.cut = 0;
		    ident = TRUE;
		    msg_print("You feel better.");
		}
		i_ptr->timeout = 888;
		break;
	      case RD_RECALL:
		if (py.flags.word_recall == 0) {
		    py.flags.word_recall = 15 + randint(20);
		    msg_print("The air about you becomes charged...");
		} else {
		    py.flags.word_recall = 0;
		    msg_print("A tension leaves the air around you...");
		}
		ident = TRUE;
		i_ptr->timeout = 60;
		break;
	      case RD_PROBE:
		probing();
		ident = TRUE;
		i_ptr->timeout = 50;
		break;
	      case RD_DETECT:
		detection();
		ident = TRUE;
		i_ptr->timeout = 99;
		break;
	      case RD_RESTORE:
		if (restore_level() || res_stat(A_STR) || res_stat(A_INT) ||
		    res_stat(A_WIS) || res_stat(A_DEX) || res_stat(A_CON) ||
		    res_stat(A_CHR))
		    ident = TRUE;
		i_ptr->timeout = 999;
		break;
	      case RD_SPEED:
		if (py.flags.fast == 0)
		    ident = TRUE;
		py.flags.fast += randint(30) + 15;
		i_ptr->timeout = 99;
		break;
	      case RD_TRAP_LOC:
		if (detect_trap())
		    ident = TRUE;
		i_ptr->timeout = 99;	/* fairly long timeout because rod so
					 * low lv -CFT */
		break;
#if 0
	      case RD_MK_WALL:	   /* JLS */
		if (!get_dir_c(NULL, &dir))
		    goto no_charge;
		ident = build_wall(dir, k, l);
		i_ptr->timeout = 999;	/* don't want people to abuse this
					 * -JLS */
		break;
#endif
	      default:
		msg_print("Internal error in rods() ");
		break;
	    }
	    if (ident) {
		if (!known1_p(i_ptr)) {
		    m_ptr = &py.misc;
		/* round half-way case up */
		    m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) /
			m_ptr->lev;
		    prt_experience();

		    identify(&item_val);
		    i_ptr = &inventory[item_val];
		}
	    } else if (!known1_p(i_ptr)) {
		sample(i_ptr);
	    }
    no_charge:;
	} else {
	    msg_print("The rod is currently exhausted.");
	}
    }
}
