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




/*
 * Free some memory (that was allocated by ralloc).
 */
int rnfree(void *p, unsigned long len)
{
  /* Easy to free zero bytes */
  if (len == 0) return (0);

  /* just use "free" */
  free ((char*)(p));

  /* Success */
  return (0);
}


/*
 * Allocate some memory
 */
void *ralloc(unsigned long len)
{
  void *mem;

  /* Allow allocation of "zero bytes" */
  if (len == 0) return ((void *)NULL);

  /* Use malloc() to allocate some memory */
  mem = ((void *)(malloc((size_t)(len))));

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
  register unsigned long len = 0;
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
  register unsigned long len = 0;

  /* Succeed on non-strings */
  if (!str) return (0);

  /* Count the number of chars in 'str' plus the terminator */
  while (str[len++]);

  /* Kill the buffer of chars we must have allocated above */
  rnfree((vptr)(str), len);

  /* Success */
  return (0);
}


