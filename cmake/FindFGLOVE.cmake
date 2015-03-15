# - Find FGLOVE - this file should be in the CMAKE_MODULE_PATH
# Find FGLOVE headers and libraries
#
#  FGLOVE_INCLUDE_DIRS - where to find FGLOVE.h, etc.
#  FGLOVE_LIBRARIES    - List of libraries when using openni.
#  FGLOVE_FOUND        - True if FGLOVE found.
#=============================================================================
if( WIN32 )
	find_path( FGLOVE_INCLUDE_DIR NAMES fglove.h 
		PATHS
				${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fglove/include )

	find_library( FGLOVE_LIBRARY NAMES fglove 
		PATHS
			${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fglove/lib )

	find_path( FGLOVE_DLL
		NAMES fglove.dll
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fglove/dist
	)
endif()

# handle the QUIETLY and REQUIRED arguments and set FGLOVE_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( FGLOVE REQUIRED_VARS FGLOVE_LIBRARY FGLOVE_INCLUDE_DIR FGLOVE_DLL )

# Copy the results to the output variables.
if( FGLOVE_FOUND )
	set( FGLOVE_DRIVER_DIRS ${FGLOVE_DLL} )
	set( FGLOVE_LIBRARIES ${FGLOVE_LIBRARY} )
	set( FGLOVE_INCLUDE_DIRS ${FGLOVE_INCLUDE_DIR} )
endif()

mark_as_advanced( FGLOVE_INCLUDE_DIR FGLOVE_LIBRARY )
