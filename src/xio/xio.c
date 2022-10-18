#include "xio.h"
#include <global.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

size_t XIO_read(XIO *io, uint8_t *__ptr, size_t __maxlen)
{
    if (io->can_read)
    {
        return io->methods->read(io, __ptr, __maxlen);
    }
    return 0;
}
size_t XIO_write(XIO *io, const uint8_t *__ptr, size_t __len)
{
    if (io->can_write)
    {
        return io->methods->write(io, __ptr, __len);
    }
    return 0;
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
    io->methods->flush(io);
}
void XIO_close(XIO *io)
{
    XIO_flush(io);
    io->methods->close(io);
}
void XIO_dump(XIO *io, XIO *out)
{
    io->methods->dump(io, out);
}
void XIO_copy(XIO *in, XIO *out)
{
    uint8_t buf[1024];
    size_t len;
    // TODO 读一个字符使下列命令生效 ./xx gen -l | ./xx echo
    while ((len = XIO_read(in, buf, 1)) > 0)
    {
        XIO_write(out, buf, len);
    }
}
