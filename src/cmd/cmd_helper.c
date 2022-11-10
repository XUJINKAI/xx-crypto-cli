#include "cmd_helper.h"
#include "utils/string.h"

#define XX(name) #name,
static const char *format_strings[] = {FORMAT_TYPE_MAP(XX)};
#undef XX

const char *format_to_string(FORMAT_t format)
{
    if (format <= 0 || format > sizeof(format_strings) / sizeof(format_strings[0]))
    {
        return "unknown";
    }
    return format_strings[format - 1];
}

FORMAT_t cmd_get_format(const char *format, FORMAT_t def_fmt)
{
    if (format == NULL)
    {
        return def_fmt;
    }
    else if (STREQ_NoCase(format, "bin"))
    {
        return FORMAT_BIN;
    }
    else if (STREQ_NoCase(format, "hex"))
    {
        return FORMAT_HEX;
    }
    else if (STREQ_NoCase(format, "base64"))
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
            LOG_ERROR("Can't open file: %s", filename);
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
            LOG_ERROR("Can't open file: %s", filename);
        }
    }
    else if (__stdout)
    {
        r = XIO_new_fp(stdout, false);
    }
    return r;
}
