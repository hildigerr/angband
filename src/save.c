/*
 * save.c: save and restore games and monster memory info 
 *
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"
#include "monster.h"

#ifndef USG
/* stuff only needed for Berkeley UNIX */
#include <sys/types.h>
#include <sys/file.h>
#include <sys/param.h>
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#include <fcntl.h>
#else
#include "string.h"
#endif
#else
#include <strings.h>
#endif

#ifdef __MINT__
#include <stat.h>		/* chmod() */
#endif

/* Lets do all prototypes correctly.... -CWS */
#ifndef NO_LINT_ARGS
static int sv_write();
static void wr_byte();
static void wr_short();
static void wr_long();
static void wr_bytes();
static void wr_string();
static void wr_shorts();
static void wr_item();
static void wr_monster();
static void rd_byte();
static void rd_short();
static void rd_long();
static void rd_bytes();
static void rd_string();
static void rd_shorts();
static void rd_item();
static void rd_monster();
#endif

#if !defined(ATARIST_MWC)
#ifdef MAC
#include <time.h>
#else
long time();

#endif
#else
char *malloc();

#endif

#ifndef SET_UID
#include <sys/stat.h>
#endif

/*
 * these are used for the save file, to avoid having to pass them to every
 * procedure 
 */
static FILE        *fileptr;
static byte        xor_byte;
static int          from_savefile; /* can overwrite old savefile when save */
static u32b       start_time;	   /* time that play started */
static byte        version_maj, version_min, patch_level;

/*
 * This save package was brought to by			-JWT- and
 * RAK- and has been completely rewritten for UNIX by	-JEW-  
 */
/* and has been completely rewritten again by	 -CJS-	 */
/* and completely rewritten again! for portability by -JEW- */


static char *
basename(a)
char *a;
{
    char *b;
    char *strrchr();

    if ((b = strrchr(a, (int)'/')) == (char *)0)
	return a;
    return b;
}

static void 
wr_unique(item)
register struct unique_mon *item;
{
    wr_long((u32b) item->exist);
    wr_long((u32b) item->dead);
}

static void 
rd_unique(item)
register struct unique_mon *item;
{
    rd_long((u32b *) & item->exist);
    rd_long((u32b *) & item->dead);
}


