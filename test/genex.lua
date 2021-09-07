
-- >>>>>>>> gen lua: actually just doc - TODO or make some classes?

-- TODO The usual convention in Lua libraries is to use strings instead of numbers to select options.

state_type = {
  READY = 1,       -- Ready to be scheduled
  IN_PROCESS = 2,  -- Scheduled or running
  DONE = 3         -- All done
}
-- access like state_type.READY

-- typedef struct
-- {
--     double f1;
--     int f2;
-- } my_data_t;


-------------- ctolua -----------------

-- Dumb calculator, only does addition.
-- @param op1 One arg.
-- @param op2 Another arg.
-- @param sum The answer.
-- function calc (x, y) return sum

-- Process a digital input change.
-- @param pin The input.
-- @param value True/false.
-- void ctolua_HandleDigInput(lua_State* L, unsigned int pin, bool value);


-------------luatoc---------------

-- Doc all like this....
-- @param msec Number of milliseconds to wait.
-- @return status Function that returns state of the timer.
-- function cliwr (text)

-- function msec () return nmsec

-- function digout (pin, state)

-- function digin (pin) return state
