# Copyright (C) 2013-2014 3DSoftviz.
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
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

# Warnings for Debug mode
option( USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON )
if( USE_STRICT_COMPILE_WARNINGS )
	if( CMAKE_COMPILER_IS_GNUCXX )
#~ 		set( CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror" )
		set( CMAKE_CXX_FLAGS_DEBUG "-pedantic -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-declarations -Wmissing-include-dirs -Wuninitialized -Werror-implicit-function-declaration -Wfloat-equal -Wundef -Wstrict-overflow=5 -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Wno-unused-parameter" )
			# use to suppress unused stuff: -Wno-unused-parameter -Wno-unused-result
			# only for C: -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes
			# not used (too many warnings): -Wshadow -Waggregate-return
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
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE CACHE STRING "" FORCE )	# info: changed to TRUE for cotire unity build - works!
set( CMAKE_INSTALL_RPATH "$ORIGIN/../lib" CACHE STRING "" FORCE )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
set( CMAKE_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "" FORCE )

# --------------
# Build settings - COTIRE module
# --------------

include(cotire)
if (COMMAND cotire)
	if( NOT COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES )
		include( ProcessorCount )
		ProcessorCount( N )

		if( NOT N EQUAL 0 )
			set( COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j${N}" )
		endif()
	endif()

	set( COTIRE_VERBOSE FALSE )
	set( COTIRE_DEBUG FALSE )
endif()

# ------------
# Dependencies - External projects
# ------------

# Build dependencies by default
option( USE_BUILTIN_DEPENDENCIES "Use builtin dependencies." ON )

# Find OpenCV
FIND_PACKAGE(OpenCV)

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

	#add Libnoise library
	add_library(libnoise SHARED IMPORTED)
	add_dependencies(libnoise  dep_libnoise)
	set_target_properties(libnoise PROPERTIES
		IMPORTED_LOCATION  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX} )

	if( WIN32 )
		set_target_properties(libnoise PROPERTIES
			IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
			IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

		if( MSVC )
			# set "lib" prefix for MSVC, becasue libnoise.def files requires that the library is named "libnoise"
			set( MISS_MSVC_PREFIX lib )
			set_target_properties(libnoise PROPERTIES
				IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${MISS_MSVC_PREFIX}noise${CMAKE_STATIC_LIBRARY_SUFFIX} )

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

		add_library(aruco SHARED IMPORTED)
		add_dependencies(aruco dep_aruco)
		set_target_properties( aruco PROPERTIES
		IMPORTED_LOCATION ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX} )

		if( WIN32 )
			set_target_properties(aruco PROPERTIES
				IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX}
				IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_STATIC_LIBRARY_SUFFIX} )

			if( MSVC )
				set_target_properties(aruco PROPERTIES
					IMPORTED_LOCATION  ${DEP_BIN}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_SHARED_LIBRARY_SUFFIX}
					IMPORTED_IMPLIB  ${DEP_BIN}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}aruco${CMAKE_STATIC_LIBRARY_SUFFIX} )
			endif()
		endif()
	endif()  # OpenCV_FOUND


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
if( OpenCV_FOUND )
				if ( WIN32 )
					add_definitions( -DNOMINMAX )
				endif()

		# include_directories( ${OpenCV_INCLUDE_DIRS} )

		add_definitions( -DOPENCV_FOUND )
		MESSAGE( STATUS "OpenCV_LIB_DIR=${OpenCV_LIB_DIR}")
		MESSAGE( STATUS "OpenCV_LIB_DIR=${OpenCV_LIBS}")

		file(GLOB_RECURSE SRC_OPENCV  "src/OpenCV/*.cpp")
		file(GLOB_RECURSE SRC_QOPENCV  "src/QOpenCV/*.cpp")
		file(GLOB_RECURSE HEADER_OPENCV  "include/OpenCV/*.h")
		file(GLOB_RECURSE HEADER_QOPENCV  "include/QOpenCV/*.h")
		set(SRC ${SRC} ${SRC_OPENCV} ${SRC_QOPENCV})

		#aruco
		file(GLOB_RECURSE SRC_ARUCO  "src/Aruco/*.cpp")
		file(GLOB_RECURSE HEADER_ARUCO  "include/Aruco/*.h")
		set(SRC ${SRC} ${SRC_ARUCO} )

