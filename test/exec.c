
#include <string.h>
#include <conio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "board.h"
#include "interop.h"
#include "luautils.h"
#include "exec.h"


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

/// @brief Print usage.
static void p_Usage(void);

/// Sleep for msec.
/// @param msec How long.
static void p_Sleep(int msec);

/// Safe convert a string to double.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
static bool p_StrToDouble(const char* str, double* val);

/// Safe convert a string to integer.
/// @param str The input.
/// @param val The output.
/// @return Valid conversion.
static bool p_StrToInt(const char* str, int* val);


//---------------- Public Implementation -------------//

//----------------------------------------------------//
int exec_Init(void)
{
    int stat = 0;

    // Init stuff.
    p_loop_running = false;
    p_lmain = luaL_newstate();
    luautils_EvalStack(p_lmain, 0);

    // Load std libraries.
    luaL_openlibs(p_lmain);
    luautils_EvalStack(p_lmain, 0);

    // Set up all board-specific stuff.
    stat = board_Init();
    stat = board_CliOpen(0);
    stat = board_RegDigInterrupt(p_DigInputHandler);

    // Init outputs.
    stat = board_WriteDig(DIG_OUT_1, true);
    stat = board_WriteDig(DIG_OUT_2, false);
    stat = board_WriteDig(DIG_OUT_3, true);

    p_last_usec = board_GetCurrentUsec();
    luautils_EvalStack(p_lmain, 0);

    return stat;
}

//---------------------------------------------------//
int exec_Run(const char* fn)
{
    int stat = 0;
    int lua_stat = 0;
    luautils_EvalStack(p_lmain, 0);

    // Let her rip!
    board_EnableInterrupts(true);
    p_loop_running = true;

    p_Usage();

    // Set up a second Lua thread so we can background execute the script.
    p_lscript = lua_newthread(p_lmain);
    luautils_EvalStack(p_lscript, 0);
    lua_pop(p_lmain, 1); // from lua_newthread()
    luautils_EvalStack(p_lmain, 0);

    // Open std libs.
    luaL_openlibs(p_lscript);

    // Load app stuff. This table gets pushed on the stack and into globals.
    interop_Load(p_lscript);
    luautils_EvalStack(p_lscript, 1);

    // Pop the table off the stack as it interferes with calling the module function.
    lua_pop(p_lscript, 1);
    luautils_EvalStack(p_lscript, 0);

    // Now load the script/file we are going to run.
    // lua_load() pushes the compiled chunk as a Lua function on top of the stack.
    lua_stat = luaL_loadfile(p_lscript, fn);

    // Give it some data. 
    lua_pushstring(p_lscript, "Hey diddle diddle");
    lua_setglobal(p_lscript, "script_string");
    lua_pushinteger(p_lscript, 90309);
    lua_setglobal(p_lscript, "script_int");
    luautils_EvalStack(p_lscript, 1);

    // Priming run of the loaded Lua script to create the script's global variables
    lua_stat = lua_pcall(p_lscript, 0, 0, 0);
    luautils_EvalStack(p_lscript, 0);
    if (lua_stat != LUA_OK)
    {
        common_Log(LVL_ERROR, "lua_pcall() error code %i: %s", lua_stat, lua_tostring(p_lscript, -1));
    }

    if(lua_stat == LUA_OK)
    {
        // Init the script. This also starts blocking execution.
        p_script_running = true;

        lua_getglobal(p_lscript, "do_it");
        luautils_EvalStack(p_lscript, 1);

        ///// First do some yelding. /////
        do
        {
            lua_stat = lua_resume(p_lscript, p_lmain, 0, 0);
            switch(lua_stat)
            {
                case LUA_YIELD:
                    common_Log(LVL_DEBUG, "===LUA_YIELD.");
                    break;

                case LUA_OK:
                    // Script complete now.
                    break;

                default:
                    // Unexpected error.
                    PROCESS_LUA_ERROR(p_lscript, lua_stat, "exec_Run() error");
                    break;
            }

            p_Sleep(200);
        }
        while (lua_stat == LUA_YIELD);

        ///// Then loop forever doing cli requests. /////
        do
        {
            stat = board_CliReadLine(p_cli_buf, CLI_BUFF_LEN);
            if(stat == 0 && strlen(p_cli_buf) > 0)
            {
                stat = p_ProcessCommand(p_cli_buf);
            }
            p_Sleep(100);
        } while (p_script_running);

        ///// Script complete now. /////
        board_CliWriteLine("Finished script.");
    }
    else
    {
        PROCESS_LUA_ERROR(p_lscript, lua_stat, "exec_Run() error");
    }

    ///// Done, close up shop. /////
    luautils_EvalStack(p_lmain, 0);
    luautils_EvalStack(p_lscript, 0);

    board_CliWriteLine("Goodbye - come back soon!");
    board_EnableInterrupts(false);
    lua_close(p_lmain);

    return stat;
}

