--[[
Lua utilities.
--]]

local luatoc = require "luatoc"

-- Create the namespace/module.
local M = {}

-- Creates a function that returns false until the arg is exceeded.
-- @param msec Number of milliseconds to wait.
-- @return status Function that returns state of the timer.
function M.delay_timer(msec)
  -- Init our copies of the args.
  local timeout = msec

  -- Grab the start time.
  local start = luatoc.msec()
  
  -- The accessor function.
  local status =
    function()
      -- Are we there yet?
      return (luatoc.msec() - start) > timeout
    end  
      
  return { status = status }      
end

-- Blocking wait.
-- @param time Sleep time in msec.
function M.sleep(time)
  local timer = M.delay_timer(time)
  while not timer.status() do coroutine.yield() end
end

-- Concat the contents of the parameter list, separated by the string delimiter.
-- Example: strjoin(", ", {"Anna", "Bob", "Charlie", "Dolores"})
-- Borrowed from http://lua-users.org/wiki/SplitJoin.
-- @param delimiter Delimiter.
-- @param list The pieces parts.
-- @return string Concatenated list.
function M.strjoin(delimiter, list)
  local len = #list
  if len == 0 then 
    return "" 
  end
  local string = list[1]
  for i = 2, len do 
    string = string .. delimiter .. list[i] 
  end
  return string
end

-- Split text into a list.
-- Consisting of the strings in text, separated by strings matching delimiter (which may be a pattern). 
-- Example: strsplit(",%s*", "Anna, Bob, Charlie,Dolores")
-- Borrowed from http://lua-users.org/wiki/SplitJoin.
-- @param delimiter Delimiter.
-- @param text The string to split.
-- @return list Split input.
function M.strsplit(delimiter, text)
  local list = {}
  local pos = 1
  if string.find("", delimiter, 1) then -- this would result in endless loops
    error("Delimiter matches empty string.")
  end
  while 1 do
    local first, last = string.find(text, delimiter, pos)
    if first then -- found?
      table.insert(list, string.sub(text, pos, first-1))
      pos = last+1
    else
      table.insert(list, string.sub(text, pos))
      break
    end
  end
  return list
end

-- Trims whitespace from both ends of a string.
-- Borrowed from http://lua-users.org/wiki/SplitJoin.
-- @param s The string to clean up.
-- @return string Cleaned up input string.
function M.strtrim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- Generate a random number.
-- @param rmin Minimum number value.
-- @param rmax Maximum number value.
-- @return next Function that returns number.
function M.numb_rand(rmin, rmax)
  -- Init our copies of the args.
  local n = rmin
  local x = rmax
  
  -- Determine if this is an integer or real rand.
  ni, nf = math.modf(n)
  xi, xf = math.modf(x)
  
  -- If either has a fractional part > 0, it's a float.
  if nf > 0 or xf > 0 then
    local next = function() return math.random() * (x - n) + n  end 
    return { next = next }      
  else -- it's an int
    local next = function() return math.random(n, x) end  
    return { next = next }
  end
end

----------------------------------------------------------------------------
-- Module initialization.

-- Seed the randomizer.
seed = os.time()
math.randomseed(seed)
M.seed = seed

-- Return the module.
return M
