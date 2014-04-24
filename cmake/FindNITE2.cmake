# - Find NiTe2 - this file should be located in the CMAKE_MODULE_PATH
# Find NiTe2 headers and libraries
#
#  NITE2_INCLUDE_DIRS - where to find NiTe.h, etc.
#  NITE2_LIBRARIES    - List of libraries when using NiTe.
#  NITE2_FOUND        - True if NiTe found.
#=============================================================================
if (WIN32)
	find_path( NITE2_INCLUDE_DIR NAMES NiTe.h )
	find_path( OPENNI2_DLL NAMES Kinect.dll )

	find_library( NITE2_LIBRARY NAMES NiTe2 )
endif ()

# handle the QUIETLY and REQUIRED arguments and set NITE2_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( NITE2 REQUIRED_VARS NITE2_LIBRARY NITE2_INCLUDE_DIR OPENNI2_DLL )

# Copy the results to the output variables.
if( NITE2_FOUND )
	set( NITE2_LIBRARIES ${NITE2_LIBRARY} )
	set( NITE2_INCLUDE_DIRS ${NITE2_INCLUDE_DIR} )
	set( NITE2_OPENNI2_DRIVER_DIRS ${OPENNI2_DLL} )
endif()

mark_as_advanced( NITE2_INCLUDE_DIR NITE2_LIBRARY OPENNI2_DLL )
