#include "hid.h"

#include <stdio.h> // DELETE

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/USBSpec.h>

int hid_open(hid_device_t *device, unsigned short vendor_id, unsigned short product_id)
{
    CFMutableDictionaryRef matching = CFDictionaryCreateMutable(kCFAllocatorDefault, kIOHIDOptionsTypeNone, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    if (matching == NULL)
        return -1;

    CFNumberRef v = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &vendor_id);
    CFDictionarySetValue(matching, CFSTR(kIOHIDVendorIDKey), v);
    CFRelease(v);

    CFNumberRef p = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &product_id);
    CFDictionarySetValue(matching, CFSTR(kIOHIDProductIDKey), p);
    CFRelease(p);

    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    IOHIDManagerSetDeviceMatching(manager, NULL); // TODO it crashes without this call

    //if (IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone) != kIOReturnSuccess)
        //return -2;

    IOHIDManagerSetDeviceMatching(manager, matching);
    CFRelease(matching);

    CFSetRef devices = IOHIDManagerCopyDevices(manager);
    CFIndex count = CFSetGetCount(devices);

    printf("Device count: %ld\n", (long) CFSetGetCount(devices));
    /*

    IOHIDManagerClose(manager, kIOHIDOptionsTypeNone);
	CFRelease(manager);

    CFRelease(devices);
    */

	io_registry_entry_t entry = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/AppleACPIPlatformExpert/PCI0@0/AppleACPIPCI/XHC1@14/XHC1@14000000/HS10@14600000/FT260@14600000/IOUSBHostInterface@0/AppleUserUSBHostHIDDevice");

	if (entry == MACH_PORT_NULL)
        return -3;

	*device = IOHIDDeviceCreate(kCFAllocatorDefault, entry);

	if (*device == NULL)
        return -4;

	if (IOHIDDeviceOpen(*device, kIOHIDOptionsTypeSeizeDevice) != kIOReturnSuccess)
        return -5;

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
