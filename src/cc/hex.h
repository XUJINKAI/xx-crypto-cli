#pragma once

#include "global.h"

void XIO_HEX_write(XIO *io, const void *__ptr, size_t __len);

char *bytes_to_hex(const uint8_t *bytes, size_t len);
XX_MEM *hex_to_mem(const char *hex, size_t len);
