
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


/// Defines an error (allocation, iniitialization, etc) for a function that returns a pointer.
#define BAD_PTR NULL

/// Defines success for a function that returns int status.
static const int RS_PASS = 0;

/// Defines an error (memory, invalid data, etc) for a function that returns int status.
static const int RS_ERR = -1;

/// Defines failure (expected, end of iteration, etc) for a function that returns int status.
static const int RS_FAIL = -2;

/// Defines normal exit status.
static const int RS_EXIT = -3;

/// Validate pointer arg. If fails, early returns err.
/// @param ptr Pointer.
/// @param err Error value to return in case of failure.
#define VAL_PTR(ptr, err) if(ptr == NULL) { return err; }


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
