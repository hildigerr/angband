/* File: save.c */

/* Purpose: save and restore games and monster memory info */

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
 * And then was re-written again (for 2.7.0) cause it sucked.  -BEN-
 */


#ifndef USG
# include <sys/file.h>
# include <sys/param.h>
#endif

#ifdef __MINT__
# include <stat.h>		/* chmod() */
#endif

#if !defined(ATARIST_MWC)
#ifdef MAC
#else
long time();

#endif
#else
char *malloc();

#endif

#if !defined(SET_UID)
# if defined(__EMX__)
#  include <sys/stat.h>
# else
#  include <stat.h>
# endif
#endif

#ifdef linux
# include <sys/stat.h>
#endif



/*
 * these are used for the save file, to avoid having to pass them to every
 * procedure 
 */

static FILE	*fff;		/* Current save "file" */

static byte	xor_byte;	/* Simple encryption */

static byte	version_maj;	/* Major version */
static byte	version_min;	/* Minor version */
static byte	patch_level;	/* Patch level */

static int	from_savefile;	/* can overwrite old savefile when save */

static bool	say = FALSE;	/* Show "extra" messages */


/*
 * This function determines if the version of the savefile
 * currently being read is older than version "x.y.z".
 */
static bool older_than(byte x, byte y, byte z)
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
 * The basic I/O functions for savefiles
 * All information is written/read one byte at a time
 */

static void sf_put(byte v)
{
    /* Encode the value, write a character */
    xor_byte ^= v;
    (void)putc((int)xor_byte, fff);
}

static byte sf_get(void)
{
    register byte c, v;

    /* Get a character, decode the value */
    c = getc(fff) & 0xFF;
    v = c ^ xor_byte;
    xor_byte = c;

#ifdef SAVEFILE_VOMIT
    /* Hack -- debugging */
    if (1) {
	static int y = 15, x = 0;
	char buf[3];
	sprintf(buf, "%02x", v);
	prt(buf, y, x*3);
	x++;
	if (x >= 25) {
	    x = 0;
	    y++;
	    if (y >= 24) y = 15;
	}
    }
#endif

    /* Return the value */    
    return (v);
}




/*
 * Write/Read various "byte sized" objects
 */

static void wr_byte(byte v)
{
    sf_put(v);
}

static void rd_byte(byte *ip)
{
    *ip = sf_get();
}


static void wr_char(char v)
{
    wr_byte((byte)v);
}

static void rd_char(char *ip)
{
    rd_byte((byte*)ip);
}


/*
 * Write/Read various "short" objects
 */

static void wr_u16b(u16b v)
{
    sf_put(v & 0xFF);
    sf_put((v >> 8) & 0xFF);
}

static void rd_u16b(u16b *ip)
{
    (*ip) = sf_get();
    (*ip) |= ((u16b)(sf_get()) << 8);
}


static void wr_s16b(s16b v)
{
    wr_u16b((u16b)v);
}

static void rd_s16b(s16b *ip)
{
    rd_u16b((u16b*)ip);
}



/*
 * Write/Read various "long" objects
 */

static void wr_u32b(u32b v)
{
    sf_put(v & 0xFF);
    sf_put((v >> 8) & 0xFF);
    sf_put((v >> 16) & 0xFF);
    sf_put((v >> 24) & 0xFF);
}

static void rd_u32b(u32b *ip)
{
    (*ip) = sf_get();
    (*ip) |= ((u32b)(sf_get()) << 8);
    (*ip) |= ((u32b)(sf_get()) << 16);
    (*ip) |= ((u32b)(sf_get()) << 24);
}


static void wr_s32b(s32b v)
{
    wr_u32b((u32b)v);
}

static void rd_s32b(s32b *ip)
{
    rd_u32b((u32b*)ip);
}




/*
 * Strings
 */

static void wr_string(cptr str)
{
    while (*str) {
	wr_byte(*str);
	str++;
    }
    wr_byte(*str);
}

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
 * Read an item (2.7.0 or later)
 */
static void rd_item(inven_type *i_ptr)
{
    /* Get the kind */
    rd_u16b(&i_ptr->k_idx);

    rd_byte(&i_ptr->tval);
    rd_byte(&i_ptr->sval);
    rd_s16b(&i_ptr->pval);

    rd_byte(&i_ptr->name2);
    rd_byte(&i_ptr->ident);
    rd_byte(&i_ptr->number);
    rd_u16b(&i_ptr->weight);
    rd_u16b(&i_ptr->timeout);

    rd_s16b(&i_ptr->tohit);
    rd_s16b(&i_ptr->todam);
    rd_s16b(&i_ptr->toac);
    rd_s16b(&i_ptr->ac);
    rd_byte(&i_ptr->damage[0]);
    rd_byte(&i_ptr->damage[1]);
    rd_byte(&i_ptr->level);
    rd_byte(&i_ptr->tchar);

    rd_s32b(&i_ptr->cost);

    rd_u32b(&i_ptr->flags1);
    rd_u32b(&i_ptr->flags2);

    rd_string(i_ptr->inscrip);
}


static void wr_item(inven_type *i_ptr)
{
    wr_u16b(i_ptr->k_idx);

    wr_byte(i_ptr->tval);
    wr_byte(i_ptr->sval);
    wr_s16b(i_ptr->pval);

    wr_byte(i_ptr->name2);
    wr_byte(i_ptr->ident);
    wr_byte(i_ptr->number);
    wr_u16b(i_ptr->weight);
    wr_u16b(i_ptr->timeout);

    wr_s16b(i_ptr->tohit);
    wr_s16b(i_ptr->todam);
    wr_s16b(i_ptr->toac);
    wr_s16b(i_ptr->ac);
    wr_byte(i_ptr->damage[0]);
    wr_byte(i_ptr->damage[1]);
    wr_byte(i_ptr->level);
    wr_byte(i_ptr->tchar);

    wr_s32b(i_ptr->cost);

    wr_u32b(i_ptr->flags1);
    wr_u32b(i_ptr->flags2);


    wr_string(i_ptr->inscrip);
}



/*
 * Read and Write monsters
 */


static void rd_monster(monster_type *m_ptr)
{
    rd_u16b(&m_ptr->r_idx);
    rd_byte(&m_ptr->fy);
    rd_byte(&m_ptr->fx);
    rd_s16b(&m_ptr->hp);
    rd_s16b(&m_ptr->maxhp);
    rd_s16b(&m_ptr->csleep);
    rd_s16b(&m_ptr->mspeed);
    rd_byte(&m_ptr->stunned);
    rd_byte(&m_ptr->confused);
    rd_byte(&m_ptr->monfear);
    rd_byte(&m_ptr->cdis);
    rd_byte(&m_ptr->ml);
}

static void wr_monster(monster_type *m_ptr)
{
    wr_u16b(m_ptr->r_idx);
    wr_byte(m_ptr->fy);
    wr_byte(m_ptr->fx);
    wr_s16b(m_ptr->hp);
    wr_s16b(m_ptr->maxhp);
    wr_s16b(m_ptr->csleep);
    wr_s16b(m_ptr->mspeed);
    wr_byte(m_ptr->stunned);
    wr_byte(m_ptr->confused);
    wr_byte(m_ptr->monfear);
    wr_byte(m_ptr->cdis);
    wr_byte(m_ptr->ml);
}





/*
 * Write/Read the monster lore
 */

