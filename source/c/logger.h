#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include "common.h"


/// @brief Declaration of a logger module.

//---------------- Public API ----------------------//

/// Log levels.
typedef enum
{
    LVL_DEBUG = 0,
    LVL_INFO  = 1,
    LVL_ERROR = 2
} log_level_t;

/// Initialize the module.
/// @param fn File to write to.
/// @return Status.
int logger_Init(const char* fn);

/// Set log levels/cats. the module.
/// @param level
/// @return Status.
int logger_SetFilters(log_level_t level);

/// log some information.
/// @param level See log_level_t.
/// @param line Source line number.
/// @param fn Source file.
/// @param format Format string followed by varargs.
/// @return Status.
int logger_Log(log_level_t level, int line, const char* fn, const char* format, ...);


/// Helper macros.
#define LOG_ERROR(fmt, ...)  logger_Log(LVL_ERROR, __LINE__, __FILE__, fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt, ...)   logger_Log(LVL_INFO,  __LINE__, __FILE__, fmt, ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...)  logger_Log(LVL_DEBUG, __LINE__, __FILE__, fmt, ##__VA_ARGS__);

#endif // LOGGER_H