static int 
sv_write()
{
    u32b              l;
    register int        i, j;
    int                 count;
    byte               char_tmp, prev_char;
    register cave_type   *c_ptr;
    register monster_lore *r_ptr;
    struct stats         *s_ptr;

#ifdef MSDOS
    inven_type           *t_ptr;

#endif
    register struct flags *f_ptr;
    store_type  *st_ptr;
    struct misc *m_ptr;

/* clear the death flag when creating a HANGUP save file, so that player can
 * see tombstone when restart 
 */

    if (eof_flag)
	death = FALSE;

    l = 0;
    if (find_cut)
	l |= 1;
    if (find_examine)
	l |= 2;
    if (find_prself)
	l |= 4;
    if (find_bound)
	l |= 8;
    if (prompt_carry_flag)
	l |= 16;
    if (rogue_like_commands)
	l |= 32;
    if (show_inven_weight)
	l |= 64;
    if (notice_seams)
	l |= 128;
    if (find_ignore_doors)
	l |= 0x100L;
    if (no_haggle_flag)
	l |= 0x200L; 
    if (!carry_query_flag)
	l |= 0x400L;
    if (unfelt)
	l |= 0x0001000L;
    if (delay_spd > 10)
	delay_spd = 10;		/* bounds for delay_speed -CWS */
    if (delay_spd < 0)
	delay_spd = 0;
    l |= ((delay_spd & 0xf) << 13);
    l |= ((hitpoint_warn & 0xf) << 17);
    if (plain_descriptions)	/* don't do "black Mushroom of Curing" -CWS */
	l |= 0x00400000L;
    if (show_equip_weight)
	l |= 0x00800000L;
    if (feeling > 10)
	feeling = 0;		/* bounds for level feelings -CWS */
    if (feeling < 0)
	feeling = 0;
    l |= ((feeling & 0xf) << 24);
    if (equippy_chars)		/* equippy chars option -CWS */
	l |= 0x20000000L;
    if (quick_messages)		/* quick messages option -CWS */
	l |= 0x40000000L;
    if (death)
	l |= 0x80000000L;	/* Sign bit */
    wr_long(GROND);
    wr_long(RINGIL);
    wr_long(AEGLOS);
    wr_long(ARUNRUTH);
    wr_long(MORMEGIL);
    wr_long(ANGRIST);
    wr_long(GURTHANG);
    wr_long(CALRIS);
    wr_long(ANDURIL);
    wr_long(STING);
    wr_long(ORCRIST);
    wr_long(GLAMDRING);
    wr_long(DURIN);
    wr_long(AULE);
    wr_long(THUNDERFIST);
    wr_long(BLOODSPIKE);
    wr_long(DOOMCALLER);
    wr_long(NARTHANC);
    wr_long(NIMTHANC);
    wr_long(DETHANC);
    wr_long(GILETTAR);
    wr_long(RILIA);
    wr_long(BELANGIL);
    wr_long(BALLI);
    wr_long(LOTHARANG);
    wr_long(FIRESTAR);
    wr_long(ERIRIL);
    wr_long(CUBRAGOL);
    wr_long(BARD);
    wr_long(COLLUIN);
    wr_long(HOLCOLLETH);
    wr_long(TOTILA);
    wr_long(PAIN);
    wr_long(ELVAGIL);
    wr_long(AGLARANG);
    wr_long(EORLINGAS);
    wr_long(BARUKKHELED);
    wr_long(WRATH);
    wr_long(HARADEKKET);
    wr_long(MUNDWINE);
    wr_long(GONDRICAM);
    wr_long(ZARCUTHRA);
    wr_long(CARETH);
    wr_long(FORASGIL);
    wr_long(CRISDURIAN);
    wr_long(COLANNON);
    wr_long(HITHLOMIR);
    wr_long(THALKETTOTH);
    wr_long(ARVEDUI);
    wr_long(THRANDUIL);
    wr_long(THENGEL);
    wr_long(HAMMERHAND);
    wr_long(CELEGORM);
    wr_long(THROR);
    wr_long(MAEDHROS);
    wr_long(OLORIN);
    wr_long(ANGUIREL);
    wr_long(OROME);
    wr_long(EONWE);
    wr_long(THEODEN);
    wr_long(ULMO);
    wr_long(OSONDIR);
    wr_long(TURMIL);
    wr_long(CASPANION);
    wr_long(TIL);
    wr_long(DEATHWREAKER);
    wr_long(AVAVIR);
    wr_long(TARATOL);

    wr_long(DOR_LOMIN);
    wr_long(NENYA);
    wr_long(NARYA);
    wr_long(VILYA);
    wr_long(BELEGENNON);
    wr_long(FEANOR);
    wr_long(ISILDUR);
    wr_long(SOULKEEPER);
    wr_long(FINGOLFIN);
    wr_long(ANARION);
    wr_long(POWER);
    wr_long(PHIAL);
    wr_long(BELEG);
    wr_long(DAL);
    wr_long(PAURHACH);
    wr_long(PAURNIMMEN);
    wr_long(PAURAEGEN);
    wr_long(PAURNEN);
    wr_long(CAMMITHRIM);
    wr_long(CAMBELEG);
    wr_long(INGWE);
    wr_long(CARLAMMAS);
    wr_long(HOLHENNETH);
    wr_long(AEGLIN);
    wr_long(CAMLOST);
    wr_long(NIMLOTH);
    wr_long(NAR);
    wr_long(BERUTHIEL);
    wr_long(GORLIM);
    wr_long(ELENDIL);
    wr_long(THORIN);
    wr_long(CELEBORN);
    wr_long(THRAIN);
    wr_long(GONDOR);
    wr_long(THINGOL);
    wr_long(THORONGIL);
    wr_long(LUTHIEN);
    wr_long(TUOR);
    wr_long(ROHAN);
    wr_long(TULKAS);
    wr_long(NECKLACE);
    wr_long(BARAHIR);
    wr_long(RAZORBACK);
    wr_long(BLADETURNER);

    for (i = 0; i < MAX_QUESTS; i++)
	wr_long(quests[i]);

    for (i = 0; i < MAX_R_IDX; i++)
	wr_unique(&u_list[i]);

    for (i = 0; i < MAX_R_IDX; i++) {
	r_ptr = &c_recall[i];
	if (r_ptr->r_cmove || r_ptr->r_cdefense || r_ptr->r_kills ||
	    r_ptr->r_spells2 || r_ptr->r_spells3 || r_ptr->r_spells ||
	    r_ptr->r_deaths || r_ptr->r_attacks[0] || r_ptr->r_attacks[1] ||
	    r_ptr->r_attacks[2] || r_ptr->r_attacks[3]) {
	    wr_short((u16b) i);
	    wr_long(r_ptr->r_cmove);
	    wr_long(r_ptr->r_spells);
	    wr_long(r_ptr->r_spells2);
	    wr_long(r_ptr->r_spells3);
	    wr_short(r_ptr->r_kills);
	    wr_short(r_ptr->r_deaths);
	    wr_long(r_ptr->r_cdefense);
	    wr_byte(r_ptr->r_wake);
	    wr_byte(r_ptr->r_ignore);
	    wr_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
	}
    }
    wr_short((u16b) 0xFFFF);	   /* sentinel to indicate no more monster info */

    wr_short((u16b) log_index);
    wr_long(l);
    wr_long(l);	/* added some duplicates, for future flags expansion -CWS */
    wr_long(l);
    wr_long(l);

    m_ptr = &py.misc;
    wr_string(m_ptr->name);
    wr_byte(m_ptr->male);
    wr_long((u32b) m_ptr->au);
    wr_long((u32b) m_ptr->max_exp);
    wr_long((u32b) m_ptr->exp);
    wr_short(m_ptr->exp_frac);
    wr_short(m_ptr->age);
    wr_short(m_ptr->ht);
    wr_short(m_ptr->wt);
    wr_short(m_ptr->lev);
    wr_short(m_ptr->max_dlv);
    wr_short((u16b) m_ptr->srh);
    wr_short((u16b) m_ptr->fos);
    wr_short((u16b) m_ptr->bth);
    wr_short((u16b) m_ptr->bthb);
    wr_short((u16b) m_ptr->mana);
    wr_short((u16b) m_ptr->mhp);
    wr_short((u16b) m_ptr->ptohit);
    wr_short((u16b) m_ptr->ptodam);
    wr_short((u16b) m_ptr->pac);
    wr_short((u16b) m_ptr->ptoac);
    wr_short((u16b) m_ptr->dis_th);
    wr_short((u16b) m_ptr->dis_td);
    wr_short((u16b) m_ptr->dis_ac);
    wr_short((u16b) m_ptr->dis_tac);
    wr_short((u16b) m_ptr->disarm);
    wr_short((u16b) m_ptr->save);
    wr_short((u16b) m_ptr->sc);
    wr_short((u16b) m_ptr->stl);
    wr_byte(m_ptr->pclass);
    wr_byte(m_ptr->prace);
    wr_byte(m_ptr->hitdie);
    wr_byte(m_ptr->expfact);
    wr_short((u16b) m_ptr->cmana);
    wr_short(m_ptr->cmana_frac);
    wr_short((u16b) m_ptr->chp);
    wr_short(m_ptr->chp_frac);
    for (i = 0; i < 4; i++)
	wr_string(m_ptr->history[i]);

    s_ptr = &py.stats;
    wr_shorts(s_ptr->max_stat, 6);
    wr_bytes(s_ptr->cur_stat, 6);               /* Was wr_shorts -TL */
    wr_shorts((u16b *) s_ptr->mod_stat, 6);
    wr_shorts(s_ptr->use_stat, 6);

    f_ptr = &py.flags;
    wr_long(f_ptr->status);
    wr_short((u16b) f_ptr->rest);
    wr_short((u16b) f_ptr->blind);
    wr_short((u16b) f_ptr->paralysis);
    wr_short((u16b) f_ptr->confused);
    wr_short((u16b) f_ptr->food);
    wr_short((u16b) f_ptr->food_digested);
    wr_short((u16b) f_ptr->protection);
    wr_short((u16b) f_ptr->speed);
    wr_short((u16b) f_ptr->fast);
    wr_short((u16b) f_ptr->slow);
    wr_short((u16b) f_ptr->afraid);
    wr_short((u16b) f_ptr->cut);
    wr_short((u16b) f_ptr->stun);
    wr_short((u16b) f_ptr->poisoned);
    wr_short((u16b) f_ptr->image);
    wr_short((u16b) f_ptr->protevil);
    wr_short((u16b) f_ptr->invuln);
    wr_short((u16b) f_ptr->hero);
    wr_short((u16b) f_ptr->shero);
    wr_short((u16b) f_ptr->shield);
    wr_short((u16b) f_ptr->blessed);
    wr_short((u16b) f_ptr->oppose_fire);
    wr_short((u16b) f_ptr->oppose_cold);
    wr_short((u16b) f_ptr->oppose_acid);
    wr_short((u16b) f_ptr->oppose_elec);
    wr_short((u16b) f_ptr->oppose_pois);
    wr_short((u16b) f_ptr->detect_inv);
    wr_short((u16b) f_ptr->word_recall);
    wr_short((u16b) f_ptr->see_infra);
    wr_short((u16b) f_ptr->tim_infra);
    wr_byte(f_ptr->see_inv);
    wr_byte(f_ptr->teleport);
    wr_byte(f_ptr->free_act);
    wr_byte(f_ptr->slow_digest);
    wr_byte(f_ptr->aggravate);
    wr_byte(f_ptr->resist_fire);
    wr_byte(f_ptr->resist_cold);
    wr_byte(f_ptr->resist_acid);
    wr_byte(f_ptr->regenerate);
    wr_byte(f_ptr->resist_elec);
    wr_byte(f_ptr->ffall);
    wr_byte(f_ptr->sustain_str);
    wr_byte(f_ptr->sustain_int);
    wr_byte(f_ptr->sustain_wis);
    wr_byte(f_ptr->sustain_con);
    wr_byte(f_ptr->sustain_dex);
    wr_byte(f_ptr->sustain_chr);
    wr_byte(f_ptr->confuse_monster);
    wr_byte(f_ptr->new_spells);
    wr_byte(f_ptr->resist_pois);
    wr_byte(f_ptr->hold_life);
    wr_byte(f_ptr->telepathy);
    wr_byte(f_ptr->immune_fire);
    wr_byte(f_ptr->immune_acid);
    wr_byte(f_ptr->immune_pois);
    wr_byte(f_ptr->immune_cold);
    wr_byte(f_ptr->immune_elec);
    wr_byte(f_ptr->light);
    wr_byte(f_ptr->resist_conf);
    wr_byte(f_ptr->resist_sound);
    wr_byte(f_ptr->resist_lite);
    wr_byte(f_ptr->resist_dark);
    wr_byte(f_ptr->resist_chaos);
    wr_byte(f_ptr->resist_disen);
    wr_byte(f_ptr->resist_shards);
    wr_byte(f_ptr->resist_nexus);
    wr_byte(f_ptr->resist_blind);
    wr_byte(f_ptr->resist_nether);
    wr_byte(f_ptr->resist_fear); /* added -CWS */

    wr_short((u16b) missile_ctr);
    wr_long((u32b) turn);
    wr_long((u32b) old_turn);	/* added -CWS */
    wr_short((u16b) inven_ctr);
    for (i = 0; i < inven_ctr; i++)
	wr_item(&inventory[i]);
    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
	wr_item(&inventory[i]);
    wr_short((u16b) inven_weight);
    wr_short((u16b) equip_ctr);
    wr_long(spell_learned);
    wr_long(spell_worked);
    wr_long(spell_forgotten);
    wr_long(spell_learned2);
    wr_long(spell_worked2);
    wr_long(spell_forgotten2);
    wr_bytes(spell_order, 64);
    wr_bytes(object_ident, OBJECT_IDENT_SIZE);
    wr_long(randes_seed);
    wr_long(town_seed);
    wr_short((u16b) last_msg);
    for (i = 0; i < MAX_SAVE_MSG; i++)
	wr_string(old_msg[i]);

/* this indicates 'cheating' if it is a one */
    wr_short((u16b) panic_save);
    wr_short((u16b) total_winner);
    wr_short((u16b) noscore);
    wr_shorts(player_hp, MAX_PLAYER_LEVEL);


    for (i = 0; i < MAX_STORES; i++) {
	st_ptr = &store[i];
	wr_long((u32b) st_ptr->store_open);
	wr_short((u16b) st_ptr->insult_cur);
	wr_byte(st_ptr->owner);
	wr_byte(st_ptr->store_ctr);
	wr_short(st_ptr->good_buy);
	wr_short(st_ptr->bad_buy);

	for (j = 0; j < st_ptr->store_ctr; j++) {
	    wr_long((u32b) st_ptr->store_inven[j].scost);
	    wr_item(&st_ptr->store_inven[j].sitem);
	}
    }

/* save the current time in the savefile */
    l = time((long *)0);

/* if (l < start_time) { someone is messing with the clock!, assume that we
 * have been playing for 1 day l = start_time + 86400L; } 
 */
    wr_long(l);

/* starting with 5.2, put died_from string in savefile */
    wr_string(died_from);

/* only level specific info follows, this allows characters to be
 * resurrected, the dungeon level info is not needed for a resurrection 
 */
    if (death) {
	if (ferror(fileptr) || fflush(fileptr) == EOF)
	    return FALSE;
	return TRUE;
    }
    wr_short((u16b) dun_level);
    wr_short((u16b) char_row);
    wr_short((u16b) char_col);
    wr_short((u16b) mon_tot_mult);
    wr_short((u16b) cur_height);
    wr_short((u16b) cur_width);
    wr_short((u16b) max_panel_rows);
    wr_short((u16b) max_panel_cols);

    for (i = 0; i < MAX_HEIGHT; i++)
	for (j = 0; j < MAX_WIDTH; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->cptr != 0) {
		wr_byte((byte) i);
		wr_byte((byte) j);
		wr_short((u16b) c_ptr->cptr); /* was wr_byte -CWS */
	    }
	}
    wr_byte((byte) 0xFF);	   /* marks end of cptr info */
    for (i = 0; i < MAX_HEIGHT; i++)
	for (j = 0; j < MAX_WIDTH; j++) {
	    c_ptr = &cave[i][j];
	    if (c_ptr->tptr != 0) {
		wr_byte((byte) i);
		wr_byte((byte) j);
		wr_short((u16b) c_ptr->tptr);
	    }
	}
    wr_byte(0xFF);		   /* marks end of tptr info */

