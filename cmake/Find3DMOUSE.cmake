# Find 3DMOUSE headers and libraries
#
#  3DMOUSE_INCLUDE_DIRS - where to find si.h, etc.
#  3DMOUSE_LIBRARIES    - List of libraries when using 3DMOUSE.
#  3DMOUSE_FOUND        - True if 3DMOUSE found.
#=============================================================================
if( WIN32 )
	find_path( 3DMOUSE_INCLUDE_DIR
		NAMES si.h siapp.h spwmath.h
		PATHS $ENV{3DMOUSE_DIR}/Inc
	)
	find_library( 3DMOUSE_LIBRARY
		NAMES siapp spwmath
		PATHS $ENV{3DMOUSE_DIR}/Lib/x86
	)
endif()

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( 3DMOUSE REQUIRED_VARS 3DMOUSE_LIBRARY 3DMOUSE_INCLUDE_DIR )

if( 3DMOUSE_FOUND )
	set( 3DMOUSE_LIBRARIES ${SPACEMOUSE_LIBRARY} )
	set( 3DMOUSE_INCLUDE_DIRS ${SPACEMOUSE_INCLUDE_DIR} )
endif()

mark_as_advanced( 3DMOUSE_INCLUDE_DIR 3DMOUSE_LIBRARY )
