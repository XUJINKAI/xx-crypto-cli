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
} __args;

cmdp_command_st sc_hex = {
    .name = "hex",
    .desc = "hex encode/decode",
    .doc  = "hex [-d] STRING\n",
    .options =
        (cmdp_option_st[]){
            {'d', "decode", "decode", CMDP_TYPE_BOOL, &__args.decode},
            _opt_infile(__args.infile, ),
            _opt_outfile(__args.outfile, ),
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_ARG_COUNT(params, 0, 1);

    XIO *instream  = NULL;
    XIO *outstream = NULL;

    instream  = cmd_get_instream(CMDP_GET_ARG(params, 0), __args.infile, true);
    outstream = cmd_get_outstream(__args.outfile, true);

    if (__args.decode)
    {
        instream = XIO_newf_hex(instream);
    }
    else
    {
        outstream = XIO_newf_hex(outstream);
    }
    LOG_DBG_XIO("in : ", instream);
    LOG_DBG_XIO("out: ", outstream);

    XIO_drain(instream, outstream);

    XIO_CLOSE_SAFE(instream);
    XIO_CLOSE_SAFE(outstream);
    return CMDP_ACT_OK;
}
