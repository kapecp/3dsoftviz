#ifndef __3DVisualNetworkHelper_H__
#define __3DVisualNetworkHelper_H__

#include <osg/Quat>
#include <osg/Vec3>

namespace Network {
    class Helper {
    public:
        static osg::Quat lookAt(osg::Vec3 from, osg::Vec3 to);
    };
}

#endif
