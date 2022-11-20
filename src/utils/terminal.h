#pragma once

#include <stdio.h>

#define TC_RESET      "\033[0m"
#define TC_BOLD       "\033[1m"
#define TC_DIM        "\033[2m"
#define TC_ITALIC     "\033[3m"
#define TC_UNDERSCORE "\033[4m"
#define TC_REVERSE    "\033[7m"
#define TC_HIDDEN     "\033[8m"
#define TC_STRIKE     "\033[9m"

#define TC_BLACK   "\033[30m"
#define TC_RED     "\033[31m"
#define TC_GREEN   "\033[32m"
#define TC_YELLOW  "\033[33m"
#define TC_BLUE    "\033[34m"
#define TC_MAGENTA "\033[35m"
#define TC_CYAN    "\033[36m"
#define TC_WHITE   "\033[37m"

#define TC_BLACK_BRIGHT   "\033[90m"
#define TC_RED_BRIGHT     "\033[91m"
#define TC_GREEN_BRIGHT   "\033[92m"
#define TC_YELLOW_BRIGHT  "\033[93m"
#define TC_BLUE_BRIGHT    "\033[94m"
#define TC_MAGENTA_BRIGHT "\033[95m"
#define TC_CYAN_BRIGHT    "\033[96m"
#define TC_WHITE_BRIGHT   "\033[97m"

#define TC_BG_BLACK   "\033[40m"
#define TC_BG_RED     "\033[41m"
#define TC_BG_GREEN   "\033[42m"
#define TC_BG_YELLOW  "\033[43m"
#define TC_BG_BLUE    "\033[44m"
#define TC_BG_MAGENTA "\033[45m"
#define TC_BG_CYAN    "\033[46m"
#define TC_BG_WHITE   "\033[47m"

#define TC_BG_BLACK_BRIGHT   "\033[100m"
#define TC_BG_RED_BRIGHT     "\033[101m"
#define TC_BG_GREEN_BRIGHT   "\033[102m"
#define TC_BG_YELLOW_BRIGHT  "\033[103m"
#define TC_BG_BLUE_BRIGHT    "\033[104m"
#define TC_BG_MAGENTA_BRIGHT "\033[105m"
#define TC_BG_CYAN_BRIGHT    "\033[106m"
#define TC_BG_WHITE_BRIGHT   "\033[107m"

void tty_put_color(FILE *stream, const char *color);
