#include "xio.h"
#include <stdlib.h>
#include <string.h>


typedef struct XIO_FILE_st
{
    XIO base;
    FILE *fp;
} XIO_FILE;


static size_t _xio_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    return fread(__ptr, 1, __maxlen, io->fp);
}
static size_t _xio_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    return fwrite(__ptr, 1, __len, io->fp);
}
static void _xio_flush(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    fflush(io->fp);
}
static void _xio_close(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    if (io->base.release_souce)
    {
        fclose(io->fp);
    }
    free(io);
}
static void _xio_dump(XIO *__io, XIO *out)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    XIO_printf(out, "XIO_FILE: fileno=%d", fileno(io->fp));
}

const XIO_METHOD XIO_METHOD_FILE = {
    _xio_read, _xio_write, _xio_flush, _xio_close, _xio_dump,
};

XIO *XIO_new_from_FILE(FILE *fp, bool close_free)
{
    XIO_FILE *io           = (XIO_FILE *)malloc(sizeof(XIO_FILE));
    io->base.can_read      = true;
    io->base.can_write     = true;
    io->base.release_souce = close_free;
    io->base.methods       = (XIO_METHOD *)&XIO_METHOD_FILE;
    io->fp                 = fp;
    return (XIO *)io;
}
XIO *XIO_new_from_filename(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp == NULL)
    {
        ERROR("fopen(\"%s\", \"%s\") failed", filename, mode);
        return NULL;
    }
    return XIO_new_from_FILE(fp, true);
}
