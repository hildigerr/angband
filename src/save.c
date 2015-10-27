/* save.c: save and restore games and monster memory info

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

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

#include "monster.h"  /* needed for monster heal bug work-around -CFT */

static int sv_write(ARG_VOID);
static void wr_byte(ARG_INT8U);
static void wr_short(ARG_INT16U);
static void wr_long(ARG_INT32U);
static void wr_bytes(ARG_INT8U_PTR ARG_COMMA ARG_INT);
static void wr_string(ARG_CHAR_PTR);
static void wr_shorts(ARG_INT16U_PTR ARG_COMMA ARG_INT);
static void wr_item(ARG_INV_PTR);
static void wr_monster(ARG_MON_PTR);
static void rd_byte(ARG_INT8U_PTR);
static void rd_short(ARG_INT16U_PTR);
static void rd_long(ARG_INT32U_PTR);
static void rd_bytes(ARG_INT8U_PTR ARG_COMMA ARG_INT);
static void rd_string(ARG_CHAR_PTR);
static void rd_shorts(ARG_INT16U_PTR ARG_COMMA ARG_INT);
static void rd_item(ARG_INV_PTR);
static void rd_monster(ARG_MON_PTR);
static void wr_unique(ARG_UNIQ_PTR);
static void rd_unique(ARG_UNIQ_PTR);
static char *basename(ARG_CHAR_PTR);
int access(ARG_CHAR_PTR ARG_COMMA ARG_INT);
int chmod(ARG_CHAR_PTR ARG_COMMA ARG_INT);
#ifdef MSDOS
int setmode(ARG_INT ARG_COMMA ARG_INT);
#endif

#if !defined(ATARIST_MWC)
#ifdef MAC
#include <time.h>
#else
long time(ARG_INT32_PTR);
#endif
#else
char *malloc();
#endif

#ifndef SET_UID
#include <sys/stat.h>
#endif

/* these are used for the save file, to avoid having to pass them to every
   procedure */
static FILE *fileptr;
static int8u xor_byte;
static int from_savefile;	/* can overwrite old savefile when save */
static int32u start_time;	/* time that play started */

/* This save package was brought to by			-JWT-
   and							-RAK-
   and has been completely rewritten for UNIX by	-JEW-  */
/* and has been completely rewritten again by	 -CJS-	*/
/* and completely rewritten again! for portability by -JEW- */


static char *basename(a)
  char *a;
{
  char *b;
  char *strrchr();

  if ((b=strrchr(a, (int)'/'))==(char *)0)
    return a;
  return b;
}

static void wr_unique(item)
register struct unique_mon *item;
{
#ifdef MSDOS
  wr_byte((int8u)item->exist);
  wr_byte((int8u)item->dead);
#else
  wr_long((int32u)item->exist);
  wr_long((int32u)item->dead);
#endif
}

static void rd_unique(item)
register struct unique_mon *item;
{
#ifdef MSDOS
  rd_byte((int8u *)&item->exist);
  rd_byte((int8u *)&item->dead);
#else
  rd_long((int32u *)&item->exist);
  rd_long((int32u *)&item->dead);
#endif
}


