#include "base64.h"
#include "gmssl/base64.h"

char *bytes_to_base64(const uint8_t *__ptr, size_t __len)
{
    uint8_t *b64 = NULL;
    if (!(b64 = calloc(1, __len * 2)))
    {
        return NULL;
    }

    BASE64_CTX ctx;
    int len;
    base64_encode_init(&ctx);
    base64_encode_update(&ctx, __ptr, (int)__len, b64, &len);
    base64_encode_finish(&ctx, b64 + len, &len);

    return b64;
}
