/* File: generate.c */

/* Purpose: initialize/create a dungeon or town level */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


static void correct_dir(int *, int *, int, int, int, int);
static void rand_dir(int *, int *);
static void fill_cave(int);
static void place_streamer(int, int);
static cave_type *test_place_obj(int, int);
static void place_open_door(int, int);
static void place_broken_door(int, int);
static void place_closed_door(int, int);
static void place_locked_door(int, int);
static void place_stuck_door(int, int);
static void place_secret_door(int, int);
static void place_door(int, int);
static void place_up_stairs(int, int);
static void place_down_stairs(int, int);
static void place_stairs(int, int, int);
static void vault_trap(int, int, int, int, int);
static void vault_monster(int, int, int);
static void vault_jelly(int, int);
static void vault_orc(int, int, int);
static void vault_troll(int, int, int);
static void vault_undead(int, int);
static void vault_dragon(int, int, int, int);
static void vault_demon(int, int, int);
static void vault_giant(int, int, int);
static void build_room(int, int);
static void build_type1(int, int);
static void build_type5(int, int);
static void build_type2(int, int);
static void build_type3(int, int);
static void vault_nasty(int, int, int, int, int);
static void special_pit(int, int, int);
static void build_tunnel(int, int, int, int);
static int next_to(int, int);
static void try_door(int, int);
static void build_pit(int, int);
static void build_store(int, int, int);
static void place_boundary();
static void place_destroyed();
static void blank_cave();
static void cave_gen();
static void town_gen();

typedef struct coords {
    int x, y;
} coords;


static coords doorstk[100];
static int    doorindex;

extern int    rating;
extern int    peek;

/*
 * Always picks a correct direction		 
 */
static void correct_dir(int *rdir, int *cdir, int y1, int x1, int y2, int x2)
{
    if (y1 < y2)
	*rdir = 1;
    else if (y1 == y2)
	*rdir = 0;
    else
	*rdir = (-1);

    if (x1 < x2)
	*cdir = 1;
    else if (x1 == x2)
	*cdir = 0;
    else
	*cdir = (-1);
	
    if ((*rdir != 0) && (*cdir != 0)) {
	if (randint(2) == 1)
	    *rdir = 0;
	else
	    *cdir = 0;
    }
}


/*
 * Chance of wandering direction			 
 */
static void rand_dir(int *rdir, int *cdir)
{
    register int tmp;

    tmp = randint(4);
    if (tmp < 3) {
	*cdir = 0;
	*rdir = (-3) + (tmp << 1);   /* tmp=1 -> *rdir=-1; tmp=2 -> *rdir=1 */
    } else {
	*rdir = 0;
	*cdir = (-7) + (tmp << 1);   /* tmp=3 -> *cdir=-1; tmp=4 -> *cdir=1 */
    }
}


/*
 * Returns random co-ordinates for player/monster/object
 */
static void new_spot(s16b *y, s16b *x)
{
    register int        i, j;
    register cave_type *c_ptr;

    do {
	i = randint(cur_height - 2);
	j = randint(cur_width - 2);
	c_ptr = &cave[i][j];
    }
    while (c_ptr->fval >= MIN_CLOSED_SPACE || (c_ptr->m_idx != 0)
	   || (c_ptr->i_idx != 0) || (c_ptr->fval == NT_LIGHT_FLOOR)
	   || (c_ptr->fval == NT_DARK_FLOOR));
    *y = i;
    *x = j;
}



/*
 * Count walls S/N/E/W of given grid, which is "in_bounds()".
 *
 * note that y,x is always in_bounds(), i.e. 0 < y < cur_height-1, and 0 < x
 * < cur_width-1	 
 */
static int next_to_walls(int y, int x)
{
    register int        i = 0;
    register cave_type *c_ptr;

    c_ptr = &cave[y - 1][x];
    if (c_ptr->fval >= MIN_WALL) i++;
    c_ptr = &cave[y + 1][x];
    if (c_ptr->fval >= MIN_WALL) i++;
    c_ptr = &cave[y][x - 1];
    if (c_ptr->fval >= MIN_WALL) i++;
    c_ptr = &cave[y][x + 1];
    if (c_ptr->fval >= MIN_WALL) i++;

    return (i);
}

/*
 * Count corridors adjacent to given grid, which is in_bounds().
 *
 * note that y, x is always in_bounds(), hence no need to check that j, k are
 * in_bounds(), even if they are 0 or cur_x-1 is still works 
 */
static int next_to_corr(int y, int x)
{
    register int        k, j, i = 0;
    register cave_type *c_ptr;
    register inven_type *i_ptr;
    
    /* Scan the neighbors */    
    for (j = y - 1; j <= (y + 1); j++) {
	for (k = x - 1; k <= (x + 1); k++) {

	    /* Access the grid */
	    c_ptr = &cave[j][k];

	    /* Skip non-corridors */
	    if (c_ptr->fval != CORR_FLOOR) continue;

	    /* Access the item */
	    i_ptr = &i_list[c_ptr->i_idx];
	    
	    /* Skip doors */
	    if (c_ptr->i_idx == 0 || i_ptr->tval < TV_MIN_DOORS)
	    
	    /* Count these grids */
	    i++;
	}
    }

    /* Return the number of corridors */
    return (i);
}


/* 
 * Semi-Hack -- types of "places" we can allocate (below)
 */
#define ALLOC_SET_CORR	1
#define ALLOC_SET_ROOM	2
#define ALLOC_SET_BOTH	3


/*
 * Allocates an object for tunnels and rooms		-RAK-	 
 *
 * Type 1 is trap
 * Type 2 is unused (was visible traps)
 * Type 3 is rubble
 * Type 4 is gold
 * Type 5 is object
 */
static void alloc_object(int set, int typ, int num)
{
    register int y, x, k;

    /* Place some objects */
    for (k = 0; k < num; k++) {

	/* Don't put an object beneath the player, this could cause */
	/* problems if player is standing under rubble, or on a trap */

	/* Pick a "legal" spot */
	while (1) {

	    /* Location */
	    y = rand_int(cur_height);
	    x = rand_int(cur_width);

	    if ((set == ALLOC_SET_CORR) && (cave[y][x].fval == CORR_FLOOR || cave[y][x].fval == BLOCKED_FLOOR)) continue;

	    if ((set == ALLOC_SET_ROOM) && ((cave[y][x].fval == DARK_FLOOR) || (cave[y][x].fval == LIGHT_FLOOR) ||
	    (cave[y][x].fval == NT_DARK_FLOOR) || (cave[y][x].fval == NT_LIGHT_FLOOR))) continue;

	    if ((set == ALLOC_SET_BOTH) && (cave[y][x].fval <= MAX_CAVE_FLOOR)) continue;

	    if ((cave[y][x].i_idx != 0) || (y == char_row && x == char_col)) continue;

	    /* Accept it */
	    break;
	}

	if (typ < 4) {
	    if (typ == 1) {
		place_trap(y, x, randint(MAX_TRAP) - 1);
	    }
	    else /* (typ == 3) */ {
		place_rubble(y, x);
	    }
	}       
	else {
	    object_level = dun_level;
	    if (typ == 4) {
		place_gold(y, x);
	    }
	    else /* (typ == 5) */ {
		place_object(y, x);
	    }
	}
    }
}


/*
 * Blank a cave -- note new definition
 */
static void blank_cave(void)
{
    register int y;

    /* Clear each row */
    for (y = 0; y < MAX_HEIGHT; ++y) {

	/* Efficiency -- wipe a whole row at a time */
	C_WIPE(cave[y], MAX_WIDTH, cave_type);
    }
}


/*
 * Fills in empty spots with desired rock
 * Note: 9 is a temporary value.
 */
static void fill_cave(int fval)
{
    register int        i, j;
    register cave_type *c_ptr;

    /* Scan the cave (skip the outer walls) */
    for (i = cur_height - 2; i > 0; i--) {
	c_ptr = &cave[i][1];
	for (j = cur_width - 2; j > 0; j--) {
	    if ((c_ptr->fval == NULL_WALL) ||
		(c_ptr->fval == TMP1_WALL) ||
		(c_ptr->fval == TMP2_WALL)) {
		c_ptr->fval = fval;
	    }
	    c_ptr++;
	}
    }
}


/*
 * Places indestructible rock around edges of dungeon
 */
static void place_boundary(void)
{
    register int        i;
    register cave_type *top_ptr, *bottom_ptr;

    cave_type(*left_ptr)[MAX_WIDTH];
    cave_type(*right_ptr)[MAX_WIDTH];

/* put permanent wall on leftmost row and rightmost row */
    left_ptr = (cave_type(*)[MAX_WIDTH]) & cave[0][0];
    right_ptr = (cave_type(*)[MAX_WIDTH]) & cave[0][cur_width - 1];

    for (i = 0; i < cur_height; i++) {
	((cave_type *) left_ptr)->fval = BOUNDARY_WALL;
	left_ptr++;
	((cave_type *) right_ptr)->fval = BOUNDARY_WALL;
	right_ptr++;
    }

/* put permanent wall on top row and bottom row */
    top_ptr = &cave[0][0];
    bottom_ptr = &cave[cur_height - 1][0];

    for (i = 0; i < cur_width; i++) {
	top_ptr->fval = BOUNDARY_WALL;
	top_ptr++;
	bottom_ptr->fval = BOUNDARY_WALL;
	bottom_ptr++;
    }
}


/*
 * Places "streamers" of rock through dungeon		-RAK-	 
 */
static void place_streamer(int fval, int treas_chance)
{
    int		i, tx, ty;
    int		y, x, t1, t2, dir;
    cave_type	*c_ptr;

    /* Hack -- Choose starting point */
    y = (cur_height / 2) + 11 - randint(23);
    x = (cur_width / 2) + 16 - randint(33);

    /* Choose a direction (not including "5") */
    dir = randint(8);
    if (dir > 4) dir++;

    /* Place streamer into dungeon */
    t1 = 2 * DUN_STR_RNG + 1;	   /* Constants	 */
    t2 = DUN_STR_RNG + 1;
    do {
	for (i = 0; i < DUN_STR_DEN; i++) {
	
	    /* Pick a nearby grid */
	    ty = y + randint(t1) - t2;
	    tx = x + randint(t1) - t2;

	    /* Turn granite into streamers */
	    if (in_bounds(ty, tx)) {
		c_ptr = &cave[ty][tx];
		if (c_ptr->fval == GRANITE_WALL) {
		    c_ptr->fval = fval;
		    if (randint(treas_chance) == 1) {
			place_gold(ty, tx);
		    }
		}
	    }
	}
    }
    while (mmove(dir, &y, &x));
}


