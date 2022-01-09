/* types.h: global type declarations

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/*
 * some machines will not accept 'signed char' as a type, and some accept it
 * but still treat it like an unsigned character, let's just avoid it,
 * any variable which can ever hold a negative value must be 16 or 32 bits 
 */

#define VTYPESIZ    160
#define BIGVTYPESIZ 300
typedef char vtype[VTYPESIZ];
/* note that since its output can easily exceed 80 characters, objdes must
   always be called with a bigvtype as the first paramter */
typedef char bigvtype[BIGVTYPESIZ];
typedef char stat_type[8];

/* Many of the character fields used to be fixed length, which greatly
   increased the size of the executable.  I have replaced many fixed
   length fields with variable length ones. */

/* all fields are given the smallest possbile type, and all fields are
   aligned within the structure to their natural size boundary, so that
   the structures contain no padding and are minimum size */

/* bit fields are only used where they would cause a large reduction in
   data size, they should not be used otherwise because their use
   results in larger and slower code */

typedef u16b attid;

typedef struct monster_race
{
  cptr name;		    /* Descrip of creature    */
  u32b cmove;			    /* Bit field	      */
  u32b spells;		    /* Creature spells	      */
  u32b cdefense;		    /* Bit field	      */
  u32b spells2;		    /* More creature spells   */
  u32b spells3;		    /* Yes! even More creature spells */
  u32b mexp;			    /* Exp value for kill     */
  u16b sleep;			    /* Inactive counter/10    */
  byte aaf;			    /* Area affect radius     */
  u16b ac;			    /* AC		      */
  byte speed;			    /* Movement speed+10      */
  byte cchar;			    /* Character rep.	      */
  byte hd[2];			    /* Creatures hit die      */
  attid damage[4];		    /* Type attack and damage */
  u16b level;			    /* Level of creature      */
  byte rarity;			    /* Rarity of creature     */
  char gender;			    /* one of 'm','f','n','p' to genderize monsters -CWS */
} monster_race;

typedef struct monster_attack	    /* Monster attack and damage types */
  {
    byte attack_type;		    
    byte attack_desc;
    byte attack_dice;
    byte attack_sides;
  } monster_attack;

typedef struct monster_lore	    /* Monster memories. -CJS- */
  {
    u32b r_cmove;
    u32b r_spells;
    u32b r_spells2;
    u32b r_spells3;
    u16b r_kills, r_deaths;
    u32b r_cdefense;
    byte r_wake, r_ignore;
    byte r_attacks[MAX_MON_NATTACK];
  } monster_lore;

struct unique_mon {
  s32b exist;
  s32b dead;
};

typedef struct describe_mon_type {
  cptr name;
  cptr desc;
  char gender;			    /* one of 'm','f','n','p' to genderize monsters -CWS */
} describe_mon_type;

typedef struct monster_type
{
  s16b hp;			    /* Hit points		*/
  s16b maxhp;			    /* Max Hit points		*/
  s16b csleep;			    /* Inactive counter		*/
  s16b cspeed;			    /* Movement speed		*/
  u16b mptr;			    /* Pointer into creature was u16b	   */
  /* Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256 */
  byte fy;			    /* Y Pointer into map	*/
  byte fx;			    /* X Pointer into map	*/
  byte cdis;			    /* Cur dis from player	*/
  byte ml;
  byte stunned;
  byte confused;
  byte monfear;		    /* Run away! Run away! -DGK */
} monster_type;

typedef struct inven_kind
{
  cptr name;		    /* Object name		      */
  u32b flags;			    /* Special flags		      */
  byte tval;			    /* Category number		      */
  byte tchar;			    /* Character representation	      */
  s16b p1;			    /* Misc. use variable	      */
  s32b cost;			    /* Cost of item		      */
  byte sval;			    /* Sub-category number	      */
  byte number;			    /* Number of items		      */
  u16b weight;		    /* Weight			      */
  s16b tohit;			    /* Plusses to hit		      */
  s16b todam;			    /* Plusses to damage	      */
  s16b ac;			    /* Normal AC		      */
  s16b toac;			    /* Plusses to AC		      */
  byte damage[2];		    /* Damage when hits		      */
  byte level;			    /* Level item first found	      */
  byte rare;			    /* True if Rare		      */
  u32b flags2;		    /* Yes! even more froggin' flags! */
} inven_kind;

