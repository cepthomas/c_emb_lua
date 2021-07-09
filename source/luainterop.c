
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"
#include "luainterop.h"


//---------------- Private --------------------------//


//---------------- Public Implementation -------------//


//--------------------------------------------------------//
void luainterop_getArgInt(lua_State* L, int index, int* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        luainterop_luaError(L, "Invalid integer argument at index %d", index);
    }
}

//--------------------------------------------------------//
void luainterop_getArgDbl(lua_State* L, int index, double* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        luainterop_luaError(L, "Invalid double argument at index %d", index);
    }
}

//--------------------------------------------------------//
void luainterop_getArgBool(lua_State* L, int index, bool* ret)
{
    if(lua_isboolean(L, index) > 0)
    {
        *ret = lua_toboolean(L, index);
    }
    else
    {
        luainterop_luaError(L, "Invalid bool argument at index %d", index);
    }
}

//--------------------------------------------------------//
void luainterop_getArgStr(lua_State* L, int index, const char** ret)
{
    if(lua_isstring(L, index) > 0)
    {
        *ret = lua_tostring(L, index);
    }
    else
    {
        luainterop_luaError(L, "Invalid string argument at index %d", index);
    }
}

//--------------------------------------------------------//
void luainterop_luaError(lua_State* L, const char* format, ...)
{
    static char p_buff[100];

    va_list args;
    va_start(args, format);
    vsnprintf(p_buff, sizeof(p_buff), format, args);

    // Dump stacktrace.
    luaL_traceback(L, L, NULL, 1);
    printf("%s\n", lua_tostring(L, -1));

    lua_pushstring(L, p_buff);
    lua_error(L); // never returns
}

//--------------------------------------------------------//
const char* luainterop_xlatLuaStatus(int lstat)
{
    static char buff[20];
    switch(lstat)
    {
        case LUA_OK:        strcpy(buff, "LUA_OK"); break;
        case LUA_YIELD:     strcpy(buff, "LUA_YIELD"); break;
        case LUA_ERRRUN:    strcpy(buff, "LUA_ERRRUN"); break;
        case LUA_ERRSYNTAX: strcpy(buff, "LUA_ERRSYNTAX"); break;
        case LUA_ERRMEM:    strcpy(buff, "LUA_ERRMEM"); break;
        case LUA_ERRGCMM:   strcpy(buff, "LUA_ERRGCMM"); break;
        case LUA_ERRERR:    strcpy(buff, "LUA_ERRERR"); break;
        default: snprintf(buff, 20, "%d", lstat); break;
    }
    return buff;
}

//--------------------------------------------------------//
void luainterop_dumpStack(lua_State *L)
{
    #define LINE_LEN 100
    char buff[LINE_LEN];

    for(int i = lua_gettop(L); i >= 1; i--)
    {
        int t = lua_type(L, i);

        switch(t)
        {
            case LUA_TSTRING:        snprintf(buff, LINE_LEN, "ind:%d string:%s ", i, lua_tostring(L, i));   break;
            case LUA_TBOOLEAN:       snprintf(buff, LINE_LEN, "ind:%d bool:%s ", i, lua_toboolean(L, i) ? "true" : "false");   break;
            case LUA_TNUMBER:        snprintf(buff, LINE_LEN, "ind:%d number:%g ", i, lua_tonumber(L, i));   break;
            case LUA_TNIL:           snprintf(buff, LINE_LEN, "ind:%d nil", i);   break;
            case LUA_TNONE:          snprintf(buff, LINE_LEN, "ind:%d none", i);   break;
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TTHREAD:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA: snprintf(buff, LINE_LEN, "ind:%d %s:%p ", i, lua_typename(L, t), lua_topointer(L, i));   break;
            default:                 snprintf(buff, LINE_LEN, "ind:%d type:%d", i, t);   break;
        }

        board_serWriteLine(buff);
    }
}
