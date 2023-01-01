#include "cc/format/hex.h"
#include "common.h"

#define SIG_HEX                                                                                                        \
    "3045022100ace746cc4bc90c57de3987ad31b241c2809e811f7371f1490524c4db28353d410220257653a8e02b965f2e6c10985ffc8381fdd1b3e76c085beea97482d837d8c904"
#define SIG_BASE64 "MEUCIQCs50bMS8kMV945h60xskHCgJ6BH3Nx8UkFJMTbKDU9QQIgJXZTqOArll8ubBCYX/yDgf3Rs+dsCFvuqXSC2DfYyQQK"

UTEST(asn1, hex_string)
{
    CMD_START();
    CMD_RUN("xx", "asn1", "-s", SIG_HEX);
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}

UTEST(asn1, base64_string)
{
    CMD_START();
    CMD_RUN("xx", "asn1", "-s", SIG_BASE64);
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}

UTEST(asn1, stdin_raw_der)
{
    CMD_START();
    xbytes *b = cc_hex_decode(SIG_HEX, strlen(SIG_HEX), 0);
    CMD_SET_INSTREAM(xbytes_buffer(b), xbytes_length(b));
    CMD_RUN("xx", "asn1");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
    xbytes_free(b);
}

UTEST(asn1, sm2key_openssl_pem)
{
    CMD_START();
    CMD_RUN("xx", "asn1", "--in", EXAMPLES_DIR "sm2key.openssl.pem");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}
