#include "unistd.h"

void sleep_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    if (milliseconds >= 1000)
        sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

#include <time.h>
int64_t time_ns(void)
{
#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__)
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return (int64_t)((counter.QuadPart * 1000000000) / frequency.QuadPart);
#else
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (int64_t)(ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec);
#endif
}