static void rd_lore(monster_lore *l_ptr)
{
	int i;

	rd_u16b(&l_ptr->r_kills);
	rd_u16b(&l_ptr->r_deaths);

	rd_u32b(&l_ptr->r_spells1);
	rd_u32b(&l_ptr->r_spells2);
	rd_u32b(&l_ptr->r_spells3);
	rd_u32b(&l_ptr->r_cflags1);
	rd_u32b(&l_ptr->r_cflags2);

	for (i = 0; i < MAX_MON_NATTACK; i++)
	rd_byte(&l_ptr->r_attacks[i]);

	rd_byte(&l_ptr->r_wake);
	rd_byte(&l_ptr->r_ignore);
}

static void wr_lore(monster_lore *l_ptr)
{
	int i;

    /* Write the info */
    wr_u16b(l_ptr->r_kills);
    wr_u16b(l_ptr->r_deaths);

    wr_u32b(l_ptr->r_spells1);
    wr_u32b(l_ptr->r_spells2);
    wr_u32b(l_ptr->r_spells3);
    wr_u32b(l_ptr->r_cflags1);
    wr_u32b(l_ptr->r_cflags2);

    for (i = 0; i < MAX_MON_NATTACK; i++)
    wr_byte(l_ptr->r_attacks[i]);

    wr_byte(l_ptr->r_wake);
    wr_byte(l_ptr->r_ignore);
}



/*
 * Write/Read a store
 */
static void wr_store(store_type *st_ptr)
{
    int j;

    wr_s32b(st_ptr->store_open);
    wr_s16b(st_ptr->insult_cur);
    wr_byte(st_ptr->owner);
    wr_byte(st_ptr->store_ctr);
    wr_u16b(st_ptr->good_buy);
    wr_u16b(st_ptr->bad_buy);

    /* Write the items */
    for (j = 0; j < st_ptr->store_ctr; j++) {
	wr_s32b(st_ptr->store_inven[j].scost);
	wr_item(&st_ptr->store_inven[j].sitem);
    }
}


static errr rd_store(store_type *st_ptr)
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
	rd_s32b(&st_ptr->store_inven[j].scost);
	rd_item(&st_ptr->store_inven[j].sitem);
    }

    /* Success */
    return (0);
}





/*
 * Read options
 */
static void rd_options(void)
{
    u32b l;


    rd_u32b(&l);
	if (!older_than(2,6,0)) {
	  rd_u32b(&l);
	  rd_u32b(&l);
	  rd_u32b(&l);
	}

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


/*
 * Write the options
 */
static void wr_options(void)
{
    u32b l;

    l = 0L;

    if (rogue_like_commands)	l |= 32;
    if (prompt_carry_flag)	l |= 16;
    if (!carry_query_flag)	l |= 0x400L;
    if (quick_messages)		l |= 0x40000000L;
    
    if (notice_seams)		l |= 128;
    if (equippy_chars)		l |= 0x20000000L;

    if (find_cut)		l |= 1;
    if (find_examine)		l |= 2;
    if (find_prself)		l |= 4;
    if (find_bound)		l |= 8;
    if (find_ignore_doors)	l |= 0x100L;

    if (no_haggle_flag)			l |= 0x200L;

    if (show_inven_weight)		l |= 64;
    if (show_equip_weight)		l |= 0x00800000L;
    if (plain_descriptions)		l |= 0x00400000L;

    if (unfelt) l |= 0x0001000L;

    /* Write "delay_spd" */
    if (delay_spd > 10) delay_spd = 10;
    if (delay_spd < 0) delay_spd = 0;
    l |= ((delay_spd & 0xf) << 13);

    /* Write "hitpoint_warn" */
    l |= ((hitpoint_warn & 0xf) << 17);
}





static void rd_ghost()
{
    monster_race *r_ptr = &r_list[MAX_R_IDX-1];

    int i;
    byte tmp8u;
    u16b tmp16u;
    u32b tmp32u;


    /* A buffer for the ghost name */
    char gname[128];

    /* Allocate storage for name */
	r_ptr->name = (char*)malloc(101);
	C_WIPE(r_ptr->name, 101, char);

    /* Hack -- read the name as bytes */
    for (i = 0; i < 100; i++) rd_char(&gname[i]);
    strcpy(r_ptr->name, gname);

    rd_u16b(&r_ptr->level);

    rd_byte(&r_ptr->r_char);

    rd_byte(&r_ptr->hd[0]);
    rd_byte(&r_ptr->hd[1]);
    rd_u16b(&r_ptr->ac);
    rd_u16b(&r_ptr->sleep);
    rd_byte(&r_ptr->aaf);
    rd_byte(&r_ptr->speed);

    rd_u32b(&r_ptr->mexp);

    /* Hack -- read the attacks */
    for (i = 0; i < 4; i++) {
	rd_u16b(&r_ptr->damage[i]);

    rd_u32b(&r_ptr->cflags1);
    rd_u32b(&r_ptr->cflags2);

    rd_u32b(&r_ptr->spells1);
    }

}

static void wr_ghost()
{
    monster_race *r_ptr = &r_list[MAX_R_IDX-1];
	u16b temp;

     if (!r_ptr->name) {
	 r_ptr->name = (char*)malloc(101);
	 C_WIPE(r_ptr->name, 101, char);
     }

    for (i = 0; i < 100; i++)
    wr_byte(r_ptr->name[i]);

    wr_u16b(r_ptr->level);

    wr_byte(r_ptr->r_char);

    wr_byte(r_ptr->hd[0]);
    wr_byte(r_ptr->hd[1]);
    wr_s16b(r_ptr->ac);
    wr_u16b(r_ptr->sleep);
    wr_byte(r_ptr->aaf);
    wr_byte(r_ptr->speed);

    wr_u32b(r_ptr->mexp);

    wr_u16b(r_ptr->damage[0]);
    wr_u16b(r_ptr->damage[1]);
    wr_u16b(r_ptr->damage[2]);
    wr_u16b(r_ptr->damage[3]);

    wr_u32b(r_ptr->cflags1);
    wr_u32b(r_ptr->cflags2);

    wr_u32b(r_ptr->spells1);
}




/*
 * Read/Write the "extra" information
 */

static void rd_extra()
{
    int i;
    byte tmp8u;

    rd_string(p_ptr->name);

    rd_string(died_from);

    for (i = 0; i < 4; i++) {
	rd_string(p_ptr->history[i]);
    }

    /* Class/Race/Gender/Spells */
    rd_byte(&p_ptr->prace);
    rd_byte(&p_ptr->pclass);
    rd_byte(&p_ptr->male);
    rd_byte(&p_ptr->new_spells);

    /* Special Race/Class info */
    rd_byte(&p_ptr->hitdie);
    rd_byte(&p_ptr->expfact);

    /* Age/Height/Weight */
    rd_u16b(&p_ptr->age);
    rd_u16b(&p_ptr->ht);
    rd_u16b(&p_ptr->wt);

    /* Read the stats, Keep it simple */    
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->max_stat[i]);
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->cur_stat[i]);
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->mod_stat[i]);
    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->use_stat[i]);

    rd_s32b(&p_ptr->au);

    rd_s32b(&p_ptr->max_exp);
    rd_s32b(&p_ptr->exp);
    rd_u16b(&p_ptr->exp_frac);

    rd_u16b(&p_ptr->lev);

    rd_s16b(&p_ptr->mhp);
    rd_s16b(&p_ptr->chp);
    rd_u16b(&p_ptr->chp_frac);

    rd_s16b(&p_ptr->mana);
    rd_s16b(&p_ptr->cmana);
    rd_u16b(&p_ptr->cmana_frac);

    rd_u16b(&p_ptr->max_dlv);

    /* More info */
    rd_s16b(&p_ptr->srh);
    rd_s16b(&p_ptr->fos);
    rd_s16b(&p_ptr->disarm);
    rd_s16b(&p_ptr->save);
    rd_s16b(&p_ptr->sc);
    rd_s16b(&p_ptr->stl);
    rd_s16b(&p_ptr->bth);
    rd_s16b(&p_ptr->bthb);

    rd_s16b(&p_ptr->ptohit);
    rd_s16b(&p_ptr->ptodam);
    rd_s16b(&p_ptr->pac);
    rd_s16b(&p_ptr->ptoac);

    rd_s16b(&p_ptr->dis_th);
    rd_s16b(&p_ptr->dis_td);
    rd_s16b(&p_ptr->dis_ac);
    rd_s16b(&p_ptr->dis_tac);


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
    rd_s16b(&p_ptr->detect_inv);
    rd_s16b(&p_ptr->word_recall);
    rd_s16b(&p_ptr->see_infra);
    rd_s16b(&p_ptr->tim_infra);
    rd_s16b(&p_ptr->oppose_fire);
    rd_s16b(&p_ptr->oppose_cold);
    rd_s16b(&p_ptr->oppose_acid);
    rd_s16b(&p_ptr->oppose_elec);
    rd_s16b(&p_ptr->oppose_pois);
    rd_byte(&p_ptr->immune_acid);
    rd_byte(&p_ptr->immune_elec);
    rd_byte(&p_ptr->immune_fire);
    rd_byte(&p_ptr->immune_cold);
    rd_byte(&p_ptr->immune_pois);
    rd_byte(&p_ptr->resist_acid);
    rd_byte(&p_ptr->resist_elec);
    rd_byte(&p_ptr->resist_fire);
    rd_byte(&p_ptr->resist_cold);
    rd_byte(&p_ptr->resist_pois);
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
    rd_byte(&p_ptr->resist_fear);
    rd_byte(&p_ptr->see_inv);
    rd_byte(&p_ptr->teleport);
    rd_byte(&p_ptr->free_act);
    rd_byte(&p_ptr->slow_digest);
    rd_byte(&p_ptr->aggravate);
    rd_byte(&p_ptr->regenerate);
    rd_byte(&p_ptr->ffall);
    rd_byte(&p_ptr->sustain_str);
    rd_byte(&p_ptr->sustain_int);
    rd_byte(&p_ptr->sustain_wis);
    rd_byte(&p_ptr->sustain_con);
    rd_byte(&p_ptr->sustain_dex);
    rd_byte(&p_ptr->sustain_chr);
    rd_byte(&p_ptr->confuse_monster);
    rd_byte(&p_ptr->hold_life);
    rd_byte(&p_ptr->telepathy);
    rd_byte(&p_ptr->light);

    rd_u16b(&missile_ctr);


    /* Hack -- the two "special seeds" */            
    rd_u32b(&randes_seed);
    rd_u32b(&town_seed);


    /* Special stuff */
    rd_u16b(panic_save);
    rd_u16b(total_winner);
    rd_u16b(noscore);


    /* Important -- Read "death" */
    rd_byte(&tmp8u);
    death = tmp8u;

    /* Read "feeling" */
    rd_byte(&tmp8u);
    feeling = tmp8u;

    /* Turn of last "feeling" */
    rd_u32b(&old_turn);

    /* Current turn */
    rd_u32b(&turn);
}

