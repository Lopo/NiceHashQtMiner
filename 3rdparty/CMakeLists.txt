#add_library(QtSingleApplication SHARED
	
#	)
set(thirdpartys
	QtSingleApplication
	)

foreach (part ${thirdpartys})
	add_subdirectory(${part})
endforeach ()
