add_custom_target(managedCuda ALL cmake Nvml
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "building managedCuda library"
	)
set(managedCuda_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Nvml" CACHE PATH "")
set(managedCuda_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libManagedNvml.so" CACHE FILEPATH "")
