# Generates include/version.h

# NHQtM version
set(NHQTM_VERSION_STRING "${NHQTM_VERSION_MAJOR}.${NHQTM_VERSION_MINOR}.${NHQTM_VERSION_PATCH}")
set(NHQTM_VERSION_BUILD "" CACHE STRING "Stuff to append to version string")

if (NHQTM_VERSION_BUILD)
	set(NHQTM_VERSION_STRING "${NHQTM_VERSION_STRING}-${NHQTM_VERSION_BUILD}")
elseif (DEVELOPMENT_BUILD)
	set(NHQTM_VERSION_STRING "${NHQTM_VERSION_STRING}-dev")
else ()
	# When building from a git clone, set the extra version to the HEAD revision, replacing any existing value
	find_program(nhqtm_git git)
	if (nhqtm_git)
		if (NOT NHQTM_VERSION_SOURCE_DIR)
			set(NHQTM_VERSION_SOURCE_DIR ${PROJECT_SOURCE_DIR})
		endif ()

		execute_process(COMMAND ${nhqtm_git} rev-parse HEAD
			WORKING_DIRECTORY ${NHQTM_VERSION_SOURCE_DIR}
			RESULT_VARIABLE nhqtm_git_result
			OUTPUT_VARIABLE nhqtm_git_output
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		if (${nhqtm_git_result} EQUAL 0)
			string(SUBSTRING ${nhqtm_git_output} 0 7 nhqtm_git_short)
			set(NHQTM_VERSION_BUILD "-${nhqtm_git_short}")
		endif ()
	endif ()
endif ()

set(PROJECT_VERSION_MAJOR ${NHQTM_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${NHQTM_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${NHQTM_VERSION_PATCH})
set(PROJECT_VERSION_TWEAK ${NHQTM_VERSION_BUILD})
set(PROJECT_VERSION ${NHQTM_VERSION_STRING})

# version.h content
set(nhqtm_version_file "${INCLUDE_DIR}/version.h")
set(nhqtm_old_version "")
set(nhqtm_new_version
	"// ${AUTOGEN_WARNING}
#ifndef NHQTM_VERSION_H
#define NHQTM_VERSION_H

#define NHQTM_VERSION_MAJOR ${NHQTM_VERSION_MAJOR}
#define NHQTM_VERSION_MINOR ${NHQTM_VERSION_MINOR}
#define NHQTM_VERSION_PATCH ${NHQTM_VERSION_PATCH}
#define NHQTM_VERSION_BUILD \"${NHQTM_VERSION_BUILD}\"
#define NHQTM_VERSION_STRING \"${NHQTM_VERSION_STRING}\"

#endif // NHQTM_VERSION_H
")

# Read in the old file (if it exists)
if (EXISTS ${nhqtm_version_file})
	file(READ ${nhqtm_version_file} nhqtm_old_version)
endif ()

# Only write the file if the content has changed
string(COMPARE NOTEQUAL
	"${nhqtm_old_version}" "${nhqtm_new_version}"
	nhqtm_update_version_file
	)
if (${nhqtm_update_version_file})
	message(STATUS "Creating ${nhqtm_version_file}")
	file(WRITE ${nhqtm_version_file} ${nhqtm_new_version})
endif ()
