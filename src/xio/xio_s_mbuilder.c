#include "xio_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    XIO base;
    uint8_t *buffer;
    size_t size;
    size_t capacity;
} XIO_MBUILDER;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    LOG_ERROR("f_read not implemented");
    return 0;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_MBUILDER *io = (XIO_MBUILDER *)__io;
    if (io->size + __len > io->capacity)
    {
        io->capacity = io->size + __len + 4096;
        io->buffer   = realloc(io->buffer, io->capacity);
    }
    memcpy(io->buffer + io->size, __ptr, __len);
    io->size += __len;
    io->base.num_write += __len;
    io->base.last_char = __ptr[__len - 1];
    return __len;
}
static void f_close(XIO *__io)
{
    XIO_MBUILDER *io = (XIO_MBUILDER *)__io;
    free(io->buffer);
    free(io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_MBUILDER *io = (XIO_MBUILDER *)__io;
    XIO_printf(out_stream, "XIO_MBUILDER(size=%zu)", io->size);
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    XIO_MBUILDER *io = (XIO_MBUILDER *)__io;
    switch (ctrl)
    {
        case _XIO_CTRL_DUP: {
            XIO_MBUILDER *new_io = (XIO_MBUILDER *)XIO_new_mbuilder();
            new_io->buffer       = realloc(new_io->buffer, io->size);
            memcpy(new_io->buffer, io->buffer, io->size);
            new_io->size     = io->size;
            new_io->capacity = io->size;
            *(XIO **)parg    = (XIO *)new_io;
            return 1;
        }
        default:
            return 0;
    }
}

XIO *XIO_new_mbuilder()
{
    const size_t capacity = 1024;
    XIO_MBUILDER *io      = (XIO_MBUILDER *)malloc(sizeof(XIO_MBUILDER));
    memset(io, 0, sizeof(XIO_MBUILDER));
    *io = (XIO_MBUILDER){
        .base =
            {
                .read      = f_read,
                .write     = f_write,
                .close     = f_close,
                .to_string = f_to_string,
                .ctrl      = f_ctrl,
                .name      = "MBUILDER",
                .type      = XIO_TYPE_MBUILDER,
                .flags     = 0,
            },
        .buffer   = calloc(1, capacity),
        .size     = 0,
        .capacity = capacity,
    };
    return (XIO *)io;
}

void *XIO_MBUILDER_get_buffer(XIO *io)
{
    return ((XIO_MBUILDER *)io)->buffer;
}
size_t XIO_MBUILDER_get_size(XIO *io)
{
    return ((XIO_MBUILDER *)io)->size;
}
size_t XIO_MBUILDER_fprint(XIO *io, FILE *fp)
{
    XIO_MBUILDER *mb = (XIO_MBUILDER *)io;
    return fwrite(mb->buffer, 1, mb->size, fp);
}
