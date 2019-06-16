
#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

/// @file General defs. Not really module specific.

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


///////////////// TODO temp hackery //////////////////////

typedef char* QString;
typedef void* QStringList;


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

#endif // COMMON_DEFS_H