/*
 * copied verbatim from spells.c - damn
 * thing won't find it. :( :( -DGK
 */
static void repl_spot(int y, int x, int typ)
{
    register cave_type *c_ptr;

    c_ptr = &cave[y][x];

    switch (typ) {
      case 1:
      case 2:
      case 3:
	c_ptr->fval = CORR_FLOOR;
	break;
      case 4:
      case 7:
      case 10:
	c_ptr->fval = GRANITE_WALL;
	break;
      case 5:
      case 8:
      case 11:
	c_ptr->fval = MAGMA_WALL;
	break;
      case 6:
      case 9:
      case 12:
	c_ptr->fval = QUARTZ_WALL;
	break;
    }

    /* This is no longer part of a room */
    c_ptr->pl = FALSE;
    c_ptr->fm = FALSE;
    c_ptr->lr = FALSE;

    /* Delete any object at that location */
    if (c_ptr->i_idx != 0) delete_object(y, x);

    /* Delete any monster at that location */
    if (c_ptr->m_idx > 1) delete_monster((int)c_ptr->m_idx);
}


/*
 * Build a destroyed level
 */
static void place_destroyed()
{
    register int y1, x1;
    register int y, x, k;
    int          n;

    /* Drop a few epi-centers (usually about two) */
    for (n = 1; n <= randint(5); n++) {

	/* Pick an epi-center */
	x1 = randint(cur_width - 32) + 15;
	y1 = randint(cur_height - 32) + 15;

	/* Earthquake! */
	for (y = (y1 - 15); y <= (y1 + 15); y++) {
	    for (x = (x1 - 15); x <= (x1 + 15); x++) {

		/* Do not destroy important (or illegal) stuff */
		if (in_bounds(y, x) && (cave[y][x].fval != BOUNDARY_WALL) &&
		    ((cave[y][x].i_idx == 0) ||	/* DGK */
		     ((i_list[cave[y][x].i_idx].tval != TV_UP_STAIR) &&
		      (i_list[cave[y][x].i_idx].tval != TV_DOWN_STAIR) &&
		      (!(i_list[cave[y][x].i_idx].flags2 & TR_ARTIFACT))))) {
		    k = distance(y, x, y1, x1);
		    if (y == char_row && x == char_col) repl_spot(y, x, 1);
		    else if (k < 13) repl_spot(y, x, (int)randint(6));
		    else if (k < 16) repl_spot(y, x, (int)randint(9));
		}
	    }
	}
    }
}


/*
 * Chris Tate (fixer@faxcsl.dcrt.nih.gov) - optimized this code for size!
 * This code performs the common test in all of the place_* functions,
 * and returns c_ptr if we can go ahead and place the object, or NULL
 * if we can't.
 */
static cave_type *test_place_obj(int y, int x)
{
    cave_type *t;
    int        tv;
    
    if (!in_bounds(y,x)) return NULL;
    t = &cave[y][x];
    tv = i_list[t->i_idx].tval;
    
    if (t->i_idx != 0)
	if (((tv <= TV_MAX_WEAR) && (tv >= TV_MIN_WEAR) &&
	     (i_list[t->i_idx].flags2 & TR_ARTIFACT)) ||
	    (tv == TV_UP_STAIR) || (tv == TV_DOWN_STAIR) ||
	    (tv == TV_STORE_DOOR))
	    return NULL;
	else
    /* Destroy any object already there */
    delete_object(y, x);

    /* Return the cave pointer */
    return (&cave[y][x]);
}


static void place_open_door(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_OPEN_DOOR);
    c_ptr->fval = CORR_FLOOR;
}


static void place_broken_door(int y, int x)
{
    int		cur_pos;
    cave_type	*c_ptr;
    inven_type	*i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_OPEN_DOOR);
    c_ptr->fval = CORR_FLOOR;
    i_ptr->p1 = 1;
}


static void place_closed_door(int y, int x)
{
    int		cur_pos;
    cave_type	*c_ptr;
    inven_type	*i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_CLOSED_DOOR);
    c_ptr->fval = BLOCKED_FLOOR;
}


static void place_locked_door(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_CLOSED_DOOR);
    c_ptr->fval = BLOCKED_FLOOR;

    /* Lock the door */
    i_ptr->p1 = randint(10) + 10;
}


static void place_stuck_door(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_CLOSED_DOOR);
    c_ptr->fval = BLOCKED_FLOOR;

    /* Stick the door */
    i_ptr->p1 = (-randint(10) - 10);
}


static void place_secret_door(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_SECRET_DOOR);

    /* Put the object in the cave */
    c_ptr->i_idx = cur_pos;

    c_ptr->fval = BLOCKED_FLOOR;
}


static void place_door(int y, int x)
{
    register int        tmp;

    tmp = randint(8);
    if (tmp < 4) {
	if (randint(4) == 1)
	    place_broken_door(y, x);
	else
	    place_open_door(y, x);
    }
    else if (tmp < 7) {
	tmp = randint(100);
	if (tmp > 25)
	    place_closed_door(y, x);
	else if (tmp == 3)
	    place_stuck_door(y, x);
	else
	    place_locked_door(y, x);
    }
    else {
	place_secret_door(y, x);
    }
}


/*
 * Place an up staircase at given y, x			-RAK-	 
 */
static void place_up_stairs(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_UP_STAIR);

}


/*
 * Place a down staircase at given y, x			-RAK-	 
 */
static void place_down_stairs(int y, int x)
{
    register int        cur_pos;
    register cave_type *c_ptr;
    inven_type *i_ptr;

    /* Hack -- no stairs on quest levels */
    if (is_quest(dun_level)) {
	place_up_stairs(y, x);
	return;
    }

    c_ptr = test_place_obj(y,x);
    if (!c_ptr) return;

    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    i_ptr = &i_list[cur_pos];
    invcopy(i_ptr, OBJ_DOWN_STAIR);
    if (dun_level == 0)			/* on town level -CWS */
	c_ptr->pl = TRUE;
}


/*
 * Places a staircase 1=up, 2=down			-RAK-	 
 *
 * Note that all the function in this file seem to like "do...while".
 */
static void place_stairs(int typ, int num, int walls)
{
    register cave_type *cave_ptr;
    int                 i, j, flag;
    register int        y1, x1, y2, x2;

    /* Place "num" stairs */
    for (i = 0; i < num; i++) {

	/* Place some stairs */
	for (flag = FALSE; !flag; ) {

	    j = 0;
	    do {
	    /* Note: don't let y1/x1 be zero, and don't let y2/x2 be equal to
	     * cur_height-1/cur_width-1, these values are always
	     * BOUNDARY_ROCK. 
	     */

		/* Pick a random grid */
		y1 = randint(cur_height - 14);
		x1 = randint(cur_width - 14);
		y2 = y1 + 12;
		x2 = x1 + 12;

		do {
		    do {
			cave_ptr = &cave[y1][x1];
			if (floor_grid_bold(y1, x1)
			    && (cave_ptr->i_idx == 0)
			    && (next_to_walls(y1, x1) >= walls)) {
			    flag = TRUE;

		/* Put the stairs here */
		if (typ == 1) {
		    place_up_stairs(y1, x1);
		}
		else {
		    place_down_stairs(y1, x1);
		}

			}
			x1++;
		    }
		    while ((x1 != x2) && (!flag));
		    x1 = x2 - 12;
		    y1++;
		}
		while ((y1 != y2) && (!flag));
		j++;
	    }
	    while ((!flag) && (j <= 30));

	    /* Require fewer walls */
	    walls--;
	}
    }
}


/*
 * Place a trap with a given displacement of point
 */
static void vault_trap(int y, int x, int yd, int xd, int num)
{
    int		count, y1, x1;
    int                 i, flag;
    register cave_type *c_ptr;

    for (i = 0; i < num; i++) {
	flag = FALSE;
	count = 0;

    /* Place traps */
    do {

	    do {		   /* add another bounds check -CFT */

	/* Get a location */
	y1 = rand_spread(y, yd);
	x1 = rand_spread(x, xd);

	    } while (!in_bounds(y1, x1));

	    c_ptr = &cave[y1][x1];
	    if ((c_ptr->fval != NULL_WALL) && (c_ptr->fval <= MAX_CAVE_FLOOR)
		&& (c_ptr->i_idx == 0)) {

	/* Place the trap */
		place_trap(y1, x1, randint(MAX_TRAP) - 1);

	/* Done */
		flag = TRUE;
	    }
	    count++;
	}
	while ((!flag) && (count <= 5));
    }
}



/*
 * Place some monsters at the given location
 */
static void vault_monster(int y1, int x1, int num)
{
    int          i, y, x;

    /* Try to summon "num" monsters "near" the given location */
    for (i = 0; i < num; i++) {
	y = y1;
	x = x1;
	(void)summon_monster(&y1, &x1, TRUE);
    }
}

/*
 * Place a jelly at the given location
 */
static void vault_jelly(int y, int x)
{
    int l = m_level[MAX_R_LEV];

    /* Hack -- allocate a simple sleeping jelly */
    while (1) {
	int m = randint(l) - 1;
	if (((c_list[m].cchar == 'j') || (c_list[m].cchar == ',')
	     || (c_list[m].cchar == 'i') || (c_list[m].cchar == 'm'))
	    && !(c_list[m].cdefense & EVIL)) {
	place_monster(y, x, m, TRUE);
	break;
	}
    }
}

/*
 * Place an undead creature at the given location
 */
static void vault_undead(int y, int x)
{
    int l, m, summon;
    
    summon = FALSE;
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	if ((c_list[m].cdefense & UNDEAD) && !(c_list[m].cdefense & UNIQUE)) {
	    summon = TRUE;
	place_monster(y, x, m, TRUE);
	}
    } while (!summon);
}


static void vault_orc(int y, int x, int rank)
{
    register int i;

    i = 0;
    switch (rank) {
      case 1:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Snaga"))
		break;
	    i++;
	}
	break;
      case 2:
      case 3:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Black orc"))
		break;
	    i++;
	}
	break;
      case 4:
      case 5:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Uruk-Hai"))
		break;
	    i++;
	}
	break;
      case 6:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Orc captain"))
		break;
	    i++;
	}
	break;
    }
    if (i != MAX_R_IDX)
	place_monster(y, x, i, FALSE);
}