else()
		MESSAGE( STATUS "OpenCV Not Found")
endif()

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



# Find OPENNI2
if ( WIN32 )
	if ( MSVC )
		find_package( OPENNI2 )
		if (OPENNI2_FOUND)
		include_directories( ${OPENNI2_INCLUDE_DIRS} )
		add_definitions( -DOPENNI2_FOUND )
		MESSAGE ( STATUS "OpenNI2 FOUND")
		file(GLOB_RECURSE SRC_KINECT  "src/Kinect/*.cpp")
		file(GLOB_RECURSE HEADER_KINECT  "include/Kinect/*.h")
		else(OPENNI2_FOUND)
		MESSAGE ( STATUS "OpenNI2 NOT FOUND")
		endif()
	endif()
endif()

# Find NITE2
if ( WIN32 )
	if ( MSVC )
		find_package( NITE2 )
		if (NITE2_FOUND)
		include_directories( ${NITE2_INCLUDE_DIRS} ${NITE2_OPENNI2_DRIVER_DIRS} )
		add_definitions( -DNITE2_FOUND )
		message (STATUS "NITE2 FOUND")
		else(NITE2_FOUND)
		message (STATUS "NITE2 NOT FOUND")
		endif()
	endif()
endif()

# Find KINECT SDK
add_definitions(-DNOMINMAX)
if ( WIN32 )
        if ( MSVC )
                find_package( KINECTSDK )
                if (KINECTSDK_FOUND)
                include_directories( ${KINECTSDK_INCLUDE_DIRS} )
                add_definitions( -DKINECTSDK_FOUND )
                find_package( SPEECHSDK )
                if (SPEECHSDK_FOUND)
                    file(GLOB_RECURSE SRC_SPEECH  "src/Speech/*.cpp")
                    file(GLOB_RECURSE HEADER_SPEECH  "include/Speech/*.h")
                    include_directories( ${SPEECHSDK_INCLUDE_DIRS} )
                    add_definitions( -DSPEECHSDK_FOUND )
                    message (STATUS "SPEECHSDK FOUND")
                    else(SPEECHSDK_FOUND)
                    message (STATUS "SPEECHSDK NOT FOUND")
                endif()
                message (STATUS "KINECTSDK FOUND")
                else(KINECTSDK_FOUND)
                message (STATUS "KINECTSDK NOT FOUND")
                endif()
        endif()
endif()

# ---------------
# 3DSoftviz sources
# ---------------

# Include Paths
include_directories(SYSTEM
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}

	${DEPENDENCIES_INCLUDE_DIR}
	${DEPENDENCIES_INCLUDE_DIR}/libnoise
)
if( OpenCV_FOUND )
	include_directories(SYSTEM
		${DEPENDENCIES_INCLUDE_DIR}/aruco
		${OpenCV_INCLUDE_DIRS}
	)
endif()


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Source files
set(SRC_MAIN "src/main.cpp")
file(GLOB_RECURSE SRC_CORE  "src/Core/*.cpp")
file(GLOB_RECURSE SRC_DATA  "src/Data/*.cpp")
file(GLOB_RECURSE SRC_IMPORTER  "src/Importer/*.cpp")
file(GLOB_RECURSE SRC_LAYOUT  "src/Layout/*.cpp")
file(GLOB_RECURSE SRC_MANAGER  "src/Manager/*.cpp")
file(GLOB_RECURSE SRC_MATH  "src/Math/*.cpp")
file(GLOB_RECURSE SRC_MODEL  "src/Model/*.cpp")
file(GLOB_RECURSE SRC_NETWORK  "src/Network/*.cpp")
file(GLOB_RECURSE SRC_NOISE  "src/Noise/*.cpp")
file(GLOB_RECURSE SRC_OSGQTBROWSER  "src/OsgQtBrowser/*.cpp")
file(GLOB_RECURSE SRC_QOSG  "src/QOSG/*.cpp")
file(GLOB_RECURSE SRC_UTIL  "src/Util/*.cpp")
file(GLOB_RECURSE SRC_VIEWER  "src/Viewer/*.cpp")

