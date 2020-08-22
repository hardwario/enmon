#include "sht20.h"
#include "util.h"

sht20_t *sht20_new(bridge_t *bridge, int bus, uint8_t address)
{
    sht20_t *ctx = calloc(1, sizeof(sht20_t));

    if (ctx == NULL)
        return NULL;

    ctx->bridge = bridge;
    ctx->bus = bus;
    ctx->address = address;

    return ctx;
}

int sht20_free(sht20_t *ctx)
{
    free(ctx);

    return 0;
}

int sht20_measure(sht20_t *ctx, float *temperature, float *humidity)
{
    if (bridge_i2c_select(ctx->bridge, ctx->bus) != 0)
        return -1;

    ft260_t *ft260 = bridge_get_ft260(ctx->bridge);

    uint8_t buffer[2];

    if (!ctx->initialized)
    {
        buffer[0] = 0xfe;

        if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, true) != 0)
            return -1;

        ctx->initialized = true;

        delay(100);
    }

    buffer[0] = 0xe3;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, true) != 0)
    {
        ctx->initialized = false;

        return -2;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, buffer, 2, false) != 0)
    {
        ctx->initialized = false;

        return -3;
    }

    uint16_t t = buffer[0] << 8 | buffer[1]; t &= ~0x3;
    *temperature = -46.85f + 175.72f * (float) t / 65536.f;

    buffer[0] = 0xe5;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, true) != 0)
    {
        ctx->initialized = false;

        return -4;
    }

    if (ft260_i2c_read_request(ft260, ctx->address, buffer, 2, false) != 0)
    {
        ctx->initialized = false;

        return -5;
    }

    uint16_t rh = buffer[0] << 8 | buffer[1]; rh &= ~0x3;
    *humidity = -6.f + 125.f * (float) rh / 65536.f;

    return 0;
}
