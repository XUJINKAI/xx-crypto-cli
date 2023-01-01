#pragma once

#include "global.h"
#include "gmssl/block_cipher.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint8_t key[16];
    uint8_t iv[16];
} CC_SM4_PARAM;

RESULT cc_sm4_cbc_encrypt(CC_SM4_PARAM *param, XIO *in, XIO *out);
RESULT cc_sm4_cbc_decrypt(CC_SM4_PARAM *param, XIO *in, XIO *out);
