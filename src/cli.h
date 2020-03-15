#ifndef _CLI_H
#define _CLI_H

#include "common.h"

struct cli
{
    bool loop;
    int delay;
};

typedef struct cli cli_t;

void cli_parse(int argc, char **argv, cli_t *cli);

#endif
