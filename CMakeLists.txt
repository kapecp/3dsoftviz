# Copyright ( C ) 2013-2015 3DSoftviz.
# Created by Peter Kapec
# Redistribution and use of this file is allowed
# according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with 3DSoftviz.

project( 3DSoftviz C CXX )
cmake_minimum_required( VERSION 2.8.11 )

# --------------
# 3DSoftviz Info
# --------------

set( 3DSOFTVIZ_NAME "3DSoftviz" )
set( 3DSOFTVIZ_LONGNAME "3DSoftviz - Generic 3D graph visualization tool for software visualization" )
set( 3DSOFTVIZ_VERSION_MAJOR "0" )
set( 3DSOFTVIZ_VERSION_MINOR "0" )
set( 3DSOFTVIZ_VERSION_PATCH "0" )
set( 3DSOFTVIZ_VERSION_BUILD "1" )
set( 3DSOFTVIZ_VERSION "${3DSOFTVIZ_VERSION_MAJOR}.${3DSOFTVIZ_VERSION_MINOR}.${3DSOFTVIZ_VERSION_PATCH}" )
set( 3DSOFTVIZ_VENDOR "3DSoftviz Dev Team" )
set( 3DSOFTVIZ_COPYRIGHT_YEAR "2013" )
set( 3DSOFTVIZ_DOMAIN_FIRST "stuba" )
set( 3DSOFTVIZ_DOMAIN_SECOND "sk" )
set( 3DSOFTVIZ_DOMAIN "${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_DOMAIN_SECOND}" )

# --------------
# Build settings
# --------------

# Install destinations
set( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set( INSTALL_LIB lib CACHE PATH "Where to install lib to." )
set( INSTALL_DATA share/3dsoftviz CACHE PATH "Directory for shared data." )
set( INSTALL_DEPS . )
set( INSTALL_PLUGIN bin )
set( INSTALL_QTCONF bin )

set( INSTALL_INC include CACHE PATH "Directory for library headers." )
set( INSTALL_DATA . CACHE PATH "Directory the package can store documentation, tests or other data in." )
set( INSTALL_DOC ${INSTALL_DATA}/doc CACHE PATH "Recommended directory to install documentation into." )
set( INSTALL_ETC ${INSTALL_DATA}/etc CACHE PATH "Other files." )
set( INSTALL_TEST ${INSTALL_DATA}/test CACHE PATH "Test files." )
set( INSTALL_EXAMPLE ${INSTALL_DATA}/example CACHE PATH "Recommended directory to install examples into." )

# Additional CMake scripts
set( CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
	${CMAKE_MODULE_PATH} )

# Warnings for Debug mode
option( USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON )
if( USE_STRICT_COMPILE_WARNINGS )
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		# using Clang
		set( FLAGS_FOR_DEBUG "-std=c++11 -pedantic -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal  -Wformat  -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wformat=2  -Wimport  -Winit-self -Winline -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked  -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare  -Wsign-promo -Wstack-protector -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings  -Wno-unused-parameter -Wno-unused" )
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR CMAKE_COMPILER_IS_GNUCXX)
		# using GCC
		set( FLAGS_FOR_DEBUG "-std=c++11 -pedantic -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Wimport -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wlong-long -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare -Wsign-promo -Wstack-protector -Wstrict-aliasing=2 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wold-style-cast -Woverloaded-virtual -Wuseless-cast -Wsign-conversion -Wno-unused-parameter -Wno-unused" )

		# to suppress warnings for unused stuff use: -Wno-unused-parameter -Wno-unused-result
		# not used:
			# too much warnings in Qt headers: NULL vs 0 pointers: -Wzero-as-null-pointer-constant
			# too much warnings: -Wshadow -Waggregate-return
			# too much warnings, but should be used: -Wold-style-cast -Woverloaded-virtual -Wuseless-cast -Wsign-conversion
			# class size optimizations ( not really needed ): -Wpadded
			# style guidelines from Scott Meyers' Effective C++ books: -Weffc++
			# only for C: -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		# using Visual Studio C++
		set( FLAGS_FOR_DEBUG "/W3")
	endif()
endif()

# Default build type
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE )
endif()

# Default install destination
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
	set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install CACHE PATH "Installation Destination" FORCE )
endif()

# RPath functionality
set( CMAKE_SKIP_BUILD_RPATH FALSE CACHE STRING "" FORCE )
# next line adds "$ORIGIN/../lib" to RPath:
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN" CACHE STRING "" FORCE )
# next line adds additional link paths to RPath (e.g. for qscintilla when not in system):
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "" FORCE )

# Use ld.gold if it is available and isn't disabled explicitly
option(USE_LD_GOLD "Use GNU gold linker" ON)
if (USE_LD_GOLD)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
	if ("${LD_VERSION}" MATCHES "GNU gold")
		message(STATUS "GNU gold linker found.")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=gold" CACHE STRING "" FORCE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold" CACHE STRING "" FORCE)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags" CACHE STRING "" FORCE)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags" CACHE STRING "" FORCE)
	else ()
		message(WARNING "GNU gold linker isn't available, using the default system linker.")
	endif ()
endif ()

