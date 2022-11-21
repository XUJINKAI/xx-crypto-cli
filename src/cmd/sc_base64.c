#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    bool decode;
    char *infile;
    char *outfile;
} hex_args;

cmdp_command_st sc_base64 = {
    .name = "base64",
    .desc = "base64 encode/decode",
    .doc  = "base64 [-d] STRING\n",
    .options =
        (cmdp_option_st[]){
            {'d', "decode", "decode", CMDP_TYPE_BOOL, &hex_args.decode},
            {'r', "read", "input file", CMDP_TYPE_STRING_PTR, &hex_args.infile, "<file>"},
            {'w', "write", "output file", CMDP_TYPE_STRING_PTR, &hex_args.outfile, "<file>"},
            {0},
        },
    .fn_process = __process,
    .alias_name = "b",
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG_ERROR("too many arguments");
        return CMDP_ACT_ERROR;
    }
    XIO *instream  = NULL;
    XIO *outstream = NULL;
    instream       = cmd_get_instream(CMDP_GET_ARG(params, 0), hex_args.infile, true);
    outstream      = cmd_get_outstream(hex_args.outfile, true);

    if (hex_args.decode)
    {
        instream = XIO_newf_hex(instream);
    }
    else
    {
        outstream = XIO_newf_hex(outstream);
    }
    XIO_drain(instream, outstream);
    xio_close_safe(instream);
    xio_close_safe(outstream);
    return CMDP_ACT_OK;
}
