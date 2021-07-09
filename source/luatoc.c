
#include "luatoc.h"
#include "board.h"


//--------------------------------------------------------//

int p_luatoc_serwr(lua_State* L)
{
    ///// Get function arguments.
    const char* info = NULL;
    luainterop_getArgStr(L, 1, &info);

    ///// Do the work.
    board_serWriteLine(info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
//  local start = luatoc.msec()

int p_luatoc_msec(lua_State* L)
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
int p_luatoc_digout(lua_State* L)
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
int p_luatoc_digin(lua_State* L)
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
/// Map functions in the module.
static const luaL_Reg luatoclib[] =
{
//  { lua func, c func }
    { "serwr",  p_luatoc_serwr },
    { "msec",   p_luatoc_msec },
    { "digout", p_luatoc_digout },
    { "digin",  p_luatoc_digin },
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_open_luatoc (lua_State *L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, luatoclib);

    return 1;
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib.
/// @param L Lua state.
void luatoc_preload(lua_State* L)
{
    luaL_requiref(L, "luatoc", p_open_luatoc, 1);
}
