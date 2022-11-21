#include "common.h"

UTEST(app, version)
{
    CMD_START();
    CMD_RUN("xx", "--version");
    EXPECT_EQ(0, cmd_result);
    CMD_END();
}

UTEST(app, help)
{
    CMD_START();
    CMD_RUN("xx", "--help");
    EXPECT_EQ(0, cmd_result);
    CMD_END();
}

UTEST(app, unknown_command)
{
    CMD_START();
    CMD_RUN("xx", "unknown_command");
    EXPECT_EQ(1, cmd_result);
    CMD_END();
}
