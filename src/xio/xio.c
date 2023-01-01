#include "cc/format/base64.h"
#include "xio_internal.h"
#include <global.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

long _XIO_ctrl(XIO *io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    if (io && io->ctrl)
    {
        if (ctrl == _XIO_CTRL_FLUSH)
        {
            if (!HAS_FLAG(io->ic_flag, XIO_ICF_SHOULD_FLUSH))
            {
                return 0;
            }
            long r = io->ctrl(io, ctrl, larg, parg);
            io->ic_flag &= ~XIO_ICF_SHOULD_FLUSH;
            if (io->event_callback)
            {
                io->event_callback(io, XIO_EVENT_FLUSH, io->event_callback_arg, NULL, 0);
            }
            return r;
        }
        else
        {
            return io->ctrl(io, ctrl, larg, parg);
        }
    }
    return 0;
}
int XIO_getc(XIO *io)
{
    uint8_t c;
    if (XIO_read(io, &c, 1) == 1)
    {
        return c;
    }
    return EOF;
}
int XIO_putc(XIO *io, int c)
{
    uint8_t b = c;
    if (XIO_write(io, &b, 1) == 1)
    {
        return c;
    }
    return EOF;
}
xbytes *XIO_read_all_buf(uint8_t *buf, size_t bufsize, XIO *io)
{
    xbytes *b = xbytes_new(bufsize);
    size_t r;
    while ((r = XIO_read(io, buf, bufsize)) > 0)
    {
        xbytes_write(b, buf, r);
    }
    return b;
}
xbytes *XIO_read_all(XIO *io, size_t bufsize)
{
    if (bufsize > STACK_BUFFER_SIZE)
    {
        uint8_t *buf = malloc(bufsize);
        xbytes *b    = XIO_read_all_buf(buf, bufsize, io);
        free(buf);
        return b;
    }
    else
    {
        bufsize = bufsize == 0 ? STACK_BUFFER_SIZE : bufsize;
        uint8_t buf[STACK_BUFFER_SIZE];
        return XIO_read_all_buf(buf, bufsize, io);
    }
}
size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen)
{
    if (io == NULL || io->read == NULL)
    {
        return 0;
    }
    size_t r = io->read(io, __ptr, __maxlen);
    if (io->event_callback)
    {
        io->event_callback(io, XIO_EVENT_READ, io->event_callback_arg, __ptr, r);
    }
    return r;
}
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len)
{
    if (io == NULL || io->write == NULL)
    {
        return 0;
    }
    size_t r = io->write(io, __ptr, __len);
    io->ic_flag |= XIO_ICF_SHOULD_FLUSH;
    if (io->event_callback)
    {
        io->event_callback(io, XIO_EVENT_WRITE, io->event_callback_arg, __ptr, r);
    }
    return r;
}
size_t XIO_write_hex(XIO *io, const uint8_t *__ptr, size_t __len)
{
    const char *fmt = G_HEX_FMT;
    char buf[3];
    for (size_t i = 0; i < __len; i++)
    {
        sprintf(buf, fmt, __ptr[i]);
        XIO_write(io, (uint8_t *)buf, 2);
    }
    return __len;
}
size_t XIO_write_pem(XIO *io, const char *name, const uint8_t *__ptr, size_t __len)
{
    if (!__len)
    {
        return 0;
    }

    char *b64 = cc_base64_encode(__ptr, __len, 0);
    if (!b64)
    {
        return 0;
    }

    size_t r = 0;
    r += XIO_printf(io, "-----BEGIN %s-----\n", name);
    r += XIO_printf(io, "%s", b64);
    r += XIO_printf(io, "-----END %s-----\n", name);
    free(b64);
    return r;
}
size_t XIO_write_base64(XIO *io, const uint8_t *__ptr, size_t __len)
{
    if (!__len)
    {
        return 0;
    }

    char *b64 = cc_base64_encode(__ptr, __len, 0);
    if (!b64)
    {
        return 0;
    }

    size_t r = 0;
    r += XIO_printf(io, "%s", b64);
    free(b64);
    return r;
}
size_t XIO_printf(XIO *io, const char *format, ...)
{
    va_list ap1;
    va_start(ap1, format);
    size_t len = XIO_vprintf(io, format, ap1);
    va_end(ap1);
    return len;
}
size_t XIO_vprintf(XIO *io, const char *format, va_list args)
{
    va_list ap1;
    va_copy(ap1, args);
    size_t len = vsnprintf(NULL, 0, format, ap1);
    va_end(ap1);

    va_list ap2;
    va_copy(ap2, args);
    char *buf = (char *)malloc(len + 1);
    vsnprintf(buf, len + 1, format, ap2);
    va_end(ap2);

    size_t ret = XIO_write(io, (uint8_t *)buf, len);
    free(buf);
    return ret;
}
void XIO_put_color(XIO *io, const char *color)
{
    if (!g_state.no_color && XIO_isatty(io))
    {
        XIO_printf(io, "%s", color);
    }
}
void XIO_close(XIO *io)
{
    if (io == NULL || io->close == NULL)
    {
        return;
    }
    if (io->event_callback)
    {
        io->event_callback(io, XIO_EVENT_BEFORE_CLOSE, io->event_callback_arg, NULL, 0);
    }
    XIO_flush(io);
    io->close(io);
}
static void f_to_string_xio(XIO *io, XIO *out_stream)
{
    if (io->to_string)
    {
        io->to_string(io, out_stream);
    }
    else
    {
        XIO_printf(out_stream, "TYPE:%d", io->type);
    }
}
void XIO_dump_chain(XIO *io, XIO *out_stream)
{
    if (io == NULL)
    {
        XIO_printf(out_stream, "NULL");
        return;
    }
    while (io->target != NULL)
    {
        f_to_string_xio(io, out_stream);
        XIO_printf(out_stream, " === ");
        io = io->target;
    }
    f_to_string_xio(io, out_stream);
}
XIO *XIO_dup(XIO *io)
{
    XIO *result = NULL;
    if (io)
    {
        _XIO_ctrl(io, _XIO_CTRL_DUP, 0, &result);
    }
    return result;
}

void XIO_drain(XIO *in, XIO *out)
{
    uint8_t buf[1024];
    size_t len;
    // TODO 读一个字符使下列命令生效 ./xx gen -l | ./xx echo
    while ((len = XIO_read(in, buf, 1)) > 0)
    {
        XIO_write(out, buf, len);
    }
}
bool XIO_isatty(XIO *io)
{
    FILE *fp = XIO_FILE_get_fp(io);
    if (fp == NULL)
    {
        return false;
    }
    return isatty(fileno(fp));
}
XIO *XIO_get_source(XIO *io)
{
    if (io->target == NULL)
    {
        return io;
    }
    return XIO_get_source(io->target);
}
size_t XIO_get_num_read(XIO *io, bool source)
{
    if (source)
    {
        io = XIO_get_source(io);
    }
    return io->num_read;
}
size_t XIO_get_num_write(XIO *io, bool source)
{
    if (source)
    {
        io = XIO_get_source(io);
    }
    return io->num_write;
}
char XIO_get_last_char(XIO *io, bool source)
{
    if (source)
    {
        io = XIO_get_source(io);
    }
    return io->last_char;
}

void XIO_set_event_callback(XIO *io, XIO_EVENT_CALLBACK callback, void *arg)
{
    io->event_callback     = callback;
    io->event_callback_arg = arg;
}
