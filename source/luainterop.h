
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief Generic stuff for talking with lua.


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

//#include "common.h"


// /// Dump the lua stack contents.
// /// @param L Lua state.
// int luainterop_DumpStack(lua_State *L);

// /// Report a bad thing detected by this component.
// /// @param L Lua state.
// /// @param format Standard string stuff.
// int luainterop_LuaError(lua_State* L, const char* format, ...);

// /// Utility to get an int arg off the Lua stack.
// /// @param L Lua state.
// /// @param index Index of the entry on the Lua stack.
// /// @param[out] ret The value.
// int luainterop_GetArgInt(lua_State* L, int index, int* ret);

// /// Utility to get a double arg off the Lua stack.
// /// @param L Lua state.
// /// @param index Index of the entry on the Lua stack.
// /// @param[out] ret The value.
// int luainterop_GetArgDbl(lua_State* L, int index, double* ret);

// /// Utility to get a boolean arg off the Lua stack.
// /// @param L Lua state.
// /// @param index Index of the entry on the Lua stack.
// /// @param[out] ret The value.
// int luainterop_GetArgBool(lua_State* L, int index, bool* ret);

// /// Utility to get a string arg off the Lua stack.
// /// @param L Lua state.
// /// @param index Index of the entry on the Lua stack.
// /// @param[out] ret The value.
// int luainterop_GetArgStr(lua_State* L, int index, const char** ret);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

/// @brief Interface to call lua functions from C.

/// Function to fill context table. Could be a generic version. TODO use for a struct.
/// @param L Lua environment.
/// @param s One element - a string.
/// @param i Another element - an int.
void ctolua_Context(lua_State* L, const char* s, int i);

/// Dumb calculator.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
void ctolua_Calc(lua_State* L, int x, int y, double* res);

/// Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void ctolua_HandleDigInput(lua_State* L, unsigned int pin, bool value);



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

/// @brief Interface to call C functions from lua.


/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void luainterop_Preload(lua_State* L);


#endif // LUAINTEROP_H
