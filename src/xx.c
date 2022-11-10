#include "cmdp.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/console.h"
#include "utils/unistd.h"
#include <signal.h>

// global
APP_STATE g_state = {0};

static void repl()
{
    int argc;
    char **argv;
    printf("\n"); // after main's
    while (1)
    {
        char *line = console_readline(TC_GREEN, "xx> ");
        if (line == NULL || strlen(line) == 0)
        {
            free(line);
            continue;
        }
        commandline_to_args(line, &argc, &argv);
        run_command(argc, argv);
        free(line);
        printf("\n"); // after repl's
    }
}

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

int xx_main(int argc, char *argv[], FILE *in_stream, FILE *out_stream, FILE *log_stream)
{
    g_state.in  = in_stream;
    g_state.out = out_stream;
    log_set_stream(log_stream);
    signal(SIGINT, signal_handler);

#ifdef _WIN32
#include <Windows.h>
    SetConsoleOutputCP(CP_UTF8);
    LOG_WARN("注意：Windows控制台默认输入为GBK编码，本程序输出为UTF8编码。");
#endif
#ifdef _DEBUG
    // log_enable_type(_LOGT_DEBUG, true);
#endif

    int r = run_command(argc - 1, argv + 1);
    if (g_state.repl_mode)
    {
        repl();
    }
    return r;
}

#ifndef XX_NO_MAIN
int main(int argc, char *argv[])
{
    return xx_main(argc, argv, stdin, stdout, stderr);
}
#endif
