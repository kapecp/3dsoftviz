#include "Layout/RestrictionApplier_Sphere.h"
//-----------------------------------------------------------------------------
#include "Layout/RestrictionUtils.h"
//-----------------------------------------------------------------------------

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
	if ((point - center_).length() > radius_) {
		return RestrictionUtils::toSphere (center_, radius_, point);
	} else {
		return point;
	}
}

} // namespace
