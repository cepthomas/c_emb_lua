
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "logger.h"
#include "board.h"
#include "luainterop.h"


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

/// Add to existing lua errors.
#define IOP_INVALID_TYPE 100

LUALIB_API void luaL_tracebackEx(lua_State *L, lua_State *L1, const char *msg, int level);


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
    lstat = lua_getglobal(L, "calc");
    if(lstat >= LUA_ERRRUN)
    {
        iop_LuaError(L, lstat, __LINE__, "lua_getglobal calc() failed");
    }

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Use lua_pcall to do the actual call. TODO or int lua_pcallk (); This function behaves exactly like lua_pcall, but allows the called function to yield.
    lstat = lua_pcall(L, 2, 1, 0);
    // lstat = lua_resume(L, L, 2);
    
    if(lstat >= LUA_ERRRUN)
    {
        iop_LuaError(L, lstat, __LINE__, "lua_pcall calc() failed");
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
        iop_LuaError(L, lstat, __LINE__, "lua_getglobal hinput() failed");
    }

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if(lstat >= LUA_ERRRUN)
    {
        iop_LuaError(L, lstat, __LINE__, "Call hinput() failed");
    }

    /////
    // no return value
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib containing interop stuff.
/// @param L Lua state.
void iop_Preload(lua_State* L)
{
    luaL_requiref(L, "luainterop", p_OpenLuainterop, 1);
}

//--------------------------------------------------------//
void iop_LuaError(lua_State* L, int err, int line, const char* format, ...)
{
    static char buff[CLI_BUFF_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, CLI_BUFF_LEN-1, format, args);
    LOG_ERROR(buff);

    switch( err )
    {
        case LUA_ERRFILE:
            snprintf(buff, CLI_BUFF_LEN-1, "   LUA_ERRFILE: couldn't open the given file (caller:%d)", line);
            break;
        case LUA_ERRSYNTAX:
            snprintf(buff, CLI_BUFF_LEN-1, "   LUA_ERRSYNTAX: syntax error during pre-compilation (caller:%d)", line);
            break;
        case LUA_ERRMEM:
            snprintf(buff, CLI_BUFF_LEN-1, "   LUA_ERRMEM: memory allocation error (caller:%d)", line);
            break;
        case LUA_ERRRUN:
            snprintf(buff, CLI_BUFF_LEN-1, "   LUA_ERRRUN: (caller:%d)", line);
            break;
        case LUA_ERRERR:
            snprintf(buff, CLI_BUFF_LEN-1, "   LUA_ERRERR: error while running the error handler function (caller:%d)", line);
            break;
        case IOP_INVALID_TYPE:
            snprintf(buff, CLI_BUFF_LEN-1, "   IOP_INVALID_TYPE: (caller:%d)", line);
            break;
        default:
            snprintf(buff, CLI_BUFF_LEN-1, "unknown error %i (caller:%d)", err, line);
            break;
    }
    LOG_ERROR(buff);

    // Dump trace.
    luaL_tracebackEx(L, L, NULL, 1);
    snprintf(buff, CLI_BUFF_LEN-1, "   %s | %s | %s", lua_tostring(L, -1), lua_tostring(L, -2), lua_tostring(L, -3));
    LOG_ERROR(buff);

    // lua_pushstring(L, buff1);
    // lua_pushstring(L, buff2);
    // lua_pushstring(L, buff3);
    lua_error(L); // never returns
}

//--------------------------------------------------------//
int iop_DumpStack(lua_State *L, const char* info)
{
    LOG_DEBUG("Dump stack:%s", info);

    for(int i = lua_gettop(L); i >= 1; i--)
    {
        int t = lua_type(L, i);

        switch(t)
        {
            case LUA_TSTRING:        LOG_DEBUG("   index:%d string:%s ", i, lua_tostring(L, i));   break;
            case LUA_TBOOLEAN:       LOG_DEBUG("   index:%d bool:%s ", i, lua_toboolean(L, i) ? "true" : "false");   break;
            case LUA_TNUMBER:        LOG_DEBUG("   index:%d number:%g ", i, lua_tonumber(L, i));   break;
            case LUA_TNIL:           LOG_DEBUG("   index:%d nil", i);   break;
            case LUA_TNONE:          LOG_DEBUG("   index:%d none", i);   break;
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TTHREAD:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA: LOG_DEBUG("   index:%d %s:%p ", i, lua_typename(L, t), lua_topointer(L, i));   break;
            default:                 LOG_DEBUG("   index:%d type:%d", i, t);   break;
        }
    }

    return RS_PASS;
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
        iop_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid integer argument at index %d", index);
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
        iop_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid double argument at index %d", index);
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
        iop_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid bool argument at index %d", index);
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
        iop_LuaError(L, IOP_INVALID_TYPE, __LINE__, "Invalid string argument at index %d", index);
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
    luaL_newlib(L, luainteroplib);
    return 1;
}
