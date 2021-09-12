
#ifndef LUAINTEROP_H
#define LUAINTEROP_H

/// @brief App specific stuff for talking with lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


//---------------- Public Types ----------------------//

/// Demo enum.
typedef enum
{
    ST_READY = 1,
    ST_IN_PROCESS = 2,
    ST_DONE = 3
} state_type_t;


/// Demo struct.
typedef struct
{
    int val;
    state_type_t state;
    char* text;
} my_data_t;


//---------------- Call lua functions from C --------------------------//

/// Dumb calculator. Lua function: calc(x, y)
/// @param L Lua environment.
/// @param x One number.
/// @param y Another number.
/// @param res The answer.
void interop_Calc(lua_State* L, double x, double y, double* res);

/// Process a digital input change. Lua function: hinput(pin, value)
/// @param L Lua environment.
/// @param pin The input.
/// @param value True/false.
void interop_Hinput(lua_State* L, unsigned int pin, bool value);

/// Process some data into a struct. Lua function: structinator(data)
/// @param L Lua environment.
/// @param din Send struct.
/// @param dout Rcv struct.
void interop_Structinator(lua_State* L, my_data_t* din, my_data_t* dout);

//---------------- Call C functions from lua --------------------------//

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void interop_Preload(lua_State* L);

#endif // LUAINTEROP_H
