#include "hid.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/usb/USBSpec.h>

int hid_open(hid_device_t *device, int vendor_id, int product_id)
{
    #if 1

    CFMutableDictionaryRef matching = CFDictionaryCreateMutable(kCFAllocatorDefault, kIOHIDOptionsTypeNone, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    if (matching == NULL)
        return -1;

    SInt32 vid = vendor_id;

    CFNumberRef value_vid = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vid);
    CFDictionarySetValue(matching, CFSTR(kIOHIDVendorIDKey), value_vid);
    CFRelease(value_vid);

    SInt32 pid = product_id;

    CFNumberRef value_pid = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pid);
    CFDictionarySetValue(matching, CFSTR(kIOHIDProductIDKey), value_pid);
    CFRelease(value_pid);

    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    IOHIDManagerSetDeviceMatching(manager, matching);
    CFRelease(matching);

    CFSetRef devices = IOHIDManagerCopyDevices(manager);

    if (devices == NULL)
        return -2;

    CFIndex index, count = CFSetGetCount(devices);
    if (count == 0)
        return -3;

    IOHIDDeviceRef *array = calloc(count, sizeof(IOHIDDeviceRef));

    if (array == NULL)
        return -4;

    CFSetGetValues(devices, (const void **) array);
    CFRelease(devices);

    for (index = 0; index < count; index++)
    {
        *device = array[index];

        if (!*device)
            continue;

        CFTypeRef ref_ifn = IOHIDDeviceGetProperty(*device, CFSTR(kUSBInterfaceNumber));

        SInt32 value;
        CFNumberGetValue(ref_ifn, kCFNumberSInt32Type, &value);

        if (value != 0)
            continue;

        if (IOHIDDeviceOpen(*device, kIOHIDOptionsTypeSeizeDevice) == kIOReturnSuccess)
            break;
    }

    free(array);

    return index == count ? -5 : 0;

    #else

	io_registry_entry_t entry = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/AppleACPIPlatformExpert/PCI0@0/AppleACPIPCI/XHC1@14/XHC1@14000000/HS10@14600000/FT260@14600000/IOUSBHostInterface@0/AppleUserUSBHostHIDDevice");

	if (entry == MACH_PORT_NULL)
        return -3;

	*device = IOHIDDeviceCreate(kCFAllocatorDefault, entry);

	if (*device == NULL)
        return -4;

	if (IOHIDDeviceOpen(*device, kIOHIDOptionsTypeSeizeDevice) != kIOReturnSuccess)
        return -5;

    return 0;

    #endif
}

int hid_send_feature_report(hid_device_t device, const void *data, size_t length)
{
	if (IOHIDDeviceSetReport(device, kIOHIDReportTypeFeature, *(uint8_t *) data, (const uint8_t *) data, length) != kIOReturnSuccess)
		return -1;

	return length;
}

int hid_get_feature_report(hid_device_t device, void *data, size_t length)
{
    CFIndex l = length;

	if (IOHIDDeviceGetReport(device, kIOHIDReportTypeFeature, *(uint8_t *) data, (uint8_t *) data, &l) != kIOReturnSuccess)
		return -1;

	return l;
}
