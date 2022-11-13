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
    if (hex_args.infile)
    {
        instream = XIO_new_file(hex_args.infile, "rb");
    }
    else
    {
        instream = XIO_new_string(params->argv[0], false);
    }
    if (hex_args.outfile)
    {
        outstream = XIO_new_file(hex_args.outfile, "wb");
    }
    else
    {
        outstream = XIO_new_fp(stdout, false);
    }
    if (hex_args.decode)
    {
        instream = XIO_newf_hex(instream);
    }
    else
    {
        outstream = XIO_newf_hex(outstream);
    }
    XIO_drain(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
    return CMDP_ACT_OK;
}
