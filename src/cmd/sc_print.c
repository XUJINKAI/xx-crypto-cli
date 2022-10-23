#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "global.h"

static void fn_before(cmdp_before_param_st *params);
static cmdp_action_t fn_process(cmdp_process_param_st *params);

static struct
{
    char *informat;
    char *infile;
    char *outformat;
    char *outfile;
} args;

cmdp_command_st sc_print = {
    .name = "print",
    .desc = "Transform and print data",
    .doc  = "print [OPTIONS] <TEXT>\n",
    .options =
        (cmdp_option_st[]){
            {'I', "in-format", "Input Format", CMDP_TYPE_STRING_PTR, &args.informat, "<FORMAT>"},
            {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &args.outformat, "<FORMAT>"},
            {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &args.infile, "<filepath>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &args.outfile, "<filepath>"},
            {0},
        },
    .fn_before  = fn_before,
    .fn_process = fn_process,
    .alias_name = "p",
};


static void fn_before(cmdp_before_param_st *params)
{
    memset(&args, 0, sizeof(args));
}
static cmdp_action_t fn_process(cmdp_process_param_st *params)
{
    LOG_VERBOSE("isatty(stdin) : %d", isatty(0));
    LOG_VERBOSE("isatty(stdout): %d", isatty(1));
    XIO_CMD_IN_PARAM in_param = {
        .filename     = args.infile,
        .file_deffmt  = "bin",
        .argument     = params->argc > 0 ? params->argv[0] : NULL,
        .arg_deffmt   = "bin",
        .stdin_deffmt = "bin",
        .format       = args.informat,
    };
    XIO *instream = XIO_new_cmd_instream(&in_param);

    XIO_CMD_OUT_PARAM out_param = {
        .filename      = args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = "bin",
        .format        = args.outformat,
    };
    XIO *outstream = XIO_new_cmd_outstream(&out_param);
    XIO_copy(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
}
