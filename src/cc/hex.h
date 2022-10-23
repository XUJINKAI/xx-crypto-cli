#pragma once

#include "global.h"

char *bytes_to_hex(const uint8_t *bytes, size_t len);
XX_MEM *hex_to_mem(const char *hex, size_t len);