static void vault_troll(int y, int x, int rank)
{
    register int i;

    i = 0;
    switch (rank) {
      case 1:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Forest troll"))
		break;
	    i++;
	}
	break;
      case 2:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Stone troll"))
		break;
	    i++;
	}
	break;
      case 3:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Ice troll"))
		break;
	    i++;
	}
	break;
      case 4:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Cave troll"))
		break;
	    i++;
	}
	break;
      case 5:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Water troll"))
		break;
	    i++;
	}
	break;
      case 6:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Olog-Hai"))
		break;
	    i++;
	}
	break;
    }
    if (i != MAX_R_IDX)
	place_monster(y, x, i, FALSE);
}


static void vault_dragon(int y, int x, int rank, int type)
{
    register int i = 0;

    switch (rank) {
      case 1:
      case 2:
      case 3:
	switch (type) {
	  case 1:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young blue dragon"))
		    break;
		i++;
	    }
	    break;
	  case 2:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young white dragon"))
		    break;
		i++;
	    }
	    break;
	  case 3:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young green dragon"))
		    break;
		i++;
	    }
	    break;
	  case 4:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young black dragon"))
		    break;
		i++;
	    }
	    break;
	  case 5:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young red dragon"))
		    break;
		i++;
	    }
	    break;
	  case 6:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Young Multi-Hued Dragon"))
		    break;
		i++;
	    }
	    break;
	}
	break;
      case 4:
      case 5:
	switch (type) {
	  case 1:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature blue Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 2:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature white Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 3:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature green Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 4:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature black Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 5:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature red Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 6:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Mature Multi-Hued Dragon"))
		    break;
		i++;
	    }
	    break;
	}
	break;
      case 6:
	switch (type) {
	  case 1:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient blue Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 2:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient white Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 3:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient green Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 4:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient black Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 5:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient red Dragon"))
		    break;
		i++;
	    }
	    break;
	  case 6:
	    while (i < MAX_R_IDX) {
		if (!stricmp(c_list[i].name, "Ancient Multi-Hued Dragon"))
		    break;
		i++;
	    }
	    break;
	}
	break;
    }
    if (i != MAX_R_IDX)
	place_monster(y, x, i, FALSE);
}

static void vault_demon(int y, int x, int rank)
{
    register int i = 0;

    switch (rank) {
      case 1:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Vrock"))
		break;
	    i++;
	}
	break;
      case 2:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Hezrou"))
		break;
	    i++;
	}
	break;
      case 3:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Glabrezu"))
		break;
	    i++;
	}
	break;
      case 4:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Nalfeshnee"))
		break;
	    i++;
	}
	break;
      case 5:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Marilith"))
		break;
	    i++;
	}
	break;
      case 6:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Lesser balrog"))
		break;
	    i++;
	}
	break;
    }
    if (i != MAX_R_IDX)
	place_monster(y, x, i, FALSE);
}


static void vault_giant(int y, int x, int rank)
{
    register int i = 0;

    switch (rank) {
      case 1:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Hill giant"))
		break;
	    i++;
	}
	break;
      case 2:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Frost giant"))
		break;
	    i++;
	}
	break;
      case 3:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Fire giant"))
		break;
	    i++;
	}
	break;
      case 4:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Stone giant"))
		break;
	    i++;
	}
	break;
      case 5:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Cloud giant"))
		break;
	    i++;
	}
	break;
      case 6:
	while (i < MAX_R_IDX) {
	    if (!stricmp(c_list[i].name, "Storm giant"))
		break;
	    i++;
	}
	break;
    }
    if (i != MAX_R_IDX)
	place_monster(y, x, i, FALSE);
}


/*
 * Builds a room at a row, column coordinate
 */
static void build_room(int yval, int xval)
{
    int        i, j, y_depth, x_right;
    int                 y_height, x_left;
    byte		floor;
    cave_type *c_ptr, *d_ptr;

    if (dun_level <= randint(25))
	floor = LIGHT_FLOOR;	   /* Floor with light	 */
    else
	floor = DARK_FLOOR;	   /* Dark floor		 */

    y_height = yval - randint(4);
    y_depth = yval + randint(3);
    x_left = xval - randint(11);
    x_right = xval + randint(11);

/* for paranoia's sake: bounds-check!  Memory errors caused by accessing
 * cave[][] out-of-bounds are nearly impossible to spot!  -CFT 
 */
    if (y_height < 1)
	y_height = 1;
    if (y_depth >= (cur_height - 1))
	y_depth = cur_height - 2;
    if (x_left < 1)
	x_left = 1;
    if (x_right >= (cur_width - 1))
	x_right = cur_width - 2;

/* the x dim of rooms tends to be much larger than the y dim, so don't bother
 * rewriting the y loop 
 */
    for (i = y_height; i <= y_depth; i++) {
	c_ptr = &cave[i][x_left];
	for (j = x_left; j <= x_right; j++) {
	    c_ptr->fval = floor;
	    c_ptr->lr = TRUE;
	    c_ptr++;
	}
    }

    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	c_ptr = &cave[i][x_left - 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr = &cave[i][x_right + 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
    }

    c_ptr = &cave[y_height - 1][x_left];
    d_ptr = &cave[y_depth + 1][x_left];
    for (i = x_left; i <= x_right; i++) {
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr++;
	d_ptr->fval = GRANITE_WALL;
	d_ptr->lr = TRUE;
	d_ptr++;
    }

/* Every so often fill a normal room with pillars - Decado */

    if (randint(20) == 2) {
	for (i = y_height; i <= y_depth; i += 2) {
	    for (j = x_left; j <= x_right; j += 2) {
		c_ptr = &cave[i][j];
		c_ptr->fval = TMP1_WALL;
		c_ptr->lr = TRUE;
	    }
	}
    }
}

/*
 * Builds a room at a row, column coordinate
 *
 * Type 1 unusual rooms are several overlapping rectangular ones	
 */
static void build_type1(int yval, int xval)
{
    int                 y_height, y_depth;
    int                 x_left, x_right, limit;
    register int        i0, i, j;
    byte               floor;
    register cave_type *c_ptr, *d_ptr;

    if (dun_level <= randint(25))
	floor = LIGHT_FLOOR;	   /* Floor with light	 */
    else
	floor = DARK_FLOOR;	   /* Dark floor		 */
    limit = 1 + randint(2);
    for (i0 = 0; i0 < limit; i0++) {
	y_height = yval - randint(4);
	y_depth = yval + randint(3);
	x_left = xval - randint(11);
	x_right = xval + randint(11);

    /* for paranoia's sake: bounds-check!  Memory errors caused by accessing
     * cave[][] out-of-bounds are nearly impossible to spot!  -CFT 
     */
	if (y_height < 1)
	    y_height = 1;
	if (y_depth >= (cur_height - 1))
	    y_depth = cur_height - 2;
	if (x_left < 1)
	    x_left = 1;
	if (x_right >= (cur_width - 1))
	    x_right = cur_width - 2;

    /* the x dim of rooms tends to be much larger than the y dim, so don't
     * bother rewriting the y loop 
     */

	for (i = y_height; i <= y_depth; i++) {
	    c_ptr = &cave[i][x_left];
	    for (j = x_left; j <= x_right; j++) {
		c_ptr->fval = floor;
		c_ptr->lr = TRUE;
		c_ptr++;
	    }
	}
	for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	    c_ptr = &cave[i][x_left - 1];
	    if (c_ptr->fval != floor) {
		c_ptr->fval = GRANITE_WALL;
		c_ptr->lr = TRUE;
	    }
	    c_ptr = &cave[i][x_right + 1];
	    if (c_ptr->fval != floor) {
		c_ptr->fval = GRANITE_WALL;
		c_ptr->lr = TRUE;
	    }
	}
	c_ptr = &cave[y_height - 1][x_left];
	d_ptr = &cave[y_depth + 1][x_left];
	for (i = x_left; i <= x_right; i++) {
	    if (c_ptr->fval != floor) {
		c_ptr->fval = GRANITE_WALL;
		c_ptr->lr = TRUE;
	    }
	    c_ptr++;
	    if (d_ptr->fval != floor) {
		d_ptr->fval = GRANITE_WALL;
		d_ptr->lr = TRUE;
	    }
	    d_ptr++;
	}
    }
}


/*
 * Build various complicated vaults - Decado
 */