/* only damage, ac, and tchar are constant; level could possibly be made
   constant by changing index instead; all are used rarely */
/* extra fields x and y for location in dungeon would simplify pusht() */
/* making inscrip a pointer and mallocing space does not work, there are
   two many places where inven_types are copied, which results in dangling
   pointers, so we use a char array for them instead */
#define INSCRIP_SIZE 13	 /* notice alignment, must be 4*x + 1 */
typedef struct inven_type
{
  u16b index;			    /* Index to k_list	*/
  byte name2;			    /* Object special name	*/
  char inscrip[INSCRIP_SIZE];	    /* Object inscription	*/
  u32b flags;			    /* Special flags		*/
  byte tval;			    /* Category number		*/
  byte tchar;			    /* Character representation */
  s16b p1;			    /* Misc. use variable	*/
  s32b cost;			    /* Cost of item		*/
  byte sval;			    /* Sub-category number	*/
  byte number;			    /* Number of items		*/
  u16b weight;		    /* Weight			*/
  s16b tohit;			    /* Plusses to hit		*/
  s16b todam;			    /* Plusses to damage	*/
  s16b ac;			    /* Normal AC		*/
  s16b toac;			    /* Plusses to AC		*/
  byte damage[2];		    /* Damage when hits		*/
  byte level;			    /* Level item first found	*/
  byte ident;			    /* Identify information	*/
  u32b flags2;		    /* Yes! even more froggin' flags!			*/
  u16b timeout;		    /* How long to wait before reactivating an Artifact */
} inven_type;


