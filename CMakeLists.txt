# Copyright ( C ) 2013-2016 3DSoftviz.
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
set( 3DSOFTVIZ_COPYRIGHT_YEAR "2016" )
set( 3DSOFTVIZ_DOMAIN_FIRST "stuba" )
set( 3DSOFTVIZ_DOMAIN_SECOND "sk" )
set( 3DSOFTVIZ_DOMAIN "${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_DOMAIN_SECOND}" )

# --------------
# Build settings
# --------------

# Additional CMake scripts
set( CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
	${CMAKE_MODULE_PATH} )

# Enable C++11 features
set(CMAKE_CXX_STANDARD 11)

# Warnings for Debug mode
option( USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON )
if( USE_STRICT_COMPILE_WARNINGS )
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		# using Clang
		set( FLAGS_FOR_DEBUG
			-Weverything
			-Wno-c++98-compat -Wno-documentation -Wno-shadow -Wno-padded -Wno-covered-switch-default
			-pedantic -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal  -Wformat  -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wformat=2  -Wimport  -Winit-self -Winline -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked  -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare  -Wsign-promo -Wstack-protector -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings  -Wno-unused-parameter -Wno-unused -Wno-documentation-unknown-command )
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR CMAKE_COMPILER_IS_GNUCXX)
		# using GCC
		set( FLAGS_FOR_DEBUG -pedantic -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Wimport -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wlong-long -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare -Wsign-promo -Wstack-protector -Wstrict-aliasing=2 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wold-style-cast -Woverloaded-virtual -Wuseless-cast -Wsign-conversion -Wno-unused-parameter -Wno-unused )

		# to suppress warnings for unused stuff use: -Wno-unused-parameter -Wno-unused-result
		# not used:
		# too much warnings in Qt headers: NULL vs 0 pointers: -Wzero-as-null-pointer-constant
		# too much warnings: -Wshadow -Waggregate-return
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
		message(STATUS "GNU gold linker isn't available, using the default system linker.")
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
			# check for physical memmory
			cmake_host_system_information(RESULT PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
			math(EXPR PHYSICAL_MEMORY_GB "${PHYSICAL_MEMORY}/1024")

			# if physical memmory is less than 4GB, then split unity builds
			# note: less than 4GB RAM causes swapping
			if(${PHYSICAL_MEMORY_GB} LESS "4")
				math(EXPR JOBS "${N}*2")
			else()
				set(JOBS ${N})
			endif()

			# set number of parallel unity builds to processor count
			message(STATUS "Cotire parallel unity jobs: ${JOBS}")
			set( COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j${JOBS}" )
		endif()
	endif()

	set( COTIRE_VERBOSE FALSE )
	set( COTIRE_DEBUG FALSE )

	### Fix virtual memory range
	if( MSVC )
		set (COTIRE_PCH_MEMORY_SCALING_FACTOR "280")
	endif()

	# fix for Diluculum and LuaJIT on 64bit OSX: Lua new_state returned NULL, LuaJIT needs following flags
	#if( APPLE )
	#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "" FORCE)
	#endif()
endif()

# ------------------------------------------------------------
# Dependencies - Searching external dependencies
# ------------------------------------------------------------

# ------------------------------ Qt -------------------------------
set( ALL_SYSTEM_HEADERS )
set( ALL_COMPILE_DEFINITIONS )

set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )


# Find Qt4
find_package( Qt4 REQUIRED )
set( QT_USE_QTOPENGL 1 )
set( QT_USE_QTSQL 1 )
set( QT_USE_QTWEBKIT 1 )
set( QT_USE_QTXML 1 )
set( QT_USE_QTNETWORK 1 )
include( ${QT_USE_FILE} )

if ( NOT Qt4_FOUND )
    # Find Qt5
    find_package( Qt5Core QUIET )
    find_package( Qt5Widgets QUIET )
    find_package( Qt5OpenGL QUIET )
    find_package( Qt5Sql QUIET )
    find_package( Qt5WebEngine QUIET )
    find_package( Qt5WebEngineWidgets QUIET )
    find_package( Qt5Xml QUIET )
    find_package( Qt5Network QUIET )
endif()

if ( Qt5Core_FOUND )
	message(STATUS "Using Qt5 packages")
elseif( Qt4_FOUND )
	message(STATUS "Using Qt4 packages")
else()
	message(STATUS "No suitable Qt packages found")
endif()

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

find_package( osgQt )

list( APPEND ALL_SYSTEM_HEADERS
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}
	${OSGQT_INCLUDE_DIR}
)

if( MSVC )
	list( APPEND ALL_COMPILE_DEFINITIONS -DOSG_LIBRARY_STATIC )
endif()

# ------------------------------ OpenCV -------------------------------
# Find OpenCV
find_package( OpenCV )
if( OpenCV_FOUND )
	if( WIN32 )
		list( APPEND ALL_COMPILE_DEFINITIONS -DNOMINMAX )
	endif()

	#list( APPEND ALL_SYSTEM_HEADERS	${OpenCV_INCLUDE_DIRS} )
	list( APPEND ALL_COMPILE_DEFINITIONS -DOPENCV_FOUND )

	message( STATUS "OpenCV Found:" )
	message( STATUS "OpenCV_LIB_DIR=${OpenCV_LIB_DIR}" )
	message( STATUS "OpenCV_LIB_DIR=${OpenCV_LIBS}" )
else()
	message( STATUS "OpenCV NOT FOUND" )
endif()

