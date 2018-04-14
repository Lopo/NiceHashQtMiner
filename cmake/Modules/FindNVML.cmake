# Module for locating NVIDIA Display Library (NVML)
#
# Customizable variables:
#   NVML_ROOT_DIR
#     Specifies NVML's root directory. The find module uses this variable to
#     locate NVML header files. The variable will be filled automatically unless
#     explicitly set using CMake's -D command-line option. Instead of setting a
#     CMake variable, an environment variable called NVMLROOT can be used.
#
# Read-only variables:
#   NVML_FOUND
#     Indicates whether NVML has been found.
#
#   NVML_INCLUDE_DIR
#     Specifies the NVML include directories.
#
#   NVML_LIBRARY
#     Specifies the NVML libraries that should be passed to
#     target_link_libararies.

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (NVML_INCLUDE_DIR
	NAMES
		nvml.h
		NVML/nvml.h
		nvml/nvml.h
	PATHS
		${NVML_ROOT_DIR}/usr/include/nvidia/gdk
		${CUDA_TOOLKIT_ROOT_DIR}
		${CUDA_TOOLKIT_INCLUDE}
	DOC "NVIDIA Display Library (NVML) include directory"
	)

FIND_PATH (NVML_LIBRARY_DIR
	NAMES libnvidia-ml.so
	PATHS ${NVML_ROOT_DIR}/usr/src/gdk/nvml/lib
	DOC "NVIDIA Display Library (NVML) directory"
	)


FIND_LIBRARY(NVML_LIBRARY
	NAMES libnvidia-ml.so
	HINTS ${NVML_LIBRARY_DIR}
	DOC "NVIDIA Display Library (NVML) location"
	)


MESSAGE("** NVIDIA Display Library (NVML) root directory .....: " ${NVML_ROOT_DIR})
if (NVML_INCLUDE_DIR)
	MESSAGE("** NVIDIA Display Library (NVML) include directory...: " ${NVML_INCLUDE_DIR})
endif ()
if (NVML_LIBRARY)
	MESSAGE("** NVIDIA Display Library (NVML) library directory...: " ${NVML_LIBRARY_DIR})
	MESSAGE("** NVIDIA Display Library (NVML) library.............: " ${NVML_LIBRARY})
else ()
	MESSAGE("** NVIDIA Display Library (NVML) library.............: NOT FOUND")
	MESSAGE("")
	MESSAGE("If your have an NVIDIA GPU, please install NVIDIA Device Drivers!")
	MESSAGE("Otherwise, you should configure BOSP to DISABLE NVIDIA Power Management")
	MESSAGE("")
endif ()

MARK_AS_ADVANCED (NVML_INCLUDE_DIR NVML_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NVML DEFAULT_MSG NVML_INCLUDE_DIR)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NVML DEFAULT_MSG NVML_LIBRARY)
