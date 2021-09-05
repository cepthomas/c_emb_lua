
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"
#include "luainterop.h"


// TODO add a string describing error.


//---------------- Private --------------------------//


//---------------- Public Implementation -------------//


//--------------------------------------------------------//
int luainterop_GetArgInt(lua_State* L, int index, int* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        luainterop_LuaError(L, "Invalid integer argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luainterop_GetArgDbl(lua_State* L, int index, double* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        luainterop_LuaError(L, "Invalid double argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luainterop_GetArgBool(lua_State* L, int index, bool* ret)
{
    if(lua_isboolean(L, index) > 0)
    {
        *ret = lua_toboolean(L, index);
    }
    else
    {
        luainterop_LuaError(L, "Invalid bool argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luainterop_GetArgStr(lua_State* L, int index, const char** ret)
{
    if(lua_isstring(L, index) > 0)
    {
        *ret = lua_tostring(L, index);
    }
    else
    {
        luainterop_LuaError(L, "Invalid string argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luainterop_LuaError(lua_State* L, const char* format, ...)
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
int luainterop_DumpStack(lua_State *L)
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