static void build_type5(int yval, int xval)
{
    register int        x, y, x1, y1, vault;
    int                 width = 0, height = 0;
    char               *template = NULL;
/* use t to avoid changing template in loop.  This should fix some memory
 * troubles, because now we aren't free()ing with a bad pointer.  Thanks
 * much to gehring@pib1.physik.uni-bonn.edu for pointing this bug out... -CFT
 */
    char               *t;
    char                buf[50];
    byte		floor, wall;
    cave_type		*c_ptr;

    if (dun_level <= randint(25))
	floor = LIGHT_FLOOR;	   /* Floor with light */
    else
	floor = DARK_FLOOR;	   /* Dark floor       */


    /* Assume Not a vault */
    vault = 0;

    /* Pick a large room */
    switch (randint(8)) {
    
      case 1:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"       %%%%%%       ",
		"    %%%..##..%%%    ",
		"  %%....####....%%  ",
		" %......#**#......% ",
		"%...,.##+##+##.,...%",
		"%.,.,.#*#*&#*#.,.,.%",
		"%.,.,.#*#&*#*#.,.,.%",
		"%...,.##+##+##.,...%",
		" %......#**#......% ",
		"  %%....####....%%  ",
		"    %%%..##..%%%    ",
		"       %%%%%%       ");
	break;
	
      case 2:
	width = 20;
	height = 14;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		"   %%%%%%%%%%%%%%   ",
		"  %%.##########.%%  ",
		" %%..#..,,,,..#..%% ",
		"%%,..#.,####,.#..,%%",
		"%....#.,#**#,.#....%",
		"%.###+,##&&##,+###.%",
		"%.#..,,#*&**#,,..#.%",
		"%.#..,,#**&*#,,..#.%",
		"%.###+,##&&##,+###.%",
		"%....#.,#**#,.#....%",
		"%%,..#.,####,.#..,%%",
		" %%..#..,,,,..#..%% ",
		"  %%.##########.%%  ",
		"   %%%%%%%%%%%%%%   ");
	break;
	
      case 3:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"    %%%%%%%%%%%%    ",
		" %%%%..........%%%% ",
		" %...###+##+###...% ",
		"%%...#,,#,,#,,#...%%",
		"%.###+##+##+##+###.%",
		"%.#,,#&&#**#&&#,,#.%",
		"%.#,,#&&#**#&&#,,#.%",
		"%.###+##+##+##+###.%",
		"%%...#,,#,,#,,#...%%",
		" %...###+##+###...% ",
		" %%%%..........%%%% ",
		"    %%%%%%%%%%%%    ");
	break;
	
      case 4:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"%%%%%%%%%%%%%%%%%%%%",
		"%*.......&........*%",
		"%.################.%",
		"%.#,.,.,.,.,.,.,.#.%",
		"%.#.############,#.%",
		"%.#,+,&&+**#&*,#.#&%",
		"%&#.#,*&#**+&&,+,#.%",
		"%.#,############.#.%",
		"%.#.,.,.,.,.,.,.,#.%",
		"%.################.%",
		"%*........&.......*%",
		"%%%%%%%%%%%%%%%%%%%%");
	break;
	
      case 5:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"%%%%%%%%%%%%%%%%%   ",
		"%,,,##,,,,##....%%  ",
		"%,,,,##,,,,##....%% ",
		"%#,,,,##,,,,##....%%",
		"%##,,,,##,,,,##....%",
		"%.##,,,,,,,,,,#+...%",
		"%..#+,,,,,,,,,,##..%",
		"%...##,,,,##,,,,##.%",
		"%%...##,,,,##,,,,##%",
		" %%...##,,,,##,,,,#%",
		"  %%...##,,,,##,,,,%",
		"   %%%%%%%%%%%%%%%%%");
	break;
	
      case 6:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"   %%%%%%%%%%%%%%%%%",
		"  %%....##,,,,##,,,%",
		" %%....##,,,,##,,,,%",
		"%%....##,,,,##,,,,#%",
		"%....##,,,,##,,,,##%",
		"%...+#,,,,,,,,,,##.%",
		"%..##,,,,,,,,,,+#..%",
		"%.##,,,,##,,,,##...%",
		"%##,,,,##,,,,##...%%",
		"%#,,,,##,,,,##...%% ",
		"%,,,,##,,,,##...%%  ",
		"%%%%%%%%%%%%%%%%%   ");
	break;
	
      case 7:
	width = 20;
	height = 12;
	template = (char *)malloc((size_t) width * height + 1);
	rating += 5;
	sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s",
		"%%%%%%%%%%%%%%%%%%%%",
		"%,################,%",
		"%^#.*...&..,....,#^%",
		"%^#...,......&...#^%",
		"%^#######++#######^%",
		"%^+.,..&+,*+*....+^%",
		"%^+..*.,+.&+.,.&.+^%",
		"%^#######++#######^%",
		"%^#....,.,.....,.#^%",
		"%^#..&......*....#^%",
		"%,################,%",
		"%%%%%%%%%%%%%%%%%%%%");
	break;
	
      case 8:

	vault = TRUE;

	switch (randint(4)) {

	  case 4:
	    width = 40;
	    height = 18;
	    template = (char *)malloc((size_t) width * height + 1);
	    rating += 25;
	    sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
		    "%######################################%",
		    "%#*8..&##***++^^^^^^^^^^^^++***##&..*8#%",
		    "%#8..&##,,,,,##^^^^^^^^^^##,,,,,X#&..*#%",
		    "%#..&X#.....,.##^^^^^^^^##..&....##&..#%",
		    "%#.&##..,.&....##^^^^^^##..,...&..##&.#%",
		    "%#&##..*...&.^..##^^^^##..*....,..,##&#%",
		    "%####+#############++#############+####%",
		    "%+....,.,.#&&&&***+88+***&&&&#,.,.,...+%",
		    "%+...,.,.,#&&&&***+88+***&&&&#.,.,....+%",
		    "%####+#############++#############+####%",
		    "%#&##..*....&...##^^^^##...*...&,..X#&#%",
		    "%#.&##..&.^....##^^^^^^##....&....##&.#%",
		    "%#..&##....&..##^^^^^^^^##..,..*.##&..#%",
		    "%#*..&X#,,,,,##^^^^^^^^^^##,,,,,##&..8#%",
		    "%#8*..&##***++^^^^^^^^^^^^++***##&..*8#%",
		    "%######################################%",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
	    break;

	  case 3:
	    width = 39;
	    height = 17;
	    template = (char *)malloc((size_t) width * height + 1);
	    rating += 35;
	    sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
		    "%#####################################%",
		    "%+&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XO#%",
		    "%###################################X#%",
		    "%#OX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&#%",
		    "%#X###################################%",
		    "%#&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XO#%",
		    "%###################################X#%",
		    "%#OX&XOX&XOX&XOOOOOOOOOOOXOX&XOX&XOX&#%",
		    "%#X###################################%",
		    "%#&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XO#%",
		    "%###################################X#%",
		    "%#OX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&#%",
		    "%#X###################################%",
		    "%#&XOX&XOX&XOX&XOX&XOX&XOX&XOX&XOX&X&+%",
		    "%#####################################%",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
	    break;

	  case 2:
	    width = 40;
	    height = 18;
	    template = (char *)malloc((size_t) width * height + 1);
	    rating += 30;
	    sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
		    "%######################################%",
		    "%#,X,X,X,X,X,X,X,X*@@*X,X,X,X,X,X,X,X,#%",
		    "%#+################XX################+#%",
		    "%#.,..,.#&.&.,*##******##*,.&.&#.,...,X%",
		    "%#..,.^^#....,##***@@***##,....#^^..,.X%",
		    "%######+#^&.&##***@XX@***##&.&^#+######%",
		    "%#,.&.^^#+####***@X##X@***####+#^^.,..#%",
		    "%#..,&,.#^^^@#**@X#OO#X@**X@^^^#.,..&,#%",
		    "%#.,....#^^^@X**@X#OO#X@**#@^^^#.&.,..#%",
		    "%#...,^^#+####***@X##X@***####+#^^..,.#%",
		    "%######+#^&.&##***@XX@***##&.&^#+######%",
		    "%X.,..^^#.....##***@@***##,....#^^.,..#%",
		    "%X...,..#&.&.,*##******##*,.&.&#..,..,#%",
		    "%#+################XX################+#%",
		    "%#,X,X,X,X,X,X,X,X*@@*X,X,X,X,X,X,X,X,#%",
		    "%######################################%",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
	    break;

	  case 1:
	    width = 40;
	    height = 15;
	    template = (char *)malloc((size_t)width * height + 1);
	    rating += 25;
	    sprintf(template, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
		    "%&+.^..^..^..^..^..^..^..^..^..^..^..+&%",
		    "%+####################################+%",
		    "%.#.&.^,#&^&^#****+^*^@^#.*.&..#..*.,#.%",
		    "%^#.,.&^+^&^@#^^^^#@^*^*#....*^+.^...#^%",
		    "%.#*..,.###+####+####+###.&.^..#..&,.#.%",
		    "%^#..^.*#*..^&&@@*#,,,,,####+###,....#^%",
		    "%.##+##############,*O*,#,,,,,,###+###.%",
		    "%^#*&#.&,*.#,*&^*^#,,,,,#,,,,,,#....,#^%",
		    "%.#&,+....*+,*&^*^##########+###.,...+.%",
		    "%^#.,#.*.&.#,*&^*^+.,.&.^*.&^&^#.....#^%",
		    "%.#^*#.,..,#,*&^*^#*.^*.,..&&&^#,..,.#.%",
		    "%+####################################+%",
		    "%&+..^..^..^..^..^..^..^..^..^..^..^.+&%",
		    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
	    break;
	}

	break;
    }

/* check these rooms will fit on the map! */

    wall = TMP1_WALL;
    if (vault) {
	xval += 4;		   /* neat kludge deccy... */
	yval += 4;
	wall = BOUNDARY_WALL;
	if (floor == LIGHT_FLOOR)
	    floor = NT_LIGHT_FLOOR;
	else
	    floor = NT_DARK_FLOOR;
    }

    /* Tight fit?  Use a simpler room */
    if (!(in_bounds(yval - (height / 2), xval - (width / 2)) &&
	  in_bounds(yval + (height / 2), xval + (width / 2)))) {
	free(template);
	build_type2(yval, xval);
	return;
    }


    /* DO NOT CHANGE yval or xval AFTER THIS LINE */


    /* (Sometimes) Cause a special feeling */
    if ((dun_level <= 40) ||
	(randint((dun_level - 30)*(dun_level - 30) + 1) < 400)) {
	good_item_flag = TRUE;
    }


    /* Scan the vault, grid by grid, symbol by symbol */
    for (t = template, y = 0; y < height; y++) {
	for (x = 0; x < width; x++, t++) {

	    /* Do something */
	    x1 = xval - (width / 2) + x;
	    y1 = yval - (height / 2) + y;
	    c_ptr = &cave[y1][x1];

	    /* Analyze the grid */
	    switch (*t) {

		/* Solid wall */	    
	      case '#':
		c_ptr->fval = wall;
		c_ptr->lr = TRUE;
		break;
		
		/* Temp wall */
	      case 'X':
		c_ptr->fval = TMP1_WALL;
		c_ptr->lr = TRUE;
		break;
		
		/* Granite wall */
	      case '%':
		c_ptr->fval = GRANITE_WALL; /* Not temp, since this may have doors in */
		c_ptr->lr = TRUE;
		break;
		
	      case '.':	   /* lit up/ not lit up floor */
		c_ptr->fval = floor;
		c_ptr->lr = TRUE;
		break;

	      case '*':	   /* treasure/trap */
		c_ptr->fval = floor;
		if (randint(20) > 7) {
		    object_level = dun_level;
		    place_object(y1, x1);
		}
		else if (randint(10) > 2) {
		    place_trap(y1, x1, randint(MAX_TRAP) - 1);
		}
		else if (randint(2) == 1 && !vault) {
		    place_down_stairs(y1, x1);
		}
		else if (!vault) {
		    place_up_stairs(y1, x1);
		}
		c_ptr->lr = TRUE;
		break;

	      case '+':	   /* secret doors */
		place_secret_door(y1, x1);
		c_ptr->lr = TRUE;
		break;

	      /* Trap */
	      case '^':
		c_ptr->fval = floor;
		place_trap(y1, x1, randint(MAX_TRAP) - 1);
		c_ptr->lr = TRUE;
		break;

	      case ' ':
		break;

	      /* Monsters (see below) */
	      case '&':
	      case '@':
	      case '8':
	      case 'O':
	      case ',':
		/* do nothing for now... cannot place monster until whole
		 * area is built, OW group monsters screw things up by being
		 * placed where walls will be placed on top of them  -CFT
		 */
		break;

	      default:
#if 0
		quit_fmt("Illegal vault creation char '%c'", *t);
#endif
		break;
	    }
	}
    }


    /* Scan the vault, grid by grid, symbol by symbol -- place monsters */
    for (t = template, y = 0; y < height; y++) {
	for (x = 0; x < width; x++, t++) {

	    /* Extract the grid */
	    x1=xval-(width/2)+x;
	    y1=yval-(height/2)+y;
	    c_ptr = &cave[y1][x1];

	    /* Analyze the symbol */
	    switch (*t) {

	      case '#': case 'X': case '%': case '*':
	      case '+': case '^': case '.':
		/* we already placed all of these... -CFT */
		break;

		/* Monster */
	      case '&':
		c_ptr->fval = floor;
		place_monster(y1, x1,
			      get_mons_num(dun_level + MON_SUMMON_ADJ + 2 + vault),
			      TRUE);
		c_ptr->lr = TRUE;
		break;
		
		/* Meaner monster */
	      case '@':
		c_ptr->fval = floor;
		place_monster(y1, x1,
			      get_nmons_num(dun_level + MON_SUMMON_ADJ + 7),
			      TRUE);
		c_ptr->lr = TRUE;
		break;
		
		/* Meaner monster, plus treasure */
	      case '8':
		c_ptr->fval = floor;
		place_monster(y1, x1,
			      get_nmons_num(dun_level + MON_SUMMON_ADJ + 7),
			      TRUE);
		place_good(y1, x1, 1);
		c_ptr->lr = TRUE;
		break;

		/* Nasty monster and treasure */
	      case 'O':
		c_ptr->fval = floor;
		place_monster(y1, x1,
			      get_nmons_num(dun_level + MON_SUMMON_ADJ + 40),
			      TRUE);
		object_level = dun_level + MON_SUMMON_ADJ + 20;
		place_good(y1, x1, SPECIAL);
		object_level = dun_level + 7;
		c_ptr->lr = TRUE;
		break;
		
		/* Monster and/or object */
	      case ',':
		c_ptr->fval = floor;
		if (randint(2) == 1) {
		    place_monster(y1, x1,
				  get_mons_num(dun_level + MON_SUMMON_ADJ + vault),
				  TRUE);
		}
		if (randint(2) == 1) {
		    object_level = dun_level + 7;
		    place_object(y1, x1);
		}
		c_ptr->lr = TRUE;
		break;

	      case ' ':
		break;
	      default:
#if 0
		sprintf(buf, "Cockup='%c'", *t);
		msg_print(buf);
#endif
		break;
	    }
	}
    }
    free(template);
}

