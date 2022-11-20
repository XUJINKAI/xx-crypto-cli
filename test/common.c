#include "common.h"

static void f_outstream_callback(XIO *io, _XIO_EVENT_ENUM event, void *arg, const void *data, size_t len)
{
    switch (event)
    {
        case XIO_EVENT_WRITE:
            fprintf(LOGSTREAM, "%s%.*s%s", LOGSC(TC_BLUE_BRIGHT), (int)len, (const char *)data, LOGSC(TC_RESET));
            break;
        case XIO_EVENT_FLUSH:
        case XIO_EVENT_READ:
        case XIO_EVENT_BEFORE_CLOSE:
        default:
            break;
    }
}

XIO_EVENT_CALLBACK __outstream_callback = f_outstream_callback;
