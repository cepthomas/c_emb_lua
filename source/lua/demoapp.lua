--[[
Lua script for a simplistic coroutine application.
--]]


local li = require "luainterop" -- c module
local ut = require "utils"
local math = require "math"

-- print("*** script 1")

-- This is the same as the C type.
state_type = {
  [1] = 'READY',       -- Ready to be scheduled
  [2] = 'IN_PROCESS',  -- Scheduled or running
  [3] = 'DONE'         -- All done
}

------------------------- Main loop ----------------------------------------------------

function do_it()
  li.cliwr("demoapp: module initialization")

  -- for n in pairs(_G) do print(n) end

  -- Process the data passed from C. my_static_data contains the equivalent of my_static_data_t.
  state_name = state_type[my_static_data.state]
  slog = string.format ("my_static_data f1:%g f2:%d state:%s f3:%s", my_static_data.f1, my_static_data.f2, state_name, my_static_data.f3)
  li.cliwr(slog)

  -- Start working.
  li.cliwr("demoapp: do some pretend script work then yield")

  for i = 1, 5 do
    li.cliwr("demoapp: doing loop number " .. i)

    -- Do pretend work.
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
    -- ut.sleep(200)

    -- Plays well with others.
    coroutine.yield()
  end
  li.cliwr("demoapp: done loop")
end


-------------- Handlers for commands from C --------------------------

-- Pin input has arrived from board via c.
function hinput (pin, value) -- TODO force from cli
  li.cliwr(string.format("demoapp: got hinput pin:%d value:%s ", pin, tostring(value)))
end

-- Dumb calculator, only does addition.
function calc (x, y)
  return (x + y)
end
