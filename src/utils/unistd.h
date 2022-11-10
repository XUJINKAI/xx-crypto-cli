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

#if defined(__unix__)
#define __packed           __attribute__((packed))
#define __aligned(n)       __attribute__((aligned(n)))
#define __printflike(a, b) __attribute__((format(printf, a, b)))
#define __weak             __attribute__((weak))
#define __must_check       __attribute__((warn_unused_result))
#define __unused           __attribute__((unused))
#define __used             __attribute__((used))
#define __nullend          __attribute__((sentinel))
#define __deprecated       __attribute__((deprecated))
#else
#define __packed
#define __aligned(n)
#define __printflike(a, b)
#define __weak
#define __must_check
#define __unused
#define __used
#define __nullend
#define __deprecated
#endif
