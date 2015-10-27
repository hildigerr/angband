/* create.c: create a player character

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#else
#include <strings.h>
#endif

struct previous {
  int16u age;
  int16u wt;
  int16u ht;
  int16 disarm;
  int16u str;
  int16u itl;
  int16u wis;
  int16u dex;
  int16u con;
  int16u chr;
  int16u sc;
  char history[4][60];
  background_type bg;
} prev;

extern int peek;

/* prototypes...  ARG_* will be nothing except on TURBOC... -CFT */
static void get_stats(ARG_VOID);
static void change_stat(ARG_INT ARG_COMMA ARG_INT16);
static int monval(ARG_INT16);
static void choose_race(ARG_VOID);
static void get_sex(ARG_VOID);
static void get_class_choice(ARG_VOID);
static void get_all_stats(ARG_VOID);
static void get_auto_stats(ARG_VOID);
static void put_auto_stats(ARG_VOID);
static void get_history(ARG_VOID);
static void get_ahw(ARG_VOID);
static void print_history(ARG_VOID);
static void get_class(ARG_VOID);
static int get_prev_stats(ARG_VOID);
static void get_prev_history(ARG_VOID);
static void get_prev_ahw(ARG_VOID);
static void get_money(ARG_VOID);


/* Generates character's stats			-JWT-	*/
static void get_stats()
{
  register int i, tot;
  int dice[18];

  do
    {
      tot = 0;
      for (i = 0; i < 18; i++)
	{
	  dice[i] = randint (3 + i % 3);  /* Roll 3,4,5 sided dice once each */
	  tot += dice[i];
	}
    }
  while (tot <= 42 || tot >= 54);

  for (i = 0; i < 6; i++)
    py.stats.max_stat[i] = 5 + dice[3*i] + dice[3*i+1] + dice[3*i+2];
}


/* Changes stats by given amount				-JWT-	*/
static void change_stat(stat, amount)
int stat;
int16 amount;
{
  register int i;
  register int tmp_stat;

  tmp_stat = py.stats.max_stat[stat];
  if (amount < 0)
    for (i = 0; i > amount; i--)
      {
	if (tmp_stat > 108)
	  tmp_stat--;
	else if (tmp_stat > 88)
	  tmp_stat += -randint(6) - 2;
	else if (tmp_stat > 18)
	  {
	    tmp_stat += -randint(15) - 5;
	    if (tmp_stat < 18)
	      tmp_stat = 18;
	  }
	else if (tmp_stat > 3)
	  tmp_stat--;
      }
  else
    for (i = 0; i < amount; i++)
      {
	if (tmp_stat < 18)
	  tmp_stat++;
	else if (tmp_stat < 88)
	  tmp_stat += randint(15) + 5;
	else if (tmp_stat < 108)
	  tmp_stat += randint(6) + 2;
	else if (tmp_stat < 118)
	  tmp_stat++;
      }
  py.stats.max_stat[stat] = tmp_stat;
}

static int get_prev_stats() {
  if (!prev.str) return 0; 
  py.stats.cur_stat[0]=(py.stats.max_stat[0]=prev.str);
  py.stats.cur_stat[1]=(py.stats.max_stat[1]=prev.itl);
  py.stats.cur_stat[2]=(py.stats.max_stat[2]=prev.wis);
  py.stats.cur_stat[3]=(py.stats.max_stat[3]=prev.dex);
  py.stats.cur_stat[4]=(py.stats.max_stat[4]=prev.con);
  py.stats.cur_stat[5]=(py.stats.max_stat[5]=prev.chr);
  set_use_stat(0);
  set_use_stat(1);
  set_use_stat(2);
  set_use_stat(3);
  set_use_stat(4);
  set_use_stat(5);
  py.misc.ptodam = todam_adj();
  py.misc.ptohit = tohit_adj();
  py.misc.pac    = toac_adj();
  prev.str=0;
  return 1;
}

/* generate all stats and modify for race. needed in a separate module so
   looping of character selection would be allowed     -RGM- */
