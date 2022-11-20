#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/sm2.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *informat;
    char *infile;
    char *outformat;
    char *outfile;
} args;

cmdp_command_st sc_sm2_enc = {
    .name = "sm2",
    .desc = "SM2 asymmetric algorithm",
    .doc  = "ecdhe [OPTIONS] <TEXT>\n",
    .options =
        (cmdp_option_st[]){
            {'I', "in-format", "Input Format", CMDP_TYPE_STRING_PTR, &args.informat, "<FORMAT>"},
            {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &args.outformat, "<FORMAT>"},
            {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &args.infile, "<filepath>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &args.outfile, "<filepath>"},
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    SM2_KEY key;
    sm2_key_generate(&key);

    return CMDP_ACT_OK;
}
