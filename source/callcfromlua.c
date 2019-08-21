
#include "stringx.h"
#include "callcfromlua.h"
#include "board.h"


//--------------------------------------------------------//
int p_luafunc_log(lua_State* L)
{
    ///// Get function arguments.
    int level = 0;
    stringx_t* info = stringx_create(NULL);
    luainterop_getArgInt(L, 1, &level);
    luainterop_getArgStr(L, 2, info);

    ///// Do the work.
    // Convert log level.
    loglvl_t llevel;

    switch(level)
    {
        case 0: llevel = LOG_INFO; break;
        case 1: llevel = LOG_WARN; break;
        default: llevel = LOG_ERROR; break;
    }

    common_log(llevel, stringx_content(info));
    stringx_destroy(info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
int p_luafunc_msec(lua_State* L)
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
int p_luafunc_digout(lua_State* L)
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
int p_luafunc_digin(lua_State* L)
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
static const luaL_Reg demolib[] =
{
    { "log",    p_luafunc_log },
    { "msec",   p_luafunc_msec },
    { "digout", p_luafunc_digout },
    { "digin",  p_luafunc_digin },
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param[in] L Lua state.
/// @return Status = 1 if ok.
int p_open_demolib (lua_State *L)
{
    // Register our C <-> Lua functions.
    //LOG(LOG_INFO, "p_open_demolib()");

    luaL_newlib(L, demolib);

    return 1;
}

//--------------------------------------------------------//
void demolib_preload(lua_State* L)
{
    //LOG(LOG_INFO, "demolib_preload()");
    luaL_requiref(L, "demolib", p_open_demolib, 1);
}
