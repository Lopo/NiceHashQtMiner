#ifndef RDPARTY_ADL_H_
#define RDPARTY_ADL_H_

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
	static const int ADL_FAIL=-1;
	static const int ADL_NOT_SUPPORTED=-8;

	static bool Init();

	static bool ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK, int);
	static bool ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK, int, ADL_CONTEXT_HANDLE);
	static bool ADL_Main_Control_Destroy();
	static bool ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE);
	static bool ADL_Adapter_NumberOfAdapters_Get(int* numAdapters);
	static bool ADL_Adapter_AdapterInfo_Get(LPAdapterInfo info, int inputSize);
	static bool ADL2_Adapter_AdapterInfo_Get(ADL_CONTEXT_HANDLE context, LPAdapterInfo lpInfo, int iInputSize);
	static bool ADL_Adapter_Active_Get(int adapterIndex, int* status);

	static bool ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity* activity);
	static bool ADL_Overdrive5_Temperature_Get(int adapterIndex, int thermalControllerIndex, ADLTemperature* temperature);
	static bool ADL_Overdrive5_FanSpeed_Get(int adapterIndex, int thermalControllerIndex, ADLFanSpeedValue* temperature);

	static bool ADL2_Overdrive6_CurrentPower_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iPowerType, int* lpCurrentValue);

private:
	static void* adlLib;
	static void* getProcAddress(void* lib, const char* name);
	static bool GetAdlErr(int error);

	static ADL_MAIN_CONTROL_CREATE _ADL_Main_Control_Create;
	static ADL2_MAIN_CONTROL_CREATE _ADL2_Main_Control_Create;
	static ADL_MAIN_CONTROL_DESTROY _ADL_Main_Control_Destroy;
	static ADL2_MAIN_CONTROL_DESTROY _ADL2_Main_Control_Destroy;
	static ADL_ADAPTER_NUMBEROFADAPTERS_GET _ADL_Adapter_NumberOfAdapters_Get;
	static ADL_ADAPTER_ADAPTERINFO_GET _ADL_Adapter_AdapterInfo_Get;
	static ADL2_ADAPTER_ADAPTERINFO_GET _ADL2_Adapter_AdapterInfo_Get;
	static ADL_ADAPTER_ACTIVE_GET _ADL_Adapter_Active_Get;

	static ADL_OVERDRIVE5_CURRENTACTIVITY_GET _ADL_Overdrive5_CurrentActivity_Get;
	static ADL_OVERDRIVE5_TEMPERATURE_GET _ADL_Overdrive5_Temperature_Get;
	static ADL_OVERDRIVE5_FANSPEED_GET _ADL_Overdrive5_FanSpeed_Get;

	static ADL2_OVERDRIVE6_CURRENTPOWER_GET _ADL2_Overdrive6_CurrentPower_Get;
};

		}
	}
#endif
