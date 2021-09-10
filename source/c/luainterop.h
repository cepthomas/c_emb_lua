
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief App specific stuff for talking with lua.

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

#endif // LUAINTEROP_H
