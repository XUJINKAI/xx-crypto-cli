#include "hash.h"

RESULT cc_hash_sm3(XIO *in, XIO *out)
{
    SM3_CTX ctx;
    uint8_t buf[1024];
    size_t len;
    uint8_t dgst[32];

    sm3_init(&ctx);
    while ((len = XIO_read(in, buf, sizeof(buf))) > 0)
    {
        sm3_update(&ctx, buf, len);
    }
    sm3_finish(&ctx, dgst);
    XIO_write(out, dgst, sizeof(dgst));
    return RET_OK;
}

RESULT cc_hash(const DIGEST *digest, XIO *in, XIO *out)
{
    DIGEST_CTX ctx;
    uint8_t buf[10 * 1024 * 1024];
    size_t len;
    uint8_t dgst[128];
    size_t dgstlen;

    digest_init(&ctx, digest);
    while ((len = XIO_read(in, buf, sizeof(buf))) > 0)
    {
        digest_update(&ctx, buf, len);
    }
    digest_finish(&ctx, dgst, &dgstlen);
    XIO_write(out, dgst, dgstlen);
    return RET_OK;
}

RESULT cc_hash_ex(const DIGEST *digest, XIO *in, XIO *out, size_t bufsize)
{
    if (bufsize == 0)
    {
        return cc_hash(digest, in, out);
    }

    DIGEST_CTX ctx;
    uint8_t *buf = malloc(bufsize);
    size_t len;
    uint8_t dgst[128];
    size_t dgstlen;

    digest_init(&ctx, digest);
    while ((len = XIO_read(in, buf, bufsize)) > 0)
    {
        digest_update(&ctx, buf, len);
    }
    digest_finish(&ctx, dgst, &dgstlen);
    XIO_write(out, dgst, dgstlen);

    free(buf);
    return RET_OK;
}
