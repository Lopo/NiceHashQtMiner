# Find AMD Display Library (ADL)
# Once done this will define
# ADL_FOUND
# ADL_LIBRARY
# ADL_INCLUDE_DIR

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
endif ()

MESSAGE("** AMD Display Library (ADL) root directory .....: " ${ADL_ROOT_DIR})
if (ADL_INCLUDE_DIR)
	MESSAGE("** AMD Display Library (ADL) include directory...: " ${ADL_INCLUDE_DIR})
endif ()
if (ADL_LIBRARY)
	MESSAGE("** AMD Display Library (ADL) library directory...: " ${ADL_LIBRARY_DIR})
	MESSAGE("** AMD Display Library (ADL) library.............: " ${ADL_LIBRARY})
else ()
	MESSAGE("** AMD Display Library (ADL) library.............: NOT FOUND")
	MESSAGE("")
	MESSAGE("If your have an AMD GPU, please install AMD Device Drivers!")
	MESSAGE("")
endif ()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADL DEFAULT_MSG ADL_LIBRARY ADL_INCLUDE_DIR)
