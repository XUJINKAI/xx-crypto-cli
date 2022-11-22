#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/sm2.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

extern cmdp_command_st sc_sm2_keygen;
extern cmdp_command_st sc_sm2_sign;
extern cmdp_command_st sc_sm2_verify;
extern cmdp_command_st sc_sm2_enc;
extern cmdp_command_st sc_sm2_dec;
cmdp_command_st sc_sm2 = {
    .name = "sm2",
    .desc = "Sub-Cmds: keygen, sign/verify, enc/dec",
    .doc  = "SM2 asymmetric algorithm\n",
    .sub_commands =
        (cmdp_command_st *[]){
            &sc_sm2_keygen,
            &sc_sm2_sign,
            // &sc_sm2_verify,
            // &sc_sm2_enc,
            // &sc_sm2_dec,
            NULL,
        },
    .options =
        (cmdp_option_st[]){
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    if (params->next == NULL)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    return CMDP_ACT_CONTINUE;
}