# --------------
# Build settings - COTIRE module
# --------------
include( cotire )
if(COMMAND cotire )
		if( NOT COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES )
				include( ProcessorCount )
				ProcessorCount( N )

				if( NOT N EQUAL 0 )
						# set number of parallel unity builds to processor count
						set( COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j${N}" )
				endif()
		endif()

	set( COTIRE_VERBOSE FALSE )
	set( COTIRE_DEBUG FALSE )

	### Fix virtual memory range
	if( MSVC )
		set (COTIRE_PCH_MEMORY_SCALING_FACTOR "240")
	endif()

	# fix for Diluculum and LuaJIT on 64bit OSX: Lua new_state returned NULL, LuaJIT needs following flags
	#if( APPLE )
	#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "" FORCE)
	#endif()
endif()

# ------------------------------------------------------------
# Dependencies - Searching external dependencies
# ------------------------------------------------------------

# ------------------------------ Qt4 -------------------------------
# Find Qt4
find_package( Qt4 REQUIRED )
set( QT_USE_QTOPENGL 1 )
set( QT_USE_QTSQL 1 )
set( QT_USE_QTWEBKIT 1 )
set( QT_USE_QTXML 1 )
set( QT_USE_QTNETWORK 1 )
include( ${QT_USE_FILE} )

# autogenerate all moc files
set( CMAKE_AUTOMOC TRUE )

# add special flag to GCC compiler when using Qt 4.6.x with GCC >4.6
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND
	"${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "4.7" AND
	${CMAKE_C_COMPILER_VERSION} VERSION_GREATER "4.6" )

	set( CMAKE_CXX_FLAGS "-fpermissive" )
endif()

# ------------------------------ OpenSceneGraph -------------------------------
# Find OpenSceneGraph
find_package( OpenSceneGraph COMPONENTS
	osgGA
	osgUtil
	OpenThreads
	osg
	osgAnimation
	osgDB
	osgFX
	#osgIntrospection
	osgManipulator
	osgParticle
	osgShadow
	osgSim
	osgTerrain
	osgText
	osgVolume
	osgWidget
 )
find_package( osgViewer )

include_directories( SYSTEM
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}
)

# ------------------------------ OpenCV -------------------------------
# Find OpenCV
find_package( OpenCV )
if( OpenCV_FOUND )
	if( WIN32 )
		add_definitions( -DNOMINMAX )
	endif()

	include_directories( SYSTEM ${OpenCV_INCLUDE_DIRS} )
	add_definitions( -DOPENCV_FOUND )

	message( STATUS "OpenCV_LIB_DIR=${OpenCV_LIB_DIR}" )
	message( STATUS "OpenCV_LIB_DIR=${OpenCV_LIBS}" )
else()
	message( STATUS "OpenCV Not Found" )
endif()

# ------------------------------ OpenNI2 and Nite -------------------------------
# Find OPENNI2
find_package( OPENNI2 )
if( OPENNI2_FOUND )
	include_directories( SYSTEM ${OPENNI2_INCLUDE_DIRS} )
	add_definitions( -DOPENNI2_FOUND )
	message( STATUS "OpenNI2 FOUND" )
else( OPENNI2_FOUND )
	message( STATUS "OpenNI2 NOT FOUND" )
endif()

# Find NITE2
find_package( NITE2 )
if( NITE2_FOUND )
	include_directories( SYSTEM ${NITE2_INCLUDE_DIRS} ${NITE2_OPENNI2_DRIVER_DIRS} )
	add_definitions( -DNITE2_FOUND )
	message ( STATUS "NITE2 FOUND" )
else( NITE2_FOUND )
	message ( STATUS "NITE2 NOT FOUND" )
endif()


# ------------------------------- KINECT SDK ----------------------------------
if( WIN32 AND MSVC )

	find_package( KINECTSDK )
	if( KINECTSDK_FOUND )
		include_directories( SYSTEM ${KINECTSDK_INCLUDE_DIRS} )
		add_definitions( -DKINECTSDK_FOUND )

		# find_package( SPEECHSDK )
		# if( SPEECHSDK_FOUND )
			# include_directories( SYSTEM ${SPEECHSDK_INCLUDE_DIRS} )
			# add_definitions( -DSPEECHSDK_FOUND )
			# message( STATUS "SPEECHSDK FOUND" )
		# else( SPEECHSDK_FOUND )
			# message ( STATUS "SPEECHSDK NOT FOUND" )
		# endif()

		message ( STATUS "KINECTSDK FOUND" )
	else( KINECTSDK_FOUND )
		message ( STATUS "KINECTSDK NOT FOUND" )
	endif()
endif()

# ------------------------------ BOOST -------------------------------
# Find Boost
set( Boost_USE_STATIC_LIBS OFF )
find_package( Boost 1.39 REQUIRED )
#add_definitions( -DBOOST_ALL_DYN_LINK )

# Include directories
include_directories( SYSTEM ${Boost_INCLUDE_DIRS}  )

# ------------------------------ LEAP -------------------------------
# Find leap
find_package( LEAP )
if( LEAP_FOUND )
	add_definitions( -DLEAP_FOUND )
	message( STATUS "LEAP_LIB_DIR=${LEAP_LIBRARIES}" )
else()
	message( STATUS "LEAP Not Found" )
endif()

# ------------------------------ FGLOVE -------------------------------
# Find fglove
# find_package( FGLOVE )
# if( FGLOVE_FOUND )
#	add_definitions( -DFGLOVE_FOUND )
#	include_directories( SYSTEM
#		${FGLOVE_INCLUDE_DIRS}
#	)
#	message( STATUS "FGLOVE_LIB_DIR=${FGLOVE_LIBRARIES}" )
# else()
#	message( STATUS "FGLOVE Not Found" )
# endif()
# ------------------------------ MOUSE3D -------------------------------
# Find MOUSE3D
find_package( MOUSE3D )
if( MOUSE3D_FOUND )
	include_directories( SYSTEM ${MOUSE3D_INCLUDE_DIRS} )
	add_definitions( -DMOUSE3D_FOUND )
	message( STATUS "3DMouse FOUND" )
else()
	message( STATUS "3DMouse NOT FOUND" )
endif()

ADD_DEFINITIONS(-DOSG_LIBRARY_STATIC)

