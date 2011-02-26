#include "Layout/RestrictionApplier_Sphere.h"

namespace Layout {

RestrictionApplier_Sphere::RestrictionApplier_Sphere (
	const osg::Vec3f center,
	const float radius
) :	center_ (center),
	radius_ (radius)
{
	// nothing
}

osg::Vec3f RestrictionApplier_Sphere::applyRestriction (
	const osg::Vec3f &point
) {
	// TODO: implement this
	return point;
}

} // namespace
