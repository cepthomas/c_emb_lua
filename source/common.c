
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
  
    static char buff[LOG_LINE_LEN];

    sprintf(buff, "%s ", common_xlatLogLevel(level));
    va_list args;
    va_start(args, format);
    vsnprintf(buff + strlen(buff), LOG_LINE_LEN - 1, format, args);
    board_log(buff);

    return stat;
}

//--------------------------------------------------------//
unsigned int common_getMsec()
{
    uint64_t now = board_getCurrentUsec();
    unsigned int msec = abs((unsigned int)((now - p_startTime) / 1000));
    return msec;
}

//--------------------------------------------------------//
const char* common_xlatLogLevel(loglvl_t level)
{
    const char* ll = "???";
    switch(level)
    {
        case LOG_INFO:  ll = "INF"; break;
        case LOG_WARN:  ll = "WRN"; break;
        case LOG_ERROR: ll = "ERR"; break;
    }
    return ll;
}    

//--------------------------------------------------------//
const char* common_xlatStatus(status_t stat)
{
    const char* ss = "???";
    switch(stat)
    {
        case STATUS_OK:    ss = "STATUS_OK"; break;
        case STATUS_WARN:  ss = "STATUS_WARN"; break;
        case STATUS_ERROR: ss = "STATUS_ERROR"; break;
        case STATUS_FATAL: ss = "STATUS_FATAL"; break;
    }
    return ss;
}    

//--------------------------------------------------------//
bool common_strtoi(const char* str, int* val)
{
    bool valid = false;
    *val = atoi(str); // TODO not very safe...
    return valid;
}
