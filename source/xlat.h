
#ifndef XLAT_H
#define XLAT_H

#include "common.h"

/// @file Provides translation layer between lua and C.

/// Load the libs.
/// @param L Lua environment.
/// @return Status.
status_t xlat_loadLibs(lua_State* L);


#endif // XLAT_H
