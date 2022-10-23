#include "log.h"
#include "console.h"
#include "global.h"
#include <stdarg.h>

static FILE *log_stream          = NULL;
static bool log_enable[LOGT_MAX] = {true, false, false};

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
void log_enable_type(LOG_TYPE type, bool enable)
{
    log_enable[type] = enable;
}
bool log_is_type_enabled(LOG_TYPE type)
{
    return log_enable[type];
}

void _log_write_hex(const void *ptr, size_t len)
{
    CHECK_LOG_STREAM();
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < len; i++)
    {
        fprintf(log_stream, "%02X", p[i]);
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
    if (color != NULL && isatty(fileno(log_stream)))
    {
        fputs(color, log_stream);
    }
}

void _log_fmt_content(LOG_TYPE type, const char *color, const char *fmt, ...)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    if (color != NULL && isatty(fileno(log_stream)))
    {
        fputs(color, log_stream);
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(log_stream, fmt, args);
    va_end(args);
    if (color != NULL && isatty(fileno(log_stream)))
    {
        fputs(TC_RESET, log_stream);
    }
    fprintf(log_stream, "\n");
}

void _log_hex_content(LOG_TYPE type, const char *color, const char *title, const void *ptr, size_t len)
{
    CHECK_LOG_STREAM();
    CHECK_LOG_LEVEL(type);
    if (color != NULL && isatty(fileno(log_stream)))
    {
        fputs(color, log_stream);
    }
    fprintf(log_stream, "%s", title);
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < len; i++)
    {
        fprintf(log_stream, "%02X", p[i]);
    }
    if (color != NULL && isatty(fileno(log_stream)))
    {
        fputs(TC_RESET, log_stream);
    }
    fprintf(log_stream, "\n");
}
