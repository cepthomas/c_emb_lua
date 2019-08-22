# c-emb-lua
An example of a C embedded executable with lua script processor. It is minimal and needs
more boiler plate (error handling, logging, etc) to make it a real application.

It is a pure C99 application which should compile anywhere, including for minimal embedded 
systems - anywhere you can compile lua. There is a QMake/mingw project included to build it for demonstration purposes.
It supports only windows (search WIN32) right now but should be easy to convert to linux or bare metal.

# License
[App](https://github.com/cepthomas/c-emb-lua/blob/master/LICENSE)   
[Lua](https://github.com/cepthomas/c-emb-lua/blob/master/LUA-LICENSE)