static void get_all_stats ()
{
  register player_type *p_ptr;
  register race_type *r_ptr;
  register int j;

  prev.str = (int16u)py.stats.max_stat[0];
  prev.itl = (int16u)py.stats.max_stat[1];
  prev.wis = (int16u)py.stats.max_stat[2];
  prev.dex = (int16u)py.stats.max_stat[3];
  prev.con = (int16u)py.stats.max_stat[4];
  prev.chr = (int16u)py.stats.max_stat[5];
  p_ptr = &py;
  r_ptr = &race[p_ptr->misc.prace];
  get_stats ();
  change_stat (A_STR, r_ptr->str_adj);
  change_stat (A_INT, r_ptr->int_adj);
  change_stat (A_WIS, r_ptr->wis_adj);
  change_stat (A_DEX, r_ptr->dex_adj);
  change_stat (A_CON, r_ptr->con_adj);
  change_stat (A_CHR, r_ptr->chr_adj);
  for (j = 0; j < 6; j++)
    {
      py.stats.cur_stat[j] = py.stats.max_stat[j];
      set_use_stat (j);
    }

  p_ptr->misc.srh    = r_ptr->srh;
  p_ptr->misc.bth    = r_ptr->bth;
  p_ptr->misc.bthb   = r_ptr->bthb;
  p_ptr->misc.fos    = r_ptr->fos;
  p_ptr->misc.stl    = r_ptr->stl;
  p_ptr->misc.save   = r_ptr->bsav;
  p_ptr->misc.hitdie = r_ptr->bhitdie;
  p_ptr->misc.lev    = 1;
  p_ptr->misc.ptodam = todam_adj();
  p_ptr->misc.ptohit = tohit_adj();
  p_ptr->misc.ptoac  = 0;
  p_ptr->misc.pac    = toac_adj();
  p_ptr->misc.expfact = r_ptr->b_exp;
  p_ptr->flags.see_infra = r_ptr->infra;
}

/* slightly modified version of get_all_stats(), for use in auto-roller.
   This version never calls calc_mana(), so it will not pause the rolling
   with a "more" message.  -CFT
   generate all stats and modify for race. needed in a separate module so
   looping of character selection would be allowed     -RGM- */
static void get_auto_stats ()
{
  register player_type *p_ptr;
  register race_type *r_ptr;
  register int j;

  prev.str = (int16u)py.stats.max_stat[0];
  prev.itl = (int16u)py.stats.max_stat[1];
  prev.wis = (int16u)py.stats.max_stat[2];
  prev.dex = (int16u)py.stats.max_stat[3];
  prev.con = (int16u)py.stats.max_stat[4];
  prev.chr = (int16u)py.stats.max_stat[5];
  p_ptr = &py;
  r_ptr = &race[p_ptr->misc.prace];
  get_stats ();
  change_stat (A_STR, r_ptr->str_adj);
  change_stat (A_INT, r_ptr->int_adj);
  change_stat (A_WIS, r_ptr->wis_adj);
  change_stat (A_DEX, r_ptr->dex_adj);
  change_stat (A_CON, r_ptr->con_adj);
  change_stat (A_CHR, r_ptr->chr_adj);
  for (j = 0; j < 6; j++)
    {
      py.stats.cur_stat[j] = py.stats.max_stat[j];
      py.stats.use_stat[j] = modify_stat (j, py.stats.mod_stat[j]);
    }

  p_ptr->misc.srh    = r_ptr->srh;
  p_ptr->misc.bth    = r_ptr->bth;
  p_ptr->misc.bthb   = r_ptr->bthb;
  p_ptr->misc.fos    = r_ptr->fos;
  p_ptr->misc.stl    = r_ptr->stl;
  p_ptr->misc.save   = r_ptr->bsav;
  p_ptr->misc.hitdie = r_ptr->bhitdie;
  p_ptr->misc.lev    = 1;
  p_ptr->misc.ptodam = todam_adj();
  p_ptr->misc.ptohit = tohit_adj();
  p_ptr->misc.ptoac  = 0;
  p_ptr->misc.pac    = toac_adj();
  p_ptr->misc.expfact = r_ptr->b_exp;
  p_ptr->flags.see_infra = r_ptr->infra;
}

/* copied from misc2.c, so the display loop would work nicely -cft */
static char *stat_names[] = { "STR: ", "INT: ", "WIS: ",
				 "DEX: ", "CON: ", "CHR: " };

