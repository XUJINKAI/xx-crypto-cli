#include "cmdparser.h"
#include "global.h"

struct
{
    bool version;
    bool log_secret;
    bool log_verbose;
    bool log_debug;
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
            // &sc_sm2,
            &sc_sm4,
            CMDP_DOC("\nData process:\n"),
            &sc_data,
            &sc_hex,
            // &sc_base64,
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
            {0, "no-color", "Print no color", CMDP_TYPE_BOOL, &g_state.no_color},
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

    if (params->next == NULL)
    {
#ifdef _WIN32
        g_state.repl_mode = true;
#endif
        return CMDP_ACT_OK | CMDP_ACT_SHOW_HELP;
    }

    return CMDP_ACT_CONTINUE;
}
