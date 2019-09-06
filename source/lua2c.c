
#include "lua2c.h"
#include "board.h"


// Interface to call C functions from lua.


//--------------------------------------------------------//

int p_lua2c_log(lua_State* L)
{
    ///// Get function arguments.
    int level = 0;
    const char* info = NULL;
    luainterop_getArgInt(L, 1, &level);
    luainterop_getArgStr(L, 2, &info);

    ///// Do the work.
    // Convert log level.
    loglvl_t llevel;

    switch(level)
    {
        case 0: llevel = LOG_INFO; break;
        case 1: llevel = LOG_WARN; break;
        default: llevel = LOG_ERROR; break;
    }

    common_log(llevel, info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
//  local start = lua2c.msec()

int p_lua2c_msec(lua_State* L)
{
    ///// Get function arguments.
    // none

    ///// Do the work.
    unsigned int msec = common_getMsec();

    ///// Push return values.
    lua_pushinteger(L, msec);
    return 1; // number of results
}

//--------------------------------------------------------//
int p_lua2c_digout(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    bool state;
    luainterop_getArgInt(L, 1, &pin);
    luainterop_getArgBool(L, 2, &state);

    ///// Do the work.
    board_writeDig((unsigned int)pin, state);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
int p_lua2c_digin(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    luainterop_getArgInt(L, 1, &pin);

    ///// Do the work.
    bool state;
    board_readDig((unsigned int)pin, &state);

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}


//--------------------------------------------------------//
/// List of functions in the module.
static const luaL_Reg lua2clib[] =
{
//  { lua func, c func }
    { "log",    p_lua2c_log },
    { "msec",   p_lua2c_msec },
    { "digout", p_lua2c_digout },
    { "digin",  p_lua2c_digin },
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param[in] L Lua state.
/// @return Status = 1 if ok.
int p_open_lua2c (lua_State *L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, lua2clib);

    return 1;
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib.
/// \param L
void lua2c_preload(lua_State* L)
{
    luaL_requiref(L, "lua2c", p_open_lua2c, 1);
}