static void wr_extra()
{
    int i;

    wr_string(p_ptr->name);

    wr_string(died_from);

    for (i = 0; i < 4; i++) {
	wr_string(p_ptr->history[i]);
    }

    /* Race/Class/Gender/Spells */
    wr_byte(p_ptr->prace);
    wr_byte(p_ptr->pclass);
    wr_byte(p_ptr->male);
    wr_byte(p_ptr->new_spells);

    wr_byte(p_ptr->hitdie);
    wr_byte(p_ptr->expfact);

    wr_u16b(p_ptr->age);
    wr_u16b(p_ptr->ht);
    wr_u16b(p_ptr->wt);

    /* Dump the stats */
    for (i = 0; i < 6; ++i) wr_s16b(p_ptr->max_stat[i]);
    for (i = 0; i < 6; ++i) wr_s16b(p_ptr->cur_stat[i]);
    for (i = 0; i < 6; ++i) wr_s16b(p_ptr->mod_stat[i]);
    for (i = 0; i < 6; ++i) wr_s16b(p_ptr->use_stat[i]);

    wr_s32b(p_ptr->au);

    wr_s32b(p_ptr->max_exp);
    wr_s32b(p_ptr->exp);
    wr_u16b(p_ptr->exp_frac);
    wr_u16b(p_ptr->lev);

    wr_s16b(p_ptr->mhp);
    wr_s16b(p_ptr->chp);
    wr_u16b(p_ptr->chp_frac);

    wr_s16b(p_ptr->mana);
    wr_s16b(p_ptr->cmana);
    wr_u16b(p_ptr->cmana_frac);

    /* Max Player and Dungeon Levels */
    wr_u16b(p_ptr->max_dlv);

    /* More info */
    wr_s16b(p_ptr->srh);
    wr_s16b(p_ptr->fos);
    wr_s16b(p_ptr->disarm);
    wr_s16b(p_ptr->save);
    wr_s16b(p_ptr->sc);
    wr_s16b(p_ptr->stl);
    wr_s16b(p_ptr->bth);
    wr_s16b(p_ptr->bthb);
    
    wr_s16b(p_ptr->ptohit);
    wr_s16b(p_ptr->ptodam);
    wr_s16b(p_ptr->pac);
    wr_s16b(p_ptr->ptoac);

    wr_s16b(p_ptr->dis_th);
    wr_s16b(p_ptr->dis_td);
    wr_s16b(p_ptr->dis_ac);
    wr_s16b(p_ptr->dis_tac);

    wr_u32b(p_ptr->status);
    wr_s16b(p_ptr->rest);
    wr_s16b(p_ptr->blind);
    wr_s16b(p_ptr->paralysis);
    wr_s16b(p_ptr->confused);
    wr_s16b(p_ptr->food);
    wr_s16b(p_ptr->food_digested);
    wr_s16b(p_ptr->protection);
    wr_s16b(p_ptr->speed);
    wr_s16b(p_ptr->fast);
    wr_s16b(p_ptr->slow);
    wr_s16b(p_ptr->afraid);
    wr_s16b(p_ptr->cut);
    wr_s16b(p_ptr->stun);
    wr_s16b(p_ptr->poisoned);
    wr_s16b(p_ptr->image);
    wr_s16b(p_ptr->protevil);
    wr_s16b(p_ptr->invuln);
    wr_s16b(p_ptr->hero);
    wr_s16b(p_ptr->shero);
    wr_s16b(p_ptr->shield);
    wr_s16b(p_ptr->blessed);
    wr_s16b(p_ptr->detect_inv);
    wr_s16b(p_ptr->word_recall);
    wr_s16b(p_ptr->see_infra);
    wr_s16b(p_ptr->tim_infra);
    wr_s16b(p_ptr->oppose_fire);
    wr_s16b(p_ptr->oppose_cold);
    wr_s16b(p_ptr->oppose_acid);
    wr_s16b(p_ptr->oppose_elec);
    wr_s16b(p_ptr->oppose_pois);
    wr_byte(p_ptr->immune_acid);
    wr_byte(p_ptr->immune_elec);
    wr_byte(p_ptr->immune_fire);
    wr_byte(p_ptr->immune_cold);
    wr_byte(p_ptr->immune_pois);
    wr_byte(p_ptr->resist_acid);
    wr_byte(p_ptr->resist_elec);
    wr_byte(p_ptr->resist_fire);
    wr_byte(p_ptr->resist_cold);
    wr_byte(p_ptr->resist_pois);
    wr_byte(p_ptr->resist_conf);
    wr_byte(p_ptr->resist_sound);
    wr_byte(p_ptr->resist_lite);
    wr_byte(p_ptr->resist_dark);
    wr_byte(p_ptr->resist_chaos);
    wr_byte(p_ptr->resist_disen);
    wr_byte(p_ptr->resist_shards);
    wr_byte(p_ptr->resist_nexus);
    wr_byte(p_ptr->resist_blind);
    wr_byte(p_ptr->resist_nether);
    wr_byte(p_ptr->resist_fear);
    wr_byte(p_ptr->see_inv);
    wr_byte(p_ptr->teleport);
    wr_byte(p_ptr->free_act);
    wr_byte(p_ptr->slow_digest);
    wr_byte(p_ptr->aggravate);
    wr_byte(p_ptr->regenerate);
    wr_byte(p_ptr->ffall);
    wr_byte(p_ptr->sustain_str);
    wr_byte(p_ptr->sustain_int);
    wr_byte(p_ptr->sustain_wis);
    wr_byte(p_ptr->sustain_con);
    wr_byte(p_ptr->sustain_dex);
    wr_byte(p_ptr->sustain_chr);
    wr_byte(p_ptr->confuse_monster);
    wr_byte(p_ptr->hold_life);
    wr_byte(p_ptr->telepathy);
    wr_byte(p_ptr->light);

    wr_u16b(missile_ctr);


    /* Write the "object seeds" */
    wr_u32b(randes_seed);
    wr_u32b(town_seed);


    /* Special stuff */
    wr_u16b(panic_save);
    wr_u16b(total_winner);
    wr_u16b(noscore);


    /* Write death */
    wr_byte(death);

    /* Write feeling */
    wr_byte(feeling);

    /* Turn of last "feeling" */
    wr_u32b(old_turn);

    /* Current turn */
    wr_u32b(turn);
}


