#include "cipher.h"
#include "gmssl/sm4.h"

RESULT cc_sm4_cbc_encrypt(CC_SM4_PARAM *param, XIO *in, XIO *out)
{
    SM4_CBC_CTX cbc_ctx;
    uint8_t inbuf[1024];
    size_t inlen;
    uint8_t outbuf[1024];
    size_t outlen;
    int ret = RET_FAIL;

    if (sm4_cbc_encrypt_init(&cbc_ctx, param->key, param->iv) != 1)
    {
        LOG_ERROR("sm4_cbc_encrypt_init error");
        goto end;
    }
    while ((inlen = XIO_read(in, inbuf, sizeof(inbuf))) > 0)
    {
        if (sm4_cbc_encrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1)
        {
            LOG_ERROR("sm4_cbc_encrypt_update error");
            goto end;
        }
        if (XIO_write(out, outbuf, outlen) != outlen)
        {
            LOG_ERROR("XIO_fwrite error");
            goto end;
        }
    }
    if (sm4_cbc_encrypt_finish(&cbc_ctx, outbuf, &outlen) != 1)
    {
        LOG_ERROR("sm4_cbc_encrypt_finish error");
        goto end;
    }
    if (XIO_write(out, outbuf, outlen) != outlen)
    {
        LOG_ERROR("XIO_fwrite error");
        goto end;
    }
    ret = RET_OK;

end:
    clear_buffer(inbuf, sizeof(inbuf));
    clear_buffer(outbuf, sizeof(outbuf));
    return ret;
}

RESULT cc_sm4_cbc_decrypt(CC_SM4_PARAM *param, XIO *in, XIO *out)
{
    SM4_CBC_CTX cbc_ctx;
    uint8_t inbuf[1024];
    size_t inlen;
    uint8_t outbuf[1024];
    size_t outlen;
    int ret = RET_FAIL;

    if (sm4_cbc_decrypt_init(&cbc_ctx, param->key, param->iv) != 1)
    {
        LOG_ERROR("sm4_cbc_decrypt_init error");
        goto end;
    }
    while ((inlen = XIO_read(in, inbuf, sizeof(inbuf))) > 0)
    {
        if (sm4_cbc_decrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1)
        {
            LOG_ERROR("sm4_cbc_decrypt_update error");
            goto end;
        }
        if (XIO_write(out, outbuf, outlen) != outlen)
        {
            LOG_ERROR("XIO_fwrite error");
            goto end;
        }
    }
    if (sm4_cbc_decrypt_finish(&cbc_ctx, outbuf, &outlen) != 1)
    {
        LOG_ERROR("sm4_cbc_decrypt_finish error");
        goto end;
    }
    if (XIO_write(out, outbuf, outlen) != outlen)
    {
        LOG_ERROR("XIO_fwrite error");
        goto end;
    }
    ret = RET_OK;

end:
    clear_buffer(inbuf, sizeof(inbuf));
    clear_buffer(outbuf, sizeof(outbuf));
    return RET_OK;
}
