#include "log.h"
#include "global.h"
#include <stdarg.h>

FILE *log_stream = NULL;
int log_level =
#ifdef _DEBUG
    1;
#else
    0;
#endif

void log_content(int level, const char *file, int line, const char *func, const char *fmt, ...)
{
    if (level > log_level)
    {
        return;
    }
    if (log_stream == NULL)
    {
        log_stream = stderr;
    }
    va_list args;
    va_start(args, fmt);
#ifdef _DEBUG
    fprintf(log_stream, "%s:%d %s: ", file, line, func);
#endif
    vfprintf(log_stream, fmt, args);
    fprintf(log_stream, "\n");
}
