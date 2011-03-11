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
	// normalize (change the point as if the sphere center_ was [0, 0, 0])
	osg::Vec3f pointNormalized = osg::Vec3f (point - center_);

	int baseCoordIndex = -1;
	bool baseCoordNegative;
	for (int i = 0; i < 3; ++i) {
		if (pointNormalized[i] != 0) {
			baseCoordIndex = i;
			baseCoordNegative = (pointNormalized[i] < 0);
		}
	}

	osg::Vec3f changedPointNormalized;
	if (baseCoordIndex == -1) {
		changedPointNormalized = pointNormalized;
	} else {
		osg::Vec3f multipliers;
		for (int i = 0; i < 3; ++i) {
			multipliers[i] = pointNormalized[i] / pointNormalized[baseCoordIndex];
		}

		float multipliersPowSum = 0;
		for (int i = 0; i < 3; ++i) {
			if (i != baseCoordIndex) {
				multipliersPowSum += pow(multipliers[i], 2);
			}
		}

		if (multipliersPowSum == 0) {
			changedPointNormalized[baseCoordIndex] = radius_;
		} else {
			changedPointNormalized[baseCoordIndex] = sqrt(pow(radius_, 2) / multipliersPowSum);
		}

		if (baseCoordNegative) {
			changedPointNormalized[baseCoordIndex] = -changedPointNormalized[baseCoordIndex];
		}

		for (int i = 0; i < 3; ++i) {
			if (i != baseCoordIndex) {
				changedPointNormalized[i] = changedPointNormalized[baseCoordIndex] * multipliers[i];
			}
		}
	}

	// de-normalize
	return (changedPointNormalized + center_);
}

} // namespace