# ------------------------------ OpenNI2 and Nite -------------------------------
# Find OPENNI2
find_package( OPENNI2 )
if( OPENNI2_FOUND )

	list( APPEND ALL_SYSTEM_HEADERS	${OPENNI2_INCLUDE_DIRS} )

	list( APPEND ALL_COMPILE_DEFINITIONS  -DOPENNI2_FOUND )

	message( STATUS "OpenNI2 Found:" )
	message( STATUS "OPENNI2_INCLUDE_DIRS=${OPENNI2_INCLUDE_DIRS}" )
	message( STATUS "OPENNI2_LIBRARIES=${OPENNI2_LIBRARIES}" )
else( OPENNI2_FOUND )
	message( STATUS "OpenNI2 NOT FOUND" )
endif()

# Find NITE2
find_package( NITE2 )
if( NITE2_FOUND )

	list( APPEND ALL_SYSTEM_HEADERS	${NITE2_INCLUDE_DIRS} ${NITE2_OPENNI2_DRIVER_DIRS} )

	list( APPEND ALL_COMPILE_DEFINITIONS -DNITE2_FOUND )
	message( STATUS "NITE2 Found:" )
	message( STATUS "NITE2_INCLUDE_DIRS=${NITE2_INCLUDE_DIRS}" )
	message( STATUS "NITE2_LIBRARIES=${NITE2_LIBRARIES}" )
else( NITE2_FOUND )
	message ( STATUS "NITE2 NOT FOUND" )
endif()
# ------------------------------ FREENECT2 -------------------------------
find_package( FREENECT2 )
if( FREENECT2_FOUND )

	list( APPEND ALL_SYSTEM_HEADERS	${FREENECT2_INCLUDE_DIRS} )

	list( APPEND ALL_COMPILE_DEFINITIONS  -DFREENECT2_FOUND )

	message( STATUS "FREENECT2 Found:" )
	message( STATUS "FREENECT2_INCLUDE_DIRS=${FREENECT2_INCLUDE_DIRS}" )
	message( STATUS "FREENECT2_LIBRARIES=${FREENECT2_LIBRARIES}" )
else( FREENECT2_FOUND )
	message( STATUS "FREENECT2 NOT FOUND" )
endif()


# ------------------------------- KINECT SDK ----------------------------------
if( WIN32 AND MSVC )

	find_package( KINECTSDK )
	if( KINECTSDK_FOUND )

		list( APPEND ALL_SYSTEM_HEADERS	${KINECTSDK_INCLUDE_DIRS} )

		list( APPEND ALL_COMPILE_DEFINITIONS -DKINECTSDK_FOUND )

		# find_package( SPEECHSDK )
		# if( SPEECHSDK_FOUND )
		# include_directories( SYSTEM ${SPEECHSDK_INCLUDE_DIRS} )
		# add_definitions( -DSPEECHSDK_FOUND )
		# message( STATUS "SPEECHSDK FOUND" )
		# else( SPEECHSDK_FOUND )
		# message ( STATUS "SPEECHSDK NOT FOUND" )
		# endif()

		message( STATUS "KINECTSDK Found:" )
		message( STATUS "KINECTSDK_INCLUDE_DIRS=${KINECTSDK_INCLUDE_DIRS}" )
		message( STATUS "KINECTSDK_LIBRARIES=${KINECTSDK_LIBRARIES}" )
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
list( APPEND ALL_SYSTEM_HEADERS	${Boost_INCLUDE_DIRS}  )

# ------------------------------ LEAP -------------------------------
#enable leap orion on windows
if( WIN32 )
	set( USE_LEAP_ORION ON )
endif()

# Find leap
find_package( LEAP )
if( LEAP_FOUND )

	list( APPEND ALL_COMPILE_DEFINITIONS -DLEAP_FOUND )

	list( APPEND ALL_SYSTEM_HEADERS	${LEAP_INCLUDE_DIRS} )

	message( STATUS "LEAP Found:" )
	message( STATUS "LEAP_INCLUDE_DIRS=${LEAP_INCLUDE_DIRS}" )
	message( STATUS "LEAP_LIB_DIR=${LEAP_LIBRARIES}" )
else()
	message( STATUS "LEAP Not Found" )
endif()
# ------------------------------ PCL ----------------------------------
# find PCL
find_package(PCL 1.8)
if ( PCL_FOUND )
        list( APPEND ALL_SYSTEM_HEADERS	${PCL_INCLUDE_DIRS} )
	link_directories(${PCL_LIBRARY_DIRS})
	add_definitions(${PCL_DEFINITIONS})
	message( STATUS "PCL FOUND" )
else()
        message( STATUS "PCL NOT FOUND" )
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
if( WIN32 )
	if( MOUSE3D_FOUND )
		include_directories( SYSTEM ${MOUSE3D_INCLUDE_DIRS} )
		add_definitions( -DMOUSE3D_FOUND )
		message( STATUS "3DMouse FOUND" )
	endif()
elseif( APPLE )
	if( MOUSE3D_FOUND )
		include_directories( SYSTEM ${MOUSE3D_INCLUDE_DIRS} )
		add_definitions( -DMOUSE3D_FOUND )
		message( STATUS "3DMouse FOUND" )
	endif()
elseif( UNIX )
	set (MOUSE3D_FOUND TRUE)
	include_directories( SYSTEM include/Mouse3d/LibMouse3d/Unix )
	find_package( X11 REQUIRED )
	include_directories( SYSTEM ${X11_INCLUDE_DIR} )
	add_definitions( -DMOUSE3D_FOUND )
	message( STATUS "3DMouse FOUND" )
