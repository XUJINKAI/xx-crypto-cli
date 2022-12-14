#include "gmssl/hex.h"
#include "xio.h"
#include "xio_internal.h"

typedef struct
{
    XIO base;
} XIO_HEX;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    LOG_ERROR("XIO_HEX does not support read");
    return 0;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    size_t i    = 0;
    for (; i < __len; i++)
    {
        XIO_printf(io->base.target, G_HEX_FMT, __ptr[i]);
    }
    io->base.num_write += __len;
    io->base.last_char = __ptr[__len - 1];
    return i;
}
static void f_close(XIO *__io)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        XIO_close(io->base.target);
    }
    free(io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_printf(out_stream, "XIO_HEX");
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    switch (ctrl)
    {
        case _XIO_CTRL_DUP:
            *(XIO **)parg = XIO_newf_hex(XIO_dup(io->base.target));
            return 1;
        default:
            return 0;
    }
}

XIO *XIO_newf_hex(XIO *io)
{
    XIO_HEX *r = (XIO_HEX *)malloc(sizeof(XIO_HEX));
    memset(r, 0, sizeof(XIO_HEX));
    *r = (XIO_HEX){
        .base =
            {
                .read      = f_read,
                .write     = f_write,
                .close     = f_close,
                .to_string = f_to_string,
                .ctrl      = f_ctrl,
                .name      = "HEX",
                .type      = XIO_TYPE_HEX,
                .flags     = XIO_FLAG_CLOSE,
                .target    = io,
            },
    };
    return (XIO *)r;
}
