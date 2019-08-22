
#include "stringx.h"
#include "c2lua.h"
#include "board.h"

// Interface to call lua functions from C.


//--------------------------------------------------------//
// Internal function to fill context table. TODO make a generic version?
void c2lua_loadContext(lua_State* L, const char* s, int i)
{
    //LOG(LOG_INFO, "c2lua_loadContext()");

    ///// Pass the context vals to the Lua world in a table named "script_context".
    lua_newtable(L); // Creates a new empty table and pushes it onto the stack.

    lua_pushstring(L, "script_string");
    lua_pushstring(L, s);
    lua_settable(L, -3);
    
    lua_pushstring(L, "script_int");
    lua_pushinteger(L, i);
    lua_settable(L, -3);

    lua_setglobal(L, "script_context"); // Pops a value from the stack and sets it as the new value of global name.
}

//--------------------------------------------------------//
void c2lua_someCalc(lua_State* L, int x, int y, double* res)
{
    //LOG(LOG_INFO, "c2lua_someCalc()");

    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "somecalc");

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if (lstat >= LUA_ERRRUN)
    {
        luainterop_luaError(L, "lua_pcall somecalc() failed");
    }

    ///// Pop the results from the stack.
    if(lua_isnumber(L, -1))
    {
        *res = lua_tonumber(L, -1);
    }
    else
    {
        luainterop_luaError(L, "Bad somecalc() return value");
    }

    lua_pop(L, 1);  // pop returned value
}

//--------------------------------------------------------//
void c2lua_handleInput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "hinput");

    ///// Push the arguments to the call.
    lua_pushnumber(L, pin);
    lua_pushnumber(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);

    if (lstat >= LUA_ERRRUN)
    {
        luainterop_luaError(L, "Call hinput() failed");
    }

    /////
    // no return value
}

