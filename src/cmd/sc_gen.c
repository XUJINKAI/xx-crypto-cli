#include "cmd_helper.h"
#include "cmdparser.h"
#include "cross-platform/unistd.h"
#include "global.h"


static cmdp_action_t __process(cmdp_process_param_st *params);

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
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    XIO *outstream = cmd_get_outstream(gen_args.outfile, true);
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
    return RESULT_TO_CMDP_ACTION(RET_OK);
}
