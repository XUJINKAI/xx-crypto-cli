#pragma once

#include "utils/console.h"
#include <stdio.h>
#include <xio/xio.h>

typedef enum
{
    _LOGT_NORMAL = 0,
    _LOGT_VERBOSE,
    _LOGT_DEBUG,
    _LOGT_SECRET,
    _LOGT_MAX,
} _LOG_TYPE;

#define _LOGC_ERROR  TC_RED
#define _LOGC_WARN   TC_YELLOW
#define _LOGC_SECRET TC_MAGENTA

#define _LOGF_NONE 0
#define _LOGF_PROG 0x01

void log_set_stream(FILE *stream);
void log_enable_type(_LOG_TYPE type, bool enable);
bool log_is_type_enabled(_LOG_TYPE type);

void _log_write_hex(const void *ptr, size_t len);
void _log_write_fmt(const char *fmt, ...);
void _log_write_color(const char *color);

void _log_fmt_content(_LOG_TYPE type, int flag, const char *color, const char *fmt, ...);
void _log_hex_content(_LOG_TYPE type, int flag, const char *color, const char *title, const void *ptr, size_t len);
void _log_xio_content(_LOG_TYPE type, int flag, const char *color, const char *title, XIO *xio);

#define LOG_C(color, ...) _log_fmt_content(_LOGT_NORMAL, _LOGF_PROG, color, __VA_ARGS__)
#define LOG_ERR(...)      _log_fmt_content(_LOGT_NORMAL, _LOGF_PROG, _LOGC_ERROR, __VA_ARGS__)
#define LOG_WARN(...)     _log_fmt_content(_LOGT_NORMAL, _LOGF_PROG, _LOGC_WARN, __VA_ARGS__)
#define LOG_SECRET(...)   _log_fmt_content(_LOGT_SECRET, _LOGF_NONE, _LOGC_SECRET, __VA_ARGS__)
#define LOG_VERBOSE(...)  _log_fmt_content(_LOGT_VERBOSE, _LOGF_PROG, TC_DIM, __VA_ARGS__)
#define LOG_DBG(...)      _log_fmt_content(_LOGT_DEBUG, _LOGF_PROG, TC_DIM, __VA_ARGS__)

#define LOG_C_HEX(color, title, data, len) _log_hex_content(_LOGT_NORMAL, _LOGF_PROG, color, title, data, len)
#define LOG_SECRET_HEX(title, data, len)   _log_hex_content(_LOGT_SECRET, _LOGF_NONE, _LOGC_SECRET, title, data, len)
#define LOG_DBG_HEX(title, data, len)      _log_hex_content(_LOGT_DEBUG, _LOGF_PROG, TC_DIM, title, data, len)

#define LOG_DBG_XIO(title, xio) _log_xio_content(_LOGT_DEBUG, _LOGF_PROG, TC_DIM, title, xio)
