
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
status_t common_log(loglvl_t level, const char* format, ...)//TODOP
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
    unsigned int msec = (unsigned int)abs((int)((now - p_startTime) / 1000));
    return msec;
}

//--------------------------------------------------------//
const char* common_xlatLogLevel(loglvl_t level)
{
    static char buff[20];
    switch(level)
    {
        case LOG_INFO:  strcpy(buff, "INF"); break;
        case LOG_WARN:  strcpy(buff, "WRN"); break;
        case LOG_ERROR: strcpy(buff, "ERR"); break;
        default: snprintf(buff, 20, "%d", level); break;
    }
    return buff;
}    

//--------------------------------------------------------//
const char* common_xlatStatus(status_t stat)
{
    static char buff[20];
    switch(stat)
    {
        case STATUS_OK:    strcpy(buff, "STATUS_OK"); break;
        case STATUS_WARN:  strcpy(buff, "STATUS_WARN"); break;
        case STATUS_ERROR: strcpy(buff, "STATUS_ERROR"); break;
        case STATUS_FATAL: strcpy(buff, "STATUS_FATAL"); break;
        case STATUS_EXIT:  strcpy(buff, "STATUS_EXIT"); break;
        default: snprintf(buff, 20, "%d", stat); break;
    }
    return buff;
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
