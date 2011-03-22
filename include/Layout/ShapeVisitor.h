#ifndef Layout_ShapeVisitor_H
#define Layout_ShapeVisitor_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_Plane.h"
#include "Layout/Shape_Sphere.h"
#include "Layout/Shape_SphereSurface.h"
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeVisitor {

public:

	virtual void visit (Shape_Plane & shape) = 0;
	virtual void visit (Shape_Sphere & shape) = 0;
	virtual void visit (Shape_SphereSurface & shape) = 0;

protected:

	/***/
	virtual ~ShapeVisitor (void) {};

}; // class

} // namespace

#endif // Layout_ShapeVisitor_H
