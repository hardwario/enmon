#include "hid.h"
#include <hidsdi.h>
#include <setupapi.h>
#include <windows.h>

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

        HANDLE handle = CreateFileA(device_interface_detail_data->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

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
                PHIDP_PREPARSED_DATA pp_data;

                if (HidD_GetPreparsedData(handle, &pp_data) == TRUE)
                {
                    HIDP_CAPS caps;

                    if (HidP_GetCaps(pp_data, &caps) == HIDP_STATUS_SUCCESS)
                    {
                        if (caps.Usage == 1)
                            found = true;
                    }

                    HidD_FreePreparsedData(pp_data);
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
        printf("Found Path: %s\n", device_interface_detail_data->DevicePath);

        *device = CreateFileA(device_interface_detail_data->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

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
	return length;
}

ssize_t hid_feature_in(hid_device_t device, void *buffer, size_t length)
{
	return length;
}

ssize_t hid_interrupt_out(hid_device_t device, const void *buffer, size_t length)
{
	return length;
}

ssize_t hid_interrupt_in(hid_device_t device, void *buffer, size_t length)
{
    return length;
}
