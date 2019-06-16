
/*
Lua Stack
--------------
For convenience, most query operations in the API do not follow a strict stack discipline.
Instead, they can refer to any element in the stack by using an index: A positive index
represents an absolute stack position (starting at 1); a negative index represents an
offset relative to the top of the stack. More specifically, if the stack has n elements,
then index 1 represents the first element (that is, the element that was pushed onto the stack first)
and index n represents the last element; index -1 also represents the last element (that is,
the element at the top) and index -n represents the first element. We say that an index is valid
if it lies between 1 and the stack top (that is, if 1 = abs(index) = top).

 push  pop
  \/   /\
----------
 n     -1
----------
 n-1   -2
----------
   ...
----------
 2   -n+1
----------
 1     -n
----------


C API legend [o p x]
The first field, o, is how many elements the function pops from the stack.
The second field, p, is how many elements the function pushes onto the stack.
Note that any function always pushes its results after popping its arguments.

For instance:
void lua_gettable (lua_State *L, int index);
Pushes onto the stack the value t[k], where t is the value at the given valid
index and k is the value at the top of the stack.
This function pops the key from the stack (putting the resulting value in its place).

Alternatives
-------------
http://lua-users.org/wiki/UserDataExample
http://realmensch.org/blog/fun-lua-bindings
http://www.codenix.com/~tolua/
http://lua-users.org/wiki/BindingCodeToLua
http://lua-users.org/wiki/SimplerCppBinding
http://lua-users.org/wiki/LunaWrapper
http://www.uberengine.com/projects/pages/multi_script.html
http://code.google.com/p/oolua/
http://lua-users.org/wiki/DoItYourselfCppBinding
http://www.tecgraf.puc-rio.br/~celes/tolua/

*/


#include "common_module.h"


//--------------------------------------------------------//
void xlat_luaError(lua_State* L, const char* format, ...)
{
    static char p_buff[100];

    va_list args;
    va_start(args, format);
    vsnprintf(p_buff, sizeof(p_buff), format, args);

    lua_pushstring(L, p_buff);
    lua_error(L);
}

//--------------------------------------------------------//
status_t xlat_getArgInt(lua_State* L, int index, int* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isnumber(L, index) > 0) // valid
    {
        *ret = (int)lua_tointeger(L, index);
    }
    else
    {
        xlat_luaError(L, "Invalid integer argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t xlat_getArgDbl(lua_State* L, int index, double* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isnumber(L, index) > 0) // valid
    {
        *ret = lua_tonumber(L, index);
    }
    else
    {
        xlat_luaError(L, "Invalid double argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t xlat_getArgBool(lua_State* L, int index, bool* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isboolean(L, index) > 0) // valid
    {
        *ret = lua_toboolean(L, index);
    }
    else
    {
        xlat_luaError(L, "Invalid bool argument at index %d", index);
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t xlat_getArgStr(lua_State* L, int index, char* ret)
{
    status_t stat = STATUS_OK;

    if(lua_isstring(L, index) > 0) // valid
    {
        *ret = lua_tostring(L, index);
    }
    else
    {
        xlat_luaError(L, "Invalid string argument at index %d", index);
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
