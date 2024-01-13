
#include <stdarg.h>
#include <string.h>

#include "logger.h"
#include "luautils.h"


//---------------- Private Declarations ------------------//

#define BUFF_LEN 100

//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
int luautils_DumpStack(lua_State* L, const char* fn, int line, const char* info)
{
    static char buff[BUFF_LEN];

    logger_Log(LVL_DEBUG, fn, line, "Dump stack:%s (L:%p)", info, L);

    for(int i = lua_gettop(L); i >= 1; i--)
    {
        int t = lua_type(L, i);

        switch(t)
        {
            case LUA_TSTRING:
                snprintf(buff, BUFF_LEN-1, "index:%d string:%s ", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                snprintf(buff, BUFF_LEN-1, "index:%d bool:%s ", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                snprintf(buff, BUFF_LEN-1, "index:%d number:%g ", i, lua_tonumber(L, i));
                break;
            case LUA_TNIL:
                snprintf(buff, BUFF_LEN-1, "index:%d nil", i);
                break;
            case LUA_TNONE:
                snprintf(buff, BUFF_LEN-1, "index:%d none", i);
                break;
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TTHREAD:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
                snprintf(buff, BUFF_LEN-1, "index:%d %s:%p ", i, lua_typename(L, t), lua_topointer(L, i));
                break;
            default:
                snprintf(buff, BUFF_LEN-1, "index:%d type:%d", i, t);
                break;
        }
    
        logger_Log(LVL_DEBUG, fn, line, "   %s", buff);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
void luautils_LuaError(lua_State* L, const char* fn, int line, int err, const char* format, ...)
{
    static char buff[BUFF_LEN];

    va_list args;
    va_start(args, format);
    logger_Log(LVL_DEBUG, fn, line, format, args);
    va_end(args);

    logger_Log(LVL_DEBUG, fn, line, "   %s", luautils_LuaStatusToString(err));

    // Dump trace.
    luaL_traceback(L, L, NULL, 1);
    snprintf(buff, BUFF_LEN-1, "%s | %s | %s", lua_tostring(L, -1), lua_tostring(L, -2), lua_tostring(L, -3));
    logger_Log(LVL_DEBUG, fn, line, "   %s", buff);

    lua_error(L); // never returns
}

//--------------------------------------------------------//
const char* luautils_LuaStatusToString(int stat)
{
    const char* sstat = "UNKNOWN";
    switch(stat)
    {
        case LUA_OK: sstat = "LUA_OK"; break;
        case LUA_YIELD: sstat = "LUA_YIELD"; break;
        case LUA_ERRRUN: sstat = "LUA_ERRRUN"; break;
        case LUA_ERRSYNTAX: sstat = "LUA_ERRSYNTAX"; break; // syntax error during pre-compilation
        case LUA_ERRMEM: sstat = "LUA_ERRMEM"; break; // memory allocation error
        case LUA_ERRERR: sstat = "LUA_ERRERR"; break; // error while running the error handler function
        case LUA_ERRFILE: sstat = "LUA_ERRFILE"; break; // couldn't open the given file
        default: break; // nothing else for now.
    }
    return sstat;
}

//--------------------------------------------------------//
int luautils_DumpTable(lua_State* L, const char* name)
{
    logger_Log(LVL_DEBUG, name, -1, name);

    // Put a nil key on stack.
    lua_pushnil(L);

    // key(-1) is replaced by the next key(-1) in table(-2).
    while (lua_next(L, -2) != 0)
    {
        // Get key(-2) name.
        const char* name = lua_tostring(L, -2);

        // Get type of value(-1).
        const char* type = luaL_typename(L, -1);

        logger_Log(LVL_DEBUG, name, -1, "   %s=%s", name, type);

        // Remove value(-1), now key on top at(-1).
        lua_pop(L, 1);
    }
    
    return RS_PASS;
}

//--------------------------------------------------------//
int luautils_DumpGlobals(lua_State* L)
{
    // Get global table.
    lua_pushglobaltable(L);

    luautils_DumpTable(L, "GLOBALS");

    // Remove global table(-1).
    lua_pop(L,1);

    return RS_PASS;
}

//--------------------------------------------------------//
int luautils_GetArgStr(lua_State* L, int index, char** ret)//TODO-REF could use gen_interop instead
{
    if(lua_isstring(L, index) > 0)
    {
        // Need to copy the string because the lua one will be GCed.
        const char* st = lua_tostring(L, index);
        *ret = calloc(strlen(st) + 1, 1);
        strcpy(*ret, st);
    }
    else
    {
        PROCESS_LUA_ERROR(L, LUA_ERRRUN, "Invalid string argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luautils_GetArgInt(lua_State* L, int index, int* ret)
{
    int valid = 0;
    if(lua_isnumber(L, index) > 0)
    {
        *ret = (int)lua_tointegerx(L, index, &valid);
    }

    if(valid == 0)
    {
        PROCESS_LUA_ERROR(L, LUA_ERRRUN, "Invalid integer argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luautils_GetArgDbl(lua_State* L, int index, double* ret)
{
    if(lua_isnumber(L, index) > 0)
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        PROCESS_LUA_ERROR(L, LUA_ERRRUN, "Invalid double argument at index %d", index);
    }

    return RS_PASS;
}

//--------------------------------------------------------//
int luautils_GetArgBool(lua_State* L, int index, bool* ret)
{
    if(lua_isboolean(L, index) > 0)
    {
        *ret = lua_toboolean(L, index); // always t/f
    }
    else
    {
        PROCESS_LUA_ERROR(L, LUA_ERRRUN, "Invalid bool argument at index %d", index);
    }

    return RS_PASS;
}


//--------------------------------------------------------//
void luautils_EvalStack(lua_State* l, int expected)
{
    int num = lua_gettop(l);
    if (num != expected)
    {
        printf("Expected %d stack but is %d", expected, num);
    }
}

//---------------- Private Implementation -------------//
