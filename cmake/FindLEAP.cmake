# - Find LEAP - this file should be in the CMAKE_MODULE_PATH
# Find LEAP headers and libraries
#
#  LEAP_INCLUDE_DIRS - where to find leap.h, etc.
#  LEAP_LIBRARIES    - List of libraries when using LEAP.
#  LEAP_FOUND        - True if LEAP found.
#=============================================================================
if( USE_LEAP_ORION )
	set ( LEAP_DEP_DIR "leap-orion")
else()
	set ( LEAP_DEP_DIR "leap")
endif()

find_path( LEAP_INCLUDE_DIR NAMES leap.h Leap.h
	PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${LEAP_DEP_DIR}/include )

find_library( LEAP_LIBRARY NAMES leap libLeap.so
	PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${LEAP_DEP_DIR}/lib )

if( WIN32 )
	find_path( LEAP_DLL
		NAMES leap.dll
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${LEAP_DEP_DIR}/dist
	)
endif()

# handle the QUIETLY and REQUIRED arguments and set LEAP_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( LEAP REQUIRED_VARS LEAP_LIBRARY LEAP_INCLUDE_DIR )

# Copy the results to the output variables.
if( LEAP_FOUND )
if( WIN32 )
	set( LEAP_DRIVER_DIRS ${LEAP_DLL} )
endif()
	set( LEAP_LIBRARIES ${LEAP_LIBRARY} )
	set( LEAP_INCLUDE_DIRS ${LEAP_INCLUDE_DIR} )
endif()

mark_as_advanced( LEAP_INCLUDE_DIR LEAP_LIBRARY )
