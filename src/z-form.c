/* File: z-form.c */

#include "z-form.h"

#include "z-util.h"
#include "z-virt.h"


/*
 * Vararg interface to plog()
 */
void plog_fmt(const char *fmt, ...)
{
  char *res;
  va_list vp;

  /* Begin the Varargs Stuff */
  va_start(vp, fmt);

  /* Format the args */
  res = vformat(fmt, vp);

  /* End the Varargs Stuff */
  va_end(vp);

  /* Call plog */
  plog(res);
}



/*
 * Vararg interface to quit()
 */
void quit_fmt(const char *fmt, ...)
{
  char *res;
  va_list vp;

  /* Begin the Varargs Stuff */
  va_start(vp, fmt);

  /* Format */
  res = vformat(fmt, vp);

  /* End the Varargs Stuff */
  va_end(vp);

  /* Call quit() */
  quit(res);
}


