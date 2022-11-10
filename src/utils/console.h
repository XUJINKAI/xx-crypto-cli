#pragma once

#include "utils/terminal.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
#ifndef CP_GBK
#define CP_GBK 936
#endif

char *console_readline(tty_color color, const char *prompt);
int commandline_to_args(const char *cmd, int *retArgc, char ***retArgv);
