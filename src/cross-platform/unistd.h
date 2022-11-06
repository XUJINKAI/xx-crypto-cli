#pragma once

#if defined(__linux__)
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>

#include <io.h>
#define isatty _isatty

#include <process.h>
#define getpid _getpid
#endif

void sleep_ms(int milliseconds);
