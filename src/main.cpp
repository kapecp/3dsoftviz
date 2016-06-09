#include <QtGlobal>

#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"
#include "Application/Application.h"

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>
Q_DECLARE_METATYPE( cv::Mat )
#endif

#include <osg/Vec3d>
#include <osg/Quat>

Q_DECLARE_METATYPE( osg::Vec3d )
Q_DECLARE_METATYPE( osg::Quat )
Q_DECLARE_METATYPE( Qt::MouseButton )

#include "iostream"
#include "LuaInterface/LuaInterface.h"

#include "easylogging++.h"
// ** FOLLOWING LINE SHOULD BE USED ONCE AND ONLY ONCE IN WHOLE APPLICATION **
// ** THE BEST PLACE TO PUT THIS LINE IS IN main.cpp RIGHT AFTER INCLUDING easylogging++.h **
INITIALIZE_EASYLOGGINGPP

//#include "dirent.h"


int main( int argc, char* argv[] )
{
	LOG( INFO ) << "3DSoftViz started.";

	qRegisterMetaType< osg::Vec3d >( "osgVec3d" );
	qRegisterMetaType< osg::Quat >( "osgQuat" );
#ifdef OPENCV_FOUND
	qRegisterMetaType<cv::Mat>( "Mat" );
#endif
	qRegisterMetaType<Qt::MouseButton>( "MouseButton" );

//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir (".")) != NULL) {
//      /* print all the files and directories within directory */
//      while ((ent = readdir (dir)) != NULL) {
//        printf ("%s\n", ent->d_name);
//      }
//      closedir (dir);
//    } else {
//      /* could not open directory */
//      perror ("");
//      return EXIT_FAILURE;
//    }

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
	// may or may be not required for QObject::connect
	//qRegisterMetaType<XEvent>( "XEvent" );

	// Thi attribute doesn't work on Qt5 and higher, may be undefined in older Qt versions.
	// Still, so far the only cappable solution to auto-lock x11 display resource.
	// If you try to run the application without it, you're going to have a bad time... or segmentation faults whatever.
	// This is needed for 3DConnecetion mouse
#if QT_VERSION >= 0x050000
	// todo
#elseif QT_VERSION >= 0x040700
	QCoreApplication::setAttribute( Qt::AA_X11InitThreads );
#else
	// todo
#endif

#endif

	App::Application app( argc, argv );
	new Util::Cleaner( &app );
	AppCore::Core::getInstance( &app );
	Manager::GraphManager::getInstance();
}

