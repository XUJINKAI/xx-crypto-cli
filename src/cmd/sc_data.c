#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *instring;
    char *infile;
    char *informat;
    char *outfile;
    char *outformat;
} args;

cmdp_command_st sc_data = {
    .name = "data",
    .desc = "Data encoder",
    .doc  = "Usage: data [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            _opt_intext(args.instring, ),
            _opt_infile(args.infile, ),
            _opt_informat(args.informat, ),
            _opt_outfile(args.outfile, ),
            _opt_outformat(args.outformat, ),
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    XIO *instream       = cmd_get_instream(args.instring, args.infile, true);
    XIO *outstream      = cmd_get_outstream(args.outfile, true);
    FORMAT_t in_format  = cmd_get_format(args.informat, FORMAT_CSTR);
    FORMAT_t out_format = cmd_get_format(args.outformat, FORMAT_BIN);
    instream            = cmd_wrap_stream(instream, in_format);
    outstream           = cmd_wrap_stream(outstream, out_format);
    LOG_VERBOSE("in_format=%s", format_to_string(in_format));
    LOG_VERBOSE("out_format=%s", format_to_string(out_format));

    XIO_drain(instream, outstream);

    XIO_CLOSE_SAFE(instream);
    XIO_CLOSE_SAFE(outstream);
    return RESULT_TO_CMDP_ACTION(RET_OK);
}
