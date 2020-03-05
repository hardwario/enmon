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

int sht20_measure(sht20_t *ctx)
{
    ft260_t *ft260 = bridge_get_ft260(ctx->bridge);

    uint8_t buffer[16];

    buffer[0] = 0xfe;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, true) != 0)
        die("Call `i2c_write` failed");

    delay(100);

    buffer[0] = 0xf5;

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, true) != 0)
        die("Call `i2c_write` failed");

    delay(100);

    int res = ft260_i2c_read_request(ft260, ctx->address, buffer, 2, false);
    if (res != 0)
        die("Call `i2c_read` failed (%d)", res);

    int16_t value = buffer[0] << 8 | buffer[1];

    float humidity = -6 + 125 * value / 65536.f;
    printf("Humidity: %.1f %%\n", humidity);

    return 0;
}
