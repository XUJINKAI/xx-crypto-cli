#pragma once

#include "global.h"
#include "gmssl/sm2.h"

size_t cc_sm2_private_key_print_raw(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_raw(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_compressed_raw(XIO *io, const SM2_KEY *key);

size_t cc_sm2_private_key_print_pem(XIO *io, const SM2_KEY *key);
size_t cc_sm2_public_key_print_pem(XIO *io, const SM2_KEY *key);

RESULT cc_sm2_sign(XIO *instream, const SM2_KEY *key, const char *id, size_t idlen, size_t bufsize, uint8_t *sig,
                   size_t *siglen);

RESULT cc_sm2_try_load_private_key(SM2_KEY *sm2key, const char *key, size_t keylen, const char *pass);
