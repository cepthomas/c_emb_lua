
#include <string.h>
#include <conio.h>

#ifdef WIN32
#include <windows.h>
#define sleep(t) Sleep(t)
#endif

#include "stringx.h"

#include "exec.h"
#include "common.h"
#include "board.h"
#include "xlat.h"



//---------------- Private --------------------------//

#define SYS_TICK_MSEC 10
#define SER_PORT 0
#define SER_BUFF_LEN 128

/// Helper macro.
#define CHECKED_FUNC(_stat, _func, ...) \
{ \
    _stat = _func(__VA_ARGS__); \
    if(_stat != STATUS_OK) \
    { \
        common_log(0, "%s(%d) %s", __FILE__, __LINE__, #_func); \
    } \
}

/// The main Lua context.
static lua_State* p_LMain;

/// The Lua context where the script is running.
static lua_State* p_LScript;

/// The script execution status.
static bool p_scriptRunning = false;

/// Processing loop Status.
static bool p_loopRunning;

/// Current tick.
static int p_tick;

/// Serial contents.
static char p_rxBuf[SER_BUFF_LEN];

/// System tick timer. Handle script yielding and serial IO.
static void p_timerHandler(void);

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_digInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param[in] bin The arbitrary command and args.
/// @return status
status_t p_processCommand(stringx_t* sin);

/// @brief Starts the script running.
/// @return status
static status_t p_runScript(void);

/// @brief Stop the currently running script.
/// @return status
static status_t p_stopScript(void);

/// @brief Common handler for execution errors.
/// @return status
static status_t p_processExecError(void);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
status_t exec_init(void)
{
    status_t stat = STATUS_OK;

    // Init stuff.
    p_loopRunning = false;
    p_tick = 0;
    p_LMain = luaL_newstate();

    // Init components.
    CHECKED_FUNC(stat, common_init);
    CHECKED_FUNC(stat, board_init);

    // Set up all board-specific stuff.
    CHECKED_FUNC(stat, board_regTimerInterrupt, SYS_TICK_MSEC, p_timerHandler);
    CHECKED_FUNC(stat, board_serOpen, SER_PORT);

    // Register for input interrupts.
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);

    // Init outputs.
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_1, true);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_2, false);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_3, true);

    return stat;
}

