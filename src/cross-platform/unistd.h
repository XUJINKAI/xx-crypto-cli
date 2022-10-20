#pragma once

#if defined(__linux__)
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#define isatty _isatty
#endif

void sleep_ms(int milliseconds);