elseif()
	message( STATUS "3DMouse NOT FOUND" )
endif()


# ------------------------------ DOXYGEN -------------------------------
# Find Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
#message( STATUS "Doxygen found: " ${DOXYGEN_EXECUTABLE} )

if(BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	add_custom_target(doxygen
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

	install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/html)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()


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

	# Build StackTracePlus
	ExternalProject_Add ( dep_StackTracePlus
		PREFIX StackTracePlus
		SOURCE_DIR ${DEP_SOURCE}/StackTracePlus
		BINARY_DIR StackTracePlus
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luasocket
	ExternalProject_Add ( dep_luasocket
		PREFIX luasocket
		SOURCE_DIR ${DEP_SOURCE}/luasocket
		BINARY_DIR luasocket
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build mobdebug
	ExternalProject_Add ( dep_mobdebug
		PREFIX mobdebug
		SOURCE_DIR ${DEP_SOURCE}/mobdebug
		BINARY_DIR mobdebug
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luacov
	ExternalProject_Add ( dep_luacov
		PREFIX luacov
		SOURCE_DIR ${DEP_SOURCE}/luacov
		BINARY_DIR luacov
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luacheck
	ExternalProject_Add ( dep_luacheck
		PREFIX luacheck
		SOURCE_DIR ${DEP_SOURCE}/luacheck
		BINARY_DIR luacheck
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build lua_cliargs
	ExternalProject_Add ( dep_lua_cliargs
		PREFIX lua_cliargs
		SOURCE_DIR ${DEP_SOURCE}/lua_cliargs
		BINARY_DIR lua_cliargs
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luasystem
	ExternalProject_Add ( dep_luasystem
		PREFIX luasystem
		SOURCE_DIR ${DEP_SOURCE}/luasystem
		BINARY_DIR luasystem
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build dkjson
	ExternalProject_Add ( dep_dkjson
		PREFIX dkjson
		SOURCE_DIR ${DEP_SOURCE}/dkjson
		BINARY_DIR dkjson
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build say
	ExternalProject_Add ( dep_say
		PREFIX say
		SOURCE_DIR ${DEP_SOURCE}/say
		BINARY_DIR say
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luassert
	ExternalProject_Add ( dep_luassert
		PREFIX luassert
		SOURCE_DIR ${DEP_SOURCE}/luassert
		BINARY_DIR luassert
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build lua-term
	ExternalProject_Add ( dep_lua-term
		PREFIX lua-term
		SOURCE_DIR ${DEP_SOURCE}/lua-term
		BINARY_DIR lua-term
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build penlight
	ExternalProject_Add ( dep_penlight
		PREFIX penlight
		SOURCE_DIR ${DEP_SOURCE}/penlight
		BINARY_DIR penlight
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build mediator_lua
	ExternalProject_Add ( dep_mediator_lua
		PREFIX mediator_lua
		SOURCE_DIR ${DEP_SOURCE}/mediator_lua
		BINARY_DIR mediator_lua
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build busted
	ExternalProject_Add ( dep_busted
		PREFIX busted
		SOURCE_DIR ${DEP_SOURCE}/busted
		BINARY_DIR busted
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Build luacomments
	ExternalProject_Add ( dep_luacomments
		PREFIX luacomments
		SOURCE_DIR ${DEP_SOURCE}/luacomments
		BINARY_DIR luacomments
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)
		
	# Build luameg
	ExternalProject_Add ( dep_UMLtoSRC
		PREFIX UMLtoSRC
		SOURCE_DIR ${DEP_SOURCE}/UMLtoSRC
		BINARY_DIR UMLtoSRC
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		)

	# Ensure correct order of build (dependencies between external dependencies)
	if( MSVC OR APPLE )
		set( WHICH_LUA dep_lua )
	else()
		set( WHICH_LUA dep_luajit )
	endif()
	add_dependencies( dep_lpeg ${WHICH_LUA} )
	add_dependencies( dep_leg ${WHICH_LUA} )
	add_dependencies( dep_luametrics ${WHICH_LUA} )
	add_dependencies( dep_luafilesystem ${WHICH_LUA} )
	add_dependencies( dep_StackTracePlus ${WHICH_LUA} )
	add_dependencies( dep_luasocket ${WHICH_LUA} )
	add_dependencies( dep_mobdebug ${WHICH_LUA} )
	add_dependencies( dep_luacov ${WHICH_LUA} )
	add_dependencies( dep_luacheck ${WHICH_LUA} )
	add_dependencies( dep_lua_cliargs ${WHICH_LUA} )
	add_dependencies( dep_luasystem ${WHICH_LUA} )
	add_dependencies( dep_dkjson ${WHICH_LUA} )
	add_dependencies( dep_say ${WHICH_LUA} )
	add_dependencies( dep_luassert ${WHICH_LUA} )
	add_dependencies( dep_lua-term ${WHICH_LUA} )
	add_dependencies( dep_penlight ${WHICH_LUA} )
	add_dependencies( dep_mediator_lua ${WHICH_LUA} )
	add_dependencies( dep_busted ${WHICH_LUA} )
	add_dependencies( dep_luacomments ${WHICH_LUA} )
	add_dependencies( dep_UMLtoSRC ${WHICH_LUA} )
	if( NOT MSVC )
		add_dependencies( dep_diluculum ${WHICH_LUA} )
	endif()


	# path to include directory where external dependencies deploy headers
	set( DEPENDENCIES_INCLUDE_DIR ${DEP_BIN}/include )

else()
	# Find Lua implementation, assuming lpeg is installed by other means
	#find_package( Lua51 REQUIRED )
endif()


# ---------------------------------------------------------------
# 3DSoftviz sources
# ---------------------------------------------------------------

#-------------------------------- Source files -------------------------------
set( SRC_MAIN "src/main.cpp" )
file( GLOB_RECURSE SRC_APP		"src/Application/*.cpp" )
file( GLOB_RECURSE SRC_CORE		"src/Core/*.cpp" )
file( GLOB_RECURSE SRC_DATA		"src/Data/*.cpp" )
file( GLOB_RECURSE SRC_IMPORTER	"src/Importer/*.cpp" )
file( GLOB_RECURSE SRC_LAYOUT	"src/Layout/*.cpp" )
file( GLOB_RECURSE SRC_LUAGRAPH  "src/LuaGraph/*.cpp")
file( GLOB_RECURSE SRC_LUAINTERFACE  "src/LuaInterface/*.cpp")
file( GLOB_RECURSE SRC_LUATYPES  "src/LuaTypes/*.cpp")
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
file( GLOB_RECURSE SRC_SPECIALMATRIX   "src/SpecialMatrix/*.cpp" )
file( GLOB_RECURSE SRC_CITY		"src/City/*.cpp" )
file( GLOB_RECURSE SRC_SHAPES	"src/Shapes/*.cpp" )
file( GLOB_RECURSE SRC_MODULEGRAPH	"src/ModuleGraph/*.cpp" )

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
file ( GLOB SRC_LEAP "src/Leap/*.cpp" )
file ( GLOB_RECURSE SRC_LEAPLIB "src/Leap/LeapLib/*.cpp" )
file ( GLOB_RECURSE SRC_HANDMODULE "src/Leap/HandModule/*.cpp" )

# MOUSE3D
set ( SRC_MOUSE3D
	src/Mouse3d/Connector.cpp
	src/Mouse3d/LibMouse3d/EventThread.cpp
	src/Mouse3d/LibMouse3d/Mouse3dDevices.cpp
)

if( WIN32 )
	list(APPEND SRC_MOUSE3D
		src/Mouse3d/LibMouse3d/Windows/Mouse3DInput.cpp
		src/Mouse3d/LibMouse3d/Windows/MouseParameters.cpp
		)
elseif( APPLE )
	list(APPEND SRC_MOUSE3D
		src/Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.cpp
		)
elseif( UNIX )
	list(APPEND SRC_MOUSE3D
		src/Mouse3d/LibMouse3d/Unix/ApplicationEmitter.cpp
		src/Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.cpp
		)
endif()



# All source files
set( SRC
	${SRC_APP}
	${SRC_CORE}
	${SRC_MANAGER}
	${SRC_DATA}
	${SRC_CLUSTERING}
	${SRC_IMPORTER}
	${SRC_LAYOUT}
	${SRC_LUAGRAPH}
	${SRC_LUAINTERFACE}
	${SRC_LUATYPES}
	${SRC_MATH}
	${SRC_NETWORK}
	${SRC_MODEL}
	${SRC_OSGQTBROWSER}
	${SRC_QOSG}
	${SRC_VIEWER}
	${SRC_UTIL}
	${SRC_REPOSITORY}
	${SRC_SPECIALMATRIX}
	${SRC_CITY}
	${SRC_SHAPES}
	${SRC_MODULEGRAPH}
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
	# WORKAROUND GARAJ comment
        list( APPEND SRC ${SRC_LEAP} )
        list( APPEND SRC ${SRC_HANDMODULE} )

endif()

# add MOUSE3D if found
if( MOUSE3D_FOUND )
	list( APPEND SRC ${SRC_MOUSE3D} )
endif()

# add diluculum code directly due to MSVC problems
if( MSVC )
	include_directories( SYSTEM dependencies/diluculum/include )

	file( GLOB_RECURSE SRC_DILUCULUM	"dependencies/diluculum/Sources/*.cpp" )

	list( APPEND SRC ${SRC_DILUCULUM} )
endif()

#-------------------------------- Header files -------------------------------
file( GLOB_RECURSE HEADER_APP		"include/Application/*.h" )
file( GLOB_RECURSE HEADER_CORE		"include/Core/*.h" )
file( GLOB_RECURSE HEADER_DATA		"include/Data/*.h" )
file( GLOB_RECURSE HEADER_IMPORTER	"include/Importer/*.h" )
file( GLOB_RECURSE HEADER_LAYOUT	"include/Layout/*.h" )
file( GLOB_RECURSE HEADER_LUAGRAPH  "include/LuaGraph/*.h")
file( GLOB_RECURSE HEADER_LUAINTERFACE  "include/LuaInterface/*.h")
file( GLOB_RECURSE HEADER_LUATYPES  "include/LuaTypes/*.h")
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
file( GLOB_RECURSE HEADER_SPECIALMATRIX		"include/SpecialMatrix/*.h" )
file( GLOB_RECURSE HEADER_CITY		"include/City/*.h" )
file( GLOB_RECURSE HEADER_SHAPES	"include/Shapes/*.h" )
file( GLOB_RECURSE HEADER_MODULEGRAPH	"include/ModuleGraph/*.h" )

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
file( GLOB HEADER_LEAP	"include/Leap/*.h" )
file( GLOB_RECURSE HEADER_LEAPLIB	"include/Leap/LeapLib/*.h" )
file( GLOB_RECURSE HEADER_HANDMODULE	"include/Leap/HandModule/*.h" )

# MOUSE3D
set ( HEADER_MOUSE3D
	include/Mouse3d/Connector.h
	include/Mouse3d/LibMouse3d/EventThread.h
	include/Mouse3d/LibMouse3d/Mouse3dDevices.h
)

if( WIN32 )
	list(APPEND HEADER_MOUSE3D
		include/Mouse3d/LibMouse3d/Windows/Mouse3DInput.h
		include/Mouse3d/LibMouse3d/Windows/MouseParameters.h
		include/Mouse3d/LibMouse3d/Windows/I3dMouseParams.h
		)
elseif( APPLE )
	list(APPEND HEADER_MOUSE3D
		include/Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.h
		)
elseif( UNIX )
	list(APPEND HEADER_MOUSE3D
		include/Mouse3d/LibMouse3d/Unix/ApplicationEmitter.h
		include/Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h
		)
endif()



# All header files
set( INCL
	${HEADER_APP}
	${HEADER_CORE}
	${HEADER_MANAGER}
	${HEADER_DATA}
	${HEADER_CLUSTERING}
	${HEADER_IMPORTER}
	${HEADER_LAYOUT}
	${HEADER_LUAGRAPH}
	${HEADER_LUAINTERFACE}
	${HEADER_LUATYPES}
	${HEADER_MATH}
	${HEADER_NETWORK}
	${HEADER_MODEL}
	${HEADER_OSGQTBROWSER}
	${HEADER_QOSG}
	${HEADER_VIEWER}
	${HEADER_UTIL}
	${HEADER_REPOSITORY}
	${HEADER_SPECIALMATRIX}
	${HEADER_CITY}
	${HEADER_SHAPES}
	${HEADER_MODULEGRAPH}
)

if( OpenCV_FOUND )
	list( APPEND INCL ${HEADER_OPENCV} ${HEADER_QOPENCV} ${HEADER_ARUCO} )
endif()

# Kinect uses Nite, Openni and OpenCV
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

# LEAP
if( LEAP_FOUND )
        list( APPEND INCL ${HEADER_LEAP} )
        list( APPEND INCL ${HEADER_HANDMODULE} )
endif()

# MOUSE3D
if( MOUSE3D_FOUND )
	list( APPEND INCL ${HEADER_MOUSE3D} )
endif()

#-------------------------------------- Group source files --------------------------------------
source_group( \\src main.cpp )
source_group( \\src\\Application "^.*Application/.*$" )
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
source_group( \\src\\LuaTypes "^.*LuaTypes/.*$")
source_group( \\src\\Git "^.*Git/.*$" )
source_group( \\src\\SpecialMatrix "^.*SpecialMatrix/.*$" )
source_group( \\src\\City "^.*City/.*$" )
source_group( \\src\\Shapes "^.*Shapes/.*$" )
source_group( \\src\\ModuleGraph "^.*ModuleGraph/.*$" )

# Group header files
source_group( \\headers\\Application "^.*Application/.*h$")
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
source_group( \\headers\\LuaTypes "^.*LuaTypes/.*h$")
source_group( \\headers\\Git "^.*Git/.*h$" )
source_group( \\headers\\SpecialMatrix "^.*SpecialMatrix/.*h$" )
source_group( \\headers\\City "^.*City/.*h$" )
source_group( \\headers\\Shapes "^.*Shapes/.*h$" )
source_group( \\headers\\ModuleGraph "^.*ModuleGraph/.*h$" )

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

# Leap
if( LEAP_FOUND )
	source_group( \\src\\Leap "^.*Leap/.*$" )
	source_group( \\headers\\Leap "^.*Leap/.*h$" )
endif()

# MOUSE3D
if( MOUSE3D_FOUND )
	source_group( \\src\\Mouse3d "^.*Mouse3d/.*$" )
	source_group( \\headers\\Mouse3d "^.*Mouse3d/.*h$" )
endif()

# ---------------------------------------------------------------
# 3rd party sources
# ---------------------------------------------------------------

add_library( noiseutils STATIC dependencies/noiseutils/noiseutils.cpp )
target_include_directories( noiseutils SYSTEM PUBLIC ${DEPENDENCIES_INCLUDE_DIR}/libnoise dependencies/noiseutils dependencies/libnoise/src )
if ( UNIX AND NOT APPLE )
	add_library( mouse3d STATIC dependencies/mouse3d/xdrvlib.c )
	target_include_directories(mouse3d SYSTEM PUBLIC dependencies/mouse3d)
endif()
add_library( qtcolorpicker STATIC dependencies/qtcolorpicker/qtcolorpicker.cpp )
target_include_directories( qtcolorpicker SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/qtcolorpicker )
if ( Qt5Core_FOUND )
	qt5_use_modules( qtcolorpicker Core Widgets )
endif()

include_directories( SYSTEM dependencies/parserlib/source )
add_library( parserlib STATIC dependencies/parserlib/source/ast.cpp dependencies/parserlib/source/parser.cpp )

# ---------------------------------------------------------------
# Build modules as libraries
# ---------------------------------------------------------------

#-------------------------------- gitlib module -------------------------------
add_library( gitlib STATIC ${SRC_GITLIB} ${HEADER_GITLIB})

target_include_directories( gitlib
	SYSTEM PUBLIC
	include
	include/Repository/Git
)

target_compile_definitions( gitlib PUBLIC -DGITLIB_LIBRARY_STATIC )

target_compile_options( gitlib PUBLIC ${FLAGS_FOR_DEBUG} )

if ( Qt5Core_FOUND )
	qt5_use_modules( gitlib Core )
elseif( Qt4_FOUND )
	target_link_libraries( gitlib ${QT_LIBRARIES} )
endif()

# 'cotire' the module
if( COMMAND cotire )
	# link gitlib_unity target to all libraries to which gitlib links
	set_target_properties( gitlib PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
	cotire( gitlib CONFIGURATIONS Debug Release )
endif()

#-------------------------------- leap module -------------------------------
if( LEAP_FOUND )
        add_library( leaplib STATIC ${SRC_LEAPLIB} ${HEADER_LEAPLIB})

	target_include_directories( leaplib
		SYSTEM PUBLIC
		include
		include/Leap
		)

	target_compile_definitions( leaplib PUBLIC -DLEAPLIB_LIBRARY_STATIC )

	target_compile_options( leaplib PUBLIC ${FLAGS_FOR_DEBUG} )

	if ( Qt5Core_FOUND )
		qt5_use_modules( leaplib Core )
	elseif( Qt4_FOUND )
		target_link_libraries( leaplib ${QT_LIBRARIES} )
	endif()

	# 'leap' the module
	if( COMMAND cotire )
		# link leaplib_unity target to all libraries to which leaplib links
		set_target_properties( leaplib PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
		cotire( leaplib CONFIGURATIONS Debug Release )
	endif()
endif()

# ---------------------------------------------------------------
# Build and link 3Dsoftviz
# ---------------------------------------------------------------

# 3Dsoftviz library
add_library( ${3DSOFTVIZ_NAME} STATIC ${INCL}  ${SRC} )

# set compile flags only for our code ( suppresses warnings on system headers )
target_compile_options( ${3DSOFTVIZ_NAME} PUBLIC ${FLAGS_FOR_DEBUG} )

target_compile_definitions( ${3DSOFTVIZ_NAME}
	PUBLIC ${ALL_COMPILE_DEFINITIONS}
)

if( OpenCV_FOUND )
	list( APPEND ALL_SYSTEM_HEADERS ${DEPENDENCIES_INCLUDE_DIR}/aruco )
endif()

list( APPEND ALL_SYSTEM_HEADERS ${DEPENDENCIES_INCLUDE_DIR} )
list( APPEND ALL_SYSTEM_HEADERS ${DEPENDENCIES_INCLUDE_DIR}/libnoise )
list( APPEND ALL_SYSTEM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/easyloggingpp/src )

list( APPEND ALL_SYSTEM_HEADERS include/Leap )

if( UNIX AND NOT APPLE )
	list( APPEND ALL_SYSTEM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/mouse3d )
endif()

# works:
include_directories(SYSTEM ${ALL_SYSTEM_HEADERS})
# does not work:
#list(REMOVE_DUPLICATES ALL_SYSTEM_HEADERS)
# target_include_directories( ${3DSOFTVIZ_NAME}
# 	SYSTEM PUBLIC ${ALL_SYSTEM_HEADERS}
# )
# target_include_directories( ${3DSOFTVIZ_NAME}
# 	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

if ( Qt5Core_FOUND )
	qt5_use_modules( ${3DSOFTVIZ_NAME}
		Core
		Widgets
		OpenGL
		Sql
		WebEngine
		WebEngineWidgets
		Xml
		Network
		)
endif()

target_link_libraries( ${3DSOFTVIZ_NAME}
	gitlib
	lua
	noiseutils
	libnoise
	osgModeling
	qtcolorpicker
	parserlib
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
	${OSGQT_LIBRARIES}
)

if( UNIX AND NOT APPLE )
	target_link_libraries( ${3DSOFTVIZ_NAME} mouse3d )
endif()

if ( Qt4_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${QT_LIBRARIES} )
endif()

if( NOT MSVC )
	target_link_libraries( ${3DSOFTVIZ_NAME} diluculum )
endif()

if( OpenCV_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OpenCV_LIBS} aruco )
endif()

if( OPENNI2_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OPENNI2_LIBRARIES} )
endif()

if( FREENECT2_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} ${FREENECT2_LIBRARIES} )
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
if ( PCL_FOUND )
        target_link_libraries( ${3DSOFTVIZ_NAME} ${PCL_COMMON_LIBRARIES} )
endif()
if( LEAP_FOUND )
	target_link_libraries( ${3DSOFTVIZ_NAME} leaplib ${LEAP_LIBRARIES} )
endif()

if( MOUSE3D_FOUND )
	if ( WIN32 )
		target_link_libraries( ${3DSOFTVIZ_NAME} ${MOUSE3D_LIBRARIES} )
	elseif( APPLE )
		target_link_libraries( ${3DSOFTVIZ_NAME} ${MOUSE3D_LIBRARIES} )
	elseif( UNIX )
		target_link_libraries( ${3DSOFTVIZ_NAME} ${X11_LIBRARIES} )
	endif()
endif()

# 'cotire' the executable
if( COMMAND cotire )
	# link ${3DSOFTVIZ_NAME}_unity target to all libraries to which ${3DSOFTVIZ_NAME} links
	set_target_properties( ${3DSOFTVIZ_NAME} PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )

	cotire( ${3DSOFTVIZ_NAME} CONFIGURATIONS Debug Release )

	if( LEAP_FOUND )
		target_link_libraries( ${3DSOFTVIZ_NAME}_unity leaplib_unity ${LEAP_LIBRARIES} )
	endif()

	if( MOUSE3D_FOUND )
		if ( WIN32 )
			target_link_libraries( ${3DSOFTVIZ_NAME}_unity ${MOUSE3D_LIBRARIES} )
		elseif( APPLE )
			target_link_libraries( ${3DSOFTVIZ_NAME}_unity ${MOUSE3D_LIBRARIES} )
		elseif( UNIX )
			target_link_libraries( ${3DSOFTVIZ_NAME}_unity ${X11_LIBRARIES} )
		endif()
	endif()
endif()

# Ensure correct dependencies
add_dependencies( ${3DSOFTVIZ_NAME} noiseutils qtcolorpicker)
# dependencies of unity build
if (COMMAND cotire)
	add_dependencies( ${3DSOFTVIZ_NAME} noiseutils qtcolorpicker)
endif()

if( USE_BUILTIN_DEPENDENCIES )

	# dependencies of non-unity build
	add_dependencies( ${3DSOFTVIZ_NAME} libnoise osgModeling gitlib dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging dep_StackTracePlus dep_luasocket dep_mobdebug dep_luacov dep_luacheck dep_lua_cliargs dep_luasystem dep_dkjson dep_say dep_luassert dep_lua-term dep_penlight dep_mediator_lua dep_busted dep_luacomments dep_UMLtoSRC )

	if( MSVC OR APPLE )
		add_dependencies( ${3DSOFTVIZ_NAME} dep_lua )
	else()
		add_dependencies( ${3DSOFTVIZ_NAME} dep_diluculum dep_luajit )
	endif()

	if( OpenCV_FOUND )
		add_dependencies( ${3DSOFTVIZ_NAME} aruco )
	endif()

	if( LEAP_FOUND )
		add_dependencies( ${3DSOFTVIZ_NAME} leaplib )
	endif()

	# dependencies of unity build
	if (COMMAND cotire)
		add_dependencies( ${3DSOFTVIZ_NAME}_unity libnoise osgModeling gitlib_unity dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging dep_StackTracePlus dep_luasocket dep_mobdebug dep_luacov dep_luacheck dep_lua_cliargs dep_luasystem dep_dkjson dep_say dep_luassert dep_lua-term dep_penlight dep_mediator_lua dep_busted dep_luacomments dep_UMLtoSRC )

		if( MSVC OR APPLE )
			add_dependencies( ${3DSOFTVIZ_NAME}_unity dep_lua )
		else()
			add_dependencies( ${3DSOFTVIZ_NAME}_unity dep_diluculum dep_luajit )
		endif()

		if( OpenCV_FOUND )
			add_dependencies( ${3DSOFTVIZ_NAME}_unity aruco )
		endif()
		if( LEAP_FOUND )
			add_dependencies( ${3DSOFTVIZ_NAME}_unity leaplib_unity )
		endif()

	endif()
endif()

# 3Dsoftviz executable
add_executable( ${3DSOFTVIZ_NAME}_app ${INCL}  ${SRC_MAIN} )

set_target_properties( ${3DSOFTVIZ_NAME}_app PROPERTIES OUTPUT_NAME ${3DSOFTVIZ_NAME} )
target_compile_options( ${3DSOFTVIZ_NAME}_app PUBLIC ${FLAGS_FOR_DEBUG} )
target_compile_definitions( ${3DSOFTVIZ_NAME}_app PUBLIC ${ALL_COMPILE_DEFINITIONS} )
target_link_libraries( ${3DSOFTVIZ_NAME}_app ${3DSOFTVIZ_NAME} )

if( COMMAND cotire )
	set_target_properties( ${3DSOFTVIZ_NAME}_app PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )

	cotire( ${3DSOFTVIZ_NAME}_app CONFIGURATIONS Debug Release )
endif()


# -------------------------------------------------------
# Install
# -------------------------------------------------------

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

# Default install destination
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
	set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install CACHE PATH "Installation Destination" FORCE )
endif()


# 3DSoftviz and data
install( TARGETS ${3DSOFTVIZ_NAME}_app
	BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

install( TARGETS gitlib
	ARCHIVE DESTINATION ${INSTALL_BIN} COMPONENT Runtime
	LIBRARY DESTINATION ${INSTALL_LIB} COMPONENT Runtime )

if( LEAP_FOUND )
	install( TARGETS leaplib
		ARCHIVE DESTINATION ${INSTALL_BIN} COMPONENT Runtime
		LIBRARY DESTINATION ${INSTALL_LIB} COMPONENT Runtime )
endif()

# add custom install_unity target
if( COMMAND cotire )
	add_custom_target( install_unity
		COMMAND ${CMAKE_COMMAND} -P "cmake_install.cmake" )
	add_dependencies( install_unity ${3DSOFTVIZ_NAME}_app_unity )
	add_dependencies( install_unity gitlib_unity )
	if( LEAP_FOUND )
		add_dependencies( install_unity leaplib_unity )
	endif()
endif()

# install lua modules
install( DIRECTORY resources/scripts/module/ DESTINATION ${INSTALL_LIB}/lua COMPONENT Data )

# install resources
install( DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data PATTERN resources/scripts/module EXCLUDE)

# Install needed .dll files for OpenNI2
if( NITE2_FOUND AND OPENNI2_FOUND )
	if( WIN32 )
		install( DIRECTORY ${OPENNI2_DLL}/ DESTINATION ${INSTALL_BIN}/ )
	else()
		install( FILES ${OPENNI2_LIBRARY} DESTINATION ${INSTALL_BIN}/ )
		install( FILES ${NITE2_LIBRARY} DESTINATION ${INSTALL_BIN}/ )
	endif()
endif()

# install .dll for speech recognition
if( SPEECHSDK_FOUND )
	install( DIRECTORY ${SPEECHSDK_DRIVER_DIRS}/ DESTINATION ${INSTALL_BIN}/ )
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
	if ( WIN32 )
		install( DIRECTORY ${LEAP_DRIVER_DIRS}/ DESTINATION ${INSTALL_BIN}/ )
	else()
		install( FILES ${LEAP_LIBRARIES} DESTINATION ${INSTALL_BIN}/ )
	endif()
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

# Include QT libraries in Apple bundle and on Windows
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

set( TEST_RUNNER tests/all_tests.cpp )

#-------------------------------- gitlib module tests -------------------------------
file( GLOB_RECURSE SRC_GITLIB_TESTS	"tests/Repository/Git/GitLib/*.cpp" )

add_executable( test_gitlib_module ${TEST_RUNNER} ${SRC_GITLIB_TESTS} )
add_dependencies( test_gitlib_module igloo )

target_include_directories( test_gitlib_module 	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	include/Repository/Git 	)
target_compile_options( test_gitlib_module PUBLIC ${FLAGS_FOR_DEBUG} )
if ( Qt5Core_FOUND )
	qt5_use_modules( test_gitlib_module Core )
	target_link_libraries( test_gitlib_module gitlib )
elseif( Qt4_FOUND )
	target_link_libraries( test_gitlib_module ${QT_LIBRARIES} gitlib )
endif()

# 'cotire' the executable
if( COMMAND cotire )
	set_target_properties( test_gitlib_module PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
	cotire( test_gitlib_module CONFIGURATIONS Debug Release )
endif()


#-------------------------------- run all tests -------------------------------
enable_testing()
add_test( testing_gitlib_module test_gitlib_module )


# ------------------------------ SPHINX -------------------------------
# Find Sphinx Executable

find_package(Sphinx)
if (Sphinx_FOUND)
	if(NOT DEFINED SPHINX_THEME)
		set(SPHINX_THEME sphinx_rtd_theme)
	endif()

	if(NOT DEFINED SPHINX_THEME_DIR)
		set(SPHINX_THEME_DIR)
	endif()

	# configured documentation tools and intermediate build results
	set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

	# Sphinx cache with pickled ReST documents
	set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/_doctrees")

	# HTML output directory
	set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/html")

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/docs/conf.py.in"
		"${BINARY_BUILD_DIR}/conf.py"
		@ONLY)

	add_custom_target(Sphinx_html
		${SPHINX_EXECUTABLE}
		-q -b html
		-c "${BINARY_BUILD_DIR}"
		-d "${SPHINX_CACHE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/docs"
		"${SPHINX_HTML_DIR}"
		COMMENT "Building HTML documentation with Sphinx")
endif()


# ------------------------------ ASTYLE -------------------------------
# use "make style" to format all C++ code

# Find Astyle Executable
find_package(Astyle)

if (Astyle_FOUND)
	# Add a custom target
	add_custom_target( "style" COMMAND
		"${ASTYLE_EXECUTABLE}"
		--options=${CMAKE_CURRENT_SOURCE_DIR}/astyle.options
		${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		COMMENT "Formating source code with Astyle."
		VERBATIM
	)
endif()


# ------------------------------ CPPLINT -------------------------------
# use "make cpplint" to check all C++ code with cpplint.py

# Find cpplint
find_package(cpplint)

if( CPPLINT_FOUND )
	FILE( GLOB_RECURSE ALL_INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
	FILE( GLOB_RECURSE ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
	if( WIN32 )
		add_custom_target( "cpplint"
			COMMAND
				${CMAKE_CURRENT_SOURCE_DIR}/cpplint.bat	# NOTE: this will probably work only on Linux/OSX
			WORKING_DIRECTORY
				"${CMAKE_CURRENT_BINARY_DIR}"
			COMMENT "Linting code via cpplint.py"
			VERBATIM
		)
	else()
		add_custom_target( "cpplint"
		COMMAND
			"${PYTHON_EXECUTABLE}"
			"${CPPLINT_SCRIPT}"
			${ALL_INCLUDE_FILES}
			${ALL_SRC_FILES}
			2>&1 | tee cpplint-report.txt	# NOTE: this will probably work only on Linux/OSX
		WORKING_DIRECTORY
			"${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Linting code via cpplint.py"
		VERBATIM
	)
	endif()
endif()

# ------------------------------ CPPcheck -------------------------------
# use "make cppcheck" to check all C++ code with CPPcheck

# Find cppchceck
find_package(cppcheck)

if( cppcheck_FOUND )
	file( GLOB_RECURSE ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
	if( WIN32 )
		add_custom_target( cppcheck
			COMMAND
				${CMAKE_CURRENT_SOURCE_DIR}/run_cppcheck.bat	# NOTE: this will probably work only on Linux/OSX
			WORKING_DIRECTORY
				"${CMAKE_CURRENT_BINARY_DIR}"
			COMMENT "Linting code via cpplint.py"
			VERBATIM
		)
	else()
		add_custom_target(
				cppcheck
				COMMAND cppcheck
				--enable=warning,style,performance,portability,information,missingInclude
				--std=c++11
				--library=qt.cfg
				--verbose
				--quiet
				-j2
				-I ${CMAKE_CURRENT_SOURCE_DIR}/include/
				${ALL_SRC_FILES}
		)
	endif()
endif()

