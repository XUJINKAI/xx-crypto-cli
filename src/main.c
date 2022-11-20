#include "global.h"
#include "xio/xio.h"
#include <signal.h>
#include <stdio.h>

int xx_run(int argc, char *argv[], XIO *instream, XIO *outstream, XIO *errstream);

static void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        if (!g_state.sigint_no_exit)
        {
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    XIO *instream  = XIO_new_fp(stdin, 0);
    XIO *outstream = XIO_new_fp(stdout, 0);
    XIO *errstream = XIO_new_fp(stderr, 0);
    log_set_stream(errstream);

#ifdef _WIN32
#include <Windows.h>
    SetConsoleOutputCP(CP_UTF8);
    LOG_WARN("注意：Windows控制台默认输入为GBK编码，本程序输出为UTF8编码。");
#endif

    int r = xx_run(argc, argv, instream, outstream, errstream);
    XIO_close(instream);
    XIO_close(outstream);
    XIO_close(errstream);
    return r;
}
