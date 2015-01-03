#include "Layout/Shape_Null.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

Shape_Null::Shape_Null (void) {
    // nothing
}

void Shape_Null::accept (
        ShapeVisitor & visitor
        ) {
    visitor.visit (*this);
}

} // namespace
