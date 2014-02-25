#include "Layout/Shape_Intersection.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {


void Shape_Intersection::accept (
		ShapeVisitor & visitor
		) {
	visitor.visit (*this);
}

} // namespace
