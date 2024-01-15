# Build the module with its unit tests.
# This is a Windows 64 bit build. PATH must include ...\mingw64\bin.

cmake_minimum_required(VERSION 3.5)
project(cemblua_test)

# set(CMAKE_C_STANDARD 99)
# set(CMAKE_CXX_STANDARD 11)

# Lua headers.
set(LUA "C:/Lua")

# needs -g to debug.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")


include_directories(
    source
    test
    ${LUA}/include
    )

# Source files.
add_executable(cemblua_test
    source/private/luautils.c
    source/private/luaex.c
    test/main.c
    test/exec.c
    test/board.c
    test/interop.c
    test/common.c
    )

# Libs.
target_link_libraries(cemblua_test  ${LUA}/Win64/lua54.dll)
