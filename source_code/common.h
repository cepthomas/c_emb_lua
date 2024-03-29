
#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>


/// Simple demo logger to stdout.
int common_Log(char level, const char* format, ...);

/// Helper macro to check then handle error. If error, never returns.
#define PROCESS_LUA_ERROR(L, err, fmt, ...)  if(err >= LUA_ERRRUN) { lua_error(L); }

#endif // COMMON_H
