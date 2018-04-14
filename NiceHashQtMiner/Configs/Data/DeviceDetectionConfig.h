#ifndef CONFIGS_DATA_DEVICEDETECTIONCONFIG_H_
#define CONFIGS_DATA_DEVICEDETECTIONCONFIG_H_

#include "config.h"


class DeviceDetectionConfig
{
public:
	DeviceDetectionConfig();

#if WITH_AMD
	bool DisableDetectionAMD;
#endif
#if WITH_NVIDIA
	bool DisableDetectionNVIDIA;
#endif
};

#endif
