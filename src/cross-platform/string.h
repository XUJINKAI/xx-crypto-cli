#pragma once

#include <string.h>

#if defined(_MSC_VER)
#include <io.h>
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#endif