static int sv_write()
{
  int32u l;
  register int i, j;
  int count;
  int8u char_tmp, prev_char;
  register cave_type *c_ptr;
  register recall_type *r_ptr;
  struct stats *s_ptr;
#ifdef MSDOS
  inven_type *t_ptr;
#endif
  register struct flags *f_ptr;
  store_type *st_ptr;
  struct misc *m_ptr;

  /* clear the death flag when creating a HANGUP save file, so that player
     can see tombstone when restart */

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
  if (getkey_flag)
    l |= 16;
  if (rogue_like_commands)
    l |= 32;
  if (show_weight_flag)
    l |= 64;
  if (highlight_seams)
    l |= 128;
  if (find_ignore_doors)
    l |= 256;
#ifdef MSDOS
  if (sound_beep_flag)
    l |= 0x200L;
  if (no_haggle_flag)
    l |= 0x400L;
#ifdef TC_COLOR
  if (no_color_flag)
    l |= 0x800L;
#endif
#endif    
  if (!carry_query_flag)
    l |= 0x1000L;
#ifdef TC_COLOR
  if (inven_bw_flag)
    l |= 0x2000L;
#endif
  if (death)
    l |= 0x80000000L;	/* Sign bit */

#ifdef MSDOS
#define WRITE_FN wr_byte
#else
#define WRITE_FN wr_long
#endif
  WRITE_FN(GROND);
  WRITE_FN(RINGIL);
  WRITE_FN(AEGLOS);
  WRITE_FN(ARUNRUTH);
  WRITE_FN(MORMEGIL);
  WRITE_FN(ANGRIST);
  WRITE_FN(GURTHANG);
  WRITE_FN(CALRIS);
  WRITE_FN(ANDURIL);
  WRITE_FN(STING);
  WRITE_FN(ORCRIST);
  WRITE_FN(GLAMDRING);
  WRITE_FN(DURIN);
  WRITE_FN(AULE);
  WRITE_FN(THUNDERFIST);
  WRITE_FN(BLOODSPIKE);
  WRITE_FN(DOOMCALLER);
  WRITE_FN(NARTHANC);
  WRITE_FN(NIMTHANC);
  WRITE_FN(DETHANC);
  WRITE_FN(GILETTAR);
  WRITE_FN(RILIA);
  WRITE_FN(BELANGIL);
  WRITE_FN(BALLI);
  WRITE_FN(LOTHARANG);
  WRITE_FN(FIRESTAR);
  WRITE_FN(ERIRIL);
  WRITE_FN(CUBRAGOL);
  WRITE_FN(BARD);
  WRITE_FN(COLLUIN);
  WRITE_FN(HOLCOLLETH);
  WRITE_FN(TOTILA);
  WRITE_FN(PAIN);
  WRITE_FN(ELVAGIL);
  WRITE_FN(AGLARANG);
  WRITE_FN(EORLINGAS);
  WRITE_FN(BARUKKHELED);
  WRITE_FN(WRATH);
  WRITE_FN(HARADEKKET);
  WRITE_FN(MUNDWINE);
  WRITE_FN(GONDRICAM);
  WRITE_FN(ZARCUTHRA);
  WRITE_FN(CARETH);
  WRITE_FN(FORASGIL);
  WRITE_FN(CRISDURIAN);
  WRITE_FN(COLANNON);
  WRITE_FN(HITHLOMIR);
  WRITE_FN(THALKETTOTH);
  WRITE_FN(ARVEDUI);
  WRITE_FN(THRANDUIL);
  WRITE_FN(THENGEL);
  WRITE_FN(HAMMERHAND);
  WRITE_FN(CELEFARN);
  WRITE_FN(THROR);
  WRITE_FN(MAEDHROS);
  WRITE_FN(OLORIN);
  WRITE_FN(ANGUIREL);
  WRITE_FN(OROME);
  WRITE_FN(EONWE);
  WRITE_FN(THEODEN);
  WRITE_FN(ULMO);
  WRITE_FN(OSONDIR);
  WRITE_FN(TURMIL);
  WRITE_FN(CASPANION);
  WRITE_FN(TIL);
  WRITE_FN(DEATHWREAKER);
  WRITE_FN(AVAVIR);
  WRITE_FN(TARATOL);

  WRITE_FN(DOR_LOMIN);
  WRITE_FN(NENYA);
  WRITE_FN(NARYA);
  WRITE_FN(VILYA);
  WRITE_FN(BELEGENNON);
  WRITE_FN(FEANOR);
  WRITE_FN(ISILDUR);
  WRITE_FN(SOULKEEPER);
  WRITE_FN(FINGOLFIN);
  WRITE_FN(ANARION);
  WRITE_FN(POWER);
  WRITE_FN(PHIAL);
  WRITE_FN(BELEG);
  WRITE_FN(DAL);
  WRITE_FN(PAURHACH);
  WRITE_FN(PAURNIMMEN);
  WRITE_FN(PAURAEGEN);
  WRITE_FN(PAURNEN);
  WRITE_FN(CAMMITHRIM);
  WRITE_FN(CAMBELEG);
  WRITE_FN(INGWE);
  WRITE_FN(CARLAMMAS);
  WRITE_FN(HOLHENNETH);
  WRITE_FN(AEGLIN);
  WRITE_FN(CAMLOST);
  WRITE_FN(NIMLOTH);
  WRITE_FN(NAR);
  WRITE_FN(BERUTHIEL);
  WRITE_FN(GORLIM);
  WRITE_FN(ELENDIL);
  WRITE_FN(THORIN);
  WRITE_FN(CELEBORN);
  WRITE_FN(THRAIN);
  WRITE_FN(GONDOR);
  WRITE_FN(THINGOL);
  WRITE_FN(THORONGIL);
  WRITE_FN(LUTHIEN);
  WRITE_FN(TUOR);
  WRITE_FN(ROHAN);
  WRITE_FN(TULKAS);
  WRITE_FN(NECKLACE);
  WRITE_FN(BARAHIR);
  WRITE_FN(RAZORBACK);
  WRITE_FN(BLADETURNER);

  for (i=0; i<MAX_QUESTS; i++)
#ifdef MSDOS
    wr_short(quests[i]);
#else
    wr_long(quests[i]);
#endif

  for (i=0; i<MAX_CREATURES; i++)
    wr_unique(&u_list[i]);

  for (i = 0; i < MAX_CREATURES; i++)
    {
      r_ptr = &c_recall[i];
      if (r_ptr->r_cmove || r_ptr->r_cdefense || r_ptr->r_kills ||
          r_ptr->r_spells2 || r_ptr->r_spells3 || r_ptr->r_spells ||
	  r_ptr->r_deaths || r_ptr->r_attacks[0] || r_ptr->r_attacks[1] ||
	  r_ptr->r_attacks[2] || r_ptr->r_attacks[3])
	{
	  wr_short((int16u)i);
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
  wr_short((int16u)0xFFFF); /* sentinel to indicate no more monster info */

  wr_short((int16u)log_index);
  wr_long(l);

  m_ptr = &py.misc;
  wr_string(m_ptr->name);
  wr_byte(m_ptr->male);
  wr_long((int32u)m_ptr->au);
  wr_long((int32u)m_ptr->max_exp);
  wr_long((int32u)m_ptr->exp);
  wr_short(m_ptr->exp_frac);
  wr_short(m_ptr->age);
  wr_short(m_ptr->ht);
  wr_short(m_ptr->wt);
  wr_short(m_ptr->lev);
  wr_short(m_ptr->max_dlv);
  wr_short((int16u)m_ptr->srh);
  wr_short((int16u)m_ptr->fos);
  wr_short((int16u)m_ptr->bth);
  wr_short((int16u)m_ptr->bthb);
  wr_short((int16u)m_ptr->mana);
  wr_short((int16u)m_ptr->mhp);
  wr_short((int16u)m_ptr->ptohit);
  wr_short((int16u)m_ptr->ptodam);
  wr_short((int16u)m_ptr->pac);
  wr_short((int16u)m_ptr->ptoac);
  wr_short((int16u)m_ptr->dis_th);
  wr_short((int16u)m_ptr->dis_td);
  wr_short((int16u)m_ptr->dis_ac);
  wr_short((int16u)m_ptr->dis_tac);
  wr_short((int16u)m_ptr->disarm);
  wr_short((int16u)m_ptr->save);
  wr_short((int16u)m_ptr->sc);
  wr_short((int16u)m_ptr->stl);
  wr_byte(m_ptr->pclass);
  wr_byte(m_ptr->prace);
  wr_byte(m_ptr->hitdie);
  wr_byte(m_ptr->expfact);
  wr_short((int16u)m_ptr->cmana);
  wr_short(m_ptr->cmana_frac);
  wr_short((int16u)m_ptr->chp);
  wr_short(m_ptr->chp_frac);
  for (i = 0; i < 4; i++)
    wr_string (m_ptr->history[i]);

  s_ptr = &py.stats;
  wr_shorts(s_ptr->max_stat, 6);
  wr_shorts(s_ptr->cur_stat, 6);
  wr_shorts((int16u *)s_ptr->mod_stat, 6);
  wr_shorts(s_ptr->use_stat, 6);

  f_ptr = &py.flags;
  wr_long(f_ptr->status);
  wr_short((int16u)f_ptr->rest);
  wr_short((int16u)f_ptr->blind);
  wr_short((int16u)f_ptr->paralysis);
  wr_short((int16u)f_ptr->confused);
  wr_short((int16u)f_ptr->food);
  wr_short((int16u)f_ptr->food_digested);
  wr_short((int16u)f_ptr->protection);
  wr_short((int16u)f_ptr->speed);
  wr_short((int16u)f_ptr->fast);
  wr_short((int16u)f_ptr->slow);
  wr_short((int16u)f_ptr->afraid);
  wr_short((int16u)f_ptr->cut);
  wr_short((int16u)f_ptr->stun);
  wr_short((int16u)f_ptr->poisoned);
  wr_short((int16u)f_ptr->image);
  wr_short((int16u)f_ptr->protevil);
  wr_short((int16u)f_ptr->invuln);
  wr_short((int16u)f_ptr->hero);
  wr_short((int16u)f_ptr->shero);
  wr_short((int16u)f_ptr->shield);
  wr_short((int16u)f_ptr->blessed);
  wr_short((int16u)f_ptr->resist_heat);
  wr_short((int16u)f_ptr->resist_cold);
  wr_short((int16u)f_ptr->resist_acid);
  wr_short((int16u)f_ptr->resist_light);
  wr_short((int16u)f_ptr->resist_poison);
  wr_short((int16u)f_ptr->detect_inv);
  wr_short((int16u)f_ptr->word_recall);
  wr_short((int16u)f_ptr->see_infra);
  wr_short((int16u)f_ptr->tim_infra);
  wr_byte(f_ptr->see_inv);
  wr_byte(f_ptr->teleport);
  wr_byte(f_ptr->free_act);
  wr_byte(f_ptr->slow_digest);
  wr_byte(f_ptr->aggravate);
  wr_byte(f_ptr->fire_resist);
  wr_byte(f_ptr->cold_resist);
  wr_byte(f_ptr->acid_resist);
  wr_byte(f_ptr->regenerate);
  wr_byte(f_ptr->lght_resist);
  wr_byte(f_ptr->ffall);
  wr_byte(f_ptr->sustain_str);
  wr_byte(f_ptr->sustain_int);
  wr_byte(f_ptr->sustain_wis);
  wr_byte(f_ptr->sustain_con);
  wr_byte(f_ptr->sustain_dex);
  wr_byte(f_ptr->sustain_chr);
  wr_byte(f_ptr->confuse_monster);
  wr_byte(f_ptr->new_spells);
  wr_byte(f_ptr->poison_resist);
  wr_byte(f_ptr->hold_life);
  wr_byte(f_ptr->telepathy);
  wr_byte(f_ptr->fire_im);
  wr_byte(f_ptr->acid_im);
  wr_byte(f_ptr->poison_im);
  wr_byte(f_ptr->cold_im);
  wr_byte(f_ptr->light_im);
  wr_byte(f_ptr->light);
  wr_byte(f_ptr->confusion_resist);
  wr_byte(f_ptr->sound_resist);
  wr_byte(f_ptr->light_resist);
  wr_byte(f_ptr->dark_resist);
  wr_byte(f_ptr->chaos_resist);
  wr_byte(f_ptr->disenchant_resist);
  wr_byte(f_ptr->shards_resist);
  wr_byte(f_ptr->nexus_resist);
  wr_byte(f_ptr->blindness_resist);
  wr_byte(f_ptr->nether_resist);

  wr_short((int16u)missile_ctr);
  wr_long((int32u)turn);
  wr_short((int16u)inven_ctr);
  for (i = 0; i < inven_ctr; i++)
    wr_item(&inventory[i]);
  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
    wr_item(&inventory[i]);
  wr_short((int16u)inven_weight);
  wr_short((int16u)equip_ctr);
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
  wr_short((int16u)last_msg);
  for (i = 0; i < MAX_SAVE_MSG; i++)
    wr_string(old_msg[i]);

  /* this indicates 'cheating' if it is a one */
  wr_short((int16u)panic_save);
  wr_short((int16u)total_winner);
  wr_short((int16u)noscore);
  wr_shorts(player_hp, MAX_PLAYER_LEVEL);


  for (i = 0; i < MAX_STORES; i++)
    {
      st_ptr = &store[i];
      wr_long((int32u)st_ptr->store_open);
      wr_short((int16u)st_ptr->insult_cur);
      wr_byte(st_ptr->owner);
      wr_byte(st_ptr->store_ctr);
      wr_short(st_ptr->good_buy);
      wr_short(st_ptr->bad_buy);

      for (j = 0; j < st_ptr->store_ctr; j++)
	{
	    wr_long((int32u)st_ptr->store_inven[j].scost);
	    wr_item(&st_ptr->store_inven[j].sitem);
	}
  }

  /* save the current time in the savefile */
  l = time((long *)0);
/*  if (l < start_time)
    {
        someone is messing with the clock!, assume that we have been
	 playing for 1 day
      l = start_time + 86400L;
    }
*/
  wr_long(l);

  /* starting with 5.2, put died_from string in savefile */
  wr_string(died_from);

  /* only level specific info follows, this allows characters to be
     resurrected, the dungeon level info is not needed for a resurrection */
  if (death)
    {
      if (ferror(fileptr) || fflush(fileptr) == EOF)
	return FALSE;
      return TRUE;
    }
  wr_short((int16u)dun_level);
  wr_short((int16u)char_row);
  wr_short((int16u)char_col);
  wr_short((int16u)mon_tot_mult);
  wr_short((int16u)cur_height);
  wr_short((int16u)cur_width);
  wr_short((int16u)max_panel_rows);
  wr_short((int16u)max_panel_cols);

  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->cptr != 0)
	  {
	    wr_byte((int8u)i);
	    wr_byte((int8u)j);
	    wr_byte(c_ptr->cptr);
	  }
      }
  wr_byte((int8u)0xFF); /* marks end of cptr info */
  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  {
	    wr_byte((int8u)i);
	    wr_byte((int8u)j);
#ifdef MSDOS
	    wr_short((int16u)c_ptr->tptr);
#else
	    wr_short((int16u)c_ptr->tptr);
#endif
	  }
      }
  wr_byte(0xFF); /* marks end of tptr info */
  /* must set counter to zero, note that code may write out two bytes
     unnecessarily */
  count = 0;
  prev_char = 0;
  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	char_tmp = c_ptr->fval | (c_ptr->lr << 4) | (c_ptr->fm << 5) |
	  (c_ptr->pl << 6) | (c_ptr->tl << 7);
	if (char_tmp != prev_char || count == MAX_UCHAR)
	  {
	    wr_byte((int8u)count);
	    wr_byte(prev_char);
	    prev_char = char_tmp;
	    count = 1;
	  }
	else
	  count++;
      }
  /* save last entry */
  wr_byte((int8u)count);
  wr_byte(prev_char);

