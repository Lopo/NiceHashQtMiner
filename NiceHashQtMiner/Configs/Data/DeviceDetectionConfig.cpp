#include "Configs/Data/DeviceDetectionConfig.h"


DeviceDetectionConfig::DeviceDetectionConfig()
{
#if WITH_AMD
	DisableDetectionAMD=false;
#endif
#if WITH_NVIDIA
	DisableDetectionNVIDIA=false;
#endif
}
