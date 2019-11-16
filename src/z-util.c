/* File: z-util.c */

#include "z-util.h"



/*
 * Convenient storage of the program name
 */
cptr argv0 = NULL;



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

/*
 * Print (or log) a "warning" message (ala "perror()")
 */
void plog(const char *str)
{
  /* Just do a labeled fprintf to stderr */
  (fprintf(stderr, "%s: %s\n", argv0 ? argv0 : "???", str));
}



/*
 * Exit (ala "exit()").  If 'str' is NULL, do "exit(0)".
 * If 'str' begins with "+" or "-", do "exit(atoi(str))".
 * Otherwise, plog() 'str' and exit with an error code of -1.
 */
void quit(const char *str)
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
void core(const char *str)
{
  char *crash = NULL;

  /* Dump the warning string */
  if (str) plog(str);

  /* Attempt to Crash */
  (*crash) = (*crash);

  /* Be sure we exited */
  quit("core() failed");
}




