
#include "luatoc.h"
#include "board.h"


//--------------------------------------------------------//

int p_SerWrite(lua_State* L)
{
    ///// Get function arguments.
    const char* info = NULL;
    luainterop_GetArgStr(L, 1, &info);

    ///// Do the work.
    board_SerWriteLine(info);

    ///// Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
//  local start = luatoc.msec()

int p_Msec(lua_State* L)
{
    ///// Get function arguments.
    // none

    ///// Do the work.
    unsigned int msec = common_GetMsec();

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
    luainterop_GetArgInt(L, 1, &pin);
    luainterop_GetArgBool(L, 2, &state);

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
    luainterop_GetArgInt(L, 1, &pin);

    ///// Do the work.
    bool state;
    board_ReadDig((unsigned int)pin, &state);

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}


//--------------------------------------------------------//
/// Map functions in the module.
static const luaL_Reg luatoclib[] =
{
//  { lua func, c func }
    { "serwr",  p_SerWrite },
    { "msec",   p_Msec },
    { "digout", p_DigOut },
    { "digin",  p_DigIn },
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_OpenLuatoc (lua_State *L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, luatoclib);

    return 1;
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib.
/// @param L Lua state.
void luatoc_Preload(lua_State* L)
{
    luaL_requiref(L, "luatoc", p_OpenLuatoc, 1);
}
