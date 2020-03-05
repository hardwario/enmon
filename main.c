#include "hid.h"
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
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

static int i2c_reset(hid_device_t device)
{
    uint8_t data[] = { 0xa1, 0x20 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int i2c_get_status(hid_device_t device, uint8_t **status)
{
    static uint8_t data[5];

    memset(data, 0, sizeof(data));
    data[0] = 0xc0;

    if (hid_feature_in(device, data, 5) != 5)
        return -1;

    if (data[0] != 0xc0)
        return -2;

    *status = data;

    return 0;
}

static int set_led(hid_device_t device, bool state)
{
    uint8_t data[] = { 0xb0, 0x00, 0x00, state ? 0x80 : 0x00, 0x80 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_system_clock(hid_device_t device)
{
    uint8_t data[] = { 0xa1, 0x01, 0x02 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_uart_mode(hid_device_t device)
{
    uint8_t data[] = { 0xa1, 0x03, 0x00 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_i2c_mode(hid_device_t device)
{
    uint8_t data[] = { 0xa1, 0x02, 0x01 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

static int set_i2c_clock_speed(hid_device_t device)
{
    uint8_t data[] = { 0xa1, 0x22, 0x64, 0x00 };

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    uint8_t *status;

    if (i2c_get_status(device, &status) != 0)
        return -2;

    if (data[2] != 0x64 || data[3] != 0x00)
        return -3;

    return 0;
}

static int get_chip_version(hid_device_t device, uint8_t **chip_version)
{
    static uint8_t data[13];

    memset(data, 0, sizeof(data));
    data[0] = 0xa0;

    if (hid_feature_in(device, data, sizeof(data)) != sizeof(data))
        return -1;

    if (data[0] != 0xa0)
        return -2;

    *chip_version = data;

    return 0;
}

static int get_system_status(hid_device_t device, uint8_t **status)
{
    static uint8_t data[26];

    memset(data, 0, sizeof(data));
    data[0] = 0xa1;

    if (hid_feature_in(device, data, sizeof(data)) != sizeof(data) - 1)
        return -1;

    if (data[0] != 0xa1)
        return -2;

    *status = data;

    return 0;
}

static int i2c_wait(hid_device_t device, bool accept_bus_busy)
{
    while (true)
    {
        uint8_t *status;

        if (i2c_get_status(device, &status) != 0)
            return -1;

        if ((status[1] & 0x01) == 0 && (status[1] & 0x20) != 0)
        {
            if ((status[1] & 0x1e) != 0)
                return -2;
            if (!accept_bus_busy && (status[1] & 0x40) != 0)
                return -3;
            break;
        }

        delay(100);
    }

    return 0;
}

static int i2c_write(hid_device_t device, uint8_t address, const uint8_t *buffer, size_t length, bool stop)
{
    delay(10);

    uint8_t data[4 + length];

    data[0] = 0xd0 + (length - 1) / 4;
    data[1] = address;
    data[2] = stop ? 0x06 : 0x02;
    data[3] = length;

    memcpy(&data[4], buffer, length);

    if (hid_interrupt_out(device, data, 4 + length) != 4 + length)
        return -1;

    if (i2c_wait(device, stop ? false : true) != 0)
        return -2;

    return 0;
}

static int i2c_read(hid_device_t device, uint8_t address, uint8_t *buffer, size_t length, bool restart)
{
    if (!restart)
        delay(10);

    uint8_t data[64];

    data[0] = 0xc2;
    data[1] = address;
    data[2] = restart ? 0x07 : 0x06;
    data[3] = length;
    data[4] = length >> 8;

    // TODO ?
    if (i2c_wait(device, false) != 0)
        return -3;

    if (hid_interrupt_out(device, data, 5) != 5)
        return -1;

    if (hid_interrupt_in(device, data, 64) < length + 2)
        return -2;

    if (i2c_wait(device, false) != 0)
        return -3;

    if (data[0] != 0xd0)
        return -4;

    if (data[1] != length)
        return -5;

    memcpy(buffer, &data[2], length);

    return 0;
}

static int i2c_select(hid_device_t device, int bus)
{
    uint8_t data[] = { bus == 0 ? 0x01 : 0x02 };

    return i2c_write(device, 0x70, data, sizeof(data), true);
}

int main(int argc, char **argv)
{
    hid_device_t device;

    if (hid_open(&device, 0x403, 0x6030) != 0)
        die("Call `hid_open` failed");

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
    uint8_t *chip_version;

    if (get_chip_version(device, &chip_version) != 0)
        die("Call `chip_version` failed");

    uint8_t *status;

    if (get_system_status(device, &status) != 0)
        die("Call `get_system_status` failed");

    for (int i = 0; i < 25; i++)
        printf("Status %d: 0x%02x\n", i, status[i]);

    if (set_system_clock(device) != 0)
        die("Call `set_system_clock` failed");

    if (set_uart_mode(device) != 0)
        die("Call `set_uart_mode` failed");

    if (set_i2c_mode(device) != 0)
        die("Call `set_i2c_mode` failed");

    if (i2c_reset(device) != 0)
        die("Call `i2c_reset` failed");

    if (set_i2c_clock_speed(device) != 0)
        die("Call `i2c_set_speed` failed");

    if (set_led(device, 1) != 0)
        die("Call `set_led` failed");

    delay(100);

    if (set_led(device, 0) != 0)
        die("Call `set_led` failed");

    if (i2c_select(device, 0) != 0)
        die("Call `i2c_select` failed");

    // ---------------

    uint8_t buffer[16];

    buffer[0] = 0xfe;

    if (i2c_write(device, 0x40, buffer, 1, true) != 0)
        die("Call `i2c_write` failed");

    delay(100);

    buffer[0] = 0xf5;
    if (i2c_write(device, 0x40, buffer, 1, true) != 0)
        die("Call `i2c_write` failed");

    delay(100);

    int res = i2c_read(device, 0x40, buffer, 2, false);
    if (res != 0)
        die("Call `i2c_read` failed (%d)", res);

    int16_t value = buffer[0] << 8 | buffer[1];

    float humidity = -6 + 125 * value / 65536.f;
    printf("Humidity: %.1f %%\n", humidity);

    // ---------------

    if (hid_close(device) != 0)
        die("Call `hid_close` failed");

    exit(EXIT_SUCCESS);
}