# Source files
set(SRC ${SRC}
	${SRC_MAIN} ${SRC_CORE} ${SRC_DATA} ${SRC_IMPORTER}
	${SRC_LAYOUT} ${SRC_MANAGER} ${SRC_MATH}
	${SRC_NETWORK} ${SRC_NOISE}
	${SRC_OSGQTBROWSER} ${SRC_QOSG} ${SRC_UTIL} ${SRC_VIEWER} ${SRC_MODEL}
)

if( OpenCV_FOUND )
	list(APPEND SRC ${SRC_OPENCV} ${SRC_QOPENCV})
endif()
if (OPENNI2_FOUND)
	list(APPEND SRC ${SRC_KINECT})
endif()
if (KINECTSDK_FOUND)
    if (SPEECHSDK_FOUND)
        list(APPEND SRC ${SRC_SPEECH})
    endif()
endif()

file(GLOB_RECURSE HEADER_CORE  "include/Core/*.h")
file(GLOB_RECURSE HEADER_DATA  "include/Data/*.h")
file(GLOB_RECURSE HEADER_IMPORTER  "include/Importer/*.h")
file(GLOB_RECURSE HEADER_LAYOUT  "include/Layout/*.h")
file(GLOB_RECURSE HEADER_MANAGER  "include/Manager/*.h")
file(GLOB_RECURSE HEADER_MATH  "include/Math/*.h")
file(GLOB_RECURSE HEADER_MODEL  "include/Model/*.h")
file(GLOB_RECURSE HEADER_NETWORK  "include/Network/*.h")
file(GLOB_RECURSE HEADER_NOISE  "include/Noise/*.h")
file(GLOB_RECURSE HEADER_OSGQTBROWSER  "include/OsgQtBrowser/*.h")
file(GLOB_RECURSE HEADER_QOSG  "include/QOSG/*.h")
file(GLOB_RECURSE HEADER_UTIL  "include/Util/*.h")
file(GLOB_RECURSE HEADER_VIEWER  "include/Viewer/*.h")

set(INCL
	${HEADER_CORE} ${HEADER_DATA} ${HEADER_IMPORTER}
	${HEADER_LAYOUT} ${HEADER_MANAGER} ${HEADER_MATH}
	${HEADER_NETWORK} ${HEADER_NOISE}
	${HEADER_OSGQTBROWSER} ${HEADER_QOSG} ${HEADER_UTIL} ${HEADER_VIEWER} ${HEADER_MODEL}
)
if( OpenCV_FOUND )
		list(APPEND INCL ${HEADER_OPENCV} ${HEADER_QOPENCV} ${HEADER_ARUCO})
endif()
if (OPENNI2_FOUND)
		list(APPEND INCL ${HEADER_KINECT})
endif()
if (KINECTSDK_FOUND)
    if (SPEECHSDK_FOUND)
        list(APPEND INCL ${HEADER_SPEECH})
    endif()
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

if(OpenCV_FOUND)
	source_group(\\src\\QOpenCV "^.*QOpenCV/.*$")
	source_group(\\src\\OpenCV "^.*OpenCV/.*$")
	source_group(\\headers\\QOpenCV "^.*QOpenCV/.*h$")
	source_group(\\headers\\OpenCV "^.*OpenCV/.*h$")
	source_group(\\src\\Aruco "^.*Aruco/.*$")
	source_group(\\headers\\Aruco "^.*Aruco/.*h$")
endif()
if (OPENNI2_FOUND)
	source_group(\\src\\Kinect "^.*Kinect/.*$")
	source_group(\\headers\\Kinect "^.*Kinect/.*h$")
endif()
if (KINECTSDK_FOUND)
    if (SPEECHSDK_FOUND)
        source_group(\\src\\Speech "^.*Speech/.*$")
        source_group(\\headers\\Speech "^.*Speech/.*h$")
    endif()
