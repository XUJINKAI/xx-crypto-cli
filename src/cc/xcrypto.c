#include "xcrypto.h"
#include "global.h"
#include "gmssl/rand.h"
#include "gmssl/sm3.h"
#include "gmssl/sm4.h"

int x_random_bytes(XIO *out, size_t len)
{
    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (rand_bytes(buf, n) < 0)
        {
            LOG_C(0, "rand_bytes failed");
            return X_FAILURE;
        }
        XIO_write(out, buf, n);
        len -= n;
    }
    return X_SUCCESS;
}

int x_sm3(XIO *in, XIO *out)
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
}

int x_sm4_cbc_encrypt(const uint8_t key[16], const uint8_t iv[16], XIO *in, XIO *out)
{
    SM4_CBC_CTX cbc_ctx;
    uint8_t inbuf[1024];
    size_t inlen;
    uint8_t outbuf[1024];
    size_t outlen;
    int ret = X_FAILURE;

    if (sm4_cbc_encrypt_init(&cbc_ctx, key, iv) != 1)
    {
        LOG_C(0, "sm4_cbc_encrypt_init error");
        goto end;
    }
    while ((inlen = XIO_read(in, inbuf, sizeof(inbuf))) > 0)
    {
        if (sm4_cbc_encrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1)
        {
            LOG_C(0, "sm4_cbc_encrypt_update error");
            goto end;
        }
        if (XIO_write(out, outbuf, outlen) != outlen)
        {
            LOG_C(0, "XIO_fwrite error");
            goto end;
        }
    }
    if (sm4_cbc_encrypt_finish(&cbc_ctx, outbuf, &outlen) != 1)
    {
        LOG_C(0, "sm4_cbc_encrypt_finish error");
        goto end;
    }
    if (XIO_write(out, outbuf, outlen) != outlen)
    {
        LOG_C(0, "XIO_fwrite error");
        goto end;
    }
    ret = X_SUCCESS;

end:
    return ret;
}

int x_sm4_cbc_decrypt(const uint8_t key[16], const uint8_t iv[16], XIO *in, XIO *out)
{
    SM4_CBC_CTX cbc_ctx;
    uint8_t buf[1024];
    size_t len;
    uint8_t outbuf[1024];
    size_t outlen;

    sm4_cbc_decrypt_init(&cbc_ctx, key, iv);
    while ((len = XIO_read(in, buf, sizeof(buf))) > 0)
    {
        sm4_cbc_decrypt_update(&cbc_ctx, buf, len, outbuf, &outlen);
        XIO_write(out, outbuf, outlen);
    }
    sm4_cbc_decrypt_finish(&cbc_ctx, outbuf, &outlen);
    XIO_write(out, outbuf, outlen);
}
