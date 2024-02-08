
cmake_minimum_required(VERSION 3.5)
project(cemblua)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Project is 64 bit")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# Lua headers.
set(LUA "C:/Lua")


include_directories(
    source_code
    ${LUA}/include
    )

# Source files.
add_executable(cemblua
    source_code/main.c
    source_code/exec.c
    source_code/board.c
    source_code/interop.c
    source_code/common.c
    )

# Libs.
target_link_libraries(cemblua
    ${LUA}/Win64/lua54.dll
    )
