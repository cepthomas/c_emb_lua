
#include <string.h>

#include "stringx.h"

#include "exec.h"
#include "common.h"
#include "board.h"
#include "xlat.h"



//---------------- Private --------------------------//

#define SYS_TICK_MSEC 10
#define SER_PORT 0
#define SER_BUFF_LEN 128

/// Current script contents. TODO need to support multiple script files.
static stringx_t* p_scriptText;

/// The main Lua context.
static lua_State* p_LMain;

/// The Lua context where the script is running.
static lua_State* p_LScript;

/// The script execution status.
static bool p_scriptRunning = false;

/// Processing loop Status.
static bool p_loopRunning;

/// Serial receive buffer.
static char p_rxBuf[SER_BUFF_LEN];

/// Serial send buffer.
static char p_txBuf[SER_BUFF_LEN];

/// Current tick.
static int p_tick;


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

    // Init memory.
    memset(p_rxBuf, 0x00, sizeof(p_rxBuf));
    memset(p_txBuf, 0x00, sizeof(p_txBuf));

    p_loopRunning = false;
    p_tick = 0;

    // Init modules.
    common_setLogLevel(2);
    CHECKED_FUNC(stat, common_init);
    CHECKED_FUNC(stat, board_init);
    //CHECKED_FUNC(stat, xlat_init);

    // Set up all board-specific stuff.
    CHECKED_FUNC(stat, board_regTimerInterrupt, SYS_TICK_MSEC, p_timerHandler);
    CHECKED_FUNC(stat, board_serOpen, SER_PORT);

    // Register for input interrupts.
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);

    // Init outputs.
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_1, true);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_2, false);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_3, true);

    return stat;
}

//--------------------------------------------------------//
status_t exec_run(void)
{
    status_t stat = STATUS_OK;

    // Let her rip!
    board_enbInterrupts(true);
    p_loopRunning = true;

    while(p_loopRunning)
    {
        // Forever loop.
    }

    board_enbInterrupts(false);

    /// Done, do any cleanup.
    p_stopScript(); // just in case
    lua_close(p_LMain);
    p_LMain = NULL;


    return stat;
}


//---------------- Private --------------------------//

//--------------------------------------------------------//
void p_timerHandler(void)
{
    // This arrives every SYS_TICK_MSEC.
    // Do the real work of the application.

    status_t stat = STATUS_OK;

    p_tick++;

    if(p_tick % 5 == 0)
    {
        // Poll serial.
        CHECKED_FUNC(stat, board_serReadLine, SER_PORT, p_rxBuf, SER_BUFF_LEN);

        if(strlen(p_rxBuf) > 0)
        {
            // Got something. TODO ser like CHECKED_FUNC(stat, cli_process, p_rxBuf, p_txBuf);
            CHECKED_FUNC(stat, board_serWriteLine, SER_PORT, p_txBuf);
        }
    }


    // Script stuff.
    if(p_scriptRunning && p_LScript != NULL)
    {
        // Find out where we are in the script sequence.
        int lstat = lua_status(p_LScript);

        switch(lstat)
        {
        case LUA_YIELD:
            // Still running.
            //LOG(tex_TL_DETAIL, "Continue yielding");
            // Continue the script.
            lua_resume(p_LScript, 0, 0);
            break;

        case 0:
            /// It is complete now.
            p_scriptRunning = false;
            common_log(LOG_INFO, "Finished script.");
            break;

        default:
            /// Unexpected error.
            SPX p_processExecError();
            break;
        }
    }
}

//--------------------------------------------------------//
void p_digInputHandler(unsigned int which, bool value)
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

#define CMD_LEN 4

//--------------------------------------------------------//
status_t p_processCommand(stringx_t* sin)
{
    status_t stat = STATUS_OK;

    if(stringx_len(sin) >= CMD_LEN)
    {
        stringx_t* cmd = stringx_left(sin, CMD_LEN);

        if(stringx_compare(cmd, "stop", false) == 0)
        {
            p_stopScript();
        }
        else if(stringx_compare(cmd, "runn", false) == 0)
        {
            p_runScript();
        }
        else if(stringx_compare(cmd, "load ", false) == 0)
        {
            // Load the  contents into our buffer.
            p_scriptText = stringx_copy(sin);
        }
        else
        {
            common_log(LOG_WARN, "Invalid cmd:%s", cmd);
            stat = STATUS_WARN;
        }
    }
    else
    {
        common_log(LOG_WARN, "Invalid cmd length");
        stat = STATUS_WARN;
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_runScript()
{
    status_t stat = STATUS_OK;

    //LOG(tex_TL_INFO, QString("Starting scriptFileName:%1 reportFileName:%2 dryRun:%3 failCnt:%4").arg(scriptFileName).arg(reportFileName).arg(dryRun).arg(failCnt));

    // Do the real work - run the Lua script.
    int result = 0;

    // Set up a second Lua thread so we can background execute the script.
    p_LScript = lua_newthread(p_LMain);

    CHECKED_FUNC(stat, xlat_loadLibs, p_LScript);

    // Load the script we are going to run.
    result = luaL_loadstring(p_LScript, stringx_content(p_scriptText));

    if (result)
    {
        // If something went wrong, error message is at the top of the stack.
        // LOG_ERR(TEX_SCRIPT_LOAD_ERR, QString("Script load error:%1:%2").arg(result).arg(lua_tostring(p_LScript, -1)));
    }
    else
    {
        // Pass the context vals to the Lua world in a table named "script_context".
        lua_newtable(p_LScript);

       // lua_pushstring(p_LScript, "report_filename");
       // lua_pushstring(p_LScript, reportFileName);
       // lua_settable(p_LScript, -3);

       // lua_pushstring(p_LScript, "dry_run");
       // lua_pushboolean(p_LScript, dryRun);
       // lua_settable(p_LScript, -3);

       // lua_pushstring(p_LScript, "fail_count");
       // lua_pushinteger(p_LScript, failCnt);
       // lua_settable(p_LScript, -3);

        lua_setglobal(p_LScript, "script_context");

        // Start the script running.
        int nargs = 0;
        int lstat = lua_resume(p_LScript, 0, nargs);

        switch(lstat)
        {
        case LUA_YIELD:
            // If it is long running, it will yield and get resumed in the timer callback.
            p_scriptRunning = true;
            // LOG(tex_TL_INFO, "Yielding");
            break;

        case 0:
            // If it is not long running, it is complete now.
            p_scriptRunning = false;
            common_log(LOG_INFO, "Finished script.");
            break;

        default:
            // Unexpected error.
            SPX p_processExecError();
            break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_stopScript()
{
    status_t status = STATUS_OK;
    p_scriptRunning = false;
    return status;
}

//--------------------------------------------------------//
status_t p_processExecError()
{
    status_t status = STATUS_OK;

    // The Lua error string may be of one of these two forms:
    // tt_dev_test_1.lua:42: blabla
    // do_errors:28: blabla

    p_scriptRunning = false;
    common_log(LOG_ERROR, lua_tostring(p_LScript, -1));

    return status;
}
