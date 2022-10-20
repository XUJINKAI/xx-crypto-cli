#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct CC_CIPHER_CTX_st CC_CIPHER_CTX;

CC_CIPHER_CTX *CC_CIPHER_new(uint32_t algr, uint32_t direction);
bool CC_CIPHER_set_key(const uint8_t *key, size_t key_len);
bool CC_CIPHER_set_iv(const uint8_t *iv, size_t iv_len);
bool CC_CIPHER_update(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);
bool CC_CIPHER_finish(uint8_t *out, size_t *out_len);
void CC_CIPHER_free(CC_CIPHER_CTX *ctx);