/*
 * Read the player inventory
 */
static errr rd_inventory()
{
    int i;

    inven_ctr = 0;
    equip_ctr = 0;
    inven_weight = 0;

    /* Count the items */
    rd_u16b(&inven_ctr);

    /* Verify */
    if (inven_ctr > INVEN_WIELD) return (15);

    /* Normal pack items */
    for (i = 0; i < inven_ctr; i++) {

	/* Read the item */
	rd_item(&inventory[i]);
    }

    /* Old "normal" equipment */
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {

	/* Read the item */
	rd_item(&inventory[i]);
    }

	rd_u16b(&inven_weight);

	rd_u16b(&equip_ctr);
}



/*
 * Read the saved messages
 */
static void rd_messages()
{
    int i;

    /* Hack -- circular queue */
    rd_u16b(&last_msg);

    /* Read the messages */
    for (i = 0; i < MAX_SAVE_MSG; i++) {

	/* Read the message */
	rd_string(old_msg[i]);
    }
}



/* 
 * Write/Read the actual Dungeon
 */

static void wr_dungeon()
{
    int i, j;
    byte count, prev_char;
    byte tmp8u;
    cave_type *c_ptr;
    inven_type *t_ptr;

    /* Dungeon specific info follows */
    wr_u16b(dun_level);
    wr_u16b(mon_tot_mult);
    wr_u16b(char_row);
    wr_u16b(char_col);
    wr_u16b(cur_height);
    wr_u16b(cur_width);
    wr_u16b(max_panel_rows);
    wr_u16b(max_panel_cols);


    /*** Simple "Run-Length-Encoding" of cave ***/

    /* Note that this will induce two wasted bytes */
    count = 0;
    prev_char = 0;

    /* Dump the cave */
    for (i = 0; i < cur_height; i++) {
	for (j = 0; j < cur_width; j++) {

	    /* Get the cave */
	    c_ptr = &cave[i][j];

	    /* Create an encoded byte of info */            
	    tmp8u = (c_ptr->fval);
	    tmp8u |= (c_ptr->lr << 4) | (c_ptr->fm << 5) | (c_ptr->pl << 6) | (c_ptr->tl << 7);

	    /* If the run is broken, or too full, flush it */
	    if (tmp8u != prev_char || count == MAX_UCHAR) {
		wr_byte((byte) count);
		wr_byte(prev_char);
		prev_char = tmp8u;
		count = 1;
	    }

	    /* Continue the run */
	    else {
		count++;
	    }
	}
    }

    /* save last entry */
	wr_byte((byte) count);
	wr_byte(prev_char);

#ifdef MSDOS
/* must change graphics symbols for walls and floors back to default chars,
 * this is necessary so that if the user changes the graphics line, the
 * program will be able change all existing walls/floors to the new symbol 
 */
    t_ptr = &i_list[i_max - 1];
    for (i = i_max - 1; i >= MIN_I_IDX; i--) {
	if (t_ptr->tchar == wallsym)
	    t_ptr->tchar = '#';
	t_ptr--;
    }
#endif


    /* Dump the items (note: starting at #1) */
    wr_u16b(i_max);
    for (i = MIN_I_IDX; i < i_max; i++) {
	wr_item(&i_list[i]);
    }


    /* Dump the monsters (note: starting at #2) */    
    wr_u16b(m_max);
    for (i = MIN_M_IDX; i < m_max; i++) {
	wr_monster(&m_list[i]);
    }
}









/* 
 * New Method
 */

