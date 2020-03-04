#ifndef _HID_H
#define _HID_H

#include <stddef.h>

#include <IOKit/hid/IOHIDManager.h>

typedef IOHIDDeviceRef hid_device_t;

int hid_open(hid_device_t *device, int vendor_id, int product_id);
int hid_close(hid_device_t device);
int hid_send_feature_report(hid_device_t device, const void *data, size_t length);
int hid_get_feature_report(hid_device_t device, void *data, size_t length);

#endif
