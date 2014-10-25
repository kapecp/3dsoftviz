# Copyright (C) 2013 3DSoftviz.
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
	if( CMAKE_COMPILER_IS_GNUCXX )
#~		set( CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra" )

                #set( CMAKE_CXX_FLAGS_DEBUG
                set( FLAGS_FOR_DEBUG "-ansi -pedantic -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal  -Wformat  -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wformat=2  -Wimport  -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wlong-long -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute   -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept  -Wpacked  -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare  -Wsign-promo -Wstack-protector -Wstrict-aliasing=2 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings  -Wno-unused-parameter -Wno-unused" )

		# to suppress warnings for unused stuff use: -Wno-unused-parameter -Wno-unused-result
		# not used:
			# too much warnings in Qt headers: NULL vs 0 pointers: -Wzero-as-null-pointer-constant
                        # too much warnings: -Wshadow -Waggregate-return
                        # too much warnings, but should be used: -Wold-style-cast -Woverloaded-virtual -Wuseless-cast -Wsign-conversion
			# class size optimizations (not really needed): -Wpadded
			# only for C: -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes
        endif()
endif()

# Default build type
if( NOT CMAKE_BUILD_TYPE )
#	set( CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "" FORCE )
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
set( CMAKE_INSTALL_RPATH "$ORIGIN/../lib" CACHE STRING "" FORCE )
# next line adds additional link paths to RPath (e.g. for qscintilla when not in system):
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "" FORCE )

