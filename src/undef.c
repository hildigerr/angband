/* undef.c: undefined routines */

#include "angband.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#include <sys/file.h>

extern short        log_index;

void 
init_files()
{
}

int 
_new_log()
{
    log_index = 0;
    return 1;
}
