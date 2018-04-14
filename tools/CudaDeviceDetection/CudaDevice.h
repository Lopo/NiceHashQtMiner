#ifndef NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADEVICE_H
#define NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADEVICE_H

#include <string>


struct CudaDevice
{
	unsigned int DeviceID;
	int pciBusID;
	std::string VendorName;
	std::string DeviceName;
	std::string SMVersionString;
	int SM_major;
	int SM_minor;
	std::string UUID;
	size_t DeviceGlobalMemory;
	unsigned long pciDeviceId; //!< The combined 16-bit device id and 16-bit vendor id
	unsigned int pciSubSystemId; //!< The 32-bit Sub System Device ID
	int SMX;
	int VendorID;
};

#endif // NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADEVICE_H
