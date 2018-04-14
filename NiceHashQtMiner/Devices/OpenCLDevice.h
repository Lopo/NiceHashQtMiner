#ifndef DEVICES_OPENCLDEVICE_H_
#define DEVICES_OPENCLDEVICE_H_

#include <QString>


class OpenCLDevice
{
public:
	uint DeviceID=-1;
	QString _CL_DEVICE_NAME;
	QString _CL_DEVICE_TYPE;
	ulong _CL_DEVICE_GLOBAL_MEM_SIZE=0;
	QString _CL_DEVICE_VENDOR;
	QString _CL_DEVICE_VERSION;
	QString _CL_DRIVER_VERSION;
	int AMD_BUS_ID=-1; // -1 indicates that it is not set
};

#endif