#ifdef MSDOS
  /* must change graphics symbols for walls and floors back to default chars,
     this is necessary so that if the user changes the graphics line, the
     program will be able change all existing walls/floors to the new symbol */
  t_ptr = &t_list[tcptr - 1];
  for (i = tcptr - 1; i >= MIN_TRIX; i--)
    {
      if (t_ptr->tchar == wallsym)
	t_ptr->tchar = '#';
      t_ptr--;
    }
#endif
  wr_short((int16u)tcptr);
  for (i = MIN_TRIX; i < tcptr; i++)
    wr_item(&t_list[i]);
  wr_short((int16u)mfptr);
  for (i = MIN_MONIX; i < mfptr; i++)
    wr_monster(&m_list[i]);

  /* Save ghost names & stats etc... */
  wr_bytes(c_list[MAX_CREATURES - 1].name, 100);
  wr_long((int32u)c_list[MAX_CREATURES - 1].cmove);
  wr_long((int32u)c_list[MAX_CREATURES - 1].spells);
  wr_long((int32u)c_list[MAX_CREATURES - 1].cdefense);
  {  int16u temp;  /* fix player ghost's exp bug.  The mexp field is
      			  really an int32u, but the savefile was writing/
      			  reading an int16u.  Since I don't want to change
      			  the savefile format, this insures that the low
      			  bits of mexp are written (No ghost should be
      			  worth more than 64K (Melkor is only worth 60k!),
			  but we check anyway).  Using temp insures that
			  the low bits are all written, and works perfectly
			  with a similar fix when loading a character. -CFT */
    if (c_list[MAX_CREATURES-1].mexp > (int32u)0xff00)
      temp = (int16u)0xff00;
    else
      temp = (int16u)c_list[MAX_CREATURES-1].mexp;
    wr_short((int16u)temp);
  }
  wr_byte((int8u)c_list[MAX_CREATURES - 1].sleep);
  wr_byte((int8u)c_list[MAX_CREATURES - 1].aaf);
  wr_byte((int8u)c_list[MAX_CREATURES - 1].ac);
  wr_byte((int8u)c_list[MAX_CREATURES - 1].speed);
  wr_byte((int8u)c_list[MAX_CREATURES - 1].cchar);
  wr_bytes(c_list[MAX_CREATURES - 1].hd, 2);
  wr_bytes(c_list[MAX_CREATURES - 1].damage, sizeof(attid)*4);
  wr_short((int16u)c_list[MAX_CREATURES - 1].level);

  if (ferror(fileptr) || (fflush(fileptr) == EOF))
    return FALSE;
  return TRUE;
}

int save_char()
{
  int i;
  int fd;
  vtype temp;
  char *tmp2;

#ifdef SECURE
  bePlayer();
#endif
  if (death && NO_SAVE) return TRUE;

#ifdef MSDOS
  return _save_char(savefile);
#else
  if (_save_char(savefile)) {

    tmp2=basename(savefile);

    (void) sprintf(temp, "%s/p.%s", ANGBAND_SAV, (tmp2+1));

    unlink(temp);
  } else {
    return FALSE;
  }
#ifdef SECURE
  beGames();
#endif
  return TRUE;
#endif
}

