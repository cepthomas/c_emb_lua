
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
#define CHECKED_FUNC(stat, func, ...) \
{ \
    stat = func(__VA_ARGS__); \
    if(stat != STATUS_OK) \
    { \
        LOG(LOG_ERROR, #func); \
    } \
}

/// The main Lua thread.
static lua_State* p_LMain;

/// The Lua thread where the script is running.
static lua_State* p_LScript;

/// The script execution status.
static bool p_scriptRunning = false;

/// Processing loop status.
static bool p_loopRunning;

/// Current tick count.
static int p_tick;

/// Last tick time.
static unsigned int p_lastTickTime;

/// Serial contents.
static char p_rxBuf[SER_BUFF_LEN];

/// System tick timer. Handle script yielding and serial IO.
static void p_timerHandler(void);

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_digInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param[in] sin The arbitrary command and args.
/// @return status
status_t p_processCommand(stringx_t* sin);

/// @brief Starts the script running.
/// @return status
static status_t p_startScript(void);

/// @brief Stop the currently running script.
/// @return status
static status_t p_stopScript(void);

/// @brief Common handler for lua runtime execution errors.
/// @param[in] lstat The lua status value.
/// @return status
static status_t p_processExecError(int lstat);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
status_t exec_init(void)
{
    status_t stat = STATUS_OK;

    // Init stuff.
    p_loopRunning = false;
    p_tick = 0;
    p_lastTickTime = 0;
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

        //TODO sleep() doesn't like running in win debugger.
        //sleep(5);
    }

    // Done, close up shop.
    board_enbInterrupts(false);
    p_stopScript(); // just in case

    lua_close(p_LScript);
    p_LScript = NULL;
    lua_close(p_LMain);
    p_LMain = NULL;

    return stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
status_t p_startScript()
{
//    status_t stat = STATUS_OK;

    int lstat = LUA_OK;

    // Do the real work - run the Lua script.

    // Load libraries.
    luaL_openlibs(p_LMain);
    demolib_preload(p_LMain);

    demolib_loadContext(p_LMain, "Hey diddle diddle", 90909);

    // Set up a second Lua thread so we can background execute the script.
    p_LScript = lua_newthread(p_LMain);

    // Load the script/file we are going to run.
    //int result = luaL_loadfile(p_LScript, "demoapp.lua"); TODOX use this after settling down.
    lstat = luaL_loadfile(p_LScript, "/Dev/repos/c-emb-lua/source/demoapp.lua");

    if (lstat == LUA_OK)
    {
        // Init the script. This also starts execution.
        lstat = lua_resume(p_LScript, 0, 0);
        p_scriptRunning = true;

        // A quick test. Do this after loading the file then running it.
        double d;
        demolib_luafunc_someCalc(p_LScript, 11, 22, &d);
        LOG(LOG_INFO, "demolib_luafunc_someCalc():%f", d);

        switch(lstat)
        {
            case LUA_YIELD:
                // If it is long running, it will yield and get resumed in the timer callback.
                LOG(LOG_INFO, "LUA_YIELD.");
                lstat = LUA_OK;
                break;

            case LUA_OK:
                // If it is not long running, it is complete now.
                p_scriptRunning = false;
                LOG(LOG_INFO, "Finished script.");
                break;

            default:
                // Unexpected error.
                p_processExecError(lstat);
                break;
        }
    }
    else
    {
        p_processExecError(lstat);
    }

    return lstat == LUA_OK ? STATUS_OK : STATUS_ERROR;
}

//---------------------------------------------------//
void p_timerHandler(void)
{
    // This arrives every SYS_TICK_MSEC.
    // Do the real work of the application.

    p_tick++;

    unsigned int t = common_getMsec();
    if(t - p_lastTickTime > SYS_TICK_MSEC + 1)
    {
        // TODO Missed slot?
    }
    p_lastTickTime = t;

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

            case LUA_OK:
                // It is complete now.
                p_scriptRunning = false;
                LOG(LOG_INFO, "Finished script.");
                break;

            default:
                // Unexpected error.
                p_processExecError(lstat);
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
        LOG(LOG_WARN, "Invalid cmd:%s", stringx_content(sin));
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
status_t p_processExecError(int lstat)
{
    status_t status = STATUS_OK;

    p_scriptRunning = false;

    const char* lerr = "???";
    switch(lstat)
    {
        case LUA_OK: lerr = "LUA_OK"; break;
        case LUA_YIELD: lerr = "LUA_YIELD"; break;
        case LUA_ERRRUN: lerr = "LUA_ERRRUN"; break;
        case LUA_ERRSYNTAX: lerr = "LUA_ERRSYNTAX"; break;
        case LUA_ERRMEM: lerr = "LUA_ERRMEM"; break;
        case LUA_ERRGCMM: lerr = "LUA_ERRGCMM"; break;
        case LUA_ERRERR: lerr = "LUA_ERRERR"; break;
    }

    // The error string from Lua.
    LOG(LOG_ERROR, "%s: %s", lerr, lua_tostring(p_LScript, -1));

    return status;
}
