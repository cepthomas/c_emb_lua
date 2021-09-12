
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//



//---------------- Public Implementation -------------//

//--------------------------------------------------------//
bool common_StrToDouble(const char* str, double* val)
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
bool common_StrToInt(const char* str, int* val)
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
