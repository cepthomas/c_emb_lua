
#include "ctolua.h"
#include "board.h"


//--------------------------------------------------------//
void ctolua_Context(lua_State* L, const char* s, int i)
{
    // Create a new empty table and pushes it onto the stack.
    lua_newtable(L);

    lua_pushstring(L, "script_string");
    lua_pushstring(L, s);
    lua_settable(L, -3);
    
    lua_pushstring(L, "script_int");
    lua_pushinteger(L, i);
    lua_settable(L, -3);

    lua_setglobal(L, "script_context");
}

//--------------------------------------------------------//
void ctolua_Calc(lua_State* L, int x, int y, double* res)
{
    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "calc");

    ///// Push the arguments to the call.
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if(lstat >= LUA_ERRRUN)
    {
        luainterop_LuaError(L, "lua_pcall calc() failed");
    }

    ///// Pop the results from the stack.
    if(lua_isnumber(L, -1))
    {
        *res = lua_tonumber(L, -1);
    }
    else
    {
        luainterop_LuaError(L, "Bad calc() return value");
    }

    lua_pop(L, 1);  // pop returned value
}

//--------------------------------------------------------//
void ctolua_HandleInput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "hinput");

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);

    if(lstat >= LUA_ERRRUN)
    {
        luainterop_LuaError(L, "Call hinput() failed");
    }

    /////
    // no return value
}

