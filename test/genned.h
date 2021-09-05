#ifndef %namespace%_H
#define %namespace%_H

// #include <stdio.h>

// #include "common.h"


/// @brief %desc%

//---------------- Public API ----------------------//

/// $desc%
typedef enum
{
    READY = 1,      ///> Ready to be scheduled
    IN_PROCESS = 2, ///> Scheduled or running
    DONE = 3        ///> All done
} state_type_t;


/// $desc%
typedef struct
{
    double f1;
    int f2;
} my_data_t;



//--------------------------------------------------------//



#endif // %namespace%_H
