
#ifndef COMMON_H
#define COMMON_H

/// @file General defs.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>


/// General status value.
typedef enum
{
    STATUS_OK = 0,
    STATUS_WARN,
    STATUS_ERROR,
    STATUS_FATAL
} status_t;

/// Log levels.
typedef enum
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} loglvl_t;


/// Initialize the module.
/// @return Status.
status_t common_init(void);

/// Maybe log some information. This is crude, doesn't have timestamps, or file/line info. Oh well.
/// @param level See common_setLogLevel().
/// @param format Format string followed by varargs.
status_t common_log(loglvl_t level, const char* format, ...);

/// Returns the number of milliseconds since the app was started.
/// @return The msec.
unsigned int common_getMsec(void);

/// Format a readable string from the argument.
/// @param level Log level.
/// @return The string.
const char* common_xlatLogLevel(loglvl_t level);

/// Format a readable string from the argument.
/// @param stat System status.
/// @return The string.
const char* common_xlatStatus(status_t stat);

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool common_strtoi(const char* str, int* val);

#endif // COMMON_H
