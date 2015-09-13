#include "Layout/Shape_AbstractSphere.h"

namespace Layout {

Shape_AbstractSphere::Shape_AbstractSphere(
	const osg::Vec3f center,
	const float radius
) :	center_( center ),
	radius_( radius )
{
	setAlpha( 0.2f );
	setRenderType( SOLID );
}

osg::Vec3f& Shape_AbstractSphere::getCenter()
{
	return center_;
}

float Shape_AbstractSphere::getRadius()
{
	return radius_;
}

} // namespace