static void put_auto_stats(){ /* used for auto-roller.  Just put_stats(),
				w/o the extra info -CFT */
  register int i;
  vtype buf;

  for (i = 0; i < 6; i++)
    {
      cnv_stat (py.stats.use_stat[i], buf);
      put_buffer (stat_names[i], 2+i, 61);
      put_buffer (buf, 2+i, 66);
      if (py.stats.max_stat[i] > py.stats.cur_stat[i])
	{
	  cnv_stat (py.stats.max_stat[i], buf);
	  put_buffer (buf, 2+i, 73);
	}
    }
}

/* Allows player to select a race			-JWT-	*/
static void choose_race()
{
  register int j, k;
  int l, m, exit_flag;
  char s;
  char tmp_str[80];
  register player_type *p_ptr;
  register race_type *r_ptr;

  j = 0;
  k = 0;
  l = 2;
  m = 21;
  clear_from (20);
  put_buffer("Choose a race (? for Help):", 20, 2);
  do
    {
      (void) sprintf(tmp_str, "%c) %s", k+'a', race[j].trace);
      put_buffer(tmp_str, m, l);
      k++;
      l += 15;
      if (l > 70)
	{
	  l = 2;
	  m++;
	}
      j++;
    }
  while (j < MAX_RACES);
  exit_flag = FALSE;
  do
    {
      move_cursor (20, 30);
      s = inkey();
      j = s - 'a';
      if ((j < MAX_RACES) && (j >= 0))
	exit_flag = TRUE;
      else if (s == '?')
	helpfile (ANGBAND_WELCOME);
      else
	bell ();
    }
  while (!exit_flag);

  p_ptr = &py;
  r_ptr = &race[j];
  p_ptr->misc.prace  = j;
  put_buffer(r_ptr->trace, 3, 15);
}


/* Will print the history of a character			-JWT-	*/
static void print_history()
{
  register int i;

  put_buffer("Character Background", 14, 27);
  for (i = 0; i < 4; i++)
    prt(py.misc.history[i], i+15, 10);
}

static void get_prev_history()
{
  background->info = prev.bg.info;
  background->roll = prev.bg.roll;
  background->chart = prev.bg.chart;
  background->next = prev.bg.next;
  background->bonus = prev.bg.bonus;
  py.misc.sc = prev.sc;
  
  strncpy(py.misc.history[0], prev.history[0], 60);
  strncpy(py.misc.history[1], prev.history[1], 60);
  strncpy(py.misc.history[2], prev.history[2], 60);
  strncpy(py.misc.history[3], prev.history[3], 60);
}  

/* Get the racial history, determines social class	-RAK-	*/
/* Assumptions:	Each race has init history beginning at		*/
/*		(race-1)*3+1					*/
/*		All history parts are in ascending order	*/
static void get_history()
{
  int hist_ptr, cur_ptr, test_roll, flag;
  register int start_pos, end_pos, cur_len;
  int line_ctr, new_start, social_class;
  char history_block[240];
  register background_type *b_ptr;

  prev.bg.info = background->info;
  prev.bg.roll = background->roll;
  prev.bg.chart = background->chart;
  prev.bg.next = background->next;
  prev.bg.bonus = background->bonus;
  prev.sc = py.misc.sc;

  (void) strncpy(prev.history[0], py.misc.history[0], 60);
  (void) strncpy(prev.history[1], py.misc.history[1], 60);
  (void) strncpy(prev.history[2], py.misc.history[2], 60);
  (void) strncpy(prev.history[3], py.misc.history[3], 60);

  /* Get a block of history text				*/
  if (py.misc.prace == 8)
    hist_ptr = 1;
  else if (py.misc.prace > 8)
    hist_ptr = 2*3 +1;
  else
    hist_ptr = py.misc.prace*3 + 1;
  history_block[0] = '\0';
  social_class = randint(4);
  cur_ptr = 0;
  do
    {
      flag = FALSE;
      do
	{
	  if (background[cur_ptr].chart == hist_ptr)
	    {
	      test_roll = randint(100);
	      while (test_roll > background[cur_ptr].roll)
		cur_ptr++;
	      b_ptr = &background[cur_ptr];
	      (void) strcat(history_block, b_ptr->info);
	      social_class += b_ptr->bonus - 50;
	      if (hist_ptr > b_ptr->next)
		cur_ptr = 0;
	      hist_ptr = b_ptr->next;
	      flag = TRUE;
	    }
	  else
	    cur_ptr++;
	}
      while (!flag);
    }
  while (hist_ptr >= 1);

  /* clear the previous history strings */
  for (hist_ptr = 0; hist_ptr < 4; hist_ptr++)
    py.misc.history[hist_ptr][0] = '\0';

  /* Process block of history text for pretty output	*/
  start_pos = 0;
  end_pos   = strlen(history_block) - 1;
  line_ctr  = 0;
  flag = FALSE;
  while (history_block[end_pos] == ' ')
    end_pos--;
  do
    {
      while (history_block[start_pos] == ' ')
	start_pos++;
      cur_len = end_pos - start_pos + 1;
      if (cur_len > 60)
	{
	  cur_len = 60;
	  while (history_block[start_pos+cur_len-1] != ' ')
	    cur_len--;
	  new_start = start_pos + cur_len;
	  while (history_block[start_pos+cur_len-1] == ' ')
	    cur_len--;
	}
      else
	flag = TRUE;
      (void) strncpy(py.misc.history[line_ctr], &history_block[start_pos],
		     cur_len);
      py.misc.history[line_ctr][cur_len] = '\0';
      line_ctr++;
      start_pos = new_start;
    }
  while (!flag);

  /* Compute social class for player			*/
  if (social_class > 100)
    social_class = 100;
  else if (social_class < 1)
    social_class = 1;
  py.misc.sc = social_class;
}