# ------------------------------------------------------------
# Dependencies - External projects
# ------------------------------------------------------------

# Build dependencies by default
option( USE_BUILTIN_DEPENDENCIES "Use builtin dependencies." ON )

# Build dependencies
if( USE_BUILTIN_DEPENDENCIES )
	# to use Gold linker add following flags to ExternalProject_Add command, to CMAKE_ARGS (TODO: needs more testing):
		#-DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
		#-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}

	include( ExternalProject )

	# set search paths to the dependencies
	set( DEP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies )
	set( DEP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dependencies )
	set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEP_BIN}/lib ${DEP_BIN}/bin )
	set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${DEP_BIN}/include )

	# Generate cache
	configure_file( config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.cmake )

	# Build libnoise
	ExternalProject_Add( dep_libnoise
		PREFIX libnoise
		SOURCE_DIR ${DEP_SOURCE}/libnoise
		BINARY_DIR libnoise
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
			-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
	)

	# add libnoise library
	add_library( libnoise SHARED IMPORTED )
	add_dependencies( libnoise dep_libnoise )
	set_target_properties( libnoise PROPERTIES
		IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )

	if( WIN32 )
		set_target_properties( libnoise PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
			IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

		if( MSVC )
			# set "lib" prefix for MSVC, because libnoise.def file requires that the library is named "libnoise"
			set( MISS_MSVC_PREFIX lib )
			set_target_properties( libnoise PROPERTIES
				IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

		endif()
	endif()

	# Aruco
	if( OpenCV_FOUND )
		# Build aruco
		include( ExternalProject )
		ExternalProject_Add( dep_aruco
			PREFIX aruco
			SOURCE_DIR ${DEP_SOURCE}/aruco
			BINARY_DIR aruco
			CMAKE_ARGS
				-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
				-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
				-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
		)

		add_library( aruco SHARED IMPORTED )
		add_dependencies( aruco dep_aruco )
		set_target_properties( aruco PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX} )

		if( WIN32 AND MSVC )
			set_target_properties( aruco PROPERTIES
				IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_STATIC_LIBRARY_SUFFIX} )
		endif()
	endif() # OpenCV_FOUND


	# Build osgModeling
	ExternalProject_Add( dep_osgModeling
		PREFIX osgModeling
		SOURCE_DIR ${DEP_SOURCE}/osgModeling
		BINARY_DIR osgModeling
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
			-DBUILD_PLUGINS=ON
			-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
	)

	add_library( osgModeling SHARED IMPORTED )
	add_dependencies( osgModeling dep_osgModeling )
	set_target_properties( osgModeling PROPERTIES
		IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_SHARED_LIBRARY_SUFFIX} )

	if( WIN32 AND MSVC )
		set_target_properties( osgModeling PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_SHARED_LIBRARY_SUFFIX}
			IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}osgModeling${CMAKE_STATIC_LIBRARY_SUFFIX} )
	endif()


	# Build igloo
	ExternalProject_Add( dep_igloo
		PREFIX igloo
		SOURCE_DIR ${DEP_SOURCE}/igloo
		BINARY_DIR igloo
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
			-DBUILD_TESTS=ON
			-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
		# little trick:
			# this solves building igloo twice (building normal target and building unity target)
			# prevent building the default target (make install)
		BUILD_COMMAND ""
			# force building the install_unity target
        INSTALL_COMMAND "${CMAKE_COMMAND}" --build . --target install_unity# -- "-j${N}"
	)

	add_library( igloo INTERFACE IMPORTED )
	add_dependencies( igloo dep_igloo )


	# Building Lua or LuaJIT
	if ( MSVC OR APPLE )
		# Build Lua
		ExternalProject_Add ( dep_lua
			PREFIX lua
			SOURCE_DIR ${DEP_SOURCE}/lua
			BINARY_DIR lua
			CMAKE_ARGS
				-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
				-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
				-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
		)

		# add lua library
		add_library( lua SHARED IMPORTED )
		add_dependencies( lua dep_lua )
		set_target_properties( lua PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )

		if( WIN32 AND MSVC )
			set_target_properties( lua PROPERTIES
				IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_STATIC_LIBRARY_SUFFIX} )
		endif()

	else ()
		# NOTE: Diluculum requires luaconf.h from original Lua, so original Lua headers must be available

		# Build LuaJIT
		ExternalProject_Add ( dep_luajit
			PREFIX luajit
			SOURCE_DIR ${DEP_SOURCE}/luajit
			BINARY_DIR luajit
			CMAKE_ARGS
				-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
				-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
				-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
		)

		# add lua library
		add_library( lua SHARED IMPORTED )
		add_dependencies( lua dep_luajit )
		set_target_properties( lua PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )

		if( WIN32 AND MSVC )
			set_target_properties( lua PROPERTIES
				IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_STATIC_LIBRARY_SUFFIX} )
		endif()
	endif()

	if( NOT MSVC )
		# NOTE: on MSVC Diluculum code is directly inserted

		# Build Diluculum
		ExternalProject_Add ( dep_diluculum
			PREFIX diluculum
			SOURCE_DIR ${DEP_SOURCE}/diluculum
			BINARY_DIR diluculum
			CMAKE_ARGS
				-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
				-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

		# add diluculum library
		add_library( diluculum SHARED IMPORTED )
		add_dependencies( diluculum dep_diluculum )
		set_target_properties( diluculum PROPERTIES
			IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )

		if( WIN32 AND MSVC )
			set_target_properties( diluculum PROPERTIES
				IMPORTED_LOCATION ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_STATIC_LIBRARY_SUFFIX} )
		endif()
	endif()

	# Build lpeg
	ExternalProject_Add ( dep_lpeg
		PREFIX lpeg
		SOURCE_DIR ${DEP_SOURCE}/lpeg
		BINARY_DIR lpeg
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

	# Build leg
	ExternalProject_Add ( dep_leg
		PREFIX leg
		SOURCE_DIR ${DEP_SOURCE}/leg
		BINARY_DIR leg
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

	# Build luametrics
	ExternalProject_Add ( dep_luametrics
		PREFIX luametrics
		SOURCE_DIR ${DEP_SOURCE}/luametrics
		BINARY_DIR luametrics
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

	# Build luafilesystem
	ExternalProject_Add ( dep_luafilesystem
		PREFIX luafilesystem
		SOURCE_DIR ${DEP_SOURCE}/luafilesystem
		BINARY_DIR luafilesystem
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

		# Build lualogging
	ExternalProject_Add ( dep_lualogging
		PREFIX lualogging
		SOURCE_DIR ${DEP_SOURCE}/lualogging
		BINARY_DIR lualogging
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

	# Ensure correct order of build (dependencies between external dependencies)
	if( MSVC OR APPLE )
		add_dependencies( dep_lpeg dep_lua )
		add_dependencies( dep_leg dep_lpeg )
		add_dependencies( dep_luametrics dep_leg )
		add_dependencies( dep_luafilesystem dep_lua )
	else()
		add_dependencies( dep_lpeg dep_luajit )
		add_dependencies( dep_diluculum dep_luajit )
		add_dependencies( dep_leg dep_lpeg )
		add_dependencies( dep_luametrics dep_leg )
		add_dependencies( dep_luafilesystem dep_luajit )
	endif()

	# path to include directory where external dependencies deploy headers
	set( DEPENDENCIES_INCLUDE_DIR ${DEP_BIN}/include )

else()
	# Find Lua implementation, assuming lpeg is installed by other means
	#find_package( Lua51 REQUIRED )
endif()

# -----------------------------------------------------------------
# Platform settings
# -----------------------------------------------------------------

# Apple specific overrides, we build and app bundle
option( OSX_BUNDLE "Create an app bundle on OSX." OFF ) # TODO: disabled, because currently it does not work...
if( APPLE AND OSX_BUNDLE )
	# Executable settings
	set( 3DSOFTVIZ_EXECUTABLE_TYPE MACOSX_BUNDLE )
	set( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${3DSOFTVIZ_NAME}.app" )

	# Override default install destinations into the bundle
	set( INSTALL_BUNDLE . )
	set( INSTALL_BIN	${3DSOFTVIZ_NAME}.app/Contents/MacOS )
	set( INSTALL_DATA	${3DSOFTVIZ_NAME}.app/Contents/share/3DSOFTVIZ )
	set( INSTALL_RES	${3DSOFTVIZ_NAME}.app/Contents/Resources )
	set( INSTALL_DEPS	${3DSOFTVIZ_NAME}.app/Contents )
	set( INSTALL_PLUGIN	${3DSOFTVIZ_NAME}.app/Contents/MacOS )
	set( INSTALL_QTCONF	${3DSOFTVIZ_NAME}.app/Contents/Resources )

	# Bundle settings
	set( MACOSX_BUNDLE_INFO_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_BUNDLE_VERSION "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_LONG_VERSION_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_SHORT_VERSION_STRING "${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_COPYRIGHT "${3DSOFTVIZ_COPYRIGHT_YEAR} ${3DSOFTVIZ_VENDOR}" )
	set( MACOSX_BUNDLE_ICON_FILE "3DSOFTVIZ_app.icns" )# use http://iconverticons.com/legacy/ for icon converting
	set( MACOSX_BUNDLE_GUI_IDENTIFIER "${3DSOFTVIZ_DOMAIN_SECOND}.${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_NAME}" )
	set( MACOSX_BUNDLE_BUNDLE_NAME "${3DSOFTVIZ_NAME}" )

	# CPack settings
	set( CPACK_GENERATOR "DragNDrop" )
	set( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/3DSOFTVIZ_pkg.icns" )
	set( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )

	# OSX Specific resources
	install( FILES "resources/${MACOSX_BUNDLE_ICON_FILE}" DESTINATION ${INSTALL_RES} )
endif()

# Windows specific overrides
if( WIN32 )
	# Executable settings
	set( 3DSOFTVIZ_EXECUTABLE_TYPE WIN32 )
	set( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/${3DSOFTVIZ_NAME}.exe" )

	# CPack settings
	set( CPACK_GENERATOR "NSIS" )
	set( CPACK_PACKAGE_INSTALL_DIRECTORY "${3DSOFTVIZ_NAME}" )
	set( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )
	set( CPACK_PACKAGE_EXECUTABLES "${3DSOFTVIZ_NAME}" "${3DSOFTVIZ_NAME}" )
	set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}\\\\LICENSE.txt" )

	# NSIS branding
	set( CPACK_NSIS_INSTALLED_ICON_NAME "${INSTALL_BIN}\\\\${3DSOFTVIZ_NAME}${CMAKE_EXECUTABLE_SUFFIX}" )
	set( CPACK_NSIS_DISPLAY_NAME "${3DSOFTVIZ_NAME}" )
	set( CPACK_NSIS_HELP_LINK "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
	set( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
	set( CPACK_NSIS_CONTACT "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
	set( CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\3DSOFTVIZ_pkg.ico" )
	set( CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\3DSOFTVIZ_rmv.ico" )
	set( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\logo2.bmp" )
endif()

# ---------------------------------------------------------------
# 3DSoftviz sources
# ---------------------------------------------------------------

# Include Paths
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

if( OpenCV_FOUND )
	include_directories( SYSTEM
		${DEPENDENCIES_INCLUDE_DIR}/aruco
	)
endif()

include_directories( SYSTEM
	${DEPENDENCIES_INCLUDE_DIR}
	${DEPENDENCIES_INCLUDE_DIR}/libnoise
)

if( LEAP_FOUND )
	include_directories( SYSTEM
		${LEAP_INCLUDE_DIRS}
	)
endif()

#-------------------------------- Source files -------------------------------
set( SRC_MAIN "src/main.cpp" )
file( GLOB_RECURSE SRC_CORE		"src/Core/*.cpp" )
file( GLOB_RECURSE SRC_DATA		"src/Data/*.cpp" )
file( GLOB_RECURSE SRC_IMPORTER	"src/Importer/*.cpp" )
file( GLOB_RECURSE SRC_LAYOUT	"src/Layout/*.cpp" )
file( GLOB_RECURSE SRC_LUAGRAPH  "src/LuaGraph/*.cpp")
file( GLOB_RECURSE SRC_LUAINTERFACE  "src/LuaInterface/*.cpp")
file( GLOB_RECURSE SRC_MANAGER	"src/Manager/*.cpp" )
file( GLOB_RECURSE SRC_MATH		"src/Math/*.cpp" )
file( GLOB_RECURSE SRC_MODEL	"src/Model/*.cpp" )
file( GLOB_RECURSE SRC_NETWORK	"src/Network/*.cpp" )
file( GLOB_RECURSE SRC_OSGQTBROWSER	"src/OsgQtBrowser/*.cpp" )
file( GLOB_RECURSE SRC_QOSG		"src/QOSG/*.cpp" )
file( GLOB_RECURSE SRC_UTIL		"src/Util/*.cpp" )
file( GLOB_RECURSE SRC_VIEWER	"src/Viewer/*.cpp" )
file( GLOB_RECURSE SRC_CLUSTERING	"src/Clustering/*.cpp")
file( GLOB SRC_REPOSITORY		"src/Repository/Git/*.cpp" )
file( GLOB_RECURSE SRC_GITLIB   "src/Repository/Git/GitLib/*.cpp" )

# OpenCV
file( GLOB_RECURSE SRC_OPENCV	"src/OpenCV/*.cpp" )
file( GLOB_RECURSE SRC_QOPENCV	"src/QOpenCV/*.cpp" )

# Aruco
file( GLOB_RECURSE SRC_ARUCO	"src/Aruco/*.cpp" )

# Kinect
file( GLOB_RECURSE SRC_KINECT	"src/Kinect/*.cpp" )

# SpeechSDK
file( GLOB_RECURSE SRC_SPEECH	"src/Speech/*.cpp" )

# FGlove
# file ( GLOB_RECURSE SRC_FGLOVE "src/FGlove/*.cpp" )

# Leap
file ( GLOB_RECURSE SRC_LEAP "src/Leap/*.cpp" )
# MOUSE3D
file ( GLOB_RECURSE SRC_MOUSE3D "src/Mouse3D/*.cpp" )


# All source files
set( SRC
	${SRC_MAIN} ${SRC_CORE}
	${SRC_MANAGER}
	${SRC_DATA} ${SRC_CLUSTERING}
	${SRC_IMPORTER}
	${SRC_LAYOUT}
	${SRC_LUAGRAPH} ${SRC_LUAINTERFACE}
	${SRC_MATH}
	${SRC_NETWORK}
	${SRC_MODEL}
	${SRC_OSGQTBROWSER} ${SRC_QOSG} ${SRC_VIEWER}
	${SRC_UTIL}
	${SRC_REPOSITORY}
)

if( OpenCV_FOUND )
	list( APPEND SRC ${SRC_OPENCV} ${SRC_QOPENCV} ${SRC_ARUCO} )
endif()

# add Kinect if Nite, Openni and Opencv
if( OPENNI2_FOUND AND OpenCV_FOUND )
	list( APPEND SRC ${SRC_KINECT} )
endif()

if( KINECTSDK_FOUND AND SPEECHSDK_FOUND )
	list( APPEND SRC ${SRC_SPEECH} )
endif()

# add Gloves if Fgloves
# if( FGLOVE_FOUND )
#	list( APPEND SRC ${SRC_FGLOVE} )
# endif()

# add Leap
if( LEAP_FOUND )
	list( APPEND SRC ${SRC_LEAP} )
endif()

# add MOUSE3D if found
if( MOUSE3D_FOUND )
	list( APPEND SRC ${SRC_MOUSE3D} )
endif()



#-------------------------------- Header files -------------------------------
file( GLOB_RECURSE HEADER_CORE		"include/Core/*.h" )
file( GLOB_RECURSE HEADER_DATA		"include/Data/*.h" )
file( GLOB_RECURSE HEADER_IMPORTER	"include/Importer/*.h" )
file( GLOB_RECURSE HEADER_LAYOUT	"include/Layout/*.h" )
file( GLOB_RECURSE HEADER_LUAGRAPH  "include/LuaGraph/*.h")
file( GLOB_RECURSE HEADER_LUAINTERFACE  "include/LuaInterface/*.h")
file( GLOB_RECURSE HEADER_MANAGER	"include/Manager/*.h" )
file( GLOB_RECURSE HEADER_MATH		"include/Math/*.h" )
file( GLOB_RECURSE HEADER_MODEL		"include/Model/*.h" )
file( GLOB_RECURSE HEADER_NETWORK	"include/Network/*.h" )
file( GLOB_RECURSE HEADER_OSGQTBROWSER	"include/OsgQtBrowser/*.h" )
file( GLOB_RECURSE HEADER_QOSG		"include/QOSG/*.h" )
file( GLOB_RECURSE HEADER_UTIL		"include/Util/*.h" )
file( GLOB_RECURSE HEADER_VIEWER	"include/Viewer/*.h" )
file( GLOB_RECURSE HEADER_CLUSTERING	"include/Clustering/*.h")
file( GLOB HEADER_REPOSITORY		"include/Repository/Git/*.h" )
file( GLOB_RECURSE HEADER_GITLIB		"include/Repository/Git/GitLib/*.h" )

# OpenCV
file( GLOB_RECURSE HEADER_OPENCV	"include/OpenCV/*.h" )
file( GLOB_RECURSE HEADER_QOPENCV	"include/QOpenCV/*.h" )

# Aruco
file( GLOB_RECURSE HEADER_ARUCO		"include/Aruco/*.h" )

# Kinect
file( GLOB_RECURSE HEADER_KINECT	"include/Kinect/*.h" )

# SpeechSDK
file( GLOB_RECURSE HEADER_SPEECH	"include/Speech/*.h" )

# FGLOVE
# file( GLOB_RECURSE HEADER_FGLOVE	"include/FGlove/*.h" )

# LEAP
file( GLOB_RECURSE HEADER_LEAP	"include/Leap/*.h" )

# MOUSE3D
file( GLOB_RECURSE HEADER_MOUSE3D	"include/Mouse3D/*.h" )

# All header files
set( INCL
	${HEADER_MAIN} ${HEADER_CORE}
	${HEADER_MANAGER}
	${HEADER_DATA} ${HEADER_CLUSTERING}
	${HEADER_IMPORTER}
	${HEADER_LAYOUT}
	${HEADER_LUAGRAPH} ${HEADER_LUAINTERFACE}
	${HEADER_MATH}
	${HEADER_NETWORK}
	${HEADER_MODEL}
	${HEADER_OSGQTBROWSER} ${HEADER_QOSG} ${HEADER_VIEWER}
	${HEADER_UTIL}
	${HEADER_REPOSITORY}
)

if( OpenCV_FOUND )
	list( APPEND INCL ${HEADER_OPENCV} ${HEADER_QOPENCV} ${HEADER_ARUCO} )
endif()

#Kinect use Nite, Openni and OpenCV
if( OPENNI2_FOUND AND OpenCV_FOUND )
	list( APPEND INCL ${HEADER_KINECT} )
endif()

if( KINECTSDK_FOUND AND SPEECHSDK_FOUND )
	list( APPEND INCL ${HEADER_SPEECH} )
endif()

#FGLOVE
# if( FGLOVE_FOUND )
#	list( APPEND INCL ${HEADER_FGLOVE} )
# endif()

#LEAP
if( LEAP_FOUND )
	list( APPEND INCL ${HEADER_LEAP} )
endif()

#MOUSE3D
if( MOUSE3D_FOUND )
	list( APPEND INCL ${HEADER_MOUSE3D} )
endif()

#-------------------------------------- Group source files --------------------------------------
source_group( \\src main.cpp )
source_group( \\src\\Network "^.*Network/.*$" )
source_group( \\src\\Viewer "^.*Viewer/.*$" )
source_group( \\src\\Core "^.*Core/.*$" )
source_group( \\src\\Data "^.*Data/.*$" )
source_group( \\src\\Manager "^.*Manager/.*$" )
source_group( \\src\\Importer "^.*Importer/.*$" )
source_group( \\src\\Model "^.*Model/.*$" )
source_group( \\src\\Util "^.*Util/.*$" )
source_group( \\src\\Layout "^.*Layout/.*$" )
source_group( \\src\\QOSG "^.*QOSG/.*$" )
source_group( \\src\\OsgQtBrowser "^.*OsgQtBrowser/.*$" )
source_group( \\src\\LuaGraph "^.*LuaGraph/.*$")
source_group( \\src\\LuaInterface "^.*LuaInterface/.*$")
source_group( \\src\\Git "^.*Git/.*$" )

# Group header files
source_group( \\headers\\Clustering "^.*Clustering/.*h$")
source_group( \\headers\\Network "^.*Network/.*h$" )
source_group( \\headers\\Viewer "^.*Viewer/.*h$" )
source_group( \\headers\\Core "^.*Core/.*h$" )
source_group( \\headers\\Data "^.*Data/.*h$" )
source_group( \\headers\\Manager "^.*Manager/.*h$" )
source_group( \\headers\\Importer "^.*Importer/.*h$" )
source_group( \\headers\\Model "^.*Model/.*h$" )
source_group( \\headers\\Util "^.*Util/.*h$" )
source_group( \\headers\\Layout "^.*Layout/.*h$" )
source_group( \\headers\\QOSG "^.*QOSG/.*h$" )
source_group( \\headers\\OsgQtBrowser "^.*OsgQtBrowser/.*h$" )
source_group( \\headers\\LuaGraph "^.*LuaGraph/.*h$")
source_group( \\headers\\LuaInterface "^.*LuaInterface/.*h$")
source_group( \\headers\\Git "^.*Git/.*h$" )

if( OpenCV_FOUND )
	source_group( \\src\\QOpenCV "^.*QOpenCV/.*$" )
	source_group( \\src\\OpenCV "^.*OpenCV/.*$" )
	source_group( \\headers\\QOpenCV "^.*QOpenCV/.*h$" )
	source_group( \\headers\\OpenCV "^.*OpenCV/.*h$" )
	source_group( \\src\\Aruco "^.*Aruco/.*$" )
	source_group( \\headers\\Aruco "^.*Aruco/.*h$" )
endif()

if( NITE2_FOUND AND OPENNI2_FOUND AND OpenCV_FOUND )
	source_group( \\src\\Kinect "^.*Kinect/.*$" )
	source_group( \\headers\\Kinect "^.*Kinect/.*h$" )
endif()

if( KINECTSDK_FOUND AND SPEECHSDK_FOUND )
	source_group( \\src\\Speech "^.*Speech/.*$" )
	source_group( \\headers\\Speech "^.*Speech/.*h$" )
endif()

#fglove
# if( FGLOVE_FOUND )
#	source_group( \\src\\Fglove "^.*Fglove/.*$" )
#	source_group( \\headers\\Fglove "^.*Fglove/.*h$" )
# endif()

#leap
if( LEAP_FOUND )
	source_group( \\src\\Leap "^.*Leap/.*$" )
	source_group( \\headers\\Leap "^.*Leap/.*h$" )
endif()

#MOUSE3D
if( MOUSE3D_FOUND )
	source_group( \\src\\Mouse3D "^.*Mouse3D/.*$" )
	source_group( \\headers\\Mouse3D "^.*Mouse3D/.*h$" )
endif()

# add diluculum code directly due to MSVC problems
if( MSVC )
	include_directories( SYSTEM dependencies/diluculum/include )
	#include_directories( SYSTEM $ENV{CMAKE_INCLUDE_PATH} )

	file( GLOB_RECURSE SRC_DILUCULUM	"dependencies/diluculum/Sources/*.cpp" )
endif()

# ---------------------------------------------------------------
# 3rd party sources
# ---------------------------------------------------------------

add_library( noiseutils STATIC dependencies/noiseutils/noiseutils.cpp )
target_include_directories( noiseutils SYSTEM PUBLIC dependencies/noiseutils  )

add_library( qtcolorpicker STATIC dependencies/qtcolorpicker/qtcolorpicker.cpp )
target_include_directories( noiseutils SYSTEM PUBLIC dependencies/qtcolorpicker )

# ---------------------------------------------------------------
# Build modules as libraries
# ---------------------------------------------------------------

add_library( gitlib SHARED ${SRC_GITLIB} ${HEADER_GITLIB})
target_include_directories( gitlib SYSTEM PUBLIC include/Repository/Git )
target_link_libraries( gitlib ${QT_LIBRARIES} )

set(FLAGS_FOR_DEBUG2 ${FLAGS_FOR_DEBUG})
list(APPEND FLAGS_FOR_DEBUG2 " -DGITLIB_LIBRARY")
string(REPLACE " " ";" REPLACED_FLAGS ${FLAGS_FOR_DEBUG2})
target_compile_options( gitlib PUBLIC ${REPLACED_FLAGS} )

# ---------------------------------------------------------------
# Build and link 3Dsoftviz
# ---------------------------------------------------------------

if( MSVC )
	list( APPEND SRC ${SRC_DILUCULUM} )
endif()

# Build and link
add_executable( ${3DSOFTVIZ_NAME} ${INCL}  ${SRC} )

# set compile flags only for our code ( suppresses warnigns on system headers )
set_target_properties( ${3DSOFTVIZ_NAME} PROPERTIES COMPILE_FLAGS "${FLAGS_FOR_DEBUG}" )

target_link_libraries( ${3DSOFTVIZ_NAME}
	gitlib
	lua
	noiseutils
	libnoise
	osgModeling
	qtcolorpicker
	${QT_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
)

if( NOT MSVC )
	target_link_libraries( ${3DSOFTVIZ_NAME} diluculum
	)
endif()

if( OpenCV_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OpenCV_LIBS} aruco )
endif()

if( OPENNI2_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OPENNI2_LIBRARIES} )
endif()

if( NITE2_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${NITE2_LIBRARIES} )
endif()


if( KINECTSDK_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${KINECTSDK_LIBRARIES} )
	if( SPEECHSDK_FOUND )
		target_link_libraries( ${3DSOFTVIZ_NAME} ${SPEECHSDK_LIBRARIES} )
	endif()
endif()

# if( FGLOVE_FOUND )
#	target_link_libraries( ${3DSOFTVIZ_NAME} ${FGLOVE_LIBRARIES} )
# endif()

if( LEAP_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${LEAP_LIBRARIES} )
endif()

if( MOUSE3D_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${MOUSE3D_LIBRARIES} )
endif()

# 'cotire' the executable
if( COMMAND cotire )
	# link ${3DSOFTVIZ_NAME}_unity target to all libraries to which ${3DSOFTVIZ_NAME} links
	set_target_properties( ${3DSOFTVIZ_NAME} PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY" )
	set_target_properties( gitlib PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY" )

	cotire( ${3DSOFTVIZ_NAME} CONFIGURATIONS Debug Release )
	cotire( gitlib CONFIGURATIONS Debug Release )
endif()

# Ensure correct dependencies
if( USE_BUILTIN_DEPENDENCIES )

	# dependencies of non-unity build
    add_dependencies( ${3DSOFTVIZ_NAME} libnoise osgModeling gitlib dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging)

	if( MSVC OR APPLE )
		add_dependencies( ${3DSOFTVIZ_NAME} dep_lua )
	else()
		add_dependencies( ${3DSOFTVIZ_NAME} dep_diluculum dep_luajit )
	endif()

	if( OpenCV_FOUND )
		add_dependencies( ${3DSOFTVIZ_NAME} aruco )
	endif()

	# dependencies of unity build
	if (COMMAND cotire)
		add_dependencies( ${3DSOFTVIZ_NAME}_unity libnoise osgModeling gitlib_unity dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging)

		if( MSVC OR APPLE )
			add_dependencies( ${3DSOFTVIZ_NAME}_unity dep_lua )
		else()
			add_dependencies( ${3DSOFTVIZ_NAME}_unity dep_diluculum dep_luajit )
		endif()

		if( OpenCV_FOUND )
			add_dependencies( ${3DSOFTVIZ_NAME}_unity aruco )
		endif()
	endif()
endif()

# Ensure correct dependencies
#add_dependencies( ${3DSOFTVIZ_NAME} libnoise )
#
#if( OpenCV_FOUND )
	#add_dependencies( ${3DSOFTVIZ_NAME} aruco )
#endif()

# -------------------------------------------------------
# Install
# -------------------------------------------------------

# 3DSoftviz and data
install( TARGETS ${3DSOFTVIZ_NAME}
	BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

install( TARGETS gitlib
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime
	LIBRARY DESTINATION ${INSTALL_LIB} COMPONENT Runtime )

# add custom install_unity target
if( COMMAND cotire )
	add_custom_target( install_unity
		COMMAND ${CMAKE_COMMAND} -P "cmake_install.cmake" )
	add_dependencies( install_unity ${3DSOFTVIZ_NAME}_unity )
	add_dependencies( install_unity gitlib_unity )
endif()

# install lua modules
install( DIRECTORY resources/scripts/module/ DESTINATION ${INSTALL_LIB}/lua COMPONENT Data )

# install resources
install( DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data PATTERN resources/scripts/module EXCLUDE)

# Install needed .dll files for OpenNI2
if( NITE2_FOUND AND OPENNI2_FOUND )
	install( DIRECTORY ${NITE2_OPENNI2_DRIVER_DIRS} DESTINATION ${INSTALL_BIN}/OpenNI2 )
endif()

# install .dll for speech recognition
if( SPEECHSDK_FOUND )
	install( DIRECTORY ${SPEECHSDK_DRIVER_DIRS}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/bin )
endif()

# Install needed .dll files for fglove
# if( FGLOVE_FOUND )
#
#	install( DIRECTORY ${FGLOVE_DRIVER_DIRS}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/bin )
#
#	install( FILES ${FGLOVE_LIBRARIES} DESTINATION ${INSTALL_BIN}
#			RUNTIME DESTINATION ${INSTALL_BIN}
#			LIBRARY DESTINATION ${INSTALL_LIB} )
# endif()

# Install needed .dll files for leap
if( LEAP_FOUND )

	install( DIRECTORY ${LEAP_DRIVER_DIRS}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/bin )

	#install( FILES ${LEAP_LIBRARIES} DESTINATION ${INSTALL_BIN}
#			RUNTIME DESTINATION ${INSTALL_BIN}
#   		LIBRARY DESTINATION ${INSTALL_LIB} )
endif()

# Install built in dependencies unless we are building an OSX Bundle
if( USE_BUILTIN_DEPENDENCIES )
	# Install all dependencies
	install( DIRECTORY ${DEP_BIN}/
		DESTINATION ${INSTALL_DEPS}
		COMPONENT Dependencies
		USE_SOURCE_PERMISSIONS
	)
endif()

# Include QT4 libraries in Apple bundle and on Windows
if( WIN32 OR APPLE AND OSX_BUNDLE )
	# On Apple plugins need to be bundled
	if( APPLE AND OSX_BUNDLE )
		# Install Qt Plugins
		install( DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
			DESTINATION ${INSTALL_PLUGIN}/plugins
			COMPONENT Runtime )

		# Install Qt Config
		install( CODE "
			file( WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF}/qt.conf\" \"\" )
			" COMPONENT Runtime )
	endif()

	# Search Dirs
	string( REPLACE "/lib" "/bin" QT_RUNTIME_DIR ${QT_LIBRARY_DIR} )
	set( BUNDLE_DIRS ${QT_LIBRARY_DIR} ${QT_RUNTIME_DIR} ${DEP_BIN}/bin ${DEP_BIN}/lib )


	# Bundle libraries
	install( CODE "
		file( GLOB_RECURSE LUA_PLUGINS
				\"\${CMAKE_INSTALL_PREFIX}/${INSTALL_DEPS}/lib/lua/*${CMAKE_SHARED_MODULE_SUFFIX}\" )
		file( GLOB_RECURSE BUNDLE_PLUGINS
				\"\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
		include( BundleUtilities )
		fixup_bundle( \"${BUNDLE_APP}\" \"\${BUNDLE_PLUGINS};\${LUA_PLUGINS}\" \"${BUNDLE_DIRS}\" )
		" COMPONENT Runtime )
endif()

# -------------------------------------------------------
# Packing
# -------------------------------------------------------

set( CPACK_MONOLITHIC_INSTALL 1 )
set( CPACK_STRIP_FILES ON )
set( CPACK_BINARY_DRAGNDROP ON )
set( CPACK_PACKAGE_VERSION_MAJOR "${3DSOFTVIZ_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR "${3DSOFTVIZ_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH "${3DSOFTVIZ_VERSION_PATCH}" )
set( CPACK_PACKAGE_VERSION "${3DSOFTVIZ_VERSION}" )
set( CPACK_PACKAGE_VENDOR "${3DSOFTVIZ_VENDOR}" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${3DSOFTVIZ_README}" )
#set( CPACK_COMPONENTS_ALL Runtime Dependencies Data )
include( CPack )


# -------------------------------------------------------
# BDD Igloo tests
# -------------------------------------------------------

file( GLOB_RECURSE SRC_TESTS	"tests/*.cpp" )
add_executable(all_bdd_tests ${SRC_TESTS})
set_target_properties( all_bdd_tests
	PROPERTIES
		COMPILE_FLAGS "${FLAGS_FOR_DEBUG}"
	)
target_include_directories( all_bdd_tests
	SYSTEM PUBLIC
        include/Repository/Git
	)

target_link_libraries( all_bdd_tests
	${QT_LIBRARIES}
    gitlib
)

add_dependencies( all_bdd_tests igloo)


enable_testing()
add_test(all_bdd_tests all_bdd_tests)

