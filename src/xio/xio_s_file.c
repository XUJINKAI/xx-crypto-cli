#include "xio_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct XIO_FILE_st
{
    XIO base;
    FILE *fp;
} XIO_FILE;

static size_t f_read(XIO *__io, uint8_t *__ptr, size_t __maxlen)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    size_t len   = fread(__ptr, 1, __maxlen, io->fp);
    io->base.num_read += len;
    return len;
}
static size_t f_write(XIO *__io, const uint8_t *__ptr, size_t __len)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    size_t len   = fwrite(__ptr, 1, __len, io->fp);
    io->base.num_write += len;
    io->base.last_char = __ptr[__len - 1];
    return len;
}
static void f_close(XIO *__io)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    if (HAS_FLAG(io->base.flags, XIO_FLAG_CLOSE))
    {
        fclose(io->fp);
    }
    free(io);
}
static void f_to_string(XIO *__io, XIO *out_stream)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    XIO_printf(out_stream, "XIO_FILE(fileno=%d)", fileno(io->fp));
}
static long f_ctrl(XIO *__io, _XIO_CTRL_ENUM ctrl, long larg, void *parg)
{
    XIO_FILE *io = (XIO_FILE *)__io;
    switch (ctrl)
    {
        case _XIO_CTRL_DUP:
            *(XIO **)parg = XIO_new_fp(fdopen(dup(fileno(io->fp)), "r+"), XIO_FLAG_CLOSE);
            return 1;
        case _XIO_CTRL_FLUSH:
            fflush(io->fp);
            return 0;
        case _XIO_CTRL_SEEK_SET:
        case _XIO_CTRL_SEEK_CUR:
        case _XIO_CTRL_SEEK_END:
            return fseek(io->fp, larg, (ctrl - _XIO_CTRL_SEEK_SET));
        case _XIO_CTRL_TELL:
            return ftell(io->fp);
        default:
            return 0;
    }
}

XIO *XIO_new_fp(FILE *fp, int flags)
{
    XIO_FILE *io = (XIO_FILE *)malloc(sizeof(XIO_FILE));
    *io          = (XIO_FILE){
        .base =
            {
                .read      = f_read,
                .write     = f_write,
                .close     = f_close,
                .to_string = f_to_string,
                .ctrl      = f_ctrl,
                .name      = "FILE",
                .type      = XIO_TYPE_FILE,
                .flags     = flags,
            },
        .fp = fp,
    };
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
    return XIO_new_fp(fp, XIO_FLAG_CLOSE);
}
FILE *XIO_FILE_get_fp(XIO *io)
{
    if (io->type != XIO_TYPE_FILE)
    {
        return NULL;
    }
    return ((XIO_FILE *)io)->fp;
}
