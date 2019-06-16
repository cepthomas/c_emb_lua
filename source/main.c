
#include "common_module.h"
#include "exec_module.h"


/// Main entry for the real application.
/// Process args and start system.
/// @param argc How many args.
/// @param argv The args.
/// @return Standard exit code.
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    int ret = 0;

    if(exec_init() == STATUS_OK)
    {
        // Blocks forever.
        if(exec_run() != STATUS_OK)
        {
            // Bad thing happened.
            ret = 2;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
