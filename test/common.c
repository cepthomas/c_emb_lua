
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//

#define LOG_LINE_LEN 128

//---------------- Public Implementation -------------//

//--------------------------------------------------------//
int common_Log(log_level_t level, const char* format, ...)
{
    static char buff[LOG_LINE_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, LOG_LINE_LEN-1, format, args);
    va_end(args);

    const char* slevel = "???";
    switch(level)
    {
        case LVL_DEBUG: slevel = "DBG"; break;
        case LVL_INFO:  slevel = "INF"; break;
        case LVL_ERROR: slevel = "ERR"; break;
    }

    printf("%s,%s\n", slevel, buff);

    return 0;
}
