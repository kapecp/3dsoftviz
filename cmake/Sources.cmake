#-------------------------------- Source files -------------------------------
# cmake modul generating a list of source files

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
#file( GLOB_RECURSE SRC_GITLIB   "src/Repository/Git/GitLib/*.cpp" )
file( GLOB_RECURSE SRC_SPECIALMATRIX   "src/SpecialMatrix/*.cpp" )
file( GLOB_RECURSE SRC_CITY		"src/City/*.cpp" )
file( GLOB_RECURSE SRC_SHAPES	"src/Shapes/*.cpp" )

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
#file ( GLOB_RECURSE SRC_LEAPLIB "src/Leap/LeapLib/*.cpp" )
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

