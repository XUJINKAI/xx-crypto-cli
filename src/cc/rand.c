#include "rand.h"
#include "gmssl/rand.h"

RESULT cc_random_bytes(XIO *out, size_t len)
{
    uint8_t buf[1024];
    while (len > 0)
    {
        size_t n = len > sizeof(buf) ? sizeof(buf) : len;
        if (rand_bytes(buf, n) < 0)
        {
            LOG_ERR("rand_bytes failed");
            return RET_FAIL;
        }
        XIO_write(out, buf, n);
        len -= n;
    }
    return RET_OK;
}
