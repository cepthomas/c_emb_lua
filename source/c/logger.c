
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "logger.h"


//---------------- Private Declarations ------------------//

/// Logging support items.
log_level_t p_level = LVL_INFO;
FILE* p_fp = NULL;
double p_start_sec;

/// Current time.
double p_CurrentSec();

//---------------- Public API Implementation -------------//

//--------------------------------------------------------//
int logger_Init(const char* fn)
{
    p_fp = fopen(fn, "w"); //TODO or "a"
    VAL_PTR(p_fp, RS_ERR);
    p_start_sec = p_CurrentSec();

    // Banner.
    time_t now = time(NULL);
    char snow[32];
    strftime(snow, 32, "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(p_fp, "================ %s =====================\n", snow);

    return RS_PASS;
}

//--------------------------------------------------------//
int logger_SetFilters(log_level_t level)
{
    assert(p_fp != NULL);

    p_level = level;
    return RS_PASS;
}

//--------------------------------------------------------//
int logger_Log(log_level_t level, int line, const char* fn, const char* format, ...)
{
    assert(p_fp != NULL);

    VAL_PTR(p_fp, RS_ERR);
    VAL_PTR(format, RS_ERR);
    #define LOG_LINE_LEN 100
    static char buff[LOG_LINE_LEN];

    const char* pfn = strrchr(fn, '\\');
    if(pfn == NULL)
    {
        pfn = fn;
    }

    // The strrchr() function returns a pointer to the last occurrence of c in string. If the given character is not found, a NULL pointer is returned.

    // Check filters.
    if(level >= p_level)
    {
        va_list args;
        va_start(args, format);
        // The content.
        vsnprintf(buff, LOG_LINE_LEN-1, format, args);
        va_end(args);

        const char* slevel = "???";
        switch(level)
        {
            case LVL_DEBUG: slevel = "DBG"; break;
            case LVL_INFO:  slevel = "INF"; break;
            case LVL_ERROR: slevel = "ERR"; break;
        }

        fprintf(p_fp, "%03.6f,%s,%s(%d),%s\n", p_CurrentSec() - p_start_sec, slevel, pfn, line, buff);
        fflush(p_fp);
    }

    return RS_PASS;
}


//---------------- Private Implementation --------------------------//

double p_CurrentSec()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    double sec = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    return sec;
}