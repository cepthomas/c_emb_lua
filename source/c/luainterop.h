
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief Generic stuff for talking with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

//---------------- Call lua functions from C --------------------------//

/// Fille a table with a struct.
/// @param L Lua environment.
/// @param data.
void li_my_data(lua_State* L, my_data_t* data);

/// Dumb calculator.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
void li_calc(lua_State* L, int x, int y, double* res);

/// Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void li_hinput(lua_State* L, unsigned int pin, bool value);

//---------------- Call C functions from lua --------------------------//

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void li_preload(lua_State* L);

//---------------- Uilities --------------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
int li_DumpStack(lua_State *L);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param err Specific Lua error.
/// @param line Where
/// @param format Standard string stuff.
void li_LuaError(lua_State* L, int err, int line, const char* format, ...);


#endif // LUAINTEROP_H
