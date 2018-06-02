set(fl_3rdParty
	)
set(fl_Algorithms
	Algorithm
	DualAlgorithm
	)
set(fl_Benchmarking
	BenchHelpers/ClaymoreZcashBenchHelper
	BenchHelpers/CpuBenchHelper
	BenchHelpers/PowerHelper

	BenchmarkHandler
	)
set(fl_Configs
	ConfigJsonFile/ConfigFile
	ConfigJsonFile/DeviceBenchmarkConfigFile
	ConfigJsonFile/FOLDERS
	ConfigJsonFile/GeneralConfigFile

	Data/AlgorithmConfig
	Data/BenchmarkTimeLimitsConfig
#	Data/ComputeDeviceConfig
	Data/DeviceBenchmarkConfig
	Data/DeviceDetectionConfig
	Data/DualAlgorithmConfig
	Data/GeneralConfig

	ConfigManager

	ConfigJsonFile/MinerSystemVariablesConfigFile
	Data/MinerSystemVariablesConfig
	)
set(fl_Devices
	Algorithms/DefaultAlgorithms
	Algorithms/GroupAlgorithms

	ComputeDevice/ComputeDevice
	ComputeDevice/CpuComputeDevice

#	Querying/OpenCLJsonData
#	Querying/VideoControllerData

	ComputeDeviceManager
	CPUUtils
	GroupNames
#	OpenCLDevice
	)
set(fl_Forms
	Components/AlgorithmSettingsControl
	Components/AlgorithmsListView
	Components/BenchmarkLimitControl
	Components/BenchmarkOptions
	Components/DevicesListViewEnableControl
	Components/Field
	Components/GroupProfitControl

	DriverVersionConfirmationDialog
	Form_3rdParty_TOS
	Form_Benchmark
	Form_DcriValues
	Form_ChooseLanguage
	Form_Loading
	Form_Main
	Form_Settings
#	MinerFileNotFoundDialog

	Components/AlgorithmsListViewModel
	Components/DevicesListViewEnableControlModel
	Components/IntensitiesModel
	)
set(fl_Miners
	Equihash/NhEqBase
	Equihash/NhEqMiner

	ethminer/Ethereum
	ethminer/MinerEtherum

	Grouping/GroupingLogic
	Grouping/GroupMiner
	Grouping/GroupSetupUtils
	Grouping/MinerPaths
	Grouping/MiningDevice
	Grouping/MiningPair
	Grouping/MiningSetup

	Parsing/ExtraLaunchParameters
	Parsing/ExtraLaunchParametersParser
	Parsing/MinerOption
	Parsing/MinerOptionPackage

	XmrStak/Configs/XmrStakConfig
	XmrStak/Configs/XmrStakConfigCpu
	XmrStak/Configs/XmrStakConfigGpu
	XmrStak/Configs/XmrStakConfigPool
	XmrStak/XmrStak

	ClaymoreBaseMiner
	ClaymoreDual
	CpuMiner
	Miner
	MinerFactory
	MinersApiPortsManager
	MinersManager
	MinersSettingsManager
	MiningSession
	Prospector
	Xmrig
	)
set(fl_PInvoke
	CPUID
	NiceHashProcess
	PInvokeHelpers
	)
set(fl_Stats
	ExchangeRateAPI
	NiceHashSocket
	NiceHashStats
	)
set(fl_Switching
	AlgorithmHistory
	AlgorithmSwitchingManager
	Interval
#	Models
	NHSmaData
	)
set(fl_Utils
	AlgorithmNiceHashNames
#	BINS_CODEGEN
#	BINS_CODEGEN_3rd
	BitcoinAddress
	CommandLineParser
	DownloadSetup
	Extensions
	Helpers
	Links
	Logger
#	MemoryHelper
	MinersDownloader
#	MinersDownloadManager
	MinersExistanceChecker
	ProfitabilityCalculator
	)

if (WITH_AMD)
	list(APPEND fl_Devices
		ComputeDevice/AmdComputeDevice
		Querying/AmdQuery
		AmdGpuDevice
		)
	list(APPEND fl_Miners
		Equihash/ClaymoreZcashMiner
		Equihash/OptiminerZcashMiner
		ethminer/MinerEtherumOCL
		ClaymoreCryptoNightMiner
		Sgminer
		)
	if (ADL_FOUND)
		list(APPEND fl_3rdParty
			ADL
			)
	endif ()
endif ()
if (WITH_NVIDIA)
	list(APPEND fl_3rdParty
#		NVAPI
		NVML
		)
	list(APPEND fl_Devices
		ComputeDevice/CudaComputeDevice
		CUDA_Unsupported
		CudaDevice
		)
	list(APPEND fl_Miners
		Equihash/Dtsm
		ethminer/MinerEtherumCUDA
		Excavator
		Ccminer
		Ewbf
		)
endif ()

foreach (d 3rdParty Algorithms Benchmarking Configs Devices Forms Miners PInvoke Stats Switching Utils)
	foreach (f ${fl_${d}})
		list(APPEND NHQTM_${d}_SRCS
			${d}/${f}.cpp
			)
	endforeach ()
endforeach ()


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(
	BEFORE
	${Boost_INCLUDE_DIR}
	${QtSingleApplication_SRC}
	${managedCuda_SRC}
	)
if (WITH_AMD AND ADL_FOUND)
	include_directories(
		BEFORE
		${ADL_INCLUDE_DIR}
		)
endif ()
if (WITH_NVIDIA)
	include_directories(
		BEFORE
		${NVML_INCLUDE_DIR}
#		${NVCtrlLib_INCLUDE_DIR}
		)
endif ()

add_executable(${PROJECT_NAME}
	${NHQTM_3rdParty_SRCS}
	${NHQTM_Algorithms_SRCS}
	${NHQTM_Benchmarking_SRCS}
	${NHQTM_Configs_SRCS}
	${NHQTM_Devices_SRCS}
	${NHQTM_Forms_SRCS}
	${NHQTM_Miners_SRCS}
	${NHQTM_PInvoke_SRCS}
	${NHQTM_Stats_SRCS}
	${NHQTM_Switching_SRCS}
	${NHQTM_Utils_SRCS}

	main.cpp
	Enums.cpp
	Globals.cpp
	IFTTT.cpp
	International.cpp


	Properties/Resources.cpp
	Qt/ClickableLabel.cpp
	Qt/NHMCashRegisterBitcoin.cpp
	Qt/LException.cpp
	WinPort/Process.cpp
#	${UI_HEADERS}
	${UI_RESOURCES}
	../resources.qrc
	)


target_link_libraries(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::Network
	Qt5::WebSockets
	Qt5::Sql
	${Boost_LIBRARIES}
	${HW_LIBRARIES}
	${QtSingleApplication_LIB}
	${managedCuda_LIB}
	${DL_LIBRARY}
	${LOG4_LIBRARY}
	${OPENCL_LIBRARIES}
	)
if (WITH_AMD AND ADL_FOUND)
	target_link_libraries(${PROJECT_NAME}
		${ADL_LIBRARY}
		)
endif ()
if (WITH_NVIDIA)
	target_link_libraries(${PROJECT_NAME}
		${NVML_LIBRARY}
		)
endif ()
if (LIBXSS_FOUND)
	target_link_libraries(${PROJECT_NAME}
		Xss
		X11
		)
endif ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/langs
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)

#install(PROGRAMS ${ROOT_DIR}/OnGPUsLost.sh
#	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
#	)
file(COPY ${ROOT_DIR}/OnGPUsLost.sh
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
	)
install(CODE "message(\"execute 'chown root:root ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OnGPUsLost.sh' manualy.\")")
