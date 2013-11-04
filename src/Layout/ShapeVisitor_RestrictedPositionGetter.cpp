#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
//-----------------------------------------------------------------------------
#include <cstdlib>
//-----------------------------------------------------------------------------

namespace Layout {

void ShapeVisitor_RestrictedPositionGetter::setOriginalPosition(const osg::Vec3f &position)
{
    mOriginalPosition = position;
}

const osg::Vec3f &ShapeVisitor_RestrictedPositionGetter::restrictedPositionConstRef()
{
    return mRestrictedPosition;
}

void ShapeVisitor_RestrictedPositionGetter::visit(Shape_Null &shape)
{
    mRestrictedPosition = mOriginalPosition;
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Composite &shape)
{
    Shape_Composite::ShapesListType & shapes = shape.getShapes();

	if (shapes.size () > 0) {
		osg::Vec3f restrictedPositionWithMinDistance;
		float minDistanceFound;
		for (Shape_Composite::ShapesListType::const_iterator it = shapes.begin (); it != shapes.end (); ++it) {
			(*it)->accept (*this); // restrictedPosition_ changes here

            float currentDistance = (mOriginalPosition - mRestrictedPosition).length ();

			if ((currentDistance < minDistanceFound) || (it == shapes.begin ())) {
                restrictedPositionWithMinDistance = mRestrictedPosition;
				minDistanceFound = currentDistance;
			}
		}

        mRestrictedPosition = restrictedPositionWithMinDistance;
	} else {
        mRestrictedPosition = mOriginalPosition;
	}
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Intersection & shape) {
    Shape_Intersection::ShapesListType & shapes = shape.getShapes ();

    if (shapes.size () > 0) {
        float currentDistance = 1.0;
        while (currentDistance > 0.05){
            Shape_Intersection::ShapesListType::const_iterator it;
            for (it = shapes.begin (); it != shapes.end (); ++it) {
                (*it)->accept (*this); // restrictedPosition_ changes here
                currentDistance = (mRestrictedPosition - mOriginalPosition).length ();
                mOriginalPosition = mRestrictedPosition;
            }
        }
    } else {
            mRestrictedPosition = mOriginalPosition;
    }
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Plane & shape) {
	float t;
    float m = - shape.getD () - shape.getNormalVector ().x () * mOriginalPosition.x () - shape.getNormalVector ().y () * mOriginalPosition.y () - shape.getNormalVector ().z () * mOriginalPosition.z ();
	float n = pow (shape.getNormalVector ().x (), 2) + pow (shape.getNormalVector ().y (), 2) + pow (shape.getNormalVector ().z (), 2);
	if (n != 0.0) {
		t = m / n;
	} else {
		t = 0;
	}

    mRestrictedPosition = osg::Vec3f (
        mOriginalPosition.x () + t * shape.getNormalVector ().x (),
        mOriginalPosition.y () + t * shape.getNormalVector ().y (),
        mOriginalPosition.z () + t * shape.getNormalVector ().z ()
	);
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_Sphere & shape) {
    if ((mOriginalPosition - shape.getCenter ()).length() > shape.getRadius ()) {
		// position outside

		float radiusMin;
		float radiusMax;
		switch (shape.getRestrictionPolicy ()) {
			case Shape_Sphere::SURFACE:
				radiusMin = shape.getRadius ();
				radiusMax = shape.getRadius ();
				break;
			case Shape_Sphere::RANDOM_DISTANCE_FROM_CENTER:
				radiusMin = 0;
				radiusMax = shape.getRadius ();
				break;
			default:
				radiusMin = 0;
				radiusMax = 0;
		}

        mRestrictedPosition = toSphere (shape.getCenter (), radiusMin, radiusMax, mOriginalPosition);
	} else {
		// position in sphere - OK
        mRestrictedPosition = mOriginalPosition;
	}
}

void ShapeVisitor_RestrictedPositionGetter::visit (Shape_SphereSurface & shape) {
    mRestrictedPosition = toSphere (shape.getCenter (), shape.getRadius (), shape.getRadius (), mOriginalPosition);
}

void ShapeVisitor_RestrictedPositionGetter::visit(Shape_CylinderSurface &shape)
{
    // Get positions of base centers of the cylinder
    osg::Vec3f firstBaseCenter = shape.firstBaseCenter();
    osg::Vec3f secondBaseCenter = shape.secondBaseCenter();

    // Compute dot product and point on line
    osg::Vec3f normalizedLineOfSightVector = secondBaseCenter - firstBaseCenter;
    normalizedLineOfSightVector.normalize();
    float dotProduct = (mOriginalPosition - firstBaseCenter) * normalizedLineOfSightVector;
    osg::Vec3f pointOnLine = firstBaseCenter + normalizedLineOfSightVector * dotProduct;
    osg::Vec3f perpenVector = mOriginalPosition - pointOnLine;
    float perpenVectorLength = perpenVector.length();
    perpenVector.normalize();

    // Apply restriction only if the node is inside the cylinder
    if (perpenVectorLength > 0 && perpenVectorLength < shape.baseRadius())   // Test for zero equality
    {
        mRestrictedPosition = mOriginalPosition + (perpenVector * (shape.baseRadius() - perpenVectorLength));
    }
    else
    {
        mRestrictedPosition = mOriginalPosition;
    }
}

void ShapeVisitor_RestrictedPositionGetter::visit(Shape_ConeSurface &shape)
{
    // Get positions of base center and spike of the cone
    osg::Vec3f baseCenter = shape.baseCenter();
    osg::Vec3f spike = shape.spike();

    // Compute dot product and point on line
    osg::Vec3f normalizedLineOfSightVector = spike - baseCenter;
    normalizedLineOfSightVector.normalize();
    float dotProduct = (mOriginalPosition - baseCenter) * normalizedLineOfSightVector;
    osg::Vec3f pointOnLine = baseCenter + normalizedLineOfSightVector * dotProduct;
    osg::Vec3f perpenVector = mOriginalPosition - pointOnLine;
    float perpenVectorLength = perpenVector.length();
    perpenVector.normalize();

    // THIS IS THE DIFFERENCE CODE FROM CYLINDER
    float spikeBaseCenterDistance = (spike - baseCenter).length();
    if ( (dotProduct > 0) && (dotProduct < spikeBaseCenterDistance) )
        perpenVectorLength *= (dotProduct) / spikeBaseCenterDistance;
    else
        perpenVectorLength = 0;
    // THIS IS THE DIFFERENCE CODE FROM CYLINDER

    // Apply restriction only if the node is inside the cone
    if (perpenVectorLength > 0 && perpenVectorLength < shape.baseRadius())   // Test for zero equality
    {
        mRestrictedPosition = mOriginalPosition + (perpenVector * (shape.baseRadius() - perpenVectorLength));
    }
    else
    {
        mRestrictedPosition = mOriginalPosition;
    }
}

osg::Vec3f ShapeVisitor_RestrictedPositionGetter::toSphere(const osg::Vec3f &center, float radiusMin,
                                                           float radiusMax, const osg::Vec3f &point)
{
	// change the point as if the sphere shape.getCenter () was [0, 0, 0]
	osg::Vec3f pointMoved = (point - center);

	osg::Vec3f changedPointMoved = pointMoved;

	if (changedPointMoved.length() == 0.0) {
		// create random point
		for (int i = 0; i < 3; ++i) {
			changedPointMoved[i] = (rand () % 100) + 1;
		}
	}

	changedPointMoved.normalize();   //zmeni velkost vectora na 1

	float multiplier = radiusMin;
	if (radiusMax > radiusMin) {
		float randomIncrement = ((float)rand() / (float)RAND_MAX) * (radiusMax - radiusMin);
		multiplier += randomIncrement;
	}
	changedPointMoved *= multiplier;

	osg::Vec3f changedPoint = changedPointMoved + center;

	return changedPoint;
}

} // namespace
