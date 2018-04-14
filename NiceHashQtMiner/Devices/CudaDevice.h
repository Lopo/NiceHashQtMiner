#ifndef DEVICES_CUDADEVICE_H_
#define DEVICES_CUDADEVICE_H_

#include <QString>


class CudaDevice
{
public:
	uint32_t DeviceID;
	int32_t pciBusID;
	int32_t VendorID;
	QString VendorName;
	QString DeviceName;
	QString SMVersionString;
	int32_t SM_major;
	int32_t SM_minor;
	QString UUID;
	uint64_t DeviceGlobalMemory;
	uint32_t pciDeviceId; // the combined 16b device id and 16b vendor id
	uint32_t pciSubSystemId; // the 32b Sub System Device ID
	int32_t SMX;

	QString GetName();
	bool IsEtherumCapable();
};

#endif
