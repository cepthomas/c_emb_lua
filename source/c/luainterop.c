
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"
#include "luainterop.h"


//---------------- Private Declarations ---------------------//

/// Dump the lua stack contents.
/// @param L Lua state.
int p_DumpStack(lua_State *L);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param format Standard string stuff.
int p_LuaError(lua_State* L, const char* format, ...);

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


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
void li_context(lua_State* L, const char* s, int i)
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
void li_calc(lua_State* L, int x, int y, double* res)
{
    int lstat = 0;

    ///// Get the function to be called.
    printf("!!!!! %p\r\n", L);
    lstat = lua_getglobal(L, "calc");
    printf("!!!!! 2\r\n");
    if(lstat >= LUA_ERRRUN)
    {
        printf("!!!!! 3\r\n");
        p_LuaError(L, "lua_getglobal calc() failed");
    }
    printf("!!!!! 5\r\n");

    ///// Push the arguments to the call.
    lua_pushinteger(L, x);
    printf("!!!!! 6\r\n");
    lua_pushinteger(L, y);
    printf("!!!!! 7\r\n");

    ///// Use lua_pcall to do the actual call. TODO or int lua_pcallk (); This function behaves exactly like lua_pcall, but allows the called function to yield.
    lstat = lua_pcall(L, 2, 1, 0);
    printf("!!!!! 8\r\n");
    if(lstat >= LUA_ERRRUN)
    {
        p_LuaError(L, "lua_pcall calc() failed");
    }

    ///// Pop the results from the stack.
    if(lua_isnumber(L, -1))
    {
        *res = lua_tonumber(L, -1);
    }
    else
    {
        p_LuaError(L, "Bad calc() return value");
    }

    lua_pop(L, 1);  // pop returned value
}

//--------------------------------------------------------//
void li_hinput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Get the function to be called.
    lstat = lua_getglobal(L, "hinput");
    if(lstat >= LUA_ERRRUN)
    {
        p_LuaError(L, "lua_getglobal hinput() failed");
    }

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if(lstat >= LUA_ERRRUN)
    {
        p_LuaError(L, "Call hinput() failed");
    }

    /////
    // no return value
}


//---------------- Private Implementation -------------//

//--------------------------------------------------------//
int p_GetArgInt(lua_State* L, int index, int* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        p_LuaError(L, "Invalid integer argument at index %d", index);
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
        p_LuaError(L, "Invalid double argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_GetArgBool(lua_State* L, int index, bool* ret)
{
    if(lua_isboolean(L, index) > 0)
    {
        *ret = lua_toboolean(L, index);
    }
    else
    {
        p_LuaError(L, "Invalid bool argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_GetArgStr(lua_State* L, int index, const char** ret)
{
    if(lua_isstring(L, index) > 0)
    {
        *ret = lua_tostring(L, index);
    }
    else
    {
        p_LuaError(L, "Invalid string argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int p_LuaError(lua_State* L, const char* format, ...)
{
    static char buff[CLI_BUFF_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, CLI_BUFF_LEN-1, format, args);

    // Dump stacktrace.
    luaL_traceback(L, L, NULL, 1);
    printf("%s\n", lua_tostring(L, -1));

    lua_pushstring(L, buff);
    lua_error(L); // never returns

    return RS_PASS;
}

//--------------------------------------------------------//
int p_DumpStack(lua_State *L)
{
    char buff[CLI_BUFF_LEN];

    for(int i = lua_gettop(L); i >= 1; i--)
    {
        int t = lua_type(L, i);

        switch(t)
        {
            case LUA_TSTRING:        snprintf(buff, CLI_BUFF_LEN, "ind:%d string:%s ", i, lua_tostring(L, i));   break;
            case LUA_TBOOLEAN:       snprintf(buff, CLI_BUFF_LEN, "ind:%d bool:%s ", i, lua_toboolean(L, i) ? "true" : "false");   break;
            case LUA_TNUMBER:        snprintf(buff, CLI_BUFF_LEN, "ind:%d number:%g ", i, lua_tonumber(L, i));   break;
            case LUA_TNIL:           snprintf(buff, CLI_BUFF_LEN, "ind:%d nil", i);   break;
            case LUA_TNONE:          snprintf(buff, CLI_BUFF_LEN, "ind:%d none", i);   break;
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TTHREAD:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA: snprintf(buff, CLI_BUFF_LEN, "ind:%d %s:%p ", i, lua_typename(L, t), lua_topointer(L, i));   break;
            default:                 snprintf(buff, CLI_BUFF_LEN, "ind:%d type:%d", i, t);   break;
        }

        board_CliWriteLine(buff);
    }

    return RS_PASS;
}

//--------------------------------------------------------//

int p_cliwr(lua_State* L)
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
//  local start = luainterop.msec()

int p_msec(lua_State* L)
{
    ///// Get function arguments.
    // none

    ///// Do the work.
    unsigned int msec = common_GetMsec();

    ///// Push return values.
    lua_pushinteger(L, msec);
    return 1; // number of results
}

//--------------------------------------------------------//
int p_digout(lua_State* L)
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
int p_digin(lua_State* L)
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
/// Map functions in the module.
static const luaL_Reg luainteroplib[] =
{
//  { lua func, c func }
    { "cliwr",  p_cliwr },      // say something to user
    { "msec",   p_msec },       // get current time
    { "digout", p_digout },     // write output pin
    { "digin",  p_digin },      // read input pin
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_OpenLuainterop (lua_State *L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, luainteroplib);

    return 1;
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib.
/// @param L Lua state.
void li_preload(lua_State* L)
{
    luaL_requiref(L, "luainterop", p_OpenLuainterop, 1);
}
