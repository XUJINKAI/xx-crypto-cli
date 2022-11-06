#include "cc/rand.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/convert.h"


static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *outformat;
    char *outfile;
} rand_args;

cmdp_command_st sc_rand = {
    .name = "rand",
    .desc = "generate random bytes",
    .doc  = "rand [options] <len>\n",
    .options =
        (cmdp_option_st[]){
            _opt_outfile(rand_args.outfile, ),
            _opt_outformat(rand_args.outformat, ),
            {0},
        },
    .fn_process = __process,
};


static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 1, 1);

    RESULT ret     = RET_FAIL;
    XIO *outstream = NULL;
    long len;
    if (string_to_long(CMDP_GET_ARG(params, 0), &len) != RET_OK)
    {
        goto end;
    }
    if (len <= 0)
    {
        LOG_ERR(0, "invalid length");
        goto end;
    }

    outstream = cmd_get_outstream(rand_args.outfile, true);

    FORMAT_t fmt_out = XIO_isatty(outstream) ? FORMAT_HEX : FORMAT_BIN;
    fmt_out          = cmd_get_format(rand_args.outformat, fmt_out);
    outstream        = cmd_wrap_stream(outstream, fmt_out);

    LOG_DBG("len=%ld", len);
    ret = cc_random_bytes(outstream, len);

end:
    XIO_CLOSE_SAFE(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
