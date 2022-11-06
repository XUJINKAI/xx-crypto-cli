#pragma once

#include "cmdparser.h"
#include "global.h"
#include "xio/xio.h"

#define _opt_intext(opt, more)                                                                                         \
    {                                                                                                                  \
        'T', "text", "Input Text", CMDP_TYPE_STRING_PTR, &opt, "<TEXT>", more                                          \
    }
#define _opt_infile(opt, more)                                                                                         \
    {                                                                                                                  \
        'I', "in", "Input File", CMDP_TYPE_STRING_PTR, &opt, "<FILE>", more                                            \
    }
#define _opt_outfile(opt, more)                                                                                        \
    {                                                                                                                  \
        'O', "out", "Output File", CMDP_TYPE_STRING_PTR, &opt, "<FILE>", more                                          \
    }
#define _opt_informat(opt, more)                                                                                       \
    {                                                                                                                  \
        0, "inform", "Input Format", CMDP_TYPE_STRING_PTR, &opt, "<FORMAT>", more                                      \
    }
#define _opt_outformat(opt, more)                                                                                      \
    {                                                                                                                  \
        0, "outform", "Output Format", CMDP_TYPE_STRING_PTR, &opt, "<FORMAT>", more                                    \
    }

#define CMDP_CHECK_EMPTY_HELP(params)                                                                                  \
    if (params->argc == 0 && params->opts == 0)                                                                        \
    {                                                                                                                  \
        return CMDP_ACT_SHOW_HELP;                                                                                     \
    }

#define CMDP_CHECK_ARG_COUNT(params, min, max)                                                                         \
    if (params->argc < min || params->argc > max)                                                                      \
    {                                                                                                                  \
        LOG_DBG("params->argc: %d", params->argc);                                                                     \
        if (max == 0)                                                                                                  \
        {                                                                                                              \
            LOG_ERR("NO need argument.");                                                                              \
        }                                                                                                              \
        else if (params->argc > max)                                                                                   \
        {                                                                                                              \
            LOG_ERR("Too many argument.");                                                                             \
        }                                                                                                              \
        else if (params->argc < min)                                                                                   \
        {                                                                                                              \
            LOG_ERR("Need argument.");                                                                                 \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            LOG_ERR("Invalid argument count.");                                                                        \
        }                                                                                                              \
        return CMDP_ACT_ERROR;                                                                                         \
    }

#define CMDP_CHECK_OPT_CONFICT(condition, opt1, opt2)                                                                  \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        LOG_ERR("Can't use %s and %s together.", opt1, opt2);                                                          \
        return CMDP_ACT_ERROR;                                                                                         \
    }

#define CMDP_CHECK_OPT_TOGETHER(condition, opt1, opt2)                                                                 \
    if (condition)                                                                                                     \
    {                                                                                                                  \
        LOG_ERR("Must use %s and %s together.", opt1, opt2);                                                           \
        return CMDP_ACT_ERROR;                                                                                         \
    }

#define CMDP_CHECK_OPT_NEITHER(condition, opt1, opt2)                                                                  \
    if (!(condition))                                                                                                  \
    {                                                                                                                  \
        LOG_ERR("Must use one of %s or %s.", opt1, opt2);                                                              \
        return CMDP_ACT_ERROR;                                                                                         \
    }

#define CMDP_GET_ARG(params, index) (params->argc > index ? params->argv[index] : NULL)

#define RESULT_TO_CMDP_ACTION(result) (result == RET_OK ? CMDP_ACT_OK : CMDP_ACT_ERROR)

typedef enum
{
    FORMAT_NONE,
    FORMAT_BIN,
    FORMAT_HEX,
    FORMAT_BASE64,
} FORMAT_t;

FORMAT_t cmd_get_format(const char *format, FORMAT_t def_fmt);
XIO *cmd_wrap_stream(XIO *xio, FORMAT_t format);
XIO *cmd_get_instream(char *text, char *filename, bool __stdin);
XIO *cmd_get_outstream(char *filename, bool __stdout);
