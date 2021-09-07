
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief Generic stuff for talking with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

//---------------- Call lua functions from C --------------------------//

// TODO slight variation from std - name functions like their lua counterparts.

/// Function to fill context table. Could be a generic version. TODO use for a struct.
/// @param L Lua environment.
/// @param s One element - a string.
/// @param i Another element - an int.
void luainterop_context(lua_State* L, const char* s, int i);

/// Dumb calculator.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
void luainterop_calc(lua_State* L, int x, int y, double* res);

/// Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void luainterop_hinput(lua_State* L, unsigned int pin, bool value);


//---------------- Call C functions from lua --------------------------//

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void luainterop_preload(lua_State* L);


#endif // LUAINTEROP_H
