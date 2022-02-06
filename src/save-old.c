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

static int	from_savefile;	/* can overwrite old savefile when save */

static u32b       start_time;	   /* time that play started */


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
    for (i = 0; i < MAX_MON_NATTACK; i++)
    rd_byte(&l_ptr->r_attacks[i]);
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
	prt("ERROR in store_ctr", 9, 0);
	return (10);
    }

    /* Read the items (and costs) */
    for (j = 0; j < st_ptr->store_ctr; j++) {
	s32b scost;
	rd_s32b(&scost);
	rd_item_old(&st_ptr->store_inven[j].sitem);
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


static void rd_unique(register struct unique_mon *item)
{
    rd_s32b(&item->exist);
    rd_s32b(&item->dead);
}




/*
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
    int                    i, j, fd, c, ok, total_count;
    u32b                 l, age, time_saved;
    vtype                  temp;
    u16b                 u16b_tmp;
    register cave_type    *c_ptr;
    register monster_lore  *r_ptr;
    store_type            *st_ptr;
    byte char_tmp, ychar, xchar, count;

    free_turn_flag = TRUE;	   /* So a feeling isn't generated upon reloading -DGK */

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
	rd_artifacts_old();
	if (to_be_wizard)
	    prt("Loaded Weapon Artifacts", 2, 0);
	put_qio();
	if (to_be_wizard)
	    prt("Loaded Armour Artifacts", 3, 0);
	put_qio();

	for (i = 0; i < MAX_QUESTS; i++)
	    rd_u32b(&quests[i]);
	if (to_be_wizard)
	    prt("Loaded Quests", 4, 0);

	for (i = 0; i < MAX_R_IDX; i++)
	    rd_unique(&u_list[i]);
	if (to_be_wizard)
	    prt("Loaded Unique Beasts", 5, 0);
	put_qio();

    /* Monster Memory */
	rd_u16b(&u16b_tmp);
	while (u16b_tmp != 0xFFFF) {
	    if (u16b_tmp >= MAX_R_IDX)
		goto error;
	    rd_lore_old(&l_list[u16b_tmp]);
	    rd_u16b(&u16b_tmp);
	}
	if (to_be_wizard)
	    prt("Loaded Recall Memory", 6, 0);

	put_qio();
        rd_u32b(&l);
	if (!older_than(2,6,0)) {
	  rd_u32b(&l);
	  rd_u32b(&l);
	  rd_u32b(&l);
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
	    for (i = 0; i < 4; i++)
		rd_string(p_ptr->history[i]);

    /* Read the stats */  
	    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->max_stat[i]);
		if (older_than(2,5,7)) {
		for (i = 0; i < 6; i++) rd_s16b(&p_ptr->cur_stat[i]);
		}
	    else
		for (i = 0; i < 6; i++) rd_byte(&p_ptr->cur_stat[i]);
	    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->mod_stat[i]);
	    for (i = 0; i < 6; i++) rd_s16b(&p_ptr->use_stat[i]);

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
	    rd_byte(&p_ptr->confuse_monster);
	    rd_byte(&p_ptr->new_spells);
	    rd_byte(&p_ptr->resist_pois);
	    rd_byte(&p_ptr->hold_life);
	    rd_byte(&p_ptr->telepathy);
	    rd_byte(&p_ptr->immune_fire);
	    rd_byte(&p_ptr->immune_acid);
	    rd_byte(&p_ptr->immune_pois);
	    rd_byte(&p_ptr->immune_cold);
	    rd_byte(&p_ptr->immune_elec);
	    rd_byte(&p_ptr->light);
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
	    if (older_than(2,6,0))
		p_ptr->resist_fear = 0;	/* sigh */
	    else
		rd_byte(&p_ptr->resist_fear);

	    rd_u16b(&missile_ctr);
	    rd_u32b(&turn);
	    if (older_than(2,6,0))
	      old_turn = turn;	/* best we can do... -CWS */
	    else
	      rd_u32b(&old_turn);

	    rd_u16b(&inven_ctr);
	    if (inven_ctr > INVEN_WIELD) {
		prt("ERROR in inven_ctr", 8, 0);
		goto error;
	    }
	    for (i = 0; i < inven_ctr; i++)
		rd_item(&inventory[i]);
	    for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
		rd_item(&inventory[i]);
	    rd_u16b(&inven_weight);
	    rd_u16b(&equip_ctr);
	    rd_u32b(&spell_learned);
	    rd_u32b(&spell_worked);
	    rd_u32b(&spell_forgotten);
	    rd_u32b(&spell_learned2);
	    rd_u32b(&spell_worked2);
	    rd_u32b(&spell_forgotten2);

	    for (i = 0; i < 64; i++) {
	    rd_byte(&spell_order[i]);
	    }

	    for (i = 0; i < OBJECT_IDENT_SIZE; i++)
	    rd_byte(&object_ident[i]);
	    rd_u32b(&randes_seed);
	    rd_u32b(&town_seed);
	    rd_u16b(&last_msg);
	    for (i = 0; i < MAX_SAVE_MSG; i++)
		rd_string(old_msg[i]);

	    rd_u16b(&panic_save);
	    rd_u16b(&total_winner);
	    rd_u16b(&noscore);

    /* Read the player_hp array */
	    for (i = 0; i < MAX_PLAYER_LEVEL; i++) {
	    rd_u16b(&player_hp[i]);
	    }

    /* Read the stores */
	    for (i = 0; i < MAX_STORES; i++) {
	      if (rd_store(&store[i])) goto error;
	    }

	    rd_u32b(&time_saved);
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
	if ((c = getc(fff)) == EOF || (l & 0x80000000L)) {
	    if ((l & 0x80000000L) == 0) {
		if (!to_be_wizard || turn < 0) {
		    prt("ERROR in to_be_wizard", 10, 0);
		    goto error;
		}

		/* Revive the player */
		prt("Attempting a resurrection!", 0, 0);

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

	    }

	    /* Normal "restoration" */
	    else {

		prt("Restoring Memory of a departed spirit...", 0, 0);

		/* Forget the turn, and old_turn */
		turn = old_turn = (-1);

	    }
	    put_qio();
	    goto closefiles;
	}

	if (ungetc(c, fff) == EOF) {
	    prt("ERROR in ungetc", 11, 0);
	    goto error;
	}

	prt("Restoring Character...", 0, 0);
	put_qio();

    /* only level specific info should follow, not present for dead characters */

	rd_u16b(&dun_level);
	rd_u16b(&char_row);
	rd_u16b(&char_col);
	rd_u16b(&mon_tot_mult);
	rd_u16b(&cur_height);
	rd_u16b(&cur_width);
	rd_u16b(&max_panel_rows);
	rd_u16b(&max_panel_cols);

    /* read in the creature ptr info */
	rd_byte(&char_tmp);
	while (char_tmp != 0xFF) {
	    ychar = char_tmp;
	    rd_byte(&xchar);

    /* let's correctly fix the invisible monster bug  -CWS */
	    if (older_than(2,6,0)) {
		rd_byte(&char_tmp);
		cave[ychar][xchar].m_idx = char_tmp;
	    } else {
		rd_u16b(&u16b_tmp);
		cave[ychar][xchar].m_idx = u16b_tmp;
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
	    rd_u16b(&u16b_tmp);
	    if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
		prt("Error in treasure pointer info", 12, 0);
		goto error;
	    }
	    cave[ychar][xchar].i_idx = u16b_tmp;
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

	rd_u16b(&i_max);
	if (i_max > MAX_I_IDX) {
	    prt("ERROR in MAX_I_IDX", 14, 0);
	    goto error;
	}
	for (i = MIN_I_IDX; i < i_max; i++)
	    rd_item(&i_list[i]);
	rd_u16b(&m_max);
	if (m_max > MAX_M_IDX) {
	    prt("ERROR in MAX_M_IDX", 15, 0);
	    goto error;
	}
	for (i = MIN_M_IDX; i < m_max; i++) {
	    rd_monster(&m_list[i]);
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

				/* Restore ghost names & stats etc... */
				/* Allocate storage for name */
	r_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	C_WIPE(r_list[MAX_R_IDX - 1].name, 101, char);
	*((char *) r_list[MAX_R_IDX - 1].name) = 'A';
	for (i = 0; i < 100; i++)
	rd_byte(&r_list[MAX_R_IDX - 1].name[i]);
	rd_u32b(&(r_list[MAX_R_IDX - 1].cflags1));
	rd_u32b(&(r_list[MAX_R_IDX - 1].spells1));
	rd_u32b(&(r_list[MAX_R_IDX - 1].cflags2));
	{
	    u16b t1;
/* fix player ghost's exp bug.  The mexp field is really an u32b, but the
 * savefile was writing/ reading an u16b.  Since I don't want to change
 * the savefile format, this insures that the mexp field is loaded, and that
 * the "high bits" of mexp do not contain garbage values which could mean that
 * player ghost are worth millions of exp. -CFT
 */

	    rd_u16b(&t1);
	    r_list[MAX_R_IDX - 1].mexp = t1;
	}

/* more stupid size bugs that would've never been needed if these variables
 * had been given enough space in the first place -CWS
 */
	if (older_than(2,6,0))
	    rd_byte(&(r_list[MAX_R_IDX - 1].sleep));
	else
	    rd_u16b(&(r_list[MAX_R_IDX - 1].sleep));

	rd_byte(&(r_list[MAX_R_IDX - 1].aaf));

	if (older_than(2,6,0))
	    rd_byte(&(r_list[MAX_R_IDX - 1].ac));
	else
	    rd_s16b(&(r_list[MAX_R_IDX - 1].ac));

	rd_byte(&(r_list[MAX_R_IDX - 1].speed));
	rd_byte(&(r_list[MAX_R_IDX - 1].r_char));

	rd_byte(&r_list[MAX_R_IDX - 1].hd[0]);
	rd_byte(&r_list[MAX_R_IDX - 1].hd[1]);

    rd_u16b(&r_ptr->damage[0]);
    rd_u16b(&r_ptr->damage[1]);
    rd_u16b(&r_ptr->damage[2]);
    rd_u16b(&r_ptr->damage[3]);

	rd_u16b(&(r_list[MAX_R_IDX - 1].level));
	*generate = FALSE;	   /* We have restored a cave - no need to generate. */

	if ((version_min == 1 && patch_level < 3)
	    || (version_min == 0))
	    for (i = 0; i < MAX_STORES; i++) {
		st_ptr = &store[i];
		rd_s32b(&st_ptr->store_open);
		rd_s16b(&st_ptr->insult_cur);
		rd_byte(&st_ptr->owner);
		rd_byte(&st_ptr->store_ctr);
		rd_u16b(&st_ptr->good_buy);
		rd_u16b(&st_ptr->bad_buy);
		if (st_ptr->store_ctr > STORE_INVEN_MAX) {
		    prt("ERROR in STORE_INVEN_MAX", 16, 0);
		    goto error;
		}
		for (j = 0; j < st_ptr->store_ctr; j++) {
		    rd_s32b(&st_ptr->store_inven[j].scost);
		    rd_item(&st_ptr->store_inven[j].sitem);
		}
	    }

    /* read the time that the file was saved */
	rd_u32b(&time_saved);

	if (ferror(fff)) {
	    prt("FILE ERROR", 17, 0);
	    goto error;
	}
	if (turn < 0) {
	    prt("Error = turn < 0", 7, 0);
    error:
	    ok = FALSE;		   /* Assume bad data. */
	} else {
	/* don't overwrite the killed by string if character is dead */
	    if (p_ptr->chp >= 0)
		(void)strcpy(died_from, "(alive and well)");
	    character_generated = 1;
	}

closefiles:

	if (fff != NULL) {
	    if (fclose(fff) < 0)
		ok = FALSE;
	}
	if (fd >= 0)
	    (void)close(fd);

	if (!ok)
	    msg_print("Error during reading of file.");
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

		tmp2 = strrchr(savefile, (int)'/');
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
    turn = (-1);
    prt("Please try again without that savefile.", 1, 0);
    signals();
#ifdef MAC
    *exit_flag = TRUE;
#else
    exit_game();
#endif

    return FALSE;		   /* not reached, unless on mac */
}




