#include "cc/format/base64.h"
#include "xio_internal.h"

typedef struct
{
    XIO base;
    uint8_t remain[3];
    int remain_len;
    int current_line_len;
    int base64_flag;
} XIO_BASE64;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    LOG_ERROR("XIO_BASE64 does not support read");
    return 0;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_BASE64 *io = (XIO_BASE64 *)__io;
    size_t i       = 0;
    char *base64   = cc_base64_encode(__ptr, __len, io->base64_flag);
    if (!base64)
    {
        LOG_ERROR("cc_base64_encode failed");
        return 0;
    }
    i = XIO_write(io->base.target, (uint8_t *)base64, strlen(base64));
    free(base64);
    io->base.num_write += __len;
    io->base.last_char = __ptr[__len - 1];
    return i;
}
static void f_close(XIO *__io)
{
    XIO_BASE64 *io = (XIO_BASE64 *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        XIO_close(io->base.target);
    }
    free(io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_printf(out_stream, "XIO_BASE64");
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    XIO_BASE64 *io = (XIO_BASE64 *)__io;
    switch (ctrl)
    {
        case _XIO_CTRL_FLUSH:
            // TODO
            return 1;
        case _XIO_CTRL_DUP:
            *(XIO **)parg = XIO_newf_base64(XIO_dup(io->base.target));
            return 1;
        default:
            return 0;
    }
}

XIO *XIO_newf_base64(XIO *io)
{
    XIO_BASE64 *r = (XIO_BASE64 *)malloc(sizeof(XIO_BASE64));
    memset(r, 0, sizeof(XIO_BASE64));
    *r = (XIO_BASE64){
        .base =
            {
                .read      = f_read,
                .write     = f_write,
                .close     = f_close,
                .to_string = f_to_string,
                .ctrl      = f_ctrl,
                .name      = "BASE64",
                .type      = XIO_TYPE_BASE64,
                .flags     = XIO_FLAG_CLOSE,
                .target    = io,
            },
        .remain_len  = 0,
        .base64_flag = 0,
    };
    return (XIO *)r;
}
