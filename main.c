#include "hid.h"
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WSTR_MAX 255

wchar_t wstr[WSTR_MAX];

static void die(const char *format, ...)
{
    va_list va;

    va_start(va, format);

    vfprintf(stderr, format, va);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(va);

    exit(EXIT_FAILURE);
}

static void delay(int milliseconds)
{
    while (1)
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

static int i2c_reset(hid_device_t device)
{
    unsigned char data[] = { 0xa1, 0x20 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int i2c_get_status(hid_device_t device, unsigned char **status)
{
    static unsigned char data[5];

    memset(data, 0, sizeof(data));
    data[0] = 0xc0;

    int res = hid_get_feature_report(device, data, 5);

    printf("res: %d\n", res);

    if (res != sizeof(data))
        return -1;

    if (data[0] != 0xc0)
        return -2;

    *status = data;

    return 0;
}

static int set_led(hid_device_t device, int state)
{
    unsigned char data[] = { 0xb0, 0x00, 0x00, state == 0 ? 0x00 : 0x80, 0x80 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_system_clock(hid_device_t device)
{
    unsigned char data[] = { 0xa1, 0x01, 0x02 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_uart_mode(hid_device_t device)
{
    unsigned char data[] = { 0xa1, 0x03, 0x00 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_i2c_mode(hid_device_t device)
{
    unsigned char data[] = { 0xa1, 0x02, 0x01 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_i2c_clock_speed(hid_device_t device)
{
    unsigned char data[] = { 0xa1, 0x22, 0x64, 0x00 };

    if (hid_send_feature_report(device, data, sizeof(data)) != sizeof(data))
        return -1;

    unsigned char *status;

    if (i2c_get_status(device, &status) != 0)
        return -2;

    if (data[2] != 0x64 || data[3] != 0x00)
        return -3;

    return 0;
}

static int get_chip_version(hid_device_t device, unsigned char **chip_version)
{
    static unsigned char data[13];

    memset(data, 0, sizeof(data));
    data[0] = 0xa0;

    int res = hid_get_feature_report(device, data, sizeof(data));

    printf("res: %d\n", res);

    if (res != sizeof(data))
        return -1;

    if (data[0] != 0xa0)
        return -2;

    *chip_version = data;

    return 0;
}

static int get_system_status(hid_device_t device, unsigned char **status)
{
    static unsigned char data[26];

    memset(data, 0, sizeof(data));
    data[0] = 0xa1;

    int res = hid_get_feature_report(device, data, sizeof(data));

    if (res != sizeof(data) - 1)
    {
        printf("res: %d\n", res);
        return -1;
    }

    if (data[0] != 0xa1)
        return -2;

    *status = data;

    return 0;
}
#if 0
static int i2c_write(hid_device_t device, unsigned char address, const unsigned char *buffer, size_t length, int stop)
{
    unsigned char data[4 + length];

    data[0] = 0xd0 + (length - 1) / 4;
    data[1] = address;
    data[2] = stop == 0 ? 0x02 : 0x06;
    data[3] = length;

    memcpy(&data[4], buffer, length);

    delay(100);

    // TODO add I2C wait?
    if (hid_write(handle, data, 4 + length) != (4 + length))
        return -1;

    /*
    unsigned char *status;

    if (i2c_get_status(handle, &status) != 0)
        return -2;
    */

    // TODO Check status

    return 0;
}

static int i2c_select(hid_device_t device, int bus)
{
    unsigned char data[] = { bus == 0 ? 0x01 : 0x02 };

    return i2c_write(handle, 0x70, data, sizeof(data), 1);
}

#endif

int main(int argc, char **argv)
{
    hid_device_t device;

    if (hid_open(&device, 0x403, 0x6030) != 0)
        die("Call `hid_open` failed");

    /*
    if (hid_init() != 0)
        die("Call `hid_init` failed");

    hid_device *handle = hid_open(0x403, 0x6030, NULL);

    if (handle == NULL)
        die("Call `hid_open` failed");
    */
    // TODO Do we really want non-blocking mode?
    /*
    if (hid_set_nonblocking(handle, 1) != 0)
        die("Call `hid_set_nonblocking` failed");
    */




    /*
    if (hid_get_manufacturer_string(handle, wstr, WSTR_MAX) != 0)
        die("Call `hid_get_manufacturer_string` failed");

    if (wcsncmp(wstr, L"FTDI", 4 + 1) != 0)
        die("Manufacturer string mismatch");

    if (hid_get_product_string(handle, wstr, WSTR_MAX) != 0)
        die("Call `hid_get_product_string` failed");

    if (wcsncmp(wstr, L"FT260", 5 + 1) != 0)
        die("Product string mismatch");

    */
    unsigned char *chip_version;

    if (get_chip_version(device, &chip_version) != 0)
        die("Call `chip_version` failed");

    unsigned char *status;

    if (get_system_status(device, &status) != 0)
        die("Call `get_system_status` failed");

    for (size_t i = 0; i < 25; i++)
        printf("Status %u: 0x%02x\n", i, status[i]);

    if (i2c_reset(device) != 0)
        die("Call `i2c_reset` failed");

    if (set_i2c_mode(device) != 0)
        die("Call `set_i2c_mode` failed");

    if (set_uart_mode(device) != 0)
        die("Call `set_uart_mode` failed");

    if (set_system_clock(device) != 0)
        die("Call `set_system_clock` failed");

    if (set_led(device, 1) != 0)
        die("Call `set_led` failed");

    delay(100);

    if (set_led(device, 0) != 0)
        die("Call `set_led` failed");

    if (set_i2c_clock_speed(device) != 0)
        die("Call `i2c_set_speed` failed");

    /*
    */

    /*
    if (i2c_select(handle, 0) != 0)
        die("Call `i2c_select` failed");
        */

    //if (i2c_get_status(handle, &status) != 0)
      //  die("Call `i2c_get_status` failed");

    //hid_close(handle);

    /*
    if (hid_exit() != 0)
        die("Call `hid_exit` failed");
    */

    exit(EXIT_SUCCESS);
}
