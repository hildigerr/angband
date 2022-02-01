/* File: birth.c */

/* Purpose: create a player character */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static void change_stat(int, int);
static int  monval(int);
static void get_stats();
static void save_prev_data();
static int  load_prev_data();
static void get_all_stats();
static void put_auto_stats();
static void choose_race();
static void put_history();
static void set_prev_history();
static void get_prev_history();
static void choose_sex();
static void get_ahw();
static void set_prev_ahw();
static void get_prev_ahw();
static void get_class();
static void choose_class();
static void get_money();

/*
 * Hold the data from the previous "roll"
 */
static struct {

    u16b age;
    u16b wt;
    u16b ht;
    s16b  disarm;
    u16b stat[6];
    u16b sc;

    char history[4][60];

    player_background bg;

} prev;

extern int peek;

/* Generates character's stats			-JWT-	 */
static void get_stats()
{
    register int i, tot;
    int dice[18];

    do {
	tot = 0;
	for (i = 0; i < 18; i++) {
	    dice[i] = randint(3 + i % 3); /* Roll 3,4,5 sided dice once each */
	    tot += dice[i];
	}
    }
    while (tot <= 42 || tot >= 54);
    
    for (i = 0; i < 6; i++)
	p_ptr->max_stat[i] = 5 + dice[3 * i] + dice[3 * i + 1] +
	    dice[3 * i + 2];
}


/* Returns adjusted stat                                       -JK-  */
/* Algorithm by ...                                            -JWT- */
/* Used by change_stats and auto_roller
 * auto_roll is boolean and states maximum changes
 * should be used rather than random ones
 * to allow specification of higher values to wait for
 */

static int adjust_stat(int stat_value, s16b amount, int auto_roll)
{
  register int i;
 
  if (amount < 0)
    for (i = 0; i > amount; i--)
      {
/* OK so auto_roll conditions not needed for negative amounts since stat_value
 * is always 15 at least currently!  -JK
 */
                
             if (stat_value > 108)
               stat_value--;
             else if (stat_value > 88)
                 stat_value -= ((auto_roll ? 6 : randint(6)) + 2);
             else if (stat_value > 18)
               {
                 stat_value -= ((auto_roll ? 15 : randint(15)) + 5);
                 if (stat_value < 18)
                   stat_value = 18;
               }
             else if (stat_value > 3)
               stat_value--;
      }
  else
    for (i = 0; i < amount; i++)
      {
        if (stat_value < 18)
          stat_value++;
        else if (stat_value < 88)
          stat_value += ((auto_roll ? 15 : randint(15)) + 5);
        else if (stat_value < 108)
          stat_value += ((auto_roll ? 6 : randint(6)) + 2);
        else if (stat_value < 118)
          stat_value++;
      }
  return stat_value;
}


/* Changes stats by given amount                                -JWT-   */
static void change_stat(int stat, int amount)
{
  p_ptr->max_stat[stat] =
        adjust_stat(p_ptr->max_stat[stat], (s16b) amount, FALSE);
}


/*
 * Save the current data for later
 */
static void save_prev_data()
{
    register int i;

    /* Save the stats */
    for (i = 0; i < 6; i++) {
	prev.stat[i] = (u16b) p_ptr->max_stat[i];
    }

    return;
}


/*
 * Load the previous data
 */
static int load_prev_data()
{
    register int        i;

    if (!prev.stat[0]) return 0;
    for (i = 0; i < 6; i++) {
	p_ptr->cur_stat[i] = prev.stat[i];
	p_ptr->max_stat[i] = prev.stat[i];
	p_ptr->use_stat[i] = prev.stat[i];
    }

    p_ptr->ptodam = todam_adj();
    p_ptr->ptohit = tohit_adj();
    p_ptr->pac = toac_adj();
    prev.stat[0] = 0;
    return 1;
}


/*
 * generate all stats and modify for race. needed in a separate module so
 * looping of character selection would be allowed     -RGM- 
 */
