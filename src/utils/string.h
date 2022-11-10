#pragma once

#include <string.h>

#if defined(_MSC_VER)
#define strdup _strdup
#include <io.h>
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#endif

#define STREQ_Case(a, b)       (strcmp((a), (b)) == 0)
#define STREQ_NoCase(a, b)     (strcasecmp((a), (b)) == 0)
#define STREQ_CaseN(a, b, n)   (strncmp((a), (b), (n)) == 0)
#define STREQ_NoCaseN(a, b, n) (strncasecmp((a), (b), (n)) == 0)
