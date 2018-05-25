#ifndef NVMLNATIVEMETHODS_H
#define NVMLNATIVEMETHODS_H

#include "NvmlTypes.h"
#include <string>


namespace ManagedCuda {
	namespace Nvml {

class NvmlNativeMethods
{
public:
	static nvmlReturn nvmlInit(); // 61
	static nvmlReturn nvmlDeviceGetHandleByUUID(const std::string& uuid, nvmlDevice* device);

private:
};

		}
	}

#endif
