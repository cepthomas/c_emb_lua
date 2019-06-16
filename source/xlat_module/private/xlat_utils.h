
#ifndef LUA_STACK_H
#define LUA_STACK_H

#include "common_module.h"
#include "xlat_module.h"

/// @file Lua helper functions.

/// @brief Report a bad thing.
/// @param[in] L Lua state.
/// @param[in] format Standard string stuff.
/// @return Status.
void xlat_luaError(lua_State* L, const char* format, ...);

/// @brief Utility to get an int arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] The value.
/// @return Status.
status_t xlat_getArgInt(lua_State* L, int index, int* ret);

/// @brief Utility to get a double arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] The value.
/// @return Status.
status_t xlat_getArgDbl(lua_State* L, int index, double* ret);

/// @brief Utility to get a boolean arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] The value.
/// @return Status.
status_t xlat_getArgBool(lua_State* L, int index, bool* ret);

/// @brief Utility to get a string arg off the Lua stack.
/// @param[in] L Lua state.
/// @param[in] index Index of the entry on the Lua stack.
/// @param[out] The value.
/// @return Status.
status_t xlat_getArgStr(lua_State* L, int index, char* ret);


/// @brief Utility to push a return vector of strings onto the Lua stack as a table.
/// @param[in] L Lua state.
/// @param[in] data The strings to push onto the Lua stack.
//void xlat_pushStrVector(lua_State* L, const QList<QString>& data);


#endif // LUA_STACK_H
