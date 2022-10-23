#include "xx_mem.h"
#include <stdlib.h>
#include <string.h>

XX_MEM *XX_MEM_new(size_t capacity)
{
    XX_MEM *mem = calloc(1, sizeof(XX_MEM));
    if (!mem)
    {
        return NULL;
    }
    mem->ptr = calloc(1, capacity);
    mem->len = capacity;
    if (mem->ptr == NULL)
    {
        free(mem);
        return NULL;
    }
    return mem;
}
XX_MEM *XX_MEM_dup(void *ptr, size_t len)
{
    XX_MEM *mem = XX_MEM_new(len);
    if (!mem)
    {
        return NULL;
    }
    memcpy(mem->ptr, ptr, len);
    mem->len = len;
    return mem;
}
void XX_MEM_free(XX_MEM *mem)
{
    if (mem)
    {
        if (mem->ptr)
        {
            free(mem->ptr);
        }
        free(mem);
    }
}
