--[[
Converts idl into c/lua interop. TODO html, json doc?
--]]

-- Setup environment.
local pp = require('pl.path')
local pd = require('pl.dir')
-- local pretty = require('pl.pretty')
-- local utils = require('utils')
local package = require('package')

-- local namespace = ""
-- local desc = ""

local enums = {}
local structs = {}
local funcs1 = {}
local funcs2 = {}


-- from dif
-- Get paths to lua components, depends on OS.
root = os.getenv("NEPTUNE_ROOT")

if os.getenv("ProgramFiles") ~= nil then -- win64
  -- package.path = "?.lua;" .. root .. "/SOUP/lua/?.lua;" .. root .. "/Source/dev_tools/lua/?.lua;"
  -- package.cpath = root .. "/SOUP/win/lua/clibs/?.dll;" -- .. package.cpath
  path_sep = "/"
else -- linux
  -- package.path = "?.lua;" .. root .. "/SOUP/lua/?.lua;" .. root .. "/Source/dev_tools/lua/?.lua;"
  -- package.cpath = root .. "/SOUP/linux/lua/clibs/?.so;" -- .. package.cpath
  path_sep = "\\"
end



-- Generic output file writer.
function write_output(content, err, outpath)
  --print("+++" .. outpath)
  cf = io.open(outpath, "w")
  if content ~= nil then
    cf:write(content)
  else
    cf:write(err)
  end
  cf:close()
end



---------------------------- Generate .h file --------------------------
function gen_h()


  write_output(content, err, outpath .. path_sep .. "data_" .. bn .. ".h")
end

---------------------------- Generate .c file --------------------------
function gen_c()

  write_output(content, err, outpath .. path_sep .. "data_" .. bn .. ".c")
end


---------------------------- Generate .lua file --------------------------
function gen_lua() -- TODO should be a class. http://www.luafaq.org/#T1.28

    write_output(content, err, outpath .. path_sep .. "data_" .. bn .. ".lua")
end


-------------------------------------------------
------------------ Utils Borrowed from http://lua-users.org/wiki/SplitJoin. ------------------------
-------------------------------------------------

-- Concat the contents of the parameter list, separated by the string delimiter.
function strjoin(delimiter, list)
  local len = #list
  if len == 0 then 
    return "" 
  end
  local s = list[1]
  for i = 2, len do 
    s = s .. delimiter .. list[i] 
  end
  return s
end

-- Split text into a list by delimiter.
function strsplit(delimiter, text)
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
function strtrim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-------------------------------------------------
------------------ My new -----------------------
-------------------------------------------------


-- Parse a standard line like: option=READY, num=1, desc="Ready to be scheduled"
-- Returns dictionary of contents.
function parse_line(line)

  ret = nil

  line = strtrim(line)

  if #line > 0 and line:sub(1, 1) ~= '#' then
    ll = strsplit(',', line)
      ret = {}

      for i = 1, #ll do
        parts = strsplit('=', ll[i])

        if #parts == 2 then
          p1 = strtrim(parts[1]):gsub('"', "")
          p2 = strtrim(parts[2]):gsub('"', "")

          ret[p1] = p2

        else
          ret = nil
          break
        end
      end
  end

  return ret
end



-------------------------------------------------
------------------ Start here -------------------
-------------------------------------------------



-- print(package.cpath)

argsok = false
errors = nil

-- get these from cmd line later - multiple files
-- if #arg == 4 then
--   otype = arg[1]
--   infile = arg[2]
--   outpath = arg[3]
--   ver = arg[4]

-- proc_idl name outpath file1.idl file2.idl ...
infiles = "C:/Dev/repos/c_emb_lua/test/genex.idl"
outpath = "C:/Dev/repos/c_emb_lua/test/temp"
name = "genex"

-- pd.makepath(outpath) -- make sure it exists
-- Read the file

for l in io.lines (infiles) do
  pl = parse_line(l)
  if pl ~= nil then
    for k, v in pairs(pl) do
      print(k, " = ", v)
    end
  end
end

  
--   pd.makepath(outpath) -- make sure it exists
--   -- Read the xml
--   txml = {}
--   xf = io.open(infile, "r" )
--   xs = xf:read("*all")
--   xf:close()
--   txml = xml.parse(xs)
--   bn = pp.splitext(pp.basename(infile)) -- base name for include path.

--   argsok = true -- if we got this far
  
--   if otype == "-c" then
--     gen_h()
--     gen_cpp()
--   elseif otype == "-l" then
--     gen_lua()
--   elseif otype == "-h" then
--     gen_html()
--   elseif otype == "-j" then
--     gen_json()
--   else
--     argsok = false
--   end
-- end

-- Finished
ret = ""

if errors ~= nil then
  ret = "Fail! " .. errors
end

if argsok == false then
  ret = "Fail! Args should be proc_data_dif -c|l|h|j infile outpath version"
end

return ret
