#include "Devices/GroupNames.h"
#include "config.h"


QStringList GroupNames::Names={
	"CPU",
	"AMD_OpenCL",
	"NVIDIA2.1",
	"NVIDIA3.x",
	"NVIDIA5.x",
	"NVIDIA6.x"
	};

QString GroupNames::GetGroupName(Enums::DeviceGroupType type, int id)
{
	if (type==Enums::DeviceGroupType::CPU) {
		return "CPU"+QString::number(id);
		}
	if ((int)type<Names.count() && (int)type>=0) {
		return Names[(int)type];
		}
	return "UnknownGroup";
}

QString GroupNames::GetNameGeneral(Enums::DeviceType type)
{
	switch (type) {
		case Enums::DeviceType::CPU:
			return "CPU";
#if WITH_NVIDIA
		case Enums::DeviceType::NVIDIA:
			return "NVIDIA";
#endif
#if WITH_AMD
		case Enums::DeviceType::AMD:
			return "AMD";
#endif
		}
	return "UnknownDeviceType";
}
