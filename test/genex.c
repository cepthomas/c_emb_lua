
#include "genned.h"

// TODO error checking everywhere

///////// xlat a struct <-> table. was ctolua_Context
void p_push_struct_my_data(lua_State* L, my_data_t* pd)
{
    // Create a new empty table and pushes it onto the stack.
    lua_newtable(L);

    lua_pushstring(L, "f1");
    lua_pushnumber(L, pd->f1);
    lua_settable(L, -3);
    
    lua_pushstring(L, "f2");
    lua_pushinteger(L, pd->f2);
    lua_settable(L, -3);

    lua_setglobal(L, "my_data");
}


void p_pop_struct_my_data(lua_State* L, my_data_t* pd)
{
    // TODO read fields from L, check for proper type


}



//--------------------------------------------------------//
//------------------luainterop.h--------------------------//
//--------------------------------------------------------//

/// @brief Generic stuff for talking with lua.

/// Dump the lua stack contents.
/// @param L Lua state.
int luainterop_DumpStack(lua_State *L);

/// Report a bad thing detected by this component.
/// @param L Lua state.
/// @param format Standard string stuff.
int luainterop_LuaError(lua_State* L, const char* format, ...);

/// Utility to get an int arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int luainterop_GetArgInt(lua_State* L, int index, int* ret);

/// Utility to get a double arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int luainterop_GetArgDbl(lua_State* L, int index, double* ret);

/// Utility to get a boolean arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int luainterop_GetArgBool(lua_State* L, int index, bool* ret);

/// Utility to get a string arg off the Lua stack.
/// @param L Lua state.
/// @param index Index of the entry on the Lua stack.
/// @param[out] ret The value.
int luainterop_GetArgStr(lua_State* L, int index, const char** ret);




////////////////////////////////////////////////////////////////////
////////////////// ctolua call lua functions from C ////////////////
////////////////////////////////////////////////////////////////////

//--------------------------------------------------------//
/// Dumb calculator.
/// @param L Lua environment.
/// @param Op1 op 1.
/// @param Op2 op 2.
/// @param Res1 one result.
void p_calc(lua_State* L, int Op1, int Op2, double* Res1) //<<< ctolua_Calc, args
{
    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "calc"); //<<< "calc"

    ///// Push the arguments to the call.
    lua_pushinteger(L, Op1); //<<< these
    lua_pushinteger(L, Op2);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0); //<<< 2
    if(lstat >= LUA_ERRRUN)
    {
        luainterop_LuaError(L, "lua_pcall calc() failed"); //<<< calc()
    }

    ///// Pop the results from the stack.
    if(lua_isnumber(L, -1)) //<<< these, mult
    {
        *res = lua_tonumber(L, -1);
    }
    else
    {
        luainterop_LuaError(L, "Bad calc() return value"); //<<< calc()
    }

    lua_pop(L, 1);  // pop returned value
}


//--------------------------------------------------------//
void ctolua_HandleDigInput(lua_State* L, unsigned int pin, bool value)
{
    int lstat = 0;

    ///// Push the function to be called.
    lstat = lua_getglobal(L, "hinput");

    ///// Push the arguments to the call.
    lua_pushinteger(L, pin);
    lua_pushboolean(L, value);

    ///// Use lua_pcall to do the actual call.
    lstat = lua_pcall(L, 2, 1, 0);

    if(lstat >= LUA_ERRRUN)
    {
        luainterop_LuaError(L, "Call hinput() failed");
    }

    /////
    // no return value
}


////////////////////////////////////////////////////////////////////
////////////////// luatoc call C functions from lua ////////////////
////////////////////////////////////////////////////////////////////

//--------------------------------------------------------//

int p_CliWrite(lua_State* L)
{
    ///// Get function arguments.
    const char* info = NULL;
    luainterop_GetArgStr(L, 1, &info);

    ///// Do the work.
    board_CliWriteLine(info);

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
    { "cliwr",  p_CliWrite },
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
