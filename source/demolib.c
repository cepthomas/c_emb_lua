
#include "stringx.h"
#include "demolib.h"
#include "board.h"


//---------------- Private --------------------------//

//---------------- Lua Functions in C ---------------//

// TODO Use template/generator for wrappers?

/// Write to the log.
/// nil log(number level, string text)
static int p_luafunc_log(lua_State* L);

/// Number of msec since the start of the application.
/// number msec()
static int p_luafunc_msec(lua_State* L);

/// Set a digital output.
/// nil digout(number pin, bool state)
static int p_luafunc_digout(lua_State* L);

/// Get a digital input (or output).
/// bool digin(number pin)
static int p_luafunc_digin(lua_State* L);


//---------------- Private Utils --------------------//


/// Called by system to actually load the lib.
/// @param[in] L Lua state.
/// @return Status = 1 if ok.
int p_open_demolib (lua_State *L);

/// List of functions in the module.
static const luaL_Reg demolib[] =
{
    { "log",  p_luafunc_log },
    { "msec", p_luafunc_msec },
    { "digout", p_luafunc_digout },
    { "digin", p_luafunc_digin },
    { NULL, NULL }
};


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
void demolib_preload(lua_State* L)
{
    //LOG(LOG_INFO, "demolib_preload()");
    luaL_requiref(L, "demolib", p_open_demolib, 1);
}

//---------------- Lua Funcs C -> Lua --------------------//

//--------------------------------------------------------//
void demolib_loadContext(lua_State* L, const char* s, int i)
{
    //LOG(LOG_INFO, "demolib_loadContext()");

    ///// Pass the context vals to the Lua world in a table named "script_context".
    lua_newtable(L); // Creates a new empty table and pushes it onto the stack.

    lua_pushstring(L, "script_string");
    lua_pushstring(L, s);
    lua_settable(L, -3);
    
    lua_pushstring(L, "script_int");
    lua_pushinteger(L, i);
    lua_settable(L, -3);

    lua_setglobal(L, "script_context"); // Pops a value from the stack and sets it as the new value of global name.
}

//--------------------------------------------------------//
void demolib_luafunc_someCalc(lua_State* L, int x, int y, double* res)
{
    //LOG(LOG_INFO, "demolib_luafunc_someCalc()");

    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "somecalc");

    ///// Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);
    if (lstat >= LUA_ERRRUN)
    {
        luainterop_luaError(L, "lua_pcall somecalc() failed");
    }

    ///// Pop the results from the stack.
    if(lua_isnumber(L, -1))
    {
        *res = lua_tonumber(L, -1);
    }
    else
    {
        luainterop_luaError(L, "Bad somecalc() return value");
    }

    lua_pop(L, 1);  // pop returned value
}

//--------------------------------------------------------//
void demolib_handleInput(lua_State* L, unsigned int pin, bool value)
{
    ///// Push the function to be called.
    lua_getglobal(L, "hinput");

    ///// Push the arguments to the call.
    lua_pushnumber(L, pin);
    lua_pushnumber(L, value);

    ///// Use lua_pcall to do the actual call.
    int lstat = lua_pcall(L, 2, 1, 0);

    if (lstat >= LUA_ERRRUN)
    {
        luainterop_luaError(L, "Call hinput() failed");
    }

    /////
    // no return value
}

//---------------- Lua Funcs Lua -> C --------------------//

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


//---------------- Private Implementation ----------------//

//--------------------------------------------------------//
int p_open_demolib (lua_State *L)
{
    // Register our C <-> Lua functions.
    //LOG(LOG_INFO, "p_open_demolib()");

    luaL_newlib(L, demolib);

    return 1;
}

