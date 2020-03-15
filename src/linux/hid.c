#include "hid.h"
#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int hid_open(hid_device_t *device, int vendor_id, int product_id)
{
    *device = open("/dev/hidraw0", O_RDWR);

    if (*device == -1)
        return -1;

    return 0;
}

int hid_close(hid_device_t device)
{
    if (close(device) == -1)
        return -1;

    return 0;
}

ssize_t hid_feature_out(hid_device_t device, const void *buffer, size_t length)
{
    int res = ioctl(device, HIDIOCSFEATURE(length), buffer);

    if (res == -1)
        return -1;

    return (size_t) res;
}

ssize_t hid_feature_in(hid_device_t device, void *buffer, size_t length)
{
    int res = ioctl(device, HIDIOCGFEATURE(length), buffer);

    if (res == -1)
        return -1;

    return (size_t) res;
}

ssize_t hid_interrupt_out(hid_device_t device, const void *buffer, size_t length)
{
    ssize_t bytes_written;

    while (true)
    {
        bytes_written = write(device, buffer, length);

        if (bytes_written == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        break;
    }

	return bytes_written;
}

ssize_t hid_interrupt_in(hid_device_t device, void *buffer, size_t length)
{
    ssize_t bytes_read;

    while (true)
    {
        bytes_read = read(device, buffer, length);

        if (bytes_read == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        break;
    }

	return bytes_read;
}
