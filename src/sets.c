/*
 * sets.c: code to emulate the original Pascal sets 
 *
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


int set_meteor_destroy(inven_type *e)		   /* added -DGK */
{
    byte fi, fo;

    fi = set_fire_destroy(e);
    fo = set_cold_destroy(e);
    return (fi | fo);
}


int set_mana_destroy(inven_type *e)		   /* added -DGK */
{				   /* destroy everything but artifacts */
    if (artifact_p(e))
	return (FALSE);
    return (TRUE);
}


int set_holy_destroy(inven_type *e)		   /* added -DGK */
{
    int element = e->tval;

    if (cursed_p(e) && (!artifact_p(e)))
	return (TRUE);
    return (FALSE);
}


int set_plasma_destroy(inven_type *e)		   /* added -DGK */
{
    byte fi, li;

    fi = set_fire_destroy(e);
    li = set_elec_destroy(e);
    return (fi | li);
}


int set_null(inven_type *e)
{
    return (FALSE);
}


