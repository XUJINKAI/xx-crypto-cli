#pragma once

#include "global.h"
#include "gmssl/sm2.h"

size_t cc_sm2_private_key_print_raw(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_raw(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_compressed_raw(XIO *io, const SM2_KEY *key);

size_t cc_sm2_private_key_print_pem(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_pem(XIO *io, const SM2_KEY *key);
