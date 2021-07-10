
#ifndef COMMON_H
#define COMMON_H

/// @brief General defs.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <errno.h>
#include <math.h>


/// General status value.
typedef enum
{
    STATUS_OK = 0,
    STATUS_WARN,
    STATUS_ERROR,
    STATUS_FATAL,
    STATUS_EXIT
} status_t;


/// Initialize the module.
/// @return Status.
int common_Init(void);

/// Returns the number of milliseconds since the app was started.
/// @return The msec.
unsigned int common_GetMsec(void);

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool common_Strtoi(const char* str, int* val);

#endif // COMMON_H