typedef struct player_type
{
  struct misc
    {
      char name[27];		    /* Character name	*/
      byte male;		    /* Sex of character */
      s32b au;			    /* Gold		*/
      s32b max_exp;		    /* Max experience	*/
      s32b exp;		    /* Cur experience	*/
      u16b exp_frac;		    /* Cur exp fraction * 2^16	*/
      u16b age;		    /* Characters age	*/
      u16b ht;		    /* Height		*/
      u16b wt;		    /* Weight		*/
      u16b lev;		    /* Level		*/
      u16b max_dlv;		    /* Max level explored	*/
      s16b srh;		    /* Chance in search */
      s16b fos;		    /* Frenq of search	*/
      s16b bth;		    /* Base to hit	*/
      s16b bthb;		    /* BTH with bows	*/
      s16b mana;		    /* Mana points	*/
      s16b mhp;		    /* Max hit pts	*/
      s16b ptohit;		    /* Plusses to hit	*/
      s16b ptodam;		    /* Plusses to dam	*/
      s16b pac;		    /* Total AC		*/
      s16b ptoac;		    /* Magical AC	*/
      s16b dis_th;		    /* Display +ToHit	*/
      s16b dis_td;		    /* Display +ToDam	*/
      s16b dis_ac;		    /* Display +ToAC	*/
      s16b dis_tac;		    /* Display +ToTAC	*/
      s16b disarm;		    /* % to Disarm	*/
      s16b save;		    /* Saving throw	*/
      s16b sc;			    /* Social Class	*/
      s16b stl;		    /* Stealth factor	*/
      byte pclass;		    /* # of class	*/
      byte prace;		    /* # of race	*/
      byte hitdie;		    /* Char hit die	*/
      byte expfact;		    /* Experience factor	*/
      s16b cmana;		    /* Cur mana pts		*/
      u16b cmana_frac;	    /* Cur mana fraction * 2^16 */
      s16b chp;		    /* Cur hit pts		*/
      u16b chp_frac;		    /* Cur hit fraction * 2^16	*/
      char history[4][60];	    /* History record		*/
    } misc;

/* Stats now kept in arrays, for more efficient access. -CJS- */
  struct stats
    {
      u16b max_stat[6];	    /* What is restored		    */
      byte cur_stat[6];	    /* What is natural		    */
      s16b mod_stat[6];	    /* What is modified, may be +/- */
      u16b use_stat[6];	    /* What is used		    */
    } stats;
  struct flags
    {
      u32b status;		    /* Status of player	   */
      s16b rest;		    /* Rest counter	   */
      s16b blind;		    /* Blindness counter   */
      s16b paralysis;		    /* Paralysis counter   */
      s16b confused;		    /* Confusion counter   */
      s16b food;		    /* Food counter	   */
      s16b food_digested;	    /* Food per round	   */
      s16b protection;		    /* Protection fr. evil */
      s16b speed;		    /* Cur speed adjust	   */
      s16b fast;		    /* Temp speed change   */
      s16b slow;		    /* Temp speed change   */
      s16b afraid;		    /* Fear		   */
      s16b cut;		    /* Wounds		   */
      s16b stun;		    /* Stunned player	   */
      s16b poisoned;		    /* Poisoned		   */
      s16b image;		    /* Hallucinate	   */
      s16b protevil;		    /* Protect VS evil	   */
      s16b invuln;		    /* Increases AC	   */
      s16b hero;		    /* Heroism		   */
      s16b shero;		    /* Super Heroism	   */
      s16b shield;		    /* Shield Spell	   */
      s16b blessed;		    /* Blessed		   */
      s16b oppose_fire;	    /* Timed heat resist   */
      s16b oppose_cold;	    /* Timed cold resist   */
      s16b oppose_acid;	    /* Timed acid resist   */
      s16b oppose_elec;	    /* Timed light resist  */
      s16b oppose_pois;	    /* Timed poison resist */
      s16b detect_inv;		    /* Timed see invisible */
      s16b word_recall;	    /* Timed teleport level*/
      s16b see_infra;		    /* See warm creatures  */
      s16b tim_infra;		    /* Timed infra vision  */
      byte see_inv;		    /* Can see invisible   */
      byte teleport;		    /* Random teleportation*/
      byte free_act;		    /* Never paralyzed	   */
      byte slow_digest;	    /* Lower food needs	   */
      byte aggravate;		    /* Aggravate monsters  */
      byte resist_fire;	    /* Resistance to fire  */
      byte resist_cold;	    /* Resistance to cold  */
      byte resist_acid;	    /* Resistance to acid  */
      byte regenerate;		    /* Regenerate hit pts  */
      byte resist_elec;	    /* Resistance to light */
      byte ffall;		    /* No damage falling   */
      byte sustain_str;	    /* Keep strength	   */
      byte sustain_int;	    /* Keep intelligence   */
      byte sustain_wis;	    /* Keep wisdom	   */
      byte sustain_con;	    /* Keep constitution   */
      byte sustain_dex;	    /* Keep dexterity	   */
      byte sustain_chr;	    /* Keep charisma	   */
      byte confuse_monster;	    /* Glowing hands.	   */
      byte new_spells;		    /* Number of spells can learn. */
      byte resist_pois;	    /* Resistance to poison	   */
      byte hold_life;		    /* Immune to life draining	   */
      byte telepathy;		    /* Gives telepathy	   */
      byte immune_fire;		    /* Immune to fire	   */
      byte immune_acid;		    /* Immune to acid	   */
      byte immune_pois;		    /* Immune to poison	   */
      byte immune_cold;		    /* Immune to cold	   */
      byte immune_elec;		    /* Immune to lightning */
      byte light;		    /* Permanent light	   */
      byte resist_conf;	    /* Resist confusion	   */
      byte resist_sound;	    /* Resist sound	   */
      byte resist_lite;	    /* Resist light	   */
      byte resist_dark;	    /* Resist darkness	   */
      byte resist_chaos;	    /* Resist chaos	   */
      byte resist_disen;	    /* Resist disenchant   */
      byte resist_shards;	    /* Resist shards	   */
      byte resist_nexus;	    /* Resist nexus	   */
      byte resist_blind;	    /* Resist blindness	   */
      byte resist_nether;	    /* Resist nether	   */
      byte resist_fear;	    /* Resist fear	   */
    } flags;
} player_type;


typedef struct spell_type
{  /* spell name is stored in spell_names[] array at index i, +31 if priest */
  byte slevel;
  byte smana;
  byte sfail;
  u16b sexp;			    /* 1/4 of exp gained for learning spell */
} spell_type;


