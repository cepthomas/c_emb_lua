--[[
Lua script for a simplistic multithreaded/coroutine application.
--]]

local lua2c = require "lua2c"
local ut = require "utils"
local math = require "math"

function do_loop()
  -- Do some work then yield.
  for i = 1, 10 do
    lua2c.log(0, "loop num:"..i)

    -- Do work
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
 
    ut.sleep(500)

    -- Be polite, give back.
    coroutine.yield()
  end
end


-- Input has arrived from board.
function hinput (pin, value)
    lua2c.log(0, "hinput on pin:%d - you really should do something useful with it...", pin)
end

-- For test/dev.
function somecalc (x, y)
  return (x^2 * math.sin(y)) / (1 - x)
end


-----------------------------------------------------------------------------
-- Module initialization.

lua2c.log(0, "Module initialization")

-- for n in pairs(_G) do print(n) end

-- Process the passed context.
ctx = script_context
slog = string.format ("context script_string:%s script_int:%d", ctx.script_string, ctx.script_int)
lua2c.log(0, slog)

-- Start working.
do_loop()
