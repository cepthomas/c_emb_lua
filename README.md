# c-emb-lua
A minimal example of a C embedded executable with lua script processor.

- Pure C99 which should compile anywhere, including small embedded systems - basically anywhere you can compile lua.
- There is a QMake/mingw project included to build it for demonstration purposes.
- It supports only windows (search WIN32) right now but should be easy to convert to linux or bare metal.
- CLI pretends to be a serial port.

# Files
- main.c - Entry stub calls exec.
- exec.c/h - Does all the top-level work.
- board.c/h - Interface to the hardware. This is simulated.
- common.c/h - Log, time, strings, ...
- ctolua.c/h - Interface to call lua functions from C.
- luatoc.c/h - Interface to call C functions from lua.
- luainterop.c/h - Generic stuff for talking with lua. This could be in a lib.
- demoapp.lua - Lua script for a simplistic multithreaded/coroutine application. Uses luatoc.
- utils.lua - Used by demoapp.lua.

# License
[App](https://github.com/cepthomas/c-emb-lua/blob/master/LICENSE)   
[Lua](https://github.com/cepthomas/c-emb-lua/blob/master/LUA-LICENSE)
