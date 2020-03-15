#include "util.h"
#if defined(_MSC_VER)
#include <windows.h>
#endif

void say(const char *format, ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stdout, format, va);
    va_end(va);

    fprintf(stdout, "\n");
    fflush(stdout);
}

void cry(const char *format, ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);

    fprintf(stderr, "\n");
    fflush(stderr);
}

void die(const char *format, ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);

    fprintf(stderr, "\n");
    fflush(stderr);

    exit(EXIT_FAILURE);
}

#if defined(_MSC_VER)
static int usleep(int64_t usec)
{
    LARGE_INTEGER ft;
    ft.QuadPart = -10 * usec;

    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);

    if (timer == NULL)
        return -1;

    if (SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0) == FALSE)
    {
        CloseHandle(timer);
        return -2;
    }

    if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0)
    {
        CloseHandle(timer);
        return -3;
    }

    if (CloseHandle(timer) != TRUE)
        return -4;

    return 0;
}
#endif

void delay(int milliseconds)
{
    if (milliseconds == 0)
        return;

    while (true)
    {
        if (usleep(milliseconds * 1000) != 0)
        {
            if (errno == EINTR)
                continue;

            die("Call `usleep` failed");
        }

        return;
    }
}
