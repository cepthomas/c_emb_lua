
#ifndef COMMON_H
#define COMMON_H

/// @file General defs.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


/// General status value.
typedef enum
{
    STATUS_OK = 0,
    STATUS_WARN,
    STATUS_ERROR,
    STATUS_FATAL
} status_t;

typedef enum
{
    LOG_OFF = 0,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO
} loglvl_t;


/// Initialize the module.
/// @return Status.
status_t common_init(void);

/// Set the log level threshhold.
/// @param level See loglvl_t
/// @return Status.
status_t common_setLogLevel(loglvl_t level);

/// Maybe log some information. TODO could be fancier with macros to capture FILE/LINE etc.
/// @param level See common_setLogLevel().
/// @param format Format string followed by varargs.
status_t common_log(loglvl_t level, const char* format, ...);

/// Returns the number of milliseconds since the app was started.
/// @return The msec.
int common_getMsec(void);


///////////////// macros TODO in pp? //////////////////////

#define SPX if(stat == 0) stat =
#define SP if(stat == 0)
#define SF if(stas != 0)


#define CHECKED_FUNC(_stat, _func, ...) \
{ \
    _stat = _func(__VA_ARGS__); \
    if(_stat != STATUS_OK) \
    { \
        common_log(0, "%s(%d) %s", __FILE__, __LINE__, #_func); \
    } \
}

#endif // COMMON_H
