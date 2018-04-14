#include "Devices/CudaDevice.h"
#include "International.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "config.h"


QString CudaDevice::GetName()
{
	if (VendorName=="UNKNOWN") {
		VendorName=International::GetText("ComputeDevice_UNKNOWN_VENDOR_REPLACE").arg(VendorID);
		}
	return VendorName+" "+DeviceName;
}

bool CudaDevice::IsEtherumCapable()
{
	// exception devices
#if WITH_NVIDIA
	if (DeviceName.contains("750") && DeviceName.contains("Ti")) {
		Helpers::ConsolePrint("CudaDevice", "GTX 750Ti found! By default this device will be disabled for ethereum as it is generally too slow to mine on it.");
		return false;
		}
#endif
	return DeviceGlobalMemory>=ComputeDevice::Memory3Gb && SM_major>=3;
}
