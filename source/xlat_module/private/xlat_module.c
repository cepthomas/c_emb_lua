
#include "xlat_module.h"
#include "xlat_utils.h"
#include "board_module.h"


//---------------- Private --------------------------//

//TODO doc how to do this:
//Lua: > cemblua = require "cemblua"
//
/// Lua function to write to the debug log.
/// @lua function dlog
/// @lua param info The log line.
/// @lua param level Int level.
/// @treturn DC?
static int p_luafunc_dlog(lua_State* L);
//
/// Number of msec since the start of the application.
/// @lua function msec
/// @treturn int Msec since application started.
static int p_luafunc_msec(lua_State* L);


//---------------- Public API Implementation -------------//


//--------------------------------------------------------//
status_t xlat_init(void)
{
    status_t stat = STATUS_OK;

    return stat;
}

//--------------------------------------------------------//
status_t xlat_destroy(void)
{
    status_t stat = STATUS_OK;

    return stat;
}

//--------------------------------------------------------//
status_t xlat_loadLibs(lua_State* L)
{
    status_t stat = STATUS_OK;

    // Load libraries.
    luaL_openlibs(L);

    // Register our C <-> Lua functions.
    common_log(LOG_INFO, "Initing Lua functions: cemblua");

    // List of functions in the module.
    static const luaL_Reg cemblua[] =
    {
        { "dlog", p_luafunc_dlog },
        { "msec", p_luafunc_msec },
        { NULL, NULL }
    };

    luaL_newlib(L, cemblua);   // was luaL_register(L, "cemblua", cemblua);

    return stat;
}


//---------------- Private --------------------------//


//--------------------------------------------------------//
int p_luafunc_dlog(lua_State* L)
{
    /// Get function arguments.
    status_t status = STATUS_OK;

    int level = 0;// TODO enum
    QString info = NULL;

    xlat_getArgInt(L, 1, &level);
    xlat_getArgStr(L, 2, info);

    common_log(level, info);

    // Push return values.
    return 0; // number of results
}


//--------------------------------------------------------//
int p_luafunc_msec(lua_State* L)
{
    int msec = common_getMsec();

    // Push return values.
    lua_pushinteger(L, msec);
    return 1; // number of results
}

