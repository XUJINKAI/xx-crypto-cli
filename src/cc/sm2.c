#include "sm2.h"
#include "data/hex.h"

size_t cc_sm2_private_key_print_raw(XIO *io, const SM2_KEY *key)
{
    size_t r = 0;
    r += XIO_printf(io, "sk: ");
    r += XIO_write_hex(io, key->private_key, 32);
    r += XIO_printf(io, "\n");
    return r;
}

size_t cc_sm2_public_key_print_raw(XIO *io, const SM2_KEY *key)
{
    size_t r = 0;
    r += XIO_printf(io, "pk-x: ");
    r += XIO_write_hex(io, key->public_key.x, 32);
    r += XIO_printf(io, "\n");

    r += XIO_printf(io, "pk-y: ");
    r += XIO_write_hex(io, key->public_key.y, 32);
    r += XIO_printf(io, "\n");
    return r;
}

size_t cc_sm2_public_key_print_compressed_raw(XIO *io, const SM2_KEY *key)
{
    uint8_t buf[33];
    sm2_point_to_compressed_octets(&key->public_key, buf);

    size_t r = 0;
    r += XIO_printf(io, "pk: ");
    r += XIO_write_hex(io, buf, 33);
    r += XIO_printf(io, "\n");
    return r;
}

size_t cc_sm2_private_key_print_pem(XIO *io, const SM2_KEY *key)
{
    uint8_t buf[512];
    uint8_t *p = buf;
    size_t len = 0;

    if (sm2_private_key_info_to_der(key, &p, &len) != 1)
    {
        LOG_DBG("sm2_private_key_info_to_der failed");
        return 0;
    }
    return XIO_write_pem(io, "EC PRIVATE KEY", buf, len);
}

size_t cc_sm2_public_key_print_pem(XIO *io, const SM2_KEY *key)
{
    uint8_t buf[512];
    uint8_t *p = buf;
    size_t len = 0;

    if (sm2_public_key_info_to_der(key, &p, &len) != 1)
    {
        LOG_DBG("sm2_public_key_info_to_der failed");
        return 0;
    }
    return XIO_write_pem(io, "PUBLIC KEY", buf, len);
}

static RESULT cc_sm2_sign_buf(uint8_t *buf, size_t bufsize, XIO *instream, const SM2_KEY *key, const char *id,
                              size_t idlen, uint8_t *sig, size_t *siglen)
{
    RESULT ret = RET_FAIL;
    SM2_SIGN_CTX ctx;

    if (sm2_sign_init(&ctx, key, id, idlen) != 1)
    {
        LOG_DBG("sm2_sign_init failed");
        goto end;
    }
    size_t len;
    while ((len = XIO_read(instream, buf, bufsize)) > 0)
    {
        if (sm2_sign_update(&ctx, buf, len) != 1)
        {
            LOG_DBG("sm2_sign_update failed");
            goto end;
        }
    }
    if (sm2_sign_finish(&ctx, sig, siglen) != 1)
    {
        LOG_DBG("sm2_sign_finish failed");
        goto end;
    }
    ret = RET_OK;

end:
    clear_buffer(&ctx, sizeof(ctx));
    return ret;
}

RESULT cc_sm2_sign(XIO *instream, const SM2_KEY *key, const char *id, size_t idlen, size_t bufsize, uint8_t *sig,
                   size_t *siglen)
{
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *buf = malloc(bufsize);
        if (!buf)
        {
            LOG_DBG("malloc failed");
            return RET_FAIL;
        }
        RESULT ret = cc_sm2_sign_buf(buf, bufsize, instream, key, id, idlen, sig, siglen);
        free(buf);
        return ret;
    }
    else
    {
        uint8_t buf[STACK_BUFFER_SIZE];
        return cc_sm2_sign_buf(buf, STACK_BUFFER_SIZE, instream, key, id, idlen, sig, siglen);
    }
}

RESULT cc_sm2_try_load_private_key(SM2_KEY *sm2key, const char *key, size_t keylen, const char *pass)
{
    RESULT ret = RET_FAIL;
    uint8_t privateKey[32];

    if (RET_OK == hex_str_expect_to_bytes(key, 32, privateKey))
    {
        if (sm2_key_set_private_key(sm2key, privateKey) != 1)
        {
            goto end;
        }
        ret = RET_OK;
    }

end:
    return ret;
}