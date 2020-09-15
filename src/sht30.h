#ifndef _SHT30_H
#define _SHT30_H

#include "bridge.h"

struct sht30
{
    bridge_t *bridge;
    int bus;
    uint8_t address;
    bool initialized;
};

typedef struct sht30 sht30_t;

sht30_t *sht30_new(bridge_t *bridge, int bus, uint8_t address);
int sht30_free(sht30_t *ctx);
int sht30_measure(sht30_t *ctx, float *temperature, float *humidity);

#endif