//---------------- Private --------------------------//

//---------------------------------------------------//
void p_DigInputHandler(unsigned int which, bool value)
{
    interop_Hinput(p_lscript, which, value);
}

//---------------------------------------------------//
int p_ProcessCommand(const char* sin)
{
    int stat = 0;

    // What are the command line options. First one should be the actual command.
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
                valid = true;
                p_script_running = false;
                break;

            case 'c':
                if(oind == 3)
                {
                    double x = -1;
                    double y = -1;
                    double res = -1;
                    if(p_StrToDouble(opts[1], &x) && p_StrToDouble(opts[2], &y))
                    {
                        interop_Calc(p_lscript, x, y, &res);
                        board_CliWriteLine("%g + %g = %g", x, y, res);
                        valid = true;
                    }
                }
                break;

            case 'f':
                if(oind == 3)
                {
                    int pin = -1;

                    if(p_StrToInt(opts[1], &pin) && (opts[2][0] == 't' || opts[2][0] == 'f'))
                    {
                        bool value = opts[2][0] == 't';
                        interop_Hinput(p_lscript, pin, value);
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

                    if(p_StrToInt(opts[1], &pin) && (opts[2][0] == 't' || opts[2][0] == 'f'))
                    {
                        bool value = opts[2][0] == 't';
                        board_WriteDig((unsigned int)pin, value);
                        valid = true;
                    }
                }
                break;


            case 's':
                if(oind == 1)
                {
                    // Populate input to script function.
                    my_data_t din;
                    din.state = ST_IN_PROCESS;
                    din.val = 50505;
                    din.text = malloc(20);
                    sprintf(din.text, "I am a test string");

                    // Prep output from script function.
                    my_data_t dout;

                    // Make call.
                    interop_Structinator(p_lscript, &din, &dout);

                    // Do something with dout.
                    const char* state_name = "???";
                    switch(dout.state)
                    {
                        case ST_READY: state_name = "ST_READY"; break;
                        case ST_IN_PROCESS: state_name = "ST_IN_PROCESS"; break;
                        case ST_DONE: state_name = "ST_DONE"; break;
                    }
                    board_CliWriteLine("interop_Structinator rcv: val:%d state:%s text:%s", dout.val, state_name, dout.text);

                    // Clean up.
                    free(din.text);
                    free(dout.text);
                    valid = true;
                }
                break;
        }
    }

    if(!valid)
    {
        // usage
        board_CliWriteLine("Invalid cmd:%s ", sin);
        p_Usage();
        stat = 1;
    }

    return stat;
}

//---------------------------------------------------//
void p_Usage(void)
{
    board_CliWriteLine("Supported commands:");
    board_CliWriteLine("  exit: x");
    board_CliWriteLine("  tell script to run calculator: c op1 op2");
    board_CliWriteLine("  tell script about a (fake) input pin change: f pin t/f");
    board_CliWriteLine("  direct C read io pin: r pin");
    board_CliWriteLine("  direct C write io pin: w pin t/f");
    board_CliWriteLine("  write/read C struct to/from Lua: s");
}


//--------------------------------------------------------//
void p_Sleep(int msec)
{
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
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