/* Gets the character's sex				-JWT-	*/
static void get_sex()
{
  register int exit_flag;
  char c;

  exit_flag = FALSE;
  clear_from (20);
  put_buffer("Choose a sex (? for Help):", 20, 2);
  put_buffer("m) Male       f) Female", 21, 2);
  do
    {
      move_cursor (20, 29);
      /* speed not important here */
      c = inkey();
      if (c == 'f' || c == 'F')
	{
	  py.misc.male = FALSE;
	  put_buffer("Female", 4, 15);
	  exit_flag = TRUE;
	}
      else if (c == 'm' || c == 'M')
	{
	  py.misc.male = TRUE;
	  put_buffer("Male", 4, 15);
	  exit_flag = TRUE;
	}
      else if (c == '?')
	helpfile (ANGBAND_WELCOME);
      else
	bell ();
    }
  while (!exit_flag);
}


/* Computes character's age, height, and weight		-JWT-	*/
static void get_ahw()
{
  register int i;

  prev.age = py.misc.age;
  prev.wt = py.misc.wt;
  prev.ht = py.misc.ht;
  prev.disarm = py.misc.disarm;
  i = py.misc.prace;
  py.misc.age = race[i].b_age + randint((int)race[i].m_age);
  if (py.misc.male)
    {
      py.misc.ht = randnor((int)race[i].m_b_ht, (int)race[i].m_m_ht);
      py.misc.wt = randnor((int)race[i].m_b_wt, (int)race[i].m_m_wt);
    }
  else
    {
      py.misc.ht = randnor((int)race[i].f_b_ht, (int)race[i].f_m_ht);
      py.misc.wt = randnor((int)race[i].f_b_wt, (int)race[i].f_m_wt);
    }
  py.misc.disarm = race[i].b_dis;
}

static void get_prev_ahw()
{
  py.misc.age = prev.age;
  py.misc.wt = prev.wt;
  py.misc.ht = prev.ht;
  py.misc.disarm = prev.disarm;
  prev.age = prev.wt = prev.ht = prev.disarm = 0;
}


