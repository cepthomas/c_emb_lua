
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//

/// When the app was started.
static uint64_t p_startTime;

/// Fixed size for logging.
#define LOG_LINE_LEN 100


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
int common_init(void)
{
    int stat = 0;

    p_startTime = board_getCurrentUsec();

    return stat;
}

//--------------------------------------------------------//
unsigned int common_getMsec()
{
    uint64_t now = board_getCurrentUsec();
    unsigned int msec = (unsigned int)abs((int)((now - p_startTime) / 1000));
    return msec;
}

//--------------------------------------------------------//
bool common_strtoi(const char* str, int* val)
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
