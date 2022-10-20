#include "cipher.h"

struct CC_CIPHER_CTX_st
{
    uint32_t algr;
    uint32_t direction;
    void *ctx;
};

CC_CIPHER_CTX *CIPHER_new(uint32_t algr, uint32_t direction);
bool CIPHER_set_key(const uint8_t *key, size_t key_len);
bool CIPHER_set_iv(const uint8_t *iv, size_t iv_len);
bool CIPHER_update(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);
bool CIPHER_finish(uint8_t *out, size_t *out_len);
void CIPHER_free(CC_CIPHER_CTX *ctx);