static void get_all_stats()
{
    register player_race *r_ptr;
    register int        j;

    r_ptr = &race[p_ptr->prace];
    get_stats();
    change_stat(A_STR, r_ptr->str_adj);
    change_stat(A_INT, r_ptr->int_adj);
    change_stat(A_WIS, r_ptr->wis_adj);
    change_stat(A_DEX, r_ptr->dex_adj);
    change_stat(A_CON, r_ptr->con_adj);
    change_stat(A_CHR, r_ptr->chr_adj);
    for (j = 0; j < 6; j++) {
	p_ptr->cur_stat[j] = p_ptr->max_stat[j];
	p_ptr->use_stat[j] = modify_stat(j, p_ptr->mod_stat[j]);
    }

    p_ptr->srh = r_ptr->srh;
    p_ptr->bth = r_ptr->bth;
    p_ptr->bthb = r_ptr->bthb;
    p_ptr->fos = r_ptr->fos;
    p_ptr->stl = r_ptr->stl;
    p_ptr->save = r_ptr->bsav;
    p_ptr->hitdie = r_ptr->bhitdie;
    p_ptr->lev = 1;
    p_ptr->ptodam = todam_adj();
    p_ptr->ptohit = tohit_adj();
    p_ptr->ptoac = 0;
    p_ptr->pac = toac_adj();
    p_ptr->expfact = r_ptr->b_exp;
    p_ptr->see_infra = r_ptr->infra;
}

/* copied from misc2.c, so the display loop would work nicely -cft */
static const char *stat_names[] = {"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "};


#ifdef AUTOROLLER
/* used for auto-roller.  Just put_stats(), w/o the extra info -CFT */
static void put_auto_stats()
{
    register int i;
    vtype        buf;

    for (i = 0; i < 6; i++) {
	cnv_stat(p_ptr->use_stat[i], buf);
	put_str(stat_names[i], 2 + i, 61);
	put_str(buf, 2 + i, 66);
	if (p_ptr->max_stat[i] > p_ptr->cur_stat[i]) {
	    cnv_stat(p_ptr->max_stat[i], buf);
	    put_str(buf, 2 + i, 73);
	}
    }
}
#endif

/*
 * Allows player to select a race			-JWT-	 
 */
static void choose_race(void)
{
    int                  j, k, l, m;
    int                  exit_flag;
    char                 s, tmp_str[80];
    register player_race   *r_ptr;

    j = 0;
    k = 0;
    l = 2;
    m = 21;

    clear_from(20);

    put_str("Choose a race (? for Help):", 20, 2);
    do {
	(void)sprintf(tmp_str, "%c) %s", k + 'a', race[j].trace);
	put_str(tmp_str, m, l);
	k++;
	l += 15;
	if (l > 70) {
	    l = 2;
	    m++;
	}
	j++;
    }

    while (j < MAX_RACES);
    exit_flag = FALSE;
    do {
	move_cursor(20, 30);
	s = inkey();
	j = s - 'a';
	if ((j < MAX_RACES) && (j >= 0)) {
	    exit_flag = TRUE;
	}
	else if (s == '?') {
	    helpfile(ANGBAND_WELCOME);
	}
	else {
	    bell();
	}
    }
    while (!exit_flag);

    r_ptr = &race[j];
    p_ptr->prace = j;
    put_str(r_ptr->trace, 3, 15);
}


/* Will print the history of a character			-JWT-	 */
static void put_history()
{
    register int        i;

    put_str("Character Background", 14, 27);
    for (i = 0; i < 4; i++)
	prt(p_ptr->history[i], i + 15, 10);
}


static void set_prev_history()
{
    prev.bg.info = background->info;
    prev.bg.roll = background->roll;
    prev.bg.chart = background->chart;
    prev.bg.next = background->next;
    prev.bg.bonus = background->bonus;
    prev.sc = p_ptr->sc;

    (void)strncpy(prev.history[0], p_ptr->history[0], 60);
    (void)strncpy(prev.history[1], p_ptr->history[1], 60);
    (void)strncpy(prev.history[2], p_ptr->history[2], 60);
    (void)strncpy(prev.history[3], p_ptr->history[3], 60);

    return;
}


