/* undef.c: undefined routines */

#include "angband.h"


extern short        log_index;

int 
_new_log()
{
    log_index = 0;
    return 1;
}
