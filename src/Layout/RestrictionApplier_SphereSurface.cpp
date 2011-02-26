#include "Layout/RestrictionApplier_SphereSurface.h"

namespace Layout {

RestrictionApplier_SphereSurface::RestrictionApplier_SphereSurface (
	const osg::Vec3f center,
	const float radius
) :	center_ (center),
	radius_ (radius)
{
	// nothing
}

osg::Vec3f RestrictionApplier_SphereSurface::applyRestriction (
	const osg::Vec3f &point
) {
	// TODO: implement this
	return point;
}

} // namespace
