#include "common.h"

UTEST_STATE();

int main(int argc, const char *const argv[])
{
    return utest_main(argc, argv);
}

// UTEST(main, fail_test)
// {
//     strdup("Hello World"); // fail in valgrind
//     ASSERT_EQ(1, 2);       // fail anyway
// }