/* Builds an unusual room at a row, column coordinate	-RAK-
 *
 * Type 2 unusual rooms all have an inner room:
 * 1 - Just an inner room with one door
 * 2 - An inner room within an inner room
 * 3 - An inner room with pillar(s)
 * 4 - Inner room has a maze
 * 5 - A set of four inner rooms
 */

static void build_type2(int yval, int xval)
{
    register int        i, j, y_height, x_left;
    int                 y_depth, x_right, tmp;
    byte               floor;
    register cave_type *c_ptr, *d_ptr;

    if (dun_level <= randint(25))
	floor = LIGHT_FLOOR;	   /* Floor with light	 */
    else
	floor = DARK_FLOOR;	   /* Dark floor		 */
    y_height = yval - 4;
    y_depth = yval + 4;
    x_left = xval - 11;
    x_right = xval + 11;

/* paranoia bounds-check...   if the room can't fit in the cave, punt out and
 * build a simpler room type (type1 is already bounds-checked) This should
 * help solve MORE memory troubles! -CFT 
 */
    if (!in_bounds(y_height, x_left) || !in_bounds(y_depth, x_right)) {
	build_type1(yval, xval);
	return;
    }

/* the x dim of rooms tends to be much larger than the y dim, so don't bother
 * rewriting the y loop 
 */

    for (i = y_height; i <= y_depth; i++) {
	c_ptr = &cave[i][x_left];
	for (j = x_left; j <= x_right; j++) {
	    c_ptr->fval = floor;
	    c_ptr->lr = TRUE;
	    c_ptr++;
	}
    }
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	c_ptr = &cave[i][x_left - 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr = &cave[i][x_right + 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
    }
    c_ptr = &cave[y_height - 1][x_left];
    d_ptr = &cave[y_depth + 1][x_left];
    for (i = x_left; i <= x_right; i++) {
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr++;
	d_ptr->fval = GRANITE_WALL;
	d_ptr->lr = TRUE;
	d_ptr++;
    }
/* The inner room		 */
    y_height = y_height + 2;
    y_depth = y_depth - 2;
    x_left = x_left + 2;
    x_right = x_right - 2;
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	cave[i][x_left - 1].fval = TMP1_WALL;
	cave[i][x_right + 1].fval = TMP1_WALL;
    }
    c_ptr = &cave[y_height - 1][x_left];
    d_ptr = &cave[y_depth + 1][x_left];
    for (i = x_left; i <= x_right; i++) {
	c_ptr->fval = TMP1_WALL;
	c_ptr++;
	d_ptr->fval = TMP1_WALL;
	d_ptr++;
    }
/* Inner room variations		 */
    switch (randint(5)) {
      case 1:			   /* Just an inner room.	 */
	tmp = randint(4);
	if (tmp < 3) {		   /* Place a door	 */
	    if (tmp == 1)
		place_secret_door(y_height - 1, xval);
	    else
		place_secret_door(y_depth + 1, xval);
	} else {
	    if (tmp == 3)
		place_secret_door(yval, x_left - 1);
	    else
		place_secret_door(yval, x_right + 1);
	}
	vault_monster(yval, xval, 1);
	break;

      case 2:			   /* Treasure Vault	 */
	tmp = randint(4);
	if (tmp < 3) {		   /* Place a door	 */
	    if (tmp == 1)
		place_secret_door(y_height - 1, xval);
	    else
		place_secret_door(y_depth + 1, xval);
	} else {
	    if (tmp == 3)
		place_secret_door(yval, x_left - 1);
	    else
		place_secret_door(yval, x_right + 1);
	}

	for (i = yval - 1; i <= yval + 1; i++) {
	    cave[i][xval - 1].fval = TMP1_WALL;
	    cave[i][xval + 1].fval = TMP1_WALL;
	}
	cave[yval - 1][xval].fval = TMP1_WALL;
	cave[yval + 1][xval].fval = TMP1_WALL;

	tmp = randint(4);	   /* Place a door	 */
	if (tmp < 3)
	    place_locked_door(yval - 3 + (tmp << 1), xval); /* 1 -> yval-1; 2 -> yval+1 */
	else
	    place_locked_door(yval, xval - 7 + (tmp << 1));

    /* Place an object in the treasure vault	 */
	tmp = randint(10);
	if (tmp > 2) {
	    object_level = dun_level;
	    place_object(yval, xval);
	}
	else if (tmp == 2)
	    place_down_stairs(yval, xval);
	else
	    place_up_stairs(yval, xval);

    /* Guard the treasure well		 */
	vault_monster(yval, xval, 2 + randint(3));
    /* If the monsters don't get 'em.	 */
	vault_trap(yval, xval, 4, 10, 2 + randint(3));
	break;

      case 3:			   /* Inner pillar(s).	 */
	tmp = randint(4);
	if (tmp < 3) {		   /* Place a door	 */
	    if (tmp == 1)
		place_secret_door(y_height - 1, xval);
	    else
		place_secret_door(y_depth + 1, xval);
	} else {
	    if (tmp == 3)
		place_secret_door(yval, x_left - 1);
	    else
		place_secret_door(yval, x_right + 1);
	}

	for (i = yval - 1; i <= yval + 1; i++) {
	    c_ptr = &cave[i][xval - 1];
	    for (j = xval - 1; j <= xval + 1; j++) {
		c_ptr->fval = TMP1_WALL;
		c_ptr++;
	    }
	}
	if (randint(2) == 1) {
	    tmp = randint(2);
	    for (i = yval - 1; i <= yval + 1; i++) {
		c_ptr = &cave[i][xval - 5 - tmp];
		for (j = xval - 5 - tmp; j <= xval - 3 - tmp; j++) {
		    c_ptr->fval = TMP1_WALL;
		    c_ptr++;
		}
	    }
	    for (i = yval - 1; i <= yval + 1; i++) {
		c_ptr = &cave[i][xval + 3 + tmp];
		for (j = xval + 3 + tmp; j <= xval + 5 + tmp; j++) {
		    c_ptr->fval = TMP1_WALL;
		    c_ptr++;
		}
	    }
	}
	if (randint(3) == 1) {	   /* Inner rooms	 */
	    c_ptr = &cave[yval - 1][xval - 5];
	    d_ptr = &cave[yval + 1][xval - 5];
	    for (i = xval - 5; i <= xval + 5; i++) {
		c_ptr->fval = TMP1_WALL;
		c_ptr++;
		d_ptr->fval = TMP1_WALL;
		d_ptr++;
	    }
	    cave[yval][xval - 5].fval = TMP1_WALL;
	    cave[yval][xval + 5].fval = TMP1_WALL;
	    place_secret_door(yval - 3 + (randint(2) << 1), xval - 3);
	    place_secret_door(yval - 3 + (randint(2) << 1), xval + 3);
	    object_level = dun_level;
	    if (randint(3) == 1)
		place_object(yval, xval - 2);
	    if (randint(3) == 1)
		place_object(yval, xval + 2);
	    vault_monster(yval, xval - 2, randint(2));
	    vault_monster(yval, xval + 2, randint(2));
	}
	break;

      case 4:			   /* Maze inside.	 */
	tmp = randint(4);
	if (tmp < 3) {		   /* Place a door	 */
	    if (tmp == 1)
		place_secret_door(y_height - 1, xval);
	    else
		place_secret_door(y_depth + 1, xval);
	} else {
	    if (tmp == 3)
		place_secret_door(yval, x_left - 1);
	    else
		place_secret_door(yval, x_right + 1);
	}

	for (i = y_height; i <= y_depth; i++)
	    for (j = x_left; j <= x_right; j++)
		if (0x1 & (j + i))
		    cave[i][j].fval = TMP1_WALL;

    /* Monsters just love mazes.		 */
	vault_monster(yval, xval - 5, randint(3));
	vault_monster(yval, xval + 5, randint(3));
    /* Traps make them entertaining.	 */
	vault_trap(yval, xval - 3, 2, 8, randint(3));
	vault_trap(yval, xval + 3, 2, 8, randint(3));
    /* Mazes should have some treasure too..	 */
	for (i = 0; i < 3; i++)
	    random_object(yval, xval, 1);
	break;

      case 5:			   /* Four small rooms.	 */
	for (i = y_height; i <= y_depth; i++)
	    cave[i][xval].fval = TMP1_WALL;

	c_ptr = &cave[yval][x_left];
	for (i = x_left; i <= x_right; i++) {
	    c_ptr->fval = TMP1_WALL;
	    c_ptr++;
	}

	if (randint(2) == 1) {
	    i = randint(10);
	    place_secret_door(y_height - 1, xval - i);
	    place_secret_door(y_height - 1, xval + i);
	    place_secret_door(y_depth + 1, xval - i);
	    place_secret_door(y_depth + 1, xval + i);
	} else {
	    i = randint(3);
	    place_secret_door(yval + i, x_left - 1);
	    place_secret_door(yval - i, x_left - 1);
	    place_secret_door(yval + i, x_right + 1);
	    place_secret_door(yval - i, x_right + 1);
	}

    /* Treasure in each one.		 */
	random_object(yval, xval, 2 + randint(2));
    /* Gotta have some monsters.		 */
	vault_monster(yval + 2, xval - 4, randint(2));
	vault_monster(yval + 2, xval + 4, randint(2));
	vault_monster(yval - 2, xval - 4, randint(2));
	vault_monster(yval - 2, xval + 4, randint(2));
	break;
    }
}


