#include "Layout/ShapeGetter_Const.h"

namespace Layout {

ShapeGetter_Const::ShapeGetter_Const (
	QSharedPointer<Shape> shape
) : shape_ (shape)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Const::getShape (void) {
	return shape_;
}

} // namespace
