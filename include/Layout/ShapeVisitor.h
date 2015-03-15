#ifndef LAYOUT_SHAPEVISITOR_H
#define LAYOUT_SHAPEVISITOR_H

#include "Layout/Shape_Null.h"
#include "Layout/Shape_Composite.h"
#include "Layout/Shape_Intersection.h"
#include "Layout/Shape_Plane.h"
#include "Layout/Shape_Sphere.h"
#include "Layout/Shape_SphereSurface.h"
#include "Layout/Shape_CylinderSurface.h"
#include "Layout/Shape_ConeSurface.h"
#include "Layout/Shape_Cube.h"

namespace Layout {

/**
 * \brief Visits shape objects.
 * [visitor pattern]
 */
class ShapeVisitor
{
protected:
	virtual ~ShapeVisitor() {}

public:

	virtual void visit( Shape_Null /*&shape*/ ) = 0;
	virtual void visit( Shape_Composite& shape ) = 0;
	virtual void visit( Shape_Intersection& shape ) = 0;
	virtual void visit( Shape_Plane& shape ) = 0;
	virtual void visit( Shape_Sphere& shape ) = 0;
	virtual void visit( Shape_SphereSurface& shape ) = 0;
	virtual void visit( Shape_CylinderSurface& shape ) = 0;
	virtual void visit( Shape_ConeSurface& shape ) = 0;
	virtual void visit( Shape_Cube& shape ) = 0;
};

} // namespace Layout

#endif // LAYOUT_SHAPEVISITOR_H
