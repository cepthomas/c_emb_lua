# Build the module with its unit tests.

cmake_minimum_required(VERSION 3.5)
project(cemblua_test)

# This is a Windows 64 bit build. PATH must include \your\path\mingw64\bin.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Project must be built for 64 bit")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# Lua headers.
set(LUA "C:/Lua")


include_directories(
    test
    ${LUA}/include
    )

# Source files.
add_executable(cemblua_test
    test/main.c
    test/exec.c
    test/board.c
    test/interop.c
    test/common.c
    )

# Libs.
target_link_libraries(cemblua_test
    ${LUA}/Win64/lua54.dll
    )
