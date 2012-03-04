#include "Network/Helper.h"
#include <osg/Matrix>

namespace Network {

osg::Quat Helper::lookAt(osg::Vec3 from, osg::Vec3 to){
    osg::Quat quad;
    osg::Matrix matrix;
    matrix.makeLookAt(from, to, osg::Z_AXIS);
    matrix.get(quad);
    return quad.inverse();
}

}
