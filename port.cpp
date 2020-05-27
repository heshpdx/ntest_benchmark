#include "port.h"
#if defined(_WIN32)

i8 GetTicks(void) {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return (i8)(ticks.QuadPart);
}

i8 GetTicksPerSecond(void) {
    LARGE_INTEGER ticks;
    QueryPerformanceFrequency(&ticks);
    return (i8)(ticks.QuadPart);
}

#else
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

i8 GetTicks(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (i8) 1000000 * tv.tv_sec + tv.tv_usec;
}

i8 GetTicksPerSecond(void) {
    return (i8) 1000000;
}
#endif // _WIN32
