#pragma once

#include "../asn1.h"
#include "base64.h"
#include "cformat.h"
#include "global.h"
#include "hex.h"

char *cc_str_while_cb(char *str, bool (*cb)(char c));

/* 
pem
 */

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