/* Gets a character class				-JWT-	*/
static void get_class()
{
  register int i;
  int min_value, max_value;
  int percent;
  char buf[50];
  register struct misc *m_ptr;
  register player_type *p_ptr;
  class_type *c_ptr;

  c_ptr = &class[py.misc.pclass];
  p_ptr = &py;	
  change_stat (A_STR, c_ptr->madj_str);
  change_stat (A_INT, c_ptr->madj_int);
  change_stat (A_WIS, c_ptr->madj_wis);
  change_stat (A_DEX, c_ptr->madj_dex);
  change_stat (A_CON, c_ptr->madj_con);
  change_stat (A_CHR, c_ptr->madj_chr);

  for(i = 0; i < 6; i++)
    {
      p_ptr->stats.cur_stat[i] = p_ptr->stats.max_stat[i];
      set_use_stat(i);
    }
  p_ptr->misc.ptodam = todam_adj();	/* Real values		*/
  p_ptr->misc.ptohit = tohit_adj();
  p_ptr->misc.ptoac  = toac_adj();
  p_ptr->misc.pac    = 0;
  p_ptr->misc.dis_td = p_ptr->misc.ptodam; /* Displayed values	*/
  p_ptr->misc.dis_th = p_ptr->misc.ptohit;
  p_ptr->misc.dis_tac= p_ptr->misc.ptoac;
  p_ptr->misc.dis_ac = p_ptr->misc.pac + p_ptr->misc.dis_tac;
  
  /* now set misc stats, do this after setting stats because
    of con_adj() for hitpoints */
  m_ptr = &py.misc;
  m_ptr->hitdie += c_ptr->adj_hd;
  m_ptr->mhp = con_adj() + m_ptr->hitdie;
  m_ptr->chp = m_ptr->mhp;
  m_ptr->chp_frac = 0;
  
  /* initialize hit_points array */
  /* put bounds on total possible hp, only succeed if it is within
    1/8 of average value */
  min_value = (MAX_PLAYER_LEVEL*3 * (m_ptr->hitdie-1))/8 +
    MAX_PLAYER_LEVEL;
  max_value = (MAX_PLAYER_LEVEL*5 * (m_ptr->hitdie-1))/8 +
    MAX_PLAYER_LEVEL;
#ifndef MSDOS /* I don't like this -CFT */
  if (is_wizard(player_uid) && !(peek || wizard)) {
    min_value = (MAX_PLAYER_LEVEL * (m_ptr->hitdie-1))/2 +
      MAX_PLAYER_LEVEL;
    max_value = (MAX_PLAYER_LEVEL*6 * (m_ptr->hitdie-1))/8 +
      MAX_PLAYER_LEVEL;
  }
#endif
  player_hp[0] = m_ptr->hitdie;
  do {
    for (i = 1; i < MAX_PLAYER_LEVEL; i++) {
      player_hp[i] = randint((int)m_ptr->hitdie);
      player_hp[i] += player_hp[i-1];
    }
  }
  while ((player_hp[MAX_PLAYER_LEVEL-1] < min_value) ||
	 (player_hp[MAX_PLAYER_LEVEL-1] > max_value));

  if (peek) {
    percent = (int)( ((long)player_hp[MAX_PLAYER_LEVEL-1]*200L) /
      (m_ptr->hitdie + ((MAX_PLAYER_LEVEL-1)*m_ptr->hitdie)) );
    sprintf(buf, "%d%% Life Rating", percent);
    msg_print(buf);
  }

  m_ptr->bth += c_ptr->mbth;
  m_ptr->bthb += c_ptr->mbthb;	/*RAK*/
  m_ptr->srh += c_ptr->msrh;
  m_ptr->disarm += c_ptr->mdis + todis_adj();
  m_ptr->fos += c_ptr->mfos;
  m_ptr->stl += c_ptr->mstl;
  m_ptr->save += c_ptr->msav;
  m_ptr->expfact += c_ptr->m_exp;
}

void rerate() {
  int min_value, max_value, i, percent;
  char buf[50];
  struct misc *m_ptr = &py.misc;

  min_value = (MAX_PLAYER_LEVEL*3 * (m_ptr->hitdie-1))/8 +
    MAX_PLAYER_LEVEL;
  max_value = (MAX_PLAYER_LEVEL*5 * (m_ptr->hitdie-1))/8 +
    MAX_PLAYER_LEVEL;
  player_hp[0] = m_ptr->hitdie;
  do {
    for (i = 1; i < MAX_PLAYER_LEVEL; i++)
      {
	player_hp[i] = randint((int)m_ptr->hitdie);
	player_hp[i] += player_hp[i-1];
      }
  }
  while ((player_hp[MAX_PLAYER_LEVEL-1] < min_value) ||
	 (player_hp[MAX_PLAYER_LEVEL-1] > max_value));

/* This taken from above...  used here to avoid flt. pt. math -CFT */
    percent = (int)( ((long)player_hp[MAX_PLAYER_LEVEL-1]*200L) /
      (m_ptr->hitdie + ((MAX_PLAYER_LEVEL-1)*m_ptr->hitdie)) );

#if 0 /* this was here before -- I'm trying to avoid floating pt.. -CFT */
  percent = (int) (((double) (player_hp[MAX_PLAYER_LEVEL-1]*100) /
		    (double) ((double)m_ptr->hitdie + 
			      (double)((double)((MAX_PLAYER_LEVEL-1)*
						m_ptr->hitdie)/(double)2))));
#endif
  sprintf(buf, "%d%% Life Rating", percent);
  calc_hitpoints();
  prt_stat_block();
  msg_print(buf);
}

