#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"

static void hex_before(cmdp_before_param_st *params);
static cmdp_action_t hex_process(cmdp_process_param_st *params);

static struct
{
    bool decode;
    char *infile;
    char *outfile;
} hex_args;

cmdp_command_st sc_hex = {
    .name = "hex",
    .desc = "hex encode/decode",
    .doc  = "hex [-d] STRING\n",
    .options =
        (cmdp_option_st[]){
            {'d', "decode", "decode", CMDP_TYPE_BOOL, &hex_args.decode},
            {'r', "read", "input file", CMDP_TYPE_STRING_PTR, &hex_args.infile, "<file>"},
            {'w', "write", "output file", CMDP_TYPE_STRING_PTR, &hex_args.outfile, "<file>"},
            {0},
        },
    .fn_before  = hex_before,
    .fn_process = hex_process,
};


static void hex_before(cmdp_before_param_st *params)
{
    memset(&hex_args, 0, sizeof(hex_args));
}
static cmdp_action_t hex_process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG_C(0, "too many arguments");
        return CMDP_ACT_FAIL;
    }
    XIO *instream  = NULL;
    XIO *outstream = NULL;
    if (hex_args.infile)
    {
        instream = XIO_new_file(hex_args.infile, "rb");
    }
    else
    {
        instream = XIO_new_from_string(params->argv[0], false);
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
        instream = XIO_new_filter_hex(instream);
    }
    else
    {
        outstream = XIO_new_filter_hex(outstream);
    }
    XIO_copy(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
    return CMDP_ACT_OVER;
}
