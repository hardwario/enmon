#include "hid.h"

#include <stdio.h> // DELETE

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/USBSpec.h>

int hid_open(hid_device_t *device, unsigned short vendor_id, unsigned short product_id)
{
	io_registry_entry_t entry = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/AppleACPIPlatformExpert/PCI0@0/AppleACPIPCI/XHC1@14/XHC1@14000000/HS10@14600000/FT260@14600000/IOUSBHostInterface@0/AppleUserUSBHostHIDDevice");

	if (entry == MACH_PORT_NULL)
        return -1;

	*device = IOHIDDeviceCreate(kCFAllocatorDefault, entry);

	if (*device == NULL)
        return -2;

	if (IOHIDDeviceOpen(*device, kIOHIDOptionsTypeSeizeDevice) != kIOReturnSuccess)
        return -3;

    return 0;
}

int hid_send_feature_report(hid_device_t device, const unsigned char *data, size_t length)
{
	if (IOHIDDeviceSetReport(device, kIOHIDReportTypeFeature, data[0], data, length) != kIOReturnSuccess)
		return -1;

	return length;
}

int hid_get_feature_report(hid_device_t device, unsigned char *data, size_t length)
{
    CFIndex l = length;

	if (IOHIDDeviceGetReport(device, kIOHIDReportTypeFeature, data[0], data, &l) != kIOReturnSuccess)
		return -1;

	return l;
}
