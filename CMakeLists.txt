# Copyright ( C ) 2013-2016 3DSoftviz.
# Created by Peter Kapec
# Redistribution and use of this file is allowed
# according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with 3DSoftviz.

option( USE_CCACHE "Use CCache compiler cache to speed-up build." OFF)
if( USE_CCACHE )
    find_program(CCACHE_PROGRAM ccache)
    if( CCACHE_PROGRAM )
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
        message(STATUS "Using ${CCACHE_PROGRAM}")
    else()
        message(STATUS "ccache NOT FOUND")
    endif()
endif()

project( 3DSoftviz C CXX )
cmake_minimum_required( VERSION 2.8.11 )

# ------------------------------------------------------------
# -------------------3DSoftviz Info
# ------------------------------------------------------------
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

# ------------------------------------------------------------
# ----------------------Build settings------------------------
# ------------------------------------------------------------

# Additional CMake scripts
set( CMAKE_MODULE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
		${CMAKE_MODULE_PATH} )

# Enable C++11 features
set(CMAKE_CXX_STANDARD 11)

# Warnings for Debug mode
option( USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON )

cmake_policy(SET CMP0054 NEW)
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

# ------------------------------------------------------------
# -----------------Build settings - COTIRE module-------------
# ------------------------------------------------------------
option(SKIP_COTIRE "Skip Cotire module build speed-up." OFF)

if (NOT SKIP_COTIRE)
	include(cotire)
else ()
	message(STATUS "Skipped Cotire module build speed-up.")
endif ()

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
# -----Dependencies - Searching external dependencies---------
# ------------------------------------------------------------
set( ALL_SYSTEM_HEADERS )
set( ALL_COMPILE_DEFINITIONS )
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

#add dependencies with search module
include(cmake/SearchDependencies.cmake)

# ------------------------------------------------------------
# ----------Support tools - Add custom build targets----------
# ------------------------------------------------------------

#generate doxygen documentation
include(cmake/doxygen.cmake)

#generate sphinx documentation
include(cmake/sphinx.cmake)

#format souce code
include(cmake/astyle.cmake)

#check code style
include(cmake/cpplint.cmake)

#analyze all code
include(cmake/cppcheck.cmake)


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

	# set search paths to the dependencies
	set( DEP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies )
	set( DEP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dependencies )
	set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEP_BIN}/lib ${DEP_BIN}/bin )
	set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${DEP_BIN}/include )

	add_subdirectory(dependencies)

else()
	message(STATUS "Skipped using builtin external dependencies.")
	# Find Lua implementation, assuming lpeg is installed by other means
	#find_package( Lua51 REQUIRED )
endif()


# ---------------------------------------------------------------
# 3DSoftviz sources
# ---------------------------------------------------------------

#-------------------------------- Source files -------------------------------
# process src directory - get all source files
add_subdirectory(src)

# TODO: task-7199 Move "Build modules as libraries" the low in the hierarchy
file( GLOB_RECURSE SRC_GITLIB   "src/Repository/Git/GitLib/*.cpp" )
file ( GLOB_RECURSE SRC_LEAPLIB "src/Leap/LeapLib/*.cpp" )

#-------------------------------- Header files -------------------------------
# process include directory - get all header files
include(include/Headers.cmake)

# TODO: task-7199 Move "Build modules as libraries" the low in the hierarchy
file( GLOB_RECURSE HEADER_GITLIB  "include/Repository/Git/GitLib/*.h" )
file( GLOB_RECURSE HEADER_LEAPLIB "include/Leap/LeapLib/*.h" )

#-------------------------------------- Group source files --------------------------------------
# generate a group into which sources will be placed in VS project files
if (MSVC)
        include(cmake/SourceGroupFiles.cmake)
endif ()

# ---------------------------------------------------------------
# 3rd party sources
# ---------------------------------------------------------------

# noiseutils library
add_library( noiseutils STATIC dependencies/noiseutils/noiseutils.cpp )
target_include_directories( noiseutils SYSTEM PUBLIC ${DEPENDENCIES_INCLUDE_DIR}/libnoise dependencies/noiseutils dependencies/libnoise/src )

# mouse3d library
if ( UNIX AND NOT APPLE )
	add_library( mouse3d STATIC dependencies/mouse3d/xdrvlib.c )
	target_include_directories(mouse3d SYSTEM PUBLIC dependencies/mouse3d)
