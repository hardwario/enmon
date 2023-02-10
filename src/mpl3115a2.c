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
        buffer[0] = 0x26; /* CTRL_REG1 */
        buffer[1] = 0x04; /* Reset device */

        ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true);

        ctx->initialized = true;

        delay(2000);
    }

    buffer[0] = 0x26; /* CTRL_REG1 */
    buffer[1] = 0xb8; /* Set altimeter mode, set oversampling 2^7 = 128 */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -2;
    }

    buffer[0] = 0x13; /* PT_DATA_CFG */
    buffer[1] = 0x07; /* Activate pressure, temperature and event flag generator */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -3;
    }

    buffer[0] = 0x26; /* CTRL_REG1 */
    buffer[1] = 0xba; /* Initiate altitute measurement */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -4;
    }

    /* Measurement delay */
    delay(2000);

    buffer[0] = 0x00; /* STATUS */

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

    /* Check for new pressure or temperature data ready bit is set */
    if (buffer[1] != 0x0e)
    {
        ctx->initialized = false;
        return -7;
    }

    buffer[0] = 0x01; /* Pressure data MSB register */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -8;
    }

    /* Read 5 bytes, 3 for altitude and 2 for temperature */
    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 5, true) != 0)
    {
        ctx->initialized = false;
        return -9;
    }

    int32_t a = buffer[1] << 24 | buffer[2] << 16 | buffer[3] << 8;
    *altitude = (a >> 12) / 16.f;

    buffer[0] = 0x26; /* CTRL_REG1 */
    buffer[1] = 0x38; /* Set pressure mode, set oversampling 2^7 = 128 */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -10;
    }

    buffer[0] = 0x13; /* PT_DATA_CFG */
    buffer[1] = 0x07; /* Activate pressure, temperature and event flag generator */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -11;
    }

    buffer[0] = 0x26; /* CTRL_REG1 */
    buffer[1] = 0x3a; /* Initiate pressure measurement */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 2, true) != 0)
    {
        ctx->initialized = false;
        return -12;
    }

    /* Measurement delay */
    delay(2000);

    buffer[0] = 0x00; /* STATUS */

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

    /* Check for new pressure or temperature data ready bit is set */
    if (buffer[1] != 0x0e)
    {
        ctx->initialized = false;
        return -15;
    }

    buffer[0] = 0x01; /* Pressure data MSB register */

    if (ft260_i2c_write_request(ft260, ctx->address, buffer, 1, false) != 0)
    {
        ctx->initialized = false;
        return -16;
    }

    /* Read 5 bytes, 3 for pressure and 2 for temperature */
    if (ft260_i2c_read_request(ft260, ctx->address, &buffer[1], 5, true) != 0)
    {
        ctx->initialized = false;
        return -17;
    }

    uint32_t p = buffer[1] << 24 | buffer[2] << 16 | buffer[3] << 8;
    *pressure = (p >> 12) / 4.f;

    return 0;
}