static errr rd_dungeon()
{
    int i;
    byte count;
    byte ychar, xchar;
    byte tmp8u;
    u16b tmp16u;
    int ymax, xmax;
    int total_count;
    cave_type *c_ptr;
    inven_type *t_ptr;


    /* Header info */            
    rd_u16b(&dun_level);
    rd_u16b(&mon_tot_mult);
    rd_u16b(&char_row);
    rd_u16b(&char_col);
    rd_u16b(&cur_height);
    rd_u16b(&cur_width);
    rd_u16b(&max_panel_rows);
    rd_u16b(&max_panel_cols);

    /* Only read as necessary */    
    ymax = cur_height;
    xmax = cur_width;

    /* Read in the actual "cave" data */
    total_count = 0;
    xchar = ychar = 0;

    /* Read until done */
    while (total_count < ymax * xmax) {

	/* Extract some RLE info */
	rd_byte(&count);
	rd_byte(&tmp8u);

	/* Apply the RLE info */
	for (i = count; i > 0; i--) {

	    /* Prevent over-run */
	    if (ychar >= ymax) {
		prt_note(-2, "Dungeon too big!");
		return (81);
	    }

	    /* Access the cave */
	    c_ptr = &cave[ychar][xchar];

	    /* Extract the floor type */
	    c_ptr->fval = tmp8u & 0xF;

	    /* Extract the "info" */
	    c_ptr->lr = (tmp8u >> 4) & 0x1;
	    c_ptr->fm = (tmp8u >> 5) & 0x1;
	    c_ptr->pl = (tmp8u >> 6) & 0x1;
	    c_ptr->tl = (tmp8u >> 7) & 0x1;

	    /* Advance the cave pointers */
	    xchar++;

	    /* Wrap to the next line */
	    if (xchar >= xmax) {
		xchar = 0;
		ychar++;
	    }
	}

	total_count += count;
    }


    /* Read the item count */
    rd_u16b(&i_max);
    if (i_max > MAX_I_IDX) {
	prt_note(-2, "Too many objects");
	return (92);
    }

    /* Read the dungeon items, note locations in cave */
    for (i = MIN_I_IDX; i < i_max; i++) {
	inven_type *i_ptr = &i_list[i];
	rd_item(i_ptr);
	cave[i_ptr->iy][i_ptr->ix].i_idx = i;
    }


    /* Read the monster count */        
    rd_u16b(&m_max);
    if (m_max > MAX_M_IDX) {
	prt_note(-2, "Too many monsters");
	return (93);
    }

    /* Read the monsters, note locations in cave */
    for (i = MIN_M_IDX; i < m_max; i++) {
	monster_type *m_ptr = &m_list[i];
	rd_monster(m_ptr);
	cave[m_ptr->fy][m_ptr->fx].m_idx = i;
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

    /* Success */
    return (0);
}


/*
 * Hack -- see "save-old.c"
 */


/*
 * Actually read the savefile
 */
static errr rd_savefile()
{
    int i;

    u16b tmp16u;

    prt_note(0,"Restoring Memory...");

    /* Get the version info */
    xor_byte = 0;
    rd_byte(&version_maj);
    xor_byte = 0;
    rd_byte(&version_min);
    xor_byte = 0;
    rd_byte(&patch_level);
    xor_byte = 0;
    rd_byte(&xor_byte);

    /* Handle stupidity from Angband 2.4 / 2.5 */
    if ((version_maj == 5) && (version_min == 2)) {
	version_maj = 2;
	version_min = 5;
    }


    /* Verify the "major version" */
    if (version_maj != CUR_VERSION_MAJ) {

	prt_note(-2,"This savefile is from a different version of Angband.");
	return (11);
    }


    /* XXX Hack -- We cannot read savefiles more recent than we are */
    if ((version_min > CUR_VERSION_MIN) ||
	(version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL)) {

	prt_note(-2,"This savefile is from a more recent version of Angband.");
	return (12);
    }


    /* Begin Wizard messages */
    if (say) prt_note(-2,"Loading savefile...");


    /* Hack -- parse old savefiles */
    if (older_than(2,7,0)) {
	extern errr rd_old_sf(FILE *fff1, int vmaj, int vmin, int vpat, int say);
	return (rd_old_sf(fff, version_maj, version_min, patch_level, say));
    }

    /* Operating system info */
    rd_u32b(&sf_xtra);

    /* Time of savefile creation */
    rd_u32b(&sf_when);


    /* A "sized" chunk of "unused" space */
    rd_u32b(&tmp32u);

    /* Read (and forget) those bytes */
    for (i = 0; i < tmp32u; i++) rd_byte(&tmp8u);


    /* A "sized" list of "strings" */
    rd_u32b(&tmp32u);

    /* Read (and forget) those strings */
    for (i = 0; i < tmp32u; i++) {

	/* Read and forget a string */
	while (1) {
	    rd_byte(&tmp8u);
	    if (!tmp8u) break;
	}
    }


    /* Then the options */
    rd_options();
    if (say) prt_note(-1,"Loaded Option Flags");


    /* Then the "messages" */
    rd_messages();
    if (say) prt_note(-1,"Loaded Messages");


    /* Monster Memory */
    rd_u16b(&tmp16u);
    for (i = 0; i < tmp16u; i++) rd_lore(&l_list[i]);
    if (say) prt_note(-1,"Loaded Monster Memory");

    /* Load the old "Uniques" flags */
    for (i = 0; i < MAX_R_IDX; i++) {

	rd_s32b(&u_list[i].exist);
	rd_s32b(&u_list[i].dead);
    }
    if (say) prt_note(-1,"Loaded Unique Beasts");

    /* Object Memory */
    rd_u16b(&tmp16u);
    for (i = 0; i < tmp16u; i++) rd_byte(&object_ident[i]);


    /* Load the Quests */
    rd_u16b(&tmp16u);
    for (i = 0; i < tmp16u; i++) {
	rd_u32b(&quests[i]);
    }
    if (say) prt_note(-1,"Loaded Quests");


    /* Load the Artifacts */
    rd_u32b(&GROND);
    rd_u32b(&RINGIL);
    rd_u32b(&AEGLOS);
    rd_u32b(&ARUNRUTH);
    rd_u32b(&MORMEGIL);
    rd_u32b(&ANGRIST);
    rd_u32b(&GURTHANG);
    rd_u32b(&CALRIS);
    rd_u32b(&ANDURIL);
    rd_u32b(&STING);
    rd_u32b(&ORCRIST);
    rd_u32b(&GLAMDRING);
    rd_u32b(&DURIN);
    rd_u32b(&AULE);
    rd_u32b(&THUNDERFIST);
    rd_u32b(&BLOODSPIKE);
    rd_u32b(&DOOMCALLER);
    rd_u32b(&NARTHANC);
    rd_u32b(&NIMTHANC);
    rd_u32b(&DETHANC);
    rd_u32b(&GILETTAR);
    rd_u32b(&RILIA);
    rd_u32b(&BELANGIL);
    rd_u32b(&BALLI);
    rd_u32b(&LOTHARANG);
    rd_u32b(&FIRESTAR);
    rd_u32b(&ERIRIL);
    rd_u32b(&CUBRAGOL);
    rd_u32b(&BARD);
    rd_u32b(&COLLUIN);
    rd_u32b(&HOLCOLLETH);
    rd_u32b(&TOTILA);
    rd_u32b(&PAIN);
    rd_u32b(&ELVAGIL);
    rd_u32b(&AGLARANG);
    rd_u32b(&EORLINGAS);
    rd_u32b(&BARUKKHELED);
    rd_u32b(&WRATH);
    rd_u32b(&HARADEKKET);
    rd_u32b(&MUNDWINE);
    rd_u32b(&GONDRICAM);
    rd_u32b(&ZARCUTHRA);
    rd_u32b(&CARETH);
    rd_u32b(&FORASGIL);
    rd_u32b(&CRISDURIAN);
    rd_u32b(&COLANNON);
    rd_u32b(&HITHLOMIR);
    rd_u32b(&THALKETTOTH);
    rd_u32b(&ARVEDUI);
    rd_u32b(&THRANDUIL);
    rd_u32b(&THENGEL);
    rd_u32b(&HAMMERHAND);
    rd_u32b(&CELEGORM);
    rd_u32b(&THROR);
    rd_u32b(&MAEDHROS);
    rd_u32b(&OLORIN);
    rd_u32b(&ANGUIREL);
    rd_u32b(&OROME);
    rd_u32b(&EONWE);
    rd_u32b(&THEODEN);
    rd_u32b(&ULMO);
    rd_u32b(&OSONDIR);
    rd_u32b(&TURMIL);
    rd_u32b(&CASPANION);
    rd_u32b(&TIL);
    rd_u32b(&DEATHWREAKER);
    rd_u32b(&AVAVIR);
    rd_u32b(&TARATOL);
    rd_u32b(&DOR_LOMIN);
    rd_u32b(&NENYA);
    rd_u32b(&NARYA);
    rd_u32b(&VILYA);
    rd_u32b(&BELEGENNON);
    rd_u32b(&FEANOR);
    rd_u32b(&ISILDUR);
    rd_u32b(&SOULKEEPER);
    rd_u32b(&FINGOLFIN);
    rd_u32b(&ANARION);
    rd_u32b(&POWER);
    rd_u32b(&PHIAL);
    rd_u32b(&BELEG);
    rd_u32b(&DAL);
    rd_u32b(&PAURHACH);
    rd_u32b(&PAURNIMMEN);
    rd_u32b(&PAURAEGEN);
    rd_u32b(&PAURNEN);
    rd_u32b(&CAMMITHRIM);
    rd_u32b(&CAMBELEG);
    rd_u32b(&INGWE);
    rd_u32b(&CARLAMMAS);
    rd_u32b(&HOLHENNETH);
    rd_u32b(&AEGLIN);
    rd_u32b(&CAMLOST);
    rd_u32b(&NIMLOTH);
    rd_u32b(&NAR);
    rd_u32b(&BERUTHIEL);
    rd_u32b(&GORLIM);
    rd_u32b(&ELENDIL);
    rd_u32b(&THORIN);
    rd_u32b(&CELEBORN);
    rd_u32b(&THRAIN);
    rd_u32b(&GONDOR);
    rd_u32b(&THINGOL);
    rd_u32b(&THORONGIL);
    rd_u32b(&LUTHIEN);
    rd_u32b(&TUOR);
    rd_u32b(&ROHAN);
    rd_u32b(&TULKAS);
    rd_u32b(&NECKLACE);
    rd_u32b(&BARAHIR);
    rd_u32b(&RAZORBACK);
    rd_u32b(&BLADETURNER);
    if (say) prt_note(-1,"Loaded Artifacts");


    /* Read the extra stuff */
    rd_extra();
    if (say) prt_note(-1, "Loaded extra information");


    /* Read the player_hp array */
    rd_u16b(&tmp16u);
    for (i = 0; i < tmp16u; i++) {
	rd_u16b(&player_hp[i]);
    }


    /* Read spell info */
    rd_u32b(&spell_learned);
    rd_u32b(&spell_learned2);
    rd_u32b(&spell_worked);
    rd_u32b(&spell_worked2);
    rd_u32b(&spell_forgotten);
    rd_u32b(&spell_forgotten2);

    for (i = 0; i < 64; i++) {
	rd_byte(&spell_order[i]);
    }


    /* Read the inventory */
    if (rd_inventory()) {
	prt_note(-2, "Unable to read inventory");
	return (21);
    }


    /* Read the stores */
    rd_u16b(&tmp16u);
    for (i = 0; i < tmp16u; i++) {
	if (rd_store(&store[i])) return (22);
    }


    /* I'm not dead yet... */
    if (!death) {

	/* Dead players have no dungeon */
	prt_note(-1,"Restoring Dungeon...");
	if (rd_dungeon()) {
	    prt_note(-2, "Error reading dungeon data");
	    return (34);
	}

	/* Read the ghost info */
	rd_ghost();
    }

    /* Success */
    return (0);
}




/*
 * Actually write a save-file
 */

static int wr_savefile()
{
    register int        i;
    register monster_lore *r_ptr;
    u32b              now;

    byte		tmp8u;
    u16b		tmp16u;


    /* Guess at the current time */
    now = time((time_t *)0);

#if 0
    /* If someone is messing with the clock, assume one day of play time */
    if (now < sf_when) now = sf_when + 86400L;
#endif


    /* Note the operating system */
    sf_xtra = 0L;

    /* Note when the file was saved */
    sf_when = now;


    /*** Actually write the file ***/

    /* Dump the file header */
    xor_byte = 0;
    wr_byte(CUR_VERSION_MAJ);
    xor_byte = 0;
    wr_byte(CUR_VERSION_MIN);
    xor_byte = 0;
    wr_byte(PATCH_LEVEL);
    xor_byte = 0;
    tmp8u = rand_int(256);
    wr_byte(tmp8u);


    /* Operating system */
    wr_u32b(sf_xtra);


    /* Time file last saved */
    wr_u32b(sf_when);


    /* No extra bytes for this operating system */
    wr_u32b(0L);

    /* No extra strings for this operating system */
    wr_u32b(0L);


/* clear the death flag when creating a HANGUP save file, so that player can
 * see tombstone when restart 
 */
    if (eof_flag)
	death = FALSE;


    /* Write the boolean "options" */
    wr_options();


    /* Dump the messages */
    wr_u16b(last_msg);
    for (i = 0; i < MAX_SAVE_MSG; i++) {
	wr_string(old_msg[i]);
    }


    /* XXX This could probably be more "efficient" for "unseen" monsters */
    /* XXX But note that "max_num" is stored here as well (always "set") */

    /* Dump the monster lore */
    tmp16u = MAX_R_IDX;
    wr_u16b(tmp16u);
    for (i = 0; i < tmp16u; i++) wr_lore(&l_list[i]);

    for (i = 0; i < MAX_R_IDX; i++) {
	wr_s32b(u_list[i].exist);
	wr_s32b(u_list[i].dead);
    }

    /* Dump the object memory */
    tmp16u = OBJECT_IDENT_SIZE;
    wr_u16b(tmp16u);
    for (i = 0; i < tmp16u; i++) wr_byte(object_ident[i]);


    /* Hack -- Dump the quests */    
    tmp16u = QUEST_MAX;
    wr_u16b(tmp16u);
    for (i = 0; i < tmp16u; i++) {
	wr_u32b(quests[i]);
    }

    /* Hack -- Dump the artifacts */
    wr_u32b(GROND);
    wr_u32b(RINGIL);
    wr_u32b(AEGLOS);
    wr_u32b(ARUNRUTH);
    wr_u32b(MORMEGIL);
    wr_u32b(ANGRIST);
    wr_u32b(GURTHANG);
    wr_u32b(CALRIS);
    wr_u32b(ANDURIL);
    wr_u32b(STING);
    wr_u32b(ORCRIST);
    wr_u32b(GLAMDRING);
    wr_u32b(DURIN);
    wr_u32b(AULE);
    wr_u32b(THUNDERFIST);
    wr_u32b(BLOODSPIKE);
    wr_u32b(DOOMCALLER);
    wr_u32b(NARTHANC);
    wr_u32b(NIMTHANC);
    wr_u32b(DETHANC);
    wr_u32b(GILETTAR);
    wr_u32b(RILIA);
    wr_u32b(BELANGIL);
    wr_u32b(BALLI);
    wr_u32b(LOTHARANG);
    wr_u32b(FIRESTAR);
    wr_u32b(ERIRIL);
    wr_u32b(CUBRAGOL);
    wr_u32b(BARD);
    wr_u32b(COLLUIN);
    wr_u32b(HOLCOLLETH);
    wr_u32b(TOTILA);
    wr_u32b(PAIN);
    wr_u32b(ELVAGIL);
    wr_u32b(AGLARANG);
    wr_u32b(EORLINGAS);
    wr_u32b(BARUKKHELED);
    wr_u32b(WRATH);
    wr_u32b(HARADEKKET);
    wr_u32b(MUNDWINE);
    wr_u32b(GONDRICAM);
    wr_u32b(ZARCUTHRA);
    wr_u32b(CARETH);
    wr_u32b(FORASGIL);
    wr_u32b(CRISDURIAN);
    wr_u32b(COLANNON);
    wr_u32b(HITHLOMIR);
    wr_u32b(THALKETTOTH);
    wr_u32b(ARVEDUI);
    wr_u32b(THRANDUIL);
    wr_u32b(THENGEL);
    wr_u32b(HAMMERHAND);
    wr_u32b(CELEGORM);
    wr_u32b(THROR);
    wr_u32b(MAEDHROS);
    wr_u32b(OLORIN);
    wr_u32b(ANGUIREL);
    wr_u32b(OROME);
    wr_u32b(EONWE);
    wr_u32b(THEODEN);
    wr_u32b(ULMO);
    wr_u32b(OSONDIR);
    wr_u32b(TURMIL);
    wr_u32b(CASPANION);
    wr_u32b(TIL);
    wr_u32b(DEATHWREAKER);
    wr_u32b(AVAVIR);
    wr_u32b(TARATOL);

    wr_u32b(DOR_LOMIN);
    wr_u32b(NENYA);
    wr_u32b(NARYA);
    wr_u32b(VILYA);
    wr_u32b(BELEGENNON);
    wr_u32b(FEANOR);
    wr_u32b(ISILDUR);
    wr_u32b(SOULKEEPER);
    wr_u32b(FINGOLFIN);
    wr_u32b(ANARION);
    wr_u32b(POWER);
    wr_u32b(PHIAL);
    wr_u32b(BELEG);
    wr_u32b(DAL);
    wr_u32b(PAURHACH);
    wr_u32b(PAURNIMMEN);
    wr_u32b(PAURAEGEN);
    wr_u32b(PAURNEN);
    wr_u32b(CAMMITHRIM);
    wr_u32b(CAMBELEG);
    wr_u32b(INGWE);
    wr_u32b(CARLAMMAS);
    wr_u32b(HOLHENNETH);
    wr_u32b(AEGLIN);
    wr_u32b(CAMLOST);
    wr_u32b(NIMLOTH);
    wr_u32b(NAR);
    wr_u32b(BERUTHIEL);
    wr_u32b(GORLIM);
    wr_u32b(ELENDIL);
    wr_u32b(THORIN);
    wr_u32b(CELEBORN);
    wr_u32b(THRAIN);
    wr_u32b(GONDOR);
    wr_u32b(THINGOL);
    wr_u32b(THORONGIL);
    wr_u32b(LUTHIEN);
    wr_u32b(TUOR);
    wr_u32b(ROHAN);
    wr_u32b(TULKAS);
    wr_u32b(NECKLACE);
    wr_u32b(BARAHIR);
    wr_u32b(RAZORBACK);
    wr_u32b(BLADETURNER);



    /* Write the "extra" information */
    wr_extra();


    /* Dump the "player hp" entries */
    tmp16u = MAX_PLAYER_LEVEL;
    wr_u16b(tmp16u);
    for (i = 0; i < tmp16u; i++) {
	wr_u16b(player_hp[i]);
    }


    /* Write spell data */
    wr_u32b(spell_learned);
    wr_u32b(spell_learned2);
    wr_u32b(spell_worked);
    wr_u32b(spell_worked2);
    wr_u32b(spell_forgotten);
    wr_u32b(spell_forgotten2);

    /* Dump the ordered spells */
    for (i = 0; i < 64; i++) {
	wr_byte(spell_order[i]);
    }


    /* Write the inventory */
    wr_u16b(inven_ctr);
    for (i = 0; i < inven_ctr; i++)
	wr_item(&inventory[i]);
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
	wr_item(&inventory[i]);
    wr_u16b(inven_weight);
    wr_u16b(equip_ctr);


    /* Note the stores */
    tmp16u = MAX_STORES;
    wr_u16b(tmp16u);

    /* Dump the stores */
    for (i = 0; i < tmp16u; i++) wr_store(&store[i]);


    /* Player is not dead, write the dungeon */
    if (!death) {

	/* Dump the dungeon */
	wr_dungeon();

	/* Dump the ghost */
	wr_ghost();
    }

    /* Error in save */
    if (ferror(fff) || (fflush(fff) == EOF)) return FALSE;

    /* Successful save */
    return TRUE;
}


/*
 * Medium level player saver
 */
int _save_player(char *fnam)
{
    int   ok, fd;
    vtype temp;

    /* Forbid suspend */
    signals_ignore_tstp();

    put_qio();
    disturb(1, 0);		   /* Turn off resting and searching. */

    /* Fix the speed */
    p_ptr->speed -= pack_heavy;
    p_ptr->status |= PY_SPEED;
    pack_heavy = 0;

    /* Assume failure */
    ok = FALSE;

#ifdef ATARIST_MWC
#else /* ATARIST_MWC */

    fd = (-1);
    fff = NULL;		   /* Do not assume it has been init'ed */

#ifdef SET_UID
    fd = my_topen(fnam, O_RDWR | O_CREAT | O_EXCL, 0600);
#else
    fd = my_topen(fnam, O_RDWR | O_CREAT | O_EXCL, 0666);
#endif

    /* This might not work... */
    if ((fd < 0) && (access(fnam, 0) >= 0) &&
	(from_savefile ||
	 (wizard && get_check("Can't make new savefile. Overwrite old?")))) {

#ifdef SET_UID
	(void)chmod(fnam, 0600);
	fd = my_topen(fnam, O_RDWR | O_TRUNC, 0600);
#else
	(void)chmod(fnam, 0666);
	fd = my_topen(fnam, O_RDWR | O_TRUNC, 0666);
#endif

    }

    if (fd >= 0) {

	/* Close the "fd" */
	(void)close(fd);

#endif				   /* !ATARIST_MWC */
    /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARIST_MWC) || defined(MSDOS) || defined(__MINT__)
	fff = my_tfopen(savefile, "wb");
#else
	fff = my_tfopen(savefile, "w");
#endif
#ifndef ATARIST_MWC
    }

#endif

    /* Successful open */
    if (fff) {

#ifdef MSDOS
	(void)setmode(fileno(fff), O_BINARY);
#endif

	/* Write the savefile */
	ok = wr_savefile();

	/* Attempt to close it */
	if (fclose(fff) == EOF) ok = FALSE;
    }


    /* Error */
    if (!ok) {

	if (fd >= 0) (void)unlink(fnam);

	/* Allow suspend again */
	signals();

	/* Oops */
	if (fd >= 0) (void)sprintf(temp, "Error writing to savefile");
	else (void)sprintf(temp, "Can't create new savefile");
	msg_print(temp);
	return FALSE;
    }

    /* Successful save */
    character_saved = 1;

    turn = (-1);

    /* Allow suspend again */
    signals();

    /* Successful save */
    return TRUE;
}



