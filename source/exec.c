
#include <string.h>
#include <conio.h>
#include <unistd.h>

#include "common.h"
#include "board.h"
#include "luatoc.h"
#include "ctolua.h"
#include "exec.h"



//---------------- Private --------------------------//

/// Note that Windows default clock is 64 times per second = 15.625 msec.
static const int SYS_TICK_MSEC = 10;

/// Caps.
static const int MAX_NUM_OPTS = 4;

/// The main Lua thread.
static lua_State* p_lstate_main;

/// The Lua thread where the script is running.
static lua_State* p_lstate_script;

/// The script execution status.
static bool p_script_running = false;

/// Processing loop status.
static bool p_loop_running;

/// Current tick count.
static int p_tick;

/// Last tick time.
static unsigned int p_last_tick_time;

/// Serial contents.
static char p_rx_buf[SER_BUFF_LEN];

/// System tick timer. Handle script yielding and serial IO.
static void p_TimerHandler(void);

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_DigInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param sin The arbitrary command and args.
/// @return status
static status_t p_ProcessCommand(const char* sin);

/// @brief Starts the script running.
/// @param fn Script filename.
/// @return status
static status_t p_StartScript(const char* fn);

/// @brief Stop the currently running script.
/// @return status
static status_t p_StopScript(void);

/// @brief Common handler for lua runtime execution errors.
/// @param lstat The lua status value.
/// @return status
static status_t p_ProcessExecError(int lstat);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
status_t exec_Init(void)
{
    status_t stat = STATUS_OK;

    // Init stuff.
    p_loop_running = false;
    p_tick = 0;
    p_last_tick_time = 0;
    p_lstate_main = luaL_newstate();

    // Init components.
    stat = common_Init();
    stat = board_Init();

    // Set up all board-specific stuff.
    stat = board_RegTimerInterrupt(SYS_TICK_MSEC, p_TimerHandler);
    stat = board_SerOpen(0);

    // Register for input interrupts.
    stat = board_RegDigInterrupt(p_DigInputHandler);

    // Init outputs.
    stat = board_WriteDig(DIG_OUT_1, true);
    stat = board_WriteDig(DIG_OUT_2, false);
    stat = board_WriteDig(DIG_OUT_3, true);

    return stat;
}

