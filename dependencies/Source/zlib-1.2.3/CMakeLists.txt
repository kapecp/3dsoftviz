# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(zlib C)

# CMake configuration:
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Where to install module parts:
#~ SET (INSTALL_DOC doc CACHE PATH "Recommended directory to install documentation into.")
#~ SET (INSTALL_LIB lib CACHE PATH "Directory for external libraries.")
#~ SET (INSTALL_INC include CACHE PATH "Directory for library headers.")


# Platform checks
INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
	ADD_DEFINITIONS(-DHAVE_UNISTD_H)
ENDIF(HAVE_UNISTD_H)

IF(WIN32)
	ADD_DEFINITIONS(-DZLIB_DLL)
ENDIF(WIN32)
IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

# ZLib Library
SET(ZLIB_SRCS
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzio.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
	)

ADD_LIBRARY(z SHARED ${ZLIB_SRCS})
#~ ADD_LIBRARY(z_static STATIC ${ZLIB_SRCS})
#~ SET_TARGET_PROPERTIES(z_static PROPERTIES OUTPUT_NAME z CLEAN_DIRECT_OUTPUT 1)

INCLUDE(CTest)
IF(BUILD_TESTING)
#~ 	ADD_EXECUTABLE(example example.c)
#~ 	TARGET_LINK_LIBRARIES(example z_static)
#~ 	ADD_EXECUTABLE(minigzip minigzip.c)
#~ 	TARGET_LINK_LIBRARIES(minigzip z_static)

#~ 	ADD_TEST(example example)
	#~2DO: check "echo hello world | ./minigzip | ./minigzip -d" succeeds
ENDIF(BUILD_TESTING)

#~ INSTALL(TARGETS z z_static DESTINATION ${INSTALL_LIB})
#~ INSTALL(TARGETS z  DESTINATION ${INSTALL_LIB})
#~ INSTALL(FILES zlib.h zconf.h DESTINATION ${INSTALL_INC})
#~ INSTALL(FILES FAQ ChangeLog INDEX README zlib.3 algorithm.txt old/zlib.html win32/DLL_FAQ.txt DESTINATION ${INSTALL_DOC})

