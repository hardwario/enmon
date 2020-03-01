#include "hidapi.h"

int main(int argc, char **argv)
{
    int res;
    res = hid_init();

    hid_device *handle = hid_open(0x4d8, 0x3f, NULL);

    hid_close(handle);

    res = hid_exit();

    return 0;
}
