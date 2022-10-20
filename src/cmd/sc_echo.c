#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "global.h"

static void echo_before(cmdp_before_param_st *params);
static cmdp_action_t echo_process(cmdp_process_param_st *params);

static struct
{
    char *informat;
    char *infile;
    char *outformat;
    char *outfile;
} echo_args;

cmdp_command_st sc_echo = {
    .name = "echo",
    .desc = "echo the input",
    .doc  = "echo [OPTIONS] <TEXT>\n",
    .options =
        (cmdp_option_st[]){
            {'I', "in-format", "Input Format", CMDP_TYPE_STRING_PTR, &echo_args.informat, "<FORMAT>"},
            {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &echo_args.outformat, "<FORMAT>"},
            {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &echo_args.infile, "<filepath>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &echo_args.outfile, "<filepath>"},
            {0},
        },
    .fn_before  = echo_before,
    .fn_process = echo_process,
};


static void echo_before(cmdp_before_param_st *params)
{
    memset(&echo_args, 0, sizeof(echo_args));
}
static cmdp_action_t echo_process(cmdp_process_param_st *params)
{
    LOG1("isatty(stdin) : %d", isatty(0));
    LOG1("isatty(stdout): %d", isatty(1));
    XIO_CMD_IN_PARAM in_param = {
        .filename     = echo_args.infile,
        .file_deffmt  = "bin",
        .argument     = params->argc > 0 ? params->argv[0] : NULL,
        .arg_deffmt   = "bin",
        .stdin_deffmt = "bin",
        .format       = echo_args.informat,
    };
    XIO *instream = XIO_new_cmd_instream(&in_param);

    XIO_CMD_OUT_PARAM out_param = {
        .filename      = echo_args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = "bin",
        .format        = echo_args.outformat,
    };
    XIO *outstream = XIO_new_cmd_outstream(&out_param);
    XIO_copy(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
}
