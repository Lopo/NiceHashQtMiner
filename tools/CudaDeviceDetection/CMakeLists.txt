set(CudaDeviceDetection_SRCS
	main.cpp
	CudaDetection.cpp
	)
source_group(tools\\CudaDeviceDetection FILES ${CudaDeviceDetection_SRCS})

include_directories(${CUDA_INCLUDE_DIRS})

add_executable(CudaDeviceDetection main.cpp ${CudaDeviceDetection_SRCS})

target_link_libraries(CudaDeviceDetection ${CUDA_LIBRARIES} ${NVML_LIBRARY})
