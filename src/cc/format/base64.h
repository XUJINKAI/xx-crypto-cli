#pragma once

#include "global.h"

bool cc_is_base64_char(char c);

#define CC_BASE64_ENC_LINE_LEN_MASK 0xffff
#define CC_BASE64_ENC_BREAK_LINE    (64 & CC_BASE64_ENC_LINE_LEN_MASK)
#define CC_BASE64_ENC_URL           0x10000

// flag: Mask 0xffff for line length
char *cc_base64_encode(const uint8_t *__ptr, size_t __len, int flag);

#define CC_BASE64_DEC_IGNORE_SPACE     0
#define CC_BASE64_DEC_IGNORE_NONBASE64 0x1
xbytes *cc_base64_decode(const char *in, size_t inlen, int flag);
