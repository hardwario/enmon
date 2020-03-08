#ifndef _HID_H
#define _HID_H

#include "common.h"

#if defined(_MSC_VER)

#include <windows.h>

typedef HANDLE hid_device_t;

#else

#include <IOKit/hid/IOHIDManager.h>

typedef IOHIDDeviceRef hid_device_t;

#endif

int hid_open(hid_device_t *device, int vendor_id, int product_id);
int hid_close(hid_device_t device);
ssize_t hid_feature_out(hid_device_t device, const void *buffer, size_t length);
ssize_t hid_feature_in(hid_device_t device, void *buffer, size_t length);
ssize_t hid_interrupt_out(hid_device_t device, const void *buffer, size_t length);
ssize_t hid_interrupt_in(hid_device_t device, void *buffer, size_t length);

#endif
