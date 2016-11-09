# Find MOUSE3D headers and libraries
#
#  MOUSE3D_INCLUDE_DIRS - where to find si.h, etc.
#  MOUSE3D_LIBRARIES    - List of libraries when using MOUSE3D.
#  MOUSE3D_FOUND        - True if MOUSE3D found.
#=============================================================================

#message("CMAKE_SYSTEM_FRAMEWORK_PATH = ${CMAKE_SYSTEM_FRAMEWORK_PATH}")

if ( WIN32)
	find_path( MOUSE3D_INCLUDE_DIR
		NAMES si.h
		PATHS $ENV{MOUSE3D_DIR}/Inc
	)
	find_library( MOUSE3D_LIBRARY
		NAMES siapp
		PATHS $ENV{MOUSE3D_DIR}/Lib/x86
	)
elseif( APPLE ) # The only platform it makes sense to check for 3DconnexionClient
	find_library( 3DconnexionClient 3DconnexionClient )
	if( 3DconnexionClient )
		set( MOUSE3D_FOUND true )
		set( MOUSE3D_INCLUDE_DIR ${3DconnexionClient} )
		set( MOUSE3D_LIBRARY ${3DconnexionClient} )
	endif()
endif()

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( MOUSE3D REQUIRED_VARS MOUSE3D_LIBRARY MOUSE3D_INCLUDE_DIR )

if( MOUSE3D_FOUND )
	set( MOUSE3D_LIBRARIES ${MOUSE3D_LIBRARY} )
	set( MOUSE3D_INCLUDE_DIRS ${MOUSE3D_INCLUDE_DIR} )
endif()

mark_as_advanced( MOUSE3D_INCLUDE_DIR MOUSE3D_LIBRARY )
