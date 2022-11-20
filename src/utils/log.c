#include "log.h"
#include "console.h"
#include "global.h"
#include <stdarg.h>

static XIO *log_stream            = NULL;
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

void log_set_stream(XIO *stream)
{
    log_stream = stream;
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
        XIO_printf(log_stream, hexfmt, p[i]);
    }
}

void _log_write_fmt(const char *fmt, ...)
{
    CHECK_LOG_STREAM();
    va_list args;
    va_start(args, fmt);
    XIO_vprintf(log_stream, fmt, args);
    va_end(args);
}

void _log_write_color(const char *color)
{
    CHECK_LOG_STREAM();
    if (XIO_isatty(log_stream))
    {
        XIO_put_color(log_stream, color);
    }
}

void _log_fmt_content(_LOG_TYPE type, int flag, const char *color, const char *fmt, ...)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    XIO_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        XIO_printf(log_stream, "%s: ", "xx");
    }
    va_list args;
    va_start(args, fmt);
    XIO_vprintf(log_stream, fmt, args);
    va_end(args);
    XIO_put_color(log_stream, TC_RESET);
    XIO_printf(log_stream, "\n");
}

void _log_hex_content(_LOG_TYPE type, int flag, const char *color, const char *title, const void *ptr, size_t len)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    XIO_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        XIO_printf(log_stream, "%s: ", "xx");
    }
    XIO_printf(log_stream, "%s", title);
    uint8_t *p      = (uint8_t *)ptr;
    const char *fmt = G_HEX_FMT;
    for (size_t i = 0; i < len; i++)
    {
        XIO_printf(log_stream, fmt, p[i]);
    }
    XIO_put_color(log_stream, TC_RESET);
    XIO_printf(log_stream, "\n");
}

void _log_xio_content(_LOG_TYPE type, int flag, const char *color, const char *title, XIO *xio)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    XIO_put_color(log_stream, color);
    if (HAS_FLAG(flag, _LOGF_PROG))
    {
        XIO_printf(log_stream, "%s: ", "xx");
    }
    XIO_printf(log_stream, "%s", title);
    XIO_dump_chain(xio, log_stream);
    XIO_put_color(log_stream, TC_RESET);
    XIO_printf(log_stream, "\n");
}
