#include "Devices/CPUUtils.h"
#include "PInvoke/CPUID.h"


using namespace NiceHashQtMiner;


Enums::CPUExtensionType CPUUtils::_detectOrder[]={
	Enums::CPUExtensionType::AVX2_AES,
	Enums::CPUExtensionType::AVX2,
	Enums::CPUExtensionType::AVX_AES,
	Enums::CPUExtensionType::AVX,
	Enums::CPUExtensionType::AES,
	Enums::CPUExtensionType::SSE2 // disabled
	};

bool CPUUtils::HasExtensionSupport(Enums::CPUExtensionType type)
{
	switch (type) {
		case Enums::CPUExtensionType::AVX2_AES:
			return (CPUID::SupportsAVX2()==1) && (CPUID::SupportsAES()==1);
		case Enums::CPUExtensionType::AVX2:
			return CPUID::SupportsAVX2()==1;
		case Enums::CPUExtensionType::AVX_AES:
			return (CPUID::SupportsAVX()==1) && (CPUID::SupportsAES()==1);
		case Enums::CPUExtensionType::AVX:
			return CPUID::SupportsAVX()==1;
		case Enums::CPUExtensionType::AES:
			return CPUID::SupportsAES()==1;
		case Enums::CPUExtensionType::SSE2:
			return CPUID::SupportsSSE2()==1;
		default: // Enums::CPUExtensionType::Automatic
			break;
		}
	return false;
}

bool CPUUtils::IsCpuMiningCapable()
{
	return HasExtensionSupport(Enums::CPUExtensionType::AES);
}
