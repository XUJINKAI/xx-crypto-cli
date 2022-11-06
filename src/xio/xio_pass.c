#include "xio.h"
#include "xio_internal.h"


static size_t _read_(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    size_t len = XIO_read(__io->target, __ptr, __maxlen);
    __io->num_read += len;
    return len;
}
static size_t _write_(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    size_t len = XIO_write(__io->target, __ptr, __len);
    __io->num_write += len;
    return len;
}
static void _flush_(XIO *__io) {}
static void _close_(XIO *__io)
{
    if (HAS_FLAG(__io->flags, XIO_FLAG_CLOSE))
    {
        XIO_close(__io->target);
    }
}
static void _dump(XIO *__io, FILE *fp)
{
    fprintf(fp, "XIO_PASS");
}

XIO *XIO_newf_pass(XIO *io)
{
    XIO *xio = (XIO *)calloc(1, sizeof(XIO));
    *xio     = (XIO){
        .target = io,
        .read   = _read_,
        .write  = _write_,
        .flush  = _flush_,
        .close  = _close_,
        .dump   = _dump,
        .type   = XIO_TYPE_PASS,
        .flags  = XIO_FLAG_CLOSE,
    };
    return xio;
}
