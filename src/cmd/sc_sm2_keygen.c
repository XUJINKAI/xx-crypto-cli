#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/sm2.h"


static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    bool raw;
    bool pem;
} args;

cmdp_command_st sc_sm2_keygen = {
    .name = "keygen",
    .desc = "SM2 Key Generator",
    .doc  = "sm2 keygen [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    if (params->argc != 0)
    {
        LOG_ERR("Invalid argument count");
        return CMDP_ACT_ERROR;
    }
    SM2_KEY key;
    sm2_key_generate(&key);
    sm2_key_print(stdout, 0, 0, "", &key);

    clear_buffer(&key, sizeof(key));
    return CMDP_ACT_OK;
}
