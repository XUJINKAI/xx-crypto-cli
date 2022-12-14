#include "cmdp.h"

extern cmdp_command_st main_cmdp;

static void (*default_error_parse)(cmdp_error_params_st *params) = NULL;
static void ctx_error_parse(cmdp_error_params_st *params)
{
    tty_put_color(params->err_stream, _LOGC_ERROR);
    fprintf(params->err_stream, "xx: ");
    default_error_parse(params);
    tty_put_color(params->err_stream, TC_RESET);
}

int run_command(int argc, char *argv[])
{
    // TODO 初始化一次ctx
    cmdp_ctx ctx = {0};
    cmdp_set_default_context(&ctx);
    ctx.out_stream      = log_get_stream();
    ctx.err_stream      = log_get_stream();
    default_error_parse = ctx.fn_error_parse;
    ctx.fn_error_parse  = ctx_error_parse;
    // TODO 对ctx添加解析后果的打印

    int64_t t = time_ns();
    int r     = cmdp_run(argc, argv, &main_cmdp, &ctx);
    t         = time_ns() - t;
    if (!g_state.no_eol && XIO_get_num_write(g_state.out, true) != 0 && XIO_get_last_char(g_state.out, true) != '\n')
    {
        XIO_printf(g_state.out, "\n");
        XIO_flush(g_state.out);
    }

    LOG_DBG("time: %ld.%ld ms", t / 1000000, t % 1000000);
    LOG_DBG("exit code: %d", r);
    return r;
}
