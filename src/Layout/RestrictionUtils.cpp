#include "Layout/RestrictionUtils.h"
//-----------------------------------------------------------------------------
#include <cstdlib>
//-----------------------------------------------------------------------------

namespace Layout {

osg::Vec3f RestrictionUtils::toSphere (
	const osg::Vec3f center,
	const float radius,
	const osg::Vec3f &point
) {
	// change the point as if the sphere center_ was [0, 0, 0]
	osg::Vec3f pointMoved = (point - center);

	osg::Vec3f changedPointMoved = pointMoved;

	if (changedPointMoved.length() == 0.0) {
		// create random point
		for (int i = 0; i < 3; ++i) {
			changedPointMoved[i] = (random() % 100) + 1;
		}
	}
	changedPointMoved.normalize();
	changedPointMoved *= radius;

	osg::Vec3f changedPoint = changedPointMoved + center;

	return (changedPoint);
}

} // namespace
