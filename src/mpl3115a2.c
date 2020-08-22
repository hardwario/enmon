#include "mpl3115a2.h"
#include "util.h"

mpl3115a2_t *mpl3115a2_new(bridge_t *bridge, int bus, uint8_t address)
{
    mpl3115a2_t *ctx = calloc(1, sizeof(mpl3115a2_t));

    if (ctx == NULL)
        return NULL;

    ctx->bridge = bridge;
    ctx->bus = bus;
    ctx->address = address;

    return ctx;
}

int mpl3115a2_free(mpl3115a2_t *ctx)
{
    free(ctx);

    return 0;
}

int mpl3115a2_measure(mpl3115a2_t *ctx, float *pressure, float *altitude)
{
    if (bridge_i2c_select(ctx->bridge, ctx->bus) != 0)
        return -1;

    ft260_t *ft260 = bridge_get_ft260(ctx->bridge);

    uint8_t buffer[6];

    if (!ctx->initialized)
    {
        buffer[0] = 0x26;
        buffer[1] = 0x04;

        ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true);

        ctx->initialized = true;

        delay(2000);
    }

    buffer[0] = 0x26;
    buffer[1] = 0xb8;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -2;
    }

    buffer[0] = 0x13;
    buffer[1] = 0x07;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -3;
    }

    buffer[0] = 0x26;
    buffer[1] = 0xba;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -4;
    }

    delay(2000);

    buffer[0] = 0x00;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -5;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 1, true) != 0)
    {
        ctx->initialized = false;
        return -6;
    }

    if (buffer[1] != 0x0e)
    {
        ctx->initialized = false;
        return -7;
    }

    buffer[0] = 0x01;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -8;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 5, true) != 0)
    {
        ctx->initialized = false;
        return -9;
    }

    int32_t a = buffer[1] << 24 | buffer[2] << 16 | buffer[3] << 8;
    *altitude = (a >> 12) / 16.f;

    buffer[0] = 0x26;
    buffer[1] = 0x38;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -10;
    }

    buffer[0] = 0x13;
    buffer[1] = 0x07;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -11;
    }

    buffer[0] = 0x26;
    buffer[1] = 0x3a;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -12;
    }

    delay(2000);

    buffer[0] = 0x00;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -13;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 1, true) != 0)
    {
        ctx->initialized = false;
        return -14;
    }

    if (buffer[1] != 0x0e)
    {
        ctx->initialized = false;
        return -15;
    }

    buffer[0] = 0x01;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -16;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 5, true) != 0)
    {
        ctx->initialized = false;
        return -17;
    }

    uint32_t p = buffer[1] << 24 | buffer[2] << 16 | buffer[3] << 8;
    *pressure = (p >> 12) / 4.f;

    return 0;
}
