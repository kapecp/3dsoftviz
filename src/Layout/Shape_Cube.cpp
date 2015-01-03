#include "Layout/Shape_Cube.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_Cube::Shape_Cube (
        const osg::Vec3f center,
        const osg::Vec3f surfaceX,
        const osg::Vec3f surfaceY,
        const osg::Vec3f surfaceZ,
        RestrictionPolicy restrictionPolicy
        ) :	Shape_AbstractCube (center, surfaceX, surfaceY, surfaceZ),
    restrictionPolicy_ (restrictionPolicy)
{
    // nothing
}

void Shape_Cube::accept (
        ShapeVisitor & visitor
        ) {
    visitor.visit (*this);
}

Shape_Cube::RestrictionPolicy Shape_Cube::getRestrictionPolicy () {
    return restrictionPolicy_;
}

} // namespace
