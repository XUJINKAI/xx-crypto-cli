#include "cc/rand.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/convert.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *size_str;
    char *source;
    char *outfile;
    char *outformat;
    bool noout;
    bool check;
} rand_args;

cmdp_command_st sc_rand = {
    .name = "rand",
    .desc = "Generate random bytes",
    .doc  = "Usage: rand [OPTIONS...] -n <SIZE>\n"
           "\n",
    .options =
        (cmdp_option_st[]){
            {'n', NULL, "Number of bytes", CMDP_TYPE_STRING_PTR, &rand_args.size_str, "<SIZE>"},
            {'S', "source", "Source of random bytes", CMDP_TYPE_STRING_PTR, &rand_args.source, "<SOURCE>"},
            _opt_outfile(rand_args.outfile, ),
            _opt_outformat(rand_args.outformat, ),
            {0, "noout", "Do not output the random bytes", CMDP_TYPE_BOOL, &rand_args.noout, NULL},
            // {0, "check", "Check the random bytes", CMDP_TYPE_BOOL, &rand_args.check, NULL},
            {0},
        },
    .doc_tail = "\n"
                "SIZE: 10, 0xFF, 1k, 1m, 1g, etc.\n"
                "SOURCE: soft(default), rdrand, rdseed, sdf:LIB.so\n",
    .fn_process = __process,
};


static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    RESULT ret     = RET_FAIL;
    XIO *outstream = NULL;
    long len;
    if (string_to_long_ex(rand_args.size_str, &len) != RET_OK)
    {
        goto end;
    }
    if (len < 0)
    {
        LOG_ERR("Invalid length: %ld", len);
        goto end;
    }
    LOG_VERBOSE("rand len=%zu", len);

    outstream = cmd_get_outstream(rand_args.outfile, true);

    FORMAT_t fmt_out = XIO_isatty(outstream) ? FORMAT_HEX : FORMAT_BIN;
    fmt_out          = cmd_get_format(rand_args.outformat, fmt_out);
    outstream        = cmd_wrap_stream(outstream, fmt_out);

    if (rand_args.noout)
    {
        XIO_CLOSE_SAFE(outstream);
        outstream = XIO_new_null();
    }

    ret = cc_random_bytes(rand_args.source, outstream, len);

end:
    XIO_CLOSE_SAFE(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
