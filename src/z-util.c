/* File: z-util.c */

#include "z-util.h"



#ifndef HAS_STRICMP

/*
 * For those systems that don't have stricmp
 */
int stricmp(const char *c1, const char *b)
{
  register const char *s1, s2;
  register const char *c1, c2;

  /* Scan the strings */
  for (s1 = a, s2 = b; TRUE; s1++, s2++)
  {
    c1 = FORCEUPPER(*s1);
    c2 = FORCEUPPER(*s2);
    if (c1 < c2) return (-1);
    if (c1 > c2) return (1);
    if (!c1) return (0);
  }
}

#endif


