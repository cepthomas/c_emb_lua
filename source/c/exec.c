
#include <string.h>
#include <conio.h>
#include <unistd.h>
#include "common.h"
#include "board.h"
#include "luainterop.h"
#include "logger.h"
#include "exec.h"

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

//---------------- Private --------------------------//

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

/// Last tick time.
static uint64_t p_last_usec;

/// CLI contents.
static char p_cli_buf[CLI_BUFF_LEN];

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_DigInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param sin The arbitrary command and args.
/// @return status
static int p_ProcessCommand(const char* sin);

/// @brief Stop the currently running script.
/// @return status
static int p_StopScript(void);

/// @brief Print usage.
static void p_Usage(void);

/// Safe convert a string to double.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool p_StrToDouble(const char* str, double* val);

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool p_StrToInt(const char* str, int* val);

/// Sleep for msec.
/// @param msec How long.
void p_Sleep(int msec);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
int exec_Init(void)
{
    int stat = RS_PASS;

    // Init stuff.
    logger_Init("C:\\Dev\\repos\\c_emb_lua\\_log.txt");
    logger_SetFilters(LVL_DEBUG);
    p_loop_running = false;
    p_lmain = luaL_newstate();
    // Load libraries.
    luaL_openlibs(p_lmain);
    // iop_Preload(p_lmain);

    p_last_usec = board_GetCurrentUsec();

    // Init components.
    stat = board_Init();

    // Set up all board-specific stuff.
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
    int lua_stat = 0;

    // Let her rip!
    board_EnableInterrupts(true);
    p_loop_running = true;

    p_Usage();

    // was p_StartScript(fn);

    // Set up a second Lua thread so we can background execute the script.
    p_lscript = lua_newthread(p_lmain);
    LOG_DEBUG("p_lmain:%p", p_lmain);
    LOG_DEBUG("p_lscript:%p", p_lscript);
    iop_DumpStack(p_lscript, "lua_newthread() - stack:empty");

    // Load the script/file we are going to run. lua_load() pushes the compiled chunk as a Lua function on top of the stack.
    lua_stat = luaL_loadfile(p_lscript, fn); //luaL_dofile??
    iop_DumpStack(p_lscript, "luaL_loadfile() - stack:function");

    // Open all std libs.
    luaL_openlibs(p_lscript);
    iop_DumpStack(p_lscript, "luaL_openlibs() - stack:function");

    // Load my stuff. This table gets pushed on the stack and global.
    iop_Preload(p_lscript);
    iop_DumpStack(p_lscript, "iop_Preload() - stack:table, function");


    lua_pushglobaltable(p_lscript);         // Get global table
    lua_pushnil(p_lscript);                 // put a nil key on stack
    while (lua_next(p_lscript,-2) != 0)     // key(-1) is replaced by the next key(-1) in table(-2)
    {
        const char* name = lua_tostring(p_lscript, -2);   // Get key(-2) name
        LOG_DEBUG("GLOBAL:%s", name);
        lua_pop(p_lscript,1);                 // remove value(-1), now key on top at(-1)
    }
    lua_pop(p_lscript,1);                   // remove global table(-1)



    int ll = lua_getglobal(p_lscript, "luainterop");
    iop_DumpStack(p_lscript, "lua_getglobal(calc) - stack:function, table, function");

    // Pop the table of the stack as it interferes with calling the module function.
    lua_pop(p_lscript, 1);
    iop_DumpStack(p_lscript, "lua_pop() - stack:function");

    // if the stack has n elements, then index 1 represents the first element (that is, the element that was pushed onto 
    // the stack first) and index n represents the last element; index -1 also represents the last element (that is, the 
    // element at the top) and index -n represents the first element.


    // Give it data.
    my_data_t md = { 12.789, 90909, IN_PROCESS, "Hey diddle diddle" };
    iop_SetGlobalMyData(p_lscript, &md, "my_data"); // TODO pass as arg instead.
    iop_DumpStack(p_lscript, "iop_SetGlobalMyData() - stack:function");

    // Priming run of the loaded Lua script to create the script's global variables
    //lua_stat = lua_pcall(p_lscript, 0, 0, 0);
    //if (lua_stat != 0)
    // {
    //     LOG_ERROR("lua_pcall() error code %i: %s", lua_stat, lua_tostring(p_lscript, -1));
    // }
    //or?...
    // lua_stat = lua_resume(p_lscript, 0, 0);
    // if (lua_stat >= LUA_ERRRUN)
    // {
    //     LOG_ERROR("lua_resume() error code %i: %s", lua_stat, lua_tostring(p_lscript, -1));
    // }

    // ??? int lua_pcallk (lua_State *L, int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k);

    // You call lua_resume on a thread returned by lua_newthread, not lua_newstate.
    // So in your code you would either have to change the first lua_resume to lua_(p)call:
    // or swap luaL_loadfile for luaL_dofile:

    // If you're in a C function that has been called from Lua, and you want to know what thread you're in... just call lua_pushthread(L).
    // Pushes the thread represented by L onto the stack. Returns 1 if this thread is the main thread of its state.

    if(lua_stat == LUA_OK)
    {
        // Init the script. This also starts blocking execution.
        p_script_running = true;

        do
        {
            lua_stat = lua_resume(p_lscript, 0, 0);

            switch(lua_stat)
            {
                case LUA_YIELD:
                    LOG_DEBUG("===LUA_YIELD.");
                    break;

                case LUA_OK:
                    // Script complete now.
                    break;

                default:
                    // Unexpected error.
                    iop_LuaError(p_lscript, lua_stat, __LINE__, "exec_Run() error");
                    break;
            }

            p_Sleep(100);
        }
        while (lua_stat == LUA_YIELD);

        do
        {
            ///// Check cli. /////
            stat = board_CliReadLine(p_cli_buf, CLI_BUFF_LEN);
            if(stat == RS_PASS && strlen(p_cli_buf) > 0)
            {
                // LOG_DEBUG("|||got:%s", p_cli_buf);
                p_script_running = stat != RS_EXIT;
                stat = p_ProcessCommand(p_cli_buf);
            }
            p_Sleep(100);

        } while (p_script_running);

        p_ProcessCommand("c 1 2");//=================================TODO test

        // Script complete now.
        p_script_running = false;
        board_CliWriteLine("Finished script.");
    }
    else
    {
        iop_LuaError(p_lscript, lua_stat, __LINE__, "exec_Run() error");
    }

    // Done, close up shop.
    board_CliWriteLine("Goodbye - come back soon!");
    board_EnableInterrupts(false);

    p_StopScript(); // just in case
    lua_close(p_lscript);
    lua_close(p_lmain);

    return stat == RS_EXIT ? 0 : stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
void p_DigInputHandler(unsigned int which, bool value)
{
    iop_Hinput(p_lscript, which, value);
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
                    double x = -1;
                    double y = -1;
                    double res = -1;
                    if(p_StrToDouble(opts[1], &x) && p_StrToDouble(opts[2], &y))
                    {
                        iop_Calc(p_lscript, x, y, &res);
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
                    if(p_StrToInt(opts[1], &pin))
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

                    if(p_StrToInt(opts[1], &pin) && (opts[2][0] == 't' || opts[2][0] == 'f'))
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
void p_Usage(void)
{
    board_CliWriteLine("Supported commands:");
    board_CliWriteLine("  exit: x");
    board_CliWriteLine("  calculator: c op1 op2");
    board_CliWriteLine("  read io pin: r pin");
    board_CliWriteLine("  write io pin: w pin t/f");
}

//--------------------------------------------------------//
bool p_StrToDouble(const char* str, double* val)
{
    bool valid = true;
    char* p;

    errno = 0;
    *val = strtof(str, &p);
    if(errno == ERANGE)
    {
        // Mag is too large.
        valid = false;
    }
    else if(p == str)
    {
        // Bad string.
        valid = false;
    }

    return valid;
}

//--------------------------------------------------------//
bool p_StrToInt(const char* str, int* val)
{
    bool valid = true;
    char* p;

    errno = 0;
    *val = strtol(str, &p, 10);
    if(errno == ERANGE)
    {
        // Mag is too large.
        valid = false;
    }
    else if(p == str)
    {
        // Bad string.
        valid = false;
    }

    return valid;
}

//--------------------------------------------------------//
void p_Sleep(int msec)
{
#ifdef WIN32
    Sleep(msec);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (msec >= 1000)
      sleep(msec / 1000);
    usleep((msec % 1000) * 1000);
#endif
}
