#include "xio_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    XIO base;
    uint8_t *buffer;
    size_t size;
    size_t readn;
} XIO_MEMORY;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_MEMORY *io    = (XIO_MEMORY *)__io;
    size_t remain_len = io->size - io->readn;
    size_t len        = __maxlen < remain_len ? __maxlen : remain_len;
    memcpy(__ptr, io->buffer + io->readn, len);
    io->readn += len;
    io->base.num_read += len;
    return len;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    LOG_ERROR("XIO_MEMORY is read-only");
    return 0;
}
static void f_close(XIO *__io)
{
    XIO_MEMORY *io = (XIO_MEMORY *)__io;
    free(io->buffer);
    free(io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_MEMORY *io = (XIO_MEMORY *)__io;
    XIO_printf(out_stream, "XIO_MEMORY(size=%zu)", io->size);
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    XIO_MEMORY *io = (XIO_MEMORY *)__io;
    switch (ctrl)
    {
        case _XIO_CTRL_DUP: {
            XIO_MEMORY *new_io = (XIO_MEMORY *)XIO_new_memory(io->buffer, io->size);
            *(XIO **)parg      = (XIO *)new_io;
            return 1;
        }
        default:
            return 0;
    }
}

XIO *XIO_new_memory(const void *data, size_t datalen)
{
    XIO_MEMORY *io = (XIO_MEMORY *)malloc(sizeof(XIO_MEMORY));
    memset(io, 0, sizeof(XIO_MEMORY));
    *io = (XIO_MEMORY){
        .base =
            {
                .read      = f_read,
                .write     = f_write,
                .close     = f_close,
                .to_string = f_to_string,
                .ctrl      = f_ctrl,
                .name      = "MEMORY",
                .type      = XIO_TYPE_MEMORY,
                .flags     = 0,
            },
        .buffer = NULL,
        .size   = datalen,
        .readn  = 0,
    };
    io->buffer = malloc(datalen);
    memcpy(io->buffer, data, datalen);
    return (XIO *)io;
}
XIO *XIO_new_string(const char *str)
{
    return XIO_new_memory(str, strlen(str));
}
