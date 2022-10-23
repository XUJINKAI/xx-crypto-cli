#include "cc/xcrypto.h"
#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "global.h"

static void rand_before(cmdp_before_param_st *params);
static cmdp_action_t rand_process(cmdp_process_param_st *params);

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
            {'o', "out", "write format", CMDP_TYPE_STRING_PTR, &rand_args.outformat, "<FORMAT>"},
            {'w', "write", "output file", CMDP_TYPE_STRING_PTR, &rand_args.outfile, "<file>"},
            {0},
        },
    .fn_before  = rand_before,
    .fn_process = rand_process,
};


static void rand_before(cmdp_before_param_st *params)
{
    memset(&rand_args, 0, sizeof(rand_args));
}
static cmdp_action_t rand_process(cmdp_process_param_st *params)
{
    cmdp_action_t ret = CMDP_ACT_FAIL;
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG_C(0, "too many arguments");
        return CMDP_ACT_FAIL;
    }
    char *endptr = NULL;
    long len     = strtol(params->argv[0], &endptr, 10);
    if (endptr == params->argv[0] || *endptr != '\0' || len <= 0)
    {
        LOG_C(0, "invalid length");
        return CMDP_ACT_FAIL;
    }
    XIO *out = NULL;
    if (rand_args.outfile != NULL)
    {
        out = XIO_new_file(rand_args.outfile, "wb");
    }
    else
    {
        out = XIO_new_fp(stdout, false);
    }
    if (rand_args.outformat == NULL && rand_args.outfile == NULL || rand_args.outformat == "hex")
    {
        out = XIO_new_filter_hex(out);
    }
    x_random_bytes(out, len);
    ret = CMDP_ACT_OVER;

end:
    XIO_close(out);
    return ret;
}
