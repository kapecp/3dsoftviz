#ifndef Layout_ShapeVisitor_H
#define Layout_ShapeVisitor_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_Null.h"
#include "Layout/Shape_Composite.h"
#include "Layout/Shape_Plane.h"
#include "Layout/Shape_Sphere.h"
#include "Layout/Shape_SphereSurface.h"
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Visits shape objects.
 * [visitor pattern]
 */
class ShapeVisitor {

public:

	virtual void visit (Shape_Null & shape) = 0;
	virtual void visit (Shape_Composite & shape) = 0;
	virtual void visit (Shape_Plane & shape) = 0;
	virtual void visit (Shape_Sphere & shape) = 0;
	virtual void visit (Shape_SphereSurface & shape) = 0;

protected:

	/***/
	virtual ~ShapeVisitor (void) {};

}; // class

} // namespace

#endif // Layout_ShapeVisitor_H
