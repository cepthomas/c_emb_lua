
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

//---------------- Public Types ----------------------//

/// Demo enum.
typedef enum
{
    READY = 1,      ///> Ready to be scheduled
    IN_PROCESS = 2, ///> Scheduled or running
    DONE = 3        ///> All done
} state_type_t;


/// Demo struct.
typedef struct
{
    double f1;          ///> A value
    int f2;             ///> Another value
    state_type_t state; ///> Another value
    const char* f3;     ///> Yet another
} my_data_t;


//---------------- Public Consts ----------------------//

/// Defines an error (allocation, iniitialization, etc) for a function that returns a pointer.
#define BAD_PTR NULL

/// Validate pointer arg. If fails, early returns err.
/// @param ptr Pointer.
/// @param err Error value to return in case of failure.
#define VAL_PTR(ptr, err) if(ptr == NULL) { return err; }

/// Defines success for a function that returns int status.
static const int RS_PASS = 0;

/// Defines an error (memory, invalid data, etc) for a function that returns int status.
static const int RS_ERR = -1;

/// Defines failure (expected, end of iteration, etc) for a function that returns int status.
static const int RS_FAIL = -2;


//---------------- Public Functions ----------------------//

/// Current time.
double common_CurrentSec();


#endif // COMMON_H
