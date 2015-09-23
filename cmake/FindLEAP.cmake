# - Find LEAP - this file should be in the CMAKE_MODULE_PATH
# Find LEAP headers and libraries
#
#  LEAP_INCLUDE_DIRS - where to find leap.h, etc.
#  LEAP_LIBRARIES    - List of libraries when using LEAP.
#  LEAP_FOUND        - True if LEAP found.
#=============================================================================
if( WIN32 )
	find_path( LEAP_INCLUDE_DIR NAMES leap.h 
		PATHS
				${CMAKE_CURRENT_SOURCE_DIR}/dependencies/leap/include )

	find_library( LEAP_LIBRARY NAMES leap 
		PATHS
			${CMAKE_CURRENT_SOURCE_DIR}/dependencies/leap/lib )

	find_path( LEAP_DLL
		NAMES leap.dll
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/leap/dist
	)
endif()

# handle the QUIETLY and REQUIRED arguments and set LEAP_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( LEAP REQUIRED_VARS LEAP_LIBRARY LEAP_INCLUDE_DIR LEAP_DLL )

# Copy the results to the output variables.
if( LEAP_FOUND )
	set( LEAP_DRIVER_DIRS ${LEAP_DLL} )
	set( LEAP_LIBRARIES ${LEAP_LIBRARY} )
	set( LEAP_INCLUDE_DIRS ${LEAP_INCLUDE_DIR} )
endif()

mark_as_advanced( LEAP_INCLUDE_DIR LEAP_LIBRARY )
