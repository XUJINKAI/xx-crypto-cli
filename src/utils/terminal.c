#include "terminal.h"
#include "global.h"

void tty_put_color(FILE *stream, const char *color)
{
    if (!g_state.no_color && isatty(fileno(stream)))
    {
        fputs(color, stream);
    }
}
