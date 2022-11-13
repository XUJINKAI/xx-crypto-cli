#include "xio_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct XIO_memory_st
{
    XIO base;
    bool read_only;
    uint8_t *buffer;
    size_t size;
    size_t capacity;
    uint8_t *ptr;
} XIO_memory;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_memory *io    = (XIO_memory *)__io;
    size_t remain_len = io->size - (io->ptr - io->buffer);
    size_t len        = __maxlen < remain_len ? __maxlen : remain_len;
    memcpy(__ptr, io->ptr, len);
    io->ptr += len;
    io->base.num_read += len;
    return len;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    LOG_ERROR("XIO_memory is read-only");
    return 0;
}
static void f_flush(XIO *__io)
{
    (void)__io;
}
static void f_close(XIO *__io)
{
    XIO_memory *io = (XIO_memory *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        free(io->buffer);
    }
    free(io);
}
static void f_dump(XIO *__io, FILE *fp)
{
    XIO_memory *io = (XIO_memory *)__io;
    fprintf(fp, "XIO_MEMORY(size=%zu)", io->size);
}

XIO *XIO_new_memory(const void *data, size_t datalen, bool close_free)
{
    XIO_memory *io = (XIO_memory *)malloc(sizeof(XIO_memory));
    memset(io, 0, sizeof(XIO_memory));
    *io = (XIO_memory){
        .base =
            {
                .read  = f_read,
                .write = f_write,
                .flush = f_flush,
                .close = f_close,
                .dump  = f_dump,
                .type  = XIO_TYPE_MEMORY,
                .flags = close_free ? XIO_FLAG_CLOSE : 0,
            },
        .buffer = (uint8_t *)data,
        .size   = datalen,
        .ptr    = (uint8_t *)data,
    };
    return (XIO *)io;
}
XIO *XIO_new_string(const char *str, bool close_free)
{
    return XIO_new_memory(str, strlen(str), close_free);
}
