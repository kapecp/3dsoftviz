# Copyright (C) 2013 3DSoftviz.
# Created by Peter Kapec
# Redistribution and use of this file is allowed 
# according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with 3DSoftviz.

PROJECT(3DSoftviz C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# --------------
# 3DSoftviz Info
# --------------

set ( 3DSOFTVIZ_NAME "3DSoftviz" )
set ( 3DSOFTVIZ_LONGNAME "3DSoftviz - Generic 3D graph visualization tool for software visualization" )
set ( 3DSOFTVIZ_VERSION_MAJOR "0" )
set ( 3DSOFTVIZ_VERSION_MINOR "0" )
set ( 3DSOFTVIZ_VERSION_PATCH "0" )
set ( 3DSOFTVIZ_VERSION_BUILD "1" )
set ( 3DSOFTVIZ_VERSION "${3DSOFTVIZ_VERSION_MAJOR}.${3DSOFTVIZ_VERSION_MINOR}.${3DSOFTVIZ_VERSION_PATCH}" )
set ( 3DSOFTVIZ_VENDOR "3DSoftviz Dev Team" )
set ( 3DSOFTVIZ_COPYRIGHT_YEAR "2013" )
set ( 3DSOFTVIZ_DOMAIN_FIRST "stuba" )
set ( 3DSOFTVIZ_DOMAIN_SECOND "sk" )
set ( 3DSOFTVIZ_DOMAIN "${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_DOMAIN_SECOND}" )

# --------------
# Build Settings
# --------------

# Install destinations
set ( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set ( INSTALL_DATA share/3dsoftviz CACHE PATH "Directory for shared data." )
set ( INSTALL_DEPS . )
set ( INSTALL_PLUGIN bin)
set ( INSTALL_QTCONF bin)

SET (INSTALL_INC include CACHE PATH "Directory for library headers.")
SET (INSTALL_DATA . CACHE PATH "Directory the package can store documentation, tests or other data in.")
SET (INSTALL_DOC ${INSTALL_DATA}/doc CACHE PATH "Recommended directory to install documentation into.")
SET (INSTALL_ETC ${INSTALL_DATA}/etc CACHE PATH "Other files.")
SET (INSTALL_TEST ${INSTALL_DATA}/test CACHE PATH "Test files.")
SET (INSTALL_EXAMPLE ${INSTALL_DATA}/example CACHE PATH "Recommended directory to install examples into.")

# Additional CMake scripts
set ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# Additional CMake scripts
set ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# Warnings for Debug mode
#if ( CMAKE_COMPILER_IS_GNUCXX )
#  set ( CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror" )
#endif ()

# Default build type
if ( NOT CMAKE_BUILD_TYPE )
#	set ( CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "" FORCE )
	set ( CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE )
endif ()

# Default install prefix
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  set ( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install CACHE PATH "Installation Destination" FORCE )
endif ()

# RPath functionality
set ( CMAKE_SKIP_BUILD_RPATH FALSE CACHE STRING "" FORCE )
set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE CACHE STRING "" FORCE )
set ( CMAKE_INSTALL_RPATH ../lib CACHE STRING "" FORCE )
set ( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
set ( CMAKE_INSTALL_NAME_DIR @executable_path/../lib CACHE STRING "" FORCE )

# --------------
# Build Settings - COTIRE module
# --------------
include(cotire)

# ------------
# Dependencies - External projects
# ------------

#TODO: replace as submodule
SET (CMAKE_INCLUDE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Windows/include
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Source/libnoisesrc-1.0.0/src
	${CMAKE_INCLUDE_PATH})

# ------------
# Dependencies - Searching external dependencies
# ------------

# Find ZLib
#FIND_PACKAGE(ZLIB)

# Find LibPNG
#FIND_PACKAGE(PNG)

# Find Qt4
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL 1)
SET(QT_USE_QTSQL 1)
SET(QT_USE_QTWEBKIT 1)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

# Find OpenSceneGraph
FIND_PACKAGE(OpenSceneGraph COMPONENTS
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
FIND_PACKAGE(osgViewer)

# -----------------
# Platform Settings
# -----------------

# Apple specific overrides, we build and app bundle
if ( APPLE )
  # Executable settings
  set ( 3DSOFTVIZ_EXECUTABLE_TYPE MACOSX_BUNDLE )
  set ( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${3DSOFTVIZ_NAME}.app" )

  # Override default install destinations into the bundle
  set ( INSTALL_BUNDLE . )
  set ( INSTALL_BIN     ${3DSOFTVIZ_NAME}.app/Contents/MacOS )
  set ( INSTALL_DATA    ${3DSOFTVIZ_NAME}.app/Contents/share/3DSOFTVIZ )
  set ( INSTALL_RES     ${3DSOFTVIZ_NAME}.app/Contents/Resources )
  set ( INSTALL_DEPS    ${3DSOFTVIZ_NAME}.app/Contents )
  set ( INSTALL_PLUGIN  ${3DSOFTVIZ_NAME}.app/Contents/MacOS)
  set ( INSTALL_QTCONF  ${3DSOFTVIZ_NAME}.app/Contents/Resources)

  # Bundle settings
  set ( MACOSX_BUNDLE_INFO_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
  set ( MACOSX_BUNDLE_BUNDLE_VERSION "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
  set ( MACOSX_BUNDLE_LONG_VERSION_STRING "${3DSOFTVIZ_NAME} ${3DSOFTVIZ_VERSION}" )
  set ( MACOSX_BUNDLE_SHORT_VERSION_STRING "${3DSOFTVIZ_VERSION}" )
  set ( MACOSX_BUNDLE_COPYRIGHT "${3DSOFTVIZ_COPYRIGHT_YEAR} ${3DSOFTVIZ_VENDOR}" )
  set ( MACOSX_BUNDLE_ICON_FILE "3DSOFTVIZ_app.icns" )
  set ( MACOSX_BUNDLE_GUI_IDENTIFIER "${3DSOFTVIZ_DOMAIN_SECOND}.${3DSOFTVIZ_DOMAIN_FIRST}.${3DSOFTVIZ_NAME}" )
  set ( MACOSX_BUNDLE_BUNDLE_NAME "${3DSOFTVIZ_NAME}" )

  # CPack Settings
  set ( CPACK_GENERATOR "DragNDrop" )
  set ( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/3DSOFTVIZ_pkg.icns" )
  set ( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )

  # OSX Specific resurces
  install ( FILES res/3DSOFTVIZ_app.icns DESTINATION ${INSTALL_RES} )
endif ()

# Windows specific overrides
if ( WIN32 )
  # Executable settings
  set ( 3DSOFTVIZ_EXECUTABLE_TYPE WIN32 )
  set ( BUNDLE_APP "\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/${3DSOFTVIZ_NAME}.exe" )

  # CPack settings
  set ( CPACK_GENERATOR "NSIS" )
  set ( CPACK_PACKAGE_INSTALL_DIRECTORY "${3DSOFTVIZ_NAME}" )
  set ( CPACK_PACKAGE_FILE_NAME "${3DSOFTVIZ_NAME}-${3DSOFTVIZ_VERSION}" )
  set ( CPACK_PACKAGE_EXECUTABLES "${3DSOFTVIZ_NAME}" "${3DSOFTVIZ_NAME}" )
  set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}\\\\LICENSE.txt" )

  # NSIS branding
  set ( CPACK_NSIS_INSTALLED_ICON_NAME "${INSTALL_BIN}\\\\${3DSOFTVIZ_NAME}${CMAKE_EXECUTABLE_SUFFIX}" )
  set ( CPACK_NSIS_DISPLAY_NAME "${3DSOFTVIZ_NAME}" )
  set ( CPACK_NSIS_HELP_LINK "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
  set ( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
  set ( CPACK_NSIS_CONTACT "http:\\\\\\\\${3DSOFTVIZ_DOMAIN}" )
  set ( CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\3DSOFTVIZ_pkg.ico" )
  set ( CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\3DSOFTVIZ_rmv.ico" )
  set ( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\logo2.bmp" )
endif ()

# ---------------
# Build 3DSOFTVIZ
# ---------------

# Include Paths
INCLUDE_DIRECTORIES(
#	${PNG_INCLUDE_DIRECTORIES}
#	${ZLIB_INCLUDE_DIRECTORIES}
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_INCLUDE_PATH}
	${CMAKE_CURRENT_BINARY_DIR} 
	"./"
)

# Build dependencies
ADD_SUBDIRECTORY(dependencies/Source)

# Source files
FILE(GLOB_RECURSE SRC  "src/*.cpp")
FILE(GLOB_RECURSE INCL "include/*.h")

# Group source files
SOURCE_GROUP(\\src main.cpp)
SOURCE_GROUP(\\src\\Network "^.*Network/.*$")
SOURCE_GROUP(\\src\\Viewer "^.*Viewer/.*$")
SOURCE_GROUP(\\src\\Core "^.*Core/.*$")
SOURCE_GROUP(\\src\\Data "^.*Data/.*$")
SOURCE_GROUP(\\src\\Manager "^.*Manager/.*$")
SOURCE_GROUP(\\src\\Importer "^.*Importer/.*$")
SOURCE_GROUP(\\src\\Model "^.*Model/.*$")
SOURCE_GROUP(\\src\\Util "^.*Util/.*$")
SOURCE_GROUP(\\src\\Layout "^.*Layout/.*$")
SOURCE_GROUP(\\src\\QOSG "^.*QOSG/.*$")
SOURCE_GROUP(\\src\\OsgQtBrowser "^.*OsgQtBrowser/.*$")
SOURCE_GROUP(\\src\\Noise "^.*Noise/.*$")
# Group header files
SOURCE_GROUP(\\headers\\Network "^.*Network/.*h$")
SOURCE_GROUP(\\headers\\Viewer "^.*Viewer/.*h$")
SOURCE_GROUP(\\headers\\Core "^.*Core/.*h$")
SOURCE_GROUP(\\headers\\Data "^.*Data/.*h$")
SOURCE_GROUP(\\headers\\Manager "^.*Manager/.*h$")
SOURCE_GROUP(\\headers\\Importer "^.*Importer/.*h$")
SOURCE_GROUP(\\headers\\Model "^.*Model/.*h$")
SOURCE_GROUP(\\headers\\Util "^.*Util/.*h$")
SOURCE_GROUP(\\headers\\Layout "^.*Layout/.*h$")
SOURCE_GROUP(\\headers\\QOSG "^.*QOSG/.*h$")
SOURCE_GROUP(\\headers\\Noise "^.*Noise/.*h$")
SOURCE_GROUP(\\headers\\OsgQtBrowser "^.*OsgQtBrowser/.*h$")
# Group generated moc files
SOURCE_GROUP(\\MOC "^.*moc_.*$")



# .h subor, ktory obsahuje Q_OBJECT
SET(SOURCES_H
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
# toto makro spracuje Q_OBJECT a vygeneruje novy cpp subor, ktory bude dostupny v ${SOURCES_H_MOC} premennej
QT4_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} )


# Build and link
ADD_EXECUTABLE( ${3DSOFTVIZ_NAME} ${INCL} ${SRC} ${SOURCES_H_MOC} )

TARGET_LINK_LIBRARIES( ${3DSOFTVIZ_NAME}
	${QT_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
	#${PNG_LIBRARIES}
	#${ZLIB_LIBRARIES}
	noise
)

cotire(${3DSOFTVIZ_NAME})

# -------
# Install
# -------

# 3DSoftviz and data
install ( TARGETS ${3DSOFTVIZ_NAME}
  BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
  RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

INSTALL(DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data)



# Include QT4 libraries in Apple bundle and on Windows
if ( WIN32 OR APPLE )
  # On Apple plugins need to be bundled
  if ( APPLE )
		# Install Qt Plugins
		install ( DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
		  DESTINATION ${INSTALL_PLUGIN}/plugins
		  COMPONENT Runtime )

		# Install Qt Config
		install ( CODE "
		  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF}/qt.conf\" \"\")
		  " COMPONENT Runtime)
  endif ()

  # Search Dirs
  string ( REPLACE "/lib" "/bin" QT_RUNTIME_DIR ${QT_LIBRARY_DIR} )
  set ( BUNDLE_DIRS ${QT_LIBRARY_DIR} ${QT_RUNTIME_DIR} ${DEP_BIN}/bin ${DEP_BIN}/lib )

  # Bundle libraries
  install ( CODE "
		file ( GLOB_RECURSE LUA_PLUGINS
				\"\${CMAKE_INSTALL_PREFIX}/${INSTALL_DEPS}/lib/lua/*${CMAKE_SHARED_MODULE_SUFFIX}\" )
		file ( GLOB_RECURSE BUNDLE_PLUGINS
				\"\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
		include ( BundleUtilities )
		fixup_bundle ( \"${BUNDLE_APP}\" \"\${BUNDLE_PLUGINS};\${LUA_PLUGINS}\" \"${BUNDLE_DIRS}\" )
		" COMPONENT Runtime )
endif ()

# -------
# Packing
# -------

set ( CPACK_MONOLITHIC_INSTALL 1 )
set ( CPACK_STRIP_FILES ON )
set ( CPACK_BINARY_DRAGNDROP ON )
set ( CPACK_PACKAGE_VERSION_MAJOR "${3DSOFTVIZ_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR "${3DSOFTVIZ_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH "${3DSOFTVIZ_VERSION_PATCH}" )
set ( CPACK_PACKAGE_VERSION "${3DSOFTVIZ_VERSION}" )
set ( CPACK_PACKAGE_VENDOR "${3DSOFTVIZ_VENDOR}" )
set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${3DSOFTVIZ_README}" )
#set ( CPACK_COMPONENTS_ALL Runtime Dependencies Data )
include ( CPack )