//---------------------------------------------------//
status_t exec_run(void)
{
    status_t stat = STATUS_OK;

    // Let her rip!
    board_enbInterrupts(true);
    p_loopRunning = true;
    // Prompt.
    board_serWrite(0, "\r\n>");

    ////// TODOX //////////////////
    p_runScript();
    ////////////////////////


    // Forever loop.
    while(p_loopRunning && stat == STATUS_OK)
    {
        stat = board_serReadLine(SER_PORT, p_rxBuf, SER_BUFF_LEN);

        if(strlen(p_rxBuf) > 0)
        {
            stringx_t* cmd = stringx_create(p_rxBuf);
            stat = p_processCommand(cmd);
            stringx_destroy(cmd);
        }

        //TODO sleep() doesn't like running in debugger??
        //sleep(5);
    }

    // Done, close up shop.
    board_enbInterrupts(false);
    p_stopScript(); // just in case
    lua_close(p_LMain);
    p_LMain = NULL;

    return stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
void p_timerHandler(void)
{
    // This arrives every SYS_TICK_MSEC.
    // Do the real work of the application.

    p_tick++;

    // Script stuff.
    if(p_scriptRunning && p_LScript != NULL)
    {
        // Find out where we are in the script sequence.
        int lstat = lua_status(p_LScript);

        switch(lstat)
        {
        case LUA_YIELD:
            // Still running - continue the script.
            lua_resume(p_LScript, 0, 0);
            break;

        case 0:
            // It is complete now.
            p_scriptRunning = false;
            common_log(LOG_INFO, "Finished script.");
            break;

        default:
            // Unexpected error.
            p_processExecError();
            break;
        }
    }
}

//---------------------------------------------------//
void p_digInputHandler(unsigned int which, bool value) // TODOX
{
    (void)value;

    // Real simple logic.
    switch(which)
    {
       case DIG_IN_1:
           break;

       case DIG_IN_2:
           break;

       case DIG_IN_3:
           break;

        default:
            break;
    }
}

//---------------------------------------------------//
status_t p_processCommand(stringx_t* sin)
{
    status_t stat = STATUS_OK;

    if(stringx_starts(sin, "stop", true))
    {
        p_stopScript();
    }
    else if(stringx_starts(sin, "run", false))
    {
        p_runScript();
    }
    else
    {
        common_log(LOG_WARN, "Invalid cmd:%s", stringx_content(sin));
        stat = STATUS_WARN;
    }

    return stat;
}

//---------------------------------------------------//
status_t p_runScript()
{
    status_t stat = STATUS_OK;

    // Do the real work - run the Lua script.

    // Set up a second Lua thread so we can background execute the script.
    p_LScript = lua_newthread(p_LMain);

    CHECKED_FUNC(stat, xlat_loadLibs, p_LScript);

    // Load the script/file we are going to run. Hard coded.
    int result = luaL_loadfile(p_LScript, "../demo_app.lua");

    if (result)
    {
        // If something went wrong, error message is at the top of the stack.
        const char* serr = lua_tostring(p_LScript, -1); // TODOX xlat?
        common_log(LOG_ERROR, serr);
    }
    else
    {
        // Pass the context vals to the Lua world in a table named "script_context".
        lua_newtable(p_LScript); // TODOX xlat all this?

        lua_pushstring(p_LScript, "script_string");
        lua_pushstring(p_LScript, "Hey diddle diddle");
        lua_settable(p_LScript, -3);

        lua_pushstring(p_LScript, "script_int");
        lua_pushinteger(p_LScript, 90909);
        lua_settable(p_LScript, -3);

        lua_setglobal(p_LScript, "script_context");

        // Start the script running.
        int nargs = 0;
        int lstat = lua_resume(p_LScript, 0, nargs);

        switch(lstat)
        {
            case LUA_YIELD:
                // If it is long running, it will yield and get resumed in the timer callback.
                p_scriptRunning = true;
                break;

            case 0:
                // If it is not long running, it is complete now.
                p_scriptRunning = false;
                common_log(LOG_INFO, "Finished script.");
                break;

            default:
                // Unexpected error.
                p_processExecError();
                break;
        }
    }

    return stat;
}

//---------------------------------------------------//
status_t p_stopScript()
{
    status_t status = STATUS_OK;

    p_scriptRunning = false;
    
    return status;
}

//---------------------------------------------------//
status_t p_processExecError()
{
    status_t status = STATUS_OK;

    // The Lua error string may be of one of these two forms:
    // script.lua:42: blabla
    // func:28: blabla

    p_scriptRunning = false;
    common_log(LOG_ERROR, lua_tostring(p_LScript, -1));//TODOX xlat?

    return status;
}



double callLuaFunc(lua_State* L) //TODOX
{
    // The API protocol to call a function is simple: First, you push the function to be called; second, you push the arguments to the call; then you use lua_pcall to do the actual call; finally, you pop the results from the stack.
    //
    // As an example, let us assume that our configuration file has a function like
    //
    // function f (x, y)
    //   return (x^2 * math.sin(y))/(1 - x)
    // end
    //
    // and you want to evaluate, in C, z = f(x, y) for given x and y. Assuming that you have already opened the Lua library and run the configuration file, you can encapsulate this call in the following C function:

    // call a function `f' defined in Lua 
    double z;
    int x = 99;
    int y = 101;

    // push functions and arguments
    lua_getglobal(L, "f");  // function to be called //TODO xlat this?
    lua_pushnumber(L, x);   // push 1st argument
    lua_pushnumber(L, y);   // push 2nd argument

    // do the call (2 arguments, 1 result)
    if (lua_pcall(L, 2, 1, 0) != 0)
    {
        p_processExecError();
//        error(L, "error running function `f': %s", lua_tostring(L, -1));
    }



    // retrieve result
    if (!lua_isnumber(L, -1))
    {
//        error(L, "function `f' must return a number");
    }

    z = lua_tonumber(L, -1);
    lua_pop(L, 1);  // pop returned value

    return z;

    // You call lua_pcall with the number of arguments you are passing and the number of results you want. The fourth argument indicates an error-handling function; we will discuss it in a moment. As in a Lua assignment, lua_pcall adjusts the actual number of results to what you have asked for, pushing nils or discarding extra values as needed. Before pushing the results, lua_pcall removes from the stack the function and its arguments. If a function returns multiple results, the first result is pushed first; so, if there are n results, the first one will be at index -n and the last at index -1.

    // If there is any error while lua_pcall is running, lua_pcall returns a value different from zero; moreover, it pushes the error message on the stack (but still pops the function and its arguments). Before pushing the message, however, lua_pcall calls the error handler function, if there is one. To specify an error handler function, we use the last argument of lua_pcall. A zero means no error handler function; that is, the final error message is the original message. Otherwise, that argument should be the index in the stack where the error handler function is located. Notice that, in such cases, the handler must be pushed in the stack before the function to be called and its arguments.

    // For normal errors, lua_pcall returns the error code LUA_ERRRUN. Two special kinds of errors deserve different codes, because they never run the error handler. The first kind is a memory allocation error. For such errors, lua_pcall always returns LUA_ERRMEM. The second kind is an error while Lua is running the error handler itself. In that case it is of little use to call the error handler again, so lua_pcall returns immediately with a code LUA_ERRERR.

}
