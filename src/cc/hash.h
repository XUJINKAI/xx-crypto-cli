#pragma once

#include "global.h"
#include "gmssl/digest.h"
#include "xio/xio.h"

RESULT cc_hash_sm3(XIO *in, XIO *out);
RESULT cc_hash(const DIGEST *digest, XIO *in, XIO *out, size_t bufsize);
