cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
cmake_policy(VERSION 3.0)
message(STATUS "Running CMake version ${CMAKE_VERSION}")

project(NiceHashQtMiner)

set(NHQTM_VERSION_MAJOR 0)
set(NHQTM_VERSION_MINOR 1)
set(NHQTM_VERSION_PATCH 1)
set(DEVELOPMENT_BUILD TRUE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE FALSE)

option(WITH_DEBUG "With debug" ON)
option(SWITCH_TESTING "SWITCH_TESTING" OFF)
option(ENABLE_AMD "Enable AMD" ON)
option(ENABLE_NVIDIA "Enable NVIDIA" ON)

if (ENABLE_NVIDIA)
	if (NOT DEFINED CUDA_TOOLKIT_ROOT_DIR)
		if (DEFINED $ENV{CUDA_TOOLKIT_ROOT_DIR})
			message("Using ENV[CUDA_TOOLKIT_ROOT_DIR]=$ENV{CUDA_TOOLKIT_ROOT_DIR}")
			set(CUDA_TOOLKIT_ROOT_DIR $ENV{CUDA_TOOLKIT_ROOT_DIR})
		else ()
			set(CUDA_TOOLKIT_ROOT_DIR "/opt/cuda")
			message("CUDA_TOOLKIT_ROOT_DIR set to ${CUDA_TOOLKIT_ROOT_DIR}")
		endif ()
	else ()
		message("Using CUDA_TOOLKIT_ROOT_DIR=${CUDA_TOOLKIT_ROOT_DIR}")
	endif ()
endif ()

if (DEFINED CMAKE_BUILD_TYPE AND NOT ${CMAKE_BUILD_TYPE} EQUAL "")
	set(HAVE_CMAKE_BUILD_TYPE TRUE)
endif ()
set(CUSTOM_C_FLAGS $ENV{CFLAGS})
set(CUSTOM_CXX_FLAGS $ENV{CXXFLAGS})

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} REALPATH CACHE)
set(SOURCE_DIR ${ROOT_DIR}/src)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)

#SET(CMAKE_SKIP_BUILD_RPATH FALSE)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# for all binaries created in a CMake project:
#SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")
#set(CMAKE_INSTALL_RPATH "${ROOT_DIR}/lib")
#SET(CMAKE_INSTALL_RPATH "\$ORIGIN:\$ORIGIN/lib:\$ORIGIN/../lib")
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
#if ("${isSystemDir}" STREQUAL "-1")
#	SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#endif ()

# Arch
if (NOT DEFINED ${NHQTM_ARCH})
	if (CMAKE_SIZEOF_VOID_P MATCHES "8")
		SET(NHQTM_ARCH "x86_64" CACHE STRING "ARCH")
	elseif (CMAKE_SIZEOF_VOID_P MATCHES "4")
		SET(NHQTM_ARCH "x86" CACHE STRING "ARCH")
	else ()
		SET(NHQTM_ARCH "x86_64" CACHE STRING "ARCH")
		MESSAGE(WARNING "Could not define ARCH, using ${NHQTM_ARCH}.")
	endif ()
elseif (NOT NHQTM_ARCH MATCHES "x86_64" AND NOT NHQTM_ARCH MATCHES "x86")
		MESSAGE(FATAL_ERROR "Invalid NHQTM_ARCH (${NHQTM_ARCH}), use x86_64 or x86")
endif ()

if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
	set(LINUX true)
endif ()

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

include(cmake/Macros.cmake)
include(CheckInSourceBuild)
#include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(cmake/util.cmake)

set(BUILDTYPE_DOCSTRING
	"Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel"
	)
