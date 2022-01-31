/* File: signals.c */

/* Purpose: signal handlers */

/*
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/*
 * This signal package was brought to you by		-JEW- 
 * Completely rewritten by				-CJS-
 */

/* Signals have no significance on the Mac */

#ifdef MAC

void 
signals_ignore_tstp()
{
}

void 
signals()
{
}

void 
signals_init()
{
}

#else				   /* a non-Mac system */

#include <stdio.h>

#ifdef linux
#define SIGBUS SIGUSR1
#endif


#ifdef ATARIST_MWC
/*
 * need these for atari st, but for unix, must include signals.h first, or
 * else suspend won't be properly declared 
 */
#include "angband.h"
#include "config.h"
#include "externs.h"
#endif

/* skip most of the file on an ATARI ST */
#ifndef ATARIST_MWC

/* to get the SYS_V def if needed */
#include "config.h"

#if defined(SYS_V) && defined(lint)
/*
 * for AIX, prevent hundreds of unnecessary lint errors, define before
 * signal.h is included 
 */
#define _h_IEEETRAP
typedef struct {
    int                 stuff;
}                   fpvmach;

#endif

/* must include before externs.h, because that uses SIGTSTP */
#include <signal.h>

#include "angband.h"
#include "externs.h"

#ifdef USG
void                exit();

#ifdef __TURBOC__
void                sleep();

#else
unsigned            sleep();

#endif
#endif

static int          error_sig = (-1);
static int          signal_count = 0;