/* Gets a character class				-JWT-	*/
static void get_class_choice()
{
  register int i, j;
  int k, l, m;
  int cl[MAX_CLASS], exit_flag;
  class_type *c_ptr;
  char tmp_str[80], s;
  int32u mask;

  for (j = 0; j < MAX_CLASS; j++)
    cl[j] = 0;
  i = py.misc.prace;
  j = 0;
  k = 0;
  l = 2;
  m = 21;
  mask = 0x1;
  clear_from (20);
  put_buffer("Choose a class (? for Help):", 20, 2);
  do
    {
      if (race[i].rtclass & mask)
	{
	  (void) sprintf(tmp_str, "%c) %s", k+'a', class[j].title);
	  put_buffer(tmp_str, m, l);
	  cl[k] = j;
	  l += 15;
	  if (l > 70)
	    {
	      l = 2;
	      m++;
	    }
	  k++;
	}
      j++;
      mask <<= 1;
    }
  while (j < MAX_CLASS);
  py.misc.pclass = 0;
  exit_flag = FALSE;
  do {
      move_cursor (20, 31);
      s = inkey();
      j = s - 'a';
      if ((j < k) && (j >= 0))
	{
	  py.misc.pclass = cl[j];
	  c_ptr = &class[py.misc.pclass];
	  exit_flag = TRUE;
	  clear_from (20);
	  put_buffer(c_ptr->title, 5, 15);
	}
      else if (s == '?')
        helpfile(ANGBAND_WELCOME);
      else
        bell();
    } while (!exit_flag);
}


/* Given a stat value, return a monetary value, which affects the amount
   of gold a player has. */
static int monval (i)
int16u i;
{
  return 5 * ((int)i - 10);
}


static void get_money()
{
  register int tmp, gold;
  register int16u *a_ptr;

  a_ptr = py.stats.max_stat;
  tmp = monval (a_ptr[A_STR]) + monval (a_ptr[A_INT])
      + monval (a_ptr[A_WIS]) + monval (a_ptr[A_CON])
      + monval (a_ptr[A_DEX]);

  gold = py.misc.sc*6 + randint (25) + 325;	/* Social Class adj */
  gold -= tmp;					/* Stat adj */
  gold += monval (a_ptr[A_CHR]);		/* Charisma adj	*/
  if (!py.misc.male)
    gold += 50;			/* She charmed the banker into it! -CJS- */
                                /* She slept with the banker.. :) -GDH- */
  if (gold < 80)
    gold = 80;			/* Minimum */
  py.misc.au = gold;
}


