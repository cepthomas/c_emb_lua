

#ifndef EXEC_H
#define EXEC_H

#include "common.h"


/// @file Does the heavy lifting of the application.


/// Initialize the module.
/// @return Status.
status_t exec_init(void);

/// Enter the forever loop.
/// @return Status on exit.
status_t exec_run(void);

#endif // EXEC_H
