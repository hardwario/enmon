#include "main.h"
#include "bridge.h"
#include "sht20.h"
#include "util.h"

int main(int argc, char **argv)
{
    bridge_t *bridge = bridge_new();

    if (bridge == NULL)
        die("Call `bridge_new` failed");

    sht20_t *sht20 = sht20_new(bridge, 0, 0x40);

    float temperature;
    float humidity;

    if (sht20_measure(sht20, &temperature, &humidity) != 0)
        temperature = humidity = NAN;

    if (isnan(temperature))
        printf("@SENSOR: \"Temperature\",NULL\n");
    else
        printf("@SENSOR: \"Temperature\",%.1f\n", temperature);

    if (isnan(humidity))
        printf("@SENSOR: \"Humidity\",NULL\n");
    else
        printf("@SENSOR: \"Humidity\",%.1f\n", humidity);

    if (sht20_free(sht20) != 0)
        die("Call `sht20_free` failed");

    if (bridge_free(bridge) != 0)
        die("Call `bridge_free` failed");

    exit(EXIT_SUCCESS);
}
