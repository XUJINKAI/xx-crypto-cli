#pragma once

#include "global.h"

bool cc_is_hex_char(char c);

#define CC_HEX_ENC_LINE_LEN_MASK 0xffff
#define CC_HEX_ENC_BREAK_LINE    (64 & CC_HEX_ENC_LINE_LEN_MASK)
#define CC_HEX_ENC_UPPERCASE     0x10000
#define CC_HEX_ENC_LOWERCASE     0x20000

// flag: 0 no line break, 0xffff auto line break
char *cc_hex_encode(const uint8_t *bytes, size_t len, int flag);

#define CC_HEX_DEC_IGNORE_SPACE  0
#define CC_HEX_DEC_IGNORE_NONHEX 0x1
// flag: 0 for ignore space (always), 0x1 for ignore non-hex
xbytes *cc_hex_decode(const char *in, size_t inlen, int flag);

RESULT cc_hex_dec_expect_len(const char *hex, size_t expectlen, uint8_t *bytes);
RESULT cc_hex_dec_max_len(const char *hex, size_t maxlen, uint8_t *bytes, size_t *len);
