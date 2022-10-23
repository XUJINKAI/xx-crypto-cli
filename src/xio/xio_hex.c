#include "gmssl/hex.h"
#include "xio_interface.h"

void XIO_HEX_write(XIO *io, const void *__ptr, size_t __len)
{
    uint8_t *p = (uint8_t *)__ptr;
    for (size_t i = 0; i < __len; i++)
    {
        XIO_printf(io, "%02X", p[i]);
    }
}

typedef struct XIO_hex_st
{
    XIO base;
    XIO *source;
} XIO_HEX;

static size_t _xio_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_HEX *io      = (XIO_HEX *)__io;
    uint8_t *hex_buf = (uint8_t *)calloc(1, __maxlen * 2);
    size_t hex_len   = XIO_read(io->source, hex_buf, __maxlen * 2);
    if (hex_len % 2)
    {
        free(hex_buf);
        LOG_C(0, "hex decode error");
        return 0;
    }
    size_t bytes_len;
    if (hex_to_bytes(hex_buf, hex_len, __ptr, &bytes_len) < 0)
    {
        free(hex_buf);
        LOG_C(0, "hex decode error");
        return 0;
    }
    free(hex_buf);
    return bytes_len;
}
static size_t _xio_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    size_t i    = 0;
    for (; i < __len; i++)
    {
        XIO_printf(io->source, "%02X", __ptr[i]);
    }
    return i;
}
static void _xio_flush(XIO *__io)
{
    (void)__io;
}
static void _xio_close(XIO *__io)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    // XIO_printf(io->source, "\n");
    if (io->base.release_resource)
    {
        XIO_close(io->source);
    }
    free(io);
}
static void _xio_dump(XIO *__io, XIO *out)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    XIO_printf(out, "XIO_HEX: ");
    XIO_dump(io->source, out);
}

const XIO_METHOD __xio_methods_hex = {
    .read  = _xio_read,
    .write = _xio_write,
    .flush = _xio_flush,
    .close = _xio_close,
    .dump  = _xio_dump,
};

XIO *XIO_new_filter_hex(XIO *io)
{
    XIO_HEX *r               = (XIO_HEX *)malloc(sizeof(XIO_HEX));
    r->base.methods          = (XIO_METHOD *)&__xio_methods_hex;
    r->base.can_read         = io->can_read;
    r->base.can_write        = io->can_write;
    r->base.release_resource = true;
    r->source                = io;
    return (XIO *)r;
}
