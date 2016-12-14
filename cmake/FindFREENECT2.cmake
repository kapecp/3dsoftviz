# - Find Freenect2 - this file should be in the CMAKE_MODULE_PATH
# Find Freenect2 headers and libraries
#
#  FREENECT2_INCLUDE_DIR - where to find openni2.h, etc.
#  FREENECT2_LIBRARIES    - List of libraries when using Freenect.
#  FREENECT2_FOUND        - True if Freenect was found.
#=============================================================================

find_path( FREENECT2_INCLUDE_DIR
	NAMES libfreenect2.hpp
	HINTS 
		/usr/local/include/libfreenect2
		PATHS
			$ENV{FREENECT2_INCLUDE}

	)
find_library( FREENECT2_LIBRARY
	NAMES libfreenect2.so Freenect2

	HINTS 
		/usr/local/lib/libfreenect2
		PATHS
			$ENV{FREENECT2_LIB}
			$ENV{FREENECT2_REDIST}
	)


# handle the QUIETLY and REQUIRED arguments and set OPENNI2_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( FREENECT2 REQUIRED_VARS FREENECT2_LIBRARY FREENECT2_INCLUDE_DIR )

# Copy the results to the output variables.
if( OPENNI2_FOUND )
	set( FREENECT2_LIBRARIES ${FREENECT2_LIBRARY} )
	set( FREENECT2_INCLUDE_DIRS ${FREENECT2_INCLUDE_DIR} )
endif()

mark_as_advanced( FREENECT2_INCLUDE_DIR FREENECT2_LIBRARY )
