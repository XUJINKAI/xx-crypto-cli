#include "base64.h"

bool cc_is_base64_char(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '+' || c == '/' ||
           c == '=' || c == '-' || c == '_';
    // - and _ are base64 url characters (replace + and /)
}

static const unsigned char BASE64ENCSTR_STD[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char BASE64ENCSTR_URL[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
// clang-format off
static const unsigned char BASE64DECSTR[128] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //  !"#$%&'
    0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0x3E, 0xFF, 0x3F, // ()*+,-./
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, // 01234567
    0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, // 89:;<=>?
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // @ABCDEFG
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, // HIJKLMNO
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, // PQRSTUVW
    0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, // XYZ[\]^_
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, // `abcdefg
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // hijklmno
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, // pqrstuvw
    0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // xyz{|}~
};
// clang-format on

char *cc_base64_encode(const uint8_t *__ptr, size_t __len, int flag)
{
    if (__ptr == NULL || __len == 0)
    {
        LOG_DBG("bytes is NULL or len is 0");
        return NULL;
    }
    uint16_t line_len = flag & CC_BASE64_ENC_LINE_LEN_MASK;
    int enc_url       = flag & CC_BASE64_ENC_URL;
    if (line_len % 4 != 0)
    {
        LOG_DBG("line_len must be multiple of 4");
        return NULL;
    }
    const char *base64encstr = enc_url ? BASE64ENCSTR_URL : BASE64ENCSTR_STD;

    size_t need_size = ((__len + 2) / 3 * 4) + 1 + 80;
    if (line_len)
    {
        need_size += ((__len + 2) / 3 * 4) / line_len + 1;
    }
    uint8_t *result = calloc(1, need_size);
    if (!result)
    {
        LOG_DBG("calloc failed");
        return NULL;
    }

    uint8_t *p  = result;
    int lc      = 0; // line count
    size_t i    = 0;
    size_t len3 = __len / 3 * 3;
    uint32_t v;
    for (i = 0; i < len3; i += 3)
    {
        v = __ptr[i];
        v <<= 8;
        v |= __ptr[i + 1];
        v <<= 8;
        v |= __ptr[i + 2];
        *p++ = base64encstr[(v >> 18) & 0x3f];
        *p++ = base64encstr[(v >> 12) & 0x3f];
        *p++ = base64encstr[(v >> 6) & 0x3f];
        *p++ = base64encstr[v & 0x3f];
        lc += 4;
        if (line_len && lc >= line_len)
        {
            *p++ = '\n';
            lc   = 0;
        }
    }
    switch (__len - i)
    {
        case 1:
            v = __ptr[i];
            v <<= 8;
            *p++ = base64encstr[(v >> 10) & 0x3f];
            *p++ = base64encstr[(v >> 4) & 0x3f];
            *p++ = '=';
            *p++ = '=';
            break;
        case 2:
            v = __ptr[i];
            v <<= 8;
            v |= __ptr[i + 1];
            v <<= 8;
            *p++ = base64encstr[(v >> 18) & 0x3f];
            *p++ = base64encstr[(v >> 12) & 0x3f];
            *p++ = base64encstr[(v >> 6) & 0x3f];
            *p++ = '=';
            break;
    }
    if (line_len && lc)
    {
        *p++ = '\n';
    }
    *p = '\0';

    return result;
}

xbytes *cc_base64_decode(const char *in, size_t inlen, int flag)
{
    if (in == NULL || inlen == 0)
    {
        LOG_DBG("in is NULL or inlen is 0");
        return NULL;
    }
    int ignore_space  = 1;
    int ignore_nonhex = flag & CC_BASE64_DEC_IGNORE_NONBASE64;

    xbytes *out = xbytes_new(inlen / 4 * 3 + 80);
    if (!out)
    {
        LOG_DBG("xbytes_new failed");
        return NULL;
    }

    return out;
}
