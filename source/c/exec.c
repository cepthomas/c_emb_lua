
#include <string.h>
#include <conio.h>
#include <unistd.h>
#include "common.h"
#include "board.h"
#include "luainterop.h"
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

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
bool p_Strtoi(const char* str, int* val);

/// Sleep for msec.
/// @param msec How long.
void p_Sleep(int msec);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
int exec_Init(void)
{
    int stat = RS_PASS;

    // Init stuff.
    p_loop_running = false;
    p_lmain = luaL_newstate();
    // Load libraries.
    luaL_openlibs(p_lmain);
    li_preload(p_lmain);

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

    // Let her rip!
    board_EnableInterrupts(true);
    p_loop_running = true;

    p_Usage();

    // p_StartScript(fn);

    // Set up a second Lua thread so we can background execute the script.
    p_lscript = lua_newthread(p_lmain);
    // printf(">>>p_lmain:%p\r\n", p_lmain);
    // printf(">>>p_lscript:%p\r\n", p_lscript);

    luaL_openlibs(p_lscript);
    li_preload(p_lscript);

    // Give it data.
    my_data_t md = { 12.789, 90909, IN_PROCESS, "Hey diddle diddle" };
    li_my_data(p_lscript, &md);

    // Load the script/file we are going to run.
    int lua_stat = luaL_loadfile(p_lscript, fn);

    // TODO A quick test. Do this after loading the file then running it.
    double d;
    li_calc(p_lscript, 11, 22, &d);
    printf(">>>li_calc:%g\r\n", d);

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
                    //board_CliWriteLine("===LUA_YIELD.");
                    break;

                case LUA_OK:
                    // Script complete now.
                    break;

                default:
                    // Unexpected error.
                    li_LuaError(p_lscript, lua_stat, __LINE__, "exec_Run() error");
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
                printf("===got:%s\r\n", p_cli_buf);
                p_script_running = stat != RS_EXIT;
                stat = p_ProcessCommand(p_cli_buf);
            }

            p_Sleep(100);

        } while (p_script_running);
        

        // Script complete now.
        p_script_running = false;
        board_CliWriteLine("Finished script.");
    }
    else
    {
        li_LuaError(p_lscript, lua_stat, __LINE__, "exec_Run() error");
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
void p_DigInputHandler(unsigned int which, bool value)
{
    li_hinput(p_lscript, which, value);
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
                    if(p_Strtoi(opts[1], &x) && p_Strtoi(opts[2], &y))
                    {
                        li_calc(p_lscript, x, y, &res);
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
                    if(p_Strtoi(opts[1], &pin))
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

                    if(p_Strtoi(opts[1], &pin) && (opts[2][0] == 't' || opts[2][0] == 'f'))
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
int p_ProcessLuaError(int lua_stat)
{
    int status = RS_PASS;

    p_script_running = false;

    static char buff[20];
    switch(lua_stat)
    {

        case LUA_OK:        strcpy(buff, "LUA_OK"); break;
        case LUA_YIELD:     strcpy(buff, "LUA_YIELD"); break;
        case LUA_ERRRUN:    strcpy(buff, "LUA_ERRRUN"); break;
        case LUA_ERRSYNTAX: strcpy(buff, "LUA_ERRSYNTAX"); break;
        case LUA_ERRMEM:    strcpy(buff, "LUA_ERRMEM"); break;
        case LUA_ERRGCMM:   strcpy(buff, "LUA_ERRGCMM"); break;
        case LUA_ERRERR:    strcpy(buff, "LUA_ERRERR"); break;
        default:            snprintf(buff, 20, "%d", lua_stat); break;
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
    board_CliWriteLine("  calculator: c op1 op2");
    board_CliWriteLine("  read io pin: r pin");
    board_CliWriteLine("  write io pin: w pin t/f");
}

//--------------------------------------------------------//
bool p_Strtoi(const char* str, int* val)
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
