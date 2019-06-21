
#ifndef XLAT_H
#define XLAT_H

#include "common.h"

/// @file Provides a lua lib implemented in C.

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void demolib_preload(lua_State* L);

/// Create a table of context values.
/// @param L Lua environment.
/// @param s One arg.
/// @param i Another arg.
/// @return Result.
void demolib_loadContext(lua_State* L, const char* s, int i);

/// Call a lua function from C.
/// @param L Lua environment.
/// @param x One arg.
/// @param y Another arg.
/// @param res The answer.
/// @return Result.
void demolib_luafunc_someCalc(lua_State* L, int x, int y, float* res);

/// Call a lua function from C. Process a digital input change.
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
/// @return Result.
void demolib_handleInput(lua_State* L, unsigned int pin, bool value);


#endif // XLAT_H
