#ifndef _FT260_H
#define _FT260_H

#include "hid.h"

struct ft260
{
    hid_device_t device;
};

typedef struct ft260 ft260_t;

ft260_t *ft260_new(void);
int ft260_free(ft260_t *ctx);
hid_device_t ft260_get_device(ft260_t *ctx);
int ft260_get_chip_version(ft260_t *ctx, uint8_t **chip_version);
int ft260_get_system_status(ft260_t *ctx, uint8_t **system_status);
int ft260_set_system_clock(ft260_t *ctx);
int ft260_set_i2c_mode(ft260_t *ctx);
int ft260_set_uart_mode(ft260_t *ctx);
int ft260_i2c_reset(ft260_t *ctx);
int ft260_set_i2c_clock_speed(ft260_t *ctx);
int ft260_get_i2c_status(ft260_t *ctx, uint8_t *status);
int ft260_i2c_write_request(ft260_t *ctx, uint8_t address, const uint8_t *buffer, size_t length, bool stop);
int ft260_i2c_read_request(ft260_t *ctx, uint8_t address, uint8_t *buffer, size_t length, bool restart);

#endif
