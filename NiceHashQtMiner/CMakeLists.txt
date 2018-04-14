set(forms
	Main
	Settings
	Benchmark
	Loading
	ChooseLanguage
	3rdParty_TOS
	)
set(formComponents
	DevicesListViewEnableControl
	DevicesListViewEnableControlModel
	BenchmarkLimitControl
	BenchmarkOptions
	AlgorithmsListView
	AlgorithmsListViewModel
	AlgorithmSettingsControl
	GroupProfitControl
	)
set(utils
	DownloadSetup
	Links
	BitcoinAddress
	Helpers
	MinersDownloader
	MinersExistanceChecker
	AlgorithmNiceHashNames
	Logger
	ProfitabilityCalculator
	Extensions
	CommandLineParser
	)

list(APPEND NHQTM_devices_SRCS
	Devices/ComputeDeviceManager.cpp
	Devices/GroupAlgorithms.cpp
	Devices/GroupNames.cpp
	Devices/CPUUtils.cpp

	Devices/ComputeDevice/ComputeDevice.cpp #done
	Devices/ComputeDevice/CPUComputeDevice.cpp
	)
list(APPEND NHQTM_3rdParty_SRCS
	)
if (WITH_AMD)
	list(APPEND NHQTM_devices_SRCS
		Devices/AmdGpuDevice.cpp
		Devices/ComputeDevice/AmdComputeDevice.cpp #done
		)
	list(APPEND NHQTM_3rdParty_SRCS
		3rdParty/ADL.cpp
		)
endif ()
if (WITH_NVIDIA)
	list(APPEND NHQTM_devices_SRCS
		Devices/CudaDevice.cpp #done
		Devices/CUDA_Unsupported.cpp #done
		Devices/ComputeDevice/CudaComputeDevice.cpp
		)
	list(APPEND NHQTM_3rdParty_SRCS
#		3rdParty/NVAPI.cpp
		3rdParty/NVML.cpp
		)
endif ()
list(APPEND NHQTM_miners_SRCS
	Miners/Miner.cpp
	Miners/MinerFactory.cpp
	Miners/MiningSession.cpp
	Miners/MinersManager.cpp
	Miners/MinersSettingsManager.cpp #done
	Miners/MinersApiPortsManager.cpp #done

	Miners/Ccminer.cpp
	Miners/ClaymoreBaseMiner.cpp
	Miners/ClaymoreDual.cpp
	Miners/ClaymoreCryptoNightMiner.cpp
	Miners/Sgminer.cpp
	Miners/Ewbf.cpp
	Miners/Excavator.cpp
	Miners/Xmrig.cpp
	Miners/Prospector.cpp

	Miners/ethminer/Ethereum.cpp
	Miners/ethminer/MinerEtherum.cpp
	Miners/ethminer/MinerEtherumOCL.cpp
	Miners/ethminer/MinerEtherumCUDA.cpp

	Miners/Equihash/NhEqBase.cpp
	Miners/Equihash/NhEqMiner.cpp
	Miners/Equihash/OptiminerZcashMiner.cpp
	Miners/Equihash/ClaymoreZcashMiner.cpp
	Miners/Equihash/Dtsm.cpp

	Miners/Grouping/MinerPaths.cpp #done
	Miners/Grouping/MiningPair.cpp #done
	Miners/Grouping/MiningSetup.cpp #done
	Miners/Grouping/MiningDevice.cpp
	Miners/Grouping/GroupSetupUtils.cpp #done
	Miners/Grouping/GroupMiner.cpp #done
	Miners/Grouping/GroupingLogic.cpp

	Miners/Parsing/MinerOption.cpp
	Miners/Parsing/MinerOptionPackage.cpp
	Miners/Parsing/ExtraLaunchParameters.cpp
	Miners/Parsing/ExtraLaunchParametersParser.cpp

	Miners/XmrStak/XmrStakConfig.cpp
	Miners/XmrStak/XmrStak.cpp
	Miners/XmrStak/XmrStackCPUMiner.cpp
	Miners/XmrStak/XmrStakAMD.cpp
	)
