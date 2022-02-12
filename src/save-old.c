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
	rd_item_old(&st_ptr->store_inven[j]);
	st_ptr->store_inven[j].scost = scost;
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

    /* Allocate storage for name */
	r_ptr->name = (char*)malloc(101);
	C_WIPE(r_ptr->name, 101, char);

    /* Hack -- read the name as bytes */
    for (i = 0; i < 100; i++) rd_char(&gname[i]);
    strcpy(r_ptr->name, gname);

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

	rd_s32b(&u_list[i].exist);
	rd_s32b(&u_list[i].dead);
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




