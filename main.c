#include "main.h"
#include "bridge.h"
#include "sht20.h"
#include "util.h"

#define WSTR_MAX 255

wchar_t wstr[WSTR_MAX];

int main(int argc, char **argv)
{./en
    bridge_t *bridge = bridge_new();

    if (bridge == NULL)
        die("Call `bridge_new` failed");

    sht20_t *sht20 = sht20_new(bridge, 0, 0x40);

    if (sht20_measure(sht20) != 0)
        die("Call `sht20_measure` failed");


    /*
    if (hid_get_manufacturer_string(handle, wstr, WSTR_MAX) != 0)
        die("Call `hid_get_manufacturer_string` failed");

    if (wcsncmp(wstr, L"FTDI", 4 + 1) != 0)
        die("Manufacturer string mismatch");

    if (hid_get_product_string(handle, wstr, WSTR_MAX) != 0)
        die("Call `hid_get_product_string` failed");

    if (wcsncmp(wstr, L"FT260", 5 + 1) != 0)
        die("Product string mismatch");
    */

    if (sht20_free(sht20) != 0)
        die("Call `sht20_free` failed");

    if (bridge_free(bridge) != 0)
        die("Call `bridge_free` failed");

    exit(EXIT_SUCCESS);
}
