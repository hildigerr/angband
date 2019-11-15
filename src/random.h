/* File: random.h */

#ifndef INCLUDED_RANDOM_H
#define INCLUDED_RANDOM_H

#include "h-include.h"

/*
 * Header file for the Random Number Generator
 *
 * This library is a copy of the standard BSD "random" library,
 * with a few minor changes related to error messages and such.
 */


/**** Available functions ****/

extern s32b random(void);
extern void srandom(u32b);
extern char *initstate(u32b, char *, int);
extern char *setstate(char *);


#endif