list(APPEND NHQTM_configs_SRCS
	Configs/ConfigManager.cpp

	Configs/Data/GeneralConfig.cpp
	Configs/Data/AlgorithmConfig.cpp
	Configs/Data/DeviceDetectionConfig.cpp #done
	Configs/Data/BenchmarkTimeLimitsConfig.cpp
	Configs/Data/DeviceBenchmarkConfig.cpp
	Configs/Data/MinerSystemVariablesConfig.cpp

	Configs/ConfigJsonFile/ConfigFile.cpp;
	Configs/ConfigJsonFile/GeneralConfigFile.cpp;
	Configs/ConfigJsonFile/DeviceBenchmarkConfigFile.cpp;
	Configs/ConfigJsonFile/MinerSystemVariablesConfigFile.cpp;
#	Configs/ConfigJsonFile/FOLDERS.cpp;
	)
list(APPEND NHQTM_pinvoke_SRCS
	PInvoke/CPUID.cpp
	PInvoke/NiceHashProcess.cpp
	PInvoke/PInvokeHelpers.cpp
	)
list(APPEND NHQTM_WinPort_SRCS
	WinPort/Process.cpp
	)
list(APPEND NHQTM_Properties_SRCS
	Properties/Resources.cpp
	)
list(APPEND NHQTM_Switching_SRCS
	Switching/NHSmaData.cpp
	Switching/Interval.cpp
	Switching/AlgorithmSwitchingManager.cpp
	Switching/AlgorithmHistory.cpp
	)
list(APPEND NHQTM_Stats_SRCS
	Stats/NiceHashSocket.cpp
	Stats/NiceHashStats.cpp
	)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(NHQTM_forms_UI MainWindow.ui)
#set(NHQTM_forms_SRCS Form_Main.cpp)

foreach (form ${forms})
#	list(APPEND NHQTM_forms_UI
#		Forms/Form_${form}.ui
#		)
	list(APPEND NHQTM_forms_SRCS
		Forms/Form_${form}.cpp
		)
endforeach ()
list(APPEND NHQTM_forms_SRCS Forms/DriverVersionConfirmationDialog.cpp)

foreach (component ${formComponents})
#	list(APPEND NHQTM_forms_UI
#		Forms/Form_${form}.ui
#		)
	list(APPEND NHQTM_formComponents_SRCS
		Forms/Components/${component}.cpp
		)
endforeach ()

foreach (util ${utils})
	list(APPEND NHQTM_utils_SRCS
		Utils/${util}.cpp
		)
endforeach ()

include_directories(
	BEFORE
	${Boost_INCLUDE_DIR}
	${QtSingleApplication_SRC}
	)
if (WITH_AMD)
include_directories(
	BEFORE
	${ADL_INCLUDE_DIR}
	)
endif ()
if (WITH_NVIDIA)
include_directories(
	BEFORE
	${NVML_INCLUDE_DIR}
#	${NVCtrlLib_INCLUDE_DIR}
	)
endif ()

add_executable(${PROJECT_NAME}
	main.cpp #done
	Enums.cpp
	ExchangeRateAPI.cpp
	Algorithm.cpp
	Globals.cpp
	IFTTT.cpp
	International.cpp
	Qt/ClickableLabel.cpp
	Qt/NHMCashRegisterBitcoin.cpp
	Qt/LException.cpp
#	Qt/LLineEditInt.cpp
#	Qt/LLineEditDouble.cpp
	${NHQTM_3rdParty_SRCS}
	${NHQTM_forms_SRCS}
	${NHQTM_formComponents_SRCS}
	${NHQTM_utils_SRCS}
	${NHQTM_devices_SRCS}
	${NHQTM_miners_SRCS}
	${NHQTM_configs_SRCS}
	${NHQTM_pinvoke_SRCS}
	${NHQTM_WinPort_SRCS}
	${NHQTM_Properties_SRCS}
	${NHQTM_Switching_SRCS}
	${NHQTM_Stats_SRCS}
	${UI_HEADERS}
	${UI_RESOURCES}
	../resources.qrc
	)


target_link_libraries(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::Network
	Qt5::WebSockets
	Qt5::Sql
	${Boost_LIBRARIES}
#	${NSJson_LIBRARY}
#	${CRYPTO_LIBRARIES}
	${HW_LIBRARIES}
	${QtSingleApplication_LIB}
#	${ADL_LIBRARY}
	${DL_LIBRARY}
	${LOG4_LIBRARY}
#	${UDEV_LIBRARY}
	${OPENCL_LIBRARIES}
	)
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
#SET_TARGET_PROPERTIES(${PROJECT_NAME}
 #   PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/"
#	)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(BSEd PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
	set_target_properties(BSEd PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif ()

qt5_use_modules(${PROJECT_NAME} Widgets)

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
