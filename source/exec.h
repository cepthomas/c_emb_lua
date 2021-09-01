

#ifndef EXEC_H
#define EXEC_H

#include "common.h"


/// @brief Does the heavy lifting of the application.


/// Initialize the module.
/// @return Status.
int exec_Init(void);

/// Enter the forever loop.
/// @param fn Script filename.
/// @return Status on exit.
int exec_Run(const char* fn);

#endif // EXEC_H
