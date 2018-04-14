#ifndef ADL_H
#define ADL_H

#ifndef LINUX
#	define LINUX
#endif

#include <ADL/adl_sdk.h>
#include "3rdParty/ADL_defs.h"
#include <QString>
#include <QVector>


void* __stdcall ADL_Main_Memory_Alloc(int iSize);


namespace ATI {
	namespace ADL {


class ADL
{
public:
	static const int ADL_SUCCESS=0;

	static bool Init();

	static bool ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK, int);
	static bool ADL_Main_Control_Destroy();
	static bool ADL_Adapter_NumberOfAdapters_Get(int* numAdapters);
	static bool ADL_Adapter_AdapterInfo_Get(LPAdapterInfo info, int inputSize);
	static bool ADL_Adapter_Active_Get(int adapterIndex, int* status);

	static bool ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity* activity);
	static bool ADL_Overdrive5_Temperature_Get(int adapterIndex, int thermalControllerIndex, ADLTemperature* temperature);
	static bool ADL_Overdrive5_FanSpeed_Get(int adapterIndex, int thermalControllerIndex, ADLFanSpeedValue* temperature);

private:
	static void* adlLib;
	static void* getProcAddress(void* lib, const char* name);
	static bool GetAdlErr(int error);

	static ADL_MAIN_CONTROL_CREATE _ADL_Main_Control_Create;
	static ADL_MAIN_CONTROL_DESTROY _ADL_Main_Control_Destroy;
	static ADL_ADAPTER_NUMBEROFADAPTERS_GET _ADL_Adapter_NumberOfAdapters_Get;
	static ADL_ADAPTER_ADAPTERINFO_GET _ADL_Adapter_AdapterInfo_Get;
	static ADL_ADAPTER_ACTIVE_GET _ADL_Adapter_Active_Get;

	static ADL_OVERDRIVE5_CURRENTACTIVITY_GET _ADL_Overdrive5_CurrentActivity_Get;
	static ADL_OVERDRIVE5_TEMPERATURE_GET _ADL_Overdrive5_Temperature_Get;
	static ADL_OVERDRIVE5_FANSPEED_GET _ADL_Overdrive5_FanSpeed_Get;
};

/*
struct ADLAdapterInfo {
	int Size;
	int AdapterIndex;
	QString UDID;
	int BusNumber;
	int DriverNumber;
	int functionNumber;
	int VendorID;
	QString AdapterName;
	QString DisplayName;
	int Present;
	int Exists;
	QString DriverPath;
	QString DriverPathExt;
	QString PNPString;
	int OSDisplayIndex;
	};

struct ADLAdapterInfoArray {
	QVector<ADLAdapterInfo> ADLAdapterInfo_;
	};
*/
/*
typedef int ( *ADL_MAIN_CONTROL_CREATE ) (ADL_MAIN_MALLOC_CALLBACK, int);
typedef int ( *ADL_MAIN_CONTROL_DESTROY )();
typedef int ( *ADL_ADAPTER_NUMBEROFADAPTERS_GET ) (int*);
typedef int ( *ADL_ADAPTER_ADAPTERINFO_GET ) (LPAdapterInfo, int);
typedef int ( *ADL_ADAPTER_ACTIVE_GET ) (int, int*);

ADL_MAIN_CONTROL_CREATE ADL_Main_Control_Create;
ADL_MAIN_CONTROL_DESTROY ADL_Main_Control_Destroy;
ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get;
ADL_ADAPTER_ADAPTERINFO_GET ADL_Adapter_AdapterInfo_Get;
ADL_ADAPTER_ACTIVE_GET ADL_Adapter_Active_Get;

void* __stdcall ADL_Main_Memory_Alloc(int iSize);
*/
		}
	}
#endif