/*
 * Attempt to save the player in a savefile
 */
int save_player()
{
    int result = FALSE;
    vtype temp;
    char *tmp2;

#ifdef SECURE
    beGames();
#endif

    if (_save_player(savefile)) {

	tmp2 = strrchr(savefile, (int)'/');
	if (tmp2 == (char *)0) tmp2 = savefile;

	(void)sprintf(temp, "%s/p.%s", ANGBAND_DIR_SAVE, (tmp2 + 1));

	unlink(temp);

	/* Success */
	result = TRUE;
    }

#ifdef SECURE
    bePlayer();
#endif

    /* Return the result */
    return (result);
}







/*
 * Version 2.7.0 uses an entirely different "savefile" format.
 * It can still read the old files, though it may lose a little
 * data in transfer, in particular, some of the "saved messages".
 *
 * Note that versions "5.2.x" can never be made.
 * This boneheadedness is a direct result of the fact that Angband 2.4
 * had version constants of 5.2, not 2.4.  2.5 inherited this.  2.6 fixes
 * the problem.  Note that there must never be a 5.2.x version of Angband,
 * or else this code will get confused. -CWS
 *
 * Actually, this is not true, since by the time version 5.2 comes around,
 * anybody trying to use a version 2.5 savefile deserves what they get!
 */

