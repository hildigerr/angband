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
    int                 nfds, readfds, writefds, exceptfds;
    struct timeval      Timer;

    nfds = readfds = writefds = exceptfds = 0;
    
    if (microSeconds > 4000000L) {
	errno = ERANGE;		   /* value out of range */
	perror("usleep time out of range ( 0 -> 4000000 ) ");
	return -1;
    }
    Timer.tv_sec = (microSeconds / 1000000L);
    Timer.tv_usec = (microSeconds % 1000000L);

    if (select(nfds, &readfds, &writefds, &exceptfds, &Timer) < 0) {
	if (errno != EINTR) {
	    perror("usleep (select) failed");
	    return -1;
	}
    }
    return 0;
}

#endif
