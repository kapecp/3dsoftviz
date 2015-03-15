#include "Layout/Shape_SphereSurface.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_SphereSurface::Shape_SphereSurface(
	const osg::Vec3f center,
	const float radius
) :	Shape_AbstractSphere( center, radius )
{
	// nothing
}

void Shape_SphereSurface::accept(
	ShapeVisitor& visitor
)
{
	visitor.visit( *this );
}

} // namespace
