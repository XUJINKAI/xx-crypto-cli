#include "common.h"

UTEST(pbkdf2, 123)
{
    CMD_START();
    CMD_RUN("xx", "pbkdf2", "-p", "123", "-n", "10");
    EXPECT_EQ(cmd_result, 0);
    CMD_END();
}
