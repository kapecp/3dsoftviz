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

	// get base coordinate (some of the coordinates that != 0) and multipliers
	int baseCoord = -1;
	int xMultiplier, yMultiplier, zMultiplier;
	if (pointNormalized.x () != 0) {
		baseCoord = 1;
		xMultiplier = pointNormalized.x () / pointNormalized.x ();
		yMultiplier = pointNormalized.y () / pointNormalized.x ();
		zMultiplier = pointNormalized.z () / pointNormalized.x ();
	} else if (pointNormalized.y () != 0) {
		baseCoord = 2;
		xMultiplier = pointNormalized.x () / pointNormalized.y ();
		yMultiplier = pointNormalized.y () / pointNormalized.y ();
		zMultiplier = pointNormalized.z () / pointNormalized.y ();
	} else if (pointNormalized.z () != 0) {
		baseCoord = 3;
		xMultiplier = pointNormalized.x () / pointNormalized.z ();
		yMultiplier = pointNormalized.y () / pointNormalized.z ();
		zMultiplier = pointNormalized.z () / pointNormalized.z ();
	}

	// get changed point
	osg::Vec3f changedPointNormalized;
	if (baseCoord == -1) {
		changedPointNormalized = pointNormalized;
	} else {
		float baseCoordValue = pow(radius_, 2) / (pow(xMultiplier, 2) + pow(yMultiplier, 2) + pow(zMultiplier, 2));
		float changedXValue;
		float changedYValue;
		float changedZValue;
		if (baseCoord == 1) {
			changedXValue = baseCoordValue;
			changedYValue = baseCoordValue * yMultiplier;
			changedZValue = baseCoordValue * zMultiplier;
		} else if (baseCoord == 2) {
			changedYValue = baseCoordValue;
			changedXValue = baseCoordValue * xMultiplier;
			changedZValue = baseCoordValue * zMultiplier;
		} else if (baseCoord == 3) {
			changedZValue = baseCoordValue;
			changedXValue = baseCoordValue * xMultiplier;
			changedYValue = baseCoordValue * yMultiplier;
		}
		changedPointNormalized = osg::Vec3f (changedXValue, changedYValue, changedZValue);
	}

	// de-normalize
	return (changedPointNormalized + center_);
}

} // namespace
