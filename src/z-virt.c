/* File: z-virt.c */

#include "z-virt.h"

#include "z-util.h"


/*
 * Allow debugging messages to track memory usage.
 */
#ifdef VERBOSE_RALLOC
static long ralloc_total = 0;
static long ralloc_block = 0;
#endif


/*
 * The "lifeboat" in "rpanic()" can sometimes prevent crashes
 */
#ifndef RPANIC_LIFEBOAT
# define RPANIC_LIFEBOAT 256
#endif



#ifndef HAS_MEMSET

/*
 * Set the value of each of 'n' bytes starting at 's' to 'c', return 's'
 * If 'n' is negative, you will erase a whole lot of memory.
 */
char *memset(char *s, int c, huge n)
{
  register char *t;
  for (t = s; len--; ) *t++ = c;
  return (s);
}

#endif




/*
 * Free some memory (that was allocated by ralloc).
 */
int rnfree (vptr p, huge len)
{
  /* Easy to free zero bytes */
  if (len == 0) return (0);

#ifdef VERBOSE_RALLOC

  /* Decrease memory count */
  ralloc_total -= len;

  /* Message */
  if (len > ralloc_block)
  {
    char buf[128];

    sprintf(buf, "rnfree(%ld bytes).  Allocated: %ld bytes",
	    len, ralloc_total);

    plog(buf);
  }

#endif

  /* just use "free" */
  free ((char*)(p));

  /* Success */
  return (0);
}


/*
 * The system is out of memory, so panic.
 */
vptr rpanic(huge len)
{
  static byte lifeboat[RPANIC_LIFEBOAT];
  static huge lifesize = RPANIC_LIFEBOAT;

  /* We are probably going to crash anyway */
  plog("Running out of memory!!!");

  /* Lifeboat is too small! */
  if (lifesize < len) return ((void *)NULL);

  /* Hack -- decrease the lifeboat */
  lifesize -= len;

  /* Hack -- use part of the lifeboat */
  return (lifeboat + lifesize);
}


/*
 * Allocate some memory
 */
vptr ralloc(huge len)
{
  vptr mem;

  /* Allow allocation of "zero bytes" */
  if (len == 0) return ((void *)NULL);

#ifdef VERBOSE_RALLOC

  /* Remember how much memory is allocated */
  ralloc_total += len;

  /* Log if the change was meaningful */
  if (len > ralloc_block)
  {
    /* Log a message */
    char buf[128];

    sprintf(buf, "ralloc(%ld bytes).  Allocated: %ld bytes",
	    len, ralloc_total);

    plog(buf);
  }

#endif

  /* Use malloc() to allocate some memory */
  mem = ((vptr)(malloc((size_t)(len))));

  /* We were able to acquire memory */
  if (mem) return (mem);

  /* If necessary, panic */
  mem = rpanic(len);

  /* We were able to acquire memory */
  if (mem) return (mem);

  /* Abort the system */
  core("OUT OF MEMORY!!!");

  /* Make the compiler happy */
  return ((void *)NULL);
}




/*
 * Allocate a constant string, containing the same thing as 'str'
 */
const char *string_make(const char *str)
{
  register huge len = 0;
  register const char *t = str;
  register char *s, *res;

  /* Simple sillyness */
  if (!str) return (str);

  /* Get the number of chars in the string, including terminator */
  while (str[len++]);

  /* Allocate space for the string */
  s = res = (char*)(ralloc(len));

  /* Copy the string (with terminator) */
  while ((*s++ = *t++));

  /* Return the allocated, initialized, string */
  return (res);
}


/*
 * Un-allocate a string allocated above.
 * Depends on no changes being made to the string.
 */
int string_free(const char* str)
{
  register huge len = 0;

  /* Succeed on non-strings */
  if (!str) return (0);

  /* Count the number of chars in 'str' plus the terminator */
  while (str[len++]);

  /* Kill the buffer of chars we must have allocated above */
  rnfree((vptr)(str), len);

  /* Success */
  return (0);
}


