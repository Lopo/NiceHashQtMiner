#include "Devices/Querying/AmdQuery.h"
#include "Utils/Helpers.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Forms/DriverVersionConfirmationDialog.h"
#include "Devices/ComputeDeviceManager.h"
#include "Devices/AmdGpuDevice.h"
#include "Devices/ComputeDevice/AmdComputeDevice.h"
#if ADL_LIBWORKS
#	include "3rdParty/ADL.h"
#endif
#include <QVersionNumber>

#if ADL_LIBWORKS
using namespace ATI::ADL;
#endif


AmdQuery::AmdQuery(QList<VideoControllerData*>* availControllers)
{
	_availableControllers=availControllers;
}

QList<OpenCLDevice*>* AmdQuery::QueryAmd(bool openCLSuccess, QList<OpenCLJsonData>* openCLData)
{
	Helpers::ConsolePrint(Tag, "QueryAMD START");

	DriverCheck();

	QList<OpenCLDevice*>* amdDevices= openCLSuccess? ProcessDevices(openCLData) : new QList<OpenCLDevice*>;

	Helpers::ConsolePrint(Tag, "QueryAMD END");

	return amdDevices;
}

void AmdQuery::DriverCheck()
{
	// check the driver version bool EnableOptimizedVersion = true;
	bool showWarningDialog=false;

	foreach (VideoControllerData* vidContrllr, *_availableControllers) {
		Helpers::ConsolePrint(Tag, QString("Checking AMD device (driver): %1 (%2)").arg(vidContrllr->Name).arg(vidContrllr->DriverVersion));

		_driverOld.insert(vidContrllr->Name, false);
		_noNeoscryptLyra2.insert(vidContrllr->Name, false);
		QVersionNumber sgminerNoNeoscryptLyra2RE=QVersionNumber::fromString("21.19.164.1");

		// TODO checking radeon drivers only?
		if ((!vidContrllr->Name.contains("AMD") && !vidContrllr->Name.contains("Radeon")) || showWarningDialog) {
			continue;
			}

		QVersionNumber amdDriverVersion=QVersionNumber::fromString(vidContrllr->DriverVersion);

		if (!ConfigManager.generalConfig->ForceSkipAMDNeoscryptLyraCheck) {
			if (QVersionNumber::compare(amdDriverVersion, sgminerNoNeoscryptLyra2RE)>=0) {
				_noNeoscryptLyra2.insert(vidContrllr->Name, true);
				Helpers::ConsolePrint(Tag, "Driver version seems to be "+sgminerNoNeoscryptLyra2RE.toString()+" or higher. NeoScrypt and Lyra2REv2 will be removed from list");
				}
			}

		if (amdDriverVersion.majorVersion()>15) {
			continue;
			}

		showWarningDialog=true;
		_driverOld.insert(vidContrllr->Name, true);
		Helpers::ConsolePrint(Tag, "WARNING!!! Old AMD GPU driver detected! All optimized versions disabled, mining speed will not be optimal. Consider upgrading AMD GPU driver. Recommended AMD GPU driver version is 15.7.1.");
		}

	if (ConfigManager.generalConfig->ShowDriverVersionWarning && showWarningDialog) {
		auto warningDialog=new DriverVersionConfirmationDialog;
		warningDialog->exec();
		delete warningDialog;
		}
}

