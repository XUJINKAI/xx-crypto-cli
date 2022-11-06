#include "gmssl/base64.h"
#include "xio_internal.h"

struct
{
    XIO base;
    BASE64_CTX ctx;
} XIO_BASE64;
