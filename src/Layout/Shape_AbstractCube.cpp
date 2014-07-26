#include "Layout/Shape_AbstractCube.h"

namespace Layout {

Shape_AbstractCube::Shape_AbstractCube (
		const osg::Vec3f center,
        const osg::Vec3f surfaceX,
        const osg::Vec3f surfaceY,
        const osg::Vec3f surfaceZ
        ) :	center_ (center), surfaceX_ (surfaceX), surfaceY_ (surfaceY), surfaceZ_ (surfaceZ)
{
	// nothing
}

osg::Vec3f & Shape_AbstractCube::getCenter () {
	return center_;
}

osg::Vec3f & Shape_AbstractCube::getSurfaceX () {
    return surfaceX_;
}

osg::Vec3f & Shape_AbstractCube::getSurfaceY () {
    return surfaceY_;
}

osg::Vec3f & Shape_AbstractCube::getSurfaceZ () {
    return surfaceZ_;
}

} // namespace
