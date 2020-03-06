#ifndef _BRIDGE_H
#define _BRIDGE_H

#include "ft260.h"

struct bridge
{
    ft260_t *ft260;
    int bus;
};

typedef struct bridge bridge_t;

bridge_t *bridge_new(void);
int bridge_free(bridge_t *ctx);
ft260_t *bridge_get_ft260(bridge_t *ctx);
int bridge_i2c_select(bridge_t *ctx, int bus);
int bridge_set_led(bridge_t *ctx, bool state);

#endif