/* must set counter to zero, note that code may write out two bytes
 * unnecessarily 
 */
    count = 0;
    prev_char = 0;
    for (i = 0; i < MAX_HEIGHT; i++)
	for (j = 0; j < MAX_WIDTH; j++) {
	    c_ptr = &cave[i][j];
	    char_tmp = c_ptr->fval | (c_ptr->lr << 4) | (c_ptr->fm << 5) |
		(c_ptr->pl << 6) | (c_ptr->tl << 7);
	    if (char_tmp != prev_char || count == MAX_UCHAR) {
		wr_byte((byte) count);
		wr_byte(prev_char);
		prev_char = char_tmp;
		count = 1;
	    } else
		count++;
	}
/* save last entry */
    wr_byte((byte) count);
    wr_byte(prev_char);

#ifdef MSDOS

/* must change graphics symbols for walls and floors back to default chars,
 * this is necessary so that if the user changes the graphics line, the
 * program will be able change all existing walls/floors to the new symbol 
 */
    t_ptr = &i_list[tcptr - 1];
    for (i = tcptr - 1; i >= MIN_TRIX; i--) {
	if (t_ptr->tchar == wallsym)
	    t_ptr->tchar = '#';
	t_ptr--;
    }
#endif
    wr_short((u16b) tcptr);
    for (i = MIN_TRIX; i < tcptr; i++)
	wr_item(&i_list[i]);
    wr_short((u16b) mfptr);
    for (i = MIN_M_IDX; i < mfptr; i++)
	wr_monster(&m_list[i]);

/* Save ghost names & stats etc... */
     if (!c_list[MAX_R_IDX - 1].name) {
                                  /* Can't dereference NULL! */
	 c_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	 bzero((char *)c_list[MAX_R_IDX - 1].name, 101);
     }
    wr_bytes(c_list[MAX_R_IDX - 1].name, 100);
    wr_long((u32b) c_list[MAX_R_IDX - 1].cmove);
    wr_long((u32b) c_list[MAX_R_IDX - 1].spells);
    wr_long((u32b) c_list[MAX_R_IDX - 1].cdefense);
    {
	u16b temp;
/* fix player ghost's exp bug.  The mexp field is really an u32b, but the
 * savefile was writing/reading an u16b.  Since I don't want to change
 * the savefile format, this insures that the low bits of mexp are written (No
 * ghost should be worth more than 64K (Melkor is only worth 60k!), but we
 * check anyway).  Using temp insures that the low bits are all written, and works
 * perfectly with a similar fix when* loading a character. -CFT
 */

	if (c_list[MAX_R_IDX - 1].mexp > (u32b) 0xff00)
	    temp = (u16b) 0xff00;
	else
	    temp = (u16b) c_list[MAX_R_IDX - 1].mexp;
	wr_short((u16b) temp);
    }
    wr_short((byte) c_list[MAX_R_IDX - 1].sleep);
    wr_byte((byte) c_list[MAX_R_IDX - 1].aaf);
    wr_short((byte) c_list[MAX_R_IDX - 1].ac);
    wr_byte((byte) c_list[MAX_R_IDX - 1].speed);
    wr_byte((byte) c_list[MAX_R_IDX - 1].cchar);
    wr_bytes(c_list[MAX_R_IDX - 1].hd, 2);
    wr_bytes(c_list[MAX_R_IDX - 1].damage, sizeof(attid) * 4);
    wr_short((u16b) c_list[MAX_R_IDX - 1].level);

    if (ferror(fileptr) || (fflush(fileptr) == EOF))
	return FALSE;
    return TRUE;
}

int 
save_player()
{
    vtype temp;
    char *tmp2;

#ifdef SECURE
    beGames();
#endif
    if (death && NO_SAVE)
	return TRUE;

    if (_save_player(savefile)) {

	tmp2 = basename(savefile);

	(void)sprintf(temp, "%s/p.%s", ANGBAND_DIR_SAVE, (tmp2 + 1));

	unlink(temp);
    } else {
	return FALSE;
    }
#ifdef SECURE
    bePlayer();
#endif
    return TRUE;
}

int 
_save_player(fnam)
char *fnam;
{
    vtype temp;
    int   ok, fd;
    byte char_tmp;

    if (log_index < 0)
	return TRUE;		   /* Nothing to save. */

    signals_ignore_tstp();
    put_qio();
    disturb(1, 0);		   /* Turn off resting and searching. */
    change_speed(-pack_heavy);	   /* Fix the speed */
    pack_heavy = 0;
    ok = FALSE;
#ifndef ATARIST_MWC
    fd = (-1);
    fileptr = NULL;		   /* Do not assume it has been init'ed */
#ifdef SET_UID
    fd = my_topen(fnam, O_RDWR | O_CREAT | O_EXCL, 0600);
#else
    fd = my_topen(fnam, O_RDWR | O_CREAT | O_EXCL, 0666);
#endif
    if (fd < 0 && access(fnam, 0) >= 0 &&
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
	(void)close(fd);
#endif				   /* !ATARIST_MWC */
    /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARIST_MWC) || defined(MSDOS) || defined(__MINT__)
	fileptr = my_tfopen(savefile, "wb");
#else
	fileptr = my_tfopen(savefile, "w");
#endif
#ifndef ATARIST_MWC
    }
