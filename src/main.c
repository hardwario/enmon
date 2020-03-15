#include "main.h"
#include "bridge.h"
#include "cli.h"
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

    while (true)
    {
        float temperature;
        float humidity;

        if (bridge_ping(bridge) != 0)
            break;

        if (sht20_measure(sht20, &temperature, &humidity) != 0)
            temperature = humidity = NAN;

        if (isnan(temperature))
            say("@SENSOR: \"Temperature\",NULL");
        else
            say("@SENSOR: \"Temperature\",%.1f", temperature);

        if (isnan(humidity))
            say("@SENSOR: \"Humidity\",NULL");
        else
            say("@SENSOR: \"Humidity\",%.1f", humidity);

        say("@SENSOR: \"Illuminance\",NULL");
        say("@SENSOR: \"Pressure\",NULL");
        say("@SENSOR: \"Altitude\",NULL");

        if (!cli.loop)
            break;
        else
            delay(cli.delay * 1000);
    }

    if (sht20_free(sht20) != 0)
        die("Call `sht20_free` failed");

    if (bridge_free(bridge) != 0)
        die("Call `bridge_free` failed");

    exit(EXIT_SUCCESS);
}
