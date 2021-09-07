--[[
Lua script for a simplistic multithreaded/coroutine application. Uses luainterop.
--]]


local luainterop = require "luainterop"
local ut = require "utils"
local math = require "math"

------------------------- Main loop ----------------------------------------------------

function do_loop()
  luainterop.cliwr("do some pretend script work then yield")

  for i = 1, 10 do
    luainterop.cliwr("loop num: " .. i)

    -- Do pretend work.
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
    luainterop.cliwr("do_loop() 10")
    ut.sleep(200)
    luainterop.cliwr("do_loop() 20")

    -- Plays well with others.
    coroutine.yield()
    luainterop.cliwr("do_loop() 30")
  end
end

--------------------------- Module initialization ----------------------------------

luainterop.cliwr("Module initialization")

-- for n in pairs(_G) do print(n) end

-- Process the passed context.
ctx = script_context
slog = string.format ("context script_string:%s script_int:%d", ctx.script_string, ctx.script_int)
luainterop.cliwr(slog)

-- Start working.
do_loop()


-------------- Handlers for commands from C --------------------------

-- Pin input has arrived from board.
function hinput (pin, value) -- TODO force from cli
  luainterop.cliwr(string.format("lua got input pin %d = %s ", pin, tostring(value)))
end

-- Dumb calculator, only does addition.
function calc (x, y)
  return (x + y)
end
