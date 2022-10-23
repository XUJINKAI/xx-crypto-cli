#pragma once

#include "utils/console.h"
#include <stdio.h>

typedef enum
{
    LOGT_NORMAL = 0,
    LOGT_VERBOSE,
    LOGT_DEBUG,
    LOGT_MAX,
} LOG_TYPE;

#define LOGC_ERROR TC_RED
#define LOGC_WARN  TC_YELLOW

void log_set_stream(FILE *stream);
void log_enable_type(LOG_TYPE type, bool enable);
bool log_is_type_enabled(LOG_TYPE type);

void _log_write_hex(const void *ptr, size_t len);
void _log_write_fmt(const char *fmt, ...);
void _log_write_color(const char *color);

void _log_fmt_content(LOG_TYPE type, const char *color, const char *fmt, ...);
void _log_hex_content(LOG_TYPE type, const char *color, const char *title, const void *ptr, size_t len);

#define LOG_C(color, ...) _log_fmt_content(LOGT_NORMAL, color, __VA_ARGS__)
#define LOG_ERR(...)      _log_fmt_content(LOGT_NORMAL, LOGC_ERROR, __VA_ARGS__)
#define LOG_WARN(...)     _log_fmt_content(LOGT_NORMAL, LOGC_WARN, __VA_ARGS__)
#define LOG_VERBOSE(...)  _log_fmt_content(LOGT_VERBOSE, TC_DIM, __VA_ARGS__)

#define LOG_C_HEX(color, title, data, len) _log_hex_content(LOGT_NORMAL, color, title, data, len)

#ifdef _DEBUG
#define LOG_DBG(...)                  _log_fmt_content(LOGT_DEBUG, TC_DIM, __VA_ARGS__)
#define LOG_DBG_HEX(title, data, len) _log_hex_content(LOGT_DEBUG, TC_DIM, title, data, len)
#else
#define LOG_DBG(...)
#define LOG_DBG_HEX(title, data, len)
#endif