int load_player(int *generate)
{
    int                    i, fd, ok, days;
    u32b                 age;

    vtype temp;


    /* Hack -- allow "debugging" */
    int wiz = to_be_wizard;


    /* Set "say" as well */
    if (wiz) say = TRUE;


    /* Forbid suspend */
    signals_ignore_tstp();

    /* Assume a cave must be generated */
    *generate = TRUE;

    /* Assume no file (used to catch errors below) */
    fd = (-1);


    /* Hack -- Cannot restore a game while still playing */
    if (turn > 0) {
	msg_print("IMPOSSIBLE! Attempt to restore while still alive!");
	return (FALSE);
    }

    if (access(savefile, 0) < 0) {

	/* Allow suspend again */
	signals();

	msg_print("Savefile does not exist.");
	return FALSE;
    }

    /* Notify the player */
    clear_screen();
    (void)sprintf(temp, "Restoring Character.");
    put_str(temp, 23, 0);

    /* Hack -- let the message get read */
    sleep(1);

    /* Allow restoring a file belonging to someone else, */
    /* but only if we can delete it. */
    /* Hence first try to read without doing a chmod. */

    /* Open the savefile */
    fd = my_topen(savefile, O_RDONLY, 0);

    if (fd < 0) {
	msg_print("Can't open file for reading.");
    }

    else {

#if !defined(SET_UID)
	struct stat         statbuf;
#endif

	turn = (-1);

	ok = TRUE;

#if !defined(SET_UID)
	(void)fstat(fd, &statbuf);
#endif

	(void)close(fd);


	/* GCC for atari st defines atarist */
#if defined(__MINT__) || defined(atarist) || defined(ATARIST_MWC) || defined(MSDOS)
	fff = my_tfopen(savefile, "rb");
#else
	fff = my_tfopen(savefile, "r");
#endif
	if (!fff) goto error;


	/* Actually read the savefile */
	if (rd_savefile()) goto error;

	/* Hack -- Alive, so no need to make a cave */
	if (!death) *generate = FALSE;


#if !defined(SET_UID) && !defined(ALLOW_FIDDLING)
	if (!wiz) {
	    if (sf_when > (statbuf.st_ctime + 100) ||
		sf_when < (statbuf.st_ctime - 100)) {
		prt_note(-2,"Fiddled save file");
		goto error;
	    }
	}
#endif



	/* Check for errors */
	if (ferror(fff)) {
	    prt_note(-2,"FILE ERROR");
	    goto error;
	}


	/* Process "dead" players */
	if (death) {

	    /* Wizards can revive dead characters */
	    if (wiz && get_check("Resurrect a dead character?")) {

		/* Revive the player */
		prt_note(0,"Attempting a resurrection!");

		/* Not quite dead */
		if (p_ptr->chp < 0) {
		    p_ptr->chp = 0;
		    p_ptr->chp_frac = 0;
		}

		/* don't let him starve to death immediately */
		if (p_ptr->food < 5000) p_ptr->food = 5000;

		cure_poison();
		cure_blindness();
		cure_confusion();
		remove_fear();

		if (p_ptr->image > 0) p_ptr->image = 0;
		if (p_ptr->cut > 0) p_ptr->cut = 0;
		if (p_ptr->stun > 0) {
		    if (p_ptr->stun > 50) {
			p_ptr->ptohit += 20;
			p_ptr->ptodam += 20;
		    } else {
			p_ptr->ptohit += 5;
			p_ptr->ptodam += 5;
		    }
		    p_ptr->stun = 0;
		}
		if (p_ptr->word_recall > 0) p_ptr->word_recall = 0;

		/* Resurrect on the town level. */
		dun_level = 0;

		/* Set character_generated */
		character_generated = 1;

		/* set noscore to indicate a resurrection */
		noscore |= 0x1;

		/* XXX Don't enter wizard mode */
		to_be_wizard = FALSE;

		/* Player is no longer "dead" */
		death = FALSE;
	    }

	    /* Normal "restoration" */
	    else {

		prt_note(0,"Restoring Memory of a departed spirit...");

		/* Forget the turn, and old_turn */
		turn = old_turn = (-1);

		/* Hack -- skip file verification */
		goto closefiles;
	    }
	}        


	if (turn < 0) {

	    prt_note(-2,"Invalid turn");

error:

	    /* Assume bad data. */
	    ok = FALSE;
	}

	else {

	    /* don't overwrite the "killed by" string if character is dead */
	    if (p_ptr->chp >= 0) {
		(void)strcpy(died_from, "(alive and well)");
	    }

	    character_generated = 1;
	}

closefiles:

	if (fff) {
	    if (fclose(fff) < 0) ok = FALSE;
	}

	if (fd >= 0) (void)close(fd);

	if (!ok) {
	    msg_print("Error during reading of file.");
	    msg_print(NULL);
	}

	else {

	    /* Hack -- Let the user overwrite the old savefile when save/quit */
	    from_savefile = 1;

	    /* Allow suspend again */
	    signals();

	    /* Only if a full restoration. */
	    if (turn >= 0) {

		weapon_heavy = FALSE;
		pack_heavy = 0;
		check_strength();

		/* rotate store inventory, based on time passed */
		/* foreach day old (rounded up), call store_maint */

		/* Get the current time */
		age = time((time_t *)0);

		/* Subtract the save-file time */
		age = age - sf_when;

		/* Convert age to "real time" in days */
		days = age / 86400L;

		/* Assume no more than 10 days old */
		if (days > 10) days = 10;

		/* Rotate the store inventories (once per day) */
		for (i = 0; i < days; i++) store_maint();
	    }

#if 0
	    if (noscore) {
		msg_print("This savefile cannot yield high scores.");
	    }
#endif

	    /* Give a warning */
	    if (version_maj != CUR_VERSION_MAJ ||
		version_min != CUR_VERSION_MIN) {

		(void)sprintf(temp,
			"Save file version %d.%d %s on game version %d.%d.",
			      version_maj, version_min,
			      version_maj == CUR_VERSION_MAJ
			      ? "accepted" : "very risky",
			      CUR_VERSION_MAJ, CUR_VERSION_MIN);
		msg_print(temp);
	    }

	    if (turn >= 0) {

		char *tmp2 = strrchr(savefile, (int)'/');
		if (tmp2 == (char *)0) tmp2 = savefile;

		(void)sprintf(temp, "%s/p.%s", ANGBAND_DIR_SAVE, (tmp2 + 1));

		link(savefile, temp);
		unlink(savefile);
		return TRUE;
	    } else {
		return FALSE;	   /* Only restored options and monster memory. */
	    }
	}
    }


    /* Oh well... */
    prt_note(-2,"Please try again without that savefile.");

    /* No game in progress */
    turn = (-1);

    /* Allow suspend again */
    signals();

#ifdef MAC
    *exit_flag = TRUE;
#else
    exit_game();
#endif

    /* Compiler food */
    return FALSE;		   /* not reached, unless on mac */
}


