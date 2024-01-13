
#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <errno.h>
#include <math.h>


//---------------- Public Types ----------------------//


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

/// Safe convert a string to double.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool common_StrToDouble(const char* str, double* val);

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool common_StrToInt(const char* str, int* val);


#endif // COMMON_H
