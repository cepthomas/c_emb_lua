
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "logger.h"
#include "board.h"
#include "luautils.h"


//---------------- Private Declarations ------------------//


//---------------- Public Implementation -----------------//


//--------------------------------------------------------//
int lu_DumpStack(lua_State *L, const char* info, int line)
{
    LOG_DEBUG("Dump stack:%s (L:%p)(caller:%d)", info, L, line);

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

//--------------------------------------------------------//
void lu_LuaError(lua_State* L, int err, int line, const char* format, ...)
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
            snprintf(buff, CLI_BUFF_LEN-1, "   Unknown error %i (caller:%d)", err, line);
            break;
    }
    LOG_ERROR(buff);

    // Dump trace.
    luaL_traceback(L, L, NULL, 1);
    snprintf(buff, CLI_BUFF_LEN-1, "   %s | %s | %s", lua_tostring(L, -1), lua_tostring(L, -2), lua_tostring(L, -3));
    LOG_ERROR(buff);

    // lua_pushstring(L, buff1);
    // lua_pushstring(L, buff2);
    // lua_pushstring(L, buff3);
    lua_error(L); // never returns
}

//--------------------------------------------------------//
int lu_DumpGlobals(lua_State* L)
{
    // Get global table.
    lua_pushglobaltable(L);
    // Put a nil key on stack.
    lua_pushnil(L);
    // key(-1) is replaced by the next key(-1) in table(-2).
    while (lua_next(L, -2) != 0)
    {
        // Get key(-2) name.
        const char* name = lua_tostring(L, -2);
        // Get type of value(-1).
        const char* type = luaL_typename(L, -1);
        LOG_DEBUG("GLOBAL:%s=%s", name, type);
        // Remove value(-1), now key on top at(-1).
        lua_pop(L, 1);
    }
    // Remove global table(-1).
    lua_pop(L,1);

    return RS_PASS;
}


//---------------- Private Implementation -------------//
