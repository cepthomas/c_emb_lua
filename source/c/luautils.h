
#ifndef LUAUTILS_H
#define LUAUTILS_H

/// @brief Generic stuff for playing with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/// Add to existing lua errors. TODO wrong home.
#define IOP_INVALID_TYPE 100

//---------------- Uilities --------------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
/// @param info Extra info.
/// @param line Where
int lu_DumpStack(lua_State *L, const char* info, int line);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param err Specific Lua error.
/// @param line Where
/// @param format Standard string stuff.
void lu_LuaError(lua_State* L, int err, int line, const char* format, ...);

/// Dump the lua globals.
/// @param L Lua state.
int lu_DumpGlobals(lua_State* L);

#endif // LUAUTILS_H
