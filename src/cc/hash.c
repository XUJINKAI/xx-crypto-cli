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

static RESULT cc_hash_buf(uint8_t *__buf, size_t __size, const DIGEST *digest, XIO *in, XIO *out)
{
    size_t len = 0;
    DIGEST_CTX ctx;
    uint8_t dgst[128];
    size_t dgstlen;

    digest_init(&ctx, digest);
    while ((len = XIO_read(in, __buf, __size)) > 0)
    {
        digest_update(&ctx, __buf, len);
    }
    digest_finish(&ctx, dgst, &dgstlen);
    XIO_write(out, dgst, dgstlen);
    return RET_OK;
}

RESULT cc_hash(const DIGEST *digest, XIO *in, XIO *out, size_t bufsize)
{
    RESULT ret;
    uint8_t buf[STACK_BUFFER_SIZE];
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *p = malloc(bufsize);
        if (!p)
        {
            LOG_ERROR("malloc failed");
            return RET_FAIL;
        }
        ret = cc_hash_buf(p, bufsize, digest, in, out);
        free(p);
    }
    else
    {
        ret = cc_hash_buf(buf, STACK_BUFFER_SIZE, digest, in, out);
    }
    return ret;
}
