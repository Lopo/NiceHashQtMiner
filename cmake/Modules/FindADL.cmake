# Find AMD Display Library (ADL)
# Once done this will define
# ADL_FOUND
# ADL_LIBRARY
# ADL_INCLUDE_DIR
# ADL_LIBWORKS

FIND_PATH(ADL_INCLUDE_DIR
	NAMES
		adl_sdk.h
		ADL/adl_sdk.h
		adl/adl_sdk.h
	HINTS
		${CMAKE_SOURCE_DIR}/3rdParty/ADL
	PATHS
		$ENV{ADLROOT}
		${ADL_ROOT_DIR}
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src
	DOC "AMD Display Library (ADL) include directory"
	)

FIND_PATH(ADL_LIBRARY_DIR
	NAMES
		libatiadlxx.so
		/usr/lib/fglrx
	PATHS
		/usr/lib
	DOC "AMD Display Library (ADL) directory"
	)

FIND_LIBRARY(ADL_LIBRARY
	NAMES
		atiadlxx
		atiadlxy
	HINTS
		${ADL_LIBRARY_DIR}
		${CMAKE_SOURCE_DIR}/lib
	PATHS /usr/lib/fglrx
	NO_DEFAULT_PATH
	DOC "AMD Display Library (ADL) location"
	)

if (NOT ADL_LIBRARY)
	MESSAGE(STATUS "Library not found, finding file instead.")
	FIND_FILE(ADL_LIBRARY
		NAMES
			atiadlxx.so
			atiadlxy.so
		PATHS /usr/lib/fglrx
		DOC "ADL library location"
		)
else ()
	set(SOURCE "
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

void* ADL_Main_Memory_Alloc(int iSize)
{
    void* vP=malloc(iSize);
	return vP;
}

int main()
{
	void* lib;
	int(*f)(void*, int);
	int ret;

	lib=dlopen(\"libatiadlxx.so\", RTLD_LAZY|RTLD_GLOBAL);
	if (lib==0) {
		return -1;
		}
	f=(int(*)(void*, int))dlsym(lib, \"ADL_Main_Control_Create\");
	if (f==0) {
		return -2;
		}
	ret=f(ADL_Main_Memory_Alloc, 1);

	printf(\"%d\", ret);
	exit(ret);
}
")
	file(WRITE "${CMAKE_BINARY_DIR}/CMakeTmp/CheckADL.c" "${SOURCE}")
#	message(STATUS "Checking whether ADL works")
	try_run(CHECKADL_R CHECKADL_C
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}/CMakeTmp/CheckADL.c
		LINK_LIBRARIES ${ADL_LIBRARY} dl
		COMPILE_OUTPUT_VARIABLE COMPILE_OUTPUT
		RUN_OUTPUT_VARIABLE RUN_OUTPUT
		)
file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
	"Checking whether ADL works\n"
	"compile output:\n"
	"${COMPILE_OUTPUT}\n"
	"run output\n"
	"${RUN_OUTPUT}\n"
	"Source file was:\n${SOURCE}\n"
	)
	if (CHECKADL_C AND CHECKADL_R AND RUN_OUTPUT STREQUAL "0")
#		message(STATUS "Checking whether ADL works -- yes")
		set(ADL_LIBWORKS TRUE)
	else ()
#		message(STATUS "Checking whether ADL works -- no")
		file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
			"Checking whether ADL works failed with the following output:\n"
			"${COMPILE_OUTPUT}\n"
			"${RUN_OUTPUT}\n"
			"Source file was:\n${SOURCE}\n"
			)
		set(ADL_LIBWORKS FALSE)
	endif ()
endif ()

MESSAGE("** AMD Display Library (ADL) root directory .....: " ${ADL_ROOT_DIR})
if (ADL_INCLUDE_DIR)
	MESSAGE("** AMD Display Library (ADL) include directory...: " ${ADL_INCLUDE_DIR})
endif ()
if (ADL_LIBRARY)
	MESSAGE("** AMD Display Library (ADL) library directory...: " ${ADL_LIBRARY_DIR})
	MESSAGE("** AMD Display Library (ADL) library.............: " ${ADL_LIBRARY})
	MESSAGE("** AMD Display Library (ADL) library works.......: " ${ADL_LIBWORKS})
else ()
	MESSAGE("** AMD Display Library (ADL) library.............: NOT FOUND")
	MESSAGE("")
	MESSAGE("If your have an AMD GPU, please install AMD Device Drivers!")
	MESSAGE("")
endif ()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADL DEFAULT_MSG ADL_LIBRARY ADL_INCLUDE_DIR)
