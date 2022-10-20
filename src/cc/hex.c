#include "hex.h"
#include <stdlib.h>

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
