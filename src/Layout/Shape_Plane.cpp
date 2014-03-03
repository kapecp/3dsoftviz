#include "Layout/Shape_Plane.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_Plane::Shape_Plane (
		const osg::Vec3f normalVector,
		const float d
		) :	normalVector_ (normalVector),
	d_ (d)
{
	// nothing
}

osg::Vec3f & Shape_Plane::getNormalVector (void) {
	return normalVector_;
}

float Shape_Plane::getD (void) {
	return d_;
}

void Shape_Plane::accept (
		ShapeVisitor & visitor
		) {
	visitor.visit (*this);
}

} // namespace
