
#include "lua_interop.h"
#include "my_custom.h"

TODO error checking everywhere


void %namespace%_my_data_struct_to_table(lua_State* L, my_data_t* pd) // was ctolua_Context
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


void %namespace%_my_data_table_to_struct(lua_State* L, my_data_t* pd)
{
    // TODO read fields from L, check for proper type


}


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


////////////////////////////////////////////////////////////////////
////////////////// luatoc call C functions from lua ////////////////
////////////////////////////////////////////////////////////////////

//--------------------------------------------------------//
/// Get dig input.
/// @param L Lua environment.
/// @param state what it be
int p_DigIn(lua_State* L)
{
    ///// Get function arguments.
    int pin;
    luainterop_GetArgInt(L, 1, &pin);
    // luainterop_GetArgBool(L, 2, &state); p_DigOut

    ///// Do the work.
    bool state;
    board_ReadDig((unsigned int)pin, &state); // TODO app must implement this
    /////

    ///// Push return values.
    lua_pushboolean(L, state);
    return 1; // number of results
}

//--------------------------------------------------------//
/// Map functions in the module.
static const luaL_Reg %namespace%_lib[] =
{
    //lua       c
    { "digin",  p_DigIn },
    { "digout", p_DigOut },
    { NULL, NULL }
};

//--------------------------------------------------------//
/// Called by system to actually load the lib.
/// @param L Lua state.
/// @return Status = 1 if ok.
int p_open_%namespace%(lua_State *L)
{
    // Register our C <-> Lua functions.
    luaL_newlib(L, %namespace%_lib);

    return 1;
}

//--------------------------------------------------------//
/// Identify the system callback to load the lib.
/// @param L Lua state.
void %namespace%_Preload(lua_State* L)
{
    luaL_requiref(L, "my_custom", p_open_%namespace%, 1);
}

