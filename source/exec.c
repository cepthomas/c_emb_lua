
#include <string.h>
#include <conio.h>
#include <unistd.h>

#include "common.h"
#include "board.h"
#include "luainterop.h"
#include "exec.h"

#include "luaconf.h"


//---------------- Private --------------------------//

/// Note that Windows default clock is 64 times per second = 15.625 msec.
static const int SYS_TICK_MSEC = 10;

/// Caps.
static const int MAX_NUM_OPTS = 4;

/// The main Lua thread.
static lua_State* p_lmain;

/// The Lua thread where the script is running.
static lua_State* p_lscript;

/// The script execution status.
static bool p_script_running = false;

/// Processing loop status.
static bool p_loop_running;

/// Current tick count.
static int p_tick;

/// Last tick time.
static unsigned int p_last_tick_time;

/// CLI contents.
static char p_cli_buf[CLI_BUFF_LEN];

/// System tick timer. Handles coroutine yielding and CLI.
static void p_TimerHandler(void);

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_DigInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param sin The arbitrary command and args.
/// @return status
static int p_ProcessCommand(const char* sin);

/// @brief Starts the script running.
/// @param fn Script filename.
/// @return status
static int p_StartScript(const char* fn);

/// @brief Stop the currently running script.
/// @return status
static int p_StopScript(void);

/// @brief Common handler for lua runtime execution errors.
/// @param lstat The lua status value.
/// @return status
static int p_ProcessExecError(int lstat);

/// @brief Print usage.
static void p_Usage(void);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
int exec_Init(void)
{
    int stat = RS_PASS;

    // Init stuff.
    p_loop_running = false;
    p_tick = 0;
    p_last_tick_time = 0;
    p_lmain = luaL_newstate();

    // Init components.
    stat = common_Init(); // TODO handle all errors.
    stat = board_Init();

    // Set up all board-specific stuff.
    stat = board_RegTimerInterrupt(SYS_TICK_MSEC, p_TimerHandler);
    stat = board_CliOpen(0);

    // Register for input interrupts.
    stat = board_RegDigInterrupt(p_DigInputHandler);

    // Init outputs.
    stat = board_WriteDig(DIG_OUT_1, true);
    stat = board_WriteDig(DIG_OUT_2, false);
    stat = board_WriteDig(DIG_OUT_3, true);

    return stat;
}

//---------------------------------------------------//
int exec_Run(const char* fn)
{
    int stat = RS_PASS;

    // Let her rip!
    board_EnableInterrupts(true);
    p_loop_running = true;

    p_Usage();

    p_StartScript(fn);

    // A quick test. Do this after loading the file then running it.
    // double d;
    // ctolua_Calc(p_lscript, 11, 22, &d);
    // printf(">>>%g\r\n", d);


    // Forever loop.
    while(p_loop_running && stat == RS_PASS)
    {
        stat = board_CliReadLine(p_cli_buf, CLI_BUFF_LEN);

        if(stat == RS_PASS && strlen(p_cli_buf) > 0)
        {
            stat = p_ProcessCommand(p_cli_buf);
        }
    }

    // Done, close up shop.
    board_CliWriteLine("Goodbye - come back soon!");
    board_EnableInterrupts(false);

    p_StopScript(); // just in case
    lua_close(p_lmain);

    return stat == RS_EXIT ? 0 : stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
int p_StartScript(const char* fn)
{
    int lstat = LUA_OK;

    // Do the real work - run the Lua script.

    // Load libraries.
    luaL_openlibs(p_lmain);
    luainterop_Preload(p_lmain);
    ctolua_Context(p_lmain, "Hey diddle diddle", 90909);

    // Set up a second Lua thread so we can background execute the script.
    p_lscript = lua_newthread(p_lmain);

    // Load the script/file we are going to run.
    lstat = luaL_loadfile(p_lscript, fn);

    if(lstat == LUA_OK)
    {
        // Init the script. This also starts blocking execution.
        lstat = lua_resume(p_lscript, 0, 0);
        p_script_running = true;

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
                board_CliWriteLine("Finished script.");
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

    return lstat == LUA_OK ? RS_PASS : RS_ERR;
}

//---------------------------------------------------//
void p_TimerHandler(void)
{
    // Do the periodic real work of the application.
    p_tick++;

    // Crude responsiveness measurement. Win timer is sloppy.
    unsigned int t = common_GetMsec(); // can range from 15 to ~32
    // unsigned int dur = t - p_last_tick_time;
    p_last_tick_time = t;

    // Script stuff, coroutine handling.
    if(p_script_running && p_lscript != NULL)
    {
        // Find out where we are in the script sequence.
        int lstat = lua_status(p_lscript);

        switch(lstat)
        {
            case LUA_YIELD:
                // Script is still running - continue.
                lua_resume(p_lscript, 0, 0);
                break;

            case LUA_OK:
                // Script is complete now.
                p_script_running = false;
                board_CliWriteLine("Finished script.");
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
    ctolua_HandleDigInput(p_lscript, which, value);
}

//---------------------------------------------------//
int p_ProcessCommand(const char* sin)
{
    int stat = RS_PASS;

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
                stat = RS_EXIT;
                break;

            case 'c':
                if(oind == 3)
                {
                    int x = -1;
                    int y = -1;
                    double res = -1;
                    if(common_Strtoi(opts[1], &x) && common_Strtoi(opts[2], &y))
                    {
                        ctolua_Calc(p_lscript, x, y, &res);
                        board_CliWriteLine("%d + %d = %g", x, y, res);
                        valid = true;
                    }
                }
                break;

            case 'r':
                if(oind == 2)
                {
                    int pin = -1;
                    bool bval;
                    if(common_Strtoi(opts[1], &pin))
                    {
                        board_ReadDig((unsigned int)pin, &bval);
                        board_CliWriteLine("read pin:%d = %s", pin, bval ? "t" : "f");
                        valid = true;
                    }
                }
                break;

            case 'w':
                if(oind == 3)
                {
                    int pin = -1;
                    bool value;

                    if(common_Strtoi(opts[1], &pin) && (opts[2][0] == 't' || opts[2][0] == 'f'))
                    {
                        value = opts[2][0] == 't';
                        board_WriteDig((unsigned int)pin, value);
                        //board_CliWriteLine("write pin:%d = %d", pin, value);
                        //ctolua_HandleDigInput(p_lscript, (unsigned int)pin, value);
                        valid = true;
                        
                    }
                }
                break;
        }
    }

    if(!valid)
    {
        // usage
        board_CliWriteLine("Invalid cmd:%s: ", sin);
        p_Usage();
    }

    return stat;
}

//---------------------------------------------------//
int p_StopScript()
{
    int status = RS_PASS;

    p_script_running = false;
    
    return status;
}

//---------------------------------------------------//
int p_ProcessExecError(int lstat)
{
    int status = RS_PASS;

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
        default:            snprintf(buff, 20, "%d", lstat); break;
    }

    // The error string from Lua.
    board_CliWriteLine("%s: %s", buff, lua_tostring(p_lscript, -1));

    return status;
}

//---------------------------------------------------//
void p_Usage(void)
{
    board_CliWriteLine("Supported commands:");
    board_CliWriteLine("  exit: x");
    board_CliWriteLine("  calculator: c #1 #2");
    board_CliWriteLine("  read io pin: r #");
    board_CliWriteLine("  write io pin: w # t/f");
}
