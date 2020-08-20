#include "hid.h"
#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libudev.h>
#include <util.h>

int hid_open(hid_device_t *device, int vendor_id, int product_id)
{
    struct udev *udev;
    udev = udev_new();

    if (udev == NULL)
        die("Call `udev_new` failed");

    char *hid_path = NULL;

    struct udev_enumerate *enumerate;
    struct udev_list_entry *entry, *devices_hid;
    struct udev_device *hid_dev;
    struct udev_device *usb_dev;
    const char *str;

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices_hid = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(entry, devices_hid)
    {
        str = (char *) udev_list_entry_get_name(entry);
        hid_dev = udev_device_new_from_syspath(udev, str);

        usb_dev = udev_device_get_parent_with_subsystem_devtype(hid_dev, "usb", "usb_device");

        str = udev_device_get_sysattr_value(usb_dev, "idVendor");

        if ((uint16_t) ((str) ? strtol(str, NULL, 16) : -1) != vendor_id)
        {
            udev_device_unref(hid_dev);
            continue;
        }

        str = udev_device_get_sysattr_value(usb_dev, "idProduct");

        if ((uint16_t) ((str) ? strtol(str, NULL, 16) : -1) != product_id)
        {
            udev_device_unref(hid_dev);
            continue;
        }

        hid_path = strdup(udev_device_get_devnode(hid_dev));
        udev_device_unref(hid_dev);
        break;
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    if (hid_path == NULL)
        return -1;

    *device = open(hid_path, O_RDWR);

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
