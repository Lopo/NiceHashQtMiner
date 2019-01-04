add_custom_target(QtSingleApplication ALL ./configure -library
	COMMAND qmake
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "building QtSingleApplication library"
	)
set(QtSingleApplication_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "")
set(QtSingleApplication_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libQt5Solutions_SingleApplication-head.so" CACHE FILEPATH "")
