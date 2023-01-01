#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct xbytes xbytes;

xbytes *xbytes_new(size_t capacity);
xbytes *xbytes_new_copy(void *ptr, size_t len);
void *xbytes_buffer(xbytes *mem);
size_t xbytes_length(xbytes *mem);
// return: actual remain size
size_t xbytes_require_size(xbytes *mem, size_t required_remain_size);
void xbytes_clean(xbytes *mem);
size_t xbytes_write(xbytes *mem, void *ptr, size_t len);
size_t xbytes_printf(xbytes *mem, const char *fmt, ...);
size_t xbytes_vprintf(xbytes *mem, const char *fmt, va_list ap);
void xbytes_free(xbytes *mem);
uint8_t *xbytes_detach_and_free(xbytes *mem);

#define xbytes_free_safe(mem)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mem)                                                                                                       \
        {                                                                                                              \
            xbytes_free(mem);                                                                                          \
            mem = NULL;                                                                                                \
        }                                                                                                              \
    } while (0)
