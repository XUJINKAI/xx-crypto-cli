#include "cmd_helper.h"
#include "cross-platform/string.h"

FORMAT_t cmd_get_format(const char *format, FORMAT_t def_fmt)
{
    if (format == NULL)
    {
        return def_fmt;
    }
    else if (strcasecmp(format, "bin") == 0)
    {
        return FORMAT_BIN;
    }
    else if (strcasecmp(format, "hex") == 0)
    {
        return FORMAT_HEX;
    }
    else if (strcasecmp(format, "base64") == 0)
    {
        return FORMAT_BASE64;
    }
    return def_fmt;
}

XIO *cmd_wrap_stream(XIO *xio, FORMAT_t format)
{
    switch (format)
    {
        case FORMAT_HEX:
            return XIO_newf_hex(xio);
        case FORMAT_BIN:
            return xio;
        default:
            LOG_DBG("Unknown format: %d", format);
            return xio;
    }
}

XIO *cmd_get_instream(char *text, char *filename, bool __stdin)
{
    XIO *r = NULL;
    if (text)
    {
        r = XIO_new_string(text, false);
    }
    else if (filename)
    {
        r = XIO_new_file(filename, "rb");
        if (r == NULL)
        {
            LOG_ERR("Can't open file: %s", filename);
        }
    }
    else if (__stdin)
    {
        r = XIO_new_fp(stdin, false);
    }
    return r;
}

XIO *cmd_get_outstream(char *filename, bool __stdout)
{
    XIO *r = NULL;
    if (filename)
    {
        r = XIO_new_file(filename, "wb");
        if (r == NULL)
        {
            LOG_ERR("Can't open file: %s", filename);
        }
    }
    else if (__stdout)
    {
        r = XIO_new_fp(stdout, false);
    }
    return r;
}
