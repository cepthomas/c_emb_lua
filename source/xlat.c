
#include "stringx.h"
#include "xlat.h"
#include "board.h"


//---------------- Private --------------------------//

//---------------- Lua Functions in C ---------------//

// TODO template/generator?

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

/// @brief Report a bad thing.
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

/// @brief Utility to push a return vector of strings onto the Lua stack as a table.
/// @param[in] L Lua state.
/// @param[in] data The strings to push onto the Lua stack.
//void xlat_pushStrVector(lua_State* L, const QList<QString>& data);


//---------------- Public Implementation -------------//

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
        { "log",  p_luafunc_log },
        { "msec", p_luafunc_msec },
        { "digout", p_luafunc_digout },
        { "digin", p_luafunc_digin },
        { "interrupt", p_luafunc_interrupt },
        { NULL, NULL }
    };

    luaL_newlib(L, cemblua);  // was luaL_register(L, "cemblua", cemblua);

    return stat;
}

//---------------- Lua Funcs Implementation ----------------//

//--------------------------------------------------------//
int p_luafunc_log(lua_State* L)
{
    // Get function arguments.
    int level = 0;
    stringx_t* info = stringx_create(NULL);

    p_getArgInt(L, 1, &level);
    p_getArgStr(L, 2, info);

    // Convert log level.
    loglvl_t ll;

    switch(level)
    {
        case 0: ll = LOG_INFO; break;
        case 1: ll = LOG_WARN; break;
        default: ll = LOG_ERROR; break;
    }

    // Do the work.
    common_log(ll, stringx_content(info));

    stringx_destroy(info);

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

//--------------------------------------------------------//
int p_luafunc_digout(lua_State* L)
{
    (void)L;
/// nil digout(number pin, bool state)

    // Push return values.
    return 0; // number of results
}

//--------------------------------------------------------//
int p_luafunc_digin(lua_State* L)
{
    (void)L;
/// bool digin(number pin)

    // Push return values.
//    lua_pushinteger(L, msec);
    return 1; // number of results
}

//--------------------------------------------------------//
int p_luafunc_interrupt(lua_State* L)
{
    (void)L;

/// nil interrupt(number pin)

    // Push return values.
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


//--------------------------------------------------------//
// status_t xlat_pushStrVector(lua_State* L, const QStringList data)
// {
//     status_t stat = STATUS_OK;

//     // Start array structure.
//     lua_newtable(L);

//     // Determine the index.
//     int index = lua_gettop(L);

//     // Add the values.
//    QListIterator<QString> i(data);
//    while (i.hasNext())
//    {
//        lua_pushinteger(L, index); // key
//        lua_pushstring(L, i.next()); // value
//        lua_settable(L, -3); // set table entry
//        ++index;
//    }

//     // Push the new table.
//     lua_pushvalue(L, -1);
// }
