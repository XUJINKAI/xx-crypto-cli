#include "common.h"

UTEST(rand, 10)
{
    CMD_START();
    CMD_RUN("xx", "rand", "-n", "10", "--hex");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}