endif()

# ---------------
# Build and link 3Dsoftviz
# ---------------


# Build and link
add_executable( ${3DSOFTVIZ_NAME} ${INCL} ${SRC} )

target_link_libraries( ${3DSOFTVIZ_NAME}
	${QT_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
	${OSGVIEWER_LIBRARIES}
	libnoise
)

if(OpenCV_FOUND)
	target_link_libraries( ${3DSOFTVIZ_NAME}
		${OpenCV_LIBS}
		aruco
	)
endif()

if (NITE2_FOUND AND OPENNI2_FOUND)
	target_link_libraries( ${3DSOFTVIZ_NAME} ${NITE2_LIBRARIES} ${OPENNI2_LIBRARIES} )
endif()

# KINECT SDK
if (KINECTSDK_FOUND)
    target_link_libraries( ${3DSOFTVIZ_NAME} ${KINECTSDK_LIBRARIES})
    if (SPEECHSDK_FOUND)
        target_link_libraries( ${3DSOFTVIZ_NAME} ${SPEECHSDK_LIBRARIES})
    endif()
endif()

# 'cotire' the executable
if( COMMAND cotire )

	# this needs CMake 2.8.11 and greater
	if(${CMAKE_VERSION} VERSION_GREATER "2.8.10")
		set_target_properties(${3DSOFTVIZ_NAME} PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY")
	endif()

	cotire( ${3DSOFTVIZ_NAME} )

	if(${CMAKE_VERSION} VERSION_LESS "2.8.11")
		target_link_libraries(${3DSOFTVIZ_NAME}_unity
			${QT_LIBRARIES}
			${OPENSCENEGRAPH_LIBRARIES}
			${OSGVIEWER_LIBRARIES}
			libnoise
		)
# toto kuknut este
		if(OpenCV_FOUND)
			target_link_libraries( ${3DSOFTVIZ_NAME}_unity aruco )
		endif()
	endif()

if(OpenCV_FOUND)
		target_link_libraries( ${3DSOFTVIZ_NAME}_unity ${OpenCV_LIBS})
	endif()

	if( MSVC )
		### Fix virtual memory range
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200" CACHE STRING "" FORCE)
	endif()
endif()

# ensure correct dependencies





add_dependencies( ${3DSOFTVIZ_NAME} libnoise)
if( OpenCV_FOUND )
		add_dependencies( ${3DSOFTVIZ_NAME} aruco )
endif()

# -------
# Install
# -------

# 3DSoftviz and data
install( TARGETS ${3DSOFTVIZ_NAME}
	BUNDLE DESTINATION ${INSTALL_BUNDLE} COMPONENT Runtime
	RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT Runtime )

# add custom install_unity target
if (COMMAND cotire)
	add_custom_target(install_unity
		COMMAND ${CMAKE_COMMAND} -P "cmake_install.cmake")
	add_dependencies(install_unity ${3DSOFTVIZ_NAME}_unity)
endif()

# install resources
install(DIRECTORY resources/ DESTINATION ${INSTALL_DATA} COMPONENT Data)

# Install needed .dll files for OpenNI2
if (NITE2_FOUND AND OPENNI2_FOUND)
	install(DIRECTORY ${NITE2_OPENNI2_DRIVER_DIRS} DESTINATION ${INSTALL_BIN}/OpenNI2)
endif()

# install .dll for speech recognition
if (SPEECHSDK_FOUND)
        install(DIRECTORY ${SPEECHSDK_DRIVER_DIRS}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/bin)
endif()

# Install built in dependencies unless we are building an OSX Bundle
if( USE_BUILTIN_DEPENDENCIES )

	# Make sure dependencies are built
		add_dependencies( ${3DSOFTVIZ_NAME} libnoise )

		if( OpenCV_FOUND )
			add_dependencies( ${3DSOFTVIZ_NAME} aruco )

		endif()


	# Install all dependencies
	install( DIRECTORY ${DEP_BIN}/
		DESTINATION ${INSTALL_DEPS}
		COMPONENT Dependencies
		USE_SOURCE_PERMISSIONS )
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
