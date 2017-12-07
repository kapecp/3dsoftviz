#-------------------------------- Header files -------------------------------
# cmake modul generating a list of header files

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
#file( GLOB_RECURSE HEADER_GITLIB		"include/Repository/Git/GitLib/*.h" )
file( GLOB_RECURSE HEADER_SPECIALMATRIX		"include/SpecialMatrix/*.h" )
file( GLOB_RECURSE HEADER_CITY		"include/City/*.h" )
file( GLOB_RECURSE HEADER_SHAPES	"include/Shapes/*.h" )

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
#file( GLOB_RECURSE HEADER_LEAPLIB	"include/Leap/LeapLib/*.h" )
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

