

#ifndef EXEC_MODULE_H
#define EXEC_MODULE_H

#include "common_module.h"


/// @file Interface to the exec module.


//---------------- Public API ----------------------//


//---------------- Definitions ---------------------//


//---------------- Functions ---------------------//

/// Initialize the module.
/// @return Status.
status_t exec_init(void);

/// Enter the forever loop.
/// @return Status on exit.
status_t exec_run(void);

/// Stop running.
/// @return Status.
//status_t exec_exit(void);

/// Clean up module resources.
/// @return Status.
status_t exec_destroy(void);


#endif // EXEC_MODULE_H