/* ---------- M A I N  for Character Creation Routine ---------- */
/*							-JWT-	*/
void create_character()
{
  register int exit_flag = 1;
  register char c;
#ifdef AUTOROLLER
  register int auto_round = 0, i;
  int stat[6];
  char inp[60];
#endif
  int previous_exists = 0; /* flag to prevent prev from garbage values */
  
  put_character();
  choose_race();
  get_sex();
  get_class_choice();
  
#ifdef AUTOROLLER
/* This auto-roller stolen from a post on rec.games.moria, which I belive
   was taken from druid moria 5.something.  If this intrudes on someone's
   copyright, take it out, and someone let me know -CFT */
  put_buffer("Do you want to use automatic rolling? (? for Help) ", 20,2);
  do {	/* allow multiple key entry, so they can ask for help and still
  		get back to this menu... -CFT */
    move_cursor(20,52);
    c=inkey();
    if (c == '?')
      helpfile(ANGBAND_WELCOME);
  } while ((c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N'));
  
  if ((c == 'Y') || (c == 'y')){
    clear_from(15);
    put_buffer("Enter minimum attribute for: ",15,2);
    for(i=0;i<6;i++){
      int stat_idx;
      switch(i) {
      case 0:
       stat_idx = A_STR;
       clear_from(16+i);
       put_buffer("    Strength: ",16+i,5);break;
      case 1:
       stat_idx = A_INT;
       clear_from(16+i);
       put_buffer("Intelligence: ",16+i,5);break;
      case 2:
       stat_idx = A_WIS;
       clear_from(16+i);
       put_buffer("      Wisdom: ",16+i,5);break;
      case 3:
       stat_idx = A_DEX;
       clear_from(16+i);
       put_buffer("   Dexterity: ",16+i,5);break;
      case 4:
       stat_idx = A_CON;
       clear_from(16+i);
       put_buffer("Constitution: ",16+i,5);break;
      case 5:
       stat_idx = A_CHR;
       clear_from(16+i);
       put_buffer("    Charisma: ",16+i,5);break;
      } /* switch */
      do{
       exit_flag=0;
       inp[0] = '\000';
       get_string(inp, 16+i, 19, 3);
       stat[stat_idx]=atoi(inp);
      } while (stat[stat_idx]>118 || stat[stat_idx]<0);
      exit_flag=1;
     } /* for i 0 - 5 */
     clear_from(15);
     put_qio();
     do {
      char hlp[64];
      sprintf(hlp, "auto-rolling round #%d.", auto_round);
      put_buffer(hlp, 20 ,2);
      put_qio();
      get_auto_stats();
      get_history();
      get_ahw();
      get_class();
      put_auto_stats();
      auto_round++;
      } while(((stat[A_STR] > py.stats.cur_stat[A_STR]) ||
            (stat[A_INT] > py.stats.cur_stat[A_INT]) ||
  	   (stat[A_DEX] > py.stats.cur_stat[A_DEX]) ||
  	   (stat[A_CHR] > py.stats.cur_stat[A_CHR]) ||
  	   (stat[A_CON] > py.stats.cur_stat[A_CON]) ||
  	   (stat[A_WIS] > py.stats.cur_stat[A_WIS]))
#if (defined(unix) || defined(ATART_ST))
  	    && (!check_input(1)));
#elif (defined(MSDOS) || defined(VMS))  /* #if/elif/else rewritten -CFT */
	    && (!kbhit()));
#else
	    );	    
#endif
     if (kbhit()) flush(); /* -CFT */
     calc_bonuses(); /* need this because it was taken out of the loop -CFT */
     print_history();
     put_misc1();
   } /* if 'y' */    /* end of Sam */
  else { /* use the current code... */
#endif
  /* here we start a loop giving a player a choice of characters -RGM- */
  get_all_stats (); 
  get_history();
  get_ahw();
  print_history();
  put_misc1();
  get_class();
  put_stats();

  clear_from (20);
  do
    {
      if (previous_exists == 1) {
        put_buffer("Hit space: Reroll, ^P: Previous or ESC: Accept: ", 20, 2);
	previous_exists = 2; /* don't bother displaying this hundreds of times -CFT */
        }
      else if (previous_exists == 0)
        put_buffer("Hit space: Reroll, or ESC: Accept: ", 20, 2);
      move_cursor (20, 50);
      c = inkey();
      if (c == ESCAPE) {
	exit_flag = 0;
      } else if (c == ' ') {
        if (!previous_exists) previous_exists = 1; /* once rolled again, we have a previous */
	get_all_stats();
	get_history();
	get_ahw();
	print_history();
	put_misc1();
	get_class();
	put_stats();
      } else if (previous_exists && (c == CTRL('P'))) {
	if (get_prev_stats()) {
	  get_prev_history();
	  get_prev_ahw();
	  print_history();
	  put_misc1();
	  put_stats();
	} else {
	  bell();
	}
      } else {
	bell();
      }
    }		    /* done with stats generation */
  while (exit_flag == 1);
#ifdef AUTOROLLER
  } /* close if-use-auto-else-use-normal clause  -CFT */
#endif
  get_money();
  put_stats();
  put_misc2();
  put_misc3();
  get_name();

  /* This delay may be reduced, but is recommended to keep players	*/
  /* from continuously rolling up characters, which can be VERY	*/
  /* expensive CPU wise.						*/
  pause_exit(23, PLAYER_EXIT_PAUSE);
}

