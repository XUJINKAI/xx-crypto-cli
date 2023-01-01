#pragma once

#include "global.h"

typedef struct
{
    uint32_t tagClass;
    uint32_t tagConstructed;
    uint32_t tagNumber;
    int64_t length;
    uint8_t *tagPtr;
    uint8_t *lengthPtr;
    uint8_t *dataPtr;
} ASN1_PARSE_INFO;

RESULT cc_asn1_parse_1(const uint8_t *__ptr, size_t __len, ASN1_PARSE_INFO *outinfo);

typedef RESULT (*ASN1_PARSE_CALLBACK)(ASN1_PARSE_INFO *info, int level, void *arg);
RESULT cc_asn1_parse_cb(uint8_t *__ptr, size_t __len, ASN1_PARSE_CALLBACK cb, void *arg);

RESULT cc_asn1_print(XIO *__stream, uint8_t *__ptr, size_t __len);
RESULT cc_is_asn1(uint8_t *__ptr, size_t __len);