int _save_char(fnam)
char *fnam;
{
  vtype temp;
  register int ok, fd;
  int8u char_tmp;

  if (log_index < 0)
    return TRUE;	/* Nothing to save. */

  nosignals();
  put_qio();
  disturb (1, 0);		/* Turn off resting and searching. */
  change_speed(-pack_heavy);	/* Fix the speed */
  pack_heavy = 0;
  ok = FALSE;
#ifndef ATARIST_MWC
  fd = -1;
  fileptr = NULL;		/* Do not assume it has been init'ed */
#ifdef SET_UID
  fd = open(fnam, O_RDWR|O_CREAT|O_EXCL, 0600);
#else
  fd = open(fnam, O_RDWR|O_CREAT|O_EXCL, 0666);
#endif
  if (fd < 0 && access(fnam, 0) >= 0 &&
      (from_savefile ||
       (wizard && get_check("Can't make new savefile. Overwrite old?"))))
    {
#ifdef SET_UID
      (void) chmod(fnam, 0600);
      fd = open(fnam, O_RDWR|O_TRUNC, 0600);
#else
      (void) chmod(fnam, 0666);
      fd = open(fnam, O_RDWR|O_TRUNC, 0666);
#endif
  }
  if (fd >= 0)
    {
      (void) close(fd);
#endif /* !ATARIST_MWC */
      /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARIST_MWC) || defined(MSDOS)
      fileptr = fopen(savefile, "wb");
#else
      fileptr = fopen(savefile, "w");
#endif
#ifndef ATARIST_MWC
    }
#endif
  if (fileptr != NULL)
    {
      xor_byte = 0;
      wr_byte((int8u)CUR_VERSION_MAJ);
      xor_byte = 0;
      wr_byte((int8u)CUR_VERSION_MIN);
      xor_byte = 0;
      wr_byte((int8u)PATCH_LEVEL);
      xor_byte = 0;
      char_tmp = randint(256) - 1;
      wr_byte(char_tmp);
      /* Note that xor_byte is now equal to char_tmp */

      ok = sv_write();
      if (fclose(fileptr) == EOF)
	ok = FALSE;
    }

  if (!ok)
    {
      if (fd >= 0)
	(void) unlink(fnam);
      signals();
      if (fd >= 0)
	(void) sprintf(temp, "Error writing to savefile", fnam);
      else
/* here? */
	(void) sprintf(temp, "Can't create new savefile", fnam);
      msg_print(temp);
      return FALSE;
    }
  else { /* successful save, now add char to highscore table, and
  		show him where he stands... */
    if (!wizard && !to_be_wizard && !noscore) {
#ifdef MSDOS
      int t;

      msg_print(NULL); /* clear msg line's "saving..." note */
      t = top_twenty();  /* if t==0, then top_twenty didn't show them, */
      if (!t) display_scores(0, MAX_SAVE_HISCORES, -1); /* so do it here */
#else
      msg_print(NULL); /* clear msg line's "saving..." note */
      top_twenty();
#endif
      }
    else msg_print("Score not registered.");

    character_saved = 1;
  }
  turn = -1;
  log_index = -1;

  signals();

  return TRUE;
}


/* Certain checks are ommitted for the wizard. -CJS- */

