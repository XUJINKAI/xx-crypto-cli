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
#endif

#define X_SUCCESS 1
#define X_FAILURE 0

#define clear_buffer gmssl_secure_clear

extern FILE *err_stream;

#define ERROR(...) fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define DEBUG(...) fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define ABORT(...)                                                                                                     \
    fprintf((err_stream ? err_stream : stderr), __VA_ARGS__);                                                          \
    abort();

#endif /* __GLOBAL_H__ */
