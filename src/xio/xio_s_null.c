#include "xio.h"
#include "xio_internal.h"

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    __io->num_read += __maxlen;
    return __maxlen;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    __io->num_write += __len;
    __io->last_char = __ptr[__len - 1];
    return __len;
}
static void f_close(XIO *__io)
{
    free(__io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_printf(out_stream, "XIO_NULL");
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    switch (ctrl)
    {
        case _XIO_CTRL_DUP: {
            *(XIO **)parg = XIO_new_null();
            return 1;
        }
        default:
            return 0;
    }
}

XIO *XIO_new_null(void)
{
    XIO *xio = (XIO *)calloc(1, sizeof(XIO));
    *xio     = (XIO){
        .read      = f_read,
        .write     = f_write,
        .close     = f_close,
        .to_string = f_to_string,
        .ctrl      = f_ctrl,
        .name      = "NULL",
        .type      = XIO_TYPE_NULL,
        .flags     = XIO_FLAG_CLOSE,
    };
    return xio;
}
