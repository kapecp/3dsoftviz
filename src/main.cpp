#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

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
	LOG(INFO) << "3DSoftViz started.";

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

	QApplication app( argc, argv );
	new Util::Cleaner( &app );
	AppCore::Core::getInstance( &app );
	Manager::GraphManager::getInstance();
}

