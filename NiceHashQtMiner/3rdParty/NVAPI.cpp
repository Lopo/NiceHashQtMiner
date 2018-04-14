#include "3rdParty/NVAPI.h"

using namespace NVIDIA::NVAPI;


bool NVAPI::available=false;

NVAPI::NVAPI()
{
	adlLib=dlopen("nvapi64.so", RTLD_LAZY|RTLD_GLOBAL);
}
