
#ifndef COMMON_MODULE_H
#define COMMON_MODULE_H

/// @file Interface to the common module.


#include "common_defs.h"


//---------------- Public API ----------------------//


//---------------- Definitions ---------------------//

typedef enum
{
    LOG_OFF = 0,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO
} loglvl_t;



//---------------- Functions ---------------------//

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

/// Clean up module resources.
/// @return Status.
status_t common_destroy(void);

/// Returns the number of milliseconds since the app was started.
/// @return The msec.
int common_getMsec(void);

#endif // COMMON_MODULE_H
