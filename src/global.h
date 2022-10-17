#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "gmssl/mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__GLIBC__)
#include <unistd.h>
#endif

#if defined(_MSC_VER)
#include <io.h>
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

#define X_SUCCESS 1
#define X_FAILURE 0

#define clear_buffer gmssl_secure_clear

extern FILE *err_stream;

#define ERROR(...)   fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define VERBOSE(...) fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define ABORT(...)                                                                                                     \
    fprintf((err_stream ? err_stream : stderr), __VA_ARGS__);                                                          \
    abort();

#endif /* __GLOBAL_H__ */
