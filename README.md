# Lua Embedded in C

A minimal example of a C embedded executable with lua script processor.
It demonstrates:
- Coroutines - one state for the C side and one for the script side.
- Calling script functions with args from the C side.
- Calling C functions with args from the script side.
- Simulated embedded system with a hardware level, CLI for control, and exec loop running everything.
- Rudimentary error handling model.
- Uses [C code conventions](https://github.com/cepthomas/c_bag_of_tricks/blob/master/conventions.md).

# Build
- Application is pure C99 which should compile anywhere, including small embedded systems - basically anywhere you can compile lua.
- TODO1 Test code is Windows 64 bit build using CMake. PATH must include \your\path\mingw64\bin.

# Files
- make_test.cmd builds the executable.
- run_test.cmd runs the executable.
- source_code folder:
    - main.c - Entry stub calls exec.
    - exec.c/h - Does all the top-level work.
    - board.c/h - Interface to the (simulated) hardware.
    - common.c/h - Misc time, strings, ...
    - interop.c/h - Interfaces to call lua functions from C and to call C functions from lua.
    - demoapp.lua - Lua script for a simplistic multithreaded coroutine application. Uses luatoc.
    - utils.lua - Used by demoapp.lua.