#endif
    if (fileptr != NULL) {
#ifdef MSDOS
	(void)setmode(fileno(fileptr), O_BINARY);
#endif
	xor_byte = 0;
	wr_byte((byte) CUR_VERSION_MAJ);
	xor_byte = 0;
	wr_byte((byte) CUR_VERSION_MIN);
	xor_byte = 0;
	wr_byte((byte) PATCH_LEVEL);
	xor_byte = 0;
	char_tmp = randint(256) - 1;
	wr_byte(char_tmp);
    /* Note that xor_byte is now equal to char_tmp */

	ok = sv_write();
	if (fclose(fileptr) == EOF)
	    ok = FALSE;
    }
    if (!ok) {
	if (fd >= 0)
	    (void)unlink(fnam);
	signals();
	if (fd >= 0)
	    (void)sprintf(temp, "Error writing to savefile");
	else
	/* here? */
	    (void)sprintf(temp, "Can't create new savefile");
	msg_print(temp);
	return FALSE;
    } else
	character_saved = 1;

    turn = (-1);
    log_index = (-1);

    signals();

    return TRUE;
}


/* Certain checks are ommitted for the wizard. -CJS- */

int 
load_player(generate)
int *generate;
{
    int                    i, j, fd, c, ok, total_count;
    u32b                 l, age, time_saved;
    vtype                  temp;
    u16b                 u16b_tmp;
    register cave_type    *c_ptr;
    register monster_lore  *r_ptr;
    struct misc           *m_ptr;
    struct stats          *s_ptr;
    register struct flags *f_ptr;
    store_type            *st_ptr;
    byte char_tmp, ychar, xchar, count;

    free_turn_flag = TRUE;	   /* So a feeling isn't generated upon reloading -DGK */
    signals_ignore_tstp();
    *generate = TRUE;
    fd = (-1);

    if (access(savefile, 0) < 0) {
	signals();
	msg_print("Savefile does not exist.");
	return FALSE;
    }
    clear_screen();

    (void)sprintf(temp, "Restoring Character.");
    put_str(temp, 23, 0);
    sleep(1);

    if (turn >= 0)
	msg_print("IMPOSSIBLE! Attempt to restore while still alive!");

/* Allow restoring a file belonging to someone else - if we can delete it. */
/* Hence first try to read without doing a chmod. */

    else if ((fd = my_topen(savefile, O_RDONLY, 0)) < 0)
	msg_print("Can't open file for reading.");
    else {
#ifndef SET_UID
	struct stat         statbuf;

#endif
	turn = (-1);
	log_index = (-1);
	ok = TRUE;

#ifndef SET_UID
	(void)fstat(fd, &statbuf);
#endif
	(void)close(fd);
    /* GCC for atari st defines atarist */
#if defined(__MINT__) || defined(atarist) || defined(ATARIST_MWC) || defined(MSDOS)
	fileptr = my_tfopen(savefile, "rb");
#else
	fileptr = my_tfopen(savefile, "r");
#endif
	if (fileptr == NULL)
	    goto error;

	prt("Restoring Memory...", 0, 0);
	put_qio();

	xor_byte = 0;
	rd_byte(&version_maj);
	xor_byte = 0;
	rd_byte(&version_min);
	xor_byte = 0;
	rd_byte(&patch_level);
	xor_byte = 0;
	rd_byte(&xor_byte);

/* This boneheadedness is a direct result of the fact that Angband 2.4
 * had version constants of 5.2, not 2.4.  2.5 inherited this.  2.6 fixes
 * the problem.  Note that there must never be a 5.2.x version of Angband,
 * or else this code will get confused. -CWS
 */
	if ((version_maj == 5) && (version_min == 2)) {
	  version_maj = 2;
	  version_min = 5;
	}

	if ((version_maj != CUR_VERSION_MAJ)
	    || (version_min > CUR_VERSION_MIN)
	    || (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL)) {
	    prt("Sorry. This savefile is from a different version of Angband.",
		2, 0);
	    goto error;
	}
	put_qio();
	rd_long(&GROND);
	rd_long(&RINGIL);
	rd_long(&AEGLOS);
	rd_long(&ARUNRUTH);
	rd_long(&MORMEGIL);
	rd_long(&ANGRIST);
	rd_long(&GURTHANG);
	rd_long(&CALRIS);
	rd_long(&ANDURIL);
	rd_long(&STING);
	rd_long(&ORCRIST);
	rd_long(&GLAMDRING);
	rd_long(&DURIN);
	rd_long(&AULE);
	rd_long(&THUNDERFIST);
	rd_long(&BLOODSPIKE);
	rd_long(&DOOMCALLER);
	rd_long(&NARTHANC);
	rd_long(&NIMTHANC);
	rd_long(&DETHANC);
	rd_long(&GILETTAR);
	rd_long(&RILIA);
	rd_long(&BELANGIL);
	rd_long(&BALLI);
	rd_long(&LOTHARANG);
	rd_long(&FIRESTAR);
	rd_long(&ERIRIL);
	rd_long(&CUBRAGOL);
	rd_long(&BARD);
	rd_long(&COLLUIN);
	rd_long(&HOLCOLLETH);
	rd_long(&TOTILA);
	rd_long(&PAIN);
	rd_long(&ELVAGIL);
	rd_long(&AGLARANG);
	rd_long(&EORLINGAS);
	rd_long(&BARUKKHELED);
	rd_long(&WRATH);
	rd_long(&HARADEKKET);
	rd_long(&MUNDWINE);
	rd_long(&GONDRICAM);
	rd_long(&ZARCUTHRA);
	rd_long(&CARETH);
	rd_long(&FORASGIL);
	rd_long(&CRISDURIAN);
	rd_long(&COLANNON);
	rd_long(&HITHLOMIR);
	rd_long(&THALKETTOTH);
	rd_long(&ARVEDUI);
	rd_long(&THRANDUIL);
	rd_long(&THENGEL);
	rd_long(&HAMMERHAND);
	rd_long(&CELEGORM);
	rd_long(&THROR);
	rd_long(&MAEDHROS);
	rd_long(&OLORIN);
	rd_long(&ANGUIREL);
	rd_long(&OROME);
	rd_long(&EONWE);
	rd_long(&THEODEN);
	rd_long(&ULMO);
	rd_long(&OSONDIR);
	rd_long(&TURMIL);
	rd_long(&CASPANION);
	rd_long(&TIL);
	rd_long(&DEATHWREAKER);
	rd_long(&AVAVIR);
	rd_long(&TARATOL);
	if (to_be_wizard)
	    prt("Loaded Weapon Artifacts", 2, 0);
	put_qio();


	rd_long(&DOR_LOMIN);
	rd_long(&NENYA);
	rd_long(&NARYA);
	rd_long(&VILYA);
	rd_long(&BELEGENNON);
	rd_long(&FEANOR);
	rd_long(&ISILDUR);
	rd_long(&SOULKEEPER);
	rd_long(&FINGOLFIN);
	rd_long(&ANARION);
	rd_long(&POWER);
	rd_long(&PHIAL);
	rd_long(&BELEG);
	rd_long(&DAL);
	rd_long(&PAURHACH);
	rd_long(&PAURNIMMEN);
	rd_long(&PAURAEGEN);
	rd_long(&PAURNEN);
	rd_long(&CAMMITHRIM);
	rd_long(&CAMBELEG);
	rd_long(&INGWE);
	rd_long(&CARLAMMAS);
	rd_long(&HOLHENNETH);
	rd_long(&AEGLIN);
	rd_long(&CAMLOST);
	rd_long(&NIMLOTH);
	rd_long(&NAR);
	rd_long(&BERUTHIEL);
	rd_long(&GORLIM);
	rd_long(&ELENDIL);
	rd_long(&THORIN);
	rd_long(&CELEBORN);
	rd_long(&THRAIN);
	rd_long(&GONDOR);
	rd_long(&THINGOL);
	rd_long(&THORONGIL);
	rd_long(&LUTHIEN);
	rd_long(&TUOR);
	rd_long(&ROHAN);
	rd_long(&TULKAS);
	rd_long(&NECKLACE);
	rd_long(&BARAHIR);
	rd_long(&RAZORBACK);
	rd_long(&BLADETURNER);
	if (to_be_wizard)
	    prt("Loaded Armour Artifacts", 3, 0);
	put_qio();

	for (i = 0; i < MAX_QUESTS; i++)
	    rd_long(&quests[i]);
	if (to_be_wizard)
	    prt("Loaded Quests", 4, 0);

	for (i = 0; i < MAX_R_IDX; i++)
	    rd_unique(&u_list[i]);
	if (to_be_wizard)
	    prt("Loaded Unique Beasts", 5, 0);
	put_qio();

	rd_short(&u16b_tmp);
	while (u16b_tmp != 0xFFFF) {
	    if (u16b_tmp >= MAX_R_IDX)
		goto error;
	    r_ptr = &c_recall[u16b_tmp];
	    rd_long(&r_ptr->r_cmove);
	    rd_long(&r_ptr->r_spells);
	    rd_long(&r_ptr->r_spells2);
	    rd_long(&r_ptr->r_spells3);
	    rd_short(&r_ptr->r_kills);
	    rd_short(&r_ptr->r_deaths);
	    rd_long(&r_ptr->r_cdefense);
	    rd_byte(&r_ptr->r_wake);
	    rd_byte(&r_ptr->r_ignore);
	    rd_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
	    rd_short(&u16b_tmp);
	}
	if (to_be_wizard)
	    prt("Loaded Recall Memory", 6, 0);
	put_qio();
	rd_short((u16b *) & log_index);
        rd_long(&l);
	if ((version_maj >= 2) && (version_min >= 6)) {
	  rd_long(&l);
	  rd_long(&l);
	  rd_long(&l);
	}

	if (to_be_wizard)
	    prt("Loaded Options Memory", 7, 0);
	put_qio();

	if (l & 1)
	    find_cut = TRUE;
	else
	    find_cut = FALSE;
	if (l & 2)
	    find_examine = TRUE;
	else
	    find_examine = FALSE;
	if (l & 4)
	    find_prself = TRUE;
	else
	    find_prself = FALSE;
	if (l & 8)
	    find_bound = TRUE;
	else
	    find_bound = FALSE;
	if (l & 16)
	    prompt_carry_flag = TRUE;
	else
	    prompt_carry_flag = FALSE;
	if (l & 32)
	    rogue_like_commands = TRUE;
	else
	    rogue_like_commands = FALSE;
	if (l & 64)
	    show_inven_weight = TRUE;
	else
	    show_inven_weight = FALSE;
	if (l & 128)
	    notice_seams = TRUE;
	else
	    notice_seams = FALSE;
	if (l & 0x100L)
	    find_ignore_doors = TRUE;
	else
	    find_ignore_doors = FALSE;
	
	if (l & 0x200L)
	    no_haggle_flag = TRUE;
	else
	    no_haggle_flag = FALSE; 
	if (l & 0x400L)
	    carry_query_flag = FALSE;
	else
	    carry_query_flag = TRUE;
	if (l & 0x1000L)
	    unfelt = TRUE;
	else
	    unfelt = FALSE;
	delay_spd = ((l >> 13) & 0xf);
	if (delay_spd > 10)
	    delay_spd = 10;	   /* bounds check for delay_speed -CWS */
	if (delay_spd < 0)
	    delay_spd = 0;
	hitpoint_warn = ((l >> 17) & 0xf);
	if (l & 0x00400000L)	   /* don't do "black Mushroom of Curing" */
	    plain_descriptions = TRUE;
	else
	    plain_descriptions = FALSE;
	if (l & 0x00800000L)
	    show_equip_weight = TRUE;
	else
	    show_equip_weight = FALSE;
	feeling = ((l >> 24) & 0xf);
	if (feeling > 10)
	    feeling = 0;	    /* bounds for level feelings -CWS */
	if (feeling < 0)
	    feeling = 0;

	if (l & 0x20000000L)
	    equippy_chars = TRUE;   /* equippy chars option -CWS */
	else
	    equippy_chars = FALSE;

	if (l & 0x40000000L)
	    quick_messages = TRUE;  /* quick messages option -CWS */
	else
	    quick_messages = FALSE;

	if (to_be_wizard && (l & 0x80000000L)
	    && get_check("Resurrect a dead character?"))
	    l &= ~0x80000000L;
	if ((l & 0x80000000L) == 0) {
	    m_ptr = &py.misc;
	    rd_string(m_ptr->name);
	    rd_byte(&m_ptr->male);
	    rd_long((u32b *) & m_ptr->au);
	    rd_long((u32b *) & m_ptr->max_exp);
	    rd_long((u32b *) & m_ptr->exp);
	    rd_short(&m_ptr->exp_frac);
	    rd_short(&m_ptr->age);
	    rd_short(&m_ptr->ht);
	    rd_short(&m_ptr->wt);
	    rd_short(&m_ptr->lev);
	    rd_short(&m_ptr->max_dlv);
	    rd_short((u16b *) & m_ptr->srh);
	    rd_short((u16b *) & m_ptr->fos);
	    rd_short((u16b *) & m_ptr->bth);
	    rd_short((u16b *) & m_ptr->bthb);
	    rd_short((u16b *) & m_ptr->mana);
	    rd_short((u16b *) & m_ptr->mhp);
	    rd_short((u16b *) & m_ptr->ptohit);
	    rd_short((u16b *) & m_ptr->ptodam);
	    rd_short((u16b *) & m_ptr->pac);
	    rd_short((u16b *) & m_ptr->ptoac);
	    rd_short((u16b *) & m_ptr->dis_th);
	    rd_short((u16b *) & m_ptr->dis_td);
	    rd_short((u16b *) & m_ptr->dis_ac);
	    rd_short((u16b *) & m_ptr->dis_tac);
	    rd_short((u16b *) & m_ptr->disarm);
	    rd_short((u16b *) & m_ptr->save);
	    rd_short((u16b *) & m_ptr->sc);
	    rd_short((u16b *) & m_ptr->stl);
	    rd_byte(&m_ptr->pclass);
	    rd_byte(&m_ptr->prace);
	    rd_byte(&m_ptr->hitdie);
	    rd_byte(&m_ptr->expfact);
	    rd_short((u16b *) & m_ptr->cmana);
	    rd_short(&m_ptr->cmana_frac);
	    rd_short((u16b *) & m_ptr->chp);
	    rd_short(&m_ptr->chp_frac);
	    for (i = 0; i < 4; i++)
		rd_string(m_ptr->history[i]);

	    s_ptr = &py.stats;
	    rd_shorts(s_ptr->max_stat, 6);
	    if (version_maj <= 2 && version_min <=5 && patch_level <= 6)
		rd_shorts(s_ptr->cur_stat, 6);
	    else
		rd_bytes(s_ptr->cur_stat, 6);                   /* -TL */
	    rd_shorts((u16b *) s_ptr->mod_stat, 6);
	    rd_shorts(s_ptr->use_stat, 6);

	    f_ptr = &py.flags;
	    rd_long(&f_ptr->status);
	    rd_short((u16b *) & f_ptr->rest);
	    rd_short((u16b *) & f_ptr->blind);
	    rd_short((u16b *) & f_ptr->paralysis);
	    rd_short((u16b *) & f_ptr->confused);
	    rd_short((u16b *) & f_ptr->food);
	    rd_short((u16b *) & f_ptr->food_digested);
	    rd_short((u16b *) & f_ptr->protection);
	    rd_short((u16b *) & f_ptr->speed);
	    rd_short((u16b *) & f_ptr->fast);
	    rd_short((u16b *) & f_ptr->slow);
	    rd_short((u16b *) & f_ptr->afraid);
	    rd_short((u16b *) & f_ptr->cut);
	    rd_short((u16b *) & f_ptr->stun);
	    rd_short((u16b *) & f_ptr->poisoned);
	    rd_short((u16b *) & f_ptr->image);
	    rd_short((u16b *) & f_ptr->protevil);
	    rd_short((u16b *) & f_ptr->invuln);
	    rd_short((u16b *) & f_ptr->hero);
	    rd_short((u16b *) & f_ptr->shero);
	    rd_short((u16b *) & f_ptr->shield);
	    rd_short((u16b *) & f_ptr->blessed);
	    rd_short((u16b *) & f_ptr->oppose_fire);
	    rd_short((u16b *) & f_ptr->oppose_cold);
	    rd_short((u16b *) & f_ptr->oppose_acid);
	    rd_short((u16b *) & f_ptr->oppose_elec);
	    rd_short((u16b *) & f_ptr->oppose_pois);
	    rd_short((u16b *) & f_ptr->detect_inv);
	    rd_short((u16b *) & f_ptr->word_recall);
	    rd_short((u16b *) & f_ptr->see_infra);
	    rd_short((u16b *) & f_ptr->tim_infra);
	    rd_byte(&f_ptr->see_inv);
	    rd_byte(&f_ptr->teleport);
	    rd_byte(&f_ptr->free_act);
	    rd_byte(&f_ptr->slow_digest);
	    rd_byte(&f_ptr->aggravate);
	    rd_byte(&f_ptr->resist_fire);
	    rd_byte(&f_ptr->resist_cold);
	    rd_byte(&f_ptr->resist_acid);
	    rd_byte(&f_ptr->regenerate);
	    rd_byte(&f_ptr->resist_elec);
	    rd_byte(&f_ptr->ffall);
	    rd_byte(&f_ptr->sustain_str);
	    rd_byte(&f_ptr->sustain_int);
	    rd_byte(&f_ptr->sustain_wis);
	    rd_byte(&f_ptr->sustain_con);
	    rd_byte(&f_ptr->sustain_dex);
	    rd_byte(&f_ptr->sustain_chr);
	    rd_byte(&f_ptr->confuse_monster);
	    rd_byte(&f_ptr->new_spells);
	    rd_byte(&f_ptr->resist_pois);
	    rd_byte(&f_ptr->hold_life);
	    rd_byte(&f_ptr->telepathy);
	    rd_byte(&f_ptr->immune_fire);
	    rd_byte(&f_ptr->immune_acid);
	    rd_byte(&f_ptr->immune_pois);
	    rd_byte(&f_ptr->immune_cold);
	    rd_byte(&f_ptr->immune_elec);
	    rd_byte(&f_ptr->light);
	    rd_byte(&f_ptr->resist_conf);
	    rd_byte(&f_ptr->resist_sound);
	    rd_byte(&f_ptr->resist_lite);
	    rd_byte(&f_ptr->resist_dark);
	    rd_byte(&f_ptr->resist_chaos);
	    rd_byte(&f_ptr->resist_disen);
	    rd_byte(&f_ptr->resist_shards);
	    rd_byte(&f_ptr->resist_nexus);
	    rd_byte(&f_ptr->resist_blind);
	    rd_byte(&f_ptr->resist_nether);
	    if ((version_maj >= 2) && (version_min >= 6))
		rd_byte(&f_ptr->resist_fear);
	    else
		f_ptr->resist_fear = 0;	/* sigh */

	    rd_short((u16b *) & missile_ctr);
	    rd_long((u32b *) & turn);
	    if ((version_maj >= 2) && (version_min >= 6))
	      rd_long((u32b *) & old_turn);
	    else
	      old_turn = turn;	/* best we can do... -CWS */

	    rd_short((u16b *) & inven_ctr);
	    if (inven_ctr > INVEN_WIELD) {
		prt("ERROR in inven_ctr", 8, 0);
		goto error;
	    }
	    for (i = 0; i < inven_ctr; i++)
		rd_item(&inventory[i]);
	    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
		rd_item(&inventory[i]);
	    rd_short((u16b *) & inven_weight);
	    rd_short((u16b *) & equip_ctr);
	    rd_long(&spell_learned);
	    rd_long(&spell_worked);
	    rd_long(&spell_forgotten);
	    rd_long(&spell_learned2);
	    rd_long(&spell_worked2);
	    rd_long(&spell_forgotten2);
	    rd_bytes(spell_order, 64);
	    rd_bytes(object_ident, OBJECT_IDENT_SIZE);
	    rd_long(&randes_seed);
	    rd_long(&town_seed);
	    rd_short((u16b *) & last_msg);
	    for (i = 0; i < MAX_SAVE_MSG; i++)
		rd_string(old_msg[i]);

	    rd_short((u16b *) & panic_save);
	    rd_short((u16b *) & total_winner);
	    rd_short((u16b *) & noscore);
	    rd_shorts(player_hp, MAX_PLAYER_LEVEL);

	    for (i = 0; i < MAX_STORES; i++) {
	      st_ptr = &store[i];
	      rd_long((u32b *) & st_ptr->store_open);
	      rd_short((u16b *) & st_ptr->insult_cur);
	      rd_byte(&st_ptr->owner);
	      rd_byte(&st_ptr->store_ctr);
	      rd_short(&st_ptr->good_buy);
	      rd_short(&st_ptr->bad_buy);
	      if (st_ptr->store_ctr > STORE_INVEN_MAX) {
		prt("ERROR in store_ctr", 9, 0);
		goto error;
	      }
	      for (j = 0; j < st_ptr->store_ctr; j++) {
		rd_long((u32b *) & st_ptr->store_inven[j].scost);
		rd_item(&st_ptr->store_inven[j].sitem);
	      }
	    }

	    rd_long(&time_saved);
#ifndef SET_UID
#ifndef ALLOW_FIDDLING
		if (!to_be_wizard) {
		    if (time_saved > (statbuf.st_ctime + 100) ||
			time_saved < (statbuf.st_ctime - 100)) {
			prt("Fiddled save file", 10, 0);
			goto error;
		    }
		}
#endif
#endif
	    rd_string(died_from);
	}
	if ((c = getc(fileptr)) == EOF || (l & 0x80000000L)) {
	    if ((l & 0x80000000L) == 0) {
		if (!to_be_wizard || turn < 0) {
		    prt("ERROR in to_be_wizard", 10, 0);
		    goto error;
		}
		prt("Attempting a resurrection!", 0, 0);
		if (py.misc.chp < 0) {
		    py.misc.chp = 0;
		    py.misc.chp_frac = 0;
		}
	    /* don't let him starve to death immediately */
		if (py.flags.food < 5000)
		    py.flags.food = 5000;
		cure_poison();
		cure_blindness();
		cure_confusion();
		remove_fear();
		if (py.flags.image > 0)
		    py.flags.image = 0;
		if (py.flags.cut > 0)
		    py.flags.cut = 0;
		if (py.flags.stun > 0) {
		    if (py.flags.stun > 50) {
			py.misc.ptohit += 20;
			py.misc.ptodam += 20;
		    } else {
			py.misc.ptohit += 5;
			py.misc.ptodam += 5;
		    }
		    py.flags.stun = 0;
		}
		if (py.flags.word_recall > 0)
		    py.flags.word_recall = 0;
		dun_level = 0;	   /* Resurrect on the town level. */
		character_generated = 1;

	    /* set noscore to indicate a resurrection, and don't enter wizard mode */
		to_be_wizard = FALSE;
		noscore |= 0x1;
	    } else {
		prt("Restoring Memory of a departed spirit...", 0, 0);
		turn = (-1);
		old_turn = (-1);
	    }
	    put_qio();
	/* The log_index of the previous incarnation is here if later version
	 * want to use it. For now, throw it away and get a new log. 
	 */
	    log_index = (-1);
	    goto closefiles;
	}
	if (ungetc(c, fileptr) == EOF) {
	    prt("ERROR in ungetc", 11, 0);
	    goto error;
	}
	prt("Restoring Character...", 0, 0);
	put_qio();

    /* only level specific info should follow, not present for dead characters */

	rd_short((u16b *) & dun_level);
	rd_short((u16b *) & char_row);
	rd_short((u16b *) & char_col);
	rd_short((u16b *) & mon_tot_mult);
	rd_short((u16b *) & cur_height);
	rd_short((u16b *) & cur_width);
	rd_short((u16b *) & max_panel_rows);
	rd_short((u16b *) & max_panel_cols);

    /* read in the creature ptr info */
	rd_byte(&char_tmp);
	while (char_tmp != 0xFF) {
	    ychar = char_tmp;
	    rd_byte(&xchar);

    /* let's correctly fix the invisible monster bug  -CWS */
	    if ((version_maj >= 2) && (version_min >= 6)) {
		rd_short((u16b *) & u16b_tmp);
		cave[ychar][xchar].cptr = u16b_tmp;
	    } else {
		rd_byte((byte *) & char_tmp);
		cave[ychar][xchar].cptr = char_tmp;
	    }
	    if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
		vtype               t1;

		sprintf(t1,
		      "Error in creature ptr info: x=%x, y=%x, char_tmp=%x",
			(unsigned) xchar, (unsigned) ychar, (unsigned) char_tmp);
		prt(t1, 11, 0);
	    }
	    rd_byte(&char_tmp);
	}
    /* read in the treasure ptr info */
	rd_byte(&char_tmp);
	while (char_tmp != 0xFF) {
	    ychar = char_tmp;
	    rd_byte(&xchar);
	    rd_short((u16b *) & u16b_tmp);
	    if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
		prt("Error in treasure pointer info", 12, 0);
		goto error;
	    }
	    cave[ychar][xchar].tptr = u16b_tmp;
	    rd_byte(&char_tmp);
	}
    /* read in the rest of the cave info */
	c_ptr = &cave[0][0];
	total_count = 0;
	while (total_count != MAX_HEIGHT * MAX_WIDTH) {
	    rd_byte(&count);
	    rd_byte(&char_tmp);
	    for (i = count; i > 0; i--) {
#ifndef ATARIST_MWC
		if (c_ptr >= &cave[MAX_HEIGHT][0]) {
		    prt("ERROR in cave size", 13, 0);
		    goto error;
		}
#endif
		c_ptr->fval = char_tmp & 0xF;
		c_ptr->lr = (char_tmp >> 4) & 0x1;
		c_ptr->fm = (char_tmp >> 5) & 0x1;
		c_ptr->pl = (char_tmp >> 6) & 0x1;
		c_ptr->tl = (char_tmp >> 7) & 0x1;
		c_ptr++;
	    }
	    total_count += count;
	}

	rd_short((u16b *) & tcptr);
	if (tcptr > MAX_TALLOC) {
	    prt("ERROR in MAX_TALLOC", 14, 0);
	    goto error;
	}
	for (i = MIN_TRIX; i < tcptr; i++)
	    rd_item(&i_list[i]);
	rd_short((u16b *) & mfptr);
	if (mfptr > MAX_M_IDX) {
	    prt("ERROR in MAX_M_IDX", 15, 0);
	    goto error;
	}
	for (i = MIN_M_IDX; i < mfptr; i++) {
	    rd_monster(&m_list[i]);
	}
