#ifndef _HID_H
#define _HID_H

#include <stddef.h>

#include <IOKit/hid/IOHIDManager.h>

typedef IOHIDDeviceRef hid_device_t;

int hid_open(hid_device_t *device, unsigned short vendor_id, unsigned short product_id);
int hid_send_feature_report(hid_device_t device, const unsigned char *data, size_t length);
int hid_get_feature_report(hid_device_t device, unsigned char *data, size_t length);

#endif