static void get_prev_history()
{
    register int        i;

    background->info = prev.bg.info;
    background->roll = prev.bg.roll;
    background->chart = prev.bg.chart;
    background->next = prev.bg.next;
    background->bonus = prev.bg.bonus;
    p_ptr->sc = prev.sc;

    for (i = 0; i < 4; i++)
	strncpy(p_ptr->history[i], prev.history[i], 60);
}


/*
 * Get the racial history, determines social class	-RAK-
 * Assumptions:	Each race has init history beginning at
 * (race-1)*3+1.  All history parts are in ascending order
 */

static void get_history(void)
{
    int                      hist_idx, cur_idx, test_roll, flag;
    register int             start_pos, end_pos, cur_len;
    int                      line_ctr, new_start = 0, social_class;
    char                     history_block[240];
    player_background		*bp_ptr;

    /* Special race */
    if (p_ptr->prace == 8) {
	hist_idx = 1;
    }

    /* Special race */
    else if (p_ptr->prace > 8) {
	hist_idx = 2 * 3 + 1;
    }

    /* Normal races */
    else {
	hist_idx = p_ptr->prace * 3 + 1;
    }

    history_block[0] = '\0';
    social_class = randint(4);
    cur_idx = 0;

    /* Process the history */
    do {
	flag = FALSE;
	do {
	    if (background[cur_idx].chart == hist_idx) {
		test_roll = randint(100);
		while (test_roll > background[cur_idx].roll) cur_idx++;
		bp_ptr = &background[cur_idx];
		(void)strcat(history_block, bp_ptr->info);
		social_class += bp_ptr->bonus - 50;
		if (hist_idx > bp_ptr->next) cur_idx = 0;
		hist_idx = bp_ptr->next;
		flag = TRUE;
	    }
	    else {
		cur_idx++;
	    }
	}
	while (!flag);
    }
    while (hist_idx >= 1);

    /* clear the previous history strings */
    for (hist_idx = 0; hist_idx < 4; hist_idx++) {
	p_ptr->history[hist_idx][0] = '\0';
    }

    /* Process block of history text for pretty output	 */
    start_pos = 0;
    end_pos = strlen(history_block) - 1;
    line_ctr = 0;
    flag = FALSE;
    while (history_block[end_pos] == ' ') end_pos--;

    do {
	while (history_block[start_pos] == ' ') start_pos++;
	cur_len = end_pos - start_pos + 1;
	if (cur_len > 60) {
	    cur_len = 60;
	    while (history_block[start_pos + cur_len - 1] != ' ') cur_len--;
	    new_start = start_pos + cur_len;
	    while (history_block[start_pos + cur_len - 1] == ' ') cur_len--;
	}
	else {
	    flag = TRUE;
	}

	(void)strncpy(p_ptr->history[line_ctr],
		&history_block[start_pos], cur_len);
	p_ptr->history[line_ctr][cur_len] = '\0';
	line_ctr++;
	start_pos = new_start;
    }
    while (!flag);

    /* Verify social class */
    if (social_class > 100) social_class = 100;
    else if (social_class < 1) social_class = 1;

    /* Save the social class */
    p_ptr->sc = social_class;
}


/* Gets the character's sex				-JWT-	 */
static void choose_sex()
{
    register int        exit_flag;
    char                c;

    exit_flag = FALSE;
    clear_from(20);
    put_str("Choose a sex (? for Help):", 20, 2);
    put_str("m) Male       f) Female", 21, 2);
    do {
	move_cursor(20, 29);
    /* speed not important here */
	c = inkey();
	if (c == 'f' || c == 'F') {
	    p_ptr->male = FALSE;
	    put_str("Female", 4, 15);
	    exit_flag = TRUE;
	} else if (c == 'm' || c == 'M') {
	    p_ptr->male = TRUE;
	    put_str("Male", 4, 15);
	    exit_flag = TRUE;
	} else if (c == '?')
	    helpfile(ANGBAND_WELCOME);
	else
	    bell();
    }
    while (!exit_flag);
}


