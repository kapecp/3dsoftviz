#include "Layout/Shape_Sphere.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_Sphere::Shape_Sphere(
	const osg::Vec3f center,
	const float radius,
	RestrictionPolicy restrictionPolicy
) :	Shape_AbstractSphere( center, radius ),
	restrictionPolicy_( restrictionPolicy )
{
	// nothing
}

void Shape_Sphere::accept(
	ShapeVisitor& visitor
)
{
	visitor.visit( *this );
}

Shape_Sphere::RestrictionPolicy Shape_Sphere::getRestrictionPolicy()
{
	return restrictionPolicy_;
}

} // namespace
