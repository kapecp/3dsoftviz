# Find MOUSE3D headers and libraries
#
#  MOUSE3D_INCLUDE_DIRS - where to find si.h, etc.
#  MOUSE3D_LIBRARIES    - List of libraries when using MOUSE3D.
#  MOUSE3D_FOUND        - True if MOUSE3D found.
#=============================================================================
find_path( MOUSE3D_INCLUDE_DIR
	NAMES si.h
	PATHS $ENV{MOUSE3D_DIR}/Inc
)
find_library( MOUSE3D_LIBRARY
	NAMES siapp
	PATHS $ENV{MOUSE3D_DIR}/Lib/x86
)

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( MOUSE3D REQUIRED_VARS MOUSE3D_LIBRARY MOUSE3D_INCLUDE_DIR )

if( MOUSE3D_FOUND )
	set( MOUSE3D_LIBRARIES ${MOUSE3D_LIBRARY} )
	set( MOUSE3D_INCLUDE_DIRS ${MOUSE3D_INCLUDE_DIR} )
endif()

mark_as_advanced( MOUSE3D_INCLUDE_DIR MOUSE3D_LIBRARY )
