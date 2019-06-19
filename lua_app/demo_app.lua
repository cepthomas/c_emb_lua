--[[
Lua script for a simplistic realtime(ish) application.
--]]

local cemblua = require("cemblua")
local ut = require("utils")

function do_loop()
  -- Do work then yield.

  for i = 1, 10 do
    cemblua.log("loop num:"..i)
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
    coroutine.yield()
  end
end


-----------------------------------------------------------------------------
-- Module initialization.
-- ???

do_loop()
