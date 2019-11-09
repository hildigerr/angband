/*
 * angband.h: general header file for inclusion in all source files
 *
 * Copyright (c) 1994 Charles W. Swiger
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/* I am doing this so that every source file is guaranteed to be able to find
 * all of the declarations and have every header file be idempotent (able to be
 * included many times without error). -CWS
 */

#ifndef _ANGBAND_H_
#define _ANGBAND_H_

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

/* Here's some functions that've been macroized rather than being called
 * from everywhere.  They're short enough so that inlining them will probably
 * result in a smaller executable, and speed things up, to boot. -CWS
 */

#define MY_MAX(a,b) ((a) > (b) ? (a) : (b))
#define MY_MIN(a,b) ((a) < (b) ? (a) : (b))


/* Checks a co-ordinate for in bounds status		-RAK-	*/

#define in_bounds(y, x) \
   ((((y) > 0) && ((y) < cur_height-1) && ((x) > 0) && ((x) < cur_width-1)) ? \
    (TRUE) : (FALSE))


/* Checks if we can see this point (includes map edges) -CWS */
#define in_bounds2(y, x) \
   ((((y) >= 0) && ((y) < cur_height) && ((x) >= 0) && ((x) < cur_width)) ? \
    (TRUE) : (FALSE))


/* Tests a given point to see if it is within the screen -RAK-
 * boundaries.
 */

#define panel_contains(y, x) \
  ((((y) >= panel_row_min) && ((y) <= panel_row_max) && \
    ((x) >= panel_col_min) && ((x) <= panel_col_max)) ? (TRUE) : (FALSE))


/* Generates a random integer X where 1<=X<=MAXVAL -CWS */

#define randint(maxval) (((maxval) < 1) ? (1) : ((random() % (maxval)) + 1))
#define randint2(maxval) ((random() % (maxval)) + 1)

/* You would think that most compilers can do an integral abs() quickly,
 * wouldn't you?  Nope.  [But fabs is a lot worse on most machines!] -CWS
 */

#define MY_ABS(x) (((x)<0) ? (-x) : (x))


#endif /* _ANGBAND_H_ */
