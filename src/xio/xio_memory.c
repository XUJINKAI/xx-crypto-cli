#include "xio_interface.h"
#include <stdlib.h>
#include <string.h>

typedef struct XIO_memory_st
{
    XIO base;
    uint8_t *buffer;
    size_t size;
    uint8_t *ptr;
} XIO_memory;

static size_t _xio_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_memory *io    = (XIO_memory *)__io;
    size_t remain_len = io->size - (io->ptr - io->buffer);
    size_t len        = __maxlen < remain_len ? __maxlen : remain_len;
    memcpy(__ptr, io->ptr, len);
    io->ptr += len;
    return len;
}
static size_t _xio_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    LOG_C(0, "XIO_memory is read-only");
    return 0;
}
static void _xio_flush(XIO *__io)
{
    (void)__io;
}
static void _xio_close(XIO *__io)
{
    XIO_memory *io = (XIO_memory *)__io;
    if (io->base.release_resource)
    {
        free(io->buffer);
    }
    free(io);
}
static void _xio_dump(XIO *__io, XIO *out)
{
    XIO_memory *io = (XIO_memory *)__io;
    XIO_printf(out, "XIO_memory: size=%zu", io->size);
}

const XIO_METHOD XIO_METHOD_MEMORY = {
    .read  = _xio_read,
    .write = _xio_write,
    .flush = _xio_flush,
    .close = _xio_close,
    .dump  = _xio_dump,
};

XIO *XIO_new_r_mem(const void *data, size_t datalen, bool close_free)
{
    XIO_memory *io            = (XIO_memory *)malloc(sizeof(XIO_memory));
    io->base.can_read         = true;
    io->base.can_write        = false;
    io->base.release_resource = close_free;
    io->base.methods          = (XIO_METHOD *)&XIO_METHOD_MEMORY;
    io->buffer                = (uint8_t *)data;
    io->size                  = datalen;
    io->ptr                   = io->buffer;
    return (XIO *)io;
}
XIO *XIO_new_from_string(const char *str, bool close_free)
{
    return XIO_new_r_mem(str, strlen(str), close_free);
}
