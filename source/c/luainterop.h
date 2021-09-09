
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief Generic stuff for talking with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

//---------------- Call lua functions from C --------------------------//

/// Fill a Lua table with a struct and add to global.
/// @param L Lua environment.
/// @param data my_data_t
/// @param name Table name.
void iop_SetGlobalMyData(lua_State* L, my_data_t* data, const char* name);

/// Dumb calculator. Lua function: calc(x, y)
/// @param L Lua environment.
/// @param x One number.
/// @param y Another number.
/// @param res The answer.
void iop_Calc(lua_State* L, double x, double y, double* res);

/// Process a digital input change. Lua function: hinput(pin, value)
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void iop_Hinput(lua_State* L, unsigned int pin, bool value);

//---------------- Call C functions from lua --------------------------//

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void iop_Preload(lua_State* L);

//---------------- Uilities TODO other file? --------------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
/// @param info Extra info.
int iop_DumpStack(lua_State *L, const char* info);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param err Specific Lua error.
/// @param line Where
/// @param format Standard string stuff.
void iop_LuaError(lua_State* L, int err, int line, const char* format, ...);


#endif // LUAINTEROP_H