#ifdef MSDOS
    /* change walls and floors to graphic symbols */
	t_ptr = &i_list[tcptr - 1];
	for (i = tcptr - 1; i >= MIN_TRIX; i--) {
	    if (t_ptr->tchar == '#')
		t_ptr->tchar = wallsym;
	    t_ptr--;
	}
#endif

				/* Restore ghost names & stats etc... */
				/* Allocate storage for name */
	c_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	bzero((char *)c_list[MAX_R_IDX - 1].name, 101);
	*((char *) c_list[MAX_R_IDX - 1].name) = 'A';
	rd_bytes((byte *) (c_list[MAX_R_IDX - 1].name), 100);
	rd_long((u32b *) & (c_list[MAX_R_IDX - 1].cmove));
	rd_long((u32b *) & (c_list[MAX_R_IDX - 1].spells));
	rd_long((u32b *) & (c_list[MAX_R_IDX - 1].cdefense));
	{
	    u16b t1;
/* fix player ghost's exp bug.  The mexp field is really an u32b, but the
 * savefile was writing/ reading an u16b.  Since I don't want to change
 * the savefile format, this insures that the mexp field is loaded, and that
 * the "high bits" of mexp do not contain garbage values which could mean that
 * player ghost are worth millions of exp. -CFT
 */

	    rd_short((u16b *) & t1);
	    c_list[MAX_R_IDX - 1].mexp = (u32b) t1;
	}

