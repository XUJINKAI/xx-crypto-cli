#include "xio_internal.h"
#include <global.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen)
{
    if (io == NULL || io->read == NULL)
    {
        return 0;
    }
    return io->read(io, __ptr, __maxlen);
}
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len)
{
    if (io == NULL || io->write == NULL)
    {
        return 0;
    }
    return io->write(io, __ptr, __len);
}
size_t XIO_printf(XIO *io, const char *format, ...)
{
    va_list ap1;
    va_start(ap1, format);
    size_t len = vsnprintf(NULL, 0, format, ap1);
    va_end(ap1);

    va_list ap2;
    va_start(ap2, format);
    char *buf = (char *)malloc(len + 1);
    vsnprintf(buf, len + 1, format, ap2);
    va_end(ap2);

    size_t ret = XIO_write(io, (uint8_t *)buf, len);
    free(buf);
    return ret;
}
void XIO_flush(XIO *io)
{
    if (io == NULL || io->flush == NULL)
    {
        return;
    }
    io->flush(io);
}
void XIO_close(XIO *io)
{
    if (io == NULL || io->close == NULL)
    {
        return;
    }
    XIO_flush(io);
    io->close(io);
}
static void f_dump_xio(XIO *io, FILE *fp)
{
    if (io->dump)
    {
        io->dump(io, fp);
    }
    else
    {
        fprintf(fp, "TYPE:%d", io->type);
    }
}
void XIO_dump_chain(XIO *io, FILE *fp)
{
    if (io == NULL)
    {
        fprintf(fp, "NULL");
        return;
    }
    while (io->target != NULL)
    {
        f_dump_xio(io, fp);
        fprintf(fp, " === ");
        io = io->target;
    }
    f_dump_xio(io, fp);
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
XIO *XIO_get_raw(XIO *io)
{
    if (io->target == NULL)
    {
        return io;
    }
    return XIO_get_raw(io->target);
}
