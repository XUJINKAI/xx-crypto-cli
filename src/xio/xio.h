#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef SEEK_SET
#define SEEK_SET 0 /* Seek from beginning of file.  */
#define SEEK_CUR 1 /* Seek from current position.  */
#define SEEK_END 2 /* Seek from end of file.  */
#endif

// type
#define XIO_TYPE_RAW        0x0100
#define XIO_TYPE_FILTER     0x0200
#define XIO_TYPE_DESCRIPTOR 0x1000 /* socket, fd, connect or accept */

#define XIO_TYPE_NONE   0
#define XIO_TYPE_FILE   (1 | XIO_TYPE_RAW | XIO_TYPE_DESCRIPTOR)
#define XIO_TYPE_MEMORY (2 | XIO_TYPE_RAW)
#define XIO_TYPE_NULL   (3 | XIO_TYPE_RAW)

#define XIO_TYPE_HEX    (8 | XIO_TYPE_FILTER)
#define XIO_TYPE_BASE64 (9 | XIO_TYPE_FILTER)
#define XIO_TYPE_PASS   (10 | XIO_TYPE_FILTER)
#define XIO_TYPE_DIGEST (11 | XIO_TYPE_FILTER)
#define XIO_TYPE_CIPHER (12 | XIO_TYPE_FILTER)
#define XIO_TYPE_ASN1   (13 | XIO_TYPE_FILTER)

// flags
#define XIO_FLAG_NONE               0x00
#define XIO_FLAG_CLOSE              0x01 // close source(raw) or target(filter)
#define XIO_FLAG_FILE_TTY_FLUSH_EOL 0x02 // for XIO_FILE, flush eol if tty

// XSTREAM
typedef struct XIO_st XIO;

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen);
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len);
size_t XIO_printf(XIO *io, const char *format, ...);
void XIO_flush(XIO *io);
int XIO_seek(XIO *io, int64_t offset, int whence);
int64_t XIO_tell(XIO *io);
void XIO_close(XIO *io);
void XIO_dump_chain(XIO *io, FILE *fp);

void XIO_drain(XIO *in, XIO *out);
bool XIO_isatty(XIO *io);
XIO *XIO_get_raw(XIO *io);
int XIO_get_flag(XIO *io);
void XIO_set_flag(XIO *io, int flag);

FILE *XIO_FILE_get_fp(XIO *io);

XIO *XIO_new_fp(FILE *fp, bool close_free);
XIO *XIO_new_file(const char *filename, const char *mode);
XIO *XIO_new_memory(const void *data, size_t datalen, bool close_free);
XIO *XIO_new_string(const char *str, bool close_free);
XIO *XIO_new_null(void);

XIO *XIO_newf_hex(XIO *io);
XIO *XIO_newf_base64(XIO *io);
XIO *XIO_newf_pass(XIO *io);

#define XIO_CLOSE_SAFE(xio)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (xio)                                                                                                       \
        {                                                                                                              \
            XIO_close(xio);                                                                                            \
            xio = NULL;                                                                                                \
        }                                                                                                              \
    } while (0)