endif()

# qtcolorpicker library
add_library( qtcolorpicker STATIC dependencies/qtcolorpicker/qtcolorpicker.cpp )
target_include_directories( qtcolorpicker SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/qtcolorpicker )
if (Qt4_FOUND )
	target_link_libraries( qtcolorpicker Qt4::QtCore)
endif()
if ( Qt5Core_FOUND )
	qt5_use_modules( qtcolorpicker Core Widgets )
endif()

# parserlib library
# include_directories( SYSTEM dependencies/parserlib/source )
add_library( parserlib STATIC dependencies/parserlib/source/ast.cpp dependencies/parserlib/source/parser.cpp )
target_include_directories( parserlib SYSTEM PUBLIC dependencies/parserlib/source  )


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
	target_link_libraries( gitlib Qt5::Core )
elseif( Qt4_FOUND )
	target_link_libraries( gitlib Qt4::QtCore )
endif()

# 'cotire' the module
if( COMMAND cotire )
	# link gitlib_unity target to all libraries to which gitlib links
	set_target_properties( gitlib PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
	set_target_properties(gitlib PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
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
		target_link_libraries( leaplib Qt5::Core )
	elseif( Qt4_FOUND )
		target_link_libraries( leaplib ${QT_LIBRARIES} )
	endif()

	# 'leap' the module
	if( COMMAND cotire )
		# link leaplib_unity target to all libraries to which leaplib links
		set_target_properties( leaplib PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
		set_target_properties(leaplib PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
		cotire( leaplib CONFIGURATIONS Debug Release )
	endif()
endif()

# ---------------------------------------------------------------
# Build and link 3Dsoftviz
# ---------------------------------------------------------------

# 3Dsoftviz library
add_library( ${3DSOFTVIZ_NAME} STATIC ${INCL} ${SRC} )

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
list( APPEND ALL_SYSTEM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/qtcolorpicker/ )
list( APPEND ALL_SYSTEM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/leathers/Source )
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
	target_link_libraries( ${3DSOFTVIZ_NAME}
			Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Sql Qt5::WebEngine Qt5::WebEngineWidgets Qt5::Xml Qt5::Network
			${OPENGL_LIBRARIES} )
endif()

if ( Qt4_FOUND )
	#	target_link_libraries( ${3DSOFTVIZ_NAME} ${QT_LIBRARIES} )
	target_link_libraries( ${3DSOFTVIZ_NAME}
			Qt4::QtCore Qt4::QtGui Qt4::QtOpenGL Qt4::QtSql Qt4::QtWebKit Qt4::QtXml Qt4::QtNetwork
			${OPENGL_LIBRARIES} )
endif()

if( BUILD_OSGQT )
	target_link_libraries( ${3DSOFTVIZ_NAME} osgQt )
else()
	target_link_libraries( ${3DSOFTVIZ_NAME} ${OSGQT_LIBRARIES} )
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
		)

if( UNIX AND NOT APPLE )
	target_link_libraries( ${3DSOFTVIZ_NAME} mouse3d )
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

if (FGLOVE_FOUND)
	target_link_libraries(${3DSOFTVIZ_NAME} ${FGLOVE_LIBRARIES})
endif ()

if (PCL_FOUND)
	target_link_libraries(${3DSOFTVIZ_NAME} ${PCL_LIBRARIES})
endif ()

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

# 'cotire' the library
if( COMMAND cotire )
	# link ${3DSOFTVIZ_NAME}_unity target to all libraries to which ${3DSOFTVIZ_NAME} links
	set_target_properties( ${3DSOFTVIZ_NAME} PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
	set_target_properties(${3DSOFTVIZ_NAME} PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)

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
	add_dependencies( ${3DSOFTVIZ_NAME} libnoise osgModeling gitlib dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging dep_StackTracePlus dep_luasocket dep_mobdebug dep_luacov dep_luacheck dep_lua_cliargs dep_luasystem dep_dkjson dep_say dep_luassert dep_lua-term dep_penlight dep_mediator_lua dep_busted dep_luacomments )

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
		add_dependencies( ${3DSOFTVIZ_NAME}_unity libnoise osgModeling gitlib_unity dep_luametrics dep_leg dep_lpeg dep_luafilesystem igloo dep_lualogging dep_StackTracePlus dep_luasocket dep_mobdebug dep_luacov dep_luacheck dep_lua_cliargs dep_luasystem dep_dkjson dep_say dep_luassert dep_lua-term dep_penlight dep_mediator_lua dep_busted dep_luacomments )

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


if( COMMAND cotire )
	set_target_properties( ${3DSOFTVIZ_NAME}_app PROPERTIES COTIRE_UNITY_LINK_LIBRARIES_INIT "COPY_UNITY" )
	target_link_libraries( ${3DSOFTVIZ_NAME}_app ${3DSOFTVIZ_NAME}_unity )
	cotire( ${3DSOFTVIZ_NAME}_app CONFIGURATIONS Debug Release )

else()
	target_link_libraries( ${3DSOFTVIZ_NAME}_app ${3DSOFTVIZ_NAME} )
endif()

# -------------------------------------------------------
# include-what-you-use
# -------------------------------------------------------
option( USE_INCLUDE_WHAT_YOU_USE "Use include-what-you-use." OFF )
if( USE_INCLUDE_WHAT_YOU_USE )

	find_program( iwyu_path NAMES include-what-you-use iwyu )

	if( NOT iwyu_path )
		message( STATUS "Could not find the program include-what-you-use" )
	else()
		message( STATUS "Using include-what-you-use" )

		set( iwyu_path_and_options
			${iwyu_path}
			-Xiwyu
			--mapping_file=${CMAKE_CURRENT_SOURCE_DIR}/resources/iwyu/mappings/qt5_4.imp
			# TODO: use multiple mapping files (according to doc multiple --mapping_file can be used, but results in error)
			# --mapping_file=/Users/kapec/Downloads/include-what-you-use/mappings/libcxx.imp
			)

		set_property( TARGET gitlib					PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options} )
		set_property( TARGET leaplib				PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options} )
		set_property( TARGET ${3DSOFTVIZ_NAME}		PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options} )
		set_property( TARGET ${3DSOFTVIZ_NAME}_app	PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options} )
	endif()
