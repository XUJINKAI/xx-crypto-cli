#include "cmdparser.h"
#include "global.h"

struct
{
    bool repl_mode;
    bool log_secret;
    bool log_verbose;
    bool log_debug;
    bool upper_hex;
    bool no_eol;
    bool no_color;
    bool version;
} arg_main = {0};

static cmdp_action_t main_process(cmdp_process_param_st *params);

extern cmdp_command_st sc_rand;
extern cmdp_command_st sc_randcheck;
extern cmdp_command_st sc_hash;
extern cmdp_command_st sc_sm2;
extern cmdp_command_st sc_sm4;

extern cmdp_command_st sc_data;
extern cmdp_command_st sc_hex;
extern cmdp_command_st sc_base64;
extern cmdp_command_st sc_asn1;

extern cmdp_command_st sc_chat;
extern cmdp_command_st sc_gen;

cmdp_command_st main_cmdp = {
    .doc = "Usage: xx [OPTIONS] COMMAND [ARGS]\n"
           "xx is a command line tool for crypto data process.\n",
    .sub_commands =
        (cmdp_command_st *[]){
            CMDP_DOC("\nStandard crypto:\n"),
            &sc_rand,
            &sc_randcheck,
            &sc_hash,
            &sc_sm2,
            &sc_sm4,
            CMDP_DOC("\nData process:\n"),
            &sc_data,
            &sc_hex,
            // &sc_base64,
            &sc_asn1,
            CMDP_DOC("\nUtility:\n"),
            &sc_chat,
            &sc_gen,
            CMDP_DOC("\n"
                     "Options:\n"),
            NULL,
        },
    .options =
        (cmdp_option_st[]){
            {0, "log-secret", "Show secret log", CMDP_TYPE_BOOL, &arg_main.log_secret},
            {0, "log-verbose", "Show verbose log", CMDP_TYPE_BOOL, &arg_main.log_verbose},
            {0, "log-debug", "Show debug log", CMDP_TYPE_BOOL, &arg_main.log_debug},
            {0, "no-eol", "No end of line", CMDP_TYPE_BOOL, &arg_main.no_eol},
            {0, "no-color", "Print no color", CMDP_TYPE_BOOL, &arg_main.no_color},
            {0, "upper-hex", "Show uppercase hex", CMDP_TYPE_BOOL, &arg_main.upper_hex},
            {0, "repl", "Into interactive mode", CMDP_TYPE_BOOL, &arg_main.repl_mode},
            {0, "version", "Show version", CMDP_TYPE_BOOL, &arg_main.version, CMDP_HIDE},
            {0},
        },
    .doc_tail = "\n"
                "Use -h/--help to show this help, --version to show version.\n"
                "Homepage: https://github.com/XUJINKAI/crypto-tool\n",
    .fn_process = main_process,
};

static cmdp_action_t main_process(cmdp_process_param_st *params)
{
    if (arg_main.no_eol)
    {
        g_state.no_eol = true;
    }
    if (arg_main.no_color)
    {
        g_state.no_color = true;
    }
    if (arg_main.repl_mode)
    {
        g_state.repl_mode = true;
    }
    if (arg_main.upper_hex)
    {
        g_state.upper_hex = true;
    }
    if (arg_main.log_debug)
    {
        log_set_level(_LOGT_DEBUG, true);
        LOG_DBG("Debug log enabled");
        LOG_DBG("isatty(stdin) : %s", isatty(0) ? "true" : "false");
        LOG_DBG("isatty(stdout): %s", isatty(1) ? "true" : "false");
    }
    if (arg_main.log_verbose)
    {
        log_set_level(_LOGT_VERBOSE, true);
        LOG_VERBOSE("Verbose log enabled");
        LOG_VERBOSE("PID: %d", getpid());
    }
    if (arg_main.log_secret)
    {
        log_set_level(_LOGT_SECRET, true);
        LOG_DBG("Secret log enabled");
    }

    if (arg_main.version)
    {
        char *build_type =
#ifdef _DEBUG
            "Debug";
#else
            "Release";
#endif
        XIO_printf(g_state.out, "xx version %s %s %s %s build\n", GIT_TAG, __DATE__, __TIME__, build_type);
        return CMDP_ACT_OK;
    }

    if (params->next == NULL)
    {
#ifdef _WIN32
        g_state.repl_mode = true;
#endif
        return CMDP_ACT_OK | CMDP_ACT_SHOW_HELP;
    }

    return CMDP_ACT_CONTINUE;
}
