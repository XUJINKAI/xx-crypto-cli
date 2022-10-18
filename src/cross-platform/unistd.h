#pragma once

#if defined(__linux__)
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

void sleep_ms(int milliseconds);
