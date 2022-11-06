#include "xio.h"
#include "xio_internal.h"

static size_t _read_(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    __io->num_read += __maxlen;
    return __maxlen;
}
static size_t _write_(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    __io->num_write += __len;
    return __len;
}
static void _flush_(XIO *__io) {}
static void _close_(XIO *__io) {}
static void _dump(XIO *__io, FILE *fp)
{
    fprintf(fp, "XIO_NULL");
}

XIO *XIO_new_null(void)
{
    XIO *xio = (XIO *)calloc(1, sizeof(XIO));
    *xio     = (XIO){
        .read  = _read_,
        .write = _write_,
        .flush = _flush_,
        .close = _close_,
        .dump  = _dump,
        .type  = XIO_TYPE_NULL,
        .flags = XIO_FLAG_CLOSE,
    };
    return xio;
}
