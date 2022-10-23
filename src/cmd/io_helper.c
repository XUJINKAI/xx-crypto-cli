#include "io_helper.h"
#include "cross-platform/string.h"

static XIO *XIO_wrap_filter(XIO *xio, const char *__format, const char *def_format)
{
    const char *fmt = __format ? __format : def_format;
    if (fmt == NULL)
    {
        return xio;
    }
    else if (strcasecmp(fmt, "bin") == 0)
    {
        return xio;
    }
    else if (strcasecmp(fmt, "hex") == 0)
    {
        return XIO_new_filter_hex(xio);
    }
    else
    {
        LOG_C(0, "unknown format: %s", fmt);
        return xio;
    }
}

XIO *XIO_new_cmd_instream(struct XIO_CMD_IN_PARAM_st *param)
{
    XIO *r = NULL;
    if (param->filename)
    {
        r = XIO_new_file(param->filename, "rb");
        return XIO_wrap_filter(r, param->format, param->file_deffmt);
    }
    else if (param->argument)
    {
        r = XIO_new_r_mem(param->argument, strlen(param->argument), false);
        return XIO_wrap_filter(r, param->format, param->arg_deffmt);
    }
    else
    {
        r = XIO_new_fp(stdin, false);
        return XIO_wrap_filter(r, param->format, param->stdin_deffmt);
    }
    return r;
}
XIO *XIO_new_cmd_outstream(struct XIO_CMD_OUT_PARAM_st *param)
{
    XIO *r = NULL;
    if (param->filename)
    {
        r = XIO_new_file(param->filename, "wb");
        return XIO_wrap_filter(r, param->format, param->file_deffmt);
    }
    else
    {
        r = XIO_new_fp(stdout, false);
        return XIO_wrap_filter(r, param->format, param->stdout_deffmt);
    }
    return r;
}
