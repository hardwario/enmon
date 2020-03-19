#include "main.h"
#include "bridge.h"
#include "cli.h"
#include "opt3001.h"
#include "sht20.h"
#include "util.h"

int main(int argc, char **argv)
{
    cli_t cli;
    cli_parse(argc, argv, &cli);

    bridge_t *bridge = bridge_new();

    if (bridge == NULL)
        die("Call `bridge_new` failed");

    sht20_t *sht20 = sht20_new(bridge, 0, 0x40);
    opt3001_t *opt3001 = opt3001_new(bridge, 0, 0x44);

    while (true)
    {
        float temperature = NAN;
        float humidity = NAN;
        float illuminance = NAN;

        if (bridge_ping(bridge) != 0)
            break;

        sht20_measure(sht20, &temperature, &humidity);

        if (isnan(temperature))
            say("@SENSOR: \"Temperature\",NULL");
        else
            say("@SENSOR: \"Temperature\",%.2f", temperature);

        if (isnan(humidity))
            say("@SENSOR: \"Humidity\",NULL");
        else
            say("@SENSOR: \"Humidity\",%.1f", humidity);

        opt3001_measure(opt3001, &illuminance);

        if (isnan(illuminance))
            say("@SENSOR: \"Illuminance\",NULL");
        else
            say("@SENSOR: \"Illuminance\",%.0f", illuminance);

        say("@SENSOR: \"Pressure\",NULL");
        say("@SENSOR: \"Altitude\",NULL");

        if (!cli.loop)
            break;
        else
            delay(cli.delay * 1000);
    }

    if (opt3001_free(opt3001) != 0)
        die("Call `opt3001_free` failed");

    if (sht20_free(sht20) != 0)
        die("Call `sht20_free` failed");

    if (bridge_free(bridge) != 0)
        die("Call `bridge_free` failed");

    exit(EXIT_SUCCESS);
}
