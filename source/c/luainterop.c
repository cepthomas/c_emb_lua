
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "logger.h"
#include "board.h"
#include "luainterop.h"
#include "luautils.h"


//---------------- Private Declarations ---------------------//

/// Utility to get an int arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int p_GetArgInt(lua_State* L, int index, int* ret);

/// Utility to get a double arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int p_GetArgDbl(lua_State* L, int index, double* ret);

/// Utility to get a boolean arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int p_GetArgBool(lua_State* L, int index, bool* ret);

/// Utility to get a string arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int p_GetArgStr(lua_State* L, int index, const char** ret);

/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_OpenLuainterop (lua_State *L);


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
    lstat = lua_getglobal(L, "luainterop");
    lu_DumpStack(L, "lua_getglobal(luainterop)", __LINE__);
    if(lstat >= LUA_ERRRUN)
    {
        lu_LuaError(L, lstat, __LINE__, "lua_getglobal calc() failed");
    }

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Use lua_pcall to do the actual call. TODO or int lua_pcallk (); This function behaves exactly like lua_pcall, but allows the called function to yield.
    lstat = lua_pcall(L, 2, 1, 0);
    // lstat = lua_resume(L, L, 2);
    
    if(lstat >= LUA_ERRRUN)
    {
        lu_LuaError(L, lstat, __LINE__, "lua_pcall calc() failed");
    }

    ///// Get the results from the stack.
    p_GetArgDbl(L, -1, res);
    // lua_pop(L, 1);  // pop returned value
}

//--------------------------------------------------------//
void iop_Hinput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Get the function to be called.
    lstat = lua_getglobal(L, "hinput");
    if(lstat >= LUA_ERRRUN)
    {
        lu_LuaError(L, lstat, __LINE__, "lua_getglobal hinput() failed");
    }

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if(lstat >= LUA_ERRRUN)
    {
        lu_LuaError(L, lstat, __LINE__, "Call hinput() failed");
    }

    /////
    // no return value
}

//--------------------------------------------------------//
void iop_Preload(lua_State* L)
{
    //lu_DumpStack(L, "before luaL_requiref()", __LINE__);
    luaL_requiref(L, "luainterop", p_OpenLuainterop, 1);
    //lu_DumpStack(L, "after luaL_requiref()", __LINE__);
}


//---------------- Private Implementation -------------//

//--------------------------------------------------------//
int p_GetArgInt(lua_State* L, int index, int* ret) // TODO these? lua_Integer luaL_checkinteger (lua_State *L, int arg);  void luaL_checktype (lua_State *L, int arg, int t);
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        lu_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid integer argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_GetArgDbl(lua_State* L, int index, double* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        lu_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid double argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_GetArgBool(lua_State* L, int index, bool* ret)
{
    if(lua_isboolean(L, index) > 0)
    {
        *ret = lua_toboolean(L, index); // always t/f
    }
    else
    {
        lu_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid bool argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_GetArgStr(lua_State* L, int index, const char** ret)
{
    if(lua_isstring(L, index) > 0)
    {
        *ret = lua_tostring(L, index); //returns NULL if ng
    }
    else
    {
        lu_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid string argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_CliWr(lua_State* L)
{
    ///// Get function arguments.
    const char* info = NULL;
    p_GetArgStr(L, 1, &info);

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
    p_GetArgInt(L, 1, &pin);
    p_GetArgBool(L, 2, &state);

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
    p_GetArgInt(L, 1, &pin);

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
//  { lua func, c func }
    { "cliwr",  p_CliWr },      // say something to user
    { "msec",   p_Msec },       // get current time
    { "digout", p_DigOut },     // write output pin
    { "digin",  p_DigIn },      // read input pin
    { NULL, NULL }
};

//--------------------------------------------------------//
int p_OpenLuainterop (lua_State *L)
{
    // Register our C <-> Lua functions.
    //lu_DumpStack(L, "===p_OpenLuainterop()", __LINE__);
    luaL_newlib(L, luainteroplib);
    //lu_DumpStack(L, "===luaL_newlib()", __LINE__);
    //lua_setglobal(L, "luainterop");
    //lu_DumpStack(L, "===lua_setglobal()");


// Probably, you should add lua_setglobal(L, "mylib"); just after luaL_newlib() and call your function using its full name mylib.mysin(2)

// luaL_newlib creates a new table and populates it from a list of functions. Your function mysin is inside this table and is not a global function.
// If you want it to be a global function, use lua_register.


// void luaL_newlib (lua_State *L, const luaL_Reg l[]);
// Creates a new table and registers there the functions in list l.
// It is implemented as the following macro:
//      (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))


    return 1;
}
