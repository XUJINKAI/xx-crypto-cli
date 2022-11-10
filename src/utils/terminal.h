#pragma once

#include <stdio.h>

#define TTY_COLOR_MAP(XX)                                                                                              \
    XX(RED, "\033[31m")                                                                                                \
    XX(GREEN, "\033[32m")                                                                                              \
    XX(YELLOW, "\033[33m")                                                                                             \
    XX(BLUE, "\033[34m")                                                                                               \
    XX(MAGENTA, "\033[35m")                                                                                            \
    XX(CYAN, "\033[36m")                                                                                               \
    XX(WHITE, "\033[37m")                                                                                              \
    XX(RESET, "\033[0m")                                                                                               \
    XX(BOLD, "\033[1m")                                                                                                \
    XX(DIM, "\033[2m")

#define XX(name, value) TC_##name,
// Starts with TC_
typedef enum
{
    TC_NULL = 0,
    TTY_COLOR_MAP(XX)
} tty_color;
#undef XX

void fput_color(FILE *stream, tty_color color);
void tty_put_color(FILE *stream, tty_color color);
