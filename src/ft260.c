#include "ft260.h"
#include "util.h"

ft260_t *ft260_new(void)
{
    ft260_t *ctx = calloc(1, sizeof(ft260_t));

    if (ctx == NULL)
        return NULL;

    if (hid_open(&ctx->device, 0x403, 0x6030) != 0)
        return NULL;

    return ctx;
}

int ft260_free(ft260_t *ctx)
{
    if (hid_close(ctx->device) != 0)
        return -1;

    free(ctx);

    return 0;
}

hid_device_t ft260_get_device(ft260_t *ctx)
{
    return ctx->device;
}

int ft260_get_chip_version(ft260_t *ctx, uint8_t **chip_version)
{
    uint8_t *data = calloc(1, 64);

    if (data == NULL)
        return -1;


    data[0] = 0xa0;

    if (hid_feature_in(ctx->device, data, 64) != 13)
    {
        free(data);
        return -2;
    }

    if (data[0] != 0xa0)
    {
        free(data);
        return -3;
    }

    *chip_version = data;

    return 0;
}

int ft260_get_system_status(ft260_t *ctx, uint8_t **system_status)
{
    uint8_t *data = calloc(1, 64);

    if (data == NULL)
        return -1;

    data[0] = 0xa1;

    if (hid_feature_in(ctx->device, data, 64) != 25)
    {
        free(data);
        return -2;
    }

    if (data[0] != 0xa1)
    {
        free(data);
        return -3;
    }

    *system_status = data;

    return 0;
}

int ft260_set_system_clock(ft260_t *ctx)
{
    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xa1;
    data[1] = 0x01;
    data[2] = 0x02;

    if (hid_feature_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

int ft260_set_i2c_mode(ft260_t *ctx)
{
    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xa1;
    data[1] = 0x02;
    data[2] = 0x01;

    if (hid_feature_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

int ft260_set_uart_mode(ft260_t *ctx)
{
    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xa1;
    data[1] = 0x03;
    data[2] = 0x00;

    if (hid_feature_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

int ft260_i2c_reset(ft260_t *ctx)
{
    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xa1;
    data[1] = 0x20;

    if (hid_feature_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

int ft260_set_i2c_clock_speed(ft260_t *ctx)
{
    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xa1;
    data[1] = 0x22;
    data[2] = 0x64;
    data[3] = 0x00;

    if (hid_feature_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}

int ft260_get_i2c_status(ft260_t *ctx, uint8_t *status)
{
    uint8_t *data = calloc(1, 64);

    if (data == NULL)
        return -1;

    data[0] = 0xc0;

    if (hid_feature_in(ctx->device, data, 64) != 5)
    {
        free(data);
        return -2;
    }

    if (data[0] != 0xc0)
    {
        free(data);
        return -3;
    }

    *status = data[1];

    free(data);
    return 0;
}

static int _ft260_i2c_wait(ft260_t *ctx, bool accept_bus_busy)
{
    while (true)
    {
        uint8_t status;

        delay(10);

        if (ft260_get_i2c_status(ctx, &status) != 0)
            return -1;

        if ((status & 0x01) == 0)
        {
            if ((status & 0x1e) != 0)
                return -2;

            if (accept_bus_busy)
            {
                if ((status & 0x40) == 0)
                    return -3;
            }
            else
            {
                if ((status & 0x40) != 0)
                    continue;

                if ((status & 0x20) == 0)
                    return -4;
            }

            break;
        }
    }

    return 0;
}

int ft260_i2c_write_request(ft260_t *ctx, uint8_t address, const uint8_t *buffer, size_t length, bool stop)
{
    delay(10);

    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xd0 + ((uint8_t) length - 1) / 4;
    data[1] = address;
    data[2] = stop ? 0x06 : 0x02;
    data[3] = (uint8_t) length;

    memcpy(&data[4], buffer, length);

    if (hid_interrupt_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    if (_ft260_i2c_wait(ctx, stop ? false : true) != 0)
        return -2;

    return 0;
}

int ft260_i2c_read_request(ft260_t *ctx, uint8_t address, uint8_t *buffer, size_t length, bool restart)
{
    if (!restart)
        delay(10);

    uint8_t data[64];

    memset(data, 0, sizeof(data));

    data[0] = 0xc2;
    data[1] = address;
    data[2] = restart ? 0x07 : 0x06;
    data[3] = (uint8_t) length;
    data[4] = (uint8_t) (length >> 8);

    if (hid_interrupt_out(ctx->device, data, sizeof(data)) != sizeof(data))
        return -1;

    if (hid_interrupt_in(ctx->device, data, sizeof(data)) < (ssize_t) length + 2)
        return -2;

    if (_ft260_i2c_wait(ctx, false) != 0)
        return -3;

    if (data[0] != 0xd0 + ((uint8_t) length - 1) / 4)
        return -4;

    if (data[1] != length)
        return -5;

    memcpy(buffer, &data[2], length);

    return 0;
}