//---------------------------------------------------//
status_t exec_Run(const char* fn)
{
    status_t stat = STATUS_OK;

    // Let her rip!
    board_EnableInterrupts(true);
    p_loop_running = true;

    p_StartScript(fn);

    // Forever loop.
    while(p_loop_running && stat == STATUS_OK)
    {
        stat = board_SerReadLine(p_rx_buf, SER_BUFF_LEN);

        if(strlen(p_rx_buf) > 0)
        {
            stat = p_ProcessCommand(p_rx_buf);
        }
    }

    // Done, close up shop.
    board_SerWriteLine("Goodbye - come back soon!");
    board_EnableInterrupts(false);

    p_StopScript(); // just in case
    lua_close(p_lstate_main);

    return stat == STATUS_EXIT ? 0 : stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
status_t p_StartScript(const char* fn)
{
    int lstat = LUA_OK;

    // Do the real work - run the Lua script.

    // Load libraries.
    luaL_openlibs(p_lstate_main);
    luatoc_Preload(p_lstate_main);
    ctolua_Context(p_lstate_main, "Hey diddle diddle", 90909);

    // Set up a second Lua thread so we can background execute the script.
    p_lstate_script = lua_newthread(p_lstate_main);

    // Load the script/file we are going to run.
    lstat = luaL_loadfile(p_lstate_script, fn);

    if(lstat == LUA_OK)
    {
        // Init the script. This also starts blocking execution.
        lstat = lua_resume(p_lstate_script, 0, 0);
        p_script_running = true;

        // A quick test. Do this after loading the file then running it.
        double d;
        ctolua_Calc(p_lstate_script, 11, 22, &d);
        // common_log(LOG_INFO, "ctolua_someCalc():%f", d);

        switch(lstat)
        {
            case LUA_YIELD:
                // If script is long running, it will yield and get resumed in the timer callback.
                // common_log(LOG_INFO, "LUA_YIELD.");
                lstat = LUA_OK;
                break;

            case LUA_OK:
                // If script is not long running, it is complete now.
                p_script_running = false;
                board_SerWriteLine("Finished script.");
                break;

            default:
                // Unexpected error.
                p_ProcessExecError(lstat);
                break;
        }
    }
    else
    {
        p_ProcessExecError(lstat);
    }

    return lstat == LUA_OK ? STATUS_OK : STATUS_ERROR;
}

//---------------------------------------------------//
void p_TimerHandler(void)
{
    // This arrives every SYS_TICK_MSEC.
    // Do the real work of the application.

    p_tick++;

    // Crude responsiveness measurement.
    unsigned int t = common_GetMsec(); // can range from 15 to ~ 32
    if(t - p_last_tick_time > 2 * SYS_TICK_MSEC)
    {
        //common_log("Tick seems to have taken too long:%d", t - p_lastTickTime);
    }
    p_last_tick_time = t;

    // Script stuff.
    if(p_script_running && p_lstate_script != NULL)
    {
        // Find out where we are in the script sequence.
        int lstat = lua_status(p_lstate_script);

        switch(lstat)
        {
            case LUA_YIELD:
                // Still running - continue the script.
                lua_resume(p_lstate_script, 0, 0);
                break;

            case LUA_OK:
                // It is complete now.
                p_script_running = false;
                board_SerWriteLine("Finished script.");
                break;

            default:
                // Unexpected error.
                p_ProcessExecError(lstat);
                break;
        }
    }
}

//---------------------------------------------------//
void p_DigInputHandler(unsigned int which, bool value)
{
    ctolua_HandleInput(p_lstate_script, which, value);
}

//---------------------------------------------------//
status_t p_ProcessCommand(const char* sin)
{
    status_t stat = STATUS_OK;

    // What are the options.
    char* opts[MAX_NUM_OPTS];
    memset(opts, 0x00, sizeof(opts));
    int oind = 0;

    // Make writable copy and tokenize it.
    char cp[strlen(sin) + 1];
    strcpy(cp, sin);
    char* token = strtok(cp, " ");

    while(token != NULL && oind < MAX_NUM_OPTS)
    {
        opts[oind++] = token;
        token = strtok(NULL, " ");
    }

    bool valid = false; // default
    if(oind > 0)
    {
        switch(opts[0][0])
        {
            case 'x':
                p_StopScript();
                valid = true;
                stat = STATUS_EXIT;
                break;

            case 'c':
                if(oind == 3)
                {
                    int x = -1;
                    int y = -1;
                    double res = -1;
                    common_Strtoi(opts[1], &x);
                    common_Strtoi(opts[2], &y);
                    ctolua_Calc(p_lstate_script, x, y, &res);
                    board_SerWriteLine("%d + %d = %g", x, y, res);
                    valid = true;
                }
                break;

            case 'r':
                if(oind == 2)
                {
                    int pin = -1;
                    bool value;
                    common_Strtoi(opts[1], &pin);
                    board_ReadDig((unsigned int)pin, &value);
                    board_SerWriteLine("read pin:%d = %d", pin, value);
                    valid = true;
                }
                break;

            case 'w':
                if(oind == 3)
                {
                    int pin = -1;
                    bool value;
                    common_Strtoi(opts[1], &pin);
                    value = opts[2][0] == 't';
                    board_WriteDig((unsigned int)pin, value);
                    board_SerWriteLine("write pin:%d = %d", pin, value);
                    ctolua_HandleInput(p_lstate_script, (unsigned int)pin, value);
                    valid = true;
                }
                break;
        }
    }

    if(!valid)
    {
        // usage
        board_SerWriteLine("Invalid cmd:%s, try one of these:", sin);
        board_SerWriteLine("  exit: x");
        board_SerWriteLine("  calculator: c num1 num2");
        board_SerWriteLine("  read io pin: r pin");
        board_SerWriteLine("  write io pin: w pin val");
    }

    return stat;
}

//---------------------------------------------------//
status_t p_StopScript()
{
    status_t status = STATUS_OK;

    p_script_running = false;
    
    return status;
}

//---------------------------------------------------//
status_t p_ProcessExecError(int lstat)
{
    status_t status = STATUS_OK;

    p_script_running = false;

    static char buff[20];
    switch(lstat)
    {

        case LUA_OK:        strcpy(buff, "LUA_OK"); break;
        case LUA_YIELD:     strcpy(buff, "LUA_YIELD"); break;
        case LUA_ERRRUN:    strcpy(buff, "LUA_ERRRUN"); break;
        case LUA_ERRSYNTAX: strcpy(buff, "LUA_ERRSYNTAX"); break;
        case LUA_ERRMEM:    strcpy(buff, "LUA_ERRMEM"); break;
        case LUA_ERRGCMM:   strcpy(buff, "LUA_ERRGCMM"); break;
        case LUA_ERRERR:    strcpy(buff, "LUA_ERRERR"); break;
        default: snprintf(buff, 20, "%d", lstat); break;
    }

    // The error string from Lua.
    board_SerWriteLine("%s: %s", buff, lua_tostring(p_lstate_script, -1));

    return status;
}
