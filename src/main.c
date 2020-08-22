#include "main.h"
#include "bridge.h"
#include "cli.h"
#include "mpl3115a2.h"
#include "opt3001.h"
#include "sht20.h"
#include "util.h"

#define MEASURE_TEMPERATURE 1
#define MEASURE_HUMIDITY 1
#define MEASURE_ILLUMINANCE 1
#define MEASURE_PRESSURE 1
#define MEASURE_ALTITUDE 1

int main(int argc, char **argv)
{
    cli_t cli;
    cli_parse(argc, argv, &cli);

    bridge_t *bridge = bridge_new();

    if (bridge == NULL)
        die("Call `bridge_new` failed");

    sht20_t *sht20 = sht20_new(bridge, 0, 0x40);

    if (sht20 == NULL)
        die("Call `sht20_new` failed");

    opt3001_t *opt3001 = opt3001_new(bridge, 0, 0x44);

    if (opt3001 == NULL)
        die("Call `opt3001_new` failed");

    mpl3115a2_t *mpl3115a2 = mpl3115a2_new(bridge, 0, 0x60);

    if (mpl3115a2 == NULL)
        die("Call `mpl3115a2_new` failed");

    while (true)
    {
        if (bridge_ping(bridge) != 0)
            break;

        #if MEASURE_TEMPERATURE != 0 || MEASURE_HUMIDITY != 0
        float temperature = NAN;
        float humidity = NAN;

        sht20_measure(sht20, &temperature, &humidity);

        #if MEASURE_TEMPERATURE != 0
        if (isnan(temperature))
            say("@SENSOR: \"Temperature\",NULL");
        else
            say("@SENSOR: \"Temperature\",%.2f", temperature);
        #endif

        #if MEASURE_HUMIDITY != 0
        if (isnan(humidity))
            say("@SENSOR: \"Humidity\",NULL");
        else
            say("@SENSOR: \"Humidity\",%.1f", humidity);
        #endif
        #endif

        #if MEASURE_ILLUMINANCE != 0
        float illuminance = NAN;

        opt3001_measure(opt3001, &illuminance);

        if (isnan(illuminance))
            say("@SENSOR: \"Illuminance\",NULL");
        else
            say("@SENSOR: \"Illuminance\",%.0f", illuminance);
        #endif

        #if MEASURE_PRESSURE != 0 || MEASURE_ALTITUDE != 0
        float pressure = NAN;
        float altitude = NAN;

        mpl3115a2_measure(mpl3115a2, &pressure, &altitude);

        #if MEASURE_PRESSURE != 0
        if (isnan(pressure))
            say("@SENSOR: \"Pressure\",NULL");
        else
            say("@SENSOR: \"Pressure\",%.0f", pressure);
        #endif

        #if MEASURE_ALTITUDE != 0
        if (isnan(altitude))
            say("@SENSOR: \"Altitude\",NULL");
        else
            say("@SENSOR: \"Altitude\",%.1f", altitude);
        #endif
        #endif

        if (!cli.loop)
            break;
        else
            delay(cli.delay * 1000);
    }

    if (sht20_free(sht20) != 0)
        die("Call `sht20_free` failed");

    if (opt3001_free(opt3001) != 0)
        die("Call `opt3001_free` failed");

    if (mpl3115a2_free(mpl3115a2) != 0)
        die("Call `mpl3115a2_free` failed");

    if (bridge_free(bridge) != 0)
        die("Call `bridge_free` failed");

    exit(EXIT_SUCCESS);
}