int get_char(generate)
int *generate;
{
  register int i, j;
  int fd, c, ok, total_count;
  int32u l, age, time_saved;
  vtype temp;
  int16u int16u_tmp;
  register cave_type *c_ptr;
  register recall_type *r_ptr;
  struct misc *m_ptr;
  struct stats *s_ptr;
  register struct flags *f_ptr;
  store_type *st_ptr;
#ifdef MSDOS
  inven_type *t_ptr;
#endif
  int8u char_tmp, ychar, xchar, count;
  int8u version_maj, version_min, patch_level;

  nosignals();
  *generate = TRUE;
  fd = -1;

  if (access(savefile, 0) < 0)
    {
      signals();
      msg_print("Savefile does not exist.");
      return FALSE;
    }

  clear_screen();

  (void) sprintf(temp, "Restoring Character.", savefile);
  put_buffer(temp, 23, 0);
  sleep(1);

  if (turn >= 0)
    msg_print("IMPOSSIBLE! Attempt to restore while still alive!");

  /* Allow restoring a file belonging to someone else - if we can delete it. */
  /* Hence first try to read without doing a chmod. */

  else if ((fd = open(savefile, O_RDONLY)) < 0)
    msg_print("Can't open file for reading.");
  else
    {
#ifndef SET_UID
#ifndef MSDOS /* accuses people of having a fiddled savefile if write took
		too long, like if DOS6's undelete decided to take control
		of the disk for a while... */
      struct stat statbuf;
#endif
#endif
      turn = -1;
      log_index = -1;
      ok = TRUE;

#ifndef SET_UID
#ifndef MSDOS /* accuses people of having a fiddled savefile if write took
		too long, like if DOS6's undelete decided to take control
		of the disk for a while... */
      (void) fstat(fd, &statbuf);
#endif
#endif
      (void) close(fd);
      /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARI_ST) || defined(THINK_C) || defined(MSDOS)
      fileptr = fopen(savefile, "rb");
#else
      fileptr = fopen(savefile, "r");
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

      /* COMPAT support savefiles from 5.0.14 to 5.0.17 */
      /* support savefiles from 5.1.0 to present */
      if ((version_maj != CUR_VERSION_MAJ)
	  || (version_min > CUR_VERSION_MIN)
	  || (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL)
	  || (version_min == 0 && patch_level < 14))
	{
	  prt("Sorry. This savefile is from a different version of angband.",
	      2, 0);
	  goto error;
	}

      put_qio();
#ifdef MSDOS
#define READ_FN rd_byte
#else
#define READ_FN rd_long
#endif
      READ_FN(&GROND);
      READ_FN(&RINGIL);
      READ_FN(&AEGLOS);
      READ_FN(&ARUNRUTH);
      READ_FN(&MORMEGIL);
      READ_FN(&ANGRIST);
      READ_FN(&GURTHANG);
      READ_FN(&CALRIS);
      READ_FN(&ANDURIL);
      READ_FN(&STING);
      READ_FN(&ORCRIST);
      READ_FN(&GLAMDRING);
      READ_FN(&DURIN);
      READ_FN(&AULE);
      READ_FN(&THUNDERFIST);
      READ_FN(&BLOODSPIKE);
      READ_FN(&DOOMCALLER);
      READ_FN(&NARTHANC);
      READ_FN(&NIMTHANC);
      READ_FN(&DETHANC);
      READ_FN(&GILETTAR);
      READ_FN(&RILIA);
      READ_FN(&BELANGIL);
      READ_FN(&BALLI);
      READ_FN(&LOTHARANG);
      READ_FN(&FIRESTAR);
      READ_FN(&ERIRIL);
      READ_FN(&CUBRAGOL);
      READ_FN(&BARD);
      READ_FN(&COLLUIN);
      READ_FN(&HOLCOLLETH);
      READ_FN(&TOTILA);
      READ_FN(&PAIN);
      READ_FN(&ELVAGIL);
      READ_FN(&AGLARANG);
      READ_FN(&EORLINGAS);
      READ_FN(&BARUKKHELED);
      READ_FN(&WRATH);
      READ_FN(&HARADEKKET);
      READ_FN(&MUNDWINE);
      READ_FN(&GONDRICAM);
      READ_FN(&ZARCUTHRA);
      READ_FN(&CARETH);
      READ_FN(&FORASGIL);
      READ_FN(&CRISDURIAN);
      READ_FN(&COLANNON);
      READ_FN(&HITHLOMIR);
      READ_FN(&THALKETTOTH);
      READ_FN(&ARVEDUI);
      READ_FN(&THRANDUIL);
      READ_FN(&THENGEL);
      READ_FN(&HAMMERHAND);
      READ_FN(&CELEFARN);
      READ_FN(&THROR);
      READ_FN(&MAEDHROS);
      READ_FN(&OLORIN);
      READ_FN(&ANGUIREL);
      READ_FN(&OROME);
      READ_FN(&EONWE);
      READ_FN(&THEODEN);
      READ_FN(&ULMO);
      READ_FN(&OSONDIR);
      READ_FN(&TURMIL);
      READ_FN(&CASPANION);
      READ_FN(&TIL);
      READ_FN(&DEATHWREAKER);
      READ_FN(&AVAVIR);
      READ_FN(&TARATOL);
      if (to_be_wizard) prt("Loaded Weapon Artifacts", 2, 0);
      put_qio();


      READ_FN(&DOR_LOMIN);
      READ_FN(&NENYA);
      READ_FN(&NARYA);
      READ_FN(&VILYA);
      READ_FN(&BELEGENNON);
      READ_FN(&FEANOR);
      READ_FN(&ISILDUR);
      READ_FN(&SOULKEEPER);
      READ_FN(&FINGOLFIN);
      READ_FN(&ANARION);
      READ_FN(&POWER);
      READ_FN(&PHIAL);
      READ_FN(&BELEG);
      READ_FN(&DAL);
      READ_FN(&PAURHACH);
      READ_FN(&PAURNIMMEN);
      READ_FN(&PAURAEGEN);
      READ_FN(&PAURNEN);
      READ_FN(&CAMMITHRIM);
      READ_FN(&CAMBELEG);
      READ_FN(&INGWE);
      READ_FN(&CARLAMMAS);
      READ_FN(&HOLHENNETH);
      READ_FN(&AEGLIN);
      READ_FN(&CAMLOST);
      READ_FN(&NIMLOTH);
      READ_FN(&NAR);
      READ_FN(&BERUTHIEL);
      READ_FN(&GORLIM);
      READ_FN(&ELENDIL);
      READ_FN(&THORIN);
      READ_FN(&CELEBORN);
      READ_FN(&THRAIN);
      READ_FN(&GONDOR);
      READ_FN(&THINGOL);
      READ_FN(&THORONGIL);
      READ_FN(&LUTHIEN);
      READ_FN(&TUOR);
      READ_FN(&ROHAN);
      READ_FN(&TULKAS);
      READ_FN(&NECKLACE);
      READ_FN(&BARAHIR);
      READ_FN(&RAZORBACK);
      READ_FN(&BLADETURNER);
      if (to_be_wizard) prt("Loaded Armour Artifacts", 3, 0);
      put_qio();

      for (i=0; i<MAX_QUESTS; i++)
#ifdef MSDOS
	rd_short(&quests[i]);
#else	
	rd_long(&quests[i]);
#endif
      if (to_be_wizard) prt("Loaded Quests", 4, 0);

      for (i=0; i<MAX_CREATURES; i++)
	rd_unique(&u_list[i]);
      if (to_be_wizard) prt("Loaded Unique Beasts", 5, 0);
      put_qio();

      rd_short(&int16u_tmp);
      while (int16u_tmp != 0xFFFF)
	{
	  if (int16u_tmp >= MAX_CREATURES)
	    goto error;
	  r_ptr = &c_recall[int16u_tmp];
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
	  rd_short(&int16u_tmp);
	}
      if (to_be_wizard) prt("Loaded Recall Memory", 6, 0);
      put_qio();
      rd_short((int16u *)&log_index);
      rd_long(&l);
      if (to_be_wizard) prt("Loaded Options Memory", 7, 0);
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
	getkey_flag = TRUE;
      else
	getkey_flag = FALSE;
      if (l & 32)
	rogue_like_commands = TRUE;
      else
	rogue_like_commands = FALSE;
      if (l & 64)
	show_weight_flag = TRUE;
      else
	show_weight_flag = FALSE;
      if (l & 128)
	highlight_seams = TRUE;
      else
	highlight_seams = FALSE;
      if (l & 256)
	find_ignore_doors = TRUE;
      else
	find_ignore_doors = FALSE;
#ifdef MSDOS
      if (l & 0x200L)
      	sound_beep_flag = TRUE;
      else
        sound_beep_flag = FALSE;
      if (l & 0x400L)
      	no_haggle_flag = TRUE;
      else
        no_haggle_flag = FALSE;
#ifdef TC_COLOR
      if (l & 0x800L)
      	no_color_flag = TRUE;
      else
        no_color_flag = FALSE;
#endif
#endif        
      if (l & 0x1000L)
      	carry_query_flag = FALSE;
      else
        carry_query_flag = TRUE;
#ifdef TC_COLOR
      if (l & 0x2000L)
      	inven_bw_flag = TRUE;
      else
        inven_bw_flag = FALSE;
#endif
      if (to_be_wizard && (l & 0x80000000L)
	  && get_check("Resurrect a dead character?"))
	l &= ~0x80000000L;
      if ((l & 0x80000000L) == 0)
	{
	  m_ptr = &py.misc;
	  rd_string(m_ptr->name);
	  rd_byte(&m_ptr->male);
	  rd_long((int32u *)&m_ptr->au);
	  rd_long((int32u *)&m_ptr->max_exp);
	  rd_long((int32u *)&m_ptr->exp);
	  rd_short(&m_ptr->exp_frac);
	  rd_short(&m_ptr->age);
	  rd_short(&m_ptr->ht);
	  rd_short(&m_ptr->wt);
	  rd_short(&m_ptr->lev);
	  rd_short(&m_ptr->max_dlv);
	  rd_short((int16u *)&m_ptr->srh);
	  rd_short((int16u *)&m_ptr->fos);
	  rd_short((int16u *)&m_ptr->bth);
	  rd_short((int16u *)&m_ptr->bthb);
	  rd_short((int16u *)&m_ptr->mana);
	  rd_short((int16u *)&m_ptr->mhp);
	  rd_short((int16u *)&m_ptr->ptohit);
	  rd_short((int16u *)&m_ptr->ptodam);
	  rd_short((int16u *)&m_ptr->pac);
	  rd_short((int16u *)&m_ptr->ptoac);
	  rd_short((int16u *)&m_ptr->dis_th);
	  rd_short((int16u *)&m_ptr->dis_td);
	  rd_short((int16u *)&m_ptr->dis_ac);
	  rd_short((int16u *)&m_ptr->dis_tac);
	  rd_short((int16u *)&m_ptr->disarm);
	  rd_short((int16u *)&m_ptr->save);
	  rd_short((int16u *)&m_ptr->sc);
	  rd_short((int16u *)&m_ptr->stl);
	  rd_byte(&m_ptr->pclass);
	  rd_byte(&m_ptr->prace);
	  rd_byte(&m_ptr->hitdie);
	  rd_byte(&m_ptr->expfact);
	  rd_short((int16u *)&m_ptr->cmana);
	  rd_short(&m_ptr->cmana_frac);
	  rd_short((int16u *)&m_ptr->chp);
	  rd_short(&m_ptr->chp_frac);
	  for (i = 0; i < 4; i++)
	    rd_string (m_ptr->history[i]);

	  s_ptr = &py.stats;
	  rd_shorts(s_ptr->max_stat, 6);
	  rd_shorts(s_ptr->cur_stat, 6);
	  rd_shorts((int16u *)s_ptr->mod_stat, 6);
	  rd_shorts(s_ptr->use_stat, 6);

	  f_ptr = &py.flags;
	  rd_long(&f_ptr->status);
	  rd_short((int16u *)&f_ptr->rest);
	  rd_short((int16u *)&f_ptr->blind);
	  rd_short((int16u *)&f_ptr->paralysis);
	  rd_short((int16u *)&f_ptr->confused);
	  rd_short((int16u *)&f_ptr->food);
	  rd_short((int16u *)&f_ptr->food_digested);
	  rd_short((int16u *)&f_ptr->protection);
	  rd_short((int16u *)&f_ptr->speed);
	  rd_short((int16u *)&f_ptr->fast);
	  rd_short((int16u *)&f_ptr->slow);
	  rd_short((int16u *)&f_ptr->afraid);
	  rd_short((int16u *)&f_ptr->cut);
	  rd_short((int16u *)&f_ptr->stun);
	  rd_short((int16u *)&f_ptr->poisoned);
	  rd_short((int16u *)&f_ptr->image);
	  rd_short((int16u *)&f_ptr->protevil);
	  rd_short((int16u *)&f_ptr->invuln);
	  rd_short((int16u *)&f_ptr->hero);
	  rd_short((int16u *)&f_ptr->shero);
	  rd_short((int16u *)&f_ptr->shield);
	  rd_short((int16u *)&f_ptr->blessed);
	  rd_short((int16u *)&f_ptr->resist_heat);
	  rd_short((int16u *)&f_ptr->resist_cold);
	  rd_short((int16u *)&f_ptr->resist_acid);
	  rd_short((int16u *)&f_ptr->resist_light);
	  rd_short((int16u *)&f_ptr->resist_poison);
	  rd_short((int16u *)&f_ptr->detect_inv);
	  rd_short((int16u *)&f_ptr->word_recall);
	  rd_short((int16u *)&f_ptr->see_infra);
	  rd_short((int16u *)&f_ptr->tim_infra);
	  rd_byte(&f_ptr->see_inv);
	  rd_byte(&f_ptr->teleport);
	  rd_byte(&f_ptr->free_act);
	  rd_byte(&f_ptr->slow_digest);
	  rd_byte(&f_ptr->aggravate);
	  rd_byte(&f_ptr->fire_resist);
	  rd_byte(&f_ptr->cold_resist);
	  rd_byte(&f_ptr->acid_resist);
	  rd_byte(&f_ptr->regenerate);
	  rd_byte(&f_ptr->lght_resist);
	  rd_byte(&f_ptr->ffall);
	  rd_byte(&f_ptr->sustain_str);
	  rd_byte(&f_ptr->sustain_int);
	  rd_byte(&f_ptr->sustain_wis);
	  rd_byte(&f_ptr->sustain_con);
	  rd_byte(&f_ptr->sustain_dex);
	  rd_byte(&f_ptr->sustain_chr);
	  rd_byte(&f_ptr->confuse_monster);
	  rd_byte(&f_ptr->new_spells);
	  rd_byte(&f_ptr->poison_resist);
	  rd_byte(&f_ptr->hold_life);
	  rd_byte(&f_ptr->telepathy);
	  rd_byte(&f_ptr->fire_im);
	  rd_byte(&f_ptr->acid_im);
	  rd_byte(&f_ptr->poison_im);
	  rd_byte(&f_ptr->cold_im);
	  rd_byte(&f_ptr->light_im);
	  rd_byte(&f_ptr->light);
	  rd_byte(&f_ptr->confusion_resist);
  	  rd_byte(&f_ptr->sound_resist);
	  rd_byte(&f_ptr->light_resist);
 	  rd_byte(&f_ptr->dark_resist);
	  rd_byte(&f_ptr->chaos_resist);
	  rd_byte(&f_ptr->disenchant_resist);
	  rd_byte(&f_ptr->shards_resist);
	  rd_byte(&f_ptr->nexus_resist);
	  rd_byte(&f_ptr->blindness_resist);
	  rd_byte(&f_ptr->nether_resist);

	  rd_short((int16u *)&missile_ctr);
	  rd_long((int32u *)&turn);
	  rd_short((int16u *)&inven_ctr);
	  if (inven_ctr > INVEN_WIELD) {
            prt("ERROR in inven_ctr", 8, 0);
	    goto error;
          }
	  for (i = 0; i < inven_ctr; i++)
	    rd_item(&inventory[i]);
	  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
	    rd_item(&inventory[i]);
	  rd_short((int16u *)&inven_weight);
	  rd_short((int16u *)&equip_ctr);
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
	  rd_short((int16u *)&last_msg);
	  for (i = 0; i < MAX_SAVE_MSG; i++)
	    rd_string(old_msg[i]);

	  rd_short((int16u *)&panic_save);
	  rd_short((int16u *)&total_winner);
	  rd_short((int16u *)&noscore);
	  rd_shorts(player_hp, MAX_PLAYER_LEVEL);

	  if ((version_min >= 2)
	      || (version_min == 1 && patch_level >= 3))
	    for (i = 0; i < MAX_STORES; i++)
	      {
		st_ptr = &store[i];
		rd_long((int32u *)&st_ptr->store_open);
		rd_short((int16u *)&st_ptr->insult_cur);
		rd_byte(&st_ptr->owner);
		rd_byte(&st_ptr->store_ctr);
		rd_short(&st_ptr->good_buy);
		rd_short(&st_ptr->bad_buy);
		if (st_ptr->store_ctr > STORE_INVEN_MAX) {
                  prt("ERROR in store_ctr", 9, 0);
		  goto error;
                }
		for (j = 0; j < st_ptr->store_ctr; j++)
		  {
		    rd_long((int32u *)&st_ptr->store_inven[j].scost);
		    rd_item(&st_ptr->store_inven[j].sitem);
		  }
	      }

	  if ((version_min >= 2)
	      || (version_min == 1 && patch_level >= 3)) {
	    rd_long(&time_saved);
#ifndef SET_UID
#ifndef MSDOS /* accuses people of having a fiddled savefile if write took
		too long, like if DOS6's undelete decided to take control
		of the disk for a while... */
	    if (!to_be_wizard) {
	      if (time_saved > (statbuf.st_ctime+100) ||
		  time_saved < (statbuf.st_ctime-100)) {
		    prt("Fiddled save file", 10, 0);
		    goto error;
		  }
	    }
#endif
#endif
	  }

	  if (version_min >= 2)
	    rd_string(died_from);
	}
      if ((c = getc(fileptr)) == EOF || (l & 0x80000000L))
	{
	  if ((l & 0x80000000L) == 0)
	    {
	      if (!to_be_wizard || turn < 0) {
                prt("ERROR in to_be_wizard", 10, 0);
		goto error;
              }
	      prt("Attempting a resurrection!", 0, 0);
	      if (py.misc.chp < 0)
		{
		  py.misc.chp =	 0;
		  py.misc.chp_frac = 0;
		}
	      /* don't let him starve to death immediately */
	      if (py.flags.food < 100)
		py.flags.food = 10000;
	      /* don't let him die of poison again immediately */
	      if (py.flags.poisoned > 1)
		py.flags.poisoned = 1;
	      /* or from wounds -CFT */
	      if (py.flags.cut > 1)
		py.flags.cut = 1;
	      /* don't recall immediately if we died before recall could
	      	 save us -CFT */
	      if (py.flags.word_recall > 0)
	        py.flags.word_recall = 0;
	      dun_level = 0; /* Resurrect on the town level. */
	      character_generated = 1;
	      /* set noscore to indicate a resurrection, and don't enter
		 wizard mode */
	      to_be_wizard = FALSE;
	      noscore |= 0x1;
	    }
	  else
	    {
	      prt("Restoring Memory of a departed spirit...", 0, 0);
	      turn = -1;
	    }
	  put_qio();
	  /* The log_index of the previous incarnation is here if later version
	     want to use it. For now, throw it away and get a new log. */
	  log_index = -1;
	  goto closefiles;
	}
      if (ungetc(c, fileptr) == EOF) {
        prt("ERROR in ungetc", 11, 0);
	goto error;
      }
      prt("Restoring Character...", 0, 0);
      put_qio();

      /* only level specific info should follow, not present for dead
         characters */

      rd_short((int16u *)&dun_level);
      rd_short((int16u *)&char_row);
      rd_short((int16u *)&char_col);
      rd_short((int16u *)&mon_tot_mult);
      rd_short((int16u *)&cur_height);
      rd_short((int16u *)&cur_width);
      rd_short((int16u *)&max_panel_rows);
      rd_short((int16u *)&max_panel_cols);

      /* read in the creature ptr info */
      rd_byte(&char_tmp);
      while (char_tmp != 0xFF)
	{
	  ychar = char_tmp;
	  rd_byte(&xchar);
	  rd_byte(&char_tmp);
	  if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
	    vtype temp;
	    sprintf(temp,
		"Error in creature ptr info: x=%x, y=%x, char_tmp=%x",
		xchar, ychar, char_tmp);
            prt(temp, 11, 0);
	    goto error;
	  }
	  cave[ychar][xchar].cptr = char_tmp;
	  rd_byte(&char_tmp);
	}
      /* read in the treasure ptr info */
      rd_byte(&char_tmp);
      while (char_tmp != 0xFF)
	{
	  ychar = char_tmp;
	  rd_byte(&xchar);
#ifdef MSDOS
	  rd_short((int16u *)&int16u_tmp);
#else
	  rd_short((int16u *)&int16u_tmp);
#endif
	  if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
            prt("Error in treasure pointer info", 12, 0);
	    goto error;
	  }
