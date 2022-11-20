#include "common.h"

UTEST(hex, encode_stdin)
{
    CMD_START();
    CMD_SET_INSTREAM("123", 3);
    CMD_RUN("xx", "hex");
    EXPECT_EQ(cmd_result, 0);
    EXPECT_STREQ("313233\n", cmd_out);
    CMD_END();
}

UTEST(hex, encode_string)
{
    CMD_START();
    CMD_RUN("xx", "hex", "123");
    EXPECT_EQ(cmd_result, 0);
    EXPECT_STREQ("313233\n", cmd_out);
    CMD_END();
}
