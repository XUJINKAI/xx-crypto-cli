#include "common.h"

UTEST_STATE();

int main(int argc, const char *const argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (STREQ_NoCase(argv[i], "--debug"))
        {
            log_set_level(_LOGT_VERBOSE, true);
            log_set_level(_LOGT_DEBUG, true);
            log_set_level(_LOGT_SECRET, true);
        }
    }
    return utest_main(argc, argv);
}

// UTEST(main, fail_test)
// {
//     strdup("Hello World"); // fail in valgrind
//     ASSERT_EQ(1, 2);       // fail anyway
// }
