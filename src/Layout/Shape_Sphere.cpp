#include "Layout/Shape_Sphere.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_Sphere::Shape_Sphere (
	const osg::Vec3f center,
	const float radius
) :	Shape_AbstractSphere (center, radius)
{
	// nothing
}

void Shape_Sphere::accept (
	ShapeVisitor & visitor
) {
	visitor.visit (*this);
}

} // namespace
