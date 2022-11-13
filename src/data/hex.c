#include "hex.h"
#include "gmssl/hex.h"
#include <stdlib.h>

void XIO_HEX_write(XIO *io, const void *__ptr, size_t __len)
{
    uint8_t *p = (uint8_t *)__ptr;
    for (size_t i = 0; i < __len; i++)
    {
        XIO_printf(io, "%02X", p[i]);
    }
}

char *bytes_to_hex(const uint8_t *bytes, size_t len)
{
    char *hex = malloc(len * 2 + 1);
    if (!hex)
    {
        return NULL;
    }
    for (size_t i = 0; i < len; i++)
    {
        sprintf(hex + i * 2, "%02X", bytes[i]);
    }
    hex[len * 2] = 0;
    return hex;
}

XX_MEM *hex_to_mem(const char *hex, size_t len)
{
    XX_MEM *mem = XX_MEM_new((len + 1) / 2);
    if (!mem)
    {
        return NULL;
    }
    if (hex_to_bytes(hex, len, mem->ptr, &mem->len) != 1)
    {
        XX_MEM_free(mem);
        return NULL;
    }
    return mem;
}

void hexdump_encode(FILE *fp, void *__p, size_t __l)
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
