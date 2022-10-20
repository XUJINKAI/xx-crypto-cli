#ifndef __XCRYPTO_H__
#define __XCRYPTO_H__

#include "global.h"
#include "xio/xio.h"

int x_random_bytes(XIO *out, size_t len);
int x_sm3(XIO *in, XIO *out);
int x_sm4_cbc_encrypt(const uint8_t key[16], const uint8_t iv[16], XIO *in, XIO *out);
int x_sm4_cbc_decrypt(const uint8_t key[16], const uint8_t iv[16], XIO *in, XIO *out);

#endif /* __XCRYPTO_H__ */