/*
 * Builds a room at a row, column coordinate
 * Type 3 unusual rooms are cross shaped	
 */
static void build_type3(int yval, int xval)
{
    int			y_height, y_depth;
    int			x_left, x_right;
    int			tmp, i, j;
    byte		floor;
    cave_type		*c_ptr;

/* quick, basic bounds-check... If too close, then this is better suited to a
 * simpler room type. -CFT 
 */
    if (!in_bounds(yval - 3, xval - 3) || !in_bounds(yval + 3, xval + 3)) {
	build_type1(yval, xval);
	return;
    }
    if (dun_level <= randint(25))
	floor = LIGHT_FLOOR;	   /* Floor with light	 */
    else
	floor = DARK_FLOOR;	   /* Dark floor	 */
    tmp = 2 + randint(2);
    y_height = yval - tmp;
    y_depth = yval + tmp;
    x_left = xval - 1;
    x_right = xval + 1;

/* for paranoia's sake: bounds-check!  Memory errors caused by accessing
 * cave[][] out-of-bounds are nearly impossible to spot!  -CFT 
 */
    if (y_height < 1)
	y_height = 1;
    if (y_depth >= (cur_height - 1))
	y_depth = cur_height - 2;
    for (i = y_height; i <= y_depth; i++)
	for (j = x_left; j <= x_right; j++) {
	    c_ptr = &cave[i][j];
	    c_ptr->fval = floor;
	    c_ptr->lr = TRUE;
	}
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	c_ptr = &cave[i][x_left - 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr = &cave[i][x_right + 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
    }
    for (i = x_left; i <= x_right; i++) {
	c_ptr = &cave[y_height - 1][i];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr = &cave[y_depth + 1][i];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
    }
    tmp = 2 + randint(9);
    y_height = yval - 1;
    y_depth = yval + 1;
    x_left = xval - tmp;
    x_right = xval + tmp;

/* for paranoia's sake: bounds-check!  Memory errors caused by accessing
 * cave[][] out-of-bounds are nearly impossible to spot!  -CFT 
 */

    if (x_left < 1)
	x_left = 1;
    if (x_right >= (cur_width - 1))
	x_right = cur_width - 2;

    for (i = y_height; i <= y_depth; i++)
	for (j = x_left; j <= x_right; j++) {
	    c_ptr = &cave[i][j];
	    c_ptr->fval = floor;
	    c_ptr->lr = TRUE;
	}
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	c_ptr = &cave[i][x_left - 1];
	if (c_ptr->fval != floor) {
	    c_ptr->fval = GRANITE_WALL;
	    c_ptr->lr = TRUE;
	}
	c_ptr = &cave[i][x_right + 1];
	if (c_ptr->fval != floor) {
	    c_ptr->fval = GRANITE_WALL;
	    c_ptr->lr = TRUE;
	}
    }
    for (i = x_left; i <= x_right; i++) {
	c_ptr = &cave[y_height - 1][i];
	if (c_ptr->fval != floor) {
	    c_ptr->fval = GRANITE_WALL;
	    c_ptr->lr = TRUE;
	}
	c_ptr = &cave[y_depth + 1][i];
	if (c_ptr->fval != floor) {
	    c_ptr->fval = GRANITE_WALL;
	    c_ptr->lr = TRUE;
	}
    }
/* Special features.			 */
    switch (randint(4)) {
      case 1:			   /* Large middle pillar		 */
	for (i = yval - 1; i <= yval + 1; i++) {
	    c_ptr = &cave[i][xval - 1];
	    for (j = xval - 1; j <= xval + 1; j++) {
		c_ptr->fval = TMP1_WALL;
		c_ptr++;
	    }
	}
	break;

      case 2:			   /* Inner treasure vault		 */
	for (i = yval - 1; i <= yval + 1; i++) {
	    cave[i][xval - 1].fval = TMP1_WALL;
	    cave[i][xval + 1].fval = TMP1_WALL;
	}
	cave[yval - 1][xval].fval = TMP1_WALL;
	cave[yval + 1][xval].fval = TMP1_WALL;

	tmp = randint(4);	   /* Place a door	 */
	if (tmp < 3)
	    place_secret_door(yval - 3 + (tmp << 1), xval);
	else
	    place_secret_door(yval, xval - 7 + (tmp << 1));

    /* Place a treasure in the vault		 */
	object_level = dun_level;
	place_object(yval, xval);
    /* Let's guard the treasure well.	 */
	vault_monster(yval, xval, 2 + randint(2));
    /* Traps naturally			 */
	vault_trap(yval, xval, 4, 4, 1 + randint(3));
	break;

      case 3:
	if (randint(3) == 1) {
	    cave[yval - 1][xval - 2].fval = TMP1_WALL;
	    cave[yval + 1][xval - 2].fval = TMP1_WALL;
	    cave[yval - 1][xval + 2].fval = TMP1_WALL;
	    cave[yval + 1][xval + 2].fval = TMP1_WALL;
	    cave[yval - 2][xval - 1].fval = TMP1_WALL;
	    cave[yval - 2][xval + 1].fval = TMP1_WALL;
	    cave[yval + 2][xval - 1].fval = TMP1_WALL;
	    cave[yval + 2][xval + 1].fval = TMP1_WALL;
	    if (randint(3) == 1) {
		place_secret_door(yval, xval - 2);
		place_secret_door(yval, xval + 2);
		place_secret_door(yval - 2, xval);
		place_secret_door(yval + 2, xval);
	    }
	} else if (randint(3) == 1) {
	    cave[yval][xval].fval = TMP1_WALL;
	    cave[yval - 1][xval].fval = TMP1_WALL;
	    cave[yval + 1][xval].fval = TMP1_WALL;
	    cave[yval][xval - 1].fval = TMP1_WALL;
	    cave[yval][xval + 1].fval = TMP1_WALL;
	} else if (randint(3) == 1)
	    cave[yval][xval].fval = TMP1_WALL;
	break;

      case 4:
	break;
    }
}

static void 
vault_nasty(j, i, type, rank, colour)
int j, i, type, rank, colour;
{
    switch (type) {
      case 1:
	vault_jelly(j, i);
	break;
      case 2:
	vault_orc(j, i, rank);
	break;
      case 3:
	vault_troll(j, i, rank);
	break;
      case 4:
	vault_undead(j, i);
	break;
      case 5:
	vault_dragon(j, i, rank, colour);
	break;
      case 6:
	vault_demon(j, i, rank);
	break;
      case 7:
	vault_giant(j, i, rank);
	break;
    }
}

