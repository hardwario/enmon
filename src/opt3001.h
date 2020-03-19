#ifndef _OPT3001_H
#define _OPT3001_H

#include "bridge.h"

struct opt3001
{
    bridge_t *bridge;
    int bus;
    uint8_t address;
    bool initialized;
};

typedef struct opt3001 opt3001_t;

opt3001_t *opt3001_new(bridge_t *bridge, int bus, uint8_t address);
int opt3001_free(opt3001_t *ctx);
int opt3001_measure(opt3001_t *ctx, float *illuminance);

#endif
