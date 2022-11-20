#include "utest.h"
#include "xio/xio.h"

UTEST(xio, dup_fp_out)
{
    XIO *outstream = XIO_new_fp(stdout, 0);
    XIO *dupout    = XIO_dup(outstream);
    XIO_printf(dupout, "Hello, world!\n");
    XIO_close(dupout);
    XIO_close(outstream);
}

UTEST(xio, mbuilder)
{
    XIO *mbuilder = XIO_new_mbuilder();
    int _a        = 123;
    long _b       = 456;
    XIO_printf(mbuilder, "%d, %ld", _a, _b);
    char *p  = XIO_MBUILDER_get_buffer(mbuilder);
    size_t l = XIO_MBUILDER_get_size(mbuilder);
    EXPECT_STREQ("123, 456", p);
    EXPECT_EQ(strlen(p), l);
    XIO_close(mbuilder);
}
