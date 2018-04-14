if (WITH_AMD)
	add_subdirectory(AMDOpenCLDeviceDetection)
endif ()
if (WITH_NVIDIA)
	add_subdirectory(CudaDeviceDetection)
	add_subdirectory(nvidiasetp0state)
endif ()

add_library(ionbf SHARED ionbf.c)
