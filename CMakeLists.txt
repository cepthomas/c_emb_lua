cmake_minimum_required(VERSION 3.5)
project(cemblua)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# needs -g to debug.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wno-deprecated-declarations -Wno-unused-variable")

include_directories(
    source/c
    lua-5.3.5
    )

# Source files.
add_executable(cemblua
    source/c/main.c
    source/c/common.c
    source/c/exec.c
    source/c/board.c
    source/c/luainterop.c
    source/c/luautils.c
    source/c/logger.c
    lua-5.3.5/lapi.c
    lua-5.3.5/lauxlib.c
    lua-5.3.5/lbaselib.c
    lua-5.3.5/lbitlib.c
    lua-5.3.5/lcode.c
    lua-5.3.5/lcorolib.c
    lua-5.3.5/lctype.c
    lua-5.3.5/ldblib.c
    lua-5.3.5/ldebug.c
    lua-5.3.5/ldo.c
    lua-5.3.5/ldump.c
    lua-5.3.5/lfunc.c
    lua-5.3.5/lgc.c
    lua-5.3.5/linit.c
    lua-5.3.5/liolib.c
    lua-5.3.5/llex.c
    lua-5.3.5/lmathlib.c
    lua-5.3.5/lmem.c
    lua-5.3.5/loadlib.c
    lua-5.3.5/lobject.c
    lua-5.3.5/lopcodes.c
    lua-5.3.5/loslib.c
    lua-5.3.5/lparser.c
    lua-5.3.5/lstate.c
    lua-5.3.5/lstring.c
    lua-5.3.5/lstrlib.c
    lua-5.3.5/ltable.c
    lua-5.3.5/ltablib.c
    lua-5.3.5/ltm.c
    # lua-5.3.5/lua.c
    # lua-5.3.5/luac.c
    lua-5.3.5/lundump.c
    lua-5.3.5/lutf8lib.c
    lua-5.3.5/lvm.c
    lua-5.3.5/lzio.c
    )
