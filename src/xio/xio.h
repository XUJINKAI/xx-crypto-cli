#ifndef __XIO_H__
#define __XIO_H__

#include "global.h"
#include <stdio.h>

typedef struct XIO_st XIO;
typedef struct XIO_methods_st XIO_METHOD;

struct XIO_methods_st
{
    size_t (*read)(XIO *io, uint8_t *__ptr, size_t __maxlen);
    size_t (*write)(XIO *io, const uint8_t *__ptr, size_t __len);
    void (*flush)(XIO *io);
    void (*close)(XIO *io);
    void (*dump)(XIO *io, XIO *out);
};
struct XIO_st
{
    XIO_METHOD *methods;
    bool can_read;
    bool can_write;
    bool release_souce;
};

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen);
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len);
size_t XIO_printf(XIO *io, const char *format, ...);
void XIO_flush(XIO *io);
void XIO_close(XIO *io);
void XIO_dump(XIO *io, XIO *out);
void XIO_copy(XIO *in, XIO *out);

XIO *XIO_new_from_FILE(FILE *fp, bool close_free);
XIO *XIO_new_from_filename(const char *filename, const char *mode);
XIO *XIO_new_from_memory(const void *data, size_t datalen, bool close_free);
XIO *XIO_new_from_string(const char *str, bool close_free);
XIO *XIO_new_filter_hex(XIO *io);
XIO *XIO_new_filter_base64(XIO *io, int line_length);

#endif /* __XIO_H__ */
