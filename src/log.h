#pragma once

#include <stdio.h>

extern FILE *err_stream;

#define LOG0(...) fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define LOG1(...) fprintf((err_stream ? err_stream : stderr), __VA_ARGS__)
#define LOG_ABORT(...)                                                                                                 \
    fprintf((err_stream ? err_stream : stderr), __VA_ARGS__);                                                          \
    abort();
