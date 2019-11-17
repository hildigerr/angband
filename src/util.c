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

/*
 * for those systems that don't have usleep
 * grabbed from the inl netrek server -cba 
 * I think we include too many files above!
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


#if defined(SET_UID)

#include <pwd.h>

struct passwd      *getpwuid();
struct passwd      *getpwnam();


/*
 * Find a default user name from the system.
 */
void user_name(char *buf, int id)
{
    struct passwd *pwd;

    /* Look up the user name */
    pwd = getpwuid(id);
    (void)strcpy(buf, pwd->pw_name);

    if (*buf >= 'a' && *buf <= 'z')
	*buf = (*buf - 'a') + 'A';
}


/*
 * expands a tilde at the beginning of a file name to a users home directory
 */
static int parse_path(const char *file, char *exp)
{
    *exp = '\0';
    if (file) {
    if (*file == '~') {
	    char                user[128];
	    struct passwd      *pw = NULL;
	    int                 i = 0;

	    user[0] = '\0';
	    file++;
	    while (*file != PATH_SEP && i < sizeof(user))
		user[i++] = *file++;
	    user[i] = '\0';
	    if (i == 0) {
		char               *login = (char *)getlogin();

	    if (login != NULL) (void)strcpy(user, login);
	    else if ((pw = getpwuid(getuid())) == NULL) return 0;
	    }
	    if (pw == NULL && (pw = getpwnam(user)) == NULL) return 0;
	    (void)strcpy(exp, pw->pw_dir);
	}
    (void)strcat(exp, file);
    return 1;
    }
    return 0;
}


#else

/*
 * No default user name
 */
void user_name(char *buf, int id)
{
    /* No name */
    buf[0] = '\0';
}

/*
 * There is no expansion on single-user machines
 */
static int parse_path(const char *file, char *exp)
{
    /* Always fails */
    return (0);
}

#endif



/*
 * Replacement for "fopen" which parses leading tilde's
 */
FILE *my_tfopen(const char *file, const char *mode)
{
    char                buf[1024];
    extern int          errno;

    /* Try to parse the path */
    if (parse_path(file, buf))
    return (fopen(buf, mode));
    errno = ENOENT;
    return NULL;
}


/*
 * Replacement for "open" which parses leading tilde's
 */
int my_topen(const char *file, int flags, int mode)
{
    char                buf[1024];
    extern int          errno;

    /* Try to parse the path */
    if (parse_path(file, buf))
#ifdef MACINTOSH    
    /* Macintosh "open()" is brain-dead */
    return (open((char*)buf, flags, mode));
#else
    return (open(buf, flags, mode));
#endif
    errno = ENOENT;
    return -1;
}


