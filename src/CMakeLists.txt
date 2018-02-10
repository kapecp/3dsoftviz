#-------------------------------- Source files -------------------------------
# cmake module generating a list of source files

set( SRC_MAIN "src/main.cpp" PARENT_SCOPE)
file( GLOB_RECURSE SRC_APP		"Application/*.cpp" )
file( GLOB_RECURSE SRC_CORE		"Core/*.cpp" )
file( GLOB_RECURSE SRC_DATA		"Data/*.cpp" )
file( GLOB_RECURSE SRC_IMPORTER	"Importer/*.cpp" )
file( GLOB_RECURSE SRC_LAYOUT	"Layout/*.cpp" )
file( GLOB_RECURSE SRC_LUAGRAPH  "LuaGraph/*.cpp")
file( GLOB_RECURSE SRC_LUAINTERFACE  "LuaInterface/*.cpp")
file( GLOB_RECURSE SRC_LUATYPES  "LuaTypes/*.cpp")
file( GLOB_RECURSE SRC_MANAGER	"Manager/*.cpp" )
file( GLOB_RECURSE SRC_MATH		"Math/*.cpp" )
file( GLOB_RECURSE SRC_MODEL	"Model/*.cpp" )
file( GLOB_RECURSE SRC_NETWORK	"Network/*.cpp" )
file( GLOB_RECURSE SRC_OSGQTBROWSER	"OsgQtBrowser/*.cpp" )
file( GLOB_RECURSE SRC_QOSG		"QOSG/*.cpp" )
file( GLOB_RECURSE SRC_UTIL		"Util/*.cpp" )
file( GLOB_RECURSE SRC_VIEWER	"Viewer/*.cpp" )
file( GLOB_RECURSE SRC_CLUSTERING	"Clustering/*.cpp")
file( GLOB SRC_REPOSITORY		"Repository/Git/*.cpp" )
#file( GLOB_RECURSE SRC_GITLIB   "Repository/Git/GitLib/*.cpp" )
file( GLOB_RECURSE SRC_SPECIALMATRIX   "SpecialMatrix/*.cpp" )
file( GLOB_RECURSE SRC_CITY		"City/*.cpp" )
file( GLOB_RECURSE SRC_SHAPES	"Shapes/*.cpp" )

# OpenCV
file( GLOB_RECURSE SRC_OPENCV	"OpenCV/*.cpp" )
file( GLOB_RECURSE SRC_QOPENCV	"QOpenCV/*.cpp" )


# Aruco
file( GLOB_RECURSE SRC_ARUCO	"Aruco/*.cpp" )

# Kinect
file( GLOB_RECURSE SRC_KINECT	"Kinect/*.cpp" )

# SpeechSDK
file( GLOB_RECURSE SRC_SPEECH	"Speech/*.cpp" )

# FGlove
# file ( GLOB_RECURSE SRC_FGLOVE "FGlove/*.cpp" )

# Leap
file ( GLOB SRC_LEAP "Leap/*.cpp" )
#file ( GLOB_RECURSE SRC_LEAPLIB "Leap/LeapLib/*.cpp" )
file ( GLOB_RECURSE SRC_HANDMODULE "Leap/HandModule/*.cpp" )

# MOUSE3D
set ( SRC_MOUSE3D
                ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/Connector.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/EventThread.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Mouse3dDevices.cpp
                )

if( WIN32 )
        list(APPEND SRC_MOUSE3D
                        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Windows/Mouse3DInput.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Windows/MouseParameters.cpp
                        )
elseif( APPLE )
        list(APPEND SRC_MOUSE3D
                        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.cpp
                        )
elseif( UNIX )
        list(APPEND SRC_MOUSE3D
                        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Unix/ApplicationEmitter.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.cpp
                        )
endif()

# All source files
set( SRC_LIST
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
                )

if( OpenCV_FOUND )
        list( APPEND SRC_LIST ${SRC_OPENCV} ${SRC_QOPENCV} ${SRC_ARUCO} )
endif()

# add Kinect if Nite, Openni and Opencv
if( OPENNI2_FOUND AND OpenCV_FOUND )
        list( APPEND SRC_LIST ${SRC_KINECT} )
endif()

if( KINECTSDK_FOUND AND SPEECHSDK_FOUND )
        list( APPEND SRC_LIST ${SRC_SPEECH} )
endif()

# add Gloves if Fgloves
# if( FGLOVE_FOUND )
#	list( APPEND SRC_LIST ${SRC_FGLOVE} )
# endif()

# add Leap
if( LEAP_FOUND )
        # WORKAROUND GARAJ comment
        list( APPEND SRC_LIST ${SRC_LEAP} )
        list( APPEND SRC_LIST ${SRC_HANDMODULE} )
endif()

# add MOUSE3D if found
if( MOUSE3D_FOUND )
        list( APPEND SRC_LIST ${SRC_MOUSE3D} )
endif()

# add diluculum code directly due to MSVC problems
if( MSVC )
        file( GLOB_RECURSE SRC_DILUCULUM	"../dependencies/diluculum/Sources/*.cpp" )

        list( APPEND SRC_LIST ${SRC_DILUCULUM} )
endif()

#set local variables visible to main cmake
set( SRC ${SRC_LIST} PARENT_SCOPE)
