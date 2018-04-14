#define __NVML_cpp__
#include "3rdParty/NVML.h"
#include "Utils/Helpers.h"

//using namespace NVIDIA::NVML;

bool NVML::available=false;
bool NVML::init=false;


void NVML::Init()
{
	init=true;
	nvmlReturn_t result;
	try {
		result=nvmlInit();
		}
	catch (std::exception e) {
		Helpers::ConsolePrint("NVML", e.what());
		return;
		}

	
	if (result!=NVML_SUCCESS) {
		Helpers::ConsolePrint("NVML", QString("Failed to initialize: %1").arg(nvmlErrorString(result)));
		return;
		}
	available=true;
}

bool NVML::IsAvailable()
{
	if (!init) {
		Init();
		}
	return available;
}

nvmlReturn_t NVML::DeviceGetCount(unsigned int* deviceCount)
{
	return nvmlDeviceGetCount(deviceCount);
}