#ifdef MSDOS
	  cave[ychar][xchar].tptr = int16u_tmp;
#else
	  cave[ychar][xchar].tptr = int16u_tmp;
#endif
	  rd_byte(&char_tmp);
	}
      /* read in the rest of the cave info */
      c_ptr = &cave[0][0];
      total_count = 0;
      while (total_count != MAX_HEIGHT*MAX_WIDTH)
	{
	  rd_byte(&count);
	  rd_byte(&char_tmp);
	  for (i = count; i > 0; i--)
	    {
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

      rd_short((int16u *)&tcptr);
      if (tcptr > MAX_TALLOC) {
	prt("ERROR in MAX_TALLOC", 14, 0);
	goto error;
      }
      for (i = MIN_TRIX; i < tcptr; i++)
	rd_item(&t_list[i]);
      rd_short((int16u *)&mfptr);
      if (mfptr > MAX_MALLOC) {
	prt("ERROR in MAX_MALLOC", 15, 0);
	goto error;
      }
      for (i = MIN_MONIX; i < mfptr; i++)
	rd_monster(&m_list[i]);

#ifdef MSDOS
      /* change walls and floors to graphic symbols */
      t_ptr = &t_list[tcptr - 1];
      for (i = tcptr - 1; i >= MIN_TRIX; i--)
	{
	  if (t_ptr->tchar == '#')
	    t_ptr->tchar = wallsym;
	  t_ptr--;
	}
#endif

      /* Restore ghost names & stats etc... */
      c_list[MAX_CREATURES - 1].name[0]='A';
      rd_bytes((int8u *)(c_list[MAX_CREATURES - 1].name), 100);
      rd_long((int32u *)&(c_list[MAX_CREATURES - 1].cmove));
      rd_long((int32u *)&(c_list[MAX_CREATURES - 1].spells));
      rd_long((int32u *)&(c_list[MAX_CREATURES - 1].cdefense));
      {  int16u temp;  /* fix player ghost's exp bug.  The mexp field is
      			  really an int32u, but the savefile was writing/
      			  reading an int16u.  Since I don't want to change
      			  the savefile format, this insures that the mexp
      			  field is loaded, and that the "high bits" of
      			  mexp do not contain garbage values which could
      			  mean that player ghost are worth millions of
			  exp. -CFT */
        rd_short((int16u *)&temp);
        c_list[MAX_CREATURES-1].mexp = (int32u)temp;
      }
      rd_byte((int8u *)&(c_list[MAX_CREATURES - 1].sleep));
      rd_byte((int8u *)&(c_list[MAX_CREATURES - 1].aaf));
      rd_byte((int8u *)&(c_list[MAX_CREATURES - 1].ac));
      rd_byte((int8u *)&(c_list[MAX_CREATURES - 1].speed));
      rd_byte((int8u *)&(c_list[MAX_CREATURES - 1].cchar));
      rd_bytes((int8u *)(c_list[MAX_CREATURES - 1].hd), 2);
      rd_bytes((int8u *)(c_list[MAX_CREATURES - 1].damage), sizeof(attid)*4);
      rd_short((int16u *)&(c_list[MAX_CREATURES - 1].level));
      *generate = FALSE;  /* We have restored a cave - no need to generate. */

      if ((version_min == 1 && patch_level < 3)
	  || (version_min == 0))
	for (i = 0; i < MAX_STORES; i++)
	  {
	    st_ptr = &store[i];
	    rd_long((int32u *)&st_ptr->store_open);
	    rd_short((int16u *)&st_ptr->insult_cur);
	    rd_byte(&st_ptr->owner);
	    rd_byte(&st_ptr->store_ctr);
	    rd_short(&st_ptr->good_buy);
	    rd_short(&st_ptr->bad_buy);
	    if (st_ptr->store_ctr > STORE_INVEN_MAX) {
	      prt("ERROR in STORE_INVEN_MAX", 16, 0);
	      goto error;
	    }
	    for (j = 0; j < st_ptr->store_ctr; j++)
	      {
		rd_long((int32u *)&st_ptr->store_inven[j].scost);
		rd_item(&st_ptr->store_inven[j].sitem);
	      }
	  }

      /* read the time that the file was saved */
      if (version_min == 0 && patch_level < 16)
	time_saved = 0; /* no time in file, clear to zero */
      else if (version_min == 1 && patch_level < 3)
	rd_long(&time_saved);

      if (ferror(fileptr)) {
	prt("FILE ERROR", 17, 0);
	goto error;
      }

      if (turn < 0) {
        prt("Error = turn < 0", 7, 0);
      error:
	ok = FALSE;	/* Assume bad data. */
        }
      else
	{
	  /* don't overwrite the killed by string if character is dead */
	  if (py.misc.chp >= 0)
	    (void) strcpy(died_from, "(alive and well)");
	  character_generated = 1;
	}

    closefiles:

      if (fileptr != NULL)
	{
	  if (fclose(fileptr) < 0)
	    ok = FALSE;
	}
      if (fd >= 0)
	(void) close(fd);

      if (!ok)
	msg_print("Error during reading of file.");
      else if (turn >= 0 && !_new_log())
	msg_print("Can't log player in the log file.");
      else
	{
	  /* let the user overwrite the old savefile when save/quit */
	  from_savefile = 1;

	  signals();

	  if (turn >= 0)
	    {	/* Only if a full restoration. */
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

	      age = (age + 43200L) / 86400L;  /* age in days */
	      if (age > 10) age = 10; /* in case savefile is very old */
	      for (i = 0; i < age; i++)
		store_maint();
	    }
	  /*
	  if (noscore)
	    msg_print("This save file cannot be used to get on the score board.");
	  */

	  if (version_maj != CUR_VERSION_MAJ
	      || version_min != CUR_VERSION_MIN)
	    {
	      (void) sprintf(temp,
			     "Save file version %d.%d %s on game version %d.%d.",
			     version_maj, version_min,
			     version_maj == CUR_VERSION_MAJ
			     ? "accepted" : "very risky" ,
			     CUR_VERSION_MAJ, CUR_VERSION_MIN);
	      msg_print(temp);
	    }

	  if (turn >= 0) {
#ifndef MSDOS  /* I'm not quite sure what this does, but UM55 doesn't
		  use it, and link() is undefined on TC, so I'm ifndef-ing
		  it out for now...  -CFT */
	    char *tmp2;

	    tmp2=basename(savefile);

	    (void) sprintf(temp, "%s/p.%s", ANGBAND_SAV, (tmp2+1));

	    link(savefile, temp);
	    unlink(savefile);
#endif
	    return TRUE;
	  } else {
	    return FALSE;	/* Only restored options and monster memory. */
	  }
	}
    }
  turn = -1;
  log_index = -1;
  prt("Please try again without that savefile.", 1, 0);
  signals();
#ifdef MAC
  *exit_flag = TRUE;
#else
  exit_game();
#endif

  return FALSE;	/* not reached, unless on mac */
}

