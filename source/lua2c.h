
#ifndef CALLCFROMLUA_H
#define CALLCFROMLUA_H

#include "common.h"
#include "luainterop.h"

/// @file Interface to call C functions from lua.

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void lua2c_preload(lua_State* L);


#endif // CALLCFROMLUA_H
