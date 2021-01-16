
#ifndef CTOLUA_H
#define CTOLUA_H

#include "common.h"
#include "luainterop.h"


/// @file Interface to call lua functions from C.

/// Function to fill context table. Could be a generic version.
/// @param L Lua environment.
/// @param s One element - a string.
/// @param i Another element - an int.
void ctolua_context(lua_State* L, const char* s, int i);

/// Dumb calculator.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
void ctolua_calc(lua_State* L, int x, int y, double* res);

/// Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void ctolua_handleInput(lua_State* L, unsigned int pin, bool value);


#endif // CTOLUA_H