/* ARGSUSED */
#ifndef USG
static void signal_handler(int sig, int code, struct sigcontext *scp)
{
    int                 smask;

    smask = sigsetmask(0) | (1 << sig);
#else
#ifdef __TURBOC__
static void 
signal_handler(sig)
#else
static void 
signal_handler(sig)
#endif
    int                 sig;
{

#endif
    if (error_sig >= 0) {	   /* Ignore all second signals. */
	if (++signal_count > 10)   /* Be safe. We will die if persistent
				    * enough. */
	    (void)signal(sig, SIG_DFL);
	return;
    }
    error_sig = sig;

/* Allow player to think twice. Wizard may force a core dump. */
    if (sig == SIGINT
#ifndef MSDOS
	|| sig == SIGQUIT
#endif
	) {
	if (death)
	    (void)signal(sig, SIG_IGN);	/* Can't quit after death. */
	else if (!character_saved && character_generated) {
	    if ((!total_winner) ? (!get_check("Really commit *Suicide*?"))
		: (!get_check("Do you want to retire?"))) {
		if (turn > 0)
		    disturb(1, 0);
		erase_line(0, 0);
		put_qio();
		error_sig = (-1);
#ifdef USG
#ifdef linux
		      (void) signal(sig, (void (*)()) signal_handler);
#else
		      (void) signal(sig, signal_handler);/* Have to restore handler.*/
#endif

#else
		(void)sigsetmask(smask);
#endif
	    /* in case control-c typed during msg_print */
		if (wait_for_more)
		    put_str(" -more-", MSG_LINE, 0);
		put_qio();

		/* OK. We don't quit. */
		return;
	    }
	    /* Death */
	    (void)strcpy(died_from, "Interrupting");
	}
	else {
	    (void)strcpy(died_from, "Abortion");
	}

	/* Interrupted */
	prt("Interrupt!", 0, 0);

	/* Suicide */
	death = TRUE;

	/* Save and exit */
	exit_game();
    }

    /* Die. */
    prt("OH NO!!!!!!  A gruesome software bug LEAPS out at you. There is NO defense!", 23, 0);
    if (!death && !character_saved && character_generated) {

	/* Try a panic save */
	panic_save = 1;
	prt("Your guardian angel is trying to save you.", 0, 0);

	/* Attempt to save */
	(void)sprintf(died_from, "(panic save %d)", sig);
	if (!save_player()) {

	/* Oops */
	(void)strcpy(died_from, "software bug");
	death = TRUE;
	    turn = (-1);
	}
    }
    else {
	death = TRUE;

	(void)_save_player(savefile);/* Quietly save the memory anyway. */
    }

    restore_term();

#ifndef MSDOS
    /* generate a core dump if necessary */
    (void)signal(sig, SIG_DFL);
    (void)kill(getpid(), sig);
    (void)sleep(5);
#endif

    /* Quit anyway */
    quit(NULL);
}

#endif				   /* ATARIST_MWC */

#ifdef ATARIST_MWC
static int          error_sig = (-1);

#endif

#ifndef USG
static int          mask;

#endif

/*
 * signals_ignore_tstp - Ignore SIGTSTP signals.
 */
void signals_ignore_tstp(void)
{
#if !defined(ATARIST_MWC)
#ifdef SIGTSTP
#ifdef linux
  (void) signal(SIGTSTP, (void (*)()) suspend);
#else
    (void)signal(SIGTSTP, SIG_IGN);
#endif
#ifndef USG
    mask = sigsetmask(0);
#endif
#endif
    if (error_sig < 0)
	error_sig = 0;
#endif
}

void signals(void)
{
#if !defined(ATARIST_MWC)
#ifdef SIGTSTP
#ifdef __MINT__
      (void)signal(SIGTSTP, (__Sigfunc)suspend);
#else
    (void)signal(SIGTSTP, suspend);
#endif
#ifndef USG
    (void)sigsetmask(mask);
#endif
#endif
    if (error_sig == 0)
	error_sig = (-1);
#endif
}

/*
 * Prepare to handle the relevant signals
 */
void signals_init()
{
#ifndef ATARIST_MWC
#ifdef linux
  (void)signal(SIGINT, (void (*)()) signal_handler);
  (void)signal(SIGFPE, (void (*)()) signal_handler);
#else
    (void)signal(SIGINT, signal_handler);
    (void)signal(SIGFPE, signal_handler);
#endif
#ifdef MSDOS
/* many fewer signals under MSDOS */
#else
/* Ignore HANGUP, and let the EOF code take care of this case. */
    (void)signal(SIGHUP, SIG_IGN);
#ifdef linux
  (void) signal(SIGQUIT, (void (*)()) signal_handler);
  (void) signal(SIGILL, (void (*)()) signal_handler);
  (void) signal(SIGTRAP, (void (*)()) signal_handler);
  (void) signal(SIGIOT, (void (*)()) signal_handler);
#else
    (void)signal(SIGQUIT, signal_handler);
    (void)signal(SIGILL, signal_handler);
    (void)signal(SIGTRAP, signal_handler);
    (void)signal(SIGIOT, signal_handler);
#endif
#ifdef SIGEMT					   /* in BSD systems */
#ifdef linux
  (void) signal(SIGEMT, (void (*)()) signal_handler);
#else
    (void)signal(SIGEMT, signal_handler);
#endif
#endif
#ifdef SIGDANGER				   /* in SYSV systems */
#ifdef linux
  (void) signal(SIGDANGER, (void (*)()) signal_handler);
#else
    (void)signal(SIGDANGER, signal_handler);
#endif
#endif
#ifdef linux
  (void) signal(SIGKILL, (void (*)()) signal_handler);
  (void) signal(SIGBUS, (void (*)()) signal_handler);
  (void) signal(SIGSEGV, (void (*)()) signal_handler);
#ifdef SIGSYS
  (void) signal(SIGSYS, (void (*)()) signal_handler);
#endif
  (void) signal(SIGTERM, (void (*)()) signal_handler);
  (void) signal(SIGPIPE, (void (*)()) signal_handler);
#else
    (void)signal(SIGKILL, signal_handler);
    (void)signal(SIGBUS, signal_handler);
    (void)signal(SIGSEGV, signal_handler);
/*    (void)signal(SIGSYS, signal_handler);    not in linux pgb */
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGPIPE, signal_handler);
#endif
#ifdef SIGXCPU   /* BSD */
#ifdef linux
  (void) signal(SIGXCPU, (void (*)()) signal_handler);
#else
    (void)signal(SIGXCPU, signal_handler);
#endif
#endif
#ifdef SIGPWR   /* SYSV */
#ifdef linux
  (void) signal(SIGPWR, (void (*)()) signal_handler);
#else
    (void)signal(SIGPWR, signal_handler);
#endif
#endif
#endif
#endif
}
 


void ignore_signals()
{
#if !defined(ATARIST_MWC)
    (void)signal(SIGINT, SIG_IGN);
#ifdef SIGQUIT
    (void)signal(SIGQUIT, SIG_IGN);
#endif
#endif
}

void default_signals()
{
#if !defined(ATARIST_MWC)
    (void)signal(SIGINT, SIG_DFL);
#ifdef SIGQUIT
    (void)signal(SIGQUIT, SIG_DFL);
#endif
#endif
}

void restore_signals()
{
#if !defined(ATARIST_MWC)
#ifndef linux
    (void)signal(SIGINT, signal_handler);
#endif /* linux */

#ifdef SIGQUIT
#ifdef linux
	(void) signal(SIGQUIT, (void (*)()) signal_handler);
#else
    (void)signal(SIGQUIT, signal_handler);
#endif
#endif
#endif
}

#endif				   /* big Mac conditional */
