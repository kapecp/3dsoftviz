#ifndef __3DVisualNetworkHelper_H__
#define __3DVisualNetworkHelper_H__

#include <osg/Quat>
#include <osg/Vec3>
#include <osg/PositionAttitudeTransform>
#include <QString>

namespace Network {
    class Helper{
    public:
        static osg::Quat lookAt(osg::Vec3 from, osg::Vec3 to);
        static osg::PositionAttitudeTransform * generateAvatar(QString label);
    };
}

#endif
