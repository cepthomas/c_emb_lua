--[[
Lua utilities.
--]]

local sys = require("system")

-- Create the namespace/module.
local M = {}

--- Creates a function that returns false until the arg is exceeded.
-- Some description, can be over several lines.
-- @param msec Number of milliseconds to wait.
-- @return status Function that returns state of the timer.
function M.delay_timer(msec)
  -- Init our copies of the args.
  local timeout = msec

  -- Grab the start time.
  local start = sys.msec()
  
  -- The accessor function.
  local status = 
    function()
      -- Are we there yet?
      return (sys.msec() - start) > timeout
    end  
      
  return { status = status }      
end

--- Blocking wait.
-- @param time Sleep time in msec.
function M.sleep(time)
  local timer = M.delay_timer(time)
  while not timer.status() do coroutine.yield() end
end

--- Concat the contents of the parameter list, separated by the string delimiter.
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

--- Split text into a list.
-- Consisting of the strings in text, separated by strings matching delimiter (which may be a pattern). 
--  Example: strsplit(",%s*", "Anna, Bob, Charlie,Dolores")
--  Borrowed from http://lua-users.org/wiki/SplitJoin.
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

--- Trims whitespace from both ends of a string.
-- Borrowed from http://lua-users.org/wiki/SplitJoin.
-- @param s The string to clean up.
-- @return string Cleaned up input string.
function M.strtrim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- Init the valid chars list, add as needed.
local chars = "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 00123456789"

--- Generate a sequence of numbers.
-- Loops around as needed.
-- @param start_value Start number.
-- @param end_value End number.
-- @param increment Step.
-- @return next Function that returns next number in the sequence.
function M.numb_seq(start_value, end_value, increment)
  -- Init our copies of the args.
  local sv = start_value
  local ev = end_value
  local inc = increment
  local n = sv -- next value
 
  -- Check for bad args.
  if increment == 0 or
  (start_value > end_value and increment > 0) or
  (start_value < end_value and increment < 0) then 
    error("Invalid arguments to numb_seq()")
  end
  
  -- The accessor function.
  local next = 
    function()
      -- Save the return value.
      ret = n
      -- Calc the next value.
      n = n + inc
      if inc < 0 then
        if n < ev then n = sv end
      else
        if n > ev then n = sv end
      end 
      return ret   
    end  
      
  return { next = next }      
end

--- Generate a sequence of values from the source table.
-- Some description, can be over several lines.
-- @param source Source table.
-- @return next Function that returns value.
function M.array_seq(source)
  -- Init our copies of the args.
  local t = source
  local n = 1 -- next value

  -- The accessor function.
  local next = 
    function()
      -- Save the return value.
      ret = t[n]
      -- Calc the next index.
      n = n + 1
      if n > #t then n = 1 end
      return ret   
    end  
      
  return { next = next }      
end

--- Select one from the parameter table.
-- Some description, can be over several lines.
-- @param source Source table.
-- @return next Function that returns value.
function M.array_rand(source) -- TODO4 weighting - none, sd, triangle, saw, ...
  -- Init our copies of the args.
  local t = source
  
  -- The accessor function.
  local next = 
    function()
      -- Calc the index.
      i = math.random(#t)
      return t[i]
    end  
      
  return { next = next }      
end

--- Generate a random number.
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

--- Create a random string.
-- @param rmin Minimum length.
-- @param rmax Maximum length.
-- @return next Function that returns string.
function M.str_rand(rmin, rmax)
  -- Init our copies of the args.
  local n = rmin
  local x = rmax
  
  -- The accessor function.
  local next = 
    function()
      ret = ""
      len = math.random(n, x)
      
      while len > 0 do
        i = math.random(string.len(chars))
        ret = ret .. string.sub(chars, i, i+1)
        len = len - 1
      end
      return ret
    end  
      
  return { next = next }      
end

-----------------------------------------------------------------------------
-- Module initialization.

-- Seed the randomizer and log it so we can use it to recreate later.
seed = os.time()
math.randomseed(seed)
M.seed = seed

-- Return the module.
return M
