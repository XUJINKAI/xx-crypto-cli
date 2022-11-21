#pragma once

#include "global.h"

#define CONV_NULL_OK       0x01 // NULL string will return OK (do nothing)
#define CONV_HEX_PREFIX    0x02 // Allow 0x prefix
#define CONV_KMGB_SUFFIX   0x04 // Allow k/m/g[b] suffix
#define CONV_NEGATIVE_FAIL 0x08 // Negative number will return FAIL
// e.g. 16, 0x10, 1k allowd, -1 not allowed
#define CONV_SIZE (CONV_HEX_PREFIX | CONV_KMGB_SUFFIX | CONV_NEGATIVE_FAIL)
RESULT string_to_long(const char *__str, long *__out, int flag);
