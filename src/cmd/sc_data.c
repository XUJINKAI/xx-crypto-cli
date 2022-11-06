#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *informat;
    char *infile;
    char *outformat;
    char *outfile;
} args;

cmdp_command_st sc_data = {
    .name = "data",
    .desc = "Data process",
    .doc  = "print [OPTIONS] <TEXT>\n",
    .options =
        (cmdp_option_st[]){
            {'I', "in-format", "Input Format", CMDP_TYPE_STRING_PTR, &args.informat, "<FORMAT>"},
            {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &args.outformat, "<FORMAT>"},
            {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &args.infile, "<filepath>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &args.outfile, "<filepath>"},
            {0},
        },
    .fn_process = __process,
};


static cmdp_action_t __process(cmdp_process_param_st *params)
{
    LOG_VERBOSE("isatty(stdin) : %d", isatty(0));
    LOG_VERBOSE("isatty(stdout): %d", isatty(1));

    XIO *instream  = cmd_get_instream(CMDP_GET_ARG(params, 0), args.infile, true);
    XIO *outstream = cmd_get_outstream(args.outfile, true);
    XIO_drain(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
    return RESULT_TO_CMDP_ACTION(RET_OK);
}
