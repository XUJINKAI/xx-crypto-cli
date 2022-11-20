#include "sm2.h"

size_t cc_sm2_key_print_uncompressed(XIO *io, const SM2_KEY *key)
{
    size_t r = 0;
    r += XIO_printf(io, "pk-x: ");
    r += XIO_write_hex(io, key->public_key.x, 32);
    r += XIO_printf(io, "\n");

    r += XIO_printf(io, "pk-y: ");
    r += XIO_write_hex(io, key->public_key.y, 32);
    r += XIO_printf(io, "\n");

    r += XIO_printf(io, "sk:   ");
    r += XIO_write_hex(io, key->private_key, 32);
    r += XIO_printf(io, "\n");
    return r;
}

size_t cc_sm2_key_print_compresed(XIO *io, const SM2_KEY *key)
{
    uint8_t buf[33];
    sm2_point_to_compressed_octets(&key->public_key, buf);

    size_t r = 0;
    r += XIO_printf(io, "pk: ");
    r += XIO_write_hex(io, buf, 33);
    r += XIO_printf(io, "\n");

    r += XIO_printf(io, "sk: ");
    r += XIO_write_hex(io, key->private_key, 32);
    r += XIO_printf(io, "\n");
    return r;
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
