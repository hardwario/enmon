#include "util.h"

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

void delay(int milliseconds)
{
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
