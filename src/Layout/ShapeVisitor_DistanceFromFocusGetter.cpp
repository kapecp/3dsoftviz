#include "Layout/ShapeVisitor_DistanceFromFocusGetter.h"

#include <cstdlib>

namespace Layout {

void ShapeVisitor_DistanceFromFocusGetter::setPosition(const osg::Vec3f &position)
{
	mPosition = position;
}

float ShapeVisitor_DistanceFromFocusGetter::distanceFromFocus()
{
	return mDistanceFromFocus;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_Null &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_Composite &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_Intersection &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_Plane &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_Sphere &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_SphereSurface &shape)
{
	mDistanceFromFocus = -1;
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_CylinderSurface &shape)
{
	// Get position of camera (eye) and position where the camera looks at (center)
	osg::Vec3f firstBaseCenter = shape.firstBaseCenter();
	osg::Vec3f secondBaseCenter = shape.secondBaseCenter();

	// Compute dot product and point on line and perpendicular vector and its length
	osg::Vec3f normalizedLineOfSightVector = secondBaseCenter - firstBaseCenter;
	normalizedLineOfSightVector.normalize();
	float dotProduct = (mPosition - firstBaseCenter) * normalizedLineOfSightVector;
	osg::Vec3f pointOnLine = firstBaseCenter + normalizedLineOfSightVector * dotProduct;
	osg::Vec3f perpenVector = mPosition - pointOnLine;

	mDistanceFromFocus = perpenVector.length();
}

void ShapeVisitor_DistanceFromFocusGetter::visit(Shape_ConeSurface &shape)
{
	mDistanceFromFocus = (shape.spike() - mPosition).length();
}

}
