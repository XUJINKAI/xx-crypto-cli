#pragma once

#include "global.h"
#include "gmssl/hex.h"

char *bytes_to_hex(const uint8_t *bytes, size_t len);
xbytes *hex_to_xbytes(const char *hex, size_t len);
RESULT hex_str_expect_to_bytes(const char *hex, size_t expectlen, uint8_t *bytes);
