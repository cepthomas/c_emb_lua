
#include "stringx.h"
#include "demolib.h"
#include "board.h"


//---------------- Private --------------------------//

//---------------- Lua Functions in C ---------------//

// TODO template/generator for wrappers. also better error checking.

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

/// Trigger a digital interrupt.
/// nil interrupt(number pin)
static int p_luafunc_interrupt(lua_State* L);


//---------------- Private Utils --------------------//

/// @brief Report a bad thing detected by this component.
/// @param[in] L Lua state.
/// @param[in] format Standard string stuff.
/// @return Status.
void p_luaError(lua_State* L, const char* format, ...);

/// @brief Utility to get an int arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] ret The value.
/// @return Status.
status_t p_getArgInt(lua_State* L, int index, int* ret);

/// @brief Utility to get a double arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] ret The value.
/// @return Status.
status_t p_getArgDbl(lua_State* L, int index, double* ret);

/// @brief Utility to get a boolean arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] ret The value.
/// @return Status.
status_t p_getArgBool(lua_State* L, int index, bool* ret);

/// @brief Utility to get a string arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] ret The value.
/// @return Status.
status_t p_getArgStr(lua_State* L, int index, stringx_t* ret);


//---------------- Public Implementation -------------//

//--------------------------------------------------------//
status_t demolib_loadLibs(lua_State* L)
{
    status_t stat = STATUS_OK;

    // Load libraries.
    luaL_openlibs(L);

    // Register our C <-> Lua functions.
    common_log(LOG_INFO, "Initing Lua functions: demolib");

    // List of functions in the module.
    static const luaL_Reg demolib[] =
    {
        { "log",  p_luafunc_log },
        { "msec", p_luafunc_msec },
        { "digout", p_luafunc_digout },
        { "digin", p_luafunc_digin },
        { "interrupt", p_luafunc_interrupt },
        { NULL, NULL }
    };

    luaL_newlib(L, demolib);//TODO this goes into global space? old way was namespaced.

    return stat;
}

//---------------- Lua Funcs C -> Lua --------------------//

//--------------------------------------------------------//
status_t demolib_loadContext(lua_State* L, const char* s, int i)
{
    status_t stat = STATUS_OK;

    // Pass the context vals to the Lua world in a table named "script_context".
    lua_newtable(L);

    lua_pushstring(L, "script_string");
    lua_pushstring(L, s);
    lua_settable(L, -3);

    lua_pushstring(L, "script_int");
    lua_pushinteger(L, i);
    lua_settable(L, -3);

    lua_setglobal(L, "script_context");

    return stat;
}

//--------------------------------------------------------//
status_t demolib_luafunc_someCalc(lua_State* L, int x, int y, float* res)
{
    status_t stat = STATUS_OK;

    int lstat = 0;
    //LUA_OK		0
    //LUA_YIELD	1
    //LUA_ERRRUN	2
    //LUA_ERRSYNTAX	3
    //LUA_ERRMEM	4
    //LUA_ERRGCMM	5
    //LUA_ERRERR	6
    //For normal errors, lua_pcall returns the error code LUA_ERRRUN.
    //Two special kinds of errors deserve different codes, because they never run the error handler.
    //The first kind is a memory allocation error. For such errors, lua_pcall always returns LUA_ERRMEM.
    //The second kind is an error while Lua is running the error handler itself. In that case it is of
    //little use to call the error handler again, so lua_pcall returns immediately with a code LUA_ERRERR.
    //PANIC: unprotected error in call to Lua API (Call <function> failed)


    // Push the function to be called.
    lstat = lua_getglobal(L, "somecalc");
    // Push the arguments to the call.
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    // Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0); // 2!!!!
    lstat = lua_isnumber(L, -1);

    // Pop the results from the stack.
    *res = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);  // pop returned value

    if (lstat != 0)
    {
        p_luaError(L, "Call somecalc failed");
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t demolib_handleInput(lua_State* L, unsigned int pin, bool value)
{
    status_t stat = STATUS_OK;

    // Push the function to be called.
    lua_getglobal(L, "hinput");
    // Push the arguments to the call.
    lua_pushnumber(L, pin);
    lua_pushnumber(L, value);

    // Use lua_pcall to do the actual call.
    if (lua_pcall(L, 2, 1, 0) != 0 || lua_isnumber(L, -1))
    {
        p_luaError(L, "Call hinput failed");
    }

    return stat;
}

//---------------- Lua Funcs Lua -> C --------------------//

//--------------------------------------------------------//
int p_luafunc_log(lua_State* L)
{
    ///// Get function arguments.
    int level = 0;
    stringx_t* info = stringx_create(NULL);
    p_getArgInt(L, 1, &level);
    p_getArgStr(L, 2, info);

    ///// Do the work.
    // Convert log level.
    loglvl_t ll;

    switch(level)
    {
        case 0: ll = LOG_INFO; break;
        case 1: ll = LOG_WARN; break;
        default: ll = LOG_ERROR; break;
    }

    common_log(ll, stringx_content(info));

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
    int msec = common_getMsec();

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
    p_getArgInt(L, 1, &pin);
    p_getArgBool(L, 2, &state);

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
    p_getArgInt(L, 1, &pin);

    ///// Do the work.
    bool state;
    board_readDig((unsigned int)pin, &state);

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}

//--------------------------------------------------------//
int p_luafunc_interrupt(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    p_getArgInt(L, 1, &pin);

    ///// Do the work.
    //TODOX trigger a fake interrupt.

    ///// Push return values.
    return 0; // number of results
}

//---------------- Private Implementation ----------------//

//--------------------------------------------------------//
void p_luaError(lua_State* L, const char* format, ...)
{
    static char p_buff[100];

    va_list args;
    va_start(args, format);
    vsnprintf(p_buff, sizeof(p_buff), format, args);

    lua_pushstring(L, p_buff);
    lua_error(L);
}

//--------------------------------------------------------//
status_t p_getArgInt(lua_State* L, int index, int* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isnumber(L, index) > 0) // valid
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        p_luaError(L, "Invalid integer argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_getArgDbl(lua_State* L, int index, double* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isnumber(L, index) > 0) // valid
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        p_luaError(L, "Invalid double argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_getArgBool(lua_State* L, int index, bool* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isboolean(L, index) > 0) // valid
    {
        *ret = lua_toboolean(L, index);
    }
    else
    {
        p_luaError(L, "Invalid bool argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_getArgStr(lua_State* L, int index, stringx_t* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isstring(L, index) > 0) // valid
    {
        stringx_set(ret, lua_tostring(L, index));
    }
    else
    {
        p_luaError(L, "Invalid string argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}
