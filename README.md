# C Embedded Lua

A minimal example of a C embedded executable with lua script processor.
It demonstrates:
- Coroutines - one state for the C side and one for the script side.
- Calling script functions with args from the C side.
- Calling C functions with args from the script side.
- Simulated embedded system with a hardware level, CLI for control, and exec loop running everything.
- Rudimentary error handling model - more required.
- For conventions see [c_modular](https://github.com/cepthomas/c_modular/blob/master/README.md).

# Build
- Application is pure C99 which should compile anywhere, including small embedded systems - basically anywhere you can compile lua.
- Test code is Windows 64 bit build using CMake. PATH must include \your\path\mingw64\bin.
- Run build.cmd to make the executable.
- Run test.cmd to run the executable.

# Files
- `source` folder:
    - luautils.c/h - General purpose tools for probing lua stacks.
    - luaex.c/h - Lua API extensions.
- `test` folder:
    - main.c - Entry stub calls exec.
    - exec.c/h - Does all the top-level work.
    - board.c/h - Interface to the (simulated) hardware.
    - common.c/h - Misc time, strings, ...
    - luainterop.c/h - Interfaces to call lua functions from C and to call C functions from lua.
    - luautils.c/h - General purpose tools for probing lua stacks.
    - demoapp.lua - Lua script for a simplistic multithreaded coroutine application. Uses luatoc.
    - utils.lua - Used by demoapp.lua.

# Licenses
[This repo](https://github.com/cepthomas/c-emb-lua/blob/master/LICENSE)

[Lua](https://github.com/cepthomas/c-emb-lua/blob/master/LUA-LICENSE)
