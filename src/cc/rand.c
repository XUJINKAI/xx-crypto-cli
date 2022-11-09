#include "rand.h"
#include "cross-platform/string.h"
#include "gmssl/rand.h"
#include "gmssl/sdf.h"

typedef int (*rand_func)(uint8_t *buf, size_t buflen);

RESULT cc_random_bytes(const char *source, XIO *out, size_t len)
{
    rand_func fn = NULL;
    if (source == NULL || strcmp(source, "soft") == 0)
    {
        fn = rand_bytes;
    }
    else if (strcmp(source, "rdrand") == 0)
    {
        fn = rdrand_bytes;
    }
    else if (strcmp(source, "rdseed") == 0)
    {
        fn = rdseed_bytes;
    }
    else if (strncasecmp(source, "sdf:", 4) == 0)
    {
        return cc_random_bytes_sdf(source + 4, out, len);
    }
    else
    {
        LOG_ERR("Unknown random source: %s", source);
        return RET_FAIL;
    }

    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (fn(buf, n) < 0)
        {
            return RET_FAIL;
        }
        XIO_write(out, buf, n);
        len -= n;
    }
    return RET_OK;
}

RESULT cc_random_bytes_sdf(const char *libname, XIO *out, size_t len)
{
    if (sdf_load_library(libname, NULL) < 0)
    {
        return RET_FAIL;
    }

    SDF_DEVICE dev;
    if (sdf_open_device(&dev) < 0)
    {
        return RET_FAIL;
    }

    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (sdf_rand_bytes(&dev, buf, n) < 0)
        {
            return RET_FAIL;
        }
        XIO_write(out, buf, n);
        len -= n;
    }

    sdf_close_device(&dev);
    sdf_unload_library();
    return RET_OK;
}
