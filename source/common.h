
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
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} loglvl_t;


/// Initialize the module.
/// @return Status.
status_t common_init(void);

/// Maybe log some information. TODO could be fancier with macros to capture FILE/LINE etc.
/// @param level See common_setLogLevel().
/// @param format Format string followed by varargs.
status_t common_log(loglvl_t level, const char* format, ...);

/// Returns the number of milliseconds since the app was started.
/// @return The msec.
int common_getMsec(void);

#endif // COMMON_H
