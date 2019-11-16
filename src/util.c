/*
 * util.c: miscellanous utilities 
 *
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



#ifndef HAS_USLEEP
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <errno.h>

/*
 * for those systems that don't have usleep
 * grabbed from the inl netrek server -cba 
 */
int usleep(unsigned long microSeconds)
{
    struct timeval      Timer;

    int                 nfds = 0;

    int			*no_fds = NULL;


    /* Was: int readfds, writefds, exceptfds; */
    /* Was: readfds = writefds = exceptfds = 0; */


    /* Paranoia -- No excessive sleeping */
    if (microSeconds > 4000000L) core("Illegal usleep() call");
    

    /* Wait for it */
    Timer.tv_sec = (microSeconds / 1000000L);
    Timer.tv_usec = (microSeconds % 1000000L);

    /* Wait for it */
    if (select(nfds, no_fds, no_fds, no_fds, &Timer) < 0) {

	/* Hack -- ignore interrupts */
	if (errno != EINTR) return -1;
    }
    
    /* Success */
    return 0;
}

#endif
