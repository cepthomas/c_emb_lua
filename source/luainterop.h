
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief Generic stuff for talking with lua. This could be in a lib.


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "common.h"


/// Dump the lua stack contents.
/// @param L Lua state.
void luainterop_DumpStack(lua_State *L);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param format Standard string stuff.
void luainterop_LuaError(lua_State* L, const char* format, ...);

/// Utility to get an int arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
void luainterop_GetArgInt(lua_State* L, int index, int* ret);

/// Utility to get a double arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
void luainterop_GetArgDbl(lua_State* L, int index, double* ret);

/// Utility to get a boolean arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
void luainterop_GetArgBool(lua_State* L, int index, bool* ret);

/// Utility to get a string arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
void luainterop_GetArgStr(lua_State* L, int index, const char** ret);


#endif // LUAINTEROP_H