QList<OpenCLDevice*>* AmdQuery::ProcessDevices(QList<OpenCLJsonData>* openCLData)
{
	QList<OpenCLDevice*>* amdOclDevices=new QList<OpenCLDevice*>;
	QList<OpenCLDevice*>* amdDevices=new QList<OpenCLDevice*>;

	bool amdPlaformNumFound=false;
	foreach (OpenCLJsonData oclEl, *openCLData) {
		if (!oclEl.PlatformName.contains("AMD") && !oclEl.PlatformName.contains("amd")) {
			continue;
			}
		amdPlaformNumFound=true;
		QString amdOpenClPlatformStringKey=oclEl.PlatformName;
		ComputeDeviceManager.Available.AmdOpenCLPlatformNum=oclEl.PlatformNum;
		amdOclDevices=oclEl.Devices;
		Helpers::ConsolePrint(Tag, QString("AMD platform found Key: %1, Num: %2").arg(amdOpenClPlatformStringKey).arg(ComputeDeviceManager.Available.AmdOpenCLPlatformNum));
		break;
		}

	if (!amdPlaformNumFound) {
		return amdDevices;
		}

	// get only AMD gpus
	{
		foreach (OpenCLDevice* oclDev, *amdOclDevices) {
			if (oclDev->_CL_DEVICE_TYPE.contains("GPU")) {
				amdDevices->append(oclDev);
				}
			}
	}
	
	if (!amdDevices->count()) {
		Helpers::ConsolePrint(Tag, "AMD GPUs count is 0");
		return amdDevices;
		}

	Helpers::ConsolePrint(Tag, QString("AMD GPUs count : %1").arg(amdDevices->count()));
	Helpers::ConsolePrint(Tag, "AMD Getting device name and serial from ADL");
	// ADL
#if ADL_LIBWORKS
	bool isAdlInit=true;
	try {
		isAdlInit=QueryAdl();
		}
	catch (QException& ex) {
		Helpers::ConsolePrint(Tag, QString("AMD ADL exception: ")+ex.what());
		isAdlInit=false;
		}
#else
	bool isAdlInit=false;
#endif

	bool isBusIDOk=true;
	// check if buss ids are unique and different from -1
	{
		QSet<int> busIDs;
		// Override AMD bus IDs
		QStringList overrides=ConfigManager.generalConfig->OverrideAMDBusIds.split(',');
		for (int i=0; i<amdDevices->count(); i++) {
			OpenCLDevice* amdOclDev=amdDevices->value(i);
			if (overrides.count()>i) {
				bool ok=false;
				int overrideBus=overrides[i].toInt(&ok);
				if (ok && overrideBus>=0) {
					amdOclDev->AMD_BUS_ID=overrideBus;
					}
				}

			if (amdOclDev->AMD_BUS_ID<0 || !_busIdInfos.contains(amdOclDev->AMD_BUS_ID)) {
				isBusIDOk=false;
				break;
				}

			busIDs.insert(amdOclDev->AMD_BUS_ID);
			}

		// check if unique
		isBusIDOk=isBusIDOk && busIDs.count()==amdDevices->count();
	}
	// print BUS id status
	Helpers::ConsolePrint(Tag, isBusIDOk? "AMD Bus IDs are unique and valid. OK" : "AMD Bus IDs IS INVALID. Using fallback AMD detection mode");
	// AMD device creation (in NHM context)
	if (isAdlInit && isBusIDOk) {
		return AmdDeviceCreationPrimary(amdDevices);
		}
	return AmdDeviceCreationFallback(amdDevices);
}

QList<OpenCLDevice*>* AmdQuery::AmdDeviceCreationPrimary(QList<OpenCLDevice*>* amdDevices)
{
	Helpers::ConsolePrint(Tag, "Using AMD device creation DEFAULT Reliable mappings");
	Helpers::ConsolePrint(Tag, amdDevices->count()==_amdDeviceUuid.count()? "AMD OpenCL and ADL AMD query COUNTS GOOD/SAME" : "AMD OpenCL and ADL AMD query COUNTS DIFFERENT/BAD");
	QStringList stringBuilder={"", "QueryAMD [DEFAULT query] devices: "};
	foreach (OpenCLDevice* dev, *amdDevices) {
		ComputeDeviceManager.Available.HasAmd=true;

		int busID=dev->AMD_BUS_ID;
		if (busID!=-1 && _busIdInfos.contains(busID)) {
			QString deviceName=_busIdInfos.value(busID).Name;
			AmdGpuDevice* newAmdDev=new AmdGpuDevice(dev, _driverOld.value(deviceName), _busIdInfos.value(busID).InfSection, _noNeoscryptLyra2.value(deviceName));
			newAmdDev->DeviceName=deviceName;
			newAmdDev->UUID=_busIdInfos.value(busID).Uuid;
			newAmdDev->AdapterIndex=_busIdInfos.value(busID).Adl1Index;
			bool isDisabledGroup=ConfigManager.generalConfig->DeviceDetection->DisableDetectionAMD;

			ComputeDeviceManager.Available.Devices->append(new AmdComputeDevice(newAmdDev, ++ComputeDeviceManager.Query.GpuCount, false, _busIdInfos.value(busID).Adl2Index));
			// just in case
			try {
				stringBuilder << QString("\t%1 device%2:").arg(isDisabledGroup? "SKIPED" : "ADDED").arg(isDisabledGroup? " (AMD group disabled)" : "");
				stringBuilder << QString("\t\tID: %1").arg(newAmdDev->DeviceID());
				stringBuilder << QString("\t\tNAME: %1").arg(newAmdDev->DeviceName);
				stringBuilder << QString("\t\tCODE_NAME: %1").arg(newAmdDev->Codename());
				stringBuilder << QString("\t\tUUID: %1").arg(newAmdDev->UUID);
				stringBuilder << QString("\t\tMEMORY: %1").arg(newAmdDev->DeviceGlobalMemory());
				stringBuilder << QString("\t\tETHEREUM: %1").arg(newAmdDev->IsEtherumCapable()? "YES" : "NO");
				}
			catch (...) {}
			}
		else {
			stringBuilder << QString("\tDevice not added, Bus No. %1 not found:").arg(busID);
			}
		}

	Helpers::ConsolePrint(Tag, stringBuilder.join('\n'));

	return amdDevices;
}

