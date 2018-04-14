set(nvidiasetp0state_SRCS
	main.cpp
	)
source_group(tools\\nvidiasetp0state FILES ${nvidiasetp0state_SRCS})

#include_directories(${CUDA_INCLUDE_DIRS})

add_executable(nvidiasetp0state main.cpp ${nvidiasetp0state_SRCS})

target_link_libraries(nvidiasetp0state
	Qt5::Widgets
	)
