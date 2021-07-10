--[[
Lua script for a simplistic multithreaded/coroutine application. Uses luatoc.
--]]


local luatoc = require "luatoc"
local ut = require "utils"
local math = require "math"

function do_loop()
  -- Do some work then yield.
  for i = 1, 10 do
    luatoc.serwr("loop num:"..i)

    -- Do work
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
 
    ut.sleep(200)

    -- Be polite, give back.
    coroutine.yield()
  end
end

-- Pin input has arrived from board.
function hinput (pin, value)
    luatoc.serwr(string.format("lua got input pin %d = %s ", pin, tostring(value)))
end

-- Dumb calculator, only does addition.
function calc (x, y)
  return (x + y)
end


-----------------------------------------------------------------------------
-- Module initialization.

luatoc.serwr("Module initialization")

-- for n in pairs(_G) do print(n) end

-- Process the passed context.
ctx = script_context
slog = string.format ("context script_string:%s script_int:%d", ctx.script_string, ctx.script_int)
luatoc.serwr(slog)

-- Start working.
do_loop()
