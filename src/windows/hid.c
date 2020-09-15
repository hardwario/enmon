#include "hid.h"
#include <hidsdi.h>
#include <hidclass.h>
#include <setupapi.h>
#include <windows.h>
#include <winioctl.h>

int hid_open(hid_device_t *device, int vendor_id, int product_id)
{
    GUID guid = { 0x4d1e55b2, 0xf16f, 0x11cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

    HDEVINFO device_info_set = SetupDiGetClassDevsA(&guid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (device_info_set == INVALID_HANDLE_VALUE)
        return -1;

    bool found = false;

    SP_DEVICE_INTERFACE_DETAIL_DATA_A *device_interface_detail_data;

	for (int device_index = 0; !found; device_index++)
    {
        SP_DEVICE_INTERFACE_DATA device_interface_data;
        device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		if (SetupDiEnumDeviceInterfaces(device_info_set, NULL, &guid, device_index, &device_interface_data) != TRUE)
            break;

        DWORD required_size;

        if (SetupDiGetDeviceInterfaceDetailA(device_info_set, &device_interface_data, NULL, 0, &required_size, NULL) != TRUE)
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                continue;
        }
        else
            continue;

        device_interface_detail_data = malloc(required_size);

        if (device_interface_detail_data == NULL)
            break;

        device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        if (SetupDiGetDeviceInterfaceDetailA(device_info_set, &device_interface_data, device_interface_detail_data, required_size, NULL, NULL) != TRUE)
        {
            free(device_interface_detail_data);
            continue;
        }

        HANDLE handle = CreateFileA(device_interface_detail_data->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        if (handle == INVALID_HANDLE_VALUE)
        {
            free(device_interface_detail_data);
            continue;
        }

        HIDD_ATTRIBUTES attributes;

        if (HidD_GetAttributes(handle, &attributes) == TRUE)
        {
            if (product_id == attributes.ProductID && vendor_id == attributes.VendorID)
            {
                char *pos = strstr(device_interface_detail_data->DevicePath, "&mi_");

                if (pos != NULL)
                {
                    char *hex_str = pos + 4;
                    char *end_ptr = NULL;

                    int mi = strtol(hex_str, &end_ptr, 16);

                    if (end_ptr != hex_str && mi == 0)
                        found = true;
                }
            }
        }

        if (CloseHandle(handle) != TRUE)
            found = false;

        if (!found)
            free(device_interface_detail_data);
    }

    if (found)
    {
        *device = CreateFileA(device_interface_detail_data->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        free(device_interface_detail_data);

        if (*device == INVALID_HANDLE_VALUE)
        {
            SetupDiDestroyDeviceInfoList(device_info_set);
            return -2;
        }
    }

    if (SetupDiDestroyDeviceInfoList(device_info_set) != TRUE)
        return -3;

    return found ? 0 : -4;
}

int hid_close(hid_device_t device)
{
    if (CloseHandle(device) != TRUE)
        return -1;

    return 0;
}

ssize_t hid_feature_out(hid_device_t device, const void *buffer, size_t length)
{
	if (HidD_SetFeature(device, (PVOID) buffer, length) != TRUE)
        return -1;

	return length;
}

ssize_t hid_feature_in(hid_device_t device, void *buffer, size_t length)
{
	OVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));

	DWORD bytes_transferred;

	if (DeviceIoControl(device, IOCTL_HID_GET_FEATURE, buffer, length, buffer, length, &bytes_transferred, &ol) != TRUE)
    {
		if (GetLastError() != ERROR_IO_PENDING)
            return -1;
	}

	if (GetOverlappedResult(device, &ol, &bytes_transferred, TRUE) != TRUE)
		return -2;

	return bytes_transferred;
}

ssize_t hid_interrupt_out(hid_device_t device, const void *buffer, size_t length)
{
    OVERLAPPED ol;
    memset(&ol, 0, sizeof(ol));

    if (WriteFile(device, buffer, length, NULL, &ol) != TRUE)
    {
		if (GetLastError() != ERROR_IO_PENDING)
            return -1;
    }

    DWORD bytes_transferred;

    if (GetOverlappedResult(device, &ol, &bytes_transferred, TRUE) != TRUE)
        return -2;

	return (ssize_t) bytes_transferred;
}

ssize_t hid_interrupt_in(hid_device_t device, void *buffer, size_t length)
{
    OVERLAPPED ol;
    memset(&ol, 0, sizeof(ol));

    DWORD bytes_read;

    if (ReadFile(device, buffer, length, &bytes_read, &ol) != TRUE)
    {
        if (GetLastError() != ERROR_IO_PENDING)
            return -1;
    }

    if (GetOverlappedResult(device, &ol, &bytes_read, TRUE) != TRUE)
        return -2;

    return (ssize_t) bytes_read;
}
