#pragma once

#include "global.h"
#include <stdio.h>

typedef struct XIO_st XIO;
typedef struct XIO_methods_st XIO_METHOD;

struct XIO_methods_st
{
    size_t (*read)(XIO *io, uint8_t *__ptr, size_t __maxlen);
    size_t (*write)(XIO *io, const uint8_t *__ptr, size_t __len);
    void (*flush)(XIO *io);
    void (*close)(XIO *io);
    void (*dump)(XIO *io, XIO *out);
};

struct XIO_st
{
    XIO_METHOD *methods;
    bool can_read;
    bool can_write;
    bool can_seek;
    bool release_resource; // for inner resource
    bool close_filter_target;
    XIO *target; // filter
};
