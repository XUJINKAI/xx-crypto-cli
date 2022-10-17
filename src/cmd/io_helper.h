#pragma once

#include "cmdparser.h"
#include "xio/xio.h"

typedef struct XIO_CMD_IN_PARAM_st
{
    const char *filename;
    const char *file_deffmt;
    const char *argument;
    const char *arg_deffmt;
    const char *stdin_deffmt;
    const char *format;
} XIO_CMD_IN_PARAM;

typedef struct XIO_CMD_OUT_PARAM_st
{
    const char *filename;
    const char *file_deffmt;
    const char *stdout_deffmt;
    const char *format;
} XIO_CMD_OUT_PARAM;

XIO *XIO_new_cmd_instream(XIO_CMD_IN_PARAM *param);
XIO *XIO_new_cmd_outstream(XIO_CMD_OUT_PARAM *param);
