/* undef.c: undefined routines */

#include "constant.h"
#include "config.h"
#include "types.h"

#if defined(SOLARIS)
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
