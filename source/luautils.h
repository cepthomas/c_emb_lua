
#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <stdbool.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


//---------------- Uilities --------------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
/// @param fn Caller location.
/// @param line Caller location.
/// @param info Extra info.
int luautils_DumpStack(lua_State *L, const char* fn, int line, const char* info);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param fn Caller location.
/// @param line Caller location.
/// @param err Specific Lua error.
/// @param format Standard string stuff.
void luautils_LuaError(lua_State* L, const char* fn, int err, int line, const char* format, ...);

/// Make a readable string.
/// @param status Specific Lua status.
/// @return the string.
const char* luautils_LuaStatusToString(int err);

/// Dump the table at the top.
/// @param L Lua state.
/// @param L name visual.
int luautils_DumpTable(lua_State* L, const char* name);

/// Dump the lua globals.
/// @param L Lua state.
int luautils_DumpGlobals(lua_State* L);

/// Check stack.
void luautils_EvalStack(lua_State* l, int expected);

/// Helper macro to dump stack.
#define DUMP_STACK(L, info)  luautils_DumpStack(L, __FILE__, __LINE__, info);

/// Helper macro to check then handle error..
#define PROCESS_LUA_ERROR(L, err, fmt, ...)  if(err >= LUA_ERRRUN) { luautils_LuaError(L, __FILE__, __LINE__, err, fmt, ##__VA_ARGS__); }

/// Helper macro to check/log stack size.
#define EVAL_STACK(L, expected)  { int num = lua_gettop(L); if (num != expected) { logger_Log(LVL_DEBUG, __FILE__, __LINE__, "Expected %d stack but is %d", expected, num); } }

#endif // LUAUTILS_H
