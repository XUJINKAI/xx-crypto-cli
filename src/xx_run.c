#include "cmdp.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/console.h"
#include "utils/unistd.h"

// global
APP_STATE g_state = {0};

static void repl()
{
    int argc;
    char **argv;
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
    }
}

int xx_run(int argc, char *argv[], XIO *instream, XIO *outstream, FILE *errstream)
{
    g_state.in  = instream;
    g_state.out = outstream;
    log_set_stream(errstream);

#ifdef _DEBUG
    // g_state.repl_mode = true;
    // log_set_level(_LOGT_DEBUG, true);
#endif

    int r = run_command(argc - 1, argv + 1);
    if (g_state.repl_mode)
    {
        repl();
    }
    return r;
}
