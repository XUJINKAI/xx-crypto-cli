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

FORMAT_t format_from_string(const char *format)
{
    if (format == NULL)
    {
        return FORMAT_NONE;
    }
    for (int i = 0; i < (int)(sizeof(format_strings) / sizeof(format_strings[0])); i++)
    {
        if (STREQ_NoCase(format, format_strings[i]))
        {
            return i + 1;
        }
    }
    return FORMAT_NONE;
}

FORMAT_t cmd_get_format(const char *format, FORMAT_t def_fmt)
{
    if (format == NULL)
    {
        return def_fmt;
    }
    FORMAT_t fmt = format_from_string(format);
    if (fmt == FORMAT_NONE)
    {
        LOG_ERROR("Invalid format: %s", format);
        return def_fmt;
    }
    return fmt;
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

XIO *cmd_get_instream(char *text, char *filename, bool g_in)
{
    XIO *r = NULL;
    if (text)
    {
        r = XIO_new_string(text);
    }
    else if (filename)
    {
        r = XIO_new_file(filename, "rb");
        if (r == NULL)
        {
            LOG_ERROR("Can't open file: %s", filename);
        }
    }
    else if (g_in)
    {
        r = XIO_newf_pass(g_state.in, 0);
    }
    return r;
}

XIO *cmd_get_outstream(char *filename, bool g_out)
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
    else if (g_out)
    {
        r = XIO_newf_pass(g_state.out, 0);
    }
    return r;
}
