#pragma once

#include "global.h"

#define TC_RED     "\033[31m"
#define TC_GREEN   "\033[32m"
#define TC_YELLOW  "\033[33m"
#define TC_BLUE    "\033[34m"
#define TC_MAGENTA "\033[35m"
#define TC_CYAN    "\033[36m"
#define TC_WHITE   "\033[37m"
#define TC_BOLD    "\033[1m"
#define TC_RESET   "\033[0m"

#define tty_puts(fp, str)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (isatty(fileno(fp)))                                                                                        \
        {                                                                                                              \
            fputs(str, fp);                                                                                            \
        }                                                                                                              \
    } while (0)

#define printf_color(fp, color, fmt, ...)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        tty_puts(color);                                                                                               \
        fprintf(fp, fmt, __VA_ARGS__);                                                                                 \
        tty_puts(TC_RESET);                                                                                            \
    } while (0)

#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
#ifndef CP_GBK
#define CP_GBK 936
#endif

char *console_readline(const char *prompt);
int commandline_to_args(const char *cmd, int *retArgc, char ***retArgv);