/* Computes character's age, height, and weight		-JWT-	 */
static void get_ahw()
{
    register int        i;

    i = p_ptr->prace;
    p_ptr->age = race[i].b_age + randint((int)race[i].m_age);
    if (p_ptr->male) {
	p_ptr->ht = randnor((int)race[i].m_b_ht, (int)race[i].m_m_ht);
	p_ptr->wt = randnor((int)race[i].m_b_wt, (int)race[i].m_m_wt);
    } else {
	p_ptr->ht = randnor((int)race[i].f_b_ht, (int)race[i].f_m_ht);
	p_ptr->wt = randnor((int)race[i].f_b_wt, (int)race[i].f_m_wt);
    }
    p_ptr->disarm += race[i].b_dis;
}


static void set_prev_ahw()
{
    prev.age = p_ptr->age;
    prev.wt = p_ptr->wt;
    prev.ht = p_ptr->ht;
    prev.disarm = p_ptr->disarm;

    return;
}


static void get_prev_ahw()
{
    p_ptr->age = prev.age;
    p_ptr->wt = prev.wt;
    p_ptr->ht = prev.ht;
    p_ptr->disarm = prev.disarm;
    prev.age = prev.wt = prev.ht = prev.disarm = 0;
}


/* Gets a character class				-JWT-	 */
static void get_class()
{
    register int        i;
    int                 min_value, max_value;
    int                 percent;
    char                buf[50];
    player_class         *c_ptr;

    c_ptr = &class[p_ptr->pclass];
    change_stat(A_STR, c_ptr->madj_str);
    change_stat(A_INT, c_ptr->madj_int);
    change_stat(A_WIS, c_ptr->madj_wis);
    change_stat(A_DEX, c_ptr->madj_dex);
    change_stat(A_CON, c_ptr->madj_con);
    change_stat(A_CHR, c_ptr->madj_chr);

    for (i = 0; i < 6; i++) {
	p_ptr->p_ptr->cur_stat[i] = p_ptr->p_ptr->max_stat[i];
	p_ptr->p_ptr->use_stat[i] = p_ptr->p_ptr->max_stat[i];
    }
    p_ptr->ptodam = todam_adj();           /* Real values		 */
    p_ptr->ptohit = tohit_adj();
    p_ptr->ptoac = toac_adj();
    p_ptr->pac = 0;
    p_ptr->dis_td = p_ptr->ptodam;	/* Displayed values	 */
    p_ptr->dis_th = p_ptr->ptohit;
    p_ptr->dis_tac = p_ptr->ptoac;
    p_ptr->dis_ac = p_ptr->pac + p_ptr->dis_tac;

/* now set misc stats, do this after setting stats because of con_adj() for
 * hitpoints 
 */
    p_ptr->hitdie += c_ptr->adj_hd;
    p_ptr->mhp = con_adj() + p_ptr->hitdie;
    p_ptr->chp = p_ptr->mhp;
    p_ptr->chp_frac = 0;

/* initialize hit_points array: put bounds on total possible hp,
 * only succeed if it is within 1/8 of average value
 */
    min_value = (MAX_PLAYER_LEVEL * 3 * (p_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    max_value = (MAX_PLAYER_LEVEL * 5 * (p_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;

    player_hp[0] = p_ptr->hitdie;
    do {
	for (i = 1; i < MAX_PLAYER_LEVEL; i++) {
	    player_hp[i] = randint((int)p_ptr->hitdie);
	    player_hp[i] += player_hp[i - 1];
	}
    }
    while ((player_hp[MAX_PLAYER_LEVEL - 1] < min_value) ||
	   (player_hp[MAX_PLAYER_LEVEL - 1] > max_value));

    if (peek) {
	percent = (int)(((long)player_hp[MAX_PLAYER_LEVEL - 1] * 200L) /
		(p_ptr->hitdie + ((MAX_PLAYER_LEVEL - 1) * p_ptr->hitdie)));
	sprintf(buf, "%d%% Life Rating", percent);
	msg_print(buf);
    }
    p_ptr->bth += c_ptr->mbth;
    p_ptr->bthb += c_ptr->mbthb;   /* RAK */
    p_ptr->srh += c_ptr->msrh;
    p_ptr->disarm = c_ptr->mdis + todis_adj();
    p_ptr->fos += c_ptr->mfos;
    p_ptr->stl += c_ptr->mstl;
    p_ptr->save += c_ptr->msav;
    p_ptr->expfact += c_ptr->m_exp;
}

void rerate()
{
    int         min_value, max_value, i, percent;
    char        buf[50];

    min_value = (MAX_PLAYER_LEVEL * 3 * (p_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    max_value = (MAX_PLAYER_LEVEL * 5 * (p_ptr->hitdie - 1)) / 8 +
	MAX_PLAYER_LEVEL;
    player_hp[0] = p_ptr->hitdie;
    do {
	for (i = 1; i < MAX_PLAYER_LEVEL; i++) {
	    player_hp[i] = randint((int)p_ptr->hitdie);
	    player_hp[i] += player_hp[i - 1];
	}
    }
    while ((player_hp[MAX_PLAYER_LEVEL - 1] < min_value) ||
	   (player_hp[MAX_PLAYER_LEVEL - 1] > max_value));

    percent = (int)(((long)player_hp[MAX_PLAYER_LEVEL - 1] * 200L) /
		(p_ptr->hitdie + ((MAX_PLAYER_LEVEL - 1) * p_ptr->hitdie)));

    sprintf(buf, "%d%% Life Rating", percent);
    calc_hitpoints();
    prt_stat_block();
    msg_print(buf);
}

/* Gets a character class				-JWT-	 */
static void choose_class()
{
    register int i, j;
    int          k, l, m;
    int          cl[MAX_CLASS], exit_flag;
    player_class   *c_ptr;
    char         tmp_str[80], s;
    u32b       mask;

    for (j = 0; j < MAX_CLASS; j++)
	cl[j] = 0;
    i = p_ptr->prace;
    j = 0;
    k = 0;
    l = 2;
    m = 21;
    mask = 0x1;
    clear_from(20);
    put_str("Choose a class (? for Help):", 20, 2);
    do {
	if (race[i].rtclass & mask) {
	    (void)sprintf(tmp_str, "%c) %s", k + 'a', class[j].title);
	    put_str(tmp_str, m, l);
	    cl[k] = j;
	    l += 15;
	    if (l > 70) {
		l = 2;
		m++;
	    }
	    k++;
	}
	j++;
	mask <<= 1;
    }
    while (j < MAX_CLASS);
    p_ptr->pclass = 0;
    exit_flag = FALSE;
    do {
	move_cursor(20, 31);
	s = inkey();
	j = s - 'a';
	if ((j < k) && (j >= 0)) {
	    p_ptr->pclass = cl[j];
	    c_ptr = &class[p_ptr->pclass];
	    exit_flag = TRUE;
	    clear_from(20);
	    put_str(c_ptr->title, 5, 15);
	} else if (s == '?')
	    helpfile(ANGBAND_WELCOME);
	else
	    bell();
    } while (!exit_flag);
}


/* Given a stat value, return a monetary value, which affects the amount of
 * gold a player has. 
 */
static int monval(i)
    int                 i;
{
    return 5 * ((int)i - 10);
}


static void get_money()
{
    register int        tmp, gold;
    register u16b    *a_ptr;

    a_ptr = p_ptr->max_stat;
    tmp = monval(a_ptr[A_STR]) + monval(a_ptr[A_INT])
	+ monval(a_ptr[A_WIS]) + monval(a_ptr[A_CON])
	+ monval(a_ptr[A_DEX]);

    gold = p_ptr->sc * 6 + randint(25) + 325;          /* Social Class adj */
    gold -= tmp;		   /* Stat adj */
    gold += monval(a_ptr[A_CHR]);  /* Charisma adj	 */
    if (!p_ptr->male)
	gold += 50;		   /* She charmed the banker into it! -CJS- */
				   /* She slept with the banker.. :) -GDH-  */
    if (gold < 80)
	gold = 80;		   /* Minimum */
    p_ptr->au = gold;
}


/* ---------- M A I N  for Character Creation Routine ---------- */
/* -JWT-	 */
void player_birth()
{
    char		c;

    player_class	*cp_ptr;
    player_race		*rp_ptr;

#ifdef AUTOROLLER

    u32b		auto_round = 0;
    register int	i;
    int			stat[6];
    int			autoroll = 0;
    int			msstat = 0;/* Max autoroll w/ look for -SAC */
    char		inp[60];

#endif
    int previous_exists = 0;	/* flag to prevent prev from garbage values */

    town_seed = random();	/* Change random seeds for new characters -CWS */
    randes_seed = random();

    put_character();

    /* Choose a race */
    choose_race();

    /* Choose a sex */
    choose_sex();

    /* Choose a class */
    choose_class();

    /* Access the race/class */
    cp_ptr = &class[p_ptr->pclass];
    rp_ptr = &race[p_ptr->prace];


#ifdef AUTOROLLER

/*
 * This auto-roller stolen from a post on rec.games.moria, which I belive was
 * taken from druid moria 5.something.  If this intrudes on someone's
 * copyright, take it out, and someone let me know -CFT 
 */

    /* Prompt for it */
    put_str("Do you want to use automatic rolling? (? for Help) ", 20, 2);

    do {   /* allow multiple key entry, so they can ask for help and
            * still get back to this menu... -CFT */

	move_cursor(20, 52);
	c = inkey();
	if (c == '?') helpfile(ANGBAND_WELCOME);
    } while ((c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N'));


    /* Prepare the autoroller */
    if ((c == 'Y') || (c == 'y')) {

	autoroll = TRUE;

	clear_from(15);
	put_str("Enter minimum attribute for: ", 15, 2);

	/* Check the stats */
	for (i = 0; i < 6; i++) {
	    int                 stat_idx = 0;

	    switch (i) {
	      case 0:
		stat_idx = A_STR;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_str + rp_ptr->str_adj, TRUE);
		sprintf(inp, "    Strength (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	      case 1:
		stat_idx = A_INT;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_int + rp_ptr->int_adj, TRUE);
		sprintf(inp, "Intelligence (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	      case 2:
		stat_idx = A_WIS;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_wis + rp_ptr->wis_adj, TRUE);
		sprintf(inp, "      Wisdom (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	      case 3:
		stat_idx = A_DEX;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_dex + rp_ptr->dex_adj, TRUE);
		sprintf(inp, "   Dexterity (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	      case 4:
		stat_idx = A_CON;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_con + rp_ptr->con_adj, TRUE);
		sprintf(inp, "Constitution (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	      case 5:
		stat_idx = A_CHR;
		clear_from(16 + i);
		msstat = adjust_stat(17, cp_ptr->madj_chr + rp_ptr->chr_adj, TRUE);
		sprintf(inp, "    Charisma (Max of %2d): ", msstat);
		put_str(inp, 16 + i, 5);
		break;
	    }

	    do {
		inp[0] = '\000';
		get_string(inp, 16 + i, 32, 3);
		stat[stat_idx] = atoi(inp); /* have return give a stat of 3 */
		if (inp[0] == '\015' || inp[0] == '\012' || inp[0] == '\000')
		    stat[stat_idx] = 3;

		/* Use negative numbers to avoid "max stat" setting */
		if (stat[stat_idx] < 0) {
		    stat[stat_idx] = (-stat[stat_idx]);
		    if (stat[stat_idx] > msstat) msstat = stat[stat_idx];
		}
	    } while (stat[stat_idx] > msstat || stat[stat_idx] < 3);
	} /* for i 0 - 5 */
	put_qio();
    }

#endif				   /* AUTOROLLER - main setup code */


    /* Actually Generate */
    do {			   /* Main generation loop */

	/* Clear the old data */
	clear_from(9);

#ifdef AUTOROLLER

	if (autoroll)
	    for (i = 2; i < 6; i++)
		erase_line(i, 30);

	/* Start of AUTOROLLing loop */
	do {

#endif

	    /* Get a new character */
	    get_all_stats();
	    get_class();

#ifdef AUTOROLLER
	    if (autoroll) {
		put_auto_stats();

	    /* Advance the round */
	    auto_round++;

		sprintf(inp, "auto-rolling round #%lu.", (long)auto_round);
		put_str(inp, 20, 2);

#if defined(unix) && defined(NICE)
		usleep((long)100000L);
#endif

		put_qio();
	    } else
		put_stats();
	} while ((autoroll) &&
		 ((stat[A_STR] > p_ptr->cur_stat[A_STR]) ||
		  (stat[A_INT] > p_ptr->cur_stat[A_INT]) ||
		  (stat[A_WIS] > p_ptr->cur_stat[A_WIS]) ||
		  (stat[A_DEX] > p_ptr->cur_stat[A_DEX]) ||
		  (stat[A_CON] > p_ptr->cur_stat[A_CON]) ||
		  (stat[A_CHR] > p_ptr->cur_stat[A_CHR]))

#if (defined (unix) || defined(ATARI_ST)) /* CFT's if/elif/else    */
		 && (!check_input(1)));	  /* unix needs flush here */
#elif (defined(MSDOS) || defined(VMS))
	&&(!kbhit()));
    if (kbhit())
	    flush();
#else
    );
#endif				   /* character checks */
#endif				   /* AUTOROLLER main looping section */
       get_history();	           /* Common stuff */
       get_ahw();

	calc_bonuses();
	put_history();
	put_misc1();
	clear_from(20);

	do {			   /* Input loop */
	    if (previous_exists) {
		put_str("Hit space: Reroll, ^P: Previous or ESC: Accept: ",
			   20, 2);
		move_cursor(20, 50);
	    } else {
		put_str("Hit space: Reroll, or ESC: Accept: ", 20, 2);
		move_cursor(20, 37);
	    }
	    c = inkey();
	    if ((previous_exists) && (c == CTRL('P'))) {
		previous_exists = FALSE;
		if (load_prev_data()) {
		    get_prev_history();
		    get_prev_ahw();
		    put_history();
		    put_misc1();
		    calc_bonuses();
		    put_stats();
		    clear_from(20);
		}
	    } else if ((c != ' ') && (c != ESCAPE))	/* Prolly better way to do this */
		bell();
	} while ((c != ' ') && (c != ESCAPE));

/* Not going to waste space w/ a check here. So ESC takes a little longer. -SAC */
	save_prev_data();
	set_prev_history();
	set_prev_ahw();
	previous_exists = TRUE;
    } while (c == ' ');
    get_money();
    put_stats();
    put_misc2();
    put_misc3();
    get_name();
    msg_print(NULL);

/*
 * Pauses for user response before returning		-RAK-	 
 * NOTE: Delay is for players trying to roll up "perfect"	 
 * characters.  Make them wait a bit.			
 * This delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.
 */
    /* Prompt for it */    
    prt("[Press any key to continue, or Q to exit.]", 23, 10);

    /* Get a key */
    c = inkey();

    if (c == 'Q') {
	erase_line(23, 0);
#ifndef MSDOS			   /* PCs are slow enough as is  -dgk */
	if (PLAYER_EXIT_PAUSE > 0)
	    (void)sleep((unsigned)PLAYER_EXIT_PAUSE);
#else
#endif
#ifdef MAC
	enablefilemenu(FALSE);
	exit_game();
	enablefilemenu(TRUE);
#else
	exit_game();
#endif
    }
    erase_line(23, 0);
}


