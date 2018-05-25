#add_library(QtSingleApplication SHARED
	
#	)
set(thirdpartys
	QtSingleApplication
	managedCuda
	)

foreach (part ${thirdpartys})
	add_subdirectory(${part})
endforeach ()
