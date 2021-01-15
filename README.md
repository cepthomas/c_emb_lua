# c-emb-lua
An example of a C embedded executable with lua script processor. It is minimal and needs
more boiler plate (error handling, logging, etc) to make it a real application.
Probably a lot of the interop could be refactored and/or generated from some sort of IDL.

It is a pure C99 application which should compile anywhere, including minimal embedded 
systems - anywhere you can compile lua. There is a QMake/mingw project included to build it for demonstration purposes.
It supports only windows (search WIN32) right now but should be easy to convert to linux or bare metal.

# Files
- exec.c/h - Does all the top-level work.
- main.c - Entry stub starts exec.
- board.c/h - Interface to the hardware board. Ours is simulated.
- common.c/h - Log, time, strings, ...
- c2lua.c/h - Interface to call lua functions from C.
- lua2c.c/h - Interface to call C functions from lua.
- luainterop.* - Generic stuff for talking with lua. This could be in a lib.  like: Utility to get a string arg off the Lua stack.  TODO combine with c2lua?
- demoapp.lua - Lua script for a simplistic multithreaded/coroutine application. Uses lua2c.
- utils.lua - Used by demoapp.lua.

# License
[App](https://github.com/cepthomas/c-emb-lua/blob/master/LICENSE)   
[Lua](https://github.com/cepthomas/c-emb-lua/blob/master/LUA-LICENSE)