typedef struct player_race
{
  cptr trace;			    /* Type of race	    */
  s16b str_adj;		    /* adjustments	    */
  s16b int_adj;		    
  s16b wis_adj;
  s16b dex_adj;
  s16b con_adj;
  s16b chr_adj;
  byte b_age;			    /* Base age of character	 */
  byte m_age;			    /* Maximum age of character	 */
  byte m_b_ht;			    /* base height for males	 */
  byte m_m_ht;			    /* mod height for males	 */
  byte m_b_wt;			    /* base weight for males	 */
  byte m_m_wt;			    /* mod weight for males	 */
  byte f_b_ht;			    /* base height females	 */
  byte f_m_ht;			    /* mod height for females	 */
  byte f_b_wt;			    /* base weight for female	 */
  byte f_m_wt;			    /* mod weight for females	 */
  s16b b_dis;			    /* base chance to disarm	 */
  s16b srh;			    /* base chance for search	 */
  s16b stl;			    /* Stealth of character	 */
  s16b fos;			    /* frequency of auto search	 */
  s16b bth;			    /* adj base chance to hit	 */
  s16b bthb;			    /* adj base to hit with bows */
  s16b bsav;			    /* Race base for saving throw*/
  byte bhitdie;		    /* Base hit points for race	 */
  byte infra;			    /* See infra-red		 */
  byte b_exp;			    /* Base experience factor	 */
  byte rtclass;		    /* Bit field for class types */
} player_race;

typedef struct player_class
{
  cptr title;			    /* type of class		       */
  byte adj_hd;			    /* Adjust hit points	       */
  byte mdis;			    /* mod disarming traps	       */
  byte msrh;			    /* modifier to searching	       */
  byte mstl;			    /* modifier to stealth	       */
  byte mfos;			    /* modifier to freq-of-search      */
  byte mbth;			    /* modifier to base to hit	       */
  byte mbthb;			    /* modifier to base to hit - bows  */
  byte msav;			    /* Class modifier to save	       */
  s16b madj_str;		    /* Class modifier for strength     */
  s16b madj_int;		    /* Class modifier for intelligence */
  s16b madj_wis;		    /* Class modifier for wisdom       */
  s16b madj_dex;		    /* Class modifier for dexterity    */
  s16b madj_con;		    /* Class modifier for constitution */
  s16b madj_chr;		    /* Class modifier for charisma     */
  byte spell;			    /* class use mage spells	       */
  byte m_exp;			    /* Class experience factor	       */
  byte first_spell_lev;	    /* First level where class can use spells.	       */
  byte age_adj;		    /* %age, warrior =100, so diff classes age faster. */
} player_class;

typedef struct player_background
{
  cptr info;			    /* History information	    */
  byte roll;			    /* Die roll needed for history  */
  byte chart;			    /* Table number		    */
  byte next;			    /* Pointer to next table	    */
  byte bonus;			    /* Bonus to the Social Class+50 */
} player_background;

typedef struct cave_type
{
  u16b cptr;
  u16b tptr;
  byte fval;
  unsigned int lr : 1;		    /* room should be lit with perm light, walls with
				       this set should be perm lit after tunneled out	 */

  unsigned int fm : 1;		    /* field mark, used for traps/doors/stairs, object is
				       hidden if fm is FALSE				 */

  unsigned int pl : 1;		    /* permanent light, used for walls and lighted rooms */
  unsigned int tl : 1;		    /* temporary light, used for player's lamp light,etc.*/
} cave_type;

typedef struct owner_type
{
  cptr owner_name;
  s16b max_cost;
  byte max_inflate;
  byte min_inflate;
  byte haggle_per;
  byte owner_race;
  byte insult_max;
} owner_type;

typedef struct inven_record
{
  s32b scost;
  inven_type sitem;
} inven_record;

typedef struct store_type
{
  s32b store_open;
  s16b insult_cur;
  byte owner;
  byte store_ctr;
  u16b good_buy;
  u16b bad_buy;
  inven_record store_inven[STORE_INVEN_MAX];
} store_type;

typedef struct high_scores
{
  s32b points;
  u16b lev;
  u16b max_lev;
  s16b mhp;
  s16b chp;
  s16b uid;
  s16b dun_level;
  byte sex;
  vtype name;
  vtype died_from;
  byte pclass;
  byte prace;
} high_scores;