if (WITH_DEBUG OR DEVELOPMENT_BUILD)
	if (HAVE_CMAKE_BUILD_TYPE)
		if (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
			message(WARNING "WITH_DEBUG and DEVELOPMENT_BUILD are in conflict with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, disabling Debug/Dev")
			set(WITH_DEBUG FALSE)
			set(DEVELOPMENT_BUILD FALSE)
		elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
			message(WARNING "WITH_DEBUG or DEVELOPMENT_BUILD set to TRUE are in conflict with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, updating CMAKE_BUILD_TYPE to RelWithDebInfo")
			set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
		endif ()
	else ()
		set(CMAKE_BUILD_TYPE "Debug" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	endif ()
elseif (NOT HAVE_CMAKE_BUILD_TYPE)
	if (CUSTOM_C_FLAGS)
		set(CMAKE_BUILD_TYPE "" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	elseif (CMAKE_BUILDTYPE_ MATCHES "Debug" OR NOT HAVE_CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	endif ()
endif ()

include(cmake/CommonCompilerFlags.cmake)

check_include_file(stdint.h HAVE_STDINT_H)

if(false)
find_package(OpenSSL 0.9.9 REQUIRED)
if (OPENSSL_FOUND)
	list(APPEND CRYPTO_LIBRARIES ${OPENSSL_LIBRARIES})
	include(cmake/Modules/CheckOpenSSL.cmake)
	check_function_exists(SHA256_Update HAVE_SHA256_UPDATE)
endif ()
endif(false)
find_package(Threads REQUIRED)

set(WITH_AMD FALSE)
if (ENABLE_AMD)
	find_package(OpenCL 1.2 REQUIRED)
#MESSAGE( STATUS "OPENCL_INCLUDE_DIRS=${OPENCL_INCLUDE_DIRS}" )
#if (NOT OPENCL_HAS_CXX)
#	message(WARNING "C++ bindings not found.  Using Kronos supplied reference OpenCL v1.1 C++ bindings.")
#	list(APPEND OPENCL_INCLUDE_DIRS ${viscl_SOURCE_DIR}/kronos)
#endif ()
#include_directories(SYSTEM ${OPENCL_INCLUDE_DIRS})
#add_definitions(${OPENCL_CXX_DEFINITIONS})
#	foreach (lib ${OPENCL_LIBRARIES})
#		get_filename_component(fname ${lib} NAME)
#		get_filename_component(rpath ${lib} REALPATH)
#		get_filename_component(rfname ${rpath} NAME)
#		message("Copying ${lib} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#		file(COPY ${rpath}
#			DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#			)
#		file(RENAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${rfname}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${fname}")
#	endforeach ()
#	check_library_exists(atiadlxx ADL_MAIN_CONTROL_CREATE "" HAVE_LIBADL)
	if (OPENCL_FOUND)
		set(WITH_AMD true)
	endif ()
#	check_library_exists(atiadlxx ADL_MAIN_CONTROL_CREATE "" HAVE_LIBADL)
	find_package(ADL)
endif ()

set(WITH_NVIDIA FALSE)
if (ENABLE_NVIDIA)
#	include(cmake/Modules/FindCUDA.cmake)
	find_package(CUDA 8.0)
	find_package(NVML)
#	find_package(NVCtrlLib)
	if (CUDA_FOUND AND NVML_FOUND)
		set(WITH_NVIDIA true)
	endif ()
	if (CUDA_FOUND AND false)
		set(WITH_CUDA TRUE)
		find_library(NVML_LIBRARY
			NAMES
				nvidia-ml
#				libnvidia-ml
#				libnvidia-ml.so
			HINTS
				${CUDA_TOOLKIT_ROOT_DIR}
			PATH_SUFFIXES
				/lib
				/lib64
				/lib/stubs
				/lib64/stubs
			)
		if (NVML_LIBRARY)
			get_filename_component(fname ${NVML_LIBRARY} NAME)
			get_filename_component(rpath ${NVML_LIBRARY} REALPATH)
			get_filename_component(rfname ${rpath} NAME)
			message("Copying ${NVML_LIBRARY} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
			file(COPY ${rpath}
				DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
				)
			file(RENAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${rfname}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${fname}")
		endif ()
	endif ()
endif ()

check_library_exists(cpuid cpuid_present "" HAVE_LIBCPUID)
if (HAVE_LIBCPUID)
	list(APPEND HW_LIBRARIES cpuid)
endif ()
check_library_exists(hd hd_list "" HAVE_LIBHD)
if (HAVE_LIBHD)
	list(APPEND HW_LIBRARIES hd)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXSS xscrnsaver)

find_package(Qt5
	COMPONENTS
		Core
		Gui
		Widgets
		Network
		WebSockets
		Concurrent
		Sql
	REQUIRED
	)
find_library(DL_LIBRARY
	NAMES dl
	)
find_library(LOG4_LIBRARY
	log4qt
	)

configure_file(
	${ROOT_DIR}/cmake/config.h.in
	${INCLUDE_DIR}/config.h
	)
set_source_files_properties(${INCLUDE_DIR}/config.h PROPERTIES GENERATED 1)
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${INCLUDE_DIR}/config.h)
include(cmake/Version.cmake)
set_source_files_properties(${INCLUDE_DIR}/version.h PROPERTIES GENERATED 1)
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${INCLUDE_DIR}/version.h)

if (ENABLE_STRIP OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
	if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
		message(WARNING "ENABLE_STRIP & CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} are in conflict, ENABLE_STRIP ignored")
	elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
	endif ()
endif ()

if (ENABLE_MULTIBUILD)
	if (WIN32 AND MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
	endif ()
endif ()

include_directories(include)
set(SUBDIRS
	3rdparty
	tools
	NiceHashQtMiner
	)
foreach (subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach ()

add_custom_target(
	permission chown root NiceHashQtMiner
	COMMAND chmod u+s NiceHashQtMiner
	)

message("${PROJECT_NAME} ${PROJECT_VERSION}\n")
if (WIN32 AND MSVC)
	message("Platform: Windows (MSVC)")
elseif (WIN32 AND MINGW)
	message("Platform: Windows (MinGW)")
elseif (WIN32)
	message("Platform: Windows (Other)")
elseif (LINUX)
	message("Platform: Linux")
else ()
	message("Platform: Other/No platform code")
endif ()
message("System  : ${CMAKE_SYSTEM}")
message("	Arch: ${NHQTM_ARCH}
	compiler  | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
	cxxflags  | ${CMAKE_CXX_FLAGS}
	builddir  | ${CMAKE_BINARY_DIR}
	buildtype | ${CMAKE_BUILD_TYPE}
Building with support for:")
show_end_message_yesno("Debug" WITH_DEBUG)
show_end_message_yesno("	- SWITCH_TESTING" SWITCH_TESTING)
show_end_message_yesno("AMD" WITH_AMD)
show_end_message_yesno("	- OPENCL" OPENCL_FOUND)
#show_end_message_yesno("	- ADL" ADL_FOUND)
show_end_message("	- ADL" "")
show_end_message_yesno("		- FOUND" ADL_FOUND)
show_end_message_yesno("		- LIBWORKS" ADL_LIBWORKS)
show_end_message_yesno("NVIDIA" WITH_NVIDIA)
show_end_message_yesno("	- CUDA" CUDA_FOUND)
show_end_message_yesno("	- NVML" NVML_FOUND)
#show_end_message_yesno("	- NVCtrlLib" NVCtrlLib_FOUND)
show_end_message_yesno("SCRNSAVER" LIBXSS_FOUND)

install(SCRIPT chown.cmake)
