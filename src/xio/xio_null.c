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
    return __len;
}
static void f_flush(XIO *__io) {}
static void f_close(XIO *__io) {}
static void f_dump(XIO *__io, FILE *fp)
{
    fprintf(fp, "XIO_NULL");
}

XIO *XIO_new_null(void)
{
    XIO *xio = (XIO *)calloc(1, sizeof(XIO));
    *xio     = (XIO){
        .read  = f_read,
        .write = f_write,
        .flush = f_flush,
        .close = f_close,
        .dump  = f_dump,
        .type  = XIO_TYPE_NULL,
        .flags = XIO_FLAG_CLOSE,
    };
    return xio;
}
