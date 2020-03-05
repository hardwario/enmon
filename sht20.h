#ifndef _SHT20_H
#define _SHT20_H

#include "bridge.h"

struct sht20
{
    bridge_t *bridge;
    int bus;
    uint8_t address;
    bool initialized;
};

typedef struct sht20 sht20_t;

sht20_t *sht20_new(bridge_t *bridge, int bus, uint8_t address);
int sht20_free(sht20_t *ctx);
int sht20_measure(sht20_t *ctx, float *temperature, float *humidity);

#endif
