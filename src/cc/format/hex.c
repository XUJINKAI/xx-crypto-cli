#include "hex.h"
#include <stdlib.h>

bool cc_is_hex_char(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

char *cc_hex_encode(const uint8_t *bytes, size_t len, int flag)
{
    if (bytes == NULL || len == 0)
    {
        LOG_DBG("bytes is NULL or len is 0");
        return NULL;
    }
    uint16_t line_len = flag & CC_HEX_ENC_LINE_LEN_MASK;
    int uppercase     = flag & CC_HEX_ENC_UPPERCASE;
    int lowercase     = flag & CC_HEX_ENC_LOWERCASE;
    if (line_len % 2 != 0)
    {
        LOG_DBG("line_len must be even");
        return NULL;
    }
    if (uppercase && lowercase)
    {
        LOG_DBG("uppercase and lowercase are mutually exclusive");
        return NULL;
    }
    const char *fmt = uppercase ? "%02X" : lowercase ? "%02x" : G_HEX_FMT;

    size_t need_size = len * 2 + 1 + 80;
    if (line_len)
    {
        need_size += len * 2 / line_len + 1;
    }
    char *result = calloc(1, need_size);
    if (!result)
    {
        LOG_DBG("calloc failed");
        return NULL;
    }
    char *p = result;
    int lc  = 0; // line count
    for (size_t i = 0; i < len; i++)
    {
        p += sprintf(p, fmt, bytes[i]);
        lc += 2;
        if (line_len && lc >= line_len)
        {
            *p++ = '\n';
            lc   = 0;
        }
    }
    if (line_len && lc)
    {
        *p++ = '\n';
    }
    *p = '\0';
    return result;
}

static int hexchar2int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}

xbytes *cc_hex_decode(const char *in, size_t inlen, int flag)
{
    if (in == NULL || inlen == 0)
    {
        LOG_DBG("in is NULL or inlen is 0");
        return NULL;
    }
    int ignore_space  = 1;
    int ignore_nonhex = flag & CC_HEX_DEC_IGNORE_NONHEX;
    xbytes *out       = xbytes_new(inlen / 2 + 80);
    if (!out)
    {
        LOG_DBG("xbytes_new failed");
        return NULL;
    }
    uint8_t b = 0, t = 0;
    int part = 0;
    for (size_t i = 0; i < inlen; i++)
    {
        if (ignore_space && is_space_char(in[i]))
        {
            continue;
        }
        if (ignore_nonhex && !cc_is_hex_char(in[i]))
        {
            continue;
        }
        if (part)
        {
            t = hexchar2int(in[i]);
            if (t < 0)
            {
                LOG_DBG("invalid hex char: %c", in[i]);
                xbytes_free(out);
                return NULL;
            }
            b = (b << 4) | t;
            xbytes_write(out, &b, 1);
            b    = 0;
            part = 0;
        }
        else
        {
            t = hexchar2int(in[i]);
            if (t < 0)
            {
                LOG_DBG("invalid hex char: %c", in[i]);
                xbytes_free(out);
                return NULL;
            }
            b    = t;
            part = 1;
        }
    }
    if (part)
    {
        LOG_DBG("odd hex string");
        xbytes_free(out);
        return NULL;
    }
    return out;
}

RESULT cc_hex_dec_expect_len(const char *hex, size_t expectlen, uint8_t *bytes)
{
    if (hex == NULL)
    {
        return RET_FAIL;
    }
    xbytes *mem = cc_hex_decode(hex, strlen(hex), 0);
    if (mem == NULL)
    {
        return RET_FAIL;
    }
    if (xbytes_length(mem) != expectlen)
    {
        xbytes_free(mem);
        return RET_FAIL;
    }
    memcpy(bytes, xbytes_buffer(mem), xbytes_length(mem));
    xbytes_free(mem);
    return RET_OK;
}

RESULT cc_hex_dec_max_len(const char *hex, size_t maxlen, uint8_t *bytes, size_t *len)
{
    if (hex == NULL)
    {
        return RET_FAIL;
    }
    xbytes *mem = cc_hex_decode(hex, strlen(hex), 0);
    if (mem == NULL)
    {
        return RET_FAIL;
    }
    if (xbytes_length(mem) > maxlen)
    {
        xbytes_free(mem);
        return RET_FAIL;
    }
    memcpy(bytes, xbytes_buffer(mem), xbytes_length(mem));
    *len = xbytes_length(mem);
    xbytes_free(mem);
    return RET_OK;
}

void cc_hexdump_encode(FILE *fp, void *__p, size_t __l)
{
    if (__p == NULL)
    {
        fprintf(fp, "dump: pointer NULL\n");
        return;
    }
    if (__l == 0)
    {
        fprintf(fp, "dump: length 0\n");
        return;
    }

    const size_t BYTES_PER_LINE = 16;
    uint8_t *p;

    for (size_t line = 0; line < __l; line += BYTES_PER_LINE)
    {
        fprintf(fp, "%.8lx: ", line);
        size_t count = __l - line;
        if (count > BYTES_PER_LINE)
            count = BYTES_PER_LINE;

        p = (uint8_t *)__p + line;
        for (size_t i = 0; i < BYTES_PER_LINE; i++)
        {
            if (i == 8)
                fprintf(fp, " ");
            if (i < count)
                fprintf(fp, "%.2x ", *p);
            else
                fprintf(fp, "   ");
            p++;
        }

        fprintf(fp, " ");
        p = (uint8_t *)__p + line;
        for (size_t i = 0; i < count; i++)
        {
            if (*p > 31 && *p < 127)
                fprintf(fp, "%c", *p);
            else
                fprintf(fp, ".");
            p++;
        }

        fprintf(fp, "\n");
    }
}
