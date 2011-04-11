#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
//-----------------------------------------------------------------------------
#include <cstdlib>
//-----------------------------------------------------------------------------

namespace Layout {

void ShapeVisitor_RestrictedPositionGetter::setOriginalPosition (osg::Vec3f originalPosition) {
	originalPosition_ = originalPosition;
}

osg::Vec3f ShapeVisitor_RestrictedPositionGetter::getRestrictedPosition (void) {
	return restrictedPosition_;
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Composite & shape) {
	Shape_Composite::ShapesListType & shapes = shape.getShapes ();

	if (shapes.size () > 0) {
		osg::Vec3f restrictedPositionWithMinDistance;
		float minDistanceFound;
		for (Shape_Composite::ShapesListType::const_iterator it = shapes.begin (); it != shapes.end (); ++it) {
			(*it)->accept (*this); // restrictedPosition_ changes here

			float currentDistance = (originalPosition_ - restrictedPosition_).length ();

			if ((currentDistance < minDistanceFound) || (it == shapes.begin ())) {
				restrictedPositionWithMinDistance = restrictedPosition_;
				minDistanceFound = currentDistance;
			}
		}

		restrictedPosition_ = restrictedPositionWithMinDistance;
	} else {
		restrictedPosition_ = originalPosition_;
	}
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Plane & shape) {
	float t;
	float m = - shape.getD () - shape.getNormalVector ().x () * originalPosition_.x () - shape.getNormalVector ().y () * originalPosition_.y () - shape.getNormalVector ().z () * originalPosition_.z ();
	float n = pow (shape.getNormalVector ().x (), 2) + pow (shape.getNormalVector ().y (), 2) + pow (shape.getNormalVector ().z (), 2);
	if (n != 0.0) {
		t = m / n;
	} else {
		t = 0;
	}

	restrictedPosition_ = osg::Vec3f (
		originalPosition_.x () + t * shape.getNormalVector ().x (),
		originalPosition_.y () + t * shape.getNormalVector ().y (),
		originalPosition_.z () + t * shape.getNormalVector ().z ()
	);
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Sphere & shape) {
	if ((originalPosition_ - shape.getCenter ()).length() > shape.getRadius ()) {
		restrictedPosition_ = toSphere (shape.getCenter (), shape.getRadius (), originalPosition_);
	} else {
		restrictedPosition_ = originalPosition_;
	}
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_SphereSurface & shape) {
	restrictedPosition_ = toSphere (shape.getCenter (), shape.getRadius (), originalPosition_);
}

osg::Vec3f ShapeVisitor_RestrictedPositionGetter::toSphere (
	const osg::Vec3f center,
	const float radius,
	const osg::Vec3f &point
) {
	// change the point as if the sphere shape.getCenter () was [0, 0, 0]
	osg::Vec3f pointMoved = (point - center);

	osg::Vec3f changedPointMoved = pointMoved;

	if (changedPointMoved.length() == 0.0) {
		// create random point
		for (int i = 0; i < 3; ++i) {
			changedPointMoved[i] = (rand() % 100) + 1;
		}
	}
	changedPointMoved.normalize();
	changedPointMoved *= radius;

	osg::Vec3f changedPoint = changedPointMoved + center;

	return changedPoint;
}

} // namespace