/* more stupid size bugs that would've never been needed if these variables
 * had been given enough space in the first place -CWS
 */
	if ((version_maj >= 2) && (version_min >= 6))
	    rd_short((u16b *) & (c_list[MAX_R_IDX - 1].sleep));
	else
	    rd_byte((byte *) & (c_list[MAX_R_IDX - 1].sleep));

	rd_byte((byte *) & (c_list[MAX_R_IDX - 1].aaf));

	if ((version_maj >= 2) && (version_min >= 6))
	    rd_short((u16b *) & (c_list[MAX_R_IDX - 1].ac));
	else
	    rd_byte((byte *) & (c_list[MAX_R_IDX - 1].ac));

	rd_byte((byte *) & (c_list[MAX_R_IDX - 1].speed));
	rd_byte((byte *) & (c_list[MAX_R_IDX - 1].cchar));

	rd_bytes((byte *) (c_list[MAX_R_IDX - 1].hd), 2);

	rd_bytes((byte *) (c_list[MAX_R_IDX - 1].damage), sizeof(attid) * 4);
	rd_short((u16b *) & (c_list[MAX_R_IDX - 1].level));
	*generate = FALSE;	   /* We have restored a cave - no need to generate. */

	if ((version_min == 1 && patch_level < 3)
	    || (version_min == 0))
	    for (i = 0; i < MAX_STORES; i++) {
		st_ptr = &store[i];
		rd_long((u32b *) & st_ptr->store_open);
		rd_short((u16b *) & st_ptr->insult_cur);
		rd_byte(&st_ptr->owner);
		rd_byte(&st_ptr->store_ctr);
		rd_short(&st_ptr->good_buy);
		rd_short(&st_ptr->bad_buy);
		if (st_ptr->store_ctr > STORE_INVEN_MAX) {
		    prt("ERROR in STORE_INVEN_MAX", 16, 0);
		    goto error;
		}
		for (j = 0; j < st_ptr->store_ctr; j++) {
		    rd_long((u32b *) & st_ptr->store_inven[j].scost);
		    rd_item(&st_ptr->store_inven[j].sitem);
		}
	    }

    /* read the time that the file was saved */
	rd_long(&time_saved);

	if (ferror(fileptr)) {
	    prt("FILE ERROR", 17, 0);
	    goto error;
	}
	if (turn < 0) {
	    prt("Error = turn < 0", 7, 0);
    error:
	    ok = FALSE;		   /* Assume bad data. */
	} else {
	/* don't overwrite the killed by string if character is dead */
	    if (py.misc.chp >= 0)
		(void)strcpy(died_from, "(alive and well)");
	    character_generated = 1;
	}

