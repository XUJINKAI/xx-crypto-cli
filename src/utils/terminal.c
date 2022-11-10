#include "terminal.h"
#include "global.h"

#define XX(name, value) value,
static const char *tty_colors[] = {"", TTY_COLOR_MAP(XX)};
#undef XX

void fput_color(FILE *stream, tty_color color)
{
    if (g_state.no_color)
    {
        return;
    }
    fputs(tty_colors[color], stream);
}

void tty_put_color(FILE *stream, tty_color color)
{
    if (isatty(fileno(stream)))
    {
        fput_color(stream, color);
    }
}