static void special_pit(int yval, int xval, int type)
{
    register int        i, j, y_height, x_left;
    int                 y_depth, x_right, colour;
    byte               floor;
    register cave_type *c_ptr, *d_ptr;

    floor = DARK_FLOOR;
    y_height = yval - 4;
    y_depth = yval + 4;
    x_left = xval - 11;
    x_right = xval + 11;

    /* for paranoia's sake: bounds-check!  Memory errors caused by
     * accessing cave[][] out-of-bounds are nearly impossible to
     * spot!  -CFT */
    if ((y_height < 1) || (y_depth >= (cur_height-1)) ||
	(x_left < 1) || (x_right >= (cur_width-1))) {
	build_type1(yval,xval); /* type1 is heavily bounds-checked, and considered
				 * safe to use as a fall-back room type -CFT */
	return;
    } else
	if ((randint(dun_level*dun_level + 1) < 300) && (dun_level <= 40))
	    good_item_flag = TRUE;

/* the x dim of rooms tends to be much larger than the y dim, so don't bother
 * rewriting the y loop 
 */

    for (i = y_height; i <= y_depth; i++) {
	c_ptr = &cave[i][x_left];
	for (j = x_left; j <= x_right; j++) {
	    c_ptr->fval = floor;
	    c_ptr->lr = TRUE;
	    c_ptr++;
	}
    }
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	c_ptr = &cave[i][x_left - 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr = &cave[i][x_right + 1];
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
    }
    c_ptr = &cave[y_height - 1][x_left];
    d_ptr = &cave[y_depth + 1][x_left];
    for (i = x_left; i <= x_right; i++) {
	c_ptr->fval = GRANITE_WALL;
	c_ptr->lr = TRUE;
	c_ptr++;
	d_ptr->fval = GRANITE_WALL;
	d_ptr->lr = TRUE;
	d_ptr++;
    }
/* The inner room		 */
    y_height = y_height + 2;
    y_depth = y_depth - 2;
    x_left = x_left + 2;
    x_right = x_right - 2;
    for (i = (y_height - 1); i <= (y_depth + 1); i++) {
	cave[i][x_left - 1].fval = TMP1_WALL;
	cave[i][x_right + 1].fval = TMP1_WALL;
    }
    c_ptr = &cave[y_height - 1][x_left];
    d_ptr = &cave[y_depth + 1][x_left];
    for (i = x_left; i <= x_right; i++) {
	c_ptr->fval = TMP1_WALL;
	c_ptr++;
	d_ptr->fval = TMP1_WALL;
	d_ptr++;
    }
    switch (randint(4)) {
      case 1:
	place_secret_door(y_height - 1, xval);
	break;
      case 2:
	place_secret_door(y_depth + 1, xval);
	break;
      case 3:
	place_secret_door(yval, x_left - 1);
	break;
      case 4:
	place_secret_door(yval, x_right + 1);
	break;
    }
    colour = randint(6);
    if (wizard || peek) {
	switch (type) {
	  case 1:
	    msg_print("A Slime Pit");
	    break;
	  case 2:
	    msg_print("An Orc Pit");
	    break;
	  case 3:
	    msg_print("A Troll Pit");
	    break;
	  case 4:
	    msg_print("A Graveyard");
	    break;
	  case 5:
	    switch (colour) {
	      case 1:
		msg_print("A Blue Dragon Pit");
		break;
	      case 2:
		msg_print("A White Dragon Pit");
		break;
	      case 3:
		msg_print("A Green Dragon Pit");
		break;
	      case 4:
		msg_print("A Black Dragon Pit");
		break;
	      case 5:
		msg_print("A Red Dragon Pit");
		break;
	      case 6:
		msg_print("A Multi-Hued Dragon Pit");
		break;
	    }
	    break;
	  case 6:
	    msg_print("A Demon Pit");
	    break;
	  case 7:
	    msg_print("A Giant Pit");
	    break;
	}
    }
    j = y_height;
    for (i = x_left; i <= x_right; i++)
	vault_nasty(j, i, type, 1, colour);
    j = y_depth;
    for (i = x_left; i <= x_right; i++)
	vault_nasty(j, i, type, 1, colour);
    i = x_left;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 1, colour);
    i = x_right;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 1, colour);
    i = x_left + 1;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 2, colour);
    i = x_left + 2;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 2, colour);
    i = x_right - 1;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 2, colour);
    i = x_right - 2;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 2, colour);
    i = x_left + 3;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 3, colour);
    i = x_left + 4;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 3, colour);
    i = x_right - 3;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 3, colour);
    i = x_right - 4;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 3, colour);
    i = x_left + 5;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 4, colour);
    i = x_left + 6;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 4, colour);
    i = x_right - 5;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 4, colour);
    i = x_right - 6;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 4, colour);
    i = x_left + 7;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 5, colour);
    i = x_left + 8;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 5, colour);
    i = x_right - 7;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 5, colour);
    i = x_right - 8;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 5, colour);
    i = x_right - 9;
    for (j = (y_height + 1); j <= (y_depth - 1); j++)
	vault_nasty(j, i, type, 6, colour);
}


/*
 * Constructs a tunnel between two points
 */
static void build_tunnel(int row1, int col1, int row2, int col2)
{
    register int        tmp_row, tmp_col, i, j;
    cave_type		*c_ptr;
    coords              tunstk[1000], wallstk[1000];
    coords		*tun_ptr;
    int                 row_dir, col_dir, tunindex, wallindex;
    int                 stop_flag, door_flag, main_loop_count;
    int                 start_row, start_col;

    stop_flag = FALSE;
    door_flag = FALSE;
    tunindex = 0;
    wallindex = 0;
    main_loop_count = 0;
    start_row = row1;
    start_col = col1;
    correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

    do {

	/* Paranoia -- prevent infinite loops */
	if (main_loop_count++ > 2000) stop_flag = TRUE;

	if (randint(100) > DUN_TUN_CHG) {
	    if (randint(DUN_TUN_RND) == 1)
		rand_dir(&row_dir, &col_dir);
	    else
		correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
	}
	tmp_row = row1 + row_dir;
	tmp_col = col1 + col_dir;
	while (!in_bounds(tmp_row, tmp_col)) {
	    if (randint(DUN_TUN_RND) == 1)
		rand_dir(&row_dir, &col_dir);
	    else
		correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
	    tmp_row = row1 + row_dir;
	    tmp_col = col1 + col_dir;
	}
	c_ptr = &cave[tmp_row][tmp_col];
	if (c_ptr->fval == NULL_WALL) {
	    row1 = tmp_row;
	    col1 = tmp_col;
	    if (tunindex < 1000) {
		tunstk[tunindex].y = row1;
		tunstk[tunindex].x = col1;
		tunindex++;
	    }
	    door_flag = FALSE;
	}
	else if (c_ptr->fval == TMP2_WALL) {
	    /* do nothing */
	}
	else if (c_ptr->fval == GRANITE_WALL) {

	    cave_type *d_ptr = &cave[tmp_row + row_dir][tmp_col + col_dir];

	    if ((d_ptr->fval == GRANITE_WALL) || (d_ptr->fval == TMP2_WALL)) {
		c_ptr->fval = TMP2_WALL;
	    }

	/* if can not pass completely through wall don't try... And mark as
	 * impassible for future -KOC 
	 */
	    else {

		/* Save the wall data */
		row1 = tmp_row;
		col1 = tmp_col;
		if (wallindex < 1000) {
		    wallstk[wallindex].y = row1;
		    wallstk[wallindex].x = col1;
		    wallindex++;
		}
		for (i = row1 - 1; i <= row1 + 1; i++) {
		    for (j = col1 - 1; j <= col1 + 1; j++) {
			if (in_bounds(i, j)) {
			    d_ptr = &cave[i][j];
			/*
			 * values 11 and 12 are impossible here,
			 * place_streamer is never run before build_tunnel 
			 */
			    if (d_ptr->fval == GRANITE_WALL) {
				d_ptr->fval = TMP2_WALL;
			    }
			}
		    }
		}
	    }
	}

	/* Check for corridor */	
	else if (c_ptr->fval == CORR_FLOOR ||
		 (c_ptr->fval == BLOCKED_FLOOR)) {

	    row1 = tmp_row;
	    col1 = tmp_col;
	    if (!door_flag) {
		if (doorindex < 100) {
		    doorstk[doorindex].y = row1;
		    doorstk[doorindex].x = col1;
		    doorindex++;
		}
		door_flag = TRUE;
	    }

	    /* make sure that tunnel has gone a reasonable distance before
	     * stopping it, this helps prevent isolated rooms 
	     */
	    if (randint(100) > DUN_TUN_CON) {
		tmp_row = row1 - start_row;
		if (tmp_row < 0) {
		    tmp_row = (-tmp_row);
		}
		tmp_col = col1 - start_col;
		if (tmp_col < 0) {
		    tmp_col = (-tmp_col);
		}
		if (tmp_row > 10 || tmp_col > 10) {
		    stop_flag = TRUE;
		}
	    }
	}

	/* c_ptr->fval != NULL, TMP2, GRANITE, CORR */
	else {
	    row1 = tmp_row;
	    col1 = tmp_col;
	}
    }
    while (((row1 != row2) || (col1 != col2)) && (!stop_flag));

    tun_ptr = &tunstk[0];
    for (i = 0; i < tunindex; i++) {
	d_ptr = &cave[tun_ptr->y][tun_ptr->x];
	d_ptr->fval = CORR_FLOOR;
	tun_ptr++;
    }

    for (i = 0; i < wallindex; i++) {
	c_ptr = &cave[wallstk[i].y][wallstk[i].x];
	if (c_ptr->fval == TMP2_WALL) {
	    if (randint(100) < DUN_TUN_PEN) {
		place_door(wallstk[i].y, wallstk[i].x);
	    }
	    else {
		/* these have to be doorways to rooms */
		c_ptr->fval = CORR_FLOOR;
	    }
	}
    }
}


/*
 * Determine if the given location is between two walls,
 * and next to two corridor spaces.
 */

static int next_to(int y, int x)
{
    int next = FALSE;

    /* Paranoia */
    if (!in_bounds(y, x)) return FALSE;

    /* Count the adjacent corridors */
    if (next_to_corr(y, x) > 2) {
	if ((cave[y - 1][x].fval >= MIN_WALL) &&
	    (cave[y + 1][x].fval >= MIN_WALL)) {
	    next = TRUE;
	}
	else if ((cave[y][x - 1].fval >= MIN_WALL) &&
		 (cave[y][x + 1].fval >= MIN_WALL)) {
	    next = TRUE;
	}
	else {
	    next = FALSE;
	}
    }

    return (next);
}

/*
 * Places door at y, x position if at least 2 walls found	 
 */
static void try_door(int y, int x)
{
    /* Paranoia */
    if (!in_bounds(y, x)) return;

    /* Floor, next to walls, and corridors */
    if ((cave[y][x].fval == CORR_FLOOR) &&
	(randint(100) > DUN_TUN_JCT) &&
	next_to(y, x)) {

	/* Place a door */
	place_door(y, x);
    }
}


static void build_pit(int yval, int xval)
{
    int tmp;

    tmp = randint(dun_level > 80 ? 80 : dun_level);
    rating += 10;
    if (tmp < 10)
	special_pit(yval, xval, 1);
    else if (tmp < 20)
	special_pit(yval, xval, 2);
    else if (tmp < 43)
	((randint(3) == 1)
	 ? special_pit(yval, xval, 7)
	 : special_pit(yval, xval, 3));
    else if (tmp < 57)
	special_pit(yval, xval, 4);
    else if (tmp < 73)
	special_pit(yval, xval, 5);
    else
	special_pit(yval, xval, 6);
}

/*
 * Cave logic flow for generation of new dungeon
 */