# --------------
# Build settings - COTIRE module
# --------------
include( cotire )
if( COMMAND cotire )
	if( NOT COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES )
		include( ProcessorCount )
		ProcessorCount( N )

		if( NOT N EQUAL 0 )
			set( COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j${N}" )
		endif()
	endif()

        set( COTIRE_VERBOSE TRUE )
        set( COTIRE_DEBUG TRUE )
endif()

# ------------
# Dependencies - External projects
# ------------

# Build dependencies by default
option( USE_BUILTIN_DEPENDENCIES "Use builtin dependencies." ON )

# Build dependencies
if( USE_BUILTIN_DEPENDENCIES )

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

	 # Build LuaJIT2
	ExternalProject_Add ( dep_luajit
		PREFIX luajit
		SOURCE_DIR ${DEP_SOURCE}/luajit
		BINARY_DIR luajit
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
			-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
	)

	# Build Diluculum
	ExternalProject_Add ( dep_diluculum
		PREFIX diluculum
		SOURCE_DIR ${DEP_SOURCE}/diluculum
		BINARY_DIR diluculum
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	)

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

	# Build aruco
	#include( ExternalProject )
	#ExternalProject_Add( dep_aruco
	#	PREFIX aruco
	#	SOURCE_DIR ${DEP_SOURCE}/aruco
	#	BINARY_DIR aruco
	#	CMAKE_ARGS
	#		-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
	#		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
	#		-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
	#)


	# Ensure correct order of build(dependencies between ext. dependencies)
	add_dependencies( dep_lpeg dep_luajit )
	add_dependencies( dep_diluculum dep_luajit )
	add_dependencies( dep_leg dep_lpeg )
	add_dependencies( dep_luametrics dep_leg )
	add_dependencies( dep_luafilesystem dep_luajit )

	# Define LUA_ variables

	set( LUA_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
	set( DILUCULUM_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )

	#set( LIBARUCO_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX} )

	# more correct:
	add_library(libnoise SHARED IMPORTED)
	add_dependencies(libnoise  dep_libnoise)
	set_target_properties(libnoise PROPERTIES
		IMPORTED_LOCATION  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )

	if( WIN32 )
		set( LUA_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
		set( DILUCULUM_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )
		#set( LIBARUCO_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX} )
		set_target_properties(libnoise PROPERTIES
			IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
			IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

		if( MSVC )
			# set "lib" prefix for MSVC, becasue libnoise.def files requires that the library is named "libnoise"
				#set( LIBARUCO_LIBRARY ${DEP_BIN}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}aruco${CMAKE_STATIC_LIBRARY_SUFFIX})
				set( MISS_MSVC_PREFIX lib )
				set_target_properties(libnoise PROPERTIES
				IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

		endif()
	endif()
	set( DEPENDENCIES_INCLUDE_DIR ${DEP_BIN}/include )
else()
	# Find Lua implementation, assuming lpeg is installed by other means
	find_package( Lua51 REQUIRED )
endif()

# ------------
# Dependencies - Searching external dependencies
# ------------

# Find Qt4
find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTWEBKIT 1)
set(QT_USE_QTXML 1)
set(QT_USE_QTNETWORK 1)
include(${QT_USE_FILE})
# autogenerate all moc files
set( CMAKE_AUTOMOC TRUE )

# add special flag to GCC compiler when using Qt 4.6.x with GCC >4.6
if(	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND
	"${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "4.7" AND
	${CMAKE_C_COMPILER_VERSION} VERSION_GREATER "4.6" )

	set( CMAKE_CXX_FLAGS "-fpermissive" )
endif()


# Find OpenSceneGraph
find_package(OpenSceneGraph COMPONENTS
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
find_package(osgViewer)


# Find OpenCV
#find_package( OpenCV )
if( OpenCV_FOUND )
	add_definitions( -DOPENCV_FOUND )
	message("OpenCV FOUND")
else()
	message("OpenCV NOT FOUND")
endif()


# Find Boost
set ( Booqst_USE_STATIC_LIBS OFF)
find_package ( Boost 1.39 COMPONENTS unit_test_framework REQUIRED )
#add_definitions ( -DBOOST_ALL_DYN_LINK )

# Include directories
include_directories ( ${Boost_INCLUDE_DIRS}  )


# -----------------
# Platform settings
# -----------------

# Apple specific overrides, we build and app bundle
if( APPLE )
	# Executable settings
	set( 3DSOFTVIZ_EXECUTABLE_TYPE MACOSX_BUNDLE )
	set( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${3DSOFTVIZ_NAME}.app" )

	# Override default install destinations into the bundle
	set( INSTALL_BUNDLE . )
	set( INSTALL_BIN     ${3DSOFTVIZ_NAME}.app/Contents/MacOS )
	set( INSTALL_DATA    ${3DSOFTVIZ_NAME}.app/Contents/share/3DSOFTVIZ )
	set( INSTALL_RES     ${3DSOFTVIZ_NAME}.app/Contents/Resources )
	set( INSTALL_DEPS    ${3DSOFTVIZ_NAME}.app/Contents )
	set( INSTALL_PLUGIN  ${3DSOFTVIZ_NAME}.app/Contents/MacOS)
	set( INSTALL_QTCONF  ${3DSOFTVIZ_NAME}.app/Contents/Resources)

	# Bundle settings
	set( MACOSX_BUNDLE_INFO_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_BUNDLE_VERSION "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_LONG_VERSION_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_SHORT_VERSION_STRING "${3DSOFTVIZ_VERSION}" )
	set( MACOSX_BUNDLE_COPYRIGHT "${3DSOFTVIZ_COPYRIGHT_YEAR} ${3DSOFTVIZ_VENDOR}" )
	#set( MACOSX_BUNDLE_ICON_FILE "3DSOFTVIZ_app.icns" )
	set( MACOSX_BUNDLE_GUI_IDENTIFIER "${3DSOFTVIZ_DOMAIN_SECOND}.${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_NAME}" )
	set( MACOSX_BUNDLE_BUNDLE_NAME "${3DSOFTVIZ_NAME}" )

	# CPack settings
	set( CPACK_GENERATOR "DragNDrop" )
	set( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/3DSOFTVIZ_pkg.icns" )
	set( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )

	# OSX Specific resources
	#install( FILES resources/${MACOSX_BUNDLE_ICON_FILE} DESTINATION ${INSTALL_RES} )
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

# ---------------
# Build 3DSOFTVIZ
# ---------------

# Include Paths
include_directories(SYSTEM
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}

	${DEPENDENCIES_INCLUDE_DIR}
	${DEPENDENCIES_INCLUDE_DIR}/libnoise
	#${DEPENDENCIES_INCLUDE_DIR}/aruco

	${OpenCV_INCLUDE_DIRS}
)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Source files
set(SRC_MAIN "src/main.cpp")
file(GLOB_RECURSE SRC_CORE  "src/Core/*.cpp")
file(GLOB_RECURSE SRC_DATA  "src/Data/*.cpp")
file(GLOB_RECURSE SRC_IMPORTER  "src/Importer/*.cpp")
file(GLOB_RECURSE SRC_LAYOUT  "src/Layout/*.cpp")
file(GLOB_RECURSE SRC_LUAGRAPH  "src/LuaGraph/*.cpp")
file(GLOB_RECURSE SRC_LUAINTERFACE  "src/LuaInterface/*.cpp")
file(GLOB_RECURSE SRC_MANAGER  "src/Manager/*.cpp")
file(GLOB_RECURSE SRC_MATH  "src/Math/*.cpp")
file(GLOB_RECURSE SRC_MODEL  "src/Model/*.cpp")
file(GLOB_RECURSE SRC_NETWORK  "src/Network/*.cpp")
file(GLOB_RECURSE SRC_NOISE  "src/Noise/*.cpp")
file(GLOB_RECURSE SRC_OSGQTBROWSER  "src/OsgQtBrowser/*.cpp")
file(GLOB_RECURSE SRC_QOSG  "src/QOSG/*.cpp")
file(GLOB_RECURSE SRC_UTIL  "src/Util/*.cpp")
file(GLOB_RECURSE SRC_VIEWER  "src/Viewer/*.cpp")
file(GLOB_RECURSE SRC_OPENCV  "src/OpenCV/*.cpp")
file(GLOB_RECURSE SRC_QOPENCV  "src/QOpenCV/*.cpp")

# Source files
set(SRC
	${SRC_MAIN} ${SRC_CORE} ${SRC_DATA} ${SRC_IMPORTER}
        ${SRC_LAYOUT} ${SRC_LUAGRAPH} ${SRC_LUAINTERFACE} ${SRC_MANAGER} ${SRC_MATH}
	${SRC_NETWORK} ${SRC_NOISE}
	${SRC_OSGQTBROWSER} ${SRC_QOSG} ${SRC_UTIL} ${SRC_VIEWER} ${SRC_MODEL}
)

if( OpenCV_FOUND )
	list(APPEND SRC ${SRC_OPENCV} ${SRC_QOPENCV})
endif()

#file(GLOB_RECURSE INCL "include/*.h")
file(GLOB_RECURSE HEADER_CORE  "include/Core/*.h")
file(GLOB_RECURSE HEADER_DATA  "include/Data/*.h")
file(GLOB_RECURSE HEADER_IMPORTER  "include/Importer/*.h")
file(GLOB_RECURSE HEADER_LAYOUT  "include/Layout/*.h")
file(GLOB_RECURSE HEADER_LUAGRAPH  "include/LuaGraph/*.h")
file(GLOB_RECURSE HEADER_LUAINTERFACE  "include/LuaInterface/*.h")
file(GLOB_RECURSE HEADER_MANAGER  "include/Manager/*.h")
file(GLOB_RECURSE HEADER_MATH  "include/Math/*.h")
file(GLOB_RECURSE HEADER_MODEL  "include/Model/*.h")
file(GLOB_RECURSE HEADER_NETWORK  "include/Network/*.h")
file(GLOB_RECURSE HEADER_NOISE  "include/Noise/*.h")
file(GLOB_RECURSE HEADER_OSGQTBROWSER  "include/OsgQtBrowser/*.h")
file(GLOB_RECURSE HEADER_QOSG  "include/QOSG/*.h")
file(GLOB_RECURSE HEADER_UTIL  "include/Util/*.h")
file(GLOB_RECURSE HEADER_VIEWER  "include/Viewer/*.h")
file(GLOB_RECURSE HEADER_QOPENCV  "include/OpenCV/*.h")
file(GLOB_RECURSE HEADER_QOPENCV  "include/QOpenCV/*.h")

set(INCL
	${HEADER_CORE} ${HEADER_DATA} ${HEADER_IMPORTER}
        ${HEADER_LAYOUT} ${HEADER_LUAGRAPH} ${HEADER_LUAINTERFACE} ${HEADER_MANAGER} ${HEADER_MATH}
	${HEADER_NETWORK} ${HEADER_NOISE}
	${HEADER_OSGQTBROWSER} ${HEADER_QOSG} ${HEADER_UTIL} ${HEADER_VIEWER} ${HEADER_MODEL}
)

if( OpenCV_FOUND )
    list(APPEND INCL ${HEADER_OPENCV} ${HEADER_QOPENCV} ${HEADER_ARUCO})
endif()


# Group source files
source_group(\\src main.cpp)
source_group(\\src\\Network "^.*Network/.*$")
source_group(\\src\\Viewer "^.*Viewer/.*$")
source_group(\\src\\Core "^.*Core/.*$")
source_group(\\src\\Data "^.*Data/.*$")
source_group(\\src\\Manager "^.*Manager/.*$")
source_group(\\src\\Importer "^.*Importer/.*$")
source_group(\\src\\Model "^.*Model/.*$")
source_group(\\src\\Util "^.*Util/.*$")
source_group(\\src\\Layout "^.*Layout/.*$")
source_group(\\src\\QOSG "^.*QOSG/.*$")
source_group(\\src\\OsgQtBrowser "^.*OsgQtBrowser/.*$")
source_group(\\src\\Noise "^.*Noise/.*$")
source_group(\\src\\LuaGraph "^.*LuaGraph/.*$")
source_group(\\src\\LuaInterface "^.*LuaInterface/.*$")
# Group header files
source_group(\\headers\\Network "^.*Network/.*h$")
source_group(\\headers\\Viewer "^.*Viewer/.*h$")
source_group(\\headers\\Core "^.*Core/.*h$")
source_group(\\headers\\Data "^.*Data/.*h$")
source_group(\\headers\\Manager "^.*Manager/.*h$")
source_group(\\headers\\Importer "^.*Importer/.*h$")
source_group(\\headers\\Model "^.*Model/.*h$")
source_group(\\headers\\Util "^.*Util/.*h$")
source_group(\\headers\\Layout "^.*Layout/.*h$")
source_group(\\headers\\QOSG "^.*QOSG/.*h$")
source_group(\\headers\\Noise "^.*Noise/.*h$")
source_group(\\headers\\OsgQtBrowser "^.*OsgQtBrowser/.*h$")
source_group(\\headers\\LuaGraph "^.*LuaGraph/.*h$")
source_group(\\headers\\LuaInterface "^.*LuaInterface/.*h$")

if(OpenCV_FOUND)
    source_group(\\src\\QOpenCV "^.*QOpenCV/.*$")
    source_group(\\src\\OpenCV "^.*OpenCV/.*$")
    source_group(\\headers\\QOpenCV "^.*QOpenCV/.*h$")
    source_group(\\headers\\OpenCV "^.*OpenCV/.*h$")
endif()

# Group generated moc files
#source_group(\\MOC "^.*moc_.*$")

# Qt Generators
# process header files containing Q_OBJECT macros - generates moc files
# not needed anymore - using CMAKE_AUTOMOC set above
#QT4_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} )

# Build and link
add_executable( ${3DSOFTVIZ_NAME} ${INCL} ${SRC} )

# set compile flags only for our code (suppresses warnigns on system headers)
#set_target_properties( ${3DSOFTVIZ_NAME} PROPERTIES COMPILE_FLAGS ${FLAGS_FOR_DEBUG})

target_link_libraries( ${3DSOFTVIZ_NAME}
	${QT_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
	${DILUCULUM_LIBRARY}
	${LUA_LIBRARY}
	#${LIBARUCO_LIBRARY}
	libnoise
)

if(OpenCV_FOUND)
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OpenCV_LIBS} )
endif()

# 'cotire' the executable
if( COMMAND cotire )
	# this needs CMake 2.8.11 and greater
	if (${CMAKE_VERSION} VERSION_GREATER "2.8.10")
		set_target_properties(${3DSOFTVIZ_NAME} PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY")
	endif()

        cotire( ${3DSOFTVIZ_NAME} CONFIGURATIONS Release MinSizeRel Debug)

	if (${CMAKE_VERSION} VERSION_LESS "2.8.11")
		target_link_libraries(${3DSOFTVIZ_NAME}_unity
			${QT_LIBRARIES}
			${OPENSCENEGRAPH_LIBRARIES}
			${OSGVIEWER_LIBRARIES}
			libnoise)

                if(OpenCV_FOUND)
                    target_link_libraries( ${3DSOFTVIZ_NAME}_unity ${OpenCV_LIBS} )
                endif()
	endif()

	if( MSVC )
		### Fix virtual memory range
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200" CACHE STRING "" FORCE)
	endif()

        # fix for Diluculum and LuaJIT on 64bit OSX: Lua new_state returned NULL, LuaJIT needs following flags
        if( APPLE )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "" FORCE)
        endif()

endif()


# -------
# Install
# -------

# 3DSoftviz and data
install( TARGETS ${3DSOFTVIZ_NAME}
	BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

if (COMMAND cotire)
	add_custom_target(install_unity
		COMMAND ${CMAKE_COMMAND} -P "cmake_install.cmake")
	add_dependencies(install_unity ${3DSOFTVIZ_NAME}_unity)
endif()


install(DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data)

# Install built in dependencies unless we are building an OSX Bundle
if( USE_BUILTIN_DEPENDENCIES )
	# Make sure dependencies are built
        add_dependencies( ${3DSOFTVIZ_NAME} libnoise dep_diluculum dep_luametrics dep_leg dep_lpeg dep_luajit dep_luafilesystem )
        #add_dependencies( ${3DSOFTVIZ_NAME} dep_aruco )
	if (COMMAND cotire)
                add_dependencies( ${3DSOFTVIZ_NAME}_unity libnoise dep_diluculum dep_luametrics dep_leg dep_lpeg dep_luajit dep_luafilesystem )
                #add_dependencies( ${3DSOFTVIZ_NAME}_unity dep_aruco )
	endif()

	# Install all dependencies
	install( DIRECTORY ${DEP_BIN}/
		DESTINATION ${INSTALL_DEPS}
		COMPONENT Dependencies
		USE_SOURCE_PERMISSIONS )
endif()

# Include QT4 libraries in Apple bundle and on Windows
if( WIN32 ) # APPLE removed - TODO mac app bundle + uncomment & add icns file for MACOSX_BUNDLE_ICON_FILE
	# On Apple plugins need to be bundled
	if( APPLE )
		# Install Qt Plugins
		install( DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
			DESTINATION ${INSTALL_PLUGIN}/plugins
			COMPONENT Runtime )

		# Install Qt Config
		install( CODE "
			file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF}/qt.conf\" \"\")
			" COMPONENT Runtime)
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

# -------
# Packing
# -------

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

