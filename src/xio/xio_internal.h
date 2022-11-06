#pragma once

#include "global.h"
#include <stdio.h>

typedef struct XIO_st XIO;

struct XIO_st
{
    size_t (*read)(XIO *io, uint8_t *__ptr, size_t __maxlen);
    size_t (*write)(XIO *io, const uint8_t *__ptr, size_t __len);
    void (*flush)(XIO *io);
    int (*seek)(XIO *io, int64_t offset, int whence);
    int64_t (*tell)(XIO *io);
    void (*close)(XIO *io);
    void (*dump)(XIO *io, FILE *fp);

    int type;
    int flags;   // extra flags
    XIO *target; // for filter
    uint64_t num_read;
    uint64_t num_write;
};
