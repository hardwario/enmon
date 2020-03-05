#include "bridge.h"
#include "util.h"

static int _bridge_initialize(bridge_t *ctx)
{
    uint8_t *chip_version;

    if (ft260_get_chip_version(ctx->ft260, &chip_version) != 0)
        die("Call `ft260_get_chip_version` failed");

    uint8_t *system_status;

    if (ft260_get_system_status(ctx->ft260, &system_status) != 0)
        die("Call `ft260_get_system_status` failed");

    if (ft260_set_system_clock(ctx->ft260) != 0)
        die("Call `ft260_set_system_clock` failed");

    if (ft260_set_i2c_mode(ctx->ft260) != 0)
        die("Call `ft260_set_i2c_mode` failed");

    if (ft260_set_uart_mode(ctx->ft260) != 0)
        die("Call `ft260_set_uart_mode` failed");

    if (ft260_i2c_reset(ctx->ft260) != 0)
        die("Call `ft260_i2c_reset` failed");

    if (ft260_set_i2c_clock_speed(ctx->ft260) != 0)
        die("Call `ft260_set_i2c_clock_speed` failed");

    return 0;
}

bridge_t *bridge_new(void)
{
    bridge_t *ctx = calloc(1, sizeof(bridge_t));

    if (ctx == NULL)
        return NULL;

    ctx->ft260 = ft260_new();

    if (ctx->ft260 == NULL)
        goto except;

    if (_bridge_initialize(ctx) != 0)
        goto except;

    if (bridge_set_led(ctx, true) != 0)
        goto except;

    delay(100);

    if (bridge_set_led(ctx, false) != 0)
        goto except;

    if (bridge_i2c_select(ctx, 0) != 0)
        goto except;

    return ctx;

except:

    if (ctx != NULL)
        free(ctx);

    return NULL;
}

int bridge_free(bridge_t *ctx)
{
    free(ctx);

    return 0;
}

ft260_t *bridge_get_ft260(bridge_t *ctx)
{
    return ctx->ft260;
}

int bridge_i2c_select(bridge_t *ctx, int bus)
{
    uint8_t data[] = { bus == 0 ? 0x01 : 0x02 };

    if (ft260_i2c_write_request(ctx->ft260, 0x70, data, sizeof(data), true) != 0)
        return -1;

    return 0;
}

int bridge_set_led(bridge_t *ctx, bool state)
{
    uint8_t data[] = { 0xb0, 0x00, 0x00, state ? 0x80 : 0x00, 0x80 };

    hid_device_t device = ft260_get_device(ctx->ft260);

    if (hid_feature_out(device, data, sizeof(data)) != sizeof(data))
        return -1;

    return 0;
}
