#include "sht30.h"
#include "util.h"

sht30_t *sht30_new(bridge_t *bridge, int bus, uint8_t address)
{
    sht30_t *ctx = calloc(1, sizeof(sht30_t));

    if (ctx == NULL)
        return NULL;

    ctx->bridge = bridge;
    ctx->bus = bus;
    ctx->address = address;

    return ctx;
}

int sht30_free(sht30_t *ctx)
{
    free(ctx);

    return 0;
}

int sht30_measure(sht30_t *ctx, float *temperature, float *humidity)
{
    if (bridge_i2c_select(ctx->bridge, ctx->bus) != 0)
        return -1;

    ft260_t *ft260 = bridge_get_ft260(ctx->bridge);

    uint8_t buffer[6];

    if (!ctx->initialized)
    {
        buffer[0] = 0x30;
        buffer[1] = 0xa2;

        if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
            return -1;

        ctx->initialized = true;

        delay(100);
    }

    buffer[0] = 0x24;
    buffer[1] = 0x00;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;

        return -2;
    }

    delay(100);

    if (ft260_i2c_read_request(ft260, ctx->address, buffer, 6, false) != 0)
    {
        ctx->initialized = false;

        return -3;
    }

    uint16_t t = buffer[0] << 8 | buffer[1];
    *temperature = -45.f + 175.f * (float) t / 65535.f;

    uint16_t rh = buffer[3] << 8 | buffer[4];
    *humidity = 100.f * (float) rh / 65535.f;

    return 0;
}
