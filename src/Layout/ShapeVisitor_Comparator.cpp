#include "Layout/ShapeVisitor_Comparator.h"
//-----------------------------------------------------------------------------

namespace Layout {

void ShapeVisitor_Comparator::setOtherShape (QSharedPointer<Shape> otherShape) {
	otherShape_ = otherShape;
}

bool ShapeVisitor_Comparator::getComparisonResult (void) {
	return comparisonResult_;
}

void ShapeVisitor_Comparator::visit (Shape_Composite & shape) {
	QSharedPointer<Shape_Composite> otherShape;
	if ((!otherShape_.isNull ()) && (!(otherShape = otherShape_.dynamicCast<Shape_Composite> ()).isNull ())) {
		Shape_Composite::ShapesListType & list1 = shape.getShapes ();
		Shape_Composite::ShapesListType & list2 = otherShape->getShapes ();
		if (list1.size () == list2.size()) {
			Shape_Composite::ShapesListType::iterator it1 = list1.begin ();
			Shape_Composite::ShapesListType::iterator it2 = list2.begin ();
			for (; it1 != list1.end (); ++it1, ++it2) {
				otherShape_ = *it2;
				(*it1)->accept (*this);
				if (!comparisonResult_) {
					return;
				}
			}
		} else {
			comparisonResult_ = false;
		}
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_Plane & shape) {
	QSharedPointer<Shape_Plane> otherShape;
	if ((!otherShape_.isNull ()) && (!(otherShape = otherShape_.dynamicCast<Shape_Plane> ()).isNull ())) {
		comparisonResult_ =
			(shape.getNormalVector () == otherShape->getNormalVector ())
			&&
			(shape.getD () == otherShape->getD ())
		;
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_Sphere & shape) {
	QSharedPointer<Shape_Sphere> otherShape;
	if ((!otherShape_.isNull ()) && (!(otherShape = otherShape_.dynamicCast<Shape_Sphere> ()).isNull ())) {
		compareSpheres (shape, *otherShape);
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_SphereSurface & shape) {
	QSharedPointer<Shape_SphereSurface> otherShape;
	if ((!otherShape_.isNull ()) && (!(otherShape = otherShape_.dynamicCast<Shape_SphereSurface> ()).isNull ())) {
		compareSpheres (shape, *otherShape);
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::compareSpheres (Shape_AbstractSphere & shape, Shape_AbstractSphere & otherShape) {
	comparisonResult_ =
		(shape.getCenter () == otherShape.getCenter ())
		&&
		(shape.getRadius () == otherShape.getRadius ())
	;
}

} // namespace
