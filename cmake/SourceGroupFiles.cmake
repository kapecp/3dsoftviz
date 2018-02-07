#-------------------------------------- Group source files --------------------------------------
# cmake modul generating a group into which sources will be placed in project files
# source_group() relevant only for Visual Studio

source_group( \\src main.cpp )
source_group( \\src\\Application "^.*Application/.*$" )
source_group( \\src\\Network "^.*Network/.*$" )
source_group( \\src\\Viewer "^.*Viewer/.*$" )
source_group( \\src\\Core "^.*Core/.*$" )
source_group( \\src\\Data "^.*Data/.*$" )
source_group( \\src\\Manager "^.*Manager/.*$" )
source_group( \\src\\Importer "^.*Importer/.*$" )
source_group( \\src\\Model "^.*Model/.*$" )
source_group( \\src\\Util "^.*Util/.*$" )
source_group( \\src\\Layout "^.*Layout/.*$" )
source_group( \\src\\QOSG "^.*QOSG/.*$" )
source_group( \\src\\OsgQtBrowser "^.*OsgQtBrowser/.*$" )
source_group( \\src\\LuaGraph "^.*LuaGraph/.*$")
source_group( \\src\\LuaInterface "^.*LuaInterface/.*$")
source_group( \\src\\LuaTypes "^.*LuaTypes/.*$")
source_group( \\src\\Git "^.*Git/.*$" )
source_group( \\src\\SpecialMatrix "^.*SpecialMatrix/.*$" )
source_group( \\src\\City "^.*City/.*$" )
source_group( \\src\\Shapes "^.*Shapes/.*$" )

# Group header files
source_group( \\headers\\Application "^.*Application/.*h$")
source_group( \\headers\\Clustering "^.*Clustering/.*h$")
source_group( \\headers\\Network "^.*Network/.*h$" )
source_group( \\headers\\Viewer "^.*Viewer/.*h$" )
source_group( \\headers\\Core "^.*Core/.*h$" )
source_group( \\headers\\Data "^.*Data/.*h$" )
source_group( \\headers\\Manager "^.*Manager/.*h$" )
source_group( \\headers\\Importer "^.*Importer/.*h$" )
source_group( \\headers\\Model "^.*Model/.*h$" )
source_group( \\headers\\Util "^.*Util/.*h$" )
source_group( \\headers\\Layout "^.*Layout/.*h$" )
source_group( \\headers\\QOSG "^.*QOSG/.*h$" )
source_group( \\headers\\OsgQtBrowser "^.*OsgQtBrowser/.*h$" )
source_group( \\headers\\LuaGraph "^.*LuaGraph/.*h$")
source_group( \\headers\\LuaInterface "^.*LuaInterface/.*h$")
source_group( \\headers\\LuaTypes "^.*LuaTypes/.*h$")
source_group( \\headers\\Git "^.*Git/.*h$" )
source_group( \\headers\\SpecialMatrix "^.*SpecialMatrix/.*h$" )
source_group( \\headers\\City "^.*City/.*h$" )
source_group( \\headers\\Shapes "^.*Shapes/.*h$" )

if( OpenCV_FOUND )
	source_group( \\src\\QOpenCV "^.*QOpenCV/.*$" )
	source_group( \\src\\OpenCV "^.*OpenCV/.*$" )
	source_group( \\headers\\QOpenCV "^.*QOpenCV/.*h$" )
	source_group( \\headers\\OpenCV "^.*OpenCV/.*h$" )
	source_group( \\src\\Aruco "^.*Aruco/.*$" )
	source_group( \\headers\\Aruco "^.*Aruco/.*h$" )
endif()

if( NITE2_FOUND AND OPENNI2_FOUND AND OpenCV_FOUND )
	source_group( \\src\\Kinect "^.*Kinect/.*$" )
	source_group( \\headers\\Kinect "^.*Kinect/.*h$" )
endif()

if( KINECTSDK_FOUND AND SPEECHSDK_FOUND )
	source_group( \\src\\Speech "^.*Speech/.*$" )
	source_group( \\headers\\Speech "^.*Speech/.*h$" )
endif()

#fglove
# if( FGLOVE_FOUND )
#	source_group( \\src\\Fglove "^.*Fglove/.*$" )
#	source_group( \\headers\\Fglove "^.*Fglove/.*h$" )
# endif()

# Leap
if( LEAP_FOUND )
	source_group( \\src\\Leap "^.*Leap/.*$" )
	source_group( \\headers\\Leap "^.*Leap/.*h$" )
endif()

# MOUSE3D
if( MOUSE3D_FOUND )
	source_group( \\src\\Mouse3d "^.*Mouse3d/.*$" )
	source_group( \\headers\\Mouse3d "^.*Mouse3d/.*h$" )
endif()

