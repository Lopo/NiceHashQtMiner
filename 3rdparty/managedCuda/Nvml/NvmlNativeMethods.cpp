#include "NvmlNativeMethods.h"


using namespace ManagedCuda::Nvml;

nvmlReturn NvmlNativeMethods::nvmlInit()
{
	return nvmlReturn::Unknown;
}

nvmlReturn NvmlNativeMethods::nvmlDeviceGetHandleByUUID(const std::string& uuid, nvmlDevice* device)
{
	return nvmlReturn::Unknown;
}
