
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//

/// When the app was started.
static uint64_t p_start_time;


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
int common_Init(void)
{
    int stat = 0;

    p_start_time = board_GetCurrentUsec();

    return stat;
}

//--------------------------------------------------------//
unsigned int common_GetMsec()
{
    uint64_t now = board_GetCurrentUsec();
    unsigned int msec = (unsigned int)abs((int)((now - p_start_time) / 1000));
    return msec;
}

//--------------------------------------------------------//
bool common_Strtoi(const char* str, int* val)
{
    bool valid = true;
    char* p;

    errno = 0;
    *val = strtol(str, &p, 10);
    if(errno == ERANGE)
    {
        // Mag is too large.
        valid = false;
    }
    else if(p == str)
    {
        // Bad string.
        valid = false;
    }

    return valid;
}
