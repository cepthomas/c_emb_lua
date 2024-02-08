--[[
Lua utilities.
--]]

local li = require "luainterop" -- C module

-- Create the namespace/module.
local M = {}


-- Creates a function that returns false until the arg is exceeded.
-- @param msec Number of milliseconds to wait.
-- @return status Function that returns state of the timer.
function M.delay_timer(msec)
  -- Init our copies of the args.
  local timeout = msec

  -- Grab the start time.
  local start = li.msec()
  
  -- The accessor function.
  local status =
    function()
      -- Are we there yet?
      return (li.msec() - start) > timeout
    end  
      
  return { status = status }
end

-- Blocking wait.
-- @param time Sleep time in msec.
function M.sleep(time)
  local timer = M.delay_timer(time)
  while not timer.status() do coroutine.yield() end
end


----------------------------------------------------------------------------
-- Module initialization.

-- Seed the randomizer.
seed = os.time()
math.randomseed(seed)
M.seed = seed

-- Return the module.
return M
