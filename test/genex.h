#ifndef GENEX_H
#define GENEX_H

// #include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
// #include "common.h"



//---------------- Public API ----------------------//

/// $desc%
typedef enum
{
    READY = 1,      ///> Ready to be scheduled
    IN_PROCESS = 2, ///> Scheduled or running
    DONE = 3        ///> All done
} state_type_t;


/// $desc%
typedef struct
{
    double f1;
    int f2;
} my_data_t;




//--------------------------------------------------------//
//-------------------luatoc.h-----------------------------//
//--------------------------------------------------------//

////////////////// @brief Interface to call C functions from lua.

// #include "common.h"
// #include "luainterop.h"

/// Setup to load the libs.
/// @param L Lua environment.
/// @return Status.
void genex_Preload(lua_State* L);


//--------------------------------------------------------//
//-------------------ctolua.h-----------------------------//
//--------------------------------------------------------//

/////////////// @brief Interface to call lua functions from C.

/// Function to fill context table. Could be a generic version.
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



#endif // GENEX_H
