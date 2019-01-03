#ifndef NHQTM_TOOLS_AMDOPENCLDEVICEDETECTION_OPENCLDEVICE_H
#define NHQTM_TOOLS_AMDOPENCLDEVICEDETECTION_OPENCLDEVICE_H

#include <string>

// This will list OpenCL devices, but AMD will only have aditional BusID
struct OpenCLDevice
{
	unsigned int DeviceID;
	std::string _CL_DEVICE_NAME;
	std::string _CL_DEVICE_TYPE;
	unsigned long long _CL_DEVICE_GLOBAL_MEM_SIZE;
	uint64_t _CL_DEVICE_VENDOR_ID;
	std::string _CL_DEVICE_VENDOR;
	std::string _CL_DEVICE_VERSION;
	std::string _CL_DRIVER_VERSION;
	int AMD_BUS_ID=-1; // -1 indicates that it is not set
};

#endif // NHQTM_TOOLS_AMDOPENCLDEVICEDETECTION_OPENCLDEVICE_H
