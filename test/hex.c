#include "common.h"

UTEST(hex_test, encode)
{
    START_CMD();
    RUN_CMD("xx", "hex", "123");
    EXPECT_CMD(0, "313233", "");
    END_CMD();
}