static void wr_byte(c)
int8u c;
{
  xor_byte ^= c;
  (void) putc((int)xor_byte, fileptr);
}

static void wr_short(s)
int16u s;
{
  xor_byte ^= (s & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((s >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
}

static void wr_long(l)
register int32u l;
{
  xor_byte ^= (l & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 16) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 24) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
}

static void wr_bytes(c, count)
int8u *c;
register int count;
{
  register int i;
  register int8u *ptr;

  ptr = c;
  for (i = 0; i < count; i++)
    {
      xor_byte ^= *ptr++;
      (void) putc((int)xor_byte, fileptr);
    }
}

static void wr_string(str)
register char *str;
{
  while (*str != '\0')
    {
      xor_byte ^= *str++;
      (void) putc((int)xor_byte, fileptr);
    }
  xor_byte ^= *str;
  (void) putc((int)xor_byte, fileptr);
}

static void wr_shorts(s, count)
int16u *s;
register int count;
{
  register int i;
  register int16u *sptr;

  sptr = s;
  for (i = 0; i < count; i++)
    {
      xor_byte ^= (*sptr & 0xFF);
      (void) putc((int)xor_byte, fileptr);
      xor_byte ^= ((*sptr++ >> 8) & 0xFF);
      (void) putc((int)xor_byte, fileptr);
    }
}

static void wr_item(item)
register inven_type *item;
{
  wr_short(item->index);
  wr_byte(item->name2);
  wr_string(item->inscrip);
  wr_long(item->flags);
  wr_byte(item->tval);
  wr_byte(item->tchar);
  wr_short((int16u)item->p1);
  wr_long((int32u)item->cost);
  wr_byte(item->subval);
  wr_byte(item->number);
  wr_short(item->weight);
  wr_short((int16u)item->tohit);
  wr_short((int16u)item->todam);
  wr_short((int16u)item->ac);
  wr_short((int16u)item->toac);
  wr_bytes(item->damage, 2);
  wr_byte(item->level);
  wr_byte(item->ident);
  wr_long(item->flags2);
  wr_short((int16u)item->timeout);
#ifdef TC_COLOR
  wr_byte(item->color);
#endif
}

static void wr_monster(mon)
register monster_type *mon;
{
  wr_short((int16u)mon->hp);
  wr_short((int16u)mon->csleep);
  wr_short((int16u)mon->cspeed);
  wr_short(mon->mptr);
  wr_byte(mon->fy);
  wr_byte(mon->fx);
  wr_byte(mon->cdis);
  wr_byte(mon->ml);
  wr_byte(mon->stunned);
  wr_byte(mon->confused);
#ifdef TC_COLOR
  wr_byte(mon->color);
#endif
}

static void rd_byte(ptr)
int8u *ptr;
{
  int8u c;

  c = getc(fileptr) & 0xFF;
  *ptr = c ^ xor_byte;
  xor_byte = c;
}

static void rd_short(ptr)
int16u *ptr;
{
  int8u c;
  int16u s;

  c = (getc(fileptr) & 0xFF);
  s = c ^ xor_byte;
  xor_byte = (getc(fileptr) & 0xFF);
  s |= (int16u)(c ^ xor_byte) << 8;
  *ptr = s;
}

static void rd_long(ptr)
int32u *ptr;
{
  register int32u l;
  register int8u c;

  c = (getc(fileptr) & 0xFF);
  l = c ^ xor_byte;
  xor_byte = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 8;
  c = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 16;
  xor_byte = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 24;
  *ptr = l;
}

static void rd_bytes(ptr, count)
int8u *ptr;
int count;
{
  int i;
  int8u c;
  int8u nc;

  for (i = 0; i < count; i++)
    {
      c = (getc(fileptr) & 0xFF);
      nc = c ^ xor_byte;
      *ptr = nc;
      ptr++;
      xor_byte = c;
    }
}

static void rd_string(str)
char *str;
{
  register int8u c;

  do
    {
      c = (getc(fileptr) & 0xFF);
      *str = c ^ xor_byte;
      xor_byte = c;
    }
  while (*str++ != '\0');
}

static void rd_shorts(ptr, count)
int16u *ptr;
register int count;
{
  register int i;
  register int16u *sptr;
  register int16u s;
  int8u c;

  sptr = ptr;
  for (i = 0; i < count; i++)
    {
      c = (getc(fileptr) & 0xFF);
      s = c ^ xor_byte;
      xor_byte = (getc(fileptr) & 0xFF);
      s |= (int16u)(c ^ xor_byte) << 8;
      *sptr++ = s;
    }
}

static void rd_item(item)
register inven_type *item;
{
  rd_short(&item->index);
  rd_byte(&item->name2);
  rd_string(item->inscrip);
  rd_long(&item->flags);
  rd_byte(&item->tval);
  rd_byte(&item->tchar);
  rd_short((int16u *)&item->p1);
  rd_long((int32u *)&item->cost);
  rd_byte(&item->subval);
  rd_byte(&item->number);
  rd_short(&item->weight);
  rd_short((int16u *)&item->tohit);
  rd_short((int16u *)&item->todam);
  rd_short((int16u *)&item->ac);
  rd_short((int16u *)&item->toac);
  rd_bytes(item->damage, 2);
  rd_byte(&item->level);
  rd_byte(&item->ident);
  rd_long(&item->flags2);
  rd_short((int16u *)&item->timeout);
#ifdef TC_COLOR
  rd_byte(&item->color);
#endif
}

static void rd_monster(mon)
register monster_type *mon;
{
  rd_short((int16u *)&mon->hp);
  rd_short((int16u *)&mon->csleep);
  rd_short((int16u *)&mon->cspeed);
  rd_short(&mon->mptr);
  rd_byte(&mon->fy);
  rd_byte(&mon->fx);
  rd_byte(&mon->cdis);
  rd_byte(&mon->ml);
  rd_byte(&mon->stunned);
  rd_byte(&mon->confused);
#ifdef TC_COLOR
  rd_byte(&mon->color);
#endif

 /* monster heal bug mostly cause because max hp never saved!  Set it
    now to help bandage over it (real cure requires savefile change)
    Thanks to CWS and DGK for finding this one!   -CFT */
  if ((c_list[mon->mptr].cdefense & MAX_HP) || be_nasty)
    mon->maxhp = max_hp(c_list[mon->mptr].hd);
  else
    mon->maxhp = pdamroll(c_list[mon->mptr].hd);
}
