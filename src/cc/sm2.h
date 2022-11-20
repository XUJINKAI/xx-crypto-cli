#pragma once

#include "global.h"
#include "gmssl/sm2.h"

size_t cc_sm2_key_print_uncompressed(XIO *io, const SM2_KEY *key);
size_t cc_sm2_key_print_compresed(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_pem(XIO *io, const SM2_KEY *key);
size_t cc_sm2_private_key_print_pem(XIO *io, const SM2_KEY *key);
