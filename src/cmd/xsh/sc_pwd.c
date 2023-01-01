#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *_;
} args;

cmdp_command_st sc_pwd = {
    .name = "pwd",
    .desc = "print name of current/working directory",
    .doc  = "Usage: pwd [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            {0},
        },
    .fn_process = __process,
    CMDP_HIDE,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    XIO *outstream = NULL;
    outstream      = cmd_get_outstream(NULL, true);
    char *pwd      = getcwd(NULL, 0);
    XIO_printf(outstream, "%s\n", pwd);
    free(pwd);
    xio_close_safe(outstream);
    return CMDP_ACT_OK;
}
