#include "xio.h"
#include "xio_internal.h"

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    size_t len = XIO_read(__io->target, __ptr, __maxlen);
    __io->num_read += len;
    return len;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    size_t len = XIO_write(__io->target, __ptr, __len);
    __io->num_write += len;
    __io->last_char = __ptr[len - 1];
    return len;
}
static void f_close(XIO *__io)
{
    if (HAS_FLAG(__io->flags, XIO_FLAG_CLOSE))
    {
        XIO_close(__io->target);
    }
    free(__io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_printf(out_stream, "XIO_PASS");
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    switch (ctrl)
    {
        case _XIO_CTRL_DUP: {
            *(XIO **)parg = XIO_newf_pass(XIO_dup(__io->target), 0);
            return 1;
        }
        default:
            return 0;
    }
}

XIO *XIO_newf_pass(XIO *io, int flags)
{
    XIO *xio = (XIO *)calloc(1, sizeof(XIO));
    *xio     = (XIO){
        .read      = f_read,
        .write     = f_write,
        .close     = f_close,
        .to_string = f_to_string,
        .ctrl      = f_ctrl,
        .name      = "PASS",
        .type      = XIO_TYPE_PASS,
        .flags     = flags,
        .target    = io,
    };
    return xio;
}
