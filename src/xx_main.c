#include "cmdparser.h"
#include "global.h"

struct
{
    bool version;
    bool verbose;
} arg_main = {0};

static cmdp_action_t main_process(cmdp_process_param_st *params);

extern cmdp_command_st sc_echo;
extern cmdp_command_st sc_gen;
extern cmdp_command_st sc_hex;
extern cmdp_command_st sc_rand;
extern cmdp_command_st sc_sm4;
extern cmdp_command_st sc_chat;
static cmdp_command_st main_cmdp = {
    .doc = "Usage: xx [OPTIONS] COMMAND [ARGS]\n"
           "xx is a command line tool for crypto data process.\n"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, "version", "Show version", CMDP_TYPE_BOOL, &arg_main.version},
            {0, "verbose", "Verbose log", CMDP_TYPE_BOOL, &arg_main.verbose},
            {0},
        },
    .sub_commands =
        (cmdp_command_st *[]){
            CMDP_DOC("\nCommands:\n"),
            &sc_echo,
            &sc_gen,
            &sc_hex,
            &sc_rand,
            &sc_sm4,
            CMDP_DOC("\n"),
            &sc_chat,
            CMDP_DOC("\n"
                     "GitHub: https://github.com/XUJINKAI/crypto-tool\n"),
            NULL,
        },
    .fn_process = main_process,
};

int xx_main(int argc, char *argv[])
{
    return cmdp_run(argc - 1, argv + 1, &main_cmdp);
}

static cmdp_action_t main_process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
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
