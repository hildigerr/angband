/* File: z-virt.c */

#include "z-virt.h"

#include "z-util.h"


#ifndef HAS_MEMSET

/*
 * Set the value of each of 'n' bytes starting at 's' to 'c', return 's'
 * If 'n' is negative, you will erase a whole lot of memory.
 */
char *memset(char *s, int c, unsigned long n)
{
  register char *t;
  for (t = s; len--; ) *t++ = c;
  return (s);
}

#endif


