#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "cross-platform/unistd.h"
#include "global.h"

static void gen_before(cmdp_before_param_st *params);
static cmdp_action_t gen_process(cmdp_process_param_st *params);

static struct
{
    char *fix_str;
    bool count_line;
    int interval_ms;
    int total_repeat;
    char *outformat;
    char *outfile;
} gen_args;

cmdp_command_st sc_gen = {
    .name = "gen",
    .desc = "Generate data",
    .options =
        (cmdp_option_st[]){
            {'s', "string", "Fixed String", CMDP_TYPE_STRING_PTR, &gen_args.fix_str, "<STRING>"},
            {'l', "count-line", "Count Line", CMDP_TYPE_BOOL, &gen_args.count_line, NULL},
            {'S', "sleep", "Sleep interval milliseconds", CMDP_TYPE_INT4, &gen_args.interval_ms, "<MILLISECONDS>"},
            {'r', "repeat", "Repeat", CMDP_TYPE_INT4, &gen_args.total_repeat, "<COUNT>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &gen_args.outfile, "<filepath>"},
            {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &gen_args.outformat, "<FORMAT>"},
            {0},
        },
    .fn_before  = gen_before,
    .fn_process = gen_process,
};

static void gen_before(cmdp_before_param_st *params)
{
    memset(&gen_args, 0, sizeof(gen_args));
    gen_args.fix_str     = "y";
    gen_args.interval_ms = 100;
    gen_args.count_line  = false;
}
static cmdp_action_t gen_process(cmdp_process_param_st *params)
{
    XIO_CMD_OUT_PARAM out_param = {
        .filename      = gen_args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = "bin",
        .format        = gen_args.outformat,
    };
    XIO *outstream = XIO_new_cmd_outstream(&out_param);
    for (int i = 0; i < gen_args.total_repeat || gen_args.total_repeat == 0; i++)
    {
        if (gen_args.count_line)
        {
            XIO_printf(outstream, "%d:%s\n", i, gen_args.fix_str);
        }
        else
        {
            XIO_printf(outstream, "%s\n", gen_args.fix_str);
        }
        XIO_flush(outstream);
        sleep_ms(gen_args.interval_ms);
    }
    XIO_close(outstream);
}
