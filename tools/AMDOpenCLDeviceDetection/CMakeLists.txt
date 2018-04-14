set(AMDOpenCLDeviceDetection_SRCS
	main.cpp
	AMDOpenCLDeviceDetection.cpp
	)
source_group(tools\\AMDOpenCLDeviceDetection FILES ${AMDOpenCLDeviceDetection_SRCS})

include_directories(${OPENCL_INCLUDE_DIRS})

add_executable(AMDOpenCLDeviceDetection main.cpp ${AMDOpenCLDeviceDetection_SRCS})

target_link_libraries(AMDOpenCLDeviceDetection ${OPENCL_LIBRARIES})
