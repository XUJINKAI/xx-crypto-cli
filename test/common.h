#pragma once

#include "global.h"
#include "utest.h"
#include <stdio.h>

#define EXAMPLES_DIR "../test/examples/"

int xx_run(int argc, char *argv[], XIO *in_stream, XIO *out_stream, FILE *log_stream);

#define DIM(STRING)    "\x1b[2m" STRING "\x1b[0m"
#define countof(array) (sizeof(array) / sizeof(array[0]))

#define __COMBINE__(X, Y) X##Y
#define COMBINE(X, Y)     __COMBINE__(X, Y)

#define LOGSTREAM    stderr
#define LOGSC(color) (isatty(fileno(LOGSTREAM)) ? color : "")
extern XIO_EVENT_CALLBACK __outstream_callback;

#define CMD_START()                                                                                                    \
    XIO *_instream  = XIO_new_memory("", 0);                                                                           \
    XIO *_outstream = XIO_new_mbuilder();                                                                              \
    XIO_set_event_callback(_outstream, __outstream_callback, NULL);                                                    \
    __unused uint8_t *cmd_out   = NULL;                                                                                \
    __unused size_t cmd_out_len = 0;                                                                                   \
    __unused int cmd_result     = 0;

#define CMD_SET_INSTREAM(inbuf, inlen)                                                                                 \
    XIO_close(_instream);                                                                                              \
    _instream = XIO_new_memory(inbuf, inlen);

#define CMD_RUN(...)                                                                                                   \
    char *COMBINE(_arg_, __LINE__)[] = {__VA_ARGS__};                                                                  \
    fprintf(LOGSTREAM, "%s%s%s\n", LOGSC(TC_YELLOW), #__VA_ARGS__, LOGSC(TC_RESET));                                   \
    FILE *_out = log_is_level_enabled(_LOGT_DEBUG) ? stderr : NULL;                                                    \
    cmd_result = xx_run(countof(COMBINE(_arg_, __LINE__)), COMBINE(_arg_, __LINE__), _instream, _outstream, _out);     \
    XIO_flush(_outstream);                                                                                             \
    cmd_out     = XIO_MBUILDER_get_buffer(_outstream);                                                                 \
    cmd_out_len = XIO_MBUILDER_get_size(_outstream);

#define CMD_END()                                                                                                      \
    XIO_close(_instream);                                                                                              \
    XIO_close(_outstream);
