
#include "common.h"
#include "exec.h"

// The concept of environment changed. Only Lua functions have environments.
// To set the environment of a Lua function, use the variable _ENV or the function load.
// C functions no longer have environments. Use an upvalue with a shared table if you need to keep
// shared state among several C functions. (You may use luaL_setfuncs to open a C library with all functions sharing a common upvalue.)

/// Main entry for the application.
/// Process args and start system.
/// @param argc How many args.
/// @param argv The args.
/// @return Standard exit code.
int main(int argc, char* argv[])
{
    int ret = 0;

    if(argc == 2)
    {
        if(exec_Init() == RS_PASS)
        {
            // Blocks forever.
            if(exec_Run(argv[1]) != RS_PASS)
            {
                // Bad thing happened.
                ret = 3;
                printf("exec_run() failed\n");
            }
        }
        else
        {
            ret = 2;
            printf("exec_init() failed\n");
        }
    }
    else
    {
        ret = 1;
        printf("invalid args\n");
    }


    return ret;
}
