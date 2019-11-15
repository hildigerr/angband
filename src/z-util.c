/* File: z-util.c */

#include "z-util.h"



#ifdef NEEDS_STRICMP

/*
 * For those systems that don't have stricmp. -hmj
 */
int my_stricmp(const char *c1,const char *c2)		/* avoid namespace collision -CWS */
{
  char c3;
  char c4;

  for(;;)
  {
    c3 = (islower(*c1)?toupper(*c1):*c1);
    c4 = (islower(*c2)?toupper(*c2):*c2);
    if (c3 < c4) return (-1);
    if (c3 > c4) return (1);
    if (c3 == '\0') return (0);
    c1++;
    c2++;
  }
}

#endif