static void cave_gen(void)
{
    struct spot_type {
	int endx;
	int endy;
    };
    int         room_map[20][20];
    int		i, j, k;
    int         y1, x1, y2, x2, pick1, pick2, tmp;
    int         row_rooms, col_rooms, alloc_level;
    s16b       yloc[400], xloc[400];
    int          pit_ok, spec_level;

    rating = 0;
    spec_level = 0;

    /* Assume no pits allowed */
    pit_ok = FALSE;

    if ((randint(DUN_DEST) == 1) && (dun_level > 10) && (!is_quest(dun_level))) {
	if (wizard) msg_print("Destroyed Level");
	spec_level = SPEC_DEST;
    }
    else {
	pit_ok = TRUE;
    }

    row_rooms = 2 * (cur_height / SCREEN_HEIGHT);
    col_rooms = 2 * (cur_width / SCREEN_WIDTH);
    for (i = 0; i < row_rooms; i++) {
	for (j = 0; j < col_rooms; j++) {
	    room_map[i][j] = FALSE;
	}
    }

    k = randnor(DUN_ROO_MEA, 2);
    for (i = 0; i < k; i++) {
	room_map[randint(row_rooms) - 1][randint(col_rooms) - 1] = TRUE;
    }

    k = 0;
    for (i = 0; i < row_rooms; i++) {
	for (j = 0; j < col_rooms; j++) {
	    if (room_map[i][j]) {
		yloc[k] = i * (SCREEN_HEIGHT >> 1) + QUART_HEIGHT;
		xloc[k] = j * (SCREEN_WIDTH >> 1) + QUART_WIDTH;
		if (dun_level > randint(DUN_UNUSUAL)) {
		    tmp = randint(5);
		    if ((tmp == 1) || (spec_level)) {
			build_type1(yloc[k], xloc[k]);
		    }
		    else if (tmp == 2) {
			build_type2(yloc[k], xloc[k]);
		    }
		    else if (tmp == 3) {
			build_type3(yloc[k], xloc[k]);
		    }
		    else if ((tmp == 4) && (dun_level > randint(DUN_UNUSUAL))) {
			build_type5(yloc[k], xloc[k]);
			if (j + 1 < col_rooms) {
			    room_map[i][j + 1] = FALSE;
			}
			if (j + 1 < col_rooms && i + 1 < row_rooms) {
			    room_map[i + 1][j + 1] = FALSE;
			}
			if (j > 0 && i + 1 < row_rooms) {
			    room_map[i + 1][j - 1] = FALSE;
			}
			if (i + 1 < row_rooms) {
			    room_map[i + 1][j] = FALSE;
			}
		    }
		    else if (pit_ok && (dun_level > randint(DUN_UNUSUAL))) {
			build_pit(yloc[k], xloc[k]);
			pit_ok = FALSE;
		    }
		    else {
			build_room(yloc[k], xloc[k]);
		    }
		}
		else {
		    build_room(yloc[k], xloc[k]);
		}
		k++;
	    }
	}
    }

    for (i = 0; i < k; i++) {
	pick1 = rand_int(k);
	pick2 = rand_int(k);
	y1 = yloc[pick1];
	x1 = xloc[pick1];
	yloc[pick1] = yloc[pick2];
	xloc[pick1] = xloc[pick2];
	yloc[pick2] = y1;
	xloc[pick2] = x1;
    }

    doorindex = 0;

    /* move zero entry to k, so that can call build_tunnel all k times */
    yloc[k] = yloc[0];
    xloc[k] = xloc[0];
    for (i = 0; i < k; i++) {
	y1 = yloc[i];
	x1 = xloc[i];
	y2 = yloc[i + 1];
	x2 = xloc[i + 1];
	build_tunnel(y2, x2, y1, x1);
    }

    /* Fill the dungeon with walls */
    fill_cave(GRANITE_WALL);

    /* Add some streamers */
    for (i = 0; i < DUN_STR_MAG; i++) place_streamer(MAGMA_WALL, DUN_STR_MC);
    for (i = 0; i < DUN_STR_QUA; i++) place_streamer(QUARTZ_WALL, DUN_STR_QC);

    /* Place the boundary walls */
    place_boundary();

    /* Place intersection doors	 */
    for (i = 0; i < doorindex; i++) {
	try_door(doorstk[i].y, doorstk[i].x - 1);
	try_door(doorstk[i].y, doorstk[i].x + 1);
	try_door(doorstk[i].y - 1, doorstk[i].x);
	try_door(doorstk[i].y + 1, doorstk[i].x);
    }

    /* Destroy the level if necessary */
    if (spec_level == SPEC_DEST) place_destroyed();

    /* What is this used for? */
    alloc_level = (dun_level / 3);
    if (alloc_level < 2) alloc_level = 2;
    else if (alloc_level > 10) alloc_level = 10;

    /* Always place some stairs */    
    place_stairs(2, rand_range(3,4), 3);
    place_stairs(1, rand_range(1,2), 3);

/* Set up the character co-ords, used by alloc_monster, place_win_monster */
    new_spot(&char_row, &char_col);

    /* Allocate some monsters */
    alloc_monster((randint(8) + MIN_M_ALLOC_LEVEL + alloc_level), 0, TRUE);

    /* Put some treasures in corridors */
    alloc_object(ALLOC_SET_CORR, 3, randint(alloc_level));

    /* Put some treasures in rooms */
    alloc_object(ALLOC_SET_ROOM, 5, randnor(TREAS_ROOM_ALLOC, 3));

    /* Put some treasures on the floor (not on doors) */
    alloc_object(ALLOC_SET_BOTH, 5, randnor(TREAS_ANY_ALLOC, 3));
    alloc_object(ALLOC_SET_BOTH, 4, randnor(TREAS_GOLD_ALLOC, 3));
    alloc_object(ALLOC_SET_BOTH, 1, randint(alloc_level));


	/* Attempt to place a ghost */
	if (place_ghost()) {

	    /* A ghost makes the level special */
	    good_item_flag = TRUE;
	}

    else if (spec_level == SPEC_DEST) {
	int flag, counter = 0;
	do {
	    flag = place_ghost();
	    counter++;
	} while (!flag && counter < 10);
	if (flag)
	    good_item_flag = TRUE;
    }


    /* XXX XXX Hack -- Possibly place the winning monster */
    /* Perhaps should just use the normal monster allocators. */
    /* Note that they will automatically generate him eventually. */
    /* Also note that the code below means that he will NOT appear */
    /* if the dungeon level is entered without "making" him. */
    if ((dun_level >= WIN_MON_APPEAR) && (randint(5) < 4)) {
	place_win_monster();
    }
}



/*
 * Builds a store at a given (row, column)
 */
static void build_store(int store_num, int y, int x)
{
    int                 yval, y_height, y_depth;
    int                 xval, x_left, x_right;
    register int        i, j;
    int                 cur_pos, tmp;
    cave_type		*c_ptr;

    /* Find the "center" of the store */
    yval = y * 10 + 5;
    xval = x * 14 + 12;

    /* Determine the store boundaries */
    y_height = yval - randint(3);
    y_depth = yval + randint(4);
    x_left = xval - randint(4);
    x_right = xval + randint(4);

    /* Build an invulnerable rectangular building */    
    for (i = y_height; i <= y_depth; i++)
	for (j = x_left; j <= x_right; j++)

	    /* The buildings are invincible */
	    cave[i][j].fval = BOUNDARY_WALL;

    tmp = randint(4);
    if (tmp < 3) {
	i = randint(y_depth - y_height) + y_height - 1;
	if (tmp == 1)
	    j = x_left;
	else
	    j = x_right;
    } else {
	j = randint(x_right - x_left) + x_left - 1;
	if (tmp == 3)
	    i = y_depth;
	else
	    i = y_height;
    }
    c_ptr = &cave[i][j];
    c_ptr->fval = CORR_FLOOR;
    cur_pos = i_pop();
    c_ptr->i_idx = cur_pos;
    invcopy(&i_list[cur_pos], OBJ_STORE_DOOR + store_num);
}


/*
 * Town logic flow for generation of new town		 
 */
static void town_gen(void)
{
    register int        i, j, l, m;
    register cave_type *c_ptr;
    int                 rooms[MAX_STORES], k;

    set_seed(town_seed);
    for (i = 0; i < MAX_STORES; i++)
	rooms[i] = i;
    l = MAX_STORES;
    for (i = 0; i < 2; i++)
	for (j = 0; j < 4; j++) {
	    k = randint(l) - 1;
	    build_store(rooms[k], i, j);
	    for (m = k; m < l - 1; m++)
		rooms[m] = rooms[m + 1];
	    l--;
	}
    fill_cave(DARK_FLOOR);
/* make stairs before reset_seed, so that they don't move around */
    place_boundary();
    place_stairs(2, 1, 0);
    reset_seed();
/* Set up the character co-ords, used by alloc_monster below */
    new_spot(&char_row, &char_col);
    if (0x1 & (turn / 5000)) {	   /* Night	 */
	for (i = 0; i < cur_height; i++) {
	    c_ptr = &cave[i][0];
	    for (j = 0; j < cur_width; j++) {
		if (c_ptr->fval != DARK_FLOOR)
		    c_ptr->pl = TRUE;
		c_ptr++;
	    }
	}
	alloc_monster(MIN_M_ALLOC_TN, 3, TRUE);
    } else {			   /* Day	 */
	for (i = 0; i < cur_height; i++) {
	    c_ptr = &cave[i][0];
	    for (j = 0; j < cur_width; j++) {
		c_ptr->pl = TRUE;
		c_ptr++;
	    }
	}

	/* Make some daytime residents */
	alloc_monster(MIN_M_ALLOC_TD, 3, TRUE);
    }
    store_maint();
    place_ghost();
}


/*
 * Generates a random dungeon level			-RAK-	 
 */
void generate_cave()
{
    /* No current panel yet (actually a 1x1 panel) */
    panel_row_min = 0;
    panel_row_max = 0;
    panel_col_min = 0;
    panel_col_max = 0;
    char_row = (-1);
    char_col = (-1);

    /* Totally wipe the object list */
    wipe_i_list();

    /* Totally wipe the monster list */
    wipe_m_list();

    /* Start with a blank cave */
    blank_cave();

    /* Important -- Reset the object generation level */
    object_level = dun_level;

    /* Build the town */
    if (!dun_level) {
	cur_height = SCREEN_HEIGHT;
	cur_width = SCREEN_WIDTH;
	max_panel_rows = (cur_height / SCREEN_HEIGHT) * 2 - 2;
	max_panel_cols = (cur_width / SCREEN_WIDTH) * 2 - 2;
	panel_row = max_panel_rows;
	panel_col = max_panel_cols;
	town_gen();
    }

    /* Build a real level */
    else {
	cur_height = MAX_HEIGHT;
	cur_width = MAX_WIDTH;
	max_panel_rows = (cur_height / SCREEN_HEIGHT) * 2 - 2;
	max_panel_cols = (cur_width / SCREEN_WIDTH) * 2 - 2;
	panel_row = max_panel_rows;
	panel_col = max_panel_cols;
	cave_gen();
    }

    /* Extract the feeling */
    unfelt = TRUE;
    feeling = 0;
    if (rating > 100) feeling = 2;
    else if (rating > 80) feeling = 3;
    else if (rating > 60) feeling = 4;
    else if (rating > 40) feeling = 5;
    else if (rating > 30) feeling = 6;
    else if (rating > 20) feeling = 7;
    else if (rating > 10) feeling = 8;
    else if (rating > 0) feeling = 9;
    else feeling = 10;

    /* Have a special feeling, explicitly */
    if (good_item_flag) feeling = 1;
}

