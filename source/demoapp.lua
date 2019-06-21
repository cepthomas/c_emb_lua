--[[
Lua script for a simplistic realtime(ish) application.
--]]

local ut = require("utils")
local math = require("math")
--local demolib = require("demolib")

function do_loop()
  -- Do work then yield.

  for i = 1, 10 do
    --demolib.log("loop num:"..i)
    log("loop num:"..i)

    -- Do work
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end

    -- Be polite, give back.
    coroutine.yield()
  end
end


-- Input has arrived. TODO
function hinput (pin, value)
  -- do something.
end

-- For test/dev remove TODO
function somecalc (x, y)
  return 11 -- x + y --(x^2 * math.sin(y))/(1 - x)
end



-----------------------------------------------------------------------------
-- Module initialization.
-- ???

do_loop()
