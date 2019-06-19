--[[
Lua scripts for utilities.
--]]

-- Lua: > cemblua = require "cemblua"


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

-----------------------------------------------------------------------------
-- Module initialization.
sys.dlog("Loading script " .. M.clean_svn("$URL: http://ocdusrow3rndd3:8686/NEPTUNE_SANDBOX/trunk/RT/dev_tools/tex_lib/lib/lua/sys_utils.lua $"))

-- Return the module.
return M
