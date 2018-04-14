#ifndef NVML_H
#define NVML_H

#include <nvml.h>


//namespace NVIDIA {
//	namespace NVML {

class NVML
{
public:
	static const int MAX_PHYSICAL_GPUS=64;
	static bool IsAvailable();
	static nvmlReturn_t DeviceGetCount(unsigned int* deviceCount);

private:
	static void Init();
	static bool available;
	static bool init;
};

#ifndef __NVML_cpp__
#	define EXTERN extern
#else
#	define EXTERN
#endif
EXTERN NVML NVML;
#undef EXTERN

//	}
//}

#endif // NVML_H
