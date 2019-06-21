--[[
Lua script for a simplistic realtime(ish) application.
--]]

local demolib = require "demolib"
local ut = require "utils"
local math = require "math"

function do_loop()
  -- Do work then yield.

  for i = 1, 10 do
    -- demolib.log("loop num:"..i)
    demolib.log(0, "loop num:"..i)

    -- Do work
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end

    -- Be polite, give back.
    coroutine.yield()
  end
end


-- Input has arrived.
function hinput (pin, value)
    demolib.log(0, "hinput on pin:%d - you really should do something useful with it...", pin)
end

-- For test/dev.
function somecalc (x, y)
  return (x^2 * math.sin(y)) / (1 - x)
end



-----------------------------------------------------------------------------
-- Module initialization.
-- ???

demolib.log(0, "Module initialization")

demolib.log(0, demolib.script_context)
-- TODOX did this:
-- >stack traceback:
--         demoapp.lua:46: in main chunk

-- Process the passed context.
ctx = demolib.script_context
slog = string.format ("context script_string:%s script_int:%d", ctx.script_string, ctx.script_int)
demolib.log(0, slog)

do_loop()
