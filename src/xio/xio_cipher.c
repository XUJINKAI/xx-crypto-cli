#include "cc/cipher.h"
#include "xio_interface.h"
#include <gmssl/sm4.h>
#include <stdlib.h>
#include <string.h>

typedef struct XIO_sm4_st
{
    XIO base;
    CC_CIPHER_CTX *ctx;
} XIO_sm4;

static size_t _xio_read(XIO *__io, uint8_t *__ptr, size_t __maxlen) {}
static size_t _xio_write(XIO *__io, const uint8_t *__ptr, size_t __len) {}
static void _xio_flush(XIO *__io) {}
static void _xio_close(XIO *__io) {}
static void _xio_dump(XIO *__io, XIO *out) {}

const XIO_METHOD _xio_methods_sm4 = {
    .read  = _xio_read,
    .write = _xio_write,
    .flush = _xio_flush,
    .close = _xio_close,
    .dump  = _xio_dump,
};

XIO *XIO_new_cipher(XIO *target, CC_CIPHER_CTX *ctx, bool close_free)
{
    XIO_sm4 *io               = (XIO_sm4 *)malloc(sizeof(XIO_sm4));
    io->base.methods          = (XIO_METHOD *)&_xio_methods_sm4;
    io->base.can_read         = true;
    io->base.can_write        = false;
    io->base.release_resource = close_free;
    io->base.target           = target;
    io->ctx                   = ctx;
    return (XIO *)io;
}
