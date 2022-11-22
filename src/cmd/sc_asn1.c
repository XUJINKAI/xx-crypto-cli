#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *infile;
} args;

cmdp_command_st sc_asn1 = {
    .name = "asn1",
    .desc = "asn1 parser",
    .doc  = "Usage: asn1 [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            _opt_infile(args.infile, ),
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    XIO *instream  = NULL;
    XIO *outstream = NULL;
    instream       = cmd_get_instream(NULL, args.infile, true);
    outstream      = cmd_get_outstream(NULL, true);

end:
    xio_close_safe(instream);
    xio_close_safe(outstream);
    return CMDP_ACT_OK;
}
