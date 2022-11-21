#include "log.h"
#include "global.h"
#include <stdarg.h>

static FILE *log_stream           = NULL;
static bool log_enable[_LOGT_MAX] = {true, true, false, false};

#define CHECK_LOG_STREAM()                                                                                             \
    if (log_stream == NULL)                                                                                            \
    {                                                                                                                  \
        return;                                                                                                        \
    }
#define CHECK_LOG_LEVEL(type)                                                                                          \
    if (!log_enable[type])                                                                                             \
    {                                                                                                                  \
        return;                                                                                                        \
    }

void log_set_stream(FILE *stream)
{
    log_stream = stream;
}
FILE *log_get_stream(void)
{
    return log_stream;
}
void log_set_level(_LOG_TYPE type, bool enable)
{
    log_enable[type] = enable;
}
bool log_is_level_enabled(_LOG_TYPE type)
{
    return log_enable[type];
}

void _log_write_hex(const void *ptr, size_t len)
{
    CHECK_LOG_STREAM();
    uint8_t *p         = (uint8_t *)ptr;
    const char *hexfmt = G_HEX_FMT;
    for (size_t i = 0; i < len; i++)
    {
        fprintf(log_stream, hexfmt, p[i]);
    }
}

void _log_write_fmt(const char *fmt, ...)
{
    CHECK_LOG_STREAM();
    va_list args;
    va_start(args, fmt);
    vfprintf(log_stream, fmt, args);
    va_end(args);
}

void _log_write_color(const char *color)
{
    CHECK_LOG_STREAM();
    tty_put_color(log_stream, color);
}

void _log_fmt_content(_LOG_TYPE type, int flag, const char *color, const char *fmt, ...)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    tty_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        fprintf(log_stream, "%s: ", "xx");
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(log_stream, fmt, args);
    va_end(args);
    tty_put_color(log_stream, TC_RESET);
    fprintf(log_stream, "\n");
}

void _log_hex_content(_LOG_TYPE type, int flag, const char *color, const char *title, const void *ptr, size_t len)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    tty_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        fprintf(log_stream, "%s: ", "xx");
    }
    fprintf(log_stream, "%s", title);
    uint8_t *p      = (uint8_t *)ptr;
    const char *fmt = G_HEX_FMT;
    for (size_t i = 0; i < len; i++)
    {
        fprintf(log_stream, fmt, p[i]);
    }
    tty_put_color(log_stream, TC_RESET);
    fprintf(log_stream, "\n");
}

void _log_xio_content(_LOG_TYPE type, int flag, const char *color, const char *title, XIO *xio)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    tty_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        fprintf(log_stream, "%s: ", "xx");
    }
    fprintf(log_stream, "%s", title);

    XIO *dump_stream = XIO_new_fp(log_stream, 0);
    XIO_dump_chain(xio, dump_stream);
    XIO_close(dump_stream);

    tty_put_color(log_stream, TC_RESET);
    fprintf(log_stream, "\n");
}
