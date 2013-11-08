# Copyright (C) 2013 3DSoftviz.
# Created by Peter Kapec
# Redistribution and use of this file is allowed
# according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with 3DSoftviz.

project( 3DSoftviz C CXX )
cmake_minimum_required( VERSION 2.8 )

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
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# Additional CMake scripts
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# Warnings for Debug mode
option( USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." OFF )
if( USE_STRICT_COMPILE_WARNINGS )
	if( CMAKE_COMPILER_IS_GNUCXX )
  		set( CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror" )
	endif()
endif()

# Default build type
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "" FORCE )
#	set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE )
endif()

# Default install prefix
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
 	set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install CACHE PATH "Installation Destination" FORCE )
endif()

# RPath functionality
set( CMAKE_SKIP_BUILD_RPATH FALSE CACHE STRING "" FORCE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_RPATH ../lib CACHE STRING "" FORCE )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_NAME_DIR @executable_path/../lib CACHE STRING "" FORCE )

# --------------
# Build settings - COTIRE module
# --------------
include(cotire)

# ------------
# Dependencies - External projects
# ------------

# Build dependencies by default
option( USE_BUILTIN_DEPENDENCIES "Use builtin dependencies." ON )

# Build dependencies
if( USE_BUILTIN_DEPENDENCIES )

	# set search paths to the dependencies
	set( DEP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies )
	set( DEP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dependencies )
	set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEP_BIN}/lib ${DEP_BIN}/bin )
	set( CMAKE_INCLUDE_PATH ${CMaKE_INCLUDE_PATH} ${DEP_BIN}/include )

	# Generate cache
	configure_file( config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.cmake )

	# Build libnoise
	include( ExternalProject )
	ExternalProject_Add( dep_libnoise
	  PREFIX libnoise
	  SOURCE_DIR ${DEP_SOURCE}/libnoise
	  BINARY_DIR libnoise
	  CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=${DEP_BIN}
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-C ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
	)

	# Ensure correct order of build(dependencies between ext. dependencies)
	# add_dependencies( dep_lpeg dep_luajit )

	# Define LUA_ variables
	#set( LUA_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
	#set( DILUCULUM_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )
	set( LIBNOISE_LIBRARY ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )
	if( WIN32 )
		#set( LUA_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX} )
		#set( DILUCULUM_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}diluculum${CMAKE_SHARED_LIBRARY_SUFFIX} )
		set( LIBNOISE_LIBRARY ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )
	endif()
	set( DEPENDENCIES_INCLUDE_DIR ${DEP_BIN}/include )
else()
	# Find Lua implementation, assuming lpeg is installed by other means
	#find_package( Lua51 REQUIRED )
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
	set( MACOSX_BUNDLE_ICON_FILE "3DSOFTVIZ_app.icns" )
	set( MACOSX_BUNDLE_GUI_IDENTIFIER "${3DSOFTVIZ_DOMAIN_SECOND}.${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_NAME}" )
	set( MACOSX_BUNDLE_BUNDLE_NAME "${3DSOFTVIZ_NAME}" )

	# CPack settings
	set( CPACK_GENERATOR "DragNDrop" )
	set( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/3DSOFTVIZ_pkg.icns" )
	set( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )

	# OSX Specific resources
	install( FILES res/3DSOFTVIZ_app.icns DESTINATION ${INSTALL_RES} )
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
include_directories(
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include

	${DEPENDENCIES_INCLUDE_DIR}
	${DEPENDENCIES_INCLUDE_DIR}/libnoise
)

# Source files
file(GLOB_RECURSE SRC  "src/*.cpp")
file(GLOB_RECURSE INCL "include/*.h")

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
# Group generated moc files
source_group(\\MOC "^.*moc_.*$")

# header files containing Q_OBJECT macros
set(SOURCES_H
	./include/OsgQtBrowser/QGraphicsViewAdapter.h
	./include/QOSG/CoreWindow.h
	./include/QOSG/TreeModel.h
	./include/Util/Cleaner.h
	./include/QOSG/OptionsWindow.h
	./include/QOSG/LoadGraphWindow.h
	./include/QOSG/SelectLayoutWindow.h
	./include/QOSG/CheckBoxList.h
    ./include/QOSG/qtcolorpicker.h
    ./include/Viewer/PickHandler.h
    ./include/Network/Server.h
    ./include/Network/Client.h
)

# Qt Generators
# process header files containing Q_OBJECT macros - generates moc files
QT4_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} )

# Build and link
add_executable( ${3DSOFTVIZ_NAME} ${INCL} ${SRC} ${SOURCES_H_MOC} )

target_link_libraries( ${3DSOFTVIZ_NAME}
	${QT_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
	${LIBNOISE_LIBRARY}
)

# 'cotire' the executable
cotire( ${3DSOFTVIZ_NAME} )

# ensure correct dependencies
add_dependencies( ${3DSOFTVIZ_NAME} dep_libnoise )

# -------
# Install
# -------

# 3DSoftviz and data
install( TARGETS ${3DSOFTVIZ_NAME}
	BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

install(DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data)

# Install built in dependencies unless we are building an OSX Bundle
if( USE_BUILTIN_DEPENDENCIES )
  	# Make sure dependencies are built
  	add_dependencies( ${3DSOFTVIZ_NAME} dep_libnoise )

  	# Install all dependencies
	  install( DIRECTORY ${DEP_BIN}/
		DESTINATION ${INSTALL_DEPS}
		COMPONENT Dependencies
		USE_SOURCE_PERMISSIONS
  )
endif()

# Include QT4 libraries in Apple bundle and on Windows
if( WIN32 OR APPLE )
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
