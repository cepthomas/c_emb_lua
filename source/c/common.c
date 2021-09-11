
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "board.h"


//---------------- Private --------------------------//


//---------------- Public Implementation -------------//

double common_CurrentSec()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    double sec = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    return sec;
}