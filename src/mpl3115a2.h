#ifndef _MPL3115A2_H
#define _MPL3115A2_H

#include "bridge.h"

struct mpl3115a2
{
    bridge_t *bridge;
    int bus;
    uint8_t address;
    bool initialized;
};

typedef struct mpl3115a2 mpl3115a2_t;

mpl3115a2_t *mpl3115a2_new(bridge_t *bridge, int bus, uint8_t address);
int mpl3115a2_free(mpl3115a2_t *ctx);
int mpl3115a2_measure(mpl3115a2_t *ctx, float *pressure, float *altitude);

#endif
