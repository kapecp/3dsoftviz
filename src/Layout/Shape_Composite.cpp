#include "Layout/Shape_Composite.h"
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------

namespace Layout {

void Shape_Composite::addShape (
        QSharedPointer<Shape> shape
        ) {
    shapes_.push_back (shape);
}

Shape_Composite::ShapesListType & Shape_Composite::getShapes (void) {
    return shapes_;
}

void Shape_Composite::accept (
        ShapeVisitor & visitor
        ) {
    visitor.visit (*this);
}

} // namespace
