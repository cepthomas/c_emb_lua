
#ifndef XLAT_MODULE_H
#define XLAT_MODULE_H

#include "common_module.h"

/// @file Provides translation layer between lua and C.


//---------------- General Functions -----------------//

/// Initialize the module.
/// @return Status.
status_t xlat_init(void);

/// Load the libs.
/// @param L Lua environment.
/// @return Status.
status_t xlat_loadLibs(lua_State* L);

/// Clean up module resources.
/// @return Status.
status_t xlat_destroy(void);


#endif // XLAT_MODULE_H
