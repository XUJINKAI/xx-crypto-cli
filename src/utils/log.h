#pragma once

#include <stdio.h>

extern FILE *log_stream;
extern int log_level;

void log_content(int level, const char *file, int line, const char *func, const char *fmt, ...);

#define LOG0(...) log_content(0, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG1(...) log_content(1, __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef _DEBUG
#define LOG_DBG(...) log_content(2, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_DBG(...)
#endif
