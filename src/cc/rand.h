#pragma once

#include "global.h"
#include "xio/xio.h"

// source: NULL(soft), rdrand, rdseed, sdf:LIB.so
RESULT cc_random_bytes(const char *source, XIO *out, size_t len);
RESULT cc_random_bytes_sdf(const char *libname, XIO *out, size_t len);

RESULT cc_random_check(const uint8_t *rand, size_t len);
