# - Find OpenNI2 - this file should be in the CMAKE_MODULE_PATH
# Find OpenNI2 headers and libraries
#
#  OPENNI2_INCLUDE_DIRS - where to find openni2.h, etc.
#  OPENNI2_LIBRARIES    - List of libraries when using openni.
#  OPENNI2_FOUND        - True if OpenNI2 found.
#=============================================================================

find_path( OPENNI2_INCLUDE_DIR
	NAMES OpenNI.h
	HINTS 
		/home/kapec/Desktop/work-in-progress/OpenNI2/Include
		/home/kapecp/Desktop/Kinect/OpenNI2/Include
		PATHS $ENV{OPENNI2_INCLUDE}
	)
find_library( OPENNI2_LIBRARY
	NAMES OpenNI2
	HINTS 
		/home/kapec/Desktop/work-in-progress/OpenNI2/Bin/x64-Release
		/home/kapecp/Desktop/Kinect/OpenNI2/Bin/x64-Release/
		PATHS $ENV{OPENNI2_LIB}
	)


# handle the QUIETLY and REQUIRED arguments and set OPENNI2_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( OPENNI2 REQUIRED_VARS OPENNI2_LIBRARY OPENNI2_INCLUDE_DIR )

# Copy the results to the output variables.
if( OPENNI2_FOUND )
	set( OPENNI2_LIBRARIES ${OPENNI2_LIBRARY} )
	set( OPENNI2_INCLUDE_DIRS ${OPENNI2_INCLUDE_DIR} )
endif()

mark_as_advanced( OPENNI2_INCLUDE_DIR OPENNI2_LIBRARY )
