# c_emb_lua
A minimal example of a C embedded executable with lua script processor.
It demonstrates:
- Coroutines - one state for the C side and one for the script side.
- Calling script functions with args from the C side.
- Calling C functions with args from the script side.
- Simulated embedded system with a hardware level, CLI for control, and exec loop running everything.

# Build
- Pure C99 which should compile anywhere, including small embedded systems - basically anywhere you can compile lua.
- A VS Code workspace using mingw and CMake is supplied. Your PATH needs to include mingw.
- Run build.cmd to make the executables.
- It supports only windows (search WIN32) right now but should be easy to convert to linux or bare metal.

# Files
- [Conventions](https://github.com/cepthomas/c_bag_of_tricks/blob/master/CONVENTIONS.md).
- [Modular model](https://github.com/cepthomas/c_modular).
- `c` folder:
    - main.c - Entry stub calls exec.
    - exec.c/h - Does all the top-level work.
    - board.c/h - Interface to the (simulated) hardware.
    - common.c/h - Misc time, strings, ...
    - luainterop.c/h - Interfaces to call lua functions from C and to call C functions from lua.
- `lua` folder:
    - demoapp.lua - Lua script for a simplistic multithreaded coroutine application. Uses luatoc.
    - utils.lua - Used by demoapp.lua.
- `lua-5.3.5` folder:
    - lua source code for this application.

# Licenses
[This repo](https://github.com/cepthomas/c-emb-lua/blob/master/LICENSE)

[Lua](https://github.com/cepthomas/c-emb-lua/blob/master/LUA-LICENSE)
