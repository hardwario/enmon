#include "hid.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/usb/USBSpec.h>

static uint8_t m_hid_buffer[64];
static size_t m_hid_length;

static void hid_report_callback(void *context, IOReturn result, void *sender, IOHIDReportType type, uint32_t reportID, uint8_t *report, CFIndex length)
{
    m_hid_length = length;
}

int hid_open(hid_device_t *device, int vendor_id, int product_id)
{
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

        SInt32 value;

        CFTypeRef interface = IOHIDDeviceGetProperty(*device, CFSTR(kUSBInterfaceNumber));
        CFNumberGetValue(interface, kCFNumberSInt32Type, &value);
        CFRelease(interface);

        if (value != 0)
            continue;

        if (IOHIDDeviceOpen(*device, kIOHIDOptionsTypeSeizeDevice) == kIOReturnSuccess)
        {
            IOHIDDeviceRegisterInputReportCallback(*device, m_hid_buffer, sizeof(m_hid_buffer), hid_report_callback, NULL);
            IOHIDDeviceScheduleWithRunLoop(*device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

            break;
        }
    }

    free(array);

    return index == count ? -5 : 0;
}

int hid_close(hid_device_t device)
{
    if (IOHIDDeviceClose(device, kIOHIDOptionsTypeNone) != kIOReturnSuccess)
        return -1;

    return 0;
}

ssize_t hid_feature_out(hid_device_t device, const void *buffer, size_t length)
{
	if (IOHIDDeviceSetReport(device, kIOHIDReportTypeFeature, *(uint8_t *) buffer, (const uint8_t *) buffer, length) != kIOReturnSuccess)
		return -1;

	return length;
}

ssize_t hid_feature_in(hid_device_t device, void *buffer, size_t length)
{
	if (IOHIDDeviceGetReport(device, kIOHIDReportTypeFeature, *(uint8_t *) buffer, (uint8_t *) buffer, (CFIndex *) &length) != kIOReturnSuccess)
		return -1;

	return length;
}

ssize_t hid_interrupt_out(hid_device_t device, const void *buffer, size_t length)
{
	if (IOHIDDeviceSetReport(device, kIOHIDReportTypeOutput, *(uint8_t *) buffer, (const uint8_t *) buffer, length) != kIOReturnSuccess)
		return -1;

	return length;
}

ssize_t hid_interrupt_in(hid_device_t device, void *buffer, size_t length)
{
    if (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0, true) != kCFRunLoopRunHandledSource)
        return -1;

    length = length < m_hid_length ? length : m_hid_length;
    memcpy(buffer, m_hid_buffer, length);

    return length;
}
