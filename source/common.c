
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//

/// When the app was started.
static uint64_t p_startTime;

/// Fixed size.
#define LOG_LINE_LEN 100


//---------------- Public Implementation -------------//


//--------------------------------------------------------//
status_t common_init(void)
{
    status_t stat = STATUS_OK;

    p_startTime = board_getCurrentUsec();

    return stat;
}

//--------------------------------------------------------//
status_t common_log(loglvl_t level, const char* file, int line, const char* format, ...)
{
    status_t stat = STATUS_OK;
  
    static char buff[LOG_LINE_LEN];

    sprintf(buff, "%s %s(%d) ", common_xlatLogLevel(level), file, line);
    va_list args;
    va_start(args, format);
    vsnprintf(buff + strlen(buff), LOG_LINE_LEN - 1, format, args);
    board_log(buff);

    return stat;
}

//--------------------------------------------------------//
unsigned int common_getMsec()
{
    uint64_t now = board_getCurrentUsec();
    unsigned int msec = abs((unsigned int)((now - p_startTime) / 1000));
    return msec;
}

//--------------------------------------------------------//
const char* common_xlatLogLevel(loglvl_t level)
{
    const char* ll = "???";
    switch(level)
    {
        case LOG_INFO:  ll = "INF"; break;
        case LOG_WARN:  ll = "WRN"; break;
        case LOG_ERROR: ll = "ERR"; break;
    }
    return ll;
}    

//--------------------------------------------------------//
const char* common_xlatLuaStatus(int lstat)
{
    const char* lerr = "???";
    switch(lstat)
    {
        case LUA_OK: lerr = "LUA_OK"; break;
        case LUA_YIELD: lerr = "LUA_YIELD"; break;
        case LUA_ERRRUN: lerr = "LUA_ERRRUN"; break;
        case LUA_ERRSYNTAX: lerr = "LUA_ERRSYNTAX"; break;
        case LUA_ERRMEM: lerr = "LUA_ERRMEM"; break;
        case LUA_ERRGCMM: lerr = "LUA_ERRGCMM"; break;
        case LUA_ERRERR: lerr = "LUA_ERRERR"; break;
    }
    return lerr;
}

//--------------------------------------------------------//
const char* common_xlatStatus(status_t stat)
{
    const char* ss = "???";
    switch(stat)
    {
        case STATUS_OK:    ss = "STATUS_OK"; break;
        case STATUS_WARN:  ss = "STATUS_WARN"; break;
        case STATUS_ERROR: ss = "STATUS_ERROR"; break;
        case STATUS_FATAL: ss = "STATUS_FATAL"; break;
    }
    return ss;
}    

//--------------------------------------------------------//
void common_dumpStack(lua_State *L)
{
    static char buff[LOG_LINE_LEN];

    for (int i = lua_gettop(L); i >= 1; i--)
    {
        int t = lua_type(L, i);

        switch (t)
        {
            case LUA_TSTRING:        sprintf(buff, "ind:%d string:%s ", i, lua_tostring(L, i));   break;
            case LUA_TBOOLEAN:       sprintf(buff, "ind:%d bool:%s ", i, lua_toboolean(L, i) ? "true" : "false");   break;
            case LUA_TNUMBER:        sprintf(buff, "ind:%d number:%g ", i, lua_tonumber(L, i));   break;
            case LUA_TNIL:           sprintf(buff, "ind:%d nil", i);   break;
            case LUA_TNONE:          sprintf(buff, "ind:%d none", i);   break;
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TTHREAD:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA: sprintf(buff, "ind:%d %s:%p ", i, lua_typename(L, t), lua_topointer(L, i));   break;
            default: sprintf(buff, "ind:%d ????", i);   break;
        }

        board_log(buff);
    }
}


//---------------- Private --------------------------//