closefiles:

	if (fileptr != NULL) {
	    if (fclose(fileptr) < 0)
		ok = FALSE;
	}
	if (fd >= 0)
	    (void)close(fd);

	if (!ok)
	    msg_print("Error during reading of file.");
	else if (turn >= 0 && !_new_log())
	    msg_print("Can't log player in the log file.");
	else {
	/* let the user overwrite the old savefile when save/quit */
	    from_savefile = 1;

	    signals();

	    if (turn >= 0) {	   /* Only if a full restoration. */
		weapon_heavy = FALSE;
		pack_heavy = 0;
		check_strength();

	    /* rotate store inventory, depending on how old the save file */
	    /* is foreach day old (rounded up), call store_maint */
	    /* calculate age in seconds */
		start_time = time((long *)0);
	    /* check for reasonable values of time here ... */
		if (start_time < time_saved)
		    age = 0;
		else
		    age = start_time - time_saved;

		age = (age + 43200L) / 86400L;	/* age in days */
		if (age > 10)
		    age = 10;	   /* in case savefile is very old */
		for (i = 0; i < age; i++)
		    store_maint();
	    }

/* if (noscore) msg_print("This save file cannot be used to get on the score board."); */

	    if (version_maj != CUR_VERSION_MAJ
		|| version_min != CUR_VERSION_MIN) {
		(void)sprintf(temp,
			"Save file version %d.%d %s on game version %d.%d.",
			      version_maj, version_min,
			      version_maj == CUR_VERSION_MAJ
			      ? "accepted" : "very risky",
			      CUR_VERSION_MAJ, CUR_VERSION_MIN);
		msg_print(temp);
	    }
	    if (turn >= 0) {
		char               *tmp2;

		tmp2 = basename(savefile);

		(void)sprintf(temp, "%s/p.%s", ANGBAND_DIR_SAVE, (tmp2 + 1));

		link(savefile, temp);
		unlink(savefile);
		return TRUE;
	    } else {
		return FALSE;	   /* Only restored options and monster memory. */
	    }
	}
    }
    turn = (-1);
    log_index = (-1);
    prt("Please try again without that savefile.", 1, 0);
    signals();
