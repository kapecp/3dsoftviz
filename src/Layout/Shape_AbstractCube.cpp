#include "Layout/Shape_AbstractCube.h"

namespace Layout {

Shape_AbstractCube::Shape_AbstractCube (
		const osg::Vec3f center,
		const float radius
		) :	center_ (center),
	radius_ (radius)
{
	// nothing
}

osg::Vec3f & Shape_AbstractCube::getCenter () {
	return center_;
}

float Shape_AbstractCube::getRadius () {
	return radius_;
}

} // namespace
