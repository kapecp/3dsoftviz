# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT( libnoise)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)


# Determine install host
IF ( WIN32 AND NOT CYGWIN)
	IF(MSVC)
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE )
		SET(DEF_FILE src/win32/libnoise.def)
		SET(DLL_RC_FILE src/noise.rc)
	ENDIF()
ELSE ( )
ENDIF ( )

# Build Libraries
INCLUDE_DIRECTORIES(src src/model src/module)
FILE(GLOB_RECURSE SRC "src/model/*.cpp")
FILE(GLOB_RECURSE SRC2 "src/module/*.cpp")

ADD_LIBRARY ( noise STATIC src/latlon.cpp src/noisegen.cpp ${SRC} ${SRC2})


