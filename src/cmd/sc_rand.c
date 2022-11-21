#include "cc/rand.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/converter.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *size_str;
    char *engine;
    char *outfile;
    bool out_fmt_hex;
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
            {'E', "engine", "Source of generator", CMDP_TYPE_STRING_PTR, &rand_args.engine, "<ENGINE>"},
            {0, "hex", "Output hex format", CMDP_TYPE_BOOL, &rand_args.out_fmt_hex},
            _opt_outfile(rand_args.outfile, ),
            {0, "noout", "Do not output the random bytes", CMDP_TYPE_BOOL, &rand_args.noout},
            // {0, "check", "Check the random bytes", CMDP_TYPE_BOOL, &rand_args.check, NULL},
            {0},
        },
    .doc_tail = "\n"
                "SIZE: 10, 0xFF, 1k, 1m, 1g, etc.\n"
                "ENGINE: soft(default), rdrand, rdseed, sdf:LIB.so\n",
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    RESULT ret     = RET_FAIL;
    XIO *outstream = NULL;

    long len;
    if (RET_OK != string_to_long(rand_args.size_str, &len, CONV_SIZE))
    {
        LOG_ERROR("Invalid length: %ld", len);
        goto end;
    }
    LOG_VERBOSE("rand len=%ld", len);

    FORMAT_t fmt_out = rand_args.out_fmt_hex ? FORMAT_HEX : FORMAT_BIN;
    LOG_VERBOSE("rand fmt=%s", format_to_string(fmt_out));

    outstream = cmd_get_outstream(rand_args.outfile, true);
    outstream = cmd_wrap_stream(outstream, fmt_out);

    if (rand_args.noout)
    {
        xio_close_safe(outstream);
        outstream = XIO_new_null();
    }

    ret = cc_random_bytes(rand_args.engine, outstream, len);

end:
    xio_close_safe(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