QList<OpenCLDevice*>* AmdQuery::AmdDeviceCreationFallback(QList<OpenCLDevice*>* amdDevices)
{
	Helpers::ConsolePrint(Tag, "Using AMD device creation FALLBACK UnReliable mappings");
	QStringList stringBuilder={"", "QueryAMD [FALLBACK query] devices: "};

	// get video AMD controllers and sort them by RAM
	// (find a way to get PCI BUS Numbers from PNPDeviceID)
	QList<VideoControllerData*> amdVideoControllers;
	foreach (VideoControllerData* vcd, *_availableControllers) {
		if (vcd->Name.toLower().contains("amd") || vcd->Name.toLower().contains("radeon") || vcd->Name.toLower().contains("firepro")) {
			amdVideoControllers.append(vcd);
			}
		}
	// sort by ram not ideal
	std::sort(amdVideoControllers.begin(), amdVideoControllers.end(), [](const VideoControllerData* a, const VideoControllerData* b){return a->AdapterRAM>b->AdapterRAM;});
	std::sort(amdDevices->begin(), amdDevices->end(), [](const OpenCLDevice* a, const OpenCLDevice* b) { return a->_CL_DEVICE_GLOBAL_MEM_SIZE<b->_CL_DEVICE_GLOBAL_MEM_SIZE; });
	int minCount=std::min(amdVideoControllers.count(), amdDevices->count());

	for (int i=0; i<minCount; ++i) {
		ComputeDeviceManager.Available.HasAmd=true;

		VideoControllerData* avc=amdVideoControllers.value(i);
		QString deviceName=avc->Name;
		AmdGpuDevice* newAmdDev=new AmdGpuDevice(amdDevices->value(i), _driverOld.value(deviceName), avc->InfSection, _noNeoscryptLyra2.value(deviceName));
		newAmdDev->DeviceName=deviceName;
		newAmdDev->UUID="UNUSED";
		bool isDisabledGroup=ConfigManager.generalConfig->DeviceDetection->DisableDetectionAMD;

		ComputeDeviceManager.Available.Devices->append(new AmdComputeDevice(newAmdDev, ++ComputeDeviceManager.Query.GpuCount, true, -1, avc->vendor, avc->subvendor));
		// just in case
		try {
			stringBuilder << QString("\t%1 device%2:").arg(isDisabledGroup? "SKIPED" : "ADDED");
			stringBuilder << QString("\t\tID: %1").arg(newAmdDev->DeviceID());
			stringBuilder << QString("\t\tNAME: %1").arg(newAmdDev->DeviceName);
			stringBuilder << QString("\t\tCODE_NAME: %1").arg(newAmdDev->Codename());
			stringBuilder << QString("\t\tUUID: %1").arg(newAmdDev->UUID);
			stringBuilder << QString("\t\tMEMORY: %1").arg(newAmdDev->DeviceGlobalMemory());
			stringBuilder << QString("\t\tETHEREUM: %1").arg(newAmdDev->IsEtherumCapable()? "YES" : "NO");
			}
		catch (...) {}
		}

	Helpers::ConsolePrint(Tag, stringBuilder.join('\n'));

	return amdDevices;
}
#if ADL_LIBWORKS
bool AmdQuery::QueryAdl()
{
	// ADL does not get devices in order map devices by bus number
	// bus id, <name, uuid>
	bool isAdlInit=true;

	int adlRet=-1;
	int numberOfAdapters=0;
	ADL_CONTEXT_HANDLE adl2Control=nullptr;

	adlRet=ADL::ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1); // Second parameter is 1: Get only the present adapters
	if (adlRet==ADL::ADL_SUCCESS) {
		ADL::ADL_Adapter_NumberOfAdapters_Get(&numberOfAdapters);
		Helpers::ConsolePrint(Tag, "Number Of Adapters: "+QString::number(numberOfAdapters));

		if (numberOfAdapters>0) {
			// Get OS adpater info from ADL
//			ADLAdapterInfoArray* osAdapterInfoData=new ADLAdapterInfoArray;

			int size=sizeof (AdapterInfo)*numberOfAdapters;
			LPAdapterInfo adapterBuffer=(LPAdapterInfo)malloc(size);
			memset(adapterBuffer, '\0', size);

			adlRet=ADL::ADL_Adapter_AdapterInfo_Get(adapterBuffer, size);

			int adl2Ret=ADL::ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 0, &adl2Control);
			AdapterInfo adl2Info[ADL_MAX_ADAPTERS];
			if (adl2Ret==ADL::ADL_SUCCESS) {
				adl2Ret=ADL::ADL2_Adapter_AdapterInfo_Get(adl2Control, adl2Info, sizeof(adl2Info));
				}
			else {
				adl2Ret=-1;
				}

			if (adlRet==ADL::ADL_SUCCESS) {
				int isActive=0;

				for (int i=0; i<numberOfAdapters; i++) {
					// Check if the adapter is active
					AdapterInfo adapterInfo=adapterBuffer[i];
					adlRet=ADL::ADL_Adapter_Active_Get(adapterInfo.iAdapterIndex, &isActive);

					if (adlRet!=ADL::ADL_SUCCESS) {
						continue;
						}

					// we are looking for amd
					// TODO check discrete and integrated GPU separation
					int vendorID=adapterInfo.iVendorID;
					QString devName=adapterInfo.strAdapterName;

					if (vendorID!=AmdVendorID
						&& !devName.toLower().contains("amd")
						&& !devName.toLower().contains("radeon")
						&& !devName.toLower().contains("firepro")
						) {
						continue;
						}

					QString pnpStr=""; // adapterInfo.strPNPString;
					// find vi controller pnp
					QString infSection="";
					foreach (VideoControllerData* vCtrl, *_availableControllers) {
						if (vCtrl->PnpDeviceID==pnpStr) {
							infSection=vCtrl->InfSection;
							}
						}
					int backSlashLast=pnpStr.lastIndexOf('\\');
					QString serial=pnpStr.mid(backSlashLast, pnpStr.length()-backSlashLast);
					int end0=serial.indexOf('&');
					int end1=serial.indexOf('&', end0+1);
					// get serial
					serial=serial.mid(end0+1, (end1-end0)-1);

					QString udid=adapterInfo.strUDID;
					int pciVenIDStrSize=21; // PCI_VEN_XXXX&DEV_XXXX
					QString uuid=udid.left(pciVenIDStrSize)+"_"+serial;
					int budId=adapterInfo.iBusNumber;
					int index=adapterInfo.iAdapterIndex;
					if (_amdDeviceUuid.contains(uuid)) {
						continue;
						}

//					try {
						Helpers::ConsolePrint(Tag, "ADL device added BusNumber:"+QString::number(budId)+"  NAME:"+devName+"  UUID"+uuid);
//						}
//					catch (...) {
//						}

					_amdDeviceUuid.append(uuid);

					if (_busIdInfos.contains(budId)) {
						continue;
						}

					int adl2Index=-1;
					if (adl2Ret==ADL::ADL_SUCCESS) {
						for (int i=0; i<ADL_MAX_ADAPTERS; i++) {
							if (adl2Info[i].strUDID==adapterBuffer[i].strUDID) {
								adl2Index=adl2Info[i].iAdapterIndex;
								break;
								}
							}
						}
					_busIdInfos.insert(budId, {devName, uuid, infSection, index, adl2Index});
					}
				}
			else {
				Helpers::ConsolePrint(Tag, "ADL_AdapterInfo_Get() returned error code "+QString::number(adlRet));
				isAdlInit=false;
				}
			// Release the memory for the AdapterInfo structure
			if (adapterBuffer!=nullptr) {
				free(adapterBuffer);
				}
			}
		if (numberOfAdapters<=0) {
			ADL::ADL_Main_Control_Destroy(); // Close ADL if it found no AMD devices
			if (adl2Control!=nullptr) {
				ADL::ADL2_Main_Control_Destroy(adl2Control);
				}
			}
		}
	else {
		// TODO
		Helpers::ConsolePrint(Tag, "ADL_Main_Control_Create() returned error code "+QString::number(adlRet));
		Helpers::ConsolePrint(Tag, "Check if ADL is properly installed!");
		isAdlInit=false;
		}

	return isAdlInit;
}
#endif