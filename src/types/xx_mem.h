#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint8_t *ptr;
    size_t len;
} XX_MEM;

XX_MEM *XX_MEM_new(size_t capacity);
XX_MEM *XX_MEM_dup(void *ptr, size_t len);
void XX_MEM_free(XX_MEM *mem);
