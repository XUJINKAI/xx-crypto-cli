#include "xbytes.h"
#include "global.h"
#include <stdlib.h>
#include <string.h>

xbytes *xbytes_new(size_t capacity)
{
    xbytes *mem = calloc(1, sizeof(xbytes));
    if (!mem)
    {
        return NULL;
    }
    mem->ptr      = calloc(1, capacity);
    mem->len      = 0;
    mem->capacity = capacity;
    if (mem->ptr == NULL)
    {
        free(mem);
        return NULL;
    }
    return mem;
}
xbytes *xbytes_new_from(void *ptr, size_t len)
{
    xbytes *mem = xbytes_new(len);
    if (!mem)
    {
        return NULL;
    }
    xbytes_write(mem, ptr, len);
    return mem;
}
size_t xbytes_require_size(xbytes *mem, size_t required_remain_size)
{
    if (mem->len + required_remain_size + 1 > mem->capacity) // +1 for string end \0
    {
        size_t new_capacity = mem->capacity + required_remain_size + 4096;
        uint8_t *new_ptr    = realloc(mem->ptr, new_capacity);
        if (!new_ptr)
        {
            LOG_DBG("realloc failed");
            return 0;
        }
        mem->ptr      = new_ptr;
        mem->capacity = new_capacity;
    }
    return mem->capacity - mem->len;
}
size_t xbytes_write(xbytes *mem, void *ptr, size_t len)
{
    if (xbytes_require_size(mem, len) < len)
    {
        return 0;
    }
    memcpy(mem->ptr + mem->len, ptr, len);
    mem->len += len;
    return len;
}
size_t xbytes_printf(xbytes *mem, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    size_t len = xbytes_vprintf(mem, fmt, ap);
    va_end(ap);
    return len;
}
size_t xbytes_vprintf(xbytes *mem, const char *fmt, va_list ap)
{
    va_list ap1;
    va_copy(ap1, ap);
    size_t len = vsnprintf(NULL, 0, fmt, ap1);
    va_end(ap1);
    if (len == 0)
    {
        return 0;
    }
    if (xbytes_require_size(mem, len) < len)
    {
        return 0;
    }
    vsnprintf((char *)mem->ptr + mem->len, len + 1, fmt, ap);
    mem->len += len;
    return len;
}
void xbytes_free(xbytes *mem)
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
uint8_t *xbytes_detach_and_free(xbytes *mem)
{
    uint8_t *ptr = mem->ptr;
    free(mem);
    return ptr;
}
