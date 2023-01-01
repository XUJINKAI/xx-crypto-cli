#pragma once

#include "global.h"
#include "xio.h"
#include <stdio.h>

typedef struct XIO_st XIO;

// prevent multi-time flush
#define XIO_ICF_SHOULD_FLUSH 0x01

struct XIO_st
{
    size_t (*read)(XIO *io, uint8_t *__ptr, size_t __maxlen);
    size_t (*write)(XIO *io, const uint8_t *__ptr, size_t __len);
    void (*close)(XIO *io);
    void (*to_string)(XIO *io, XIO *out_stream);
    long (*ctrl)(XIO *io, _XIO_CTRL_ENUM ctrl, long larg, void *parg);

    char *name;
    int type;
    int ic_flag; // internal control flag

    int flags;   // common flags, e.g. XIO_FLAG_CLOSE
    XIO *target; // for filter
    XIO_EVENT_CALLBACK event_callback;
    void *event_callback_arg;

    uint64_t num_read;
    uint64_t num_write;
    char last_char;
};
