

#ifndef EXEC_H
#define EXEC_H

#include "common.h"


/// @brief Does the heavy lifting of the application.


/// Initialize the module.
/// @return Status.
status_t exec_init(void);

/// Enter the forever loop.
/// @param fn Script filename.
/// @return Status on exit.
status_t exec_run(const char* fn);

#endif // EXEC_H
