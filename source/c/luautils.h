
#ifndef LUAUTILS_H
#define LUAUTILS_H

/// @brief Generic stuff for playing with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


//---------------- Uilities --------------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
/// @param fn Caller location.
/// @param line Caller location.
/// @param info Extra info.
int lu_DumpStack(lua_State *L, const char* fn, int line, const char* info);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param fn Caller location.
/// @param line Caller location.
/// @param err Specific Lua error.
/// @param format Standard string stuff.
void lu_LuaError(lua_State* L, const char* fn, int err, int line, const char* format, ...);

/// Dump the table at the top.
/// @param L Lua state.
/// @param L name visual.
int lu_DumpTable(lua_State* L, const char* name);

/// Dump the lua globals.
/// @param L Lua state.
int lu_DumpGlobals(lua_State* L);

/// Helper macro.
#define DUMP_STACK(l, info)  lu_DumpStack(l, __FILE__, __LINE__, info);

/// Helper macro.
#define PROCESS_LUA_ERROR(l, err, fmt, ...)  lu_LuaError(l, __FILE__, __LINE__, err, fmt, ##__VA_ARGS__);

#endif // LUAUTILS_H
