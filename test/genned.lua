
-- >>>>>>>> gen lua: actually just doc

-- TODO The usual convention in Lua libraries is to use strings instead of numbers to select options.
-- state_type =
-- {
--   READY = 1,      -- Ready to be scheduled
--   IN_PROCESS = 2, -- Scheduled or running
--   DONE = 3        -- All done
-- }


state_type = {
  READY = 1,       -- Ready to be scheduled
  IN_PROCESS = 2,  -- Scheduled or running
  DONE = 3         -- All done
}
-- access like state_type.READY


-- >>>>>> gen lua: or just doc?????
-- function calc (x, y) server_handler (x, y) end


-- >>>>>> gen lua: or just doc?????
-- function digin (x, y) server_handler (x, y) end