endif()


# -------------------------------------------------------
# clang-tidy
# -------------------------------------------------------
option( USE_CLANG_TIDY "Use clang-tidy." OFF )
if( USE_CLANG_TIDY )
	find_program( clang_tidy NAMES clang-tidy PATHS /usr/local/opt/llvm/bin )

	if( NOT clang_tidy )
		message( STATUS "Could not find the program clang-tidy" )
	else()
		message( STATUS "Using clang-tidy" )

		set( clang_tidy_and_options
			${clang_tidy}
			# do not use checks:
			# to many warnings in easylogging++: -cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-pro-bounds-array-to-pointer-decay
			# include order used in LLVM, not our style: -llvm-include-order
			-checks=*,-llvm-include-order,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-pro-bounds-array-to-pointer-decay
			)

		set_property( TARGET gitlib					PROPERTY CXX_CLANG_TIDY ${clang_tidy_and_options} )
		set_property( TARGET leaplib				PROPERTY CXX_CLANG_TIDY ${clang_tidy_and_options} )
		set_property( TARGET ${3DSOFTVIZ_NAME}		PROPERTY CXX_CLANG_TIDY ${clang_tidy_and_options} )
		set_property( TARGET ${3DSOFTVIZ_NAME}_app	PROPERTY CXX_CLANG_TIDY ${clang_tidy_and_options} )
	endif()
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
	install(TARGETS ${3DSOFTVIZ_NAME}_app_unity RUNTIME DESTINATION ${INSTALL_BIN} )

	add_custom_target( install_unity
			COMMAND ${CMAKE_COMMAND} -P "cmake_install.cmake"
			COMMENT "Install the unity-built project..."
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
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
if (FGLOVE_FOUND)

	install(DIRECTORY ${FGLOVE_DRIVER_DIRS}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/bin)

	install(FILES ${FGLOVE_LIBRARIES} DESTINATION ${INSTALL_BIN}
			RUNTIME DESTINATION ${INSTALL_BIN}
			LIBRARY DESTINATION ${INSTALL_LIB})
endif ()

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
# BDD Igloo gitlib tests
# -------------------------------------------------------
add_subdirectory(tests)
