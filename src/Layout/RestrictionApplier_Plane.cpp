#include "Layout/RestrictionApplier_Plane.h"

namespace Layout {

RestrictionApplier_Plane::RestrictionApplier_Plane (
	const osg::Vec3f normalVector,
	const float d
) :	normalVector_ (normalVector),
    d_ (d)
{
	// nothing
}

osg::Vec3f RestrictionApplier_Plane::applyRestriction (
	const osg::Vec3f &point
) {
	float t;
	float m = d_ - normalVector_.x () * point.x () - normalVector_.y () * point.y () - normalVector_.z () * point.z ();
	float n = pow (normalVector_.x (), 2) + pow (normalVector_.y (), 2) + pow (normalVector_.z (), 2);
	if (n != 0.0) {
		t = m / n;
	} else {
		t = 0;
	}

	return osg::Vec3f(
		point.x () + t * normalVector_.x (),
		point.y () + t * normalVector_.y (),
		point.z () + t * normalVector_.z ()
	);
}

} // namespace
