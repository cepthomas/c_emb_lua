cmake_minimum_required(VERSION 3.5)
project(cemblua_test)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(LUA "C:/Lua")

# needs -g to debug.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-but-set-variable")


include_directories(
    source/c
    ${LUA}/include
    )

# Source files.
add_executable(cemblua_test
    source/c/main.c
    source/c/common.c
    source/c/exec.c
    source/c/board.c
    source/c/interop.c
    source/c/luautils.c
    source/c/logger.c
    )

# Libs. 64 bit build.
target_link_libraries(cemblua_test
    ${LUA}/Win64/lua54.dll
    )
