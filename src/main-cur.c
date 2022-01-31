/* File: main-cur.c */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart , Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#if defined(unix) || defined(__MINT__)

/* defines CTRL */
#include <sys/ioctl.h>

/* defines TRUE and FALSE */
#ifdef linux
#include <ncurses.h>
#else
#include <curses.h>
#endif


/*
 * Some annoying machines define "bool" in various packages
 * Note that this "redefinition" should work on any machine.
 */

#define bool bool_hack
#include "angband.h"
#undef bool

#if defined(SYS_V) && defined(lint)
/* for AIX, prevent hundreds of unnecessary lint errors, must define before
 * signal.h is included 
 */
#define _h_IEEETRAP
typedef struct {
    int                 stuff;
}                   fpvmach;

#endif

#   include <signal.h>

#ifdef M_XENIX
# include <sys/select.h>
#endif

#ifdef USG
# ifndef __MINT__
#  include <termio.h>
# endif
#else
# if defined(atarist) && defined(__GNUC__) && !defined(__MINT__)
   /* doesn't have <sys/wait.h> */
# else
#  include <sys/wait.h>
# endif
# include <sys/resource.h>
# include <sys/param.h>
#  include <sys/param.h>
#endif


#if defined(SYS_V) && defined(lint)
struct screen {
    int                 dumb;
};

#endif

/* Fooling lint. Unfortunately, c defines all the TIO constants to be long,
 * and lint expects them to be int. Also, ioctl is sometimes called with just
 * two arguments. The following definition keeps lint happy. It may need to
 * be reset for different systems. 
 */
#ifdef lint
#ifdef Pyramid
/* Pyramid makes constants greater than 65535 into long! Gakk! -CJS- */
/* ARGSUSED */
/* VARARGS2 */
static 
    Ioctl(i, l, p) long l;
    char               *p;
{
    return 0;
}

#else
/* ARGSUSED */
/* VARARGS2 */
static 
    Ioctl(i, l, p) char *p;
{
    return 0;
}

#endif
#define ioctl	    Ioctl
#endif

/*
 * Provides for a timeout on input. Does a non-blocking read, consuming the
 * data if any, and then returns 1 if data was read, zero otherwise. 
 *
 * Porting: 
 *
 * In systems without the select call, but with a sleep for fractional numbers
 * of seconds, one could sleep for the time and then check for input. 
 *
 * In systems which can only sleep for whole number of seconds, you might sleep
 * by writing a lot of nulls to the terminal, and waiting for them to drain,
 * or you might hack a static accumulation of times to wait. When the
 * accumulation reaches a certain point, sleep for a second. There would need
 * to be a way of resetting the count, with a call made for commands like run
 * or rest.
 */
int check_input(int microsec)
{
#if defined(USG) && !defined(M_XENIX)
    int result = 0;

    int                 arg;
#else
    struct timeval      tbuf;

#if defined(BSD4_3) || defined(M_XENIX) || defined(linux)
    fd_set              smask;
    fd_set		*no_fds = NULL;
#else
    int                 smask;
    int			*no_fds = NULL;
#endif

#endif


#if defined(USG) && !defined(M_XENIX)

    /*** SysV code (?) ***/

    /* XXX Hack -- mod 128, sleep one sec every 128 turns */
    if (microsec != 0 && (turn & 0x7F) == 0) (void)sleep(1);

    /* XXX Hack -- Can't check for input, but can do non-blocking read */
    arg = 0;
    arg = fcntl(0, F_GETFL, arg);
    arg |= O_NDELAY;
    (void)fcntl(0, F_SETFL, arg);

    result = getch();

    arg = 0;
    arg = fcntl(0, F_GETFL, arg);
    arg &= ~O_NDELAY;
    (void)fcntl(0, F_SETFL, arg);

    if (result == -1) return 0;

#else

    /*** Do a nice clean "select" ***/

    tbuf.tv_sec = 0;
    tbuf.tv_usec = microsec;

#if defined(BSD4_3) || defined(M_XENIX) || defined(linux)
    FD_ZERO(&smask);
    FD_SET(0, &smask);		/* standard input is bit 0 */
#else
    smask = 0x0001;		/* standard input is bit 0 */
#endif

    /* If we time out, no key ready */
    if (select(1, &smask, no_fds, no_fds, &tbuf) != 1) return (0);

    /* Get a key */
    retsult = getch();

    /* check for EOF errors here, select sometimes works even when EOF */
    if (result == -1) {
	    eof_flag++;
	    return 0;
	}
#endif

    return 1;
}

#if 0

/*
 * This is not used.  It will only work on some systems.
 * It should be compared against shell_out in io.c 
 */

/*
 * A command for the operating system. Standard library function 'system' is
 * unsafe, as it leaves various file descriptors open. This also is very
 * careful with signals and interrupts, and does rudimentary job control, and
 * puts the terminal back in a standard mode. 
 */
int system_cmd(cptr p)
{
    int                 pgrp, pid, i, mask;
    union wait          w;
    extern char        *getenv();

    mask = sigsetmask(~0);	   /* No interrupts. */
    restore_term();		   /* Terminal in original state. */

    /* Are we in the control terminal group? */
    if (ioctl(0, TIOCGPGRP, (char *)&pgrp) < 0 || pgrp != getpgrp(0)) {
	pgrp = (-1);
    }

    pid = fork();
    if (pid < 0) {
	(void)sigsetmask(mask);
    moriaterm();		   /* Terminal in moria mode. */
	return (-1);
    }
    if (pid == 0) {
	(void)sigsetmask(0);	   /* Interrupts on. */
    /* Transfer control terminal. */
	if (pgrp >= 0) {
	    i = getpid();
	    (void)ioctl(0, TIOCSPGRP, (char *)&i);
	    (void)setpgrp(i, i);
	}
	for (i = 2; i < 30; i++)
	    (void)close(i);	   /* Close all but standard in and out. */
	(void)dup2(1, 2);	   /* Make standard error as standard out. */
	if (p == 0 || *p == 0) {
	    p = getenv("SHELL");
	    if (p)
		execl(p, p, 0);
	    execl("/bin/sh", "sh", 0);
	}
	else {
	    execl("/bin/sh", "sh", "-c", p, 0);
	}

	/* Hack (?) */
	_exit(1);
    }

/* Wait for child termination. */
    for (;;) {
	i = wait3(&w, WUNTRACED, (struct rusage *) 0);
	if (i == pid) {
	    if (WIFSTOPPED(w)) {
	    /* Stop outselves, if child stops. */
		(void)kill(getpid(), SIGSTOP);
	    /* Restore the control terminal, and restart subprocess. */
		if (pgrp >= 0)
		    (void)ioctl(0, TIOCSPGRP, (char *)&pid);
		(void)killpg(pid, SIGCONT);
	    } else
		break;
	}
    }

    /* Get the control terminal back. */
    if (pgrp >= 0) {
	(void)ioctl(0, TIOCSPGRP, (char *)&pgrp);
    }

    (void)sigsetmask(mask);	   /* Interrupts on. */
    moriaterm();		   /* Terminal in moria mode. */
    return 0;
}

#endif

#endif


