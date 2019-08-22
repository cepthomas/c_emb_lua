
#ifndef CALLLUAFROMC_H
#define CALLLUAFROMC_H

#include "common.h"
#include "luainterop.h"


/// @file Interface to call lua functions from C.

/// Create a table of context values available to the script. Optional.
/// @param L Lua environment.
/// @param s One arg.
/// @param i Another arg.
/// @return Result.
void c2lua_loadContext(lua_State* L, const char* s, int i);

/// Call a lua function from C. Demo calculation.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
/// @return Result.
void c2lua_calc(lua_State* L, int x, int y, double* res);

/// Call a lua function from C. Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
/// @return Result.
void c2lua_handleInput(lua_State* L, unsigned int pin, bool value);


#endif // CALLLUAFROMC_H
