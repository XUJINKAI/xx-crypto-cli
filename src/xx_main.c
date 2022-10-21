#include "cmdparser.h"
#include "global.h"
#include "utils/console.h"
#include <signal.h>

APP_STATE g_state = {
    .repl_mode      = false,
    .sigint_no_exit = false,
};

static cmdp_flag_t opt_repl_disable()
{
    return g_state.repl_mode ? CMDP_FLAG_DISABLE : CMDP_FLAG_NONE;
}

struct
{
    bool help;
    bool version;
    bool verbose;
} arg_main = {0};

static void main_before(cmdp_before_param_st *params);
static cmdp_action_t main_process(cmdp_process_param_st *params);

extern cmdp_command_st sc_echo;
extern cmdp_command_st sc_gen;
extern cmdp_command_st sc_hex;
extern cmdp_command_st sc_rand;
extern cmdp_command_st sc_sm4;
extern cmdp_command_st sc_chat;
static cmdp_command_st main_cmdp = {
    .doc = "Usage: xx [OPTIONS] COMMAND [ARGS]\n"
           "xx is a command line tool for crypto data process.\n",
    .sub_commands =
        (cmdp_command_st *[]){
            CMDP_DOC("\nData gen/process:\n"),
            &sc_echo,
            &sc_gen,
            &sc_hex,
            CMDP_DOC("\nCommon crypto:\n"),
            &sc_rand,
            &sc_sm4,
            CMDP_DOC("\nHandy tool:\n"),
            &sc_chat,
            CMDP_DOC("\n"
                     "Options:\n"),
            NULL,
        },
    .options =
        (cmdp_option_st[]){
            {0, "repl", "Into interactive mode", CMDP_TYPE_BOOL, &g_state.repl_mode, .fn_flag = opt_repl_disable},
            {0, "verbose", "Verbose log", CMDP_TYPE_BOOL, &arg_main.verbose},
            {0, "version", "Show version", CMDP_TYPE_BOOL, &arg_main.version, CMDP_HIDE},
            {'h', "help", "Show this help", CMDP_TYPE_BOOL, &arg_main.help, CMDP_HIDE},
            {0},
        },
    .doc_tail = "\n"
                "Use -h/--help to show this help, --version to show version.\n"
                "GitHub: https://github.com/XUJINKAI/crypto-tool\n",
    .fn_before  = main_before,
    .fn_process = main_process,
};

static void repl()
{
    int argc;
    char **argv;
    printf("\n"); // after main's
    while (1)
    {
        u_char *line = console_readline("XX> ");
        if (line == NULL || strlen(line) == 0)
        {
            free(line);
            continue;
        }
        commandline_to_args(line, &argc, &argv);
        cmdp_run(argc, argv, &main_cmdp);
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

int xx_main(int argc, char *argv[])
{
#ifdef _DEBUG
    tty_puts(stdout, TC_GREEN);
    printf("DEBUG MODE. PID: %d\n"
           "CMD:",
           getpid());
    for (size_t i = 0; i < argc; i++)
    {
        printf(" %s", argv[i]);
    }
    tty_puts(stdout, TC_RESET);
    printf("\n");
#endif

    signal(SIGINT, signal_handler);

    // cmdp's help is short-circuit parse, but all options are needed.
    cmdp_global_config_st *cmdp_cfg = cmdp_get_global_config();
    cmdp_cfg->help_short_option     = 0;
    cmdp_cfg->help_long_option      = NULL;

    int r = cmdp_run(argc - 1, argv + 1, &main_cmdp);
    if (g_state.repl_mode)
    {
        repl();
    }
    return r;
}

static void main_before(cmdp_before_param_st *params)
{
    memset(&arg_main, 0, sizeof(arg_main));
}

static cmdp_action_t main_process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0 || arg_main.help)
    {
        return CMDP_ACT_OVER | CMDP_ACT_SHOW_HELP;
    }
    if (arg_main.version)
    {
        printf("Version: %s\n", "0.0.1");
        return CMDP_ACT_OVER;
    }
    if (arg_main.verbose)
    {
        log_level = 1;
    }
    return CMDP_ACT_CONTINUE;
}
