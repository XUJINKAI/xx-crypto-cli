#include "xio_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct XIO_FILE_st
{
    XIO base;
    FILE *fp;
} XIO_FILE;

static size_t _read_(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    size_t len   = fread(__ptr, 1, __maxlen, io->fp);
    io->base.num_read += len;
    return len;
}
static size_t _write_(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    size_t len   = fwrite(__ptr, 1, __len, io->fp);
    io->base.num_write += len;
    return len;
}
static int _seek_(XIO *__io, int64_t offset, int whence)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    return fseek(io->fp, offset, whence);
}
static int64_t _tell_(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    return ftell(io->fp);
}
static void _flush_(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_FILE_TTY_FLUSH_EOL) && isatty(fileno(io->fp)))
    {
        const char *eol = "\n";
        _write_(__io, (const uint8_t *)eol, strlen(eol));
    }
    fflush(io->fp);
}
static void _close_(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        fclose(io->fp);
    }
    free(io);
}
static void _dump(XIO *__io, FILE *fp)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    fprintf(fp, "XIO_FILE(fileno=%d)", fileno(io->fp));
}

XIO *XIO_new_fp(FILE *fp, bool close_free)
{
    XIO_FILE *io = (XIO_FILE *)malloc(sizeof(XIO_FILE));
    *io          = (XIO_FILE){
        .base =
            {
                .read  = _read_,
                .write = _write_,
                .flush = _flush_,
                .seek  = _seek_,
                .tell  = _tell_,
                .close = _close_,
                .dump  = _dump,
                .type  = XIO_TYPE_FILE,
                .flags = (close_free ? XIO_FLAG_CLOSE : 0) | XIO_FLAG_FILE_TTY_FLUSH_EOL,
            },
        .fp = fp,
    };
    io->fp = fp;
    return (XIO *)io;
}
XIO *XIO_new_file(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp == NULL)
    {
        LOG_VERBOSE("fopen(\"%s\", \"%s\") failed", filename, mode);
        return NULL;
    }
    return XIO_new_fp(fp, true);
}
FILE *XIO_FILE_get_fp(XIO *io)
{
    if (io->type != XIO_TYPE_FILE)
    {
        return NULL;
    }
    return ((XIO_FILE *)io)->fp;
}