#include "common.h"

UTEST(sm2_keygen, default)
{
    CMD_START();
    CMD_RUN("xx", "sm2", "keygen");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}

UTEST(sm2_keygen, pem)
{
    CMD_START();
    CMD_RUN("xx", "sm2", "keygen", "--pem");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}

UTEST(sm2_keygen, specify_sk)
{
    CMD_START();
    CMD_RUN("xx", "sm2", "keygen", "--sk", "883fa9f9bc0443028411f2dcef966cceb80be1845ed187f0edbe932fe66381e1");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}
