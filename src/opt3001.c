#include "opt3001.h"
#include "util.h"

opt3001_t *opt3001_new(bridge_t *bridge, int bus, uint8_t address)
{
    opt3001_t *ctx = calloc(1, sizeof(opt3001_t));

    if (ctx == NULL)
        return NULL;

    ctx->bridge = bridge;
    ctx->bus = bus;
    ctx->address = address;

    return ctx;
}

int opt3001_free(opt3001_t *ctx)
{
    free(ctx);

    return 0;
}

int opt3001_measure(opt3001_t *ctx, float *illuminance)
{
    if (bridge_i2c_select(ctx->bridge, ctx->bus) != 0)
        return -1;

    ft260_t *ft260 = bridge_get_ft260(ctx->bridge);

    uint8_t buffer[3];

    if (!ctx->initialized)
    {
        buffer[0] = 0x01;
        buffer[1] = 0xc8;
        buffer[2] = 0x10;

        if (ft260_i2c_write_request(ft260, ctx->address, buffer, 3, true) != 0)
            return -1;

        ctx->initialized = true;

        delay(1000);
    }

    buffer[0] = 0x01;
    buffer[1] = 0xca;
    buffer[2] = 0x10;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 3, true) != 0)
    {
        ctx->initialized = false;

        return -2;
    }

    delay(2000);

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;

        return -3;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 2, true) != 0)
    {
        ctx->initialized = false;

        return -4;
    }

    if ((buffer[1] & 0x06) != 0x00 || (buffer[2] & 0x80) != 0x80)
    {
        ctx->initialized = false;

        return -5;
    }

    buffer[0] = 0x00;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;

        return -6;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 2, true) != 0)
    {
        ctx->initialized = false;

        return -7;
    }

    uint16_t r = buffer[1] << 8 | buffer[2];

    *illuminance = ((1 << (r >> 12)) * (r & 0xfff)) * 0.01f;

    return 0;
}
