
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "logger.h"
#include "board.h"
#include "interop.h"
#include "luautils.h"


//---------------- Private Declarations ---------------------//


//---------------- Public Implementation -----------------//
//---------------- Call lua functions from C -------------//

//--------------------------------------------------------//
void interop_Calc(lua_State* L, double x, double y, double* res)
{
    int lstat = LUA_OK;
    
    ///// Get the function to be called.
    int gtype = lua_getglobal(L, "calc");

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);

    PROCESS_LUA_ERROR(L, lstat, "lua_pcall calc() failed");

    ///// Get the results from the stack.
    luautils_GetArgDbl(L, -1, res);
}

//--------------------------------------------------------//
void interop_Hinput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = LUA_OK;

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
void interop_Structinator(lua_State* L, my_data_t* din, my_data_t* dout)
{
    int lstat = LUA_OK;

    ///// Get the function to be called.
    int gtype = lua_getglobal(L, "structinator");

    ///// Package the input.
    // Create a new empty table and push it onto the stack.
    lua_newtable(L);

    lua_pushstring(L, "val");
    lua_pushinteger(L, din->val);
    lua_settable(L, -3);

    lua_pushstring(L, "state");
    lua_pushinteger(L, din->state);
    lua_settable(L, -3);

    lua_pushstring(L, "text");
    lua_pushstring(L, din->text);
    lua_settable(L, -3);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 1, 1, 0);

    PROCESS_LUA_ERROR(L, lstat, "lua_pcall structinator() failed");

    ///// Get the results from the stack.
    if(lua_istable(L, -1) > 0)
    {
        gtype = lua_getfield(L, -1, "val");
        lstat = luautils_GetArgInt(L, -1, &dout->val);
        lua_pop(L, 1); // remove field

        gtype = lua_getfield(L, -1, "state"); // LUA_TNUMBER
        lstat = luautils_GetArgInt(L, -1, (int*)&dout->state);
        lua_pop(L, 1); // remove field

        gtype = lua_getfield(L, -1, "text");
        lstat = luautils_GetArgStr(L, -1, &dout->text);
        lua_pop(L, 1); // remove field
    }
    else
    {
        int index = -1;
        PROCESS_LUA_ERROR(L, LUA_ERRRUN, "Invalid table argument at index %d", index);
    }

    // Remove the table.
    lua_pop(L, 1);
}

//---------------- Private Implementation ----------------//
//---------------- Call C functions from Lua -------------//

//--------------------------------------------------------//
static int p_CliWr(lua_State* L)
{
    ///// Get function arguments.
    char* info = NULL;
    luautils_GetArgStr(L, 1, &info);

    ///// Do the work.
   board_CliWriteLine(info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
static int p_Msec(lua_State* L)
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
static int p_DigOut(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    bool state;
    luautils_GetArgInt(L, 1, &pin);
    luautils_GetArgBool(L, 2, &state);

    ///// Do the work.
    board_WriteDig((unsigned int)pin, state);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
static int p_DigIn(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    luautils_GetArgInt(L, 1, &pin);

    ///// Do the work.
    bool state;
    board_ReadDig((unsigned int)pin, &state);

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}


//---------------- Implementation ------------------------//
//------------------ Infrastructure ----------------------//

//--------------------------------------------------------//
/// Map lua functions to C functions.
static const luaL_Reg function_map[] =
{
    //lua func, C func
    { "cliwr",  p_CliWr },      // say something to user
    { "msec",   p_Msec },       // get current time
    { "digout", p_DigOut },     // write output pin
    { "digin",  p_DigIn },      // read input pin
    { NULL,     NULL }
};

//--------------------------------------------------------//
// Callback from system to actually load the lib.
int p_OpenInterop (lua_State* L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, function_map);

    return 1;
}

//--------------------------------------------------------//
void interop_Load(lua_State* L)
{
    // Load app stuff. This table gets pushed on the stack and into globals.
    luaL_requiref(L, "luainterop", p_OpenInterop, 1);
}
