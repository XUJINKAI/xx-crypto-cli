#include "gmssl/hex.h"
#include "xio.h"
#include "xio_internal.h"

typedef struct
{
    XIO base;
} XIO_HEX;

static size_t _read_(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_HEX *io      = (XIO_HEX *)__io;
    uint8_t *hex_buf = (uint8_t *)calloc(1, __maxlen * 2);
    size_t hex_len   = XIO_read(io->base.target, hex_buf, __maxlen * 2);
    if (hex_len % 2)
    {
        free(hex_buf);
        LOG_ERR("hex decode error");
        return 0;
    }
    size_t bytes_len;
    if (hex_to_bytes((char *)hex_buf, hex_len, __ptr, &bytes_len) < 0)
    {
        free(hex_buf);
        LOG_ERR("hex decode error");
        return 0;
    }
    free(hex_buf);
    io->base.num_read += bytes_len;
    return bytes_len;
}
static size_t _write_(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    size_t i    = 0;
    for (; i < __len; i++)
    {
        XIO_printf(io->base.target, "%02X", __ptr[i]);
    }
    io->base.num_write += __len;
    return i;
}
static void _flush_(XIO *__io)
{
    XIO_HEX *io = (XIO_HEX *)__io;
}
static void _close_(XIO *__io)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        XIO_close(io->base.target);
    }
    free(io);
}
static void _dump(XIO *__io, FILE *fp)
{
    XIO_HEX *io = (XIO_HEX *)__io;
    fprintf(fp, "XIO_HEX");
}

XIO *XIO_newf_hex(XIO *io)
{
    XIO_HEX *r = (XIO_HEX *)malloc(sizeof(XIO_HEX));
    memset(r, 0, sizeof(XIO_HEX));
    *r = (XIO_HEX){
        .base =
            {
                .read   = _read_,
                .write  = _write_,
                .flush  = _flush_,
                .close  = _close_,
                .dump   = _dump,
                .type   = XIO_TYPE_HEX,
                .flags  = XIO_FLAG_CLOSE,
                .target = io,
            },
    };
    return (XIO *)r;
}
