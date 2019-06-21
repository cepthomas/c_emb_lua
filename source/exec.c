
#include <string.h>
#include <conio.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#define sleep(t) Sleep(t)
#endif

#include "stringx.h"

#include "exec.h"
#include "common.h"
#include "board.h"
#include "demolib.h"



//---------------- Private --------------------------//

#define SYS_TICK_MSEC 10
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
static status_t p_startScript(void);

/// @brief Stop the currently running script.
/// @return status
static status_t p_stopScript(void);

/// @brief Common handler for lua runtime execution errors.
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
    CHECKED_FUNC(stat, board_serOpen, 0);

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

    p_startScript();

    // Forever loop.
    while(p_loopRunning && stat == STATUS_OK)
    {
        stat = board_serReadLine(p_rxBuf, SER_BUFF_LEN);

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
void p_digInputHandler(unsigned int which, bool value)
{
    demolib_handleInput(p_LScript, which, value);
}

//---------------------------------------------------//
status_t p_processCommand(stringx_t* sin)
{
    status_t stat = STATUS_OK;

    if(stringx_starts(sin, "stop", true))
    {
        p_stopScript();
    }
    // else if(stringx_starts(sin, "run", false))
    // {
    //     p_startScript();
    // }
    else
    {
        common_log(LOG_WARN, "Invalid cmd:%s", stringx_content(sin));
        stat = STATUS_WARN;
    }

    return stat;
}

//---------------------------------------------------//
status_t p_startScript()
{
    status_t stat = STATUS_OK;

    // Do the real work - run the Lua script.

    // Set up a second Lua thread so we can background execute the script.
    p_LScript = lua_newthread(p_LMain);

    CHECKED_FUNC(stat, demolib_loadLibs, p_LScript);

//    char* dir = getcwd(NULL, 0);
//    free(dir);

    // Load the script/file we are going to run. Hard coded.
    int result = luaL_loadfile(p_LScript, "demoapp.lua");

    if (result == 0)
    {
        // Start the script running.
        stat = demolib_loadContext(p_LScript, "Hey diddle diddle", 90909);
        int lstat = lua_resume(p_LScript, 0, 0);

        // A quick test.
        float f;
        stat = demolib_luafunc_someCalc(p_LScript, 11, 22, &f);

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
    else
    {
        p_processExecError();
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
    common_log(LOG_ERROR, lua_tostring(p_LScript, -1));//TODO in demolib?

    return status;
}
