#pragma once

#include "global.h"
#include "gmssl/hex.h"

char *bytes_to_hex(const uint8_t *bytes, size_t len);
xbytes *hex_to_xbytes(const char *hex, size_t len);

RESULT hex_to_bytes_expect_len(const char *hex, size_t expectlen, uint8_t *bytes);
RESULT hex_to_bytes_max_len(const char *hex, size_t maxlen, uint8_t *bytes, size_t *len);
