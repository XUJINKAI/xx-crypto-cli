#include "cmdparser.h"
#include "cross-platform/unistd.h"
#include "global.h"
#include "utils/console.h"
#include <signal.h>

struct
{
    bool version;
    bool log_secret;
    bool log_verbose;
    bool log_debug;
} arg_main = {0};

static cmdp_action_t main_process(cmdp_process_param_st *params);

extern cmdp_command_st sc_rand;
extern cmdp_command_st sc_hash;
extern cmdp_command_st sc_sm2;
extern cmdp_command_st sc_sm4;

extern cmdp_command_st sc_data;
extern cmdp_command_st sc_hex;
extern cmdp_command_st sc_base64;

extern cmdp_command_st sc_chat;
extern cmdp_command_st sc_gen;

static cmdp_command_st main_cmdp = {
    .doc = "Usage: xx [OPTIONS] COMMAND [ARGS]\n"
           "xx is a command line tool for crypto data process.\n",
    .sub_commands =
        (cmdp_command_st *[]){
            CMDP_DOC("\nStandard crypto:\n"),
            &sc_rand,
            &sc_hash,
            &sc_sm2,
            &sc_sm4,
            CMDP_DOC("\nData process:\n"),
            &sc_data,
            &sc_hex,
            &sc_base64,
            CMDP_DOC("\nUtility:\n"),
            &sc_chat,
            &sc_gen,
            CMDP_DOC("\n"
                     "Options:\n"),
            NULL,
        },
    .options =
        (cmdp_option_st[]){
            {0, "repl", "Into interactive mode", CMDP_TYPE_BOOL, &g_state.repl_mode},
            {0, "log-secret", "Show secret log", CMDP_TYPE_BOOL, &arg_main.log_secret},
            {0, "log-verbose", "Show verbose log", CMDP_TYPE_BOOL, &arg_main.log_verbose},
            {0, "log-debug", "Show debug log", CMDP_TYPE_BOOL, &arg_main.log_debug},
            {0, "version", "Show version", CMDP_TYPE_BOOL, &arg_main.version, CMDP_HIDE},
            {0},
        },
    .doc_tail = "\n"
                "Use -h/--help to show this help, --version to show version.\n"
                "Homepage: https://github.com/XUJINKAI/crypto-tool\n",
    .fn_process = main_process,
};

static void repl(cmdp_ctx *ctx)
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
        cmdp_run(argc, argv, &main_cmdp, ctx);
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

// global
APP_STATE g_state                                         = {0};
void (*default_error_parse)(cmdp_error_params_st *params) = NULL;
void ctx_error_parse(cmdp_error_params_st *params)
{
    bool tty = isatty(fileno(params->err_stream));
    if (tty)
    {
        fprintf(params->err_stream, _LOGC_ERROR);
    }
    fprintf(params->err_stream, "xx: ");
    default_error_parse(params);
    if (tty)
    {
        fprintf(params->err_stream, TC_RESET);
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

    cmdp_ctx ctx = {0};
    cmdp_set_default_context(&ctx);
    default_error_parse = ctx.fn_error_parse;
    ctx.fn_error_parse  = ctx_error_parse;

    int r = cmdp_run(argc - 1, argv + 1, &main_cmdp, &ctx);
    LOG_DBG("exit code: %d", r);
    if (g_state.repl_mode)
    {
        repl(&ctx);
    }
    return r;
}

static cmdp_action_t main_process(cmdp_process_param_st *params)
{
    if (arg_main.log_debug)
    {
        log_enable_type(_LOGT_DEBUG, true);
        LOG_DBG("Debug log enabled");
    }
    if (arg_main.log_verbose)
    {
        log_enable_type(_LOGT_VERBOSE, true);
        LOG_VERBOSE("Verbose log enabled");
        LOG_VERBOSE("PID: %d", getpid());
    }
    if (arg_main.log_secret)
    {
        log_enable_type(_LOGT_SECRET, true);
        LOG_DBG("Secret log enabled");
    }

    if (params->next == NULL)
    {
        return CMDP_ACT_OK | CMDP_ACT_SHOW_HELP;
    }

    if (arg_main.version)
    {
        char *build_type =
#ifdef _DEBUG
            "Debug";
#else
            "Release";
#endif
        fprintf(stdout, "xx version %s %s %s %s build\n", GIT_TAG, __DATE__, __TIME__, build_type);
        return CMDP_ACT_OK;
    }

    return CMDP_ACT_CONTINUE;
}
