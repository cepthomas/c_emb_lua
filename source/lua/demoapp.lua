--[[
Lua script for a simplistic multithreaded/coroutine application. Uses luainterop.
--]]


local li = require "luainterop"
local ut = require "utils"
local math = require "math"


------------------------- Main loop ----------------------------------------------------

function do_loop()
  li.cliwr("do some pretend script work then yield")

  for i = 1, 10 do
    li.cliwr("doing loop num: " .. i)

    -- Do pretend work.
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
    ut.sleep(200)

    -- Plays well with others.
    coroutine.yield() --TODOX
  end
end

--------------------------- Module initialization ----------------------------------

li.cliwr("Module initialization")

-- for n in pairs(_G) do print(n) end

-- Process the passed context.
ctx = script_context
slog = string.format ("context script_string:%s script_int:%d", ctx.script_string, ctx.script_int)
li.cliwr(slog)

-- Start working.
do_loop()


-------------- Handlers for commands from C --------------------------

-- Pin input has arrived from board.
function hinput (pin, value) -- TODO force from cli
  li.cliwr(string.format("lua got input pin %d = %s ", pin, tostring(value)))
end

-- Dumb calculator, only does addition.
function calc (x, y)
  return (x + y)
end
