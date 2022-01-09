/* File: z-util.c */

#include "z-util.h"



/*
 * Convenient storage of the program name
 */
cptr argv0 = NULL;



/*
 * Is string 'small' the suffix of string 'big'?
 */
bool suffix(cptr big, cptr small)
{
  register int blen = strlen (big);
  register int slen = strlen (small);

  /* Degenerate case: 'big' is smaller than 'small' */
  if (slen > blen) return (FALSE);

  /* Compare small to the end of big */
  return (!strcmp(big + blen - slen, small));
}


#ifndef HAS_STRICMP

/*
 * For those systems that don't have stricmp
 */
int stricmp(cptr a, cptr b)
{
  register cptr s1, s2;
  register char c1, c2;

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

/*
 * Print (or log) a "warning" message (ala "perror()")
 */
void plog(cptr str)
{
  /* Just do a labeled fprintf to stderr */
  (fprintf(stderr, "%s: %s\n", argv0 ? argv0 : "???", str));
}



/*
 * Exit (ala "exit()").  If 'str' is NULL, do "exit(0)".
 * If 'str' begins with "+" or "-", do "exit(atoi(str))".
 * Otherwise, plog() 'str' and exit with an error code of -1.
 */
void quit(cptr str)
{

  /* Success */
  if (!str) (void)(exit(0));

  /* Extract a "special error code" */
  if ((str[0] == '-') || (str[0] == '+')) (void)(exit(atoi(str)));

  /* Send the string to plog() */
  plog(str);

  /* Failure */
  (void)(exit(-1));
}



/*
 * Dump a core file, after printing a warning message
 */
void core(cptr str)
{
  char *crash = NULL;

  /* Dump the warning string */
  if (str) plog(str);

  /* Attempt to Crash */
  (*crash) = (*crash);

  /* Be sure we exited */
  quit("core() failed");
}




