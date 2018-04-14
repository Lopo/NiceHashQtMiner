#include "Devices/AmdGpuDevice.h"
#include "Utils/Helpers.h"
#include "Devices/ComputeDevice/ComputeDevice.h"


const QStringList AmdGpuDevice::DefaultParam={"--keccak-unroll", "0", "--hamsi-expand-big", "4", "--remove-disabled"};
const QStringList AmdGpuDevice::TemperatureParam={"--gpu-fan", "30-95", "--temp-cutoff", "95", "--temp-overhat", "90", "--temp-target", "75", "--auto-fan", "--auto-gpu"};


AmdGpuDevice::AmdGpuDevice(OpenCLDevice* openClSubset, bool isOldDriver, QString infSection, bool driverDisableAlgo)
{
	DriverDisableAlgos=driverDisableAlgo;
	InfSection=infSection;
	if (openClSubset!=nullptr) {
//		delete _openClSubset;
		_openClSubset=openClSubset;
		}
	Helpers::ConsolePrint("AmdGpuDevice", "List: "+_openClSubset->_CL_DEVICE_NAME);
}

bool AmdGpuDevice::IsEtherumCapable()
{
	return _openClSubset->_CL_DEVICE_GLOBAL_MEM_SIZE>=ComputeDevice::Memory3Gb;
}
