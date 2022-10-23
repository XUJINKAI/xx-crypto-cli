#pragma once

#include <stdint.h>

// XSTREAM
typedef struct XIO_st XIO;

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen);
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len);
size_t XIO_printf(XIO *io, const char *format, ...);
void XIO_flush(XIO *io);
void XIO_close(XIO *io);
void XIO_dump(XIO *io, XIO *out);
void XIO_copy(XIO *in, XIO *out);

XIO *XIO_new_fp(FILE *fp, bool close_free);
XIO *XIO_new_file(const char *filename, const char *mode);
XIO *XIO_new_r_mem(const void *data, size_t datalen, bool close_free);
XIO *XIO_new_from_string(const char *str, bool close_free);

XIO *XIO_new_filter_hex(XIO *io);
void XIO_HEX_write(XIO *io, const void *__ptr, size_t __len);

XIO *XIO_new_filter_base64(XIO *io, int line_length);
