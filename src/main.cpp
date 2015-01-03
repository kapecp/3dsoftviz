#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>
Q_DECLARE_METATYPE(cv::Mat)
#endif

#include <osg/Vec3d>
#include <osg/Quat>


Q_DECLARE_METATYPE( osg::Vec3d )
Q_DECLARE_METATYPE( osg::Quat )
Q_DECLARE_METATYPE(Qt::MouseButton)

int main(int argc, char *argv[])
{
    qRegisterMetaType< osg::Vec3d >("osgVec3d");
    qRegisterMetaType< osg::Quat >("osgQuat");
#ifdef OPENCV_FOUND
qRegisterMetaType<cv::Mat>("Mat");
#endif
    qRegisterMetaType<Qt::MouseButton>("MouseButton");

    QApplication app(argc, argv);
    new Util::Cleaner(&app);
    AppCore::Core::getInstance(&app);


}

