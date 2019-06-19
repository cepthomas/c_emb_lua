
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//

/// When the app was started.
static uint64_t p_startTime;


//---------------- Public Implementation -------------//


//--------------------------------------------------------//
status_t common_init(void)
{
    status_t stat = STATUS_OK;

    p_startTime = board_getCurrentUsec();

    return stat;
}

//--------------------------------------------------------//
status_t common_log(loglvl_t level, const char* format, ...)
{
    status_t stat = STATUS_OK;
  
    static char p_logBuff[100];

    switch(level)
    {
    case LOG_INFO:  strcpy(p_logBuff, "INF "); break;
    case LOG_WARN:  strcpy(p_logBuff, "WRN "); break;
    case LOG_ERROR: strcpy(p_logBuff, "ERR "); break;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(p_logBuff + 4, sizeof(p_logBuff), format, args);
    board_log(p_logBuff);

    return stat;
}

//--------------------------------------------------------//
int common_getMsec()
{
    uint64_t now = board_getCurrentUsec();
    int msec = abs((int)((now - p_startTime) / 1000));
    return msec;
}

//---------------- Private --------------------------//