#ifdef MAC
    *exit_flag = TRUE;
#else
    exit_game();
#endif

    return FALSE;		   /* not reached, unless on mac */
}

static void 
wr_byte(c)
byte c;
{
    xor_byte ^= c;
    (void)putc((int)xor_byte, fileptr);
}

static void 
wr_short(s)
u16b s;
{
    xor_byte ^= (s & 0xFF);
    (void)putc((int)xor_byte, fileptr);
    xor_byte ^= ((s >> 8) & 0xFF);
    (void)putc((int)xor_byte, fileptr);
}

static void 
wr_long(l)
register u32b l;
{
    xor_byte ^= (l & 0xFF);
    (void)putc((int)xor_byte, fileptr);
    xor_byte ^= ((l >> 8) & 0xFF);
    (void)putc((int)xor_byte, fileptr);
    xor_byte ^= ((l >> 16) & 0xFF);
    (void)putc((int)xor_byte, fileptr);
    xor_byte ^= ((l >> 24) & 0xFF);
    (void)putc((int)xor_byte, fileptr);
}

static void 
wr_bytes(c, count)
byte       *c;
register int count;
{
    register int    i;
    register byte *ptr;

    ptr = c;
    for (i = 0; i < count; i++) {
	xor_byte ^= *ptr++;
	(void)putc((int)xor_byte, fileptr);
    }
}

static void 
wr_string(str)
register char *str;
{
    while (*str != '\0') {
	xor_byte ^= *str++;
	(void)putc((int)xor_byte, fileptr);
    }
    xor_byte ^= *str;
    (void)putc((int)xor_byte, fileptr);
}

static void 
wr_shorts(s, count)
u16b      *s;
register int count;
{
    register int        i;
    register u16b    *sptr;

    sptr = s;
    for (i = 0; i < count; i++) {
	xor_byte ^= (*sptr & 0xFF);
	(void)putc((int)xor_byte, fileptr);
	xor_byte ^= ((*sptr++ >> 8) & 0xFF);
	(void)putc((int)xor_byte, fileptr);
    }
}

static void 
wr_item(item)
register inven_type *item;
{
    wr_short(item->index);
    wr_byte(item->name2);
    wr_string(item->inscrip);
    wr_long(item->flags);
    wr_byte(item->tval);
    wr_byte(item->tchar);
    wr_short((u16b) item->p1);
    wr_long((u32b) item->cost);
    wr_byte(item->sval);
    wr_byte(item->number);
    wr_short(item->weight);
    wr_short((u16b) item->tohit);
    wr_short((u16b) item->todam);
    wr_short((u16b) item->ac);
    wr_short((u16b) item->toac);
    wr_bytes(item->damage, 2);
    wr_byte(item->level);
    wr_byte(item->ident);
    wr_long(item->flags2);
    wr_short((u16b) item->timeout);
}

static void 
wr_monster(mon)
register monster_type *mon;
{
    wr_short((u16b) mon->hp);
    wr_short((u16b) mon->maxhp); /* added -CWS */
    wr_short((u16b) mon->csleep);
    wr_short((u16b) mon->cspeed);
    wr_short(mon->mptr);
    wr_byte(mon->fy);
    wr_byte(mon->fx);
    wr_byte(mon->cdis);
    wr_byte(mon->ml);
    wr_byte(mon->stunned);
    wr_byte(mon->confused);
    wr_byte(mon->monfear);	/* added -CWS */
}

static void 
rd_byte(ptr)
byte *ptr;
{
    byte c;

    c = getc(fileptr) & 0xFF;
    *ptr = c ^ xor_byte;
    xor_byte = c;
}

static void 
rd_short(ptr)
u16b *ptr;
{
    byte  c;
    u16b s;

    c = (getc(fileptr) & 0xFF);
    s = c ^ xor_byte;
    xor_byte = (getc(fileptr) & 0xFF);
    s |= (u16b) (c ^ xor_byte) << 8;
    *ptr = s;
}

static void 
rd_long(ptr)
u32b *ptr;
{
    register u32b l;
    register byte  c;

    c = (getc(fileptr) & 0xFF);
    l = c ^ xor_byte;
    xor_byte = (getc(fileptr) & 0xFF);
    l |= (u32b) (c ^ xor_byte) << 8;
    c = (getc(fileptr) & 0xFF);
    l |= (u32b) (c ^ xor_byte) << 16;
    xor_byte = (getc(fileptr) & 0xFF);
    l |= (u32b) (c ^ xor_byte) << 24;
    *ptr = l;
}

static void 
rd_bytes(ptr, count)
byte *ptr;
int    count;
{
    int   i;
    byte c, nc;

    for (i = 0; i < count; i++) {
	c = (getc(fileptr) & 0xFF);
	nc = c ^ xor_byte;
	*ptr = nc;
	ptr++;
	xor_byte = c;
    }
}

static void 
rd_string(str)
char *str;
{
    register byte c;

    do {
	c = (getc(fileptr) & 0xFF);
	*str = c ^ xor_byte;
	xor_byte = c;
    }
    while (*str++ != '\0');
}

static void 
rd_shorts(ptr, count)
u16b      *ptr;
register int count;
{
    register int        i;
    register u16b    *sptr;
    register u16b     s;
    byte               c;

    sptr = ptr;
    for (i = 0; i < count; i++) {
	c = (getc(fileptr) & 0xFF);
	s = c ^ xor_byte;
	xor_byte = (getc(fileptr) & 0xFF);
	s |= (u16b) (c ^ xor_byte) << 8;
	*sptr++ = s;
    }
}

static void 
rd_item(item)
register inven_type *item;
{
    rd_short(&item->index);
    rd_byte(&item->name2);
    rd_string(item->inscrip);
    rd_long(&item->flags);
    rd_byte(&item->tval);
    rd_byte(&item->tchar);
    rd_short((u16b *) & item->p1);
    rd_long((u32b *) & item->cost);
    rd_byte(&item->sval);
    rd_byte(&item->number);
    rd_short(&item->weight);
    rd_short((u16b *) & item->tohit);
    rd_short((u16b *) & item->todam);
    rd_short((u16b *) & item->ac);
    rd_short((u16b *) & item->toac);
    rd_bytes(item->damage, 2);
    rd_byte(&item->level);
    rd_byte(&item->ident);
    rd_long(&item->flags2);
    rd_short((u16b *) & item->timeout);
}

static void 
rd_monster(mon)
register monster_type *mon;
{
    rd_short((u16b *) & mon->hp);
    if ((version_maj >= 2) && (version_min >= 6))
	rd_short((u16b *) & mon->maxhp);
    else {
	/* let's fix the infamous monster heal bug -CWS */
	mon->maxhp = mon->hp;
    }

    rd_short((u16b *) & mon->csleep);
    rd_short((u16b *) & mon->cspeed);
    rd_short(&mon->mptr);
    rd_byte(&mon->fy);
    rd_byte(&mon->fx);
    rd_byte(&mon->cdis);
    rd_byte(&mon->ml);
    rd_byte(&mon->stunned);
    rd_byte(&mon->confused);
    if ((version_maj >= 2) && (version_min >= 6))
	rd_byte((byte *) & mon->monfear);
    else
	mon->monfear = 0; /* this is not saved either -CWS */
}
