
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "logger.h"
#include "board.h"
#include "luainterop.h"
#include "luautils.h"


//---------------- Private Declarations ---------------------//

/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_OpenLuainterop (lua_State* L);


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
void iop_SetGlobalMyData(lua_State* L, my_data_t* data, const char* name)
{
    // Create a new empty table and pushes it onto the stack.
    lua_newtable(L);

    lua_pushstring(L, "f1");
    lua_pushnumber(L, data->f1);
    lua_settable(L, -3);
    
    lua_pushstring(L, "f2");
    lua_pushinteger(L, data->f2);
    lua_settable(L, -3);
    
    lua_pushstring(L, "state");
    lua_pushinteger(L, data->state);
    lua_settable(L, -3);
    
    lua_pushstring(L, "f3");
    lua_pushstring(L, data->f3);
    lua_settable(L, -3);

    lua_setglobal(L, name);
}

//--------------------------------------------------------//
void iop_Calc(lua_State* L, double x, double y, double* res)
{
    int lstat = 0;
    
    ///// Get the function to be called.
    int gtype = lua_getglobal(L, "calc");

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);

    PROCESS_LUA_ERROR(L, lstat, "lua_pcall calc() failed");

    ///// Get the results from the stack.
    lu_GetArgDbl(L, -1, res);
}

//--------------------------------------------------------//
void iop_Hinput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Get the function to be called.
    int gtype = lua_getglobal(L, "hinput");

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 0, 0);

    PROCESS_LUA_ERROR(L, lstat, "Call hinput() failed");

    /////
    // no return value
}

//--------------------------------------------------------//
void iop_Preload(lua_State* L)
{
    luaL_requiref(L, "luainterop", p_OpenLuainterop, 1);
}


//---------------- Private Implementation -------------//


//--------------------------------------------------------//
int p_CliWr(lua_State* L)
{
    ///// Get function arguments.
    const char* info = NULL;
    lu_GetArgStr(L, 1, &info);

    ///// Do the work.
   board_CliWriteLine(info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
int p_Msec(lua_State* L)
{
    ///// Get function arguments.
    // none

    ///// Do the work.
    unsigned int msec = board_GetCurrentUsec() / 1000;

    ///// Push return values.
    lua_pushinteger(L, msec);
    return 1; // number of results
}

//--------------------------------------------------------//
int p_DigOut(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    bool state;
    lu_GetArgInt(L, 1, &pin);
    lu_GetArgBool(L, 2, &state);

    ///// Do the work.
    board_WriteDig((unsigned int)pin, state);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
int p_DigIn(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    lu_GetArgInt(L, 1, &pin);

    ///// Do the work.
    bool state;
    board_ReadDig((unsigned int)pin, &state);

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}

//--------------------------------------------------------//
/// Map lua functions to C functions.
static const luaL_Reg luainteroplib[] =
{
    //lua func, c func
    { "cliwr",  p_CliWr },      // say something to user
    { "msec",   p_Msec },       // get current time
    { "digout", p_DigOut },     // write output pin
    { "digin",  p_DigIn },      // read input pin
    { NULL, NULL }
};

//--------------------------------------------------------//
int p_OpenLuainterop (lua_State* L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, luainteroplib);

    return 1;
}
