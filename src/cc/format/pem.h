#pragma once

#include "global.h"

typedef struct
{
    const char *data;
    size_t datalen;
    const char *label;
    size_t labellen;
    const char *pem_start;
    const char *pem_end;
} CC_PEM_DATA;

RESULT cc_pem_find(const char *text, CC_PEM_DATA *out);
xarray_of(CC_PEM_DATA) * cc_pem_find_all(const char *text);
