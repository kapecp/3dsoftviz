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
	try {
		AppCore::Core::getInstance( &app );
	}
	catch ( std::runtime_error& e ) {
		LOG( ERROR ) << e.what();

		auto res = Lua::LuaInterface::getInstance()->doString( "print(debug.traceback())" );
		// TODO/BUG: debug.traceback() does not return anything, somehow the traceback is empty
		// maybe Diluculum changes something during exception handling...
		// this way we could obtain the results from debug.traceback()
		// LOG( ERROR ) << res.at(0).asString();
		// see http://stackoverflow.com/questions/12256455/print-stacktrace-from-c-code-with-embedded-lua
		// for possible solution
	}

	Manager::GraphManager::getInstance();
}

