#pragma once

#include "utils/unistd.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// XSTREAM
typedef struct XIO_st XIO;

#ifndef SEEK_SET
#define SEEK_SET 0 /* Seek from beginning of file.  */
#define SEEK_CUR 1 /* Seek from current position.  */
#define SEEK_END 2 /* Seek from end of file.  */
#endif

// type
typedef enum
{
    XIO_TYPE_SOURCE     = 0x0100,
    XIO_TYPE_FILTER     = 0x0200,
    XIO_TYPE_DESCRIPTOR = 0x1000, /* socket, fd, connect or accept */

    XIO_TYPE_NONE     = 0,
    XIO_TYPE_NULL     = (1 | XIO_TYPE_SOURCE),
    XIO_TYPE_FILE     = (2 | XIO_TYPE_SOURCE | XIO_TYPE_DESCRIPTOR),
    XIO_TYPE_MEMORY   = (3 | XIO_TYPE_SOURCE),
    XIO_TYPE_MBUILDER = (4 | XIO_TYPE_SOURCE),

    XIO_TYPE_HEX    = (8 | XIO_TYPE_FILTER),
    XIO_TYPE_BASE64 = (9 | XIO_TYPE_FILTER),
    XIO_TYPE_PASS   = (10 | XIO_TYPE_FILTER),
    XIO_TYPE_DIGEST = (11 | XIO_TYPE_FILTER),
    XIO_TYPE_CIPHER = (12 | XIO_TYPE_FILTER),
    XIO_TYPE_ASN1   = (13 | XIO_TYPE_FILTER),
} _XIO_TYPE;

// flags
#define XIO_FLAG_NONE  0x00
#define XIO_FLAG_CLOSE 0x01 // close resource(source) or target(filter)

// ctrl
typedef enum
{
    _XIO_CTRL_DUP = 1,
    _XIO_CTRL_FLUSH,
    _XIO_CTRL_SEEK_SET,
    _XIO_CTRL_SEEK_CUR,
    _XIO_CTRL_SEEK_END,
    _XIO_CTRL_TELL,
} _XIO_CTRL_ENUM;
// return 0 if not supported
long _XIO_ctrl(XIO *io, _XIO_CTRL_ENUM ctrl, long larg, void *parg);

typedef enum
{
    XIO_EVENT_READ = 1,
    XIO_EVENT_WRITE,
    XIO_EVENT_FLUSH,
    XIO_EVENT_BEFORE_CLOSE,
} _XIO_EVENT_ENUM;
typedef void (*XIO_EVENT_CALLBACK)(XIO *io, _XIO_EVENT_ENUM event, void *arg, const void *data, size_t len);

#define XIO_flush(io)                (_XIO_ctrl(io, _XIO_CTRL_FLUSH, 0, 0))
#define XIO_seek(io, offset, whence) (_XIO_ctrl(io, _XIO_CTRL_SEEK_SET + (whence), (offset), 0))
#define XIO_tell(io)                 (_XIO_ctrl(io, _XIO_CTRL_TELL, 0, 0))

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen);
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len);
size_t XIO_write_hex(XIO *io, const uint8_t *__ptr, size_t __len);
size_t XIO_write_pem(XIO *io, const char *name, const uint8_t *__ptr, size_t __len);
size_t XIO_printf(XIO *io, const char *format, ...) __printflike(2, 3);
size_t XIO_vprintf(XIO *io, const char *format, va_list args);
void XIO_put_color(XIO *io, const char *color);
void XIO_close(XIO *io);
void XIO_dump_chain(XIO *io, XIO *out_stream);
XIO *XIO_dup(XIO *io);
void XIO_drain(XIO *in, XIO *out);
bool XIO_isatty(XIO *io);
XIO *XIO_get_source(XIO *io);
size_t XIO_get_num_read(XIO *io, bool source);
size_t XIO_get_num_write(XIO *io, bool source);
char XIO_get_last_char(XIO *io, bool source);
void XIO_set_event_callback(XIO *io, XIO_EVENT_CALLBACK callback, void *arg);

XIO *XIO_new_null(void);
XIO *XIO_new_fp(FILE *fp, int flags);
XIO *XIO_new_file(const char *filename, const char *mode);
XIO *XIO_new_memory(const void *data, size_t datalen);
XIO *XIO_new_string(const char *str);
XIO *XIO_new_mbuilder(void);

XIO *XIO_newf_hex(XIO *io);
XIO *XIO_newf_base64(XIO *io);
XIO *XIO_newf_pass(XIO *io, int flags);

FILE *XIO_FILE_get_fp(XIO *io);
void *XIO_MBUILDER_get_buffer(XIO *io);
size_t XIO_MBUILDER_get_size(XIO *io);
size_t XIO_MBUILDER_fprint(XIO *io, FILE *fp);

#define xio_close_safe(xio)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (xio)                                                                                                       \
        {                                                                                                              \
            XIO_close(xio);                                                                                            \
            xio = NULL;                                                                                                \
        }                                                                                                              \
    } while (0)
